// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

#include "CalcViewModel/DataLoaders/CurrencyDataLoader.h"
#include "CalcViewModel/Common/LocalizationService.h"

using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::LocalizationServiceProperties;
using namespace CalculatorApp::DataLoaders;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorUnitTests;
using namespace Concurrency;
using namespace Platform;
using namespace std;
using namespace UnitConversionManager;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Web::Http;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorApp
{
    namespace DataLoaders
    {
        class MockCurrencyHttpClientWithResult : public CurrencyHttpClient
        {
        public:
            MockCurrencyHttpClientWithResult(String ^ staticResponse, String ^ allRatiosResponse)
                : m_staticResponse(staticResponse)
                , m_allRatiosResponse(allRatiosResponse)
            {
            }

            IAsyncOperationWithProgress<String ^, HttpProgress> ^ GetCurrencyMetadata() override
            {
                return ref new MockAsyncOperationWithProgress(m_staticResponse);
            }

            IAsyncOperationWithProgress<String ^, HttpProgress> ^ GetCurrencyRatios() override
            {
                return ref new MockAsyncOperationWithProgress(m_allRatiosResponse);
            }

        private:
            String ^ m_staticResponse;
            String ^ m_allRatiosResponse;
        };

        class MockCurrencyHttpClientThrowsException : public CurrencyHttpClient
        {
        public:
            MockCurrencyHttpClientThrowsException()
            {
            }

            IAsyncOperationWithProgress<String ^, HttpProgress> ^ GetCurrencyMetadata() override
            {
                throw ref new NotImplementedException();
            }

            IAsyncOperationWithProgress<String ^, HttpProgress> ^ GetCurrencyRatios() override
            {
                throw ref new NotImplementedException();
            }
        };
    }
}

class DataLoadedCallback : public UnitConversionManager::IViewModelCurrencyCallback
{
public:
    DataLoadedCallback(task_completion_event<void> tce)
        : m_task_completion_event{ tce }
    {
    }

    void CurrencyDataLoadFinished(bool /*didLoad*/) override
    {
        m_task_completion_event.set();
    }

    void CurrencySymbolsCallback(_In_ const wstring& /*fromSymbol*/, _In_ const wstring& /*toSymbol*/) override
    {
    }
    void CurrencyRatiosCallback(_In_ const wstring& /*ratioEquality*/, _In_ const wstring& /*accRatioEquality*/) override
    {
    }
    void CurrencyTimestampCallback(_In_ const std::wstring& /*timestamp*/, bool /*isWeekOldData*/) override
    {
    }
    void NetworkBehaviorChanged(_In_ int /*newBehavior*/) override
    {
    }

private:
    Concurrency::task_completion_event<void> m_task_completion_event;
};

namespace CalculatorUnitTests
{
    constexpr auto sc_Language_EN = L"en-US";

    unique_ptr<CurrencyDataLoader> MakeLoaderWithResults(String ^ staticResponse, String ^ allRatiosResponse)
    {
        auto client = make_unique<MockCurrencyHttpClientWithResult>(staticResponse, allRatiosResponse);
        client->SetSourceCurrencyCode(StringReference(DefaultCurrencyCode.data()));
        return make_unique<CurrencyDataLoader>(move(client));
    }

    String^ SerializeContent(const vector<String^>& data)
    {
        String^ result = L"";
        String^ delimiter = CurrencyDataLoaderConstants::CacheDelimiter;
        for (String^ content : data)
        {
            result += (delimiter + content);
        }

        return result;
    }

    bool WriteToFileInLocalCacheFolder(String^ filename, String^ content)
    {
        try
        {
            StorageFolder ^ localFolder = ApplicationData::Current->LocalCacheFolder;
            StorageFile ^ file = create_task(localFolder->CreateFileAsync(filename, CreationCollisionOption::ReplaceExisting)).get();
            create_task(FileIO::WriteTextAsync(file, content)).wait();
            return true;
        }
        catch (Exception ^ ex)
        {
            return false;
        }
    }

    bool DeleteFileFromLocalCacheFolder(String ^ filename)
    {
        try
        {
            StorageFolder ^ folder = ApplicationData::Current->LocalCacheFolder;
            IAsyncOperation<StorageFile ^> ^ fileOperation = folder->GetFileAsync(filename);
            StorageFile ^ file = create_task(fileOperation).get();
            create_task(file->DeleteAsync()).get();
            return true;
        }
        catch (Platform::Exception ^ ex)
        {
            // FileNotFoundException is a valid result
            return ex->HResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
        catch (...)
        {
            return false;
        }
    }

    bool DeleteCurrencyCacheFiles()
    {
        try
        {
            bool deletedStaticData = DeleteFileFromLocalCacheFolder(CurrencyDataLoaderConstants::StaticDataFilename);
            bool deletedAllRatiosData = DeleteFileFromLocalCacheFolder(CurrencyDataLoaderConstants::AllRatiosDataFilename);

            return deletedStaticData && deletedAllRatiosData;
        }
        catch (...)
        {
            return false;
        }
    }

    void InsertToLocalSettings(String ^ key, Object ^ value)
    {
        ApplicationData::Current->LocalSettings->Values->Insert(key, value);
    }

    void RemoveFromLocalSettings(String ^ key)
    {
        // Safe to call, even if the key does not exist.
        ApplicationData::Current->LocalSettings->Values->Remove(key);
    }

    void StandardCacheSetup()
    {
        // Insert current time so data is less than a day old.
        DateTime now = Utils::GetUniversalSystemTime();
        InsertToLocalSettings(CurrencyDataLoaderConstants::CacheTimestampKey, now);
        InsertToLocalSettings(CurrencyDataLoaderConstants::CacheLangcodeKey, StringReference(sc_Language_EN));

        VERIFY_IS_TRUE(DeleteCurrencyCacheFiles());

        VERIFY_IS_TRUE(WriteToFileInLocalCacheFolder(CurrencyDataLoaderConstants::StaticDataFilename, CurrencyHttpClient::GetRawStaticDataResponse()));
        VERIFY_IS_TRUE(WriteToFileInLocalCacheFolder(CurrencyDataLoaderConstants::AllRatiosDataFilename, CurrencyHttpClient::GetRawAllRatiosDataResponse()));
    }

    TEST_CLASS(CurrencyConverterLoadTests){ public: TEST_METHOD_INITIALIZE(DeleteCacheFiles){ DeleteCurrencyCacheFiles();
}

TEST_METHOD(LoadFromCache_Fail_NoCacheKey)
{
    RemoveFromLocalSettings(CurrencyDataLoaderConstants::CacheTimestampKey);
    CurrencyDataLoader loader(nullptr, L"en-US");

    bool didLoad = loader.TryLoadDataFromCacheAsync().get();

    VERIFY_IS_FALSE(didLoad);
    VERIFY_IS_FALSE(loader.LoadFinished());
    VERIFY_IS_FALSE(loader.LoadedFromCache());
}

TEST_METHOD(LoadFromCache_Fail_OlderThanADay)
{
    // Insert 24 hours ago so data is considered stale.
    // This will cause the load from cache to fail.
    DateTime now = Utils::GetUniversalSystemTime();
    DateTime dayOld;
    dayOld.UniversalTime = now.UniversalTime - CurrencyDataLoaderConstants::DayDuration - 1;
    InsertToLocalSettings(CurrencyDataLoaderConstants::CacheTimestampKey, dayOld);

    CurrencyDataLoader loader(nullptr, L"en-US");

    bool didLoad = loader.TryLoadDataFromCacheAsync().get();

    VERIFY_IS_FALSE(didLoad);
    VERIFY_IS_FALSE(loader.LoadFinished());
    VERIFY_IS_FALSE(loader.LoadedFromCache());
}

TEST_METHOD(LoadFromCache_Fail_StaticDataFileDoesNotExist)
{
    // Insert current time so data is less than a day old.
    // This will cause the load to continue to attempt to load the file.
    DateTime now = Utils::GetUniversalSystemTime();
    InsertToLocalSettings(CurrencyDataLoaderConstants::CacheTimestampKey, now);

    VERIFY_IS_TRUE(DeleteFileFromLocalCacheFolder(CurrencyDataLoaderConstants::StaticDataFilename));
    VERIFY_IS_TRUE(WriteToFileInLocalCacheFolder(CurrencyDataLoaderConstants::AllRatiosDataFilename, CurrencyHttpClient::GetRawAllRatiosDataResponse()));

    CurrencyDataLoader loader(nullptr, L"en-US");

    bool didLoad = loader.TryLoadDataFromCacheAsync().get();

    VERIFY_IS_FALSE(didLoad);
    VERIFY_IS_FALSE(loader.LoadFinished());
    VERIFY_IS_FALSE(loader.LoadedFromCache());
}

TEST_METHOD(LoadFromCache_Fail_AllRatiosDataFileDoesNotExist)
{
    // Insert current time so data is less than a day old.
    // This will cause the load to continue to attempt to load the file.
    DateTime now = Utils::GetUniversalSystemTime();
    InsertToLocalSettings(CurrencyDataLoaderConstants::CacheTimestampKey, now);

    VERIFY_IS_TRUE(WriteToFileInLocalCacheFolder(CurrencyDataLoaderConstants::StaticDataFilename, CurrencyHttpClient::GetRawStaticDataResponse()));
    VERIFY_IS_TRUE(DeleteFileFromLocalCacheFolder(CurrencyDataLoaderConstants::AllRatiosDataFilename));

    CurrencyDataLoader loader(nullptr, L"en-US");

    bool didLoad = loader.TryLoadDataFromCacheAsync().get();

    VERIFY_IS_FALSE(didLoad);
    VERIFY_IS_FALSE(loader.LoadFinished());
    VERIFY_IS_FALSE(loader.LoadedFromCache());
}

TEST_METHOD(LoadFromCache_Fail_ResponseLanguageChanged)
{
    DateTime now = Utils::GetUniversalSystemTime();
    InsertToLocalSettings(CurrencyDataLoaderConstants::CacheTimestampKey, now);

    // Tests always use en-US as response language. Insert a different lang-code to fail the test.
    InsertToLocalSettings(CurrencyDataLoaderConstants::CacheLangcodeKey, L"ar-SA");

    VERIFY_IS_TRUE(WriteToFileInLocalCacheFolder(CurrencyDataLoaderConstants::StaticDataFilename, CurrencyHttpClient::GetRawStaticDataResponse()));
    VERIFY_IS_TRUE(DeleteFileFromLocalCacheFolder(CurrencyDataLoaderConstants::AllRatiosDataFilename));

    CurrencyDataLoader loader(nullptr, L"en-US");

    bool didLoad = loader.TryLoadDataFromCacheAsync().get();

    VERIFY_IS_FALSE(didLoad);
    VERIFY_IS_FALSE(loader.LoadFinished());
    VERIFY_IS_FALSE(loader.LoadedFromCache());
}

TEST_METHOD(LoadFromCache_Success)
{
    StandardCacheSetup();

    CurrencyDataLoader loader(nullptr, L"en-US");

    bool didLoad = loader.TryLoadDataFromCacheAsync().get();

    VERIFY_IS_TRUE(didLoad);
    VERIFY_IS_TRUE(loader.LoadFinished());
    VERIFY_IS_TRUE(loader.LoadedFromCache());
}

TEST_METHOD(LoadFromWeb_Fail_ClientIsNullptr)
{
    CurrencyDataLoader loader(nullptr, L"en-US");

    bool didLoad = loader.TryLoadDataFromWebAsync().get();

    VERIFY_IS_FALSE(didLoad);
    VERIFY_IS_FALSE(loader.LoadFinished());
    VERIFY_IS_FALSE(loader.LoadedFromWeb());
}

TEST_METHOD(LoadFromWeb_Fail_WebException)
{
    CurrencyDataLoader loader(make_unique<MockCurrencyHttpClientThrowsException>(), L"en-US");

    bool didLoad = loader.TryLoadDataFromWebAsync().get();

    VERIFY_IS_FALSE(didLoad);
    VERIFY_IS_FALSE(loader.LoadFinished());
    VERIFY_IS_FALSE(loader.LoadedFromWeb());
}

TEST_METHOD(LoadFromWeb_Success)
{
    String ^ staticResponse = CurrencyHttpClient::GetRawStaticDataResponse();
    String ^ allRatiosResponse = CurrencyHttpClient::GetRawAllRatiosDataResponse();
    unique_ptr<CurrencyDataLoader> loader = MakeLoaderWithResults(staticResponse, allRatiosResponse);

    bool didLoad = loader->TryLoadDataFromWebAsync().get();

    VERIFY_IS_TRUE(didLoad);
    VERIFY_IS_TRUE(loader->LoadFinished());
    VERIFY_IS_TRUE(loader->LoadedFromWeb());
}

TEST_METHOD(Load_Success_LoadedFromCache)
{
    StandardCacheSetup();
    CurrencyDataLoader loader(nullptr, L"en-US");

    auto data_loaded_event = task_completion_event<void>();
    loader.SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

    auto data_loaded_task = create_task(data_loaded_event);
    loader.LoadData();
    data_loaded_task.wait();

    VERIFY_IS_TRUE(loader.LoadFinished());
    VERIFY_IS_TRUE(loader.LoadedFromCache());
    VERIFY_IS_FALSE(loader.LoadedFromWeb());
}

TEST_METHOD(Load_Success_LoadedFromWeb)
{
    // Insert 24 hours ago so data is considered stale.
    // This will cause the load from cache to fail.
    DateTime now = Utils::GetUniversalSystemTime();
    DateTime dayOld;
    dayOld.UniversalTime = now.UniversalTime - CurrencyDataLoaderConstants::DayDuration - 1;
    InsertToLocalSettings(CurrencyDataLoaderConstants::CacheTimestampKey, dayOld);

    String ^ staticResponse = CurrencyHttpClient::GetRawStaticDataResponse();
    String ^ allRatiosResponse = CurrencyHttpClient::GetRawAllRatiosDataResponse();
    unique_ptr<CurrencyDataLoader> loader = MakeLoaderWithResults(staticResponse, allRatiosResponse);

    auto data_loaded_event = task_completion_event<void>();
    loader->SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

    auto data_loaded_task = create_task(data_loaded_event);
    loader->LoadData();
    data_loaded_task.wait();

    VERIFY_IS_TRUE(loader->LoadFinished());
    VERIFY_IS_FALSE(loader->LoadedFromCache());
    VERIFY_IS_TRUE(loader->LoadedFromWeb());
}
}
;

TEST_CLASS(CurrencyConverterUnitTests)
{
    const UCM::Category CURRENCY_CATEGORY = { NavCategory::Serialize(ViewMode::Currency), L"Currency", false /*supportsNegative*/ };

    const UCM::Unit GetUnit(const vector<UCM::Unit>& unitList, const wstring& target)
    {
        return *find_if(begin(unitList), end(unitList), [&target](const UCM::Unit& u) { return u.abbreviation == target; });
    }

    TEST_METHOD(Loaded_LoadOrderedUnits)
    {
        StandardCacheSetup();
        CurrencyDataLoader loader(nullptr, L"en-US");

        auto data_loaded_event = task_completion_event<void>();
        loader.SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

        auto data_loaded_task = create_task(data_loaded_event);
        loader.LoadData();
        data_loaded_task.wait();

        VERIFY_IS_TRUE(loader.LoadFinished());
        VERIFY_IS_TRUE(loader.LoadedFromCache());
        VERIFY_IS_FALSE(loader.LoadedFromWeb());

        vector<UCM::Unit> unitList = loader.GetOrderedUnits(CURRENCY_CATEGORY);
        VERIFY_ARE_EQUAL(size_t{ 2 }, unitList.size());

        const UCM::Unit usdUnit = GetUnit(unitList, L"USD");
        const UCM::Unit eurUnit = GetUnit(unitList, L"EUR");

        VERIFY_ARE_EQUAL(wstring(L"United States - Dollar"), usdUnit.name);
        VERIFY_ARE_EQUAL(wstring(L"USD"), usdUnit.abbreviation);

        VERIFY_ARE_EQUAL(wstring(L"Europe - Euro"), eurUnit.name);
        VERIFY_ARE_EQUAL(wstring(L"EUR"), eurUnit.abbreviation);
    }

    TEST_METHOD(Loaded_LoadOrderedRatios)
    {
        StandardCacheSetup();

        CurrencyDataLoader loader(nullptr, L"en-US");

        auto data_loaded_event = task_completion_event<void>();
        loader.SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

        auto data_loaded_task = create_task(data_loaded_event);
        loader.LoadData();
        data_loaded_task.wait();

        VERIFY_IS_TRUE(loader.LoadFinished());
        VERIFY_IS_TRUE(loader.LoadedFromCache());
        VERIFY_IS_FALSE(loader.LoadedFromWeb());

        vector<UCM::Unit> unitList = loader.GetOrderedUnits(CURRENCY_CATEGORY);
        VERIFY_ARE_EQUAL(size_t{ 2 }, unitList.size());

        const UCM::Unit usdUnit = GetUnit(unitList, L"USD");
        const UCM::Unit eurUnit = GetUnit(unitList, L"EUR");

        unordered_map<UCM::Unit, UCM::ConversionData, UCM::UnitHash> ratios = loader.LoadOrderedRatios(usdUnit);
        VERIFY_ARE_EQUAL(size_t{ 2 }, ratios.size());

        UCM::ConversionData usdRatioData = ratios[usdUnit];
        VERIFY_IS_TRUE((std::abs(1.0 - usdRatioData.ratio) < 1e-1));

        UCM::ConversionData eurRatioData = ratios[eurUnit];
        VERIFY_IS_TRUE((std::abs(0.920503 - eurRatioData.ratio) < 1e-6));
    }

    TEST_METHOD(Loaded_GetCurrencySymbols_Valid)
    {
        StandardCacheSetup();

        CurrencyDataLoader loader(nullptr, L"en-US");

        auto data_loaded_event = task_completion_event<void>();
        loader.SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

        auto data_loaded_task = create_task(data_loaded_event);
        loader.LoadData();
        data_loaded_task.wait();

        VERIFY_IS_TRUE(loader.LoadFinished());
        VERIFY_IS_TRUE(loader.LoadedFromCache());
        VERIFY_IS_FALSE(loader.LoadedFromWeb());

        vector<UCM::Unit> unitList = loader.GetOrderedUnits(CURRENCY_CATEGORY);
        VERIFY_ARE_EQUAL(size_t{ 2 }, unitList.size());

        const UCM::Unit usdUnit = GetUnit(unitList, L"USD");
        const UCM::Unit eurUnit = GetUnit(unitList, L"EUR");

        const pair<wstring, wstring> symbols = loader.GetCurrencySymbols(usdUnit, eurUnit);

        VERIFY_ARE_EQUAL(wstring(L"$"), symbols.first);
        VERIFY_ARE_EQUAL(wstring(L"\x20ac"), symbols.second); // €
    }

    TEST_METHOD(Loaded_GetCurrencySymbols_Invalid)
    {
        StandardCacheSetup();

        CurrencyDataLoader loader(nullptr, L"en-US");

        auto data_loaded_event = task_completion_event<void>();
        loader.SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

        auto data_loaded_task = create_task(data_loaded_event);
        loader.LoadData();
        data_loaded_task.wait();

        VERIFY_IS_TRUE(loader.LoadFinished());
        VERIFY_IS_TRUE(loader.LoadedFromCache());
        VERIFY_IS_FALSE(loader.LoadedFromWeb());

        const UCM::Unit fakeUnit1 = { 1, L"fakeUnit1", L"FUD1", false, false, false };

        const UCM::Unit fakeUnit2 = { 2, L"fakeUnit2", L"FUD2", false, false, false };

        pair<wstring, wstring> symbols = loader.GetCurrencySymbols(fakeUnit1, fakeUnit2);

        VERIFY_ARE_EQUAL(wstring(L""), wstring(symbols.first.c_str()));
        VERIFY_ARE_EQUAL(wstring(L""), wstring(symbols.second.c_str()));

        // Verify that when only one unit is valid, both symbols return as empty string.
        vector<UCM::Unit> unitList = loader.GetOrderedUnits(CURRENCY_CATEGORY);
        VERIFY_ARE_EQUAL(size_t{ 2 }, unitList.size());

        const UCM::Unit usdUnit = GetUnit(unitList, L"USD");

        symbols = loader.GetCurrencySymbols(fakeUnit1, usdUnit);

        VERIFY_ARE_EQUAL(wstring(L""), symbols.first);
        VERIFY_ARE_EQUAL(wstring(L""), symbols.second);

        symbols = loader.GetCurrencySymbols(usdUnit, fakeUnit1);

        VERIFY_ARE_EQUAL(wstring(L""), symbols.first);
        VERIFY_ARE_EQUAL(wstring(L""), symbols.second);
    }

    TEST_METHOD(Loaded_GetCurrencyRatioEquality_Valid)
    {
        StandardCacheSetup();

        CurrencyDataLoader loader(nullptr, L"en-US");

        auto data_loaded_event = task_completion_event<void>();
        loader.SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

        auto data_loaded_task = create_task(data_loaded_event);
        loader.LoadData();
        data_loaded_task.wait();

        VERIFY_IS_TRUE(loader.LoadFinished());
        VERIFY_IS_TRUE(loader.LoadedFromCache());
        VERIFY_IS_FALSE(loader.LoadedFromWeb());

        vector<UCM::Unit> unitList = loader.GetOrderedUnits(CURRENCY_CATEGORY);
        VERIFY_ARE_EQUAL(size_t{ 2 }, unitList.size());

        const UCM::Unit usdUnit = GetUnit(unitList, L"USD");
        const UCM::Unit eurUnit = GetUnit(unitList, L"EUR");

        const pair<wstring, wstring> ratio = loader.GetCurrencyRatioEquality(usdUnit, eurUnit);

        VERIFY_ARE_EQUAL(wstring(L"1 USD = 0.9205 EUR"), ratio.first);
        VERIFY_ARE_EQUAL(wstring(L"1 United States Dollar = 0.9205 Europe Euro"), ratio.second);
    }

    TEST_METHOD(Loaded_GetCurrencyRatioEquality_Invalid)
    {
        StandardCacheSetup();

        CurrencyDataLoader loader(nullptr, L"en-US");

        auto data_loaded_event = task_completion_event<void>();
        loader.SetViewModelCallback(make_shared<DataLoadedCallback>(data_loaded_event));

        auto data_loaded_task = create_task(data_loaded_event);
        loader.LoadData();
        data_loaded_task.wait();

        VERIFY_IS_TRUE(loader.LoadFinished());
        VERIFY_IS_TRUE(loader.LoadedFromCache());
        VERIFY_IS_FALSE(loader.LoadedFromWeb());

        const UCM::Unit fakeUnit1 = { 1, L"fakeUnit1", L"fakeCountry1", L"FUD1", false, false, false };
        const UCM::Unit fakeUnit2 = { 2, L"fakeUnit2", L"fakeCountry2", L"FUD2", false, false, false };

        pair<wstring, wstring> ratio = loader.GetCurrencyRatioEquality(fakeUnit1, fakeUnit2);

        VERIFY_ARE_EQUAL(wstring(L""), ratio.first);
        VERIFY_ARE_EQUAL(wstring(L""), ratio.second);

        // Verify that when only one unit is valid, both symbols return as empty string.
        vector<UCM::Unit> unitList = loader.GetOrderedUnits(CURRENCY_CATEGORY);
        VERIFY_ARE_EQUAL(size_t{ 2 }, unitList.size());

        const UCM::Unit usdUnit = GetUnit(unitList, L"USD");

        ratio = loader.GetCurrencyRatioEquality(fakeUnit1, usdUnit);

        VERIFY_ARE_EQUAL(wstring(L""), ratio.first);
        VERIFY_ARE_EQUAL(wstring(L""), ratio.second);

        ratio = loader.GetCurrencyRatioEquality(usdUnit, fakeUnit1);

        VERIFY_ARE_EQUAL(wstring(L""), ratio.first);
        VERIFY_ARE_EQUAL(wstring(L""), ratio.second);
    }

    TEST_METHOD(Test_RoundCurrencyRatio)
    {
        CurrencyDataLoader loader{ nullptr };
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(1234567), 1234567);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0), 0);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(9999.999), 9999.999);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(8765.4321), 8765.4321);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(4815.162342), 4815.1623);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(4815.162358), 4815.1624);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(4815.162388934723), 4815.1624);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.12), 0.12);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.123), 0.123);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.1234), 0.1234);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.12343), 0.1234);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.0321), 0.0321);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.03211), 0.03211);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.032119), 0.03212);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.00322119), 0.003221);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.00123269), 0.001233);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.00076269), 0.0007627);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.000069), 0.000069);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.000061), 0.000061);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.000054612), 0.00005461);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.000054616), 0.00005462);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.000005416), 0.000005416);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.0000016134324), 0.000001613);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.0000096134324), 0.000009613);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.0000032169348392), 0.000003217);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.000000002134987218), 0.000000002135);
        VERIFY_ARE_EQUAL(CurrencyDataLoader::RoundCurrencyRatio(0.000000000000087231445), 0.00000000000008723);
    }
};
}
