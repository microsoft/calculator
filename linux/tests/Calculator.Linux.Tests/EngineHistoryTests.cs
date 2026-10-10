// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 4, slice 2 (RED): history item texts, replay commands, expression
// tokens with command types, and memorized-number content via P/Invoke.
using Calculator.Linux.Interop;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class EngineHistoryTests : IDisposable
{
    private const int Add = 93;
    private const int Equ = 121;
    private const int D2 = 132;

    private readonly CalcEngine _engine = new();

    public void Dispose() => _engine.Dispose();

    [Fact]
    public void HistoryItem_ReturnsExpressionAndResult()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);
        _engine.Send(D2);
        _engine.Send(Equ);

        var item = _engine.HistoryItem(0, 0);
        Assert.NotNull(item);
        Assert.Equal("4", item.Value.Result);
        Assert.Contains("2", item.Value.Expression);
    }

    [Fact]
    public void HistoryItem_BadIndex_ReturnsNull()
    {
        Assert.Null(_engine.HistoryItem(0, 99));
    }

    [Fact]
    public void HistoryItemCommands_ReplayTwoPlusTwo()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);
        _engine.Send(D2);
        _engine.Send(Equ);

        Assert.Equal(new List<int> { D2, Add, D2 }, _engine.HistoryItemCommands(0, 0));
    }

    [Fact]
    public void ExpressionTokens_CarryOperandCommandType()
    {
        var seen = new List<(string Token, int CommandIndex, int CommandType)>();
        _engine.ExpressionChanged += () => seen.AddRange(_engine.ExpressionTokens);

        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);

        // CommandType: OperandCommand == 2, BinaryCommand == 1 (Command.h order).
        // A lone operand produces no expression tokens; they appear with '+'.
        Assert.Contains(seen, t => t.Token == "2" && t.CommandType == 2);
        Assert.Contains(seen, t => t.CommandType == 1);
    }

    [Fact]
    public void MemorizedNumbers_ReturnContent()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);
        _engine.Send(D2);
        _engine.Send(Equ);
        _engine.Memorize();

        Assert.Equal(new List<string> { "4" }, _engine.MemorizedNumbers());
    }

    [Fact]
    public void DisplayCommands_ReflectLiveInput()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);

        List<int> commands = _engine.DisplayCommands();
        Assert.Equal(new List<int> { D2, Add }, commands);
    }

    [Fact]
    public void RadixResult_MatchesDecimalInput()
    {
        _engine.SetProgrammerMode();
        _engine.Send(138); // 8
        Assert.Equal("8", _engine.ResultForRadix(10, 64, false));
        Assert.Equal("8", _engine.ResultForRadix(16, 64, false));
    }
}
