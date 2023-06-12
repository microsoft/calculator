// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "HistoryItemViewModel.h"
#include "Common/LocalizationService.h"

using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel;
using namespace std;
using namespace Platform;

HistoryItemViewModel::HistoryItemViewModel(
    String ^ expression,
    String ^ result,
    _In_ const shared_ptr<vector<pair<wstring, int>>>& spTokens,
    _In_ const shared_ptr<vector<shared_ptr<IExpressionCommand>>>& spCommands)
    : m_expression(expression)
    , m_result(result)
    , m_spTokens(spTokens)
    , m_spCommands(spCommands)
{
    // updating accessibility names for expression and result
    m_accExpression = HistoryItemViewModel::GetAccessibleExpressionFromTokens(spTokens, m_expression);
    m_accResult = LocalizationService::GetNarratorReadableString(m_result);
}

String
    ^ HistoryItemViewModel::GetAccessibleExpressionFromTokens(
        _In_ shared_ptr<vector<pair<wstring, int>>> const& spTokens,
        _In_ String ^ fallbackExpression)
{
    // updating accessibility names for expression and result
    wstring accExpression{};

    for (const auto& tokenItem : *spTokens)
    {
        accExpression += LocalizationService::GetNarratorReadableToken(StringReference(tokenItem.first.c_str()))->Data();
    }

    return ref new String(accExpression.c_str());
}
