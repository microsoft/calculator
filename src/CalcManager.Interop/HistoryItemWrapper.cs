// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using CalcManager;
using CalcManager.CalculationManager;

namespace CalcManager.Interop
{
    public sealed class HistoryItemWrapper
    {
        public HistoryToken[] Tokens { get; }
        public ExpressionCommandWrapper[] Commands { get; }
        public string Expression { get; }
        public string Result { get; }

        public HistoryItemWrapper()
        {
            Tokens = Array.Empty<HistoryToken>();
            Commands = Array.Empty<ExpressionCommandWrapper>();
            Expression = string.Empty;
            Result = string.Empty;
        }

        public HistoryItemWrapper(
            HistoryToken[] tokens,
            ExpressionCommandWrapper[] commands,
            string expression,
            string result)
        {
            Tokens = tokens ?? Array.Empty<HistoryToken>();
            Commands = commands ?? Array.Empty<ExpressionCommandWrapper>();
            Expression = expression ?? string.Empty;
            Result = result ?? string.Empty;
        }

        public HistoryItemWrapper(HistoryItem item)
        {
            if (item == null || item.HistoryItemVector == null)
            {
                Tokens = Array.Empty<HistoryToken>();
                Commands = Array.Empty<ExpressionCommandWrapper>();
                Expression = string.Empty;
                Result = string.Empty;
                return;
            }

            var histVec = item.HistoryItemVector;

            if (histVec.SpTokens != null)
            {
                var tokenList = new List<HistoryToken>();
                foreach (var pair in histVec.SpTokens)
                {
                    tokenList.Add(new HistoryToken(pair.Item1, pair.Item2));
                }
                Tokens = tokenList.ToArray();
            }
            else
            {
                Tokens = Array.Empty<HistoryToken>();
            }

            if (histVec.SpCommands != null)
            {
                var commandList = new List<ExpressionCommandWrapper>();
                foreach (var cmd in histVec.SpCommands)
                {
                    commandList.Add(new ExpressionCommandWrapper(cmd));
                }
                Commands = commandList.ToArray();
            }
            else
            {
                Commands = Array.Empty<ExpressionCommandWrapper>();
            }

            Expression = histVec.Expression ?? string.Empty;
            Result = histVec.Result ?? string.Empty;
        }

        public HistoryItem ToUnderlying()
        {
            var nativeItem = new HistoryItemVector();

            nativeItem.SpTokens = new List<Tuple<string, int>>();
            if (Tokens != null)
            {
                foreach (var token in Tokens)
                {
                    nativeItem.SpTokens.Add(Tuple.Create(token.Value, token.CommandIndex));
                }
            }

            nativeItem.SpCommands = new List<IExpressionCommand>();
            if (Commands != null)
            {
                foreach (var command in Commands)
                {
                    nativeItem.SpCommands.Add(command.ToUnderlying());
                }
            }

            nativeItem.Expression = Expression;
            nativeItem.Result = Result;

            return new HistoryItem { HistoryItemVector = nativeItem };
        }
    }
}
