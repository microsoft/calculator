// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 6, slice 4: UnitConverterViewModel behavior through the real native
// engine (categories, conversions incl. offsets, supplementary, switching,
// input editing, persistence).
using System.Globalization;
using Calculator.Linux.Common;
using Calculator.Linux.DataLoaders;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class UnitConverterViewModelTests : IDisposable
{
    private readonly string _settingsDir = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
    private readonly UnitConverterViewModel _vm;

    public UnitConverterViewModelTests()
    {
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("en-US"));
        FrankfurterCurrencyClient.ForceWebFailure = true; // hermetic: no live FX in converter tests
        _vm = new UnitConverterViewModel(new LinuxSettings(_settingsDir));
    }

    public void Dispose()
    {
        FrankfurterCurrencyClient.ForceWebFailure = false;
        try
        {
            Directory.Delete(_settingsDir, recursive: true);
        }
        catch
        {
        }
    }

    private void ToCategory(string name)
    {
        var category = _vm.Categories.First(c => c.Name.Contains(name));
        _vm.CurrentCategory = category;
    }

    private Unit FindUnit(string abbreviation) => _vm.Units.First(u => u.Abbreviation == abbreviation);

    private void Press(params NumbersAndOperatorsEnum[] buttons)
    {
        foreach (var button in buttons)
        {
            _vm.ButtonPressedCommand.Execute(button);
        }
    }

    [Fact]
    public void Categories_ListsTwelvePlusCurrencyPlaceholder()
    {
        Assert.Equal(13, _vm.Categories.Count);
        Assert.Contains(_vm.Categories, c => c.Name.Contains("Currency"));
        ToCategory("Length");
        Assert.NotEmpty(_vm.Units);
    }

    [Fact]
    public void Length_MetersToFeetThroughViewModel()
    {
        ToCategory("Length");
        _vm.Unit1 = FindUnit("m");
        _vm.Unit2 = FindUnit("ft");
        Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Zero);
        Assert.Equal("100", _vm.Value1);
        Assert.StartsWith("328.084", _vm.Value2);
    }

    [Fact]
    public void Temperature_OffsetConversion()
    {
        ToCategory("Temperature");
        _vm.Unit1 = _vm.Units.First(u => u.Abbreviation.Contains("C"));
        _vm.Unit2 = _vm.Units.First(u => u.Abbreviation.Contains("F"));
        Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Zero);
        Assert.Equal("212", _vm.Value2);
    }

    [Fact]
    public void Supplementary_ListsOtherUnits()
    {
        ToCategory("Length");
        _vm.Unit1 = FindUnit("m");
        _vm.Unit2 = FindUnit("ft");
        Press(NumbersAndOperatorsEnum.One);
        Assert.NotEmpty(_vm.SupplementaryResults);
    }

    [Fact]
    public void SwitchActive_SwapsDirection()
    {
        ToCategory("Length");
        _vm.Unit1 = FindUnit("m");
        _vm.Unit2 = FindUnit("ft");
        Press(NumbersAndOperatorsEnum.One);
        string forward = _vm.Value2;

        _vm.SwitchActiveCommand.Execute(null);
        Assert.True(_vm.Value2Active);
        Assert.Equal(forward, _vm.Value2);
        Assert.Equal("1", _vm.Value1);
    }

    [Fact]
    public void InputEditing_ClearBackspaceNegate()
    {
        ToCategory("Length");
        _vm.Unit1 = FindUnit("m");
        _vm.Unit2 = FindUnit("ft");
        Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Three);
        Assert.Equal("123", _vm.Value1);
        Press(NumbersAndOperatorsEnum.Backspace);
        Assert.Equal("12", _vm.Value1);
        // Length does not support negatives: negate is ignored (upstream parity).
        Press(NumbersAndOperatorsEnum.Negate);
        Assert.Equal("12", _vm.Value1);
        Press(NumbersAndOperatorsEnum.Clear);
        Assert.Equal("0", _vm.Value1);
    }

    [Fact]
    public void Temperature_NegateApplies()
    {
        ToCategory("Temperature");
        _vm.Unit1 = _vm.Units.First(u => u.Abbreviation.Contains("C"));
        _vm.Unit2 = _vm.Units.First(u => u.Abbreviation.Contains("F"));
        Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Zero);
        Press(NumbersAndOperatorsEnum.Negate);
        Assert.Equal("-10", _vm.Value1);
        Assert.Equal("14", _vm.Value2);
    }

    [Fact]
    public void Persistence_RestoresUnits()
    {
        ToCategory("Length");
        _vm.Unit1 = FindUnit("m");
        _vm.Unit2 = FindUnit("ft");
        _vm.SaveUserPreferences();

        using var reopened = new UnitConverterViewModel(new LinuxSettings(_settingsDir));
        reopened.CurrentCategory = reopened.Categories.First(c => c.Name.Contains("Length"));
        Assert.Equal("m", reopened.Unit1.Abbreviation);
        Assert.Equal("ft", reopened.Unit2.Abbreviation);
    }

    [Fact]
    public void Paste_ValidatesAndApplies()
    {
        ToCategory("Length");
        _vm.Unit1 = FindUnit("m");
        _vm.Unit2 = FindUnit("ft");
        _vm.OnPaste("2.5");
        Assert.Equal("2.5", _vm.Value1);
    }
}
