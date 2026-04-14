// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "HistoryItemWrapper.h"
#include "HistoryToken.h"

namespace winrt::CalcManager::Interop::implementation
{
    HistoryItemWrapper::HistoryItemWrapper(const std::shared_ptr<CalculationManager::HISTORYITEM>& item)
    {
        if (!item)
        {
            return;
        }

        auto const& histVec = item->historyItemVector;

        // Convert tokens
        if (histVec.spTokens)
        {
            for (auto const& [value, cmdIndex] : *histVec.spTokens)
            {
                auto token = winrt::make<implementation::HistoryToken>();
                token.Value(hstring(value));
                token.CommandIndex(cmdIndex);
                m_tokens.push_back(token);
            }
        }

        // Convert expression commands
        if (histVec.spCommands)
        {
            for (auto const& cmd : *histVec.spCommands)
            {
                auto wrapper = winrt::make<ExpressionCommandWrapper>(cmd);
                m_commands.push_back(wrapper);
            }
        }

        m_expression = hstring(histVec.expression);
        m_result = hstring(histVec.result);
    }

    com_array<CalcManager::Interop::HistoryToken> HistoryItemWrapper::Tokens()
    {
        return com_array<CalcManager::Interop::HistoryToken>(m_tokens);
    }

    com_array<CalcManager::Interop::ExpressionCommandWrapper> HistoryItemWrapper::Commands()
    {
        return com_array<CalcManager::Interop::ExpressionCommandWrapper>(m_commands);
    }

    hstring HistoryItemWrapper::Expression()
    {
        return m_expression;
    }

    hstring HistoryItemWrapper::Result()
    {
        return m_result;
    }
}
