// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "UnitConverterInterop.h"


namespace winrt::CalcManager::Interop::implementation
{
    // ---- ConverterDataLoaderBridge ----

    ConverterDataLoaderBridge::ConverterDataLoaderBridge(CalcManager::Interop::ConverterDataLoaderBase const& receiver)
        : m_receiver(receiver)
    {
    }

    void ConverterDataLoaderBridge::LoadData()
    {
        m_receiver.as<CalcManager::Interop::IConverterDataLoaderBaseOverrides>().LoadData();
    }

    std::vector<UnitConversionManager::Category> ConverterDataLoaderBridge::GetOrderedCategories()
    {
        auto categories = m_receiver.as<CalcManager::Interop::IConverterDataLoaderBaseOverrides>().GetOrderedCategories();
        std::vector<UnitConversionManager::Category> result;
        result.reserve(categories.size());
        for (auto const& cat : categories)
        {
            result.push_back(ToNativeCategory(cat));
        }
        return result;
    }

    std::vector<UnitConversionManager::Unit> ConverterDataLoaderBridge::GetOrderedUnits(const UnitConversionManager::Category& c)
    {
        auto units = m_receiver.as<CalcManager::Interop::IConverterDataLoaderBaseOverrides>().GetOrderedUnits(ToWinRTCategory(c));
        std::vector<UnitConversionManager::Unit> result;
        result.reserve(units.size());
        for (auto const& u : units)
        {
            result.push_back(ToNativeUnit(u));
        }
        return result;
    }

    std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash>
    ConverterDataLoaderBridge::LoadOrderedRatios(const UnitConversionManager::Unit& u)
    {
        auto entries = m_receiver.as<CalcManager::Interop::IConverterDataLoaderBaseOverrides>().LoadOrderedRatios(ToWinRTUnit(u));
        std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash> result;
        for (auto const& entry : entries)
        {
            UnitConversionManager::Unit targetUnit = ToNativeUnit(entry.Unit);
            UnitConversionManager::ConversionData convData(entry.Ratio, entry.Offset, entry.OffsetFirst);
            result[targetUnit] = convData;
        }
        return result;
    }

    bool ConverterDataLoaderBridge::SupportsCategory(const UnitConversionManager::Category& target)
    {
        return m_receiver.as<CalcManager::Interop::IConverterDataLoaderBaseOverrides>().SupportsCategory(ToWinRTCategory(target));
    }

    // ---- UnitConverterVMCallbackBridge ----

    UnitConverterVMCallbackBridge::UnitConverterVMCallbackBridge(
        CalcManager::Interop::UnitConverterVMCallbackBase const& receiver)
        : m_receiver(receiver)
    {
    }

    void UnitConverterVMCallbackBridge::DisplayCallback(const std::wstring& from, const std::wstring& to)
    {
        m_receiver.as<CalcManager::Interop::IUnitConverterVMCallbackBaseOverrides>().DisplayCallback(hstring(from), hstring(to));
    }

    void UnitConverterVMCallbackBridge::SuggestedValueCallback(
        const std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>>& suggestedValues)
    {
        std::vector<CalcManager::Interop::SuggestedValueWrapper> winrtValues;
        winrtValues.reserve(suggestedValues.size());
        for (auto const& [value, unit] : suggestedValues)
        {
            CalcManager::Interop::SuggestedValueWrapper sv;
            sv.Value = hstring(value);
            sv.Unit = ToWinRTUnit(unit);
            winrtValues.push_back(std::move(sv));
        }
        m_receiver.as<CalcManager::Interop::IUnitConverterVMCallbackBaseOverrides>().SuggestedValueCallback(com_array<CalcManager::Interop::SuggestedValueWrapper>(std::move(winrtValues)));
    }

    void UnitConverterVMCallbackBridge::MaxDigitsReached()
    {
        m_receiver.as<CalcManager::Interop::IUnitConverterVMCallbackBaseOverrides>().MaxDigitsReached();
    }

    // ---- ViewModelCurrencyCallbackBridge ----

    ViewModelCurrencyCallbackBridge::ViewModelCurrencyCallbackBridge(
        CalcManager::Interop::ViewModelCurrencyCallbackBase const& receiver)
        : m_receiver(receiver)
    {
    }

    void ViewModelCurrencyCallbackBridge::CurrencyDataLoadFinished(bool didLoad)
    {
        m_receiver.as<CalcManager::Interop::IViewModelCurrencyCallbackBaseOverrides>().CurrencyDataLoadFinished(didLoad);
    }

    void ViewModelCurrencyCallbackBridge::CurrencySymbolsCallback(const std::wstring& fromSymbol, const std::wstring& toSymbol)
    {
        m_receiver.as<CalcManager::Interop::IViewModelCurrencyCallbackBaseOverrides>().CurrencySymbolsCallback(hstring(fromSymbol), hstring(toSymbol));
    }

    void ViewModelCurrencyCallbackBridge::CurrencyRatiosCallback(const std::wstring& ratioEquality, const std::wstring& accRatioEquality)
    {
        m_receiver.as<CalcManager::Interop::IViewModelCurrencyCallbackBaseOverrides>().CurrencyRatiosCallback(hstring(ratioEquality), hstring(accRatioEquality));
    }

    void ViewModelCurrencyCallbackBridge::CurrencyTimestampCallback(const std::wstring& timestamp, bool isWeekOldData)
    {
        m_receiver.as<CalcManager::Interop::IViewModelCurrencyCallbackBaseOverrides>().CurrencyTimestampCallback(hstring(timestamp), isWeekOldData);
    }

    void ViewModelCurrencyCallbackBridge::NetworkBehaviorChanged(int newBehavior)
    {
        m_receiver.as<CalcManager::Interop::IViewModelCurrencyCallbackBaseOverrides>().NetworkBehaviorChanged(newBehavior);
    }

    // ---- UnitConverterWrapper ----

    UnitConverterWrapper::UnitConverterWrapper(CalcManager::Interop::ConverterDataLoaderBase const& dataLoader)
    {
        m_dataLoaderBridge = std::make_shared<ConverterDataLoaderBridge>(dataLoader);
        m_converter = std::make_shared<UnitConversionManager::UnitConverter>(
            std::static_pointer_cast<UnitConversionManager::IConverterDataLoader>(m_dataLoaderBridge));
    }

    void UnitConverterWrapper::Initialize()
    {
        m_converter->Initialize();
    }

    com_array<CalcManager::Interop::CategoryWrapper> UnitConverterWrapper::GetCategories()
    {
        auto categories = m_converter->GetCategories();
        std::vector<CalcManager::Interop::CategoryWrapper> result;
        result.reserve(categories.size());
        for (auto const& cat : categories)
        {
            result.push_back(ToWinRTCategory(cat));
        }
        return com_array<CalcManager::Interop::CategoryWrapper>(std::move(result));
    }

    CalcManager::Interop::CategorySelectionResult UnitConverterWrapper::SetCurrentCategory(
        CalcManager::Interop::CategoryWrapper const& category)
    {
        auto [units, fromUnit, toUnit] = m_converter->SetCurrentCategory(ToNativeCategory(category));

        std::vector<CalcManager::Interop::UnitWrapper> winrtUnits;
        winrtUnits.reserve(units.size());
        for (auto const& u : units)
        {
            winrtUnits.push_back(ToWinRTUnit(u));
        }

        return winrt::make<CategorySelectionResult>(
            std::move(winrtUnits), ToWinRTUnit(fromUnit), ToWinRTUnit(toUnit));
    }

    CalcManager::Interop::CategoryWrapper UnitConverterWrapper::GetCurrentCategory()
    {
        return ToWinRTCategory(m_converter->GetCurrentCategory());
    }

    void UnitConverterWrapper::SetCurrentUnitTypes(
        CalcManager::Interop::UnitWrapper const& fromType,
        CalcManager::Interop::UnitWrapper const& toType)
    {
        m_converter->SetCurrentUnitTypes(ToNativeUnit(fromType), ToNativeUnit(toType));
    }

    void UnitConverterWrapper::SwitchActive(hstring const& newValue)
    {
        m_converter->SwitchActive(std::wstring(newValue));
    }

    bool UnitConverterWrapper::IsSwitchedActive()
    {
        return m_converter->IsSwitchedActive();
    }

    hstring UnitConverterWrapper::SaveUserPreferences()
    {
        return hstring(m_converter->SaveUserPreferences());
    }

    void UnitConverterWrapper::RestoreUserPreferences(hstring const& userPreference)
    {
        m_converter->RestoreUserPreferences(std::wstring_view(userPreference));
    }

    void UnitConverterWrapper::SendCommand(CalcManager::Interop::UnitConverterCommand command)
    {
        m_converter->SendCommand(static_cast<UnitConversionManager::Command>(static_cast<int>(command)));
    }

    void UnitConverterWrapper::SetViewModelCallback(CalcManager::Interop::UnitConverterVMCallbackBase const& callback)
    {
        m_vmCallbackBridge = std::make_shared<UnitConverterVMCallbackBridge>(callback);
        m_converter->SetViewModelCallback(
            std::static_pointer_cast<UnitConversionManager::IUnitConverterVMCallback>(m_vmCallbackBridge));
    }

    void UnitConverterWrapper::SetViewModelCurrencyCallback(CalcManager::Interop::ViewModelCurrencyCallbackBase const& callback)
    {
        m_currencyCallbackBridge = std::make_shared<ViewModelCurrencyCallbackBridge>(callback);
        m_converter->SetViewModelCurrencyCallback(
            std::static_pointer_cast<UnitConversionManager::IViewModelCurrencyCallback>(m_currencyCallbackBridge));
    }

    void UnitConverterWrapper::Calculate()
    {
        m_converter->Calculate();
    }

    void UnitConverterWrapper::ResetCategoriesAndRatios()
    {
        m_converter->ResetCategoriesAndRatios();
    }
}
