// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// C++ bridge: implements the native IConverterDataLoader /
// IUnitConverterVMCallback interfaces on top of C ABI function pointers,
// so unit data and display sinks can live in C# (single source of truth:
// the ported UnitConverterDataLoader + UnitConverterViewModel).
#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "UnitConverter.h"
#include "calc_manager_c_api.h"

namespace calc_linux
{
    class BridgeDataLoader : public UnitConversionManager::IConverterDataLoader
    {
    public:
        BridgeDataLoader(const UnitConverterCbs& callbacks);
        void LoadData() override;
        std::vector<UnitConversionManager::Category> GetOrderedCategories() override;
        std::vector<UnitConversionManager::Unit> GetOrderedUnits(const UnitConversionManager::Category& category) override;
        std::unordered_map<UnitConversionManager::Unit, UnitConversionManager::ConversionData, UnitConversionManager::UnitHash>
        LoadOrderedRatios(const UnitConversionManager::Unit& unit) override;
        bool SupportsCategory(const UnitConversionManager::Category& target) override;

    private:
        UnitConverterCbs m_callbacks;
    };

    class BridgeVMCallback : public UnitConversionManager::IUnitConverterVMCallback
    {
    public:
        explicit BridgeVMCallback(const UnitConverterCbs& callbacks);
        void DisplayCallback(const std::wstring& from, const std::wstring& to) override;
        void SuggestedValueCallback(const std::vector<std::tuple<std::wstring, UnitConversionManager::Unit>>& suggestedValues) override;
        void MaxDigitsReached() override;

    private:
        UnitConverterCbs m_callbacks;
    };

    struct UnitConverterInstance
    {
        UnitConverterCbs callbacks;
        std::shared_ptr<BridgeDataLoader> dataLoader;
        std::shared_ptr<BridgeVMCallback> vmCallback;
        std::unique_ptr<UnitConversionManager::UnitConverter> converter;
        // Scratch string storage for out-param structs below. Pointers handed
        // to C# alias this storage and stay valid until the next call on this
        // instance (all calls originate on the UI thread).
        std::vector<std::string> scratch;

        explicit UnitConverterInstance(UnitConverterCbs callbacks);
    };

    UnitConversionManager::Category ToNativeCategory(const UCCategory& category);
    UnitConversionManager::Unit ToNativeUnit(const UCUnit& unit);
    UCCategory FromNativeCategory(const UnitConversionManager::Category& category);
    UCUnit FromNativeUnit(const UnitConversionManager::Unit& unit);

} // namespace calc_linux
