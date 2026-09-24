// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "UnitConverterWrapper.g.h"
#include "CategorySelectionResult.h"

namespace winrt::CalcManager::Interop::implementation
{
    // Helper conversions between WinRT structs and native types
    inline UnitConversionManager::Unit ToNativeUnit(CalcManager::Interop::UnitWrapper const& w)
    {
        UnitConversionManager::Unit u;
        u.id = w.Id;
        u.name = std::wstring(w.Name);
        u.accessibleName = std::wstring(w.AccessibleName);
        u.abbreviation = std::wstring(w.Abbreviation);
        u.isConversionSource = w.IsConversionSource;
        u.isConversionTarget = w.IsConversionTarget;
        u.isWhimsical = w.IsWhimsical;
        return u;
    }

    inline CalcManager::Interop::UnitWrapper ToWinRTUnit(UnitConversionManager::Unit const& u)
    {
        CalcManager::Interop::UnitWrapper w;
        w.Id = u.id;
        w.Name = hstring(u.name);
        w.AccessibleName = hstring(u.accessibleName);
        w.Abbreviation = hstring(u.abbreviation);
        w.IsConversionSource = u.isConversionSource;
        w.IsConversionTarget = u.isConversionTarget;
        w.IsWhimsical = u.isWhimsical;
        return w;
    }

    inline UnitConversionManager::Category ToNativeCategory(CalcManager::Interop::CategoryWrapper const& w)
    {
        return UnitConversionManager::Category(w.Id, std::wstring(w.Name), w.SupportsNegative);
    }

    inline CalcManager::Interop::CategoryWrapper ToWinRTCategory(UnitConversionManager::Category const& c)
    {
        CalcManager::Interop::CategoryWrapper w;
        w.Id = c.id;
        w.Name = hstring(c.name);
        w.SupportsNegative = c.supportsNegative;
        return w;
    }

    // Bridge from WinRT ConverterDataLoaderBase to native IConverterDataLoader
    class ConverterDataLoaderBridge : public UnitConversionManager::IConverterDataLoader
    {
    public:
        ConverterDataLoaderBridge(CalcManager::Interop::ConverterDataLoaderBase const& receiver);

        void LoadData() override;
        std::vector<UnitConversionManager::Category> GetOrderedCategories() override;
        std::vector<UnitConversionManager::Unit> GetOrderedUnits(const UnitConversionManager::Category& c) override;
        std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash>
        LoadOrderedRatios(const UnitConversionManager::Unit& u) override;
        bool SupportsCategory(const UnitConversionManager::Category& target) override;

    private:
        CalcManager::Interop::ConverterDataLoaderBase m_receiver;
    };

    // Bridge from native IUnitConverterVMCallback to WinRT UnitConverterVMCallbackBase
    class UnitConverterVMCallbackBridge : public UnitConversionManager::IUnitConverterVMCallback
    {
    public:
        UnitConverterVMCallbackBridge(CalcManager::Interop::UnitConverterVMCallbackBase const& receiver);

        void DisplayCallback(const std::wstring& from, const std::wstring& to) override;
        void SuggestedValueCallback(const std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>>& suggestedValues) override;
        void MaxDigitsReached() override;

    private:
        CalcManager::Interop::UnitConverterVMCallbackBase m_receiver;
    };

    // Bridge from native IViewModelCurrencyCallback to WinRT ViewModelCurrencyCallbackBase
    class ViewModelCurrencyCallbackBridge : public UnitConversionManager::IViewModelCurrencyCallback
    {
    public:
        ViewModelCurrencyCallbackBridge(CalcManager::Interop::ViewModelCurrencyCallbackBase const& receiver);

        void CurrencyDataLoadFinished(bool didLoad) override;
        void CurrencySymbolsCallback(const std::wstring& fromSymbol, const std::wstring& toSymbol) override;
        void CurrencyRatiosCallback(const std::wstring& ratioEquality, const std::wstring& accRatioEquality) override;
        void CurrencyTimestampCallback(const std::wstring& timestamp, bool isWeekOldData) override;
        void NetworkBehaviorChanged(int newBehavior) override;

    private:
        CalcManager::Interop::ViewModelCurrencyCallbackBase m_receiver;
    };

    struct UnitConverterWrapper : UnitConverterWrapperT<UnitConverterWrapper>
    {
        UnitConverterWrapper(CalcManager::Interop::ConverterDataLoaderBase const& dataLoader);

        void Initialize();
        com_array<CalcManager::Interop::CategoryWrapper> GetCategories();
        CalcManager::Interop::CategorySelectionResult SetCurrentCategory(CalcManager::Interop::CategoryWrapper const& category);
        CalcManager::Interop::CategoryWrapper GetCurrentCategory();
        void SetCurrentUnitTypes(CalcManager::Interop::UnitWrapper const& fromType, CalcManager::Interop::UnitWrapper const& toType);
        void SwitchActive(hstring const& newValue);
        bool IsSwitchedActive();
        hstring SaveUserPreferences();
        void RestoreUserPreferences(hstring const& userPreference);
        void SendCommand(CalcManager::Interop::UnitConverterCommand command);
        void SetViewModelCallback(CalcManager::Interop::UnitConverterVMCallbackBase const& callback);
        void SetViewModelCurrencyCallback(CalcManager::Interop::ViewModelCurrencyCallbackBase const& callback);
        void Calculate();
        void ResetCategoriesAndRatios();

    private:
        std::shared_ptr<ConverterDataLoaderBridge> m_dataLoaderBridge;
        std::shared_ptr<UnitConversionManager::UnitConverter> m_converter;
        std::shared_ptr<UnitConverterVMCallbackBridge> m_vmCallbackBridge;
        std::shared_ptr<ViewModelCurrencyCallbackBridge> m_currencyCallbackBridge;
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct UnitConverterWrapper : UnitConverterWrapperT<UnitConverterWrapper, implementation::UnitConverterWrapper>
    {
    };
}
