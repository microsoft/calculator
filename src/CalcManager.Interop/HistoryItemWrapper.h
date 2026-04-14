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

        com_array<CalcManager::Interop::HistoryToken> Tokens();
        com_array<CalcManager::Interop::ExpressionCommandWrapper> Commands();
        hstring Expression();
        hstring Result();

    private:
        std::vector<CalcManager::Interop::HistoryToken> m_tokens;
        std::vector<CalcManager::Interop::ExpressionCommandWrapper> m_commands;
        hstring m_expression;
        hstring m_result;
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct HistoryItemWrapper : HistoryItemWrapperT<HistoryItemWrapper, implementation::HistoryItemWrapper>
    {
    };
}
