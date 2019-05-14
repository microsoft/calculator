// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculationManager;
using CalculatorApp.Common;
using System;
using System.Text;
using Windows.UI.Xaml.Data;

namespace CalculatorApp
{
    namespace ViewModel
    {
        [Windows.UI.Xaml.Data.Bindable]
        public sealed class HistoryItemViewModel : ICustomPropertyProvider
        {
            public CalculatorList<(string, int)> GetTokens()
            {
                return m_spTokens;
            }

            public CalculatorList<IExpressionCommand> GetCommands()
            {
                return m_spCommands;
            }

            public string Expression => m_expression;

            public string AccExpression => m_accExpression;

            public string Result => m_result;

            public string AccResult => m_accResult;

            public ICustomProperty GetCustomProperty(string name) { return null; }

            public ICustomProperty GetIndexedProperty(string name, Type type) { return null; }

            public Type Type => this.GetType();

            public string GetStringRepresentation() { return m_accExpression + " " + m_accResult; }

            private string m_expression;
            private string m_accExpression;
            private string m_accResult;
            private string m_result;
            private CalculatorList<(string, int)> m_spTokens;
            private CalculatorList<IExpressionCommand> m_spCommands;


            public HistoryItemViewModel(
                String expression,
                String result,
                 CalculatorList<(string, int)> spTokens,
                 CalculatorList<IExpressionCommand> spCommands)
            {
                m_expression = expression;
                m_result = result;
                m_spTokens = spTokens;
                m_spCommands = spCommands;
                // updating accessibility names for expression and result
                m_accExpression = GetAccessibleExpressionFromTokens(spTokens, m_expression);
                m_accResult = LocalizationService.GetNarratorReadableString(m_result);
            }

            string GetAccessibleExpressionFromTokens(
                     CalculatorList<(string, int)> spTokens,
                     String fallbackExpression)
            {
                // updating accessibility names for expression and result
                StringBuilder accExpression = new StringBuilder();

                int nTokens;
                var hr = spTokens.GetSize(out nTokens);
                if (hr)
                {
                    (string, int) tokenItem;
                    for (int i = 0; i < nTokens; i++)
                    {
                        hr = spTokens.GetAt(i, out tokenItem);
                        if (!hr)
                        {
                            break;
                        }

                        string token = tokenItem.Item1;
                        accExpression.Append(LocalizationService.GetNarratorReadableToken(token));
                    }
                }

                if (hr)
                {
                    string expressionSuffix = "";
                    hr = spTokens.GetExpressionSuffix(out expressionSuffix);
                    if (hr)
                    {
                        accExpression.Append(expressionSuffix);
                    }
                }

                if (!hr)
                {
                    return LocalizationService.GetNarratorReadableString(fallbackExpression);
                }
                else
                {
                    return accExpression.ToString();
                }
            }
        };
    }
}
