// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 2 (RED): interop tests against the REAL libCalcManager.so (no mocks).
// Vectors mirror linux/src/CalcManager.Linux/tests/smoke_test.cpp so C++ and
// C# prove the same engine behavior through the same frozen C ABI.
using Calculator.Linux.Interop;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class EngineInteropTests : IDisposable
{
    // Command ids from src/CalcManager/Command.h.
    private const int Clear = 81;
    private const int Div = 91;
    private const int Add = 93;
    private const int Sin = 102;
    private const int Equ = 121;
    private const int D0 = 130;
    private const int D1 = 131;
    private const int D2 = 132;
    private const int D3 = 133;

    private readonly CalcEngine _engine = new();

    public void Dispose() => _engine.Dispose();

    [Fact]
    public void TwoPlusTwoEqualsFour()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);
        _engine.Send(D2);
        _engine.Send(Equ);

        Assert.Equal("4", _engine.PrimaryDisplay);
        Assert.Equal("4", _engine.PullPrimaryDisplay());
    }

    [Fact]
    public void HistoryRecordedAfterEquals()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);
        _engine.Send(D2);
        _engine.Send(Equ);

        Assert.True(_engine.HistoryAddedCount >= 1);
        Assert.True(_engine.HistoryCount >= 1);
    }

    [Fact]
    public void ClearResetsDisplayToZero()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Clear);

        Assert.Equal("0", _engine.PullPrimaryDisplay());
        Assert.False(_engine.IsInError);
    }

    [Fact]
    public void DivideByZeroEntersErrorStateAndClearRecovers()
    {
        _engine.SetStandardMode();
        _engine.Send(D1);
        _engine.Send(Div);
        _engine.Send(D0);
        _engine.Send(Equ);

        Assert.True(_engine.IsInError);

        _engine.Send(Clear);
        Assert.False(_engine.IsInError);
    }

    [Fact]
    public void Sin30DegreesIsOneHalf()
    {
        _engine.SetScientificMode();
        _engine.Send(D3);
        _engine.Send(D0);
        _engine.Send(Sin);

        Assert.Equal("0.5", _engine.PullPrimaryDisplay());
    }

    [Fact]
    public void MemoryMemorizeAndLoadRoundTrip()
    {
        _engine.SetStandardMode();
        _engine.Send(D2);
        _engine.Send(Add);
        _engine.Send(D2);
        _engine.Send(Equ);
        _engine.Memorize();

        _engine.Send(Clear);
        Assert.Equal("0", _engine.PullPrimaryDisplay());

        _engine.MemoryLoad(0);
        Assert.Equal("4", _engine.PullPrimaryDisplay());
    }
}
