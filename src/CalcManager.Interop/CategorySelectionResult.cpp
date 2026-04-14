// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CategorySelectionResult.h"
namespace winrt::CalcManager::Interop::implementation
{
    CategorySelectionResult::CategorySelectionResult(
        std::vector<CalcManager::Interop::UnitWrapper>&& units,
        CalcManager::Interop::UnitWrapper const& fromUnit,
        CalcManager::Interop::UnitWrapper const& toUnit)
        : m_units(std::move(units))
        , m_fromUnit(fromUnit)
        , m_toUnit(toUnit)
    {
    }

    com_array<CalcManager::Interop::UnitWrapper> CategorySelectionResult::Units()
    {
        return com_array<CalcManager::Interop::UnitWrapper>(m_units);
    }

    CalcManager::Interop::UnitWrapper CategorySelectionResult::FromUnit()
    {
        return m_fromUnit;
    }

    CalcManager::Interop::UnitWrapper CategorySelectionResult::ToUnit()
    {
        return m_toUnit;
    }
}
