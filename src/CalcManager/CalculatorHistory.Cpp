// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculatorHistory.h"

using namespace std;
using namespace CalculationManager;

CalculatorHistory::CalculatorHistory(CALCULATOR_MODE eMode, size_t maxSize) :
    m_mode(eMode),
    m_maxHistorySize(maxSize)
{}

unsigned int CalculatorHistory::AddToHistory(_In_ shared_ptr<CalculatorVector <pair<wstring, int>>> const &tokens, _In_ shared_ptr<CalculatorVector<shared_ptr<IExpressionCommand>>> const &commands, _In_ wstring_view result)
{
    unsigned int addedIndex;
    wstring generatedExpression;
    shared_ptr<HISTORYITEM> spHistoryItem = make_shared<HISTORYITEM>();

    spHistoryItem->historyItemVector.spTokens = tokens;
    spHistoryItem->historyItemVector.spCommands = commands;

    // to be changed when pszexp is back
    tokens->GetString(&generatedExpression);
    // Prefixing and suffixing the special Unicode markers to ensure that the expression 
    // in the history doesn't get broken for RTL languages
    spHistoryItem->historyItemVector.expression = L'\u202d' + generatedExpression + L'\u202c';
    spHistoryItem->historyItemVector.result = wstring(result);
    addedIndex = AddItem(spHistoryItem);

    return addedIndex;
}


unsigned int CalculatorHistory::AddItem(_In_ shared_ptr<HISTORYITEM> const &spHistoryItem)
{
    int lastIndex;

    if (m_historyItems.size() >= m_maxHistorySize)
    {
        m_historyItems.erase(m_historyItems.begin());
    }

    m_historyItems.push_back(spHistoryItem);
    lastIndex = static_cast<unsigned>(m_historyItems.size() - 1);
    return lastIndex;
}

bool CalculatorHistory::RemoveItem(_In_ unsigned int uIdx)
{
    if (uIdx > m_historyItems.size() - 1)
    {
        return false;
    }

    m_historyItems.erase(m_historyItems.begin() + uIdx);
    return true;
}

vector<shared_ptr<HISTORYITEM>> const& CalculatorHistory::GetHistory()
{
    return m_historyItems;
}

shared_ptr<HISTORYITEM> const& CalculatorHistory::GetHistoryItem(_In_ unsigned int uIdx)
{
    assert(uIdx >= 0 && uIdx < m_historyItems.size());
    return m_historyItems.at(uIdx);
}

CalculatorHistory::~CalculatorHistory(void)
{
    ClearHistory();
}

void CalculatorHistory::ClearHistory()
{
    m_historyItems.clear();
}
