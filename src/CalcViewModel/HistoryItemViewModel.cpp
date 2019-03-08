// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "HistoryItemViewModel.h"
#include "Common/LocalizationService.h"

using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace std;
using namespace Platform;


HistoryItemViewModel::HistoryItemViewModel(String^ expression, String^ result,
    _In_ const shared_ptr<CalculatorVector <pair<wstring, int>>> &spTokens,
    _In_ const shared_ptr<CalculatorVector<shared_ptr<IExpressionCommand>>> &spCommands) :m_expression(expression), m_result(result), m_spTokens(spTokens), m_spCommands(spCommands)
{
    // updating accessibility names for expression and result
    m_accExpression = HistoryItemViewModel::GetAccessibleExpressionFromTokens(spTokens, m_expression);
    m_accResult = LocalizationService::GetNarratorReadableString(m_result);
}

String^ HistoryItemViewModel::GetAccessibleExpressionFromTokens(_In_ shared_ptr< CalculatorVector< pair< wstring, int > > > const &spTokens, _In_ String^ fallbackExpression)
{
    // updating accessibility names for expression and result
    wstringstream accExpression{};
    accExpression << L"";

    unsigned int nTokens;
    HRESULT hr = spTokens->GetSize(&nTokens);
    if (SUCCEEDED(hr))
    {
        pair<wstring, int> tokenItem;
        for (unsigned int i = 0; i < nTokens; i++)
        {
            hr = spTokens->GetAt(i, &tokenItem);
            if (FAILED(hr))
            {
                break;
            }

            wstring token = tokenItem.first;
            accExpression << LocalizationService::GetNarratorReadableToken(StringReference(token.c_str()))->Data();
        }
    }

    if (SUCCEEDED(hr))
    {
        wstring expressionSuffix{};
        hr = spTokens->GetExpressionSuffix(&expressionSuffix);
        if (SUCCEEDED(hr))
        {
            accExpression << expressionSuffix;
        }
    }

    if (FAILED(hr))
    {
        return LocalizationService::GetNarratorReadableString(fallbackExpression);
    }
    else
    {
        return ref new String(accExpression.str().c_str());
    }
}
