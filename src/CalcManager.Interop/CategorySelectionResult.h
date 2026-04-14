// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CategorySelectionResult.g.h"

namespace winrt::CalcManager::Interop::implementation
{
    struct CategorySelectionResult : CategorySelectionResultT<CategorySelectionResult>
    {
        CategorySelectionResult() = default;
        CategorySelectionResult(
            std::vector<CalcManager::Interop::UnitWrapper>&& units,
            CalcManager::Interop::UnitWrapper const& fromUnit,
            CalcManager::Interop::UnitWrapper const& toUnit);

        com_array<CalcManager::Interop::UnitWrapper> Units();
        CalcManager::Interop::UnitWrapper FromUnit();
        CalcManager::Interop::UnitWrapper ToUnit();

    private:
        std::vector<CalcManager::Interop::UnitWrapper> m_units;
        CalcManager::Interop::UnitWrapper m_fromUnit{};
        CalcManager::Interop::UnitWrapper m_toUnit{};
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct CategorySelectionResult
        : CategorySelectionResultT<CategorySelectionResult, implementation::CategorySelectionResult>
    {
    };
}
