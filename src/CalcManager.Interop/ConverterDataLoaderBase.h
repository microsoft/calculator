#pragma once
#include "ConverterDataLoaderBase.g.h"

namespace winrt::CalcManager::Interop::implementation
{
    struct ConverterDataLoaderBase : ConverterDataLoaderBaseT<ConverterDataLoaderBase>
    {
        ConverterDataLoaderBase() = default;

        void LoadData() {}
        com_array<CalcManager::Interop::CategoryWrapper> GetOrderedCategories() { return {}; }
        com_array<CalcManager::Interop::UnitWrapper> GetOrderedUnits(CalcManager::Interop::CategoryWrapper const&) { return {}; }
        com_array<CalcManager::Interop::UnitConversionEntry> LoadOrderedRatios(CalcManager::Interop::UnitWrapper const&) { return {}; }
        bool SupportsCategory(CalcManager::Interop::CategoryWrapper const&) { return false; }
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct ConverterDataLoaderBase : ConverterDataLoaderBaseT<ConverterDataLoaderBase, implementation::ConverterDataLoaderBase>
    {
    };
}
