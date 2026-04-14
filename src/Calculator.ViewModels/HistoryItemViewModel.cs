// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using CalcManager.Interop;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class HistoryItemViewModel
    {
        private readonly string _expression;
        private readonly string _accExpression;
        private readonly string _result;
        private readonly string _accResult;
        private readonly List<(string Token, int CommandIndex)> _tokens;
        private readonly List<ExpressionCommandWrapper> _commands;

        internal HistoryItemViewModel(
            string expression,
            string result,
            HistoryToken[] tokens,
            ExpressionCommandWrapper[] commands)
        {
            _expression = expression;
            _result = result;
            _tokens = new List<(string Token, int CommandIndex)>();
            if (tokens != null)
            {
                foreach (var t in tokens)
                {
                    _tokens.Add((t.Value, t.CommandIndex));
                }
            }
            _commands = commands != null ? new List<ExpressionCommandWrapper>(commands) : new List<ExpressionCommandWrapper>();

            _accExpression = GetAccessibleExpressionFromTokens(_tokens, expression);
            _accResult = LocalizationSettings.GetNarratorReadableString(result);
        }

        internal HistoryItemViewModel(
            string expression,
            string result,
            List<(string Token, int CommandIndex)> tokens,
            List<ExpressionCommandWrapper> commands)
        {
            _expression = expression;
            _result = result;
            _tokens = tokens;
            _commands = commands;

            _accExpression = GetAccessibleExpressionFromTokens(tokens, expression);
            _accResult = LocalizationSettings.GetNarratorReadableString(result);
        }

        public string Expression => _expression;
        public string AccExpression => _accExpression;
        public string Result => _result;
        public string AccResult => _accResult;

        internal List<(string Token, int CommandIndex)> GetTokens() => _tokens;
        internal List<ExpressionCommandWrapper> GetCommands() => _commands;

        private static string GetAccessibleExpressionFromTokens(
            List<(string Token, int CommandIndex)> tokens,
            string fallbackExpression)
        {
            string accExpression = string.Empty;
            if (tokens != null)
            {
                foreach (var tokenItem in tokens)
                {
                    accExpression += LocalizationSettings.GetNarratorReadableToken(tokenItem.Token);
                }
            }
            return accExpression;
        }
    }
}
