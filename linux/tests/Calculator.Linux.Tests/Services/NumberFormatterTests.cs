// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 3, slice C (RED): LinuxNumberFormatter replaces WinRT DecimalFormatter
// / CurrencyFormatter (grouping, fraction digits, decimal-always-displayed,
// per-currency fraction digits from ISO 4217 minor units).
using System.Globalization;
using Calculator.Linux.Services;
using Xunit;

namespace Calculator.Linux.Tests.Services;

public sealed class NumberFormatterTests
{
    private static LinuxLocalization EnUs() => new(new CultureInfo("en-US"));

    [Fact]
    public void EnUs_GroupsThousands()
    {
        var f = new LinuxNumberFormatter(EnUs()) { IsGrouped = true, FractionDigits = 1 };
        Assert.Equal("1,234.5", f.Format(1234.5));
    }

    [Fact]
    public void EnUs_RoundsToFractionDigits()
    {
        var f = new LinuxNumberFormatter(EnUs()) { IsGrouped = true, FractionDigits = 0 };
        Assert.Equal("1,235", f.Format(1234.5));
    }

    [Fact]
    public void EnUs_PadsTrailingZeros()
    {
        var f = new LinuxNumberFormatter(EnUs()) { IsGrouped = true, FractionDigits = 2 };
        Assert.Equal("3.10", f.Format(3.1));
    }

    [Fact]
    public void EnUs_DecimalAlwaysDisplayed_AppendsSeparator()
    {
        var f = new LinuxNumberFormatter(EnUs()) { IsGrouped = true, FractionDigits = 0, IsDecimalPointAlwaysDisplayed = true };
        Assert.Equal("3.", f.Format(3));
    }

    [Fact]
    public void EnUs_Ungrouped_NoSeparators()
    {
        var f = new LinuxNumberFormatter(EnUs()) { IsGrouped = false, FractionDigits = 0 };
        Assert.Equal("1234567", f.Format(1234567));
    }

    [Fact]
    public void DeDe_UsesLocalSeparators()
    {
        var f = new LinuxNumberFormatter(new LinuxLocalization(new CultureInfo("de-DE"))) { IsGrouped = true, FractionDigits = 1 };
        Assert.Equal("1.234,5", f.Format(1234.5));
    }

    [Fact]
    public void FormatUInt_LocalizesDigits()
    {
        var en = new LinuxNumberFormatter(EnUs());
        Assert.Equal("5", en.FormatUInt(5));

        var ar = new LinuxNumberFormatter(new LinuxLocalization(new CultureInfo("ar-EG")));
        Assert.Equal(new LinuxLocalization(new CultureInfo("ar-EG")).DigitSymbolFromEnUsDigit('2').ToString(), ar.FormatUInt(2));
    }

    [Fact]
    public void Currency_UsdHasTwoDigits_JpyZero_JodThree()
    {
        var usd = LinuxNumberFormatter.ForCurrency("USD", EnUs());
        Assert.Equal("1,234.50", usd.Format(1234.5));

        var jpy = LinuxNumberFormatter.ForCurrency("JPY", EnUs());
        Assert.Equal("5", jpy.Format(5));

        var jod = LinuxNumberFormatter.ForCurrency("JOD", EnUs());
        Assert.Equal("1.250", jod.Format(1.25));
    }

    [Fact]
    public void Currency_UnknownCodeDefaultsToTwoDigits()
    {
        var xxx = LinuxNumberFormatter.ForCurrency("XXX", EnUs());
        Assert.Equal("1.00", xxx.Format(1));
    }
}
