// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace UnitConversionManager
{
    enum class Command;

    struct Unit
    {
        Unit(){}
        Unit(int id, std::wstring name, std::wstring abbreviation, bool isConversionSource, bool isConversionTarget, bool isWhimsical)
            : id(id), name(name), accessibleName(name), abbreviation(abbreviation), isConversionSource(isConversionSource), isConversionTarget(isConversionTarget), isWhimsical(isWhimsical)
        {
        }

        Unit(int id, std::wstring currencyName, std::wstring countryName, std::wstring abbreviation, bool isRtlLanguage, bool isConversionSource, bool isConversionTarget)
            : id(id), abbreviation(abbreviation), isConversionSource(isConversionSource), isConversionTarget(isConversionTarget), isWhimsical(false)
        {
            std::wstring nameValue1 = isRtlLanguage ? currencyName : countryName;
            std::wstring nameValue2 = isRtlLanguage ? countryName : currencyName;

            name = nameValue1 + L" - " + nameValue2;
            accessibleName = nameValue1 + L" " + nameValue2;
        }

        virtual ~Unit() {}

        int id;
        std::wstring name;
        std::wstring accessibleName;
        std::wstring abbreviation;
        bool isConversionSource;
        bool isConversionTarget;
        bool isWhimsical;

        bool operator!= (const Unit& that) const
        {
            return that.id != id;
        }

        bool operator== (const Unit& that) const
        {
            return that.id == id;
        }
    };

    // The EMPTY_UNIT acts as a 'null-struct' so that
    // Unit pointers can safely be dereferenced without
    // null checks.
    //
    // unitId, name, abbreviation, isConversionSource, isConversionTarget, isWhimsical
    const Unit EMPTY_UNIT = Unit{ -1, L"", L"", true, true, false };

    struct Category
    {
        Category(){}

        Category(int id, std::wstring name, bool supportsNegative) : id(id), name(name), supportsNegative(supportsNegative)
        {
        }

        int id;
        std::wstring name;
        bool supportsNegative;

        bool operator!= (const Category& that) const
        {
            return that.id != id;
        }

        bool operator== (const Category& that) const
        {
            return that.id == id;
        }
    };

    class UnitHash
    {
    public:
        size_t operator() (const Unit & x) const {
            return x.id;
        }
    };

    class CategoryHash
    {
    public:
        size_t operator() (const Category & x) const {
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
        ConversionData(){}
        ConversionData(double ratio, double offset, bool offsetFirst) : ratio(ratio), offset(offset), offsetFirst(offsetFirst)
        {
        }

        virtual ~ConversionData() {}

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
    typedef std::unordered_map<UnitConversionManager::Unit, std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash>, UnitConversionManager::UnitHash> UnitToUnitToConversionDataMap;
    typedef std::unordered_map<UnitConversionManager::Category, std::vector<UnitConversionManager::Unit>, UnitConversionManager::CategoryHash> CategoryToUnitVectorMap;

    class IViewModelCurrencyCallback
    {
    public:
        virtual ~IViewModelCurrencyCallback() { };
        virtual void CurrencyDataLoadFinished(bool didLoad) = 0;
        virtual void CurrencySymbolsCallback(_In_ const std::wstring& fromSymbol, _In_ const std::wstring& toSymbol) = 0;
        virtual void CurrencyRatiosCallback(_In_ const std::wstring& ratioEquality, _In_ const std::wstring& accRatioEquality) = 0;
        virtual void CurrencyTimestampCallback(_In_ const std::wstring& timestamp, bool isWeekOldData) = 0;
        virtual void NetworkBehaviorChanged(_In_ int newBehavior) = 0;
    };

    class IConverterDataLoader
    {
    public:
        virtual ~IConverterDataLoader() { };
        virtual void LoadData() = 0;    // prepare data if necessary before calling other functions
        virtual std::vector<Category> LoadOrderedCategories() = 0;
        virtual std::vector<Unit> LoadOrderedUnits(const Category& c) = 0;
        virtual std::unordered_map<Unit, ConversionData, UnitHash> LoadOrderedRatios(const Unit& u) = 0;
        virtual bool SupportsCategory(const Category& target) = 0;
    };

    class ICurrencyConverterDataLoader
    {
    public:
        virtual void SetViewModelCallback(const std::shared_ptr<UnitConversionManager::IViewModelCurrencyCallback>& callback) = 0;
        virtual std::pair<std::wstring, std::wstring> GetCurrencySymbols(_In_ const UnitConversionManager::Unit& unit1, _In_ const UnitConversionManager::Unit& unit2) = 0;
        virtual std::pair<std::wstring, std::wstring> GetCurrencyRatioEquality(_In_ const UnitConversionManager::Unit& unit1, _In_ const UnitConversionManager::Unit& unit2) = 0;
        virtual std::wstring GetCurrencyTimestamp() = 0;

        virtual concurrency::task<bool> TryLoadDataFromCacheAsync() = 0;
        virtual concurrency::task<bool> TryLoadDataFromWebAsync() = 0;
        virtual concurrency::task<bool> TryLoadDataFromWebOverrideAsync() = 0;
    };

    class IUnitConverterVMCallback
    {
    public:
        virtual ~IUnitConverterVMCallback() { };
        virtual void DisplayCallback(const std::wstring& from, const std::wstring& to) = 0;
        virtual void SuggestedValueCallback(const std::vector<std::tuple<std::wstring, Unit>>& suggestedValues) = 0;
        virtual void MaxDigitsReached() = 0;
    };

    class IUnitConverter
    {
    public:
        virtual ~IUnitConverter() { }
        virtual void Initialize() = 0;  // Use to initialize first time, use deserialize instead to rehydrate
        virtual std::vector<Category> GetCategories() = 0;
        virtual CategorySelectionInitializer SetCurrentCategory(const Category& input) = 0;
        virtual Category GetCurrentCategory() = 0;
        virtual void SetCurrentUnitTypes(const Unit& fromType, const Unit& toType) = 0;
        virtual void SwitchActive(const std::wstring& newValue) = 0;
        virtual std::wstring Serialize() = 0;
        virtual void DeSerialize(const std::wstring& serializedData) = 0;
        virtual std::wstring SaveUserPreferences() = 0;
        virtual void RestoreUserPreferences(_In_ const std::wstring& userPreferences) = 0;
        virtual void SendCommand(Command command) = 0;
        virtual void SetViewModelCallback(_In_ const std::shared_ptr<IUnitConverterVMCallback>& newCallback) = 0;
        virtual void SetViewModelCurrencyCallback(_In_ const std::shared_ptr<IViewModelCurrencyCallback>& newCallback) = 0;
        virtual concurrency::task<std::pair<bool, std::wstring>> RefreshCurrencyRatios() = 0;
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
        std::wstring Serialize() override;
        void DeSerialize(const std::wstring& serializedData) override;
        std::wstring SaveUserPreferences() override;
        void RestoreUserPreferences(const std::wstring& userPreference) override;
        void SendCommand(Command command) override;
        void SetViewModelCallback(_In_ const std::shared_ptr<IUnitConverterVMCallback>& newCallback) override;
        void SetViewModelCurrencyCallback(_In_ const std::shared_ptr<IViewModelCurrencyCallback>& newCallback) override;
        concurrency::task<std::pair<bool, std::wstring>> RefreshCurrencyRatios() override;
        // IUnitConverter

        static std::vector<std::wstring> StringToVector(const std::wstring& w, const wchar_t * delimiter, bool addRemainder = false);
        static std::wstring Quote(const std::wstring& s);
        static std::wstring Unquote(const std::wstring& s);

    private:
        bool CheckLoad();
        double Convert(double value, ConversionData conversionData);
        std::vector<std::tuple<std::wstring, Unit>> CalculateSuggested();
        void Reset();
        void ClearValues();
        void Calculate();
        void TrimString(std::wstring& input);
        void InitializeSelectedUnits();
        std::wstring RoundSignificant(double num, int numSignificant);
        Category StringToCategory(const std::wstring& w);
        std::wstring CategoryToString(const Category& c, const wchar_t * delimiter);
        std::wstring UnitToString(const Unit& u, const wchar_t * delimiter);
        Unit StringToUnit(const std::wstring& w);
        ConversionData StringToConversionData(const std::wstring& w);
        std::wstring ConversionDataToString(ConversionData d, const wchar_t * delimiter);
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
