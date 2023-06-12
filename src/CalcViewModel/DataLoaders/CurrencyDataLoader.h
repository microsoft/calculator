// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/UnitConverter.h"
#include "Common/NetworkManager.h"
#include "ICurrencyHttpClient.h"

namespace CalculatorApp
{
    namespace ViewModel::DataLoaders
    {
    public
        enum class CurrencyLoadStatus
        {
            NotLoaded = 0,
            FailedToLoad = 1,
            LoadedFromCache = 2,
            LoadedFromWeb = 3
        };

        namespace UnitConverterResourceKeys
        {
            extern Platform::StringReference CurrencyUnitFromKey;
            extern Platform::StringReference CurrencyUnitToKey;
        }

        namespace CurrencyDataLoaderConstants
        {
            extern Platform::StringReference CacheTimestampKey;
            extern Platform::StringReference CacheLangcodeKey;
            extern Platform::StringReference CacheDelimiter;
            extern Platform::StringReference StaticDataFilename;
            extern Platform::StringReference AllRatiosDataFilename;
            extern long long DayDuration;
        }

        namespace UCM = UnitConversionManager;

        typedef std::unordered_map<std::wstring, UCM::CurrencyRatio> CurrencyRatioMap;
        typedef std::pair<std::wstring, std::wstring> SelectedUnits;

        struct CurrencyUnitMetadata
        {
            CurrencyUnitMetadata(const std::wstring& s)
                : symbol(s)
            {
            }

            const std::wstring symbol;
        };

        class CurrencyDataLoader : public UCM::IConverterDataLoader, public UCM::ICurrencyConverterDataLoader
        {
        public:
            CurrencyDataLoader(
                _In_ std::unique_ptr<CalculatorApp::ViewModel::DataLoaders::ICurrencyHttpClient> client,
                const wchar_t* overrideLanguage = nullptr);
            ~CurrencyDataLoader();

            bool LoadFinished();
            bool LoadedFromCache();
            bool LoadedFromWeb();

            // IConverterDataLoader
            void LoadData() override;
            std::vector<UCM::Category> GetOrderedCategories() override;
            std::vector<UCM::Unit> GetOrderedUnits(const UCM::Category& category) override;
            std::unordered_map<UCM::Unit, UCM::ConversionData, UCM::UnitHash> LoadOrderedRatios(const UCM::Unit& unit) override;
            bool SupportsCategory(const UnitConversionManager::Category& target) override;
            // IConverterDataLoader

            // ICurrencyConverterDataLoader
            void SetViewModelCallback(const std::shared_ptr<UCM::IViewModelCurrencyCallback>& callback) override;
            std::pair<std::wstring, std::wstring> GetCurrencySymbols(const UCM::Unit& unit1, const UCM::Unit& unit2) override;
            std::pair<std::wstring, std::wstring>
            GetCurrencyRatioEquality(_In_ const UnitConversionManager::Unit& unit1, _In_ const UnitConversionManager::Unit& unit2) override;
            std::wstring GetCurrencyTimestamp() override;
            static double RoundCurrencyRatio(double ratio);

            std::future<bool> TryLoadDataFromCacheAsync() override;
            std::future<bool> TryLoadDataFromWebAsync() override;
            std::future<bool> TryLoadDataFromWebOverrideAsync() override;
            // ICurrencyConverterDataLoader

            void OnNetworkBehaviorChanged(CalculatorApp::ViewModel::Common::NetworkAccessBehavior newBehavior);

        private:
            void ResetLoadStatus();
            void NotifyDataLoadFinished(bool didLoad);

            std::future<bool> TryFinishLoadFromCacheAsync();

            bool TryParseWebResponses(
                _In_ Platform::String ^ staticDataJson,
                _In_ Platform::String ^ allRatiosJson,
                _Inout_ std::vector<UCM::CurrencyStaticData>& staticData,
                _Inout_ CurrencyRatioMap& allRatiosData);
            bool TryParseStaticData(_In_ Platform::String ^ rawJson, _Inout_ std::vector<UCM::CurrencyStaticData>& staticData);
            bool TryParseAllRatiosData(_In_ Platform::String ^ rawJson, _Inout_ CurrencyRatioMap& allRatiosData);
            concurrency::task<void> FinalizeUnits(_In_ const std::vector<UCM::CurrencyStaticData>& staticData, _In_ const CurrencyRatioMap& ratioMap);
            void GuaranteeSelectedUnits();

            void SaveLangCodeAndTimestamp();
            void UpdateDisplayedTimestamp();

            void RegisterForNetworkBehaviorChanges();
            void UnregisterForNetworkBehaviorChanges();

            concurrency::task<SelectedUnits> GetDefaultFromToCurrency();
            bool TryGetLastUsedCurrenciesFromLocalSettings(_Out_ std::wstring* const fromCurrency, _Out_ std::wstring* const toCurrency);
            void SaveSelectedUnitsToLocalSettings(_In_ const SelectedUnits& selectedUnits);

        private:
            Platform::String ^ m_responseLanguage;
            std::unique_ptr<CalculatorApp::ViewModel::DataLoaders::ICurrencyHttpClient> m_client;

            bool m_isRtlLanguage;

            std::mutex m_currencyUnitsMutex;
            std::vector<UCM::Unit> m_currencyUnits;
            UCM::UnitToUnitToConversionDataMap m_currencyRatioMap;
            std::unordered_map<UCM::Unit, CurrencyUnitMetadata, UCM::UnitHash> m_currencyMetadata;

            std::shared_ptr<UCM::IViewModelCurrencyCallback> m_vmCallback;

            Windows::Globalization::NumberFormatting::DecimalFormatter ^ m_ratioFormatter;
            Platform::String ^ m_ratioFormat;
            Windows::Foundation::DateTime m_cacheTimestamp;
            Platform::String ^ m_timestampFormat;

            CurrencyLoadStatus m_loadStatus;

            CalculatorApp::ViewModel::Common::NetworkManager ^ m_networkManager;
            CalculatorApp::ViewModel::Common::NetworkAccessBehavior m_networkAccessBehavior;
            Windows::Foundation::EventRegistrationToken m_networkBehaviorToken;
            bool m_meteredOverrideSet;
        };
    }
}
