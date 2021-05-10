// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CurrencyDataLoader.h"
#include "Common/AppResourceProvider.h"
#include "Common/LocalizationStringUtil.h"
#include "Common/LocalizationService.h"
#include "Common/LocalizationSettings.h"
#include "Common/TraceLogger.h"
#include "UnitConverterDataConstants.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::LocalizationServiceProperties;
using namespace CalculatorApp::DataLoaders;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::ViewModel::CurrencyDataLoaderConstants;
using namespace concurrency;
using namespace Platform;
using namespace std;
using namespace UnitConversionManager;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::Storage;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Core;
using namespace Windows::Web::Http;

static constexpr auto CURRENCY_UNIT_FROM_KEY = L"CURRENCY_UNIT_FROM_KEY";
static constexpr auto CURRENCY_UNIT_TO_KEY = L"CURRENCY_UNIT_TO_KEY";

// Calculate number of 100-nanosecond intervals-per-day
// (1 interval/100 nanosecond)(100 nanosecond/1e-7 s)(60 s/1 min)(60 min/1 hr)(24 hr/1 day) = (interval/day)
static constexpr long long DAY_DURATION = 1LL * 60 * 60 * 24 * 10000000;
static constexpr long long WEEK_DURATION = DAY_DURATION * 7;

static constexpr int FORMATTER_RATE_FRACTION_PADDING = 2;
static constexpr int FORMATTER_RATE_MIN_DECIMALS = 4;
static constexpr int FORMATTER_RATE_MIN_SIGNIFICANT_DECIMALS = 4;

static constexpr auto CACHE_TIMESTAMP_KEY = L"CURRENCY_CONVERTER_TIMESTAMP";
static constexpr auto CACHE_LANGCODE_KEY = L"CURRENCY_CONVERTER_LANGCODE";
static constexpr auto CACHE_DELIMITER = L"%";

static constexpr auto STATIC_DATA_FILENAME = L"CURRENCY_CONVERTER_STATIC_DATA.txt";
static constexpr array<wstring_view, 5> STATIC_DATA_PROPERTIES = { wstring_view{ L"CountryCode", 11 },
                                                                   wstring_view{ L"CountryName", 11 },
                                                                   wstring_view{ L"CurrencyCode", 12 },
                                                                   wstring_view{ L"CurrencyName", 12 },
                                                                   wstring_view{ L"CurrencySymbol", 14 } };

static constexpr auto ALL_RATIOS_DATA_FILENAME = L"CURRENCY_CONVERTER_ALL_RATIOS_DATA.txt";
static constexpr auto RATIO_KEY = L"Rt";
static constexpr auto CURRENCY_CODE_KEY = L"An";
static constexpr array<wstring_view, 2> ALL_RATIOS_DATA_PROPERTIES = { wstring_view{ RATIO_KEY, 2 }, wstring_view{ CURRENCY_CODE_KEY, 2 } };

static constexpr auto DEFAULT_FROM_TO_CURRENCY_FILE_URI = L"ms-appx:///DataLoaders/DefaultFromToCurrency.json";
static constexpr auto FROM_KEY = L"from";
static constexpr auto TO_KEY = L"to";

// Fallback default values.
static constexpr auto DEFAULT_FROM_CURRENCY = DefaultCurrencyCode.data();
static constexpr auto DEFAULT_TO_CURRENCY = L"EUR";

namespace CalculatorApp
{
    namespace ViewModel
    {
        namespace UnitConverterResourceKeys
        {
            StringReference CurrencyUnitFromKey(CURRENCY_UNIT_FROM_KEY);
            StringReference CurrencyUnitToKey(CURRENCY_UNIT_TO_KEY);
        }

        namespace CurrencyDataLoaderConstants
        {
            StringReference CacheTimestampKey(CACHE_TIMESTAMP_KEY);
            StringReference CacheLangcodeKey(CACHE_LANGCODE_KEY);
            StringReference CacheDelimiter(CACHE_DELIMITER);
            StringReference StaticDataFilename(STATIC_DATA_FILENAME);
            StringReference AllRatiosDataFilename(ALL_RATIOS_DATA_FILENAME);
            long long DayDuration = DAY_DURATION;
        }
    }
}

CurrencyDataLoader::CurrencyDataLoader(_In_ unique_ptr<ICurrencyHttpClient> client, const wchar_t* forcedResponseLanguage)
    : m_client(move(client))
    , m_loadStatus(CurrencyLoadStatus::NotLoaded)
    , m_responseLanguage(L"en-US")
    , m_ratioFormat(L"")
    , m_timestampFormat(L"")
    , m_networkManager(ref new NetworkManager())
    , m_meteredOverrideSet(false)
{
    if (forcedResponseLanguage != nullptr)
    {
        m_responseLanguage = ref new Platform::String(forcedResponseLanguage);
    }
    else
    {
        if (GlobalizationPreferences::Languages->Size > 0)
        {
            m_responseLanguage = GlobalizationPreferences::Languages->GetAt(0);
        }
        else
        {
            m_responseLanguage = L"en-US";
        }
    }

    if (m_client != nullptr)
    {
        m_client->SetSourceCurrencyCode(StringReference(DefaultCurrencyCode.data()));
        m_client->SetResponseLanguage(m_responseLanguage);
    }

    auto localizationService = LocalizationService::GetInstance();
    if (CoreWindow::GetForCurrentThread() != nullptr)
    {
        // Must have a CoreWindow to access the resource context.
        m_isRtlLanguage = localizationService->IsRtlLayout();
    }

    m_ratioFormatter = localizationService->GetRegionalSettingsAwareDecimalFormatter();
    m_ratioFormatter->IsGrouped = true;
    m_ratioFormatter->IsDecimalPointAlwaysDisplayed = true;
    m_ratioFormatter->FractionDigits = FORMATTER_RATE_FRACTION_PADDING;

    m_ratioFormat = AppResourceProvider::GetInstance()->GetResourceString(L"CurrencyFromToRatioFormat");
    m_timestampFormat = AppResourceProvider::GetInstance()->GetResourceString(L"CurrencyTimestampFormat");
}

CurrencyDataLoader::~CurrencyDataLoader()
{
    UnregisterForNetworkBehaviorChanges();
}

void CurrencyDataLoader::UnregisterForNetworkBehaviorChanges()
{
    m_networkManager->NetworkBehaviorChanged -= m_networkBehaviorToken;
}

void CurrencyDataLoader::RegisterForNetworkBehaviorChanges()
{
    UnregisterForNetworkBehaviorChanges();

    m_networkBehaviorToken = m_networkManager->NetworkBehaviorChanged +=
        ref new NetworkBehaviorChangedHandler([this](NetworkAccessBehavior newBehavior) { this->OnNetworkBehaviorChanged(newBehavior); });

    OnNetworkBehaviorChanged(NetworkManager::GetNetworkAccessBehavior());
}

void CurrencyDataLoader::OnNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
{
    m_networkAccessBehavior = newBehavior;
    if (m_vmCallback != nullptr)
    {
        m_vmCallback->NetworkBehaviorChanged(static_cast<int>(m_networkAccessBehavior));
    }
}

bool CurrencyDataLoader::LoadFinished()
{
    return m_loadStatus != CurrencyLoadStatus::NotLoaded;
}

bool CurrencyDataLoader::LoadedFromCache()
{
    return m_loadStatus == CurrencyLoadStatus::LoadedFromCache;
}

bool CurrencyDataLoader::LoadedFromWeb()
{
    return m_loadStatus == CurrencyLoadStatus::LoadedFromWeb;
}

void CurrencyDataLoader::ResetLoadStatus()
{
    m_loadStatus = CurrencyLoadStatus::NotLoaded;
}

#pragma optimize("", off) // Turn off optimizations to work around DevDiv 393321
void CurrencyDataLoader::LoadData()
{
    RegisterForNetworkBehaviorChanges();

    if (!LoadFinished())
    {
        create_task([this]() -> task<bool> {
            vector<function<future<bool>()>> loadFunctions = {
                [this]() { return TryLoadDataFromCacheAsync(); },
                [this]() { return TryLoadDataFromWebAsync(); },
            };

            bool didLoad = false;
            for (auto& f : loadFunctions)
            {
                didLoad = co_await f();
                if (didLoad)
                {
                    break;
                }
            }

            co_return didLoad;
        })
            .then(
                [this](bool didLoad) {
                    UpdateDisplayedTimestamp();
                    NotifyDataLoadFinished(didLoad);
                },
                task_continuation_context::use_current());
    }
};
#pragma optimize("", on)

vector<UCM::Category> CurrencyDataLoader::GetOrderedCategories()
{
    // This function should not be called
    // The model will use the categories from UnitConverterDataLoader
    return vector<UCM::Category>();
}

vector<UCM::Unit> CurrencyDataLoader::GetOrderedUnits(const UCM::Category& /*category*/)
{
    lock_guard<mutex> lock(m_currencyUnitsMutex);
    return m_currencyUnits;
}

unordered_map<UCM::Unit, UCM::ConversionData, UCM::UnitHash> CurrencyDataLoader::LoadOrderedRatios(const UCM::Unit& unit)
{
    lock_guard<mutex> lock(m_currencyUnitsMutex);
    return m_currencyRatioMap.at(unit);
}

bool CurrencyDataLoader::SupportsCategory(const UCM::Category& target)
{
    static int currencyId = NavCategory::Serialize(ViewMode::Currency);
    return target.id == currencyId;
}

void CurrencyDataLoader::SetViewModelCallback(const shared_ptr<UCM::IViewModelCurrencyCallback>& callback)
{
    m_vmCallback = callback;
    OnNetworkBehaviorChanged(m_networkAccessBehavior);
}

pair<wstring, wstring> CurrencyDataLoader::GetCurrencySymbols(const UCM::Unit& unit1, const UCM::Unit& unit2)
{
    lock_guard<mutex> lock(m_currencyUnitsMutex);

    wstring symbol1 = L"";
    wstring symbol2 = L"";

    auto itr1 = m_currencyMetadata.find(unit1);
    auto itr2 = m_currencyMetadata.find(unit2);
    if (itr1 != m_currencyMetadata.end() && itr2 != m_currencyMetadata.end())
    {
        symbol1 = (itr1->second).symbol;
        symbol2 = (itr2->second).symbol;
    }

    return make_pair(symbol1, symbol2);
}

double CurrencyDataLoader::RoundCurrencyRatio(double ratio)
{
    // Compute how many decimals we need to display two meaningful digits at minimum
    // For example: 0.00000000342334 -> 0.000000003423, 0.000212 -> 0.000212
    int numberDecimals = FORMATTER_RATE_MIN_DECIMALS;
    if (ratio < 1)
    {
        numberDecimals = max(
            FORMATTER_RATE_MIN_DECIMALS,
            (int)(-log10(ratio)) + FORMATTER_RATE_MIN_SIGNIFICANT_DECIMALS);
    }

    unsigned long long scale = (unsigned long long)powl(10l, numberDecimals);

    return (double)(round(ratio * scale) / scale);
}

pair<wstring, wstring> CurrencyDataLoader::GetCurrencyRatioEquality(_In_ const UCM::Unit& unit1, _In_ const UCM::Unit& unit2)
{
    try
    {
        auto iter = m_currencyRatioMap.find(unit1);
        if (iter != m_currencyRatioMap.end())
        {
            unordered_map<UCM::Unit, UCM::ConversionData, UCM::UnitHash> ratioMap = iter->second;
            auto iter2 = ratioMap.find(unit2);
            if (iter2 != ratioMap.end())
            {
                double ratio = (iter2->second).ratio;
                double rounded = RoundCurrencyRatio(ratio);

                auto digit = LocalizationSettings::GetInstance().GetDigitSymbolFromEnUsDigit(L'1');
                auto digitSymbol = ref new String(&digit, 1);
                auto roundedFormat = m_ratioFormatter->Format(rounded);

                auto ratioString = LocalizationStringUtil::GetLocalizedString(
                    m_ratioFormat, digitSymbol, StringReference(unit1.abbreviation.c_str()), roundedFormat, StringReference(unit2.abbreviation.c_str()));

                auto accessibleRatioString =
                    LocalizationStringUtil::GetLocalizedString(
                    m_ratioFormat, digitSymbol, StringReference(unit1.accessibleName.c_str()), roundedFormat, StringReference(unit2.accessibleName.c_str()));

                return make_pair(ratioString->Data(), accessibleRatioString->Data());
            }
        }
    }
    catch (...)
    {
    }

    return make_pair(L"", L"");
}

#pragma optimize("", off) // Turn off optimizations to work around DevDiv 393321
future<bool> CurrencyDataLoader::TryLoadDataFromCacheAsync()
{
    try
    {
        ResetLoadStatus();

        auto localSettings = ApplicationData::Current->LocalSettings;
        if (localSettings == nullptr || !localSettings->Values->HasKey(CacheTimestampKey))
        {
            co_return false;
        }

        bool loadComplete = false;
        m_cacheTimestamp = static_cast<DateTime>(localSettings->Values->Lookup(CacheTimestampKey));
        if (Utils::IsDateTimeOlderThan(m_cacheTimestamp, DAY_DURATION) && m_networkAccessBehavior == NetworkAccessBehavior::Normal)
        {
            loadComplete = co_await TryLoadDataFromWebAsync();
        }

        if (!loadComplete)
        {
            loadComplete = co_await TryFinishLoadFromCacheAsync();
        }

        co_return loadComplete;
    }
    catch (Exception ^ ex)
    {
        TraceLogger::GetInstance()->LogPlatformException(ViewMode::Currency, __FUNCTIONW__, ex);
        co_return false;
    }
    catch (const exception& e)
    {
        TraceLogger::GetInstance()->LogStandardException(ViewMode::Currency, __FUNCTIONW__, e);
        co_return false;
    }
    catch (...)
    {
        co_return false;
    }
}

future<bool> CurrencyDataLoader::TryFinishLoadFromCacheAsync()
{
    auto localSettings = ApplicationData::Current->LocalSettings;
    if (localSettings == nullptr)
    {
        co_return false;
    }

    if (!localSettings->Values->HasKey(CacheLangcodeKey) || !static_cast<String ^>(localSettings->Values->Lookup(CacheLangcodeKey))->Equals(m_responseLanguage))
    {
        co_return false;
    }

    StorageFolder ^ localCacheFolder = ApplicationData::Current->LocalCacheFolder;
    if (localCacheFolder == nullptr)
    {
        co_return false;
    }

    String ^ staticDataResponse = co_await Utils::ReadFileFromFolder(localCacheFolder, StaticDataFilename);
    String ^ allRatiosResponse = co_await Utils::ReadFileFromFolder(localCacheFolder, AllRatiosDataFilename);

    vector<UCM::CurrencyStaticData> staticData{};
    CurrencyRatioMap ratioMap{};

    bool didParse = TryParseWebResponses(staticDataResponse, allRatiosResponse, staticData, ratioMap);
    if (!didParse)
    {
        co_return false;
    }

    m_loadStatus = CurrencyLoadStatus::LoadedFromCache;
    co_await FinalizeUnits(staticData, ratioMap);

    co_return true;
}

future<bool> CurrencyDataLoader::TryLoadDataFromWebAsync()
{
    try
    {
        ResetLoadStatus();

        if (m_client == nullptr)
        {
            co_return false;
        }

        if (m_networkAccessBehavior == NetworkAccessBehavior::Offline || (m_networkAccessBehavior == NetworkAccessBehavior::OptIn && !m_meteredOverrideSet))
        {
            co_return false;
        }

        String ^ staticDataResponse = co_await m_client->GetCurrencyMetadata();
        String ^ allRatiosResponse = co_await m_client->GetCurrencyRatios();
        if (staticDataResponse == nullptr || allRatiosResponse == nullptr)
        {
            co_return false;
        }

        vector<UCM::CurrencyStaticData> staticData{};
        CurrencyRatioMap ratioMap{};

        bool didParse = TryParseWebResponses(staticDataResponse, allRatiosResponse, staticData, ratioMap);
        if (!didParse)
        {
            co_return false;
        }

        // Set the timestamp before saving it below.
        m_cacheTimestamp = Utils::GetUniversalSystemTime();

        try
        {
            const vector<pair<String ^, String ^>> cachedFiles = { { StaticDataFilename, staticDataResponse }, { AllRatiosDataFilename, allRatiosResponse } };

            StorageFolder ^ localCacheFolder = ApplicationData::Current->LocalCacheFolder;
            for (const auto& fileInfo : cachedFiles)
            {
                co_await Utils::WriteFileToFolder(localCacheFolder, fileInfo.first, fileInfo.second, CreationCollisionOption::ReplaceExisting);
            }

            SaveLangCodeAndTimestamp();
        }
        catch (...)
        {
            // If we fail to save to cache it's okay, we should still continue.
        }

        m_loadStatus = CurrencyLoadStatus::LoadedFromWeb;
        co_await FinalizeUnits(staticData, ratioMap);

        co_return true;
    }
    catch (Exception ^ ex)
    {
        TraceLogger::GetInstance()->LogPlatformException(ViewMode::Currency, __FUNCTIONW__, ex);
        co_return false;
    }
    catch (const exception& e)
    {
        TraceLogger::GetInstance()->LogStandardException(ViewMode::Currency, __FUNCTIONW__, e);
        co_return false;
    }
    catch (...)
    {
        co_return false;
    }
}

future<bool> CurrencyDataLoader::TryLoadDataFromWebOverrideAsync()
{
    m_meteredOverrideSet = true;
    bool didLoad = co_await TryLoadDataFromWebAsync();
    if (!didLoad)
    {
        m_loadStatus = CurrencyLoadStatus::FailedToLoad;
        TraceLogger::GetInstance()->LogError(ViewMode::Currency, L"CurrencyDataLoader::TryLoadDataFromWebOverrideAsync", L"UserRequestedRefreshFailed");
    }

    co_return didLoad;
};
#pragma optimize("", on)

bool CurrencyDataLoader::TryParseWebResponses(
    _In_ String ^ staticDataJson,
    _In_ String ^ allRatiosJson,
    _Inout_ vector<UCM::CurrencyStaticData>& staticData,
    _Inout_ CurrencyRatioMap& allRatiosData)
{
    return TryParseStaticData(staticDataJson, staticData) && TryParseAllRatiosData(allRatiosJson, allRatiosData);
}

bool CurrencyDataLoader::TryParseStaticData(_In_ String ^ rawJson, _Inout_ vector<UCM::CurrencyStaticData>& staticData)
{
    JsonArray ^ data = nullptr;
    if (!JsonArray::TryParse(rawJson, &data))
    {
        return false;
    }

    wstring countryCode{ L"" };
    wstring countryName{ L"" };
    wstring currencyCode{ L"" };
    wstring currencyName{ L"" };
    wstring currencySymbol{ L"" };

    vector<wstring*> values = { &countryCode, &countryName, &currencyCode, &currencyName, &currencySymbol };

    assert(values.size() == STATIC_DATA_PROPERTIES.size());
    staticData.resize(size_t{ data->Size });
    for (unsigned int i = 0; i < data->Size; i++)
    {
        JsonObject ^ obj;
        try
        {
            obj = data->GetAt(i)->GetObject();
        }
        catch (COMException ^ e)
        {
            if (e->HResult == E_ILLEGAL_METHOD_CALL)
            {
                continue;
            }
            else
            {
                throw;
            }
        }

        for (size_t j = 0; j < values.size(); j++)
        {
            (*values[j]) = obj->GetNamedString(StringReference(STATIC_DATA_PROPERTIES[j].data()))->Data();
        }

        staticData[i] = CurrencyStaticData{ countryCode, countryName, currencyCode, currencyName, currencySymbol };
    }

    auto sortCountryNames = [](const UCM::CurrencyStaticData & s) {
        return ref new String(s.countryName.c_str());
    };

    LocalizationService::GetInstance()->Sort<UCM::CurrencyStaticData>(staticData, sortCountryNames);

    return true;
}

bool CurrencyDataLoader::TryParseAllRatiosData(_In_ String ^ rawJson, _Inout_ CurrencyRatioMap& allRatios)
{
    JsonArray ^ data = nullptr;
    if (!JsonArray::TryParse(rawJson, &data))
    {
        return false;
    }

    wstring sourceCurrencyCode{ DefaultCurrencyCode };

    allRatios.clear();
    for (unsigned int i = 0; i < data->Size; i++)
    {
        JsonObject ^ obj;
        try
        {
            obj = data->GetAt(i)->GetObject();
        }
        catch (COMException^ e)
        {
            if (e->HResult == E_ILLEGAL_METHOD_CALL)
            {
                continue;
            }
            else
            {
                throw;
            }
        }

        // Rt is ratio, An is target currency ISO code.
        double relativeRatio = obj->GetNamedNumber(StringReference(RATIO_KEY));
        wstring targetCurrencyCode = obj->GetNamedString(StringReference(CURRENCY_CODE_KEY))->Data();

        allRatios.emplace(targetCurrencyCode, CurrencyRatio{ relativeRatio, sourceCurrencyCode, targetCurrencyCode });
    }

    return true;
}

// FinalizeUnits
//
// There are a few ways we can get the data needed for Currency Converter, including from cache or from web.
// This function accepts the data from any source, and acts as a 'last-steps' for the converter to be ready.
// This includes identifying which units will be selected and building the map of currency ratios.
#pragma optimize("", off) // Turn off optimizations to work around DevDiv 393321
task<void> CurrencyDataLoader::FinalizeUnits(_In_ const vector<UCM::CurrencyStaticData>& staticData, _In_ const CurrencyRatioMap& ratioMap)
{
    unordered_map<int, pair<UCM::Unit, double>> idToUnit{};

    SelectedUnits defaultCurrencies = co_await GetDefaultFromToCurrency();
    wstring fromCurrency = defaultCurrencies.first;
    wstring toCurrency = defaultCurrencies.second;

    {
        lock_guard<mutex> lock(m_currencyUnitsMutex);

        int i = 1;
        m_currencyUnits.clear();
        m_currencyMetadata.clear();
        bool isConversionSourceSet = false;
        bool isConversionTargetSet = false;
        for (const UCM::CurrencyStaticData& currencyUnit : staticData)
        {
            auto itr = ratioMap.find(currencyUnit.currencyCode);
            if (itr != ratioMap.end() && (itr->second).ratio > 0)
            {
                int id = static_cast<int>(UnitConverterUnits::UnitEnd + i);

                bool isConversionSource = (fromCurrency == currencyUnit.currencyCode);
                isConversionSourceSet = isConversionSourceSet || isConversionSource;

                bool isConversionTarget = (toCurrency == currencyUnit.currencyCode);
                isConversionTargetSet = isConversionTargetSet || isConversionTarget;

                UCM::Unit unit = UCM::Unit{
                    id,                        // id
                    currencyUnit.currencyName, // currencyName
                    currencyUnit.countryName,  // countryName
                    currencyUnit.currencyCode, // abbreviation
                    m_isRtlLanguage,           // isRtlLanguage
                    isConversionSource,        // isConversionSource
                    isConversionTarget         // isConversionTarget
                };

                m_currencyUnits.push_back(unit);
                m_currencyMetadata.emplace(unit, CurrencyUnitMetadata{ currencyUnit.currencySymbol });
                idToUnit.insert(pair<int, pair<UCM::Unit, double>>(unit.id, pair<UCM::Unit, double>(unit, (itr->second).ratio)));
                i++;
            }
        }

        if (!isConversionSourceSet || !isConversionTargetSet)
        {
            GuaranteeSelectedUnits();
            defaultCurrencies = { DEFAULT_FROM_CURRENCY, DEFAULT_TO_CURRENCY };
        }

        m_currencyRatioMap.clear();
        for (const auto& unit : m_currencyUnits)
        {
            unordered_map<UCM::Unit, UCM::ConversionData, UCM::UnitHash> conversions;
            double unitFactor = idToUnit[unit.id].second;
            for (auto itr = idToUnit.begin(); itr != idToUnit.end(); itr++)
            {
                UCM::Unit targetUnit = (itr->second).first;
                double conversionRatio = (itr->second).second;
                UCM::ConversionData parsedData = { 1.0, 0.0, false };
                assert(unitFactor > 0); // divide by zero assert
                parsedData.ratio = conversionRatio / unitFactor;
                conversions.insert(make_pair(targetUnit, parsedData));
            }

            m_currencyRatioMap.insert(make_pair(unit, conversions));
        }
    } // unlocked m_currencyUnitsMutex

    SaveSelectedUnitsToLocalSettings(defaultCurrencies);
};
#pragma optimize("", on)

void CurrencyDataLoader::GuaranteeSelectedUnits()
{
    bool isConversionSourceSet = false;
    bool isConversionTargetSet = false;
    for (UCM::Unit& unit : m_currencyUnits)
    {
        unit.isConversionSource = false;
        unit.isConversionTarget = false;

        if (!isConversionSourceSet && unit.abbreviation == DEFAULT_FROM_CURRENCY)
        {
            unit.isConversionSource = true;
            isConversionSourceSet = true;
        }
        if (!isConversionTargetSet && unit.abbreviation == DEFAULT_TO_CURRENCY)
        {
            unit.isConversionTarget = true;
            isConversionTargetSet = true;
        }
    }

    // If still not set for either source or target, just select the first currency in the list

    if (!m_currencyUnits.empty())
    {
        if (!isConversionSourceSet)
        {
            m_currencyUnits[0].isConversionSource = true;
            isConversionSourceSet = true;
        }

        if (!isConversionTargetSet)
        {
            m_currencyUnits[0].isConversionTarget = true;
            isConversionTargetSet = true;
        }
    }
}

void CurrencyDataLoader::NotifyDataLoadFinished(bool didLoad)
{
    if (!didLoad)
    {
        m_loadStatus = CurrencyLoadStatus::FailedToLoad;
    }

    if (m_vmCallback != nullptr)
    {
        m_vmCallback->CurrencyDataLoadFinished(didLoad);
    }
}

void CurrencyDataLoader::SaveLangCodeAndTimestamp()
{
    ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
    if (localSettings == nullptr)
    {
        return;
    }

    localSettings->Values->Insert(CacheTimestampKey, m_cacheTimestamp);
    localSettings->Values->Insert(CacheLangcodeKey, m_responseLanguage);
}

void CurrencyDataLoader::UpdateDisplayedTimestamp()
{
    if (m_vmCallback != nullptr)
    {
        wstring timestamp = GetCurrencyTimestamp();
        bool isWeekOld = Utils::IsDateTimeOlderThan(m_cacheTimestamp, WEEK_DURATION);

        m_vmCallback->CurrencyTimestampCallback(timestamp, isWeekOld);
    }
}
wstring CurrencyDataLoader::GetCurrencyTimestamp()
{
    DateTime epoch{};
    if (m_cacheTimestamp.UniversalTime != epoch.UniversalTime)
    {
        DateTimeFormatter ^ dateFormatter = ref new DateTimeFormatter(L"shortdate");
        auto date = dateFormatter->Format(m_cacheTimestamp);

        DateTimeFormatter ^ timeFormatter = ref new DateTimeFormatter(L"shorttime");
        auto time = timeFormatter->Format(m_cacheTimestamp);

        return LocalizationStringUtil::GetLocalizedString(m_timestampFormat, date, time)->Data();
    }

    return L"";
}

#pragma optimize("", off) // Turn off optimizations to work around DevDiv 393321
task<SelectedUnits> CurrencyDataLoader::GetDefaultFromToCurrency()
{
    wstring fromCurrency{ DEFAULT_FROM_CURRENCY };
    wstring toCurrency{ DEFAULT_TO_CURRENCY };

    // First, check if we previously stored the last used currencies.
    bool foundInLocalSettings = TryGetLastUsedCurrenciesFromLocalSettings(&fromCurrency, &toCurrency);
    if (!foundInLocalSettings)
    {
        try
        {
            // Second, see if the current locale has preset defaults in DefaultFromToCurrency.json.
            Uri ^ fileUri = ref new Uri(StringReference(DEFAULT_FROM_TO_CURRENCY_FILE_URI));
            StorageFile ^ defaultFromToCurrencyFile = co_await StorageFile::GetFileFromApplicationUriAsync(fileUri);
            if (defaultFromToCurrencyFile != nullptr)
            {
                String ^ fileContents = co_await FileIO::ReadTextAsync(defaultFromToCurrencyFile);
                JsonObject ^ fromToObject = JsonObject::Parse(fileContents);
                JsonObject ^ regionalDefaults = fromToObject->GetNamedObject(m_responseLanguage);

                // Get both values before assignment in-case either fails.
                String ^ selectedFrom = regionalDefaults->GetNamedString(StringReference(FROM_KEY));
                String ^ selectedTo = regionalDefaults->GetNamedString(StringReference(TO_KEY));

                fromCurrency = selectedFrom->Data();
                toCurrency = selectedTo->Data();
            }
        }
        catch (...)
        {
        }
    }

    co_return make_pair(fromCurrency, toCurrency);
};
#pragma optimize("", on)

bool CurrencyDataLoader::TryGetLastUsedCurrenciesFromLocalSettings(_Out_ wstring* const fromCurrency, _Out_ wstring* const toCurrency)
{
    String ^ fromKey = UnitConverterResourceKeys::CurrencyUnitFromKey;
    String ^ toKey = UnitConverterResourceKeys::CurrencyUnitToKey;
    ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
    if (localSettings != nullptr && localSettings->Values != nullptr)
    {
        IPropertySet ^ values = localSettings->Values;
        if (values->HasKey(fromKey) && values->HasKey(toKey))
        {
            *fromCurrency = static_cast<String ^>(values->Lookup(fromKey))->Data();
            *toCurrency = static_cast<String ^>(values->Lookup(toKey))->Data();

            return true;
        }
    }

    return false;
}

void CurrencyDataLoader::SaveSelectedUnitsToLocalSettings(_In_ const SelectedUnits& selectedUnits)
{
    String ^ fromKey = UnitConverterResourceKeys::CurrencyUnitFromKey;
    String ^ toKey = UnitConverterResourceKeys::CurrencyUnitToKey;
    ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
    if (localSettings != nullptr && localSettings->Values != nullptr)
    {
        IPropertySet ^ values = localSettings->Values;
        values->Insert(fromKey, StringReference(selectedUnits.first.c_str()));
        values->Insert(toKey, StringReference(selectedUnits.second.c_str()));
    }
}
