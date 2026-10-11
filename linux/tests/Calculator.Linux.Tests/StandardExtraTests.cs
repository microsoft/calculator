// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Extra StandardCalculatorViewModel coverage: domain errors, F-to-E,
// programmer shifts, angle modes, localized decimal display.
using System.Globalization;
using Calculator.Linux.Common;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class StandardExtraTests : IDisposable
{
    private readonly StandardCalculatorViewModel _vm;
    private readonly LinuxLocalization _previous;

    public StandardExtraTests()
    {
        _previous = LinuxLocalization.Current;
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("en-US"));
        _vm = new StandardCalculatorViewModel();
        _vm.SetCalculatorType(ViewMode.Standard);
    }

    public void Dispose()
    {
        _vm.Dispose();
        LinuxLocalization.Current = _previous;
    }

    private void Press(params NumbersAndOperatorsEnum[] buttons)
    {
        foreach (var button in buttons)
        {
            _vm.ButtonPressedCommand.Execute(button);
        }
    }

    [Fact]
    public void SqrtNegative_DomainError()
    {
        Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Negate, NumbersAndOperatorsEnum.Sqrt);
        Assert.True(_vm.IsInError);
    }

    [Fact]
    public void FToE_ShowsExponentForm()
    {
        _vm.SetCalculatorType(ViewMode.Scientific);
        foreach (char ch in "123456789")
        {
            Press(NumbersAndOperatorsEnum.Zero + (ch - '0'));
        }
        _vm.FtoEButtonToggled();
        Assert.Contains("e", _vm.DisplayValue);
    }

    [Fact]
    public void Programmer_ShiftLeft()
    {
        _vm.SetCalculatorType(ViewMode.Programmer);
        _vm.SwitchProgrammerModeBase(NumberBase.DecBase);
        Press(NumbersAndOperatorsEnum.Four, NumbersAndOperatorsEnum.Lsh, NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Equals);
        Assert.Equal("8", _vm.DecimalDisplayValue);
    }

    [Fact]
    public void Radians_SinPiOverTwoIsOne()
    {
        _vm.SetCalculatorType(ViewMode.Scientific);
        _vm.SwitchAngleType(NumbersAndOperatorsEnum.Radians);
        Press(NumbersAndOperatorsEnum.Pi, NumbersAndOperatorsEnum.Divide, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Equals);
        Press(NumbersAndOperatorsEnum.Sin);
        Assert.Equal("1", _vm.DisplayValue);
    }

    [Fact]
    public void Degrees_Sin90IsOne()
    {
        _vm.SetCalculatorType(ViewMode.Scientific);
        _vm.SwitchAngleType(NumbersAndOperatorsEnum.Degree);
        Press(NumbersAndOperatorsEnum.Nine, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Sin);
        Assert.Equal("1", _vm.DisplayValue);
    }

    [Fact]
    public void GermanLocale_DecimalCommaDisplay()
    {
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("de-DE"));
        using var vm = new StandardCalculatorViewModel();
        vm.SetCalculatorType(ViewMode.Standard);
        vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
        vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Decimal);
        vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Three);
        Assert.Equal("0,3", vm.DisplayValue);
    }
}
