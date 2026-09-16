// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "HistoryItemWrapper.g.h"
#include "ExpressionCommandWrapper.h"

namespace winrt::CalcManager::Interop::implementation
{
    struct HistoryItemWrapper : HistoryItemWrapperT<HistoryItemWrapper>
    {
        HistoryItemWrapper() = default;
        HistoryItemWrapper(const std::shared_ptr<CalculationManager::HISTORYITEM>& item);
        HistoryItemWrapper(
            array_view<CalcManager::Interop::HistoryToken const> tokens,
            array_view<CalcManager::Interop::ExpressionCommandWrapper const> commands,
            hstring const& expression,
            hstring const& result);

        com_array<CalcManager::Interop::HistoryToken> Tokens();
        com_array<CalcManager::Interop::ExpressionCommandWrapper> Commands();
        hstring Expression();
        hstring Result();
        hstring ScientificState() const
        {
            return m_scientificState;
        }
        void ScientificState(hstring const& value)
        {
            m_scientificState = value;
        }

        std::shared_ptr<CalculationManager::HISTORYITEM> ToUnderlying(bool includeCommands = true) const;

    private:
        std::vector<CalcManager::Interop::HistoryToken> m_tokens;
        std::vector<CalcManager::Interop::ExpressionCommandWrapper> m_commands;
        hstring m_expression;
        hstring m_result;
        hstring m_scientificState;
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct HistoryItemWrapper : HistoryItemWrapperT<HistoryItemWrapper, implementation::HistoryItemWrapper>
    {
    };
}
