// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 4, slice 3c: headless parity tests for the ported
// StandardCalculatorViewModel (all three modes, memory, history, errors,
// paste) against the real engine. No UI required.
using System.Globalization;
using Calculator.Linux.Common;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class StandardViewModelTests : IDisposable
{
    private readonly StandardCalculatorViewModel _vm;

    public StandardViewModelTests()
    {
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("en-US"));
        _vm = new StandardCalculatorViewModel();
        _vm.SetCalculatorType(ViewMode.Standard);
    }

    public void Dispose() => _vm.Dispose();

    private void Press(params NumbersAndOperatorsEnum[] buttons)
    {
        foreach (var button in buttons)
        {
            _vm.ButtonPressedCommand.Execute(button);
        }
    }

    [Fact]
    public void Standard_TwoPlusTwoEqualsFour()
    {
        Press(NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Add, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Equals);
        Assert.Equal("4", _vm.DisplayValue);
        Assert.Equal(1, _vm.HistoryVM.ItemsCount);
        Assert.Equal("4", _vm.HistoryVM.Items[0].Result);
    }

    [Fact]
    public void Standard_ClearResetsDisplay()
    {
        Press(NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Clear);
        Assert.Equal("0", _vm.DisplayValue);
    }

    [Fact]
    public void Standard_DivideByZeroErrorsAndDigitRecovers()
    {
        Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Divide, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Equals);
        Assert.True(_vm.IsInError);
        Press(NumbersAndOperatorsEnum.Two);
        Assert.False(_vm.IsInError);
        Assert.Equal("2", _vm.DisplayValue);
    }

    [Fact]
    public void Scientific_Sin30IsOneHalf()
    {
        _vm.SetCalculatorType(ViewMode.Scientific);
        Press(NumbersAndOperatorsEnum.Three, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Sin);
        Assert.Equal("0.5", _vm.DisplayValue);
    }

    [Fact]
    public void Scientific_ParenthesesRespected()
    {
        _vm.SetCalculatorType(ViewMode.Scientific);
        Press(
            NumbersAndOperatorsEnum.OpenParenthesis,
            NumbersAndOperatorsEnum.Two,
            NumbersAndOperatorsEnum.Add,
            NumbersAndOperatorsEnum.Three,
            NumbersAndOperatorsEnum.CloseParenthesis,
            NumbersAndOperatorsEnum.Multiply,
            NumbersAndOperatorsEnum.Four,
            NumbersAndOperatorsEnum.Equals);
        Assert.Equal("20", _vm.DisplayValue);
        Assert.Equal(0u, _vm.OpenParenthesisCount);
    }

    [Fact]
    public void Programmer_HexAdditionShowsAllRadixes()
    {
        _vm.SetCalculatorType(ViewMode.Programmer);
        _vm.SwitchProgrammerModeBase(NumberBase.HexBase);
        Press(NumbersAndOperatorsEnum.A, NumbersAndOperatorsEnum.Add, NumbersAndOperatorsEnum.B, NumbersAndOperatorsEnum.Equals);
        Assert.Equal("21", _vm.DecimalDisplayValue);
        Assert.Equal("15", _vm.HexDisplayValue);
    }

    [Fact]
    public void Memory_StoreRecallAddClear()
    {
        Press(NumbersAndOperatorsEnum.Five, NumbersAndOperatorsEnum.Equals);
        _vm.OnMemoryButtonPressed();
        Assert.Single(_vm.MemorizedNumbers);

        Press(NumbersAndOperatorsEnum.Clear);
        _vm.OnMemoryItemPressed(0);
        Assert.Equal("5", _vm.DisplayValue);

        _vm.OnMemoryAdd(0);
        _vm.OnMemorySubtract(0);
        _vm.OnMemoryClear(0);
        Assert.Empty(_vm.MemorizedNumbers);
        Assert.True(_vm.IsMemoryEmpty);
    }

    [Fact]
    public void History_RecallRestoresResult()
    {
        Press(NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Add, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Equals);
        Assert.Equal("4", _vm.DisplayValue);

        Press(NumbersAndOperatorsEnum.Clear);
        Assert.Equal("0", _vm.DisplayValue);

        _vm.SelectHistoryItem(_vm.HistoryVM.Items[0]);
        Assert.Equal("4", _vm.DisplayValue);
    }

    [Fact]
    public void History_ClearEmptiesList()
    {
        Press(NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Add, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Equals);
        Assert.Equal(1, _vm.HistoryVM.ItemsCount);
        _vm.HistoryVM.ClearCommand.Execute(null);
        Assert.Equal(0, _vm.HistoryVM.ItemsCount);
    }

    [Fact]
    public void Paste_ExpressionEvaluates()
    {
        _vm.OnPaste("12+34=");
        Assert.Equal("46", _vm.DisplayValue);
    }

    [Fact]
    public void Paste_InvalidShowsError()
    {
        _vm.OnPaste(CopyPasteManager.ValidatePasteExpression("abc!!!", ViewMode.Standard, NumberBase.DecBase, BitLength.BitLengthQWord));
        Assert.True(_vm.IsInError);
    }

    [Fact]
    public void ExpressionTokens_AppearWithOperator()
    {
        Press(NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Add);
        Assert.NotEmpty(_vm.ExpressionTokens);
    }
}
