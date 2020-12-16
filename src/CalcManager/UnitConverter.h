// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <vector>
#include <unordered_map>
#include <future>
#include "sal_cross_platform.h" // for SAL
#include <memory>               // for std::shared_ptr

namespace UnitConversionManager
{
    enum class Command;

    struct Unit
    {
        Unit()
        {
        }
        Unit(int id, std::wstring_view name, std::wstring abbreviation, bool isConversionSource, bool isConversionTarget, bool isWhimsical)
            : id(id)
            , name(name)
            , accessibleName(name)
            , abbreviation(std::move(abbreviation))
            , isConversionSource(isConversionSource)
            , isConversionTarget(isConversionTarget)
            , isWhimsical(isWhimsical)
        {
        }

        Unit(
            int id,
            std::wstring_view currencyName,
            std::wstring_view countryName,
            std::wstring abbreviation,
            bool isRtlLanguage,
            bool isConversionSource,
            bool isConversionTarget)
            : id(id)
            , abbreviation(std::move(abbreviation))
            , isConversionSource(isConversionSource)
            , isConversionTarget(isConversionTarget)
            , isWhimsical(false)
        {
            auto nameValue1 = isRtlLanguage ? currencyName : countryName;
            auto nameValue2 = isRtlLanguage ? countryName : currencyName;

            name = nameValue1;
            name.append(L" - ").append(nameValue2);

            accessibleName = nameValue1;
            accessibleName.append(1, L' ').append(nameValue2);
        }

        int id;
        std::wstring name;
        std::wstring accessibleName;
        std::wstring abbreviation;
        bool isConversionSource;
        bool isConversionTarget;
        bool isWhimsical;

        bool operator!=(const Unit& that) const
        {
            return that.id != id;
        }

        bool operator==(const Unit& that) const
        {
            return that.id == id;
        }
    };

    // The EMPTY_UNIT acts as a 'null-struct' so that
    // Unit pointers can safely be dereferenced without
    // null checks.
    //
    // unitId, name, abbreviation, isConversionSource, isConversionTarget, isWhimsical
    inline const Unit EMPTY_UNIT = Unit{ -1, L"", L"", true, true, false };

    struct Category
    {
        Category()
        {
        }

        Category(int id, std::wstring name, bool supportsNegative)
            : id(id)
            , name(std::move(name))
            , supportsNegative(supportsNegative)
        {
        }

        int id;
        std::wstring name;
        bool supportsNegative;

        bool operator!=(const Category& that) const
        {
            return that.id != id;
        }

        bool operator==(const Category& that) const
        {
            return that.id == id;
        }
    };

    class UnitHash
    {
    public:
        size_t operator()(const Unit& x) const
        {
            return x.id;
        }
    };

    struct SuggestedValueIntermediate
    {
        double magnitude;
        double value;
        Unit type;
    };

    struct ConversionData
    {
        ConversionData()
        {
        }
        ConversionData(double ratio, double offset, bool offsetFirst)
            : ratio(ratio)
            , offset(offset)
            , offsetFirst(offsetFirst)
        {
        }

        double ratio;
        double offset;
        bool offsetFirst;
    };

    struct CurrencyStaticData
    {
        std::wstring countryCode;
        std::wstring countryName;
        std::wstring currencyCode;
        std::wstring currencyName;
        std::wstring currencySymbol;
    };

    struct CurrencyRatio
    {
        double ratio;
        std::wstring sourceCurrencyCode;
        std::wstring targetCurrencyCode;
    };

    typedef std::tuple<std::vector<UnitConversionManager::Unit>, UnitConversionManager::Unit, UnitConversionManager::Unit> CategorySelectionInitializer;
    typedef std::unordered_map<
        UnitConversionManager::Unit,
        std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash>,
        UnitConversionManager::UnitHash>
        UnitToUnitToConversionDataMap;
    typedef std::unordered_map<int, std::vector<UnitConversionManager::Unit>>
        CategoryToUnitVectorMap;

    class IViewModelCurrencyCallback
    {
    public:
        virtual ~IViewModelCurrencyCallback(){};
        virtual void CurrencyDataLoadFinished(bool didLoad) = 0;
        virtual void CurrencySymbolsCallback(_In_ const std::wstring& fromSymbol, _In_ const std::wstring& toSymbol) = 0;
        virtual void CurrencyRatiosCallback(_In_ const std::wstring& ratioEquality, _In_ const std::wstring& accRatioEquality) = 0;
        virtual void CurrencyTimestampCallback(_In_ const std::wstring& timestamp, bool isWeekOldData) = 0;
        virtual void NetworkBehaviorChanged(_In_ int newBehavior) = 0;
    };

    class IConverterDataLoader
    {
    public:
        virtual ~IConverterDataLoader(){};
        virtual void LoadData() = 0; // prepare data if necessary before calling other functions
        virtual std::vector<Category> GetOrderedCategories() = 0;
        virtual std::vector<Unit> GetOrderedUnits(const Category& c) = 0;
        virtual std::unordered_map<Unit, ConversionData, UnitHash> LoadOrderedRatios(const Unit& u) = 0;
        virtual bool SupportsCategory(const Category& target) = 0;
    };

    class ICurrencyConverterDataLoader
    {
    public:
        virtual void SetViewModelCallback(const std::shared_ptr<UnitConversionManager::IViewModelCurrencyCallback>& callback) = 0;
        virtual std::pair<std::wstring, std::wstring>
        GetCurrencySymbols(_In_ const UnitConversionManager::Unit& unit1, _In_ const UnitConversionManager::Unit& unit2) = 0;
        virtual std::pair<std::wstring, std::wstring>
        GetCurrencyRatioEquality(_In_ const UnitConversionManager::Unit& unit1, _In_ const UnitConversionManager::Unit& unit2) = 0;
        virtual std::wstring GetCurrencyTimestamp() = 0;

        virtual std::future<bool> TryLoadDataFromCacheAsync() = 0;
        virtual std::future<bool> TryLoadDataFromWebAsync() = 0;
        virtual std::future<bool> TryLoadDataFromWebOverrideAsync() = 0;
    };

    class IUnitConverterVMCallback
    {
    public:
        virtual ~IUnitConverterVMCallback(){};
        virtual void DisplayCallback(const std::wstring& from, const std::wstring& to) = 0;
        virtual void SuggestedValueCallback(const std::vector<std::tuple<std::wstring, Unit>>& suggestedValues) = 0;
        virtual void MaxDigitsReached() = 0;
    };

    class IUnitConverter
    {
    public:
        virtual ~IUnitConverter()
        {
        }
        virtual void Initialize() = 0; // Use to initialize first time, use deserialize instead to rehydrate
        virtual std::vector<Category> GetCategories() = 0;
        virtual CategorySelectionInitializer SetCurrentCategory(const Category& input) = 0;
        virtual Category GetCurrentCategory() = 0;
        virtual void SetCurrentUnitTypes(const Unit& fromType, const Unit& toType) = 0;
        virtual void SwitchActive(const std::wstring& newValue) = 0;
        virtual bool IsSwitchedActive() const = 0;
        virtual std::wstring SaveUserPreferences() = 0;
        virtual void RestoreUserPreferences(_In_ std::wstring_view userPreferences) = 0;
        virtual void SendCommand(Command command) = 0;
        virtual void SetViewModelCallback(_In_ const std::shared_ptr<IUnitConverterVMCallback>& newCallback) = 0;
        virtual void SetViewModelCurrencyCallback(_In_ const std::shared_ptr<IViewModelCurrencyCallback>& newCallback) = 0;
        virtual std::future<std::pair<bool, std::wstring>> RefreshCurrencyRatios() = 0;
        virtual void Calculate() = 0;
        virtual void ResetCategoriesAndRatios() = 0;
    };

    class UnitConverter : public IUnitConverter, public std::enable_shared_from_this<UnitConverter>
    {
    public:
        UnitConverter(_In_ const std::shared_ptr<IConverterDataLoader>& dataLoader);
        UnitConverter(_In_ const std::shared_ptr<IConverterDataLoader>& dataLoader, _In_ const std::shared_ptr<IConverterDataLoader>& currencyDataLoader);

        // IUnitConverter
        void Initialize() override;
        std::vector<Category> GetCategories() override;
        CategorySelectionInitializer SetCurrentCategory(const Category& input) override;
        Category GetCurrentCategory() override;
        void SetCurrentUnitTypes(const Unit& fromType, const Unit& toType) override;
        void SwitchActive(const std::wstring& newValue) override;
        bool IsSwitchedActive() const override;
        std::wstring SaveUserPreferences() override;
        void RestoreUserPreferences(std::wstring_view userPreference) override;
        void SendCommand(Command command) override;
        void SetViewModelCallback(_In_ const std::shared_ptr<IUnitConverterVMCallback>& newCallback) override;
        void SetViewModelCurrencyCallback(_In_ const std::shared_ptr<IViewModelCurrencyCallback>& newCallback) override;
        std::future<std::pair<bool, std::wstring>> RefreshCurrencyRatios() override;
        void Calculate() override;
        void ResetCategoriesAndRatios() override;
        // IUnitConverter

        static std::vector<std::wstring> StringToVector(std::wstring_view w, std::wstring_view delimiter, bool addRemainder = false);
        static std::wstring Quote(std::wstring_view s);
        static std::wstring Unquote(std::wstring_view s);

    private:
        bool CheckLoad();
        double Convert(double value, const ConversionData& conversionData);
        std::vector<std::tuple<std::wstring, Unit>> CalculateSuggested();
        void ClearValues();
        void InitializeSelectedUnits();
        Category StringToCategory(std::wstring_view w);
        std::wstring CategoryToString(const Category& c, std::wstring_view delimiter);
        std::wstring UnitToString(const Unit& u, std::wstring_view delimiter);
        Unit StringToUnit(std::wstring_view w);
        void UpdateCurrencySymbols();
        void UpdateViewModel();
        bool AnyUnitIsEmpty();
        std::shared_ptr<IConverterDataLoader> GetDataLoaderForCategory(const Category& category);
        std::shared_ptr<ICurrencyConverterDataLoader> GetCurrencyConverterDataLoader();

    private:
        std::shared_ptr<IConverterDataLoader> m_dataLoader;
        std::shared_ptr<IConverterDataLoader> m_currencyDataLoader;
        std::shared_ptr<IUnitConverterVMCallback> m_vmCallback;
        std::shared_ptr<IViewModelCurrencyCallback> m_vmCurrencyCallback;
        std::vector<Category> m_categories;
        CategoryToUnitVectorMap m_categoryToUnits;
        UnitToUnitToConversionDataMap m_ratioMap;
        Category m_currentCategory;
        Unit m_fromType;
        Unit m_toType;
        std::wstring m_currentDisplay;
        std::wstring m_returnDisplay;
        bool m_currentHasDecimal;
        bool m_returnHasDecimal;
        bool m_switchedActive;
    };
}
