// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux port of HistoryItemViewModel. Tokens carry their native command type
// (see CalcCommandType) and recall replays pre-flattened engine command ids,
// so no WinRT interop wrappers are needed.
using Calculator.Linux.Common;
using Calculator.Linux.Services;

namespace Calculator.Linux.ViewModels;

public sealed class HistoryItemViewModel
{
    private readonly string _expression;
    private readonly string _accExpression;
    private readonly string _result;
    private readonly string _accResult;
    private readonly List<(string Token, int CommandIndex, int CommandType)> _tokens;
    private readonly List<int> _commands;

    internal HistoryItemViewModel(
        string expression,
        string result,
        List<(string Token, int CommandIndex, int CommandType)> tokens,
        List<int> commands)
    {
        _expression = expression;
        _result = result;
        _tokens = tokens ?? new List<(string, int, int)>();
        _commands = commands ?? new List<int>();

        _accExpression = GetAccessibleExpressionFromTokens(_tokens);
        _accResult = NarratorTokens.GetReadableString(result);
    }

    public string Expression => _expression;
    public string AccExpression => _accExpression;
    public string Result => _result;
    public string AccResult => _accResult;

    internal List<(string Token, int CommandIndex, int CommandType)> GetTokens() => _tokens;
    internal List<int> GetCommands() => _commands;

    private static string GetAccessibleExpressionFromTokens(List<(string Token, int CommandIndex, int CommandType)> tokens)
    {
        string accExpression = string.Empty;
        if (tokens != null)
        {
            foreach (var tokenItem in tokens)
            {
                accExpression += NarratorTokens.GetReadableToken(tokenItem.Token);
            }
        }
        return accExpression;
    }
}
