// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 6, slice 3 (RED): conversions through the REAL native UnitConverter
// fed by the ported C# data loader (no mocks).
using Calculator.Linux.DataLoaders;
using Calculator.Linux.Interop;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class UnitConverterTests : IDisposable
{
    private const int Length = 5; // NavCategoryStates.Serialize(ViewMode.Length)
    private const int Temperature = 7;
    private const int Weight = 6;
    private const int Meter = 34;
    private const int Foot = 31;
    private const int Kilometer = 33;
    private const int Celsius = 46;
    private const int Fahrenheit = 47;
    private const int Kilogram = 88;
    private const int Pound = 92;

    private readonly ManagedUnitConverter _converter;

    public UnitConverterTests()
    {
        var loader = new UnitConverterDataLoader("US");
        _converter = new ManagedUnitConverter(loader, "US");
    }

    public void Dispose() => _converter.Dispose();

    private void Enter(string digits)
    {
        foreach (char ch in digits)
        {
            _converter.Send(ch switch
            {
                '0' => UnitConverterCommand.Zero,
                '1' => UnitConverterCommand.One,
                '2' => UnitConverterCommand.Two,
                '3' => UnitConverterCommand.Three,
                '4' => UnitConverterCommand.Four,
                '5' => UnitConverterCommand.Five,
                '6' => UnitConverterCommand.Six,
                '7' => UnitConverterCommand.Seven,
                '8' => UnitConverterCommand.Eight,
                '9' => UnitConverterCommand.Nine,
                '.' => UnitConverterCommand.Decimal,
                _ => UnitConverterCommand.None,
            });
        }
    }

    [Fact]
    public void Categories_ExcludeCurrencyInTask6()
    {
        var categories = _converter.Categories();
        Assert.Equal(12, categories.Count);
        Assert.DoesNotContain(categories, c => c.Name.Contains("Currency"));
        Assert.Contains(categories, c => c.Name.Contains("Length"));
    }

    [Fact]
    public void Length_MetersToFeet()
    {
        var (units, _, _) = _converter.SetCategory(Length);
        Assert.Contains(units, u => u.Id == Meter && u.Abbreviation == "m");
        Assert.Contains(units, u => u.Id == Foot && u.Abbreviation == "ft");

        _converter.SetUnitTypes(Meter, Foot);
        Enter("100");
        Assert.Equal("100", _converter.LastFrom);
        Assert.StartsWith("328.084", _converter.LastTo);
    }

    [Fact]
    public void Length_FeetToKilometers()
    {
        _converter.SetCategory(Length);
        _converter.SetUnitTypes(Foot, Kilometer);
        Enter("1");
        // 1 ft = 0.0003048 km; the engine displays rounded significant digits.
        Assert.Equal("0.000305", _converter.LastTo);
    }

    [Fact]
    public void Temperature_ZeroCelsiusIs32Fahrenheit()
    {
        _converter.SetCategory(Temperature);
        _converter.SetUnitTypes(Celsius, Fahrenheit);
        Enter("0");
        Assert.Equal("32", _converter.LastTo);
    }

    [Fact]
    public void Temperature_BoilingPoint()
    {
        _converter.SetCategory(Temperature);
        _converter.SetUnitTypes(Celsius, Fahrenheit);
        Enter("100");
        Assert.Equal("212", _converter.LastTo);
    }

    [Fact]
    public void Weight_KilogramToPound()
    {
        _converter.SetCategory(Weight);
        _converter.SetUnitTypes(Kilogram, Pound);
        Enter("1");
        Assert.StartsWith("2.20462", _converter.LastTo);
    }

    [Fact]
    public void Supplementary_ResultsProvided()
    {
        _converter.SetCategory(Length);
        _converter.SetUnitTypes(Meter, Foot);
        Enter("100");
        Assert.NotEmpty(_converter.LastSuggested);
    }

    [Fact]
    public void Preferences_RoundTrip()
    {
        _converter.SetCategory(Length);
        string prefs = _converter.SavePrefs();
        Assert.False(string.IsNullOrEmpty(prefs));
        _converter.RestorePrefs(prefs);
    }

    [Fact]
    public void Clear_ResetsInput()
    {
        _converter.SetCategory(Length);
        _converter.SetUnitTypes(Meter, Foot);
        Enter("100");
        _converter.Send(UnitConverterCommand.Clear);
        Assert.Equal("0", _converter.LastFrom);
    }
}
