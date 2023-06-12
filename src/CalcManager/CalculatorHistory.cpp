// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <cassert>
#include "CalculatorHistory.h"

using namespace std;
using namespace CalculationManager;

namespace
{
    static wstring GetGeneratedExpression(const vector<pair<wstring, int>>& tokens)
    {
        wstring expression;
        bool isFirst = true;

        for (auto const& token : tokens)
        {
            if (isFirst)
            {
                isFirst = false;
            }
            else
            {
                expression += L' ';
            }
            expression.append(token.first);
        }

        return expression;
    }
}

CalculatorHistory::CalculatorHistory(size_t maxSize)
    : m_maxHistorySize(maxSize)
{
}

unsigned int CalculatorHistory::AddToHistory(
    _In_ shared_ptr<vector<pair<wstring, int>>> const& tokens,
    _In_ shared_ptr<vector<shared_ptr<IExpressionCommand>>> const& commands,
    wstring_view result)
{
    shared_ptr<HISTORYITEM> spHistoryItem = make_shared<HISTORYITEM>();

    spHistoryItem->historyItemVector.spTokens = tokens;
    spHistoryItem->historyItemVector.spCommands = commands;
    spHistoryItem->historyItemVector.expression = GetGeneratedExpression(*tokens);
    spHistoryItem->historyItemVector.result = wstring(result);
    return AddItem(spHistoryItem);
}

unsigned int CalculatorHistory::AddItem(_In_ shared_ptr<HISTORYITEM> const& spHistoryItem)
{
    if (m_historyItems.size() >= m_maxHistorySize)
    {
        m_historyItems.erase(m_historyItems.begin());
    }

    m_historyItems.push_back(spHistoryItem);
    return static_cast<unsigned>(m_historyItems.size() - 1);
}

bool CalculatorHistory::RemoveItem(unsigned int uIdx)
{
    if (uIdx < m_historyItems.size())
    {
        m_historyItems.erase(m_historyItems.begin() + uIdx);
        return true;
    }

    return false;
}

vector<shared_ptr<HISTORYITEM>> const& CalculatorHistory::GetHistory()
{
    return m_historyItems;
}

shared_ptr<HISTORYITEM> const& CalculatorHistory::GetHistoryItem(unsigned int uIdx)
{
    assert(uIdx < m_historyItems.size());
    return m_historyItems.at(uIdx);
}

void CalculatorHistory::ClearHistory()
{
    m_historyItems.clear();
}
