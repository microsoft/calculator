// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Manual-matrix coverage, automated: every non-currency category converts
// both directions (round-trip within display rounding) and negate gating
// matches SupportsNegative. Real native engine, no mocks.
using System.Globalization;
using Calculator.Linux.DataLoaders;
using Calculator.Linux.Interop;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class ConverterMatrixTests : IDisposable
{
    private readonly ManagedUnitConverter _converter;

    public ConverterMatrixTests()
    {
        _converter = new ManagedUnitConverter(new UnitConverterDataLoader("US"), "US");
    }

    public void Dispose() => _converter.Dispose();

    public static IEnumerable<object[]> CategoryIds()
    {
        for (int id = 4; id <= 15; id++)
        {
            yield return new object[] { id };
        }
    }

    private static void Enter(ManagedUnitConverter converter, string digits)
    {
        foreach (char ch in digits)
        {
            converter.Send(ch switch
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

    private static double Parse(string text) =>
        double.Parse(text, NumberStyles.Float, CultureInfo.InvariantCulture);

    [Theory]
    [MemberData(nameof(CategoryIds))]
    public void RoundTrip_AllCategories(int categoryId)
    {
        var (units, _, _) = _converter.SetCategory(categoryId);
        var pair = units.Where(u => !u.IsWhimsical).Take(2).ToList();
        Assert.Equal(2, pair.Count);

        _converter.SetUnitTypes(pair[0].Id, pair[1].Id);
        _converter.Send(UnitConverterCommand.Clear);
        Enter(_converter, "10");
        double forward = Parse(_converter.LastTo);

        // Reverse without re-entering text (immune to exponent notation):
        // the converted value becomes the new input, units swap back.
        _converter.SwitchActive(_converter.LastTo);
        _converter.SetUnitTypes(pair[1].Id, pair[0].Id);
        double back = Parse(_converter.LastTo);
        Assert.Equal(10.0, back, precision: 1);
    }

    [Theory]
    [MemberData(nameof(CategoryIds))]
    public void Negate_MatchesSupportsNegative(int categoryId)
    {
        var categories = _converter.Categories();
        var category = categories.First(c => c.Id == categoryId);
        var (units, _, _) = _converter.SetCategory(categoryId);
        var pair = units.Where(u => !u.IsWhimsical).Take(2).ToList();
        _converter.SetUnitTypes(pair[0].Id, pair[1].Id);
        _converter.Send(UnitConverterCommand.Clear);
        Enter(_converter, "5");
        _converter.Send(UnitConverterCommand.Negate);

        if (category.SupportsNegative)
        {
            Assert.StartsWith("-", _converter.LastFrom);
        }
        else
        {
            Assert.Equal("5", _converter.LastFrom);
        }
    }
}
