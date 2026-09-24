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

    HistoryItemWrapper::HistoryItemWrapper(
        array_view<CalcManager::Interop::HistoryToken const> tokens,
        array_view<CalcManager::Interop::ExpressionCommandWrapper const> commands,
        hstring const& expression,
        hstring const& result)
        : m_tokens(tokens.begin(), tokens.end())
        , m_commands(commands.begin(), commands.end())
        , m_expression(expression)
        , m_result(result)
    {
    }

    std::shared_ptr<CalculationManager::HISTORYITEM> HistoryItemWrapper::ToUnderlying() const
    {
        CalculationManager::HISTORYITEMVECTOR nativeItem;

        nativeItem.spTokens = std::make_shared<std::vector<std::pair<std::wstring, int>>>();
        for (auto const& token : m_tokens)
        {
            nativeItem.spTokens->push_back(std::make_pair(std::wstring(token.Value()), token.CommandIndex()));
        }

        auto nativeCommands = std::make_shared<std::vector<std::shared_ptr<IExpressionCommand>>>();
        for (auto const& command : m_commands)
        {
            nativeCommands->push_back(get_self<ExpressionCommandWrapper>(command)->ToUnderlying());
        }
        nativeItem.spCommands = std::move(nativeCommands);

        nativeItem.expression = std::wstring(m_expression);
        nativeItem.result = std::wstring(m_result);

        return std::make_shared<CalculationManager::HISTORYITEM>(
            CalculationManager::HISTORYITEM{ std::move(nativeItem) });
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
