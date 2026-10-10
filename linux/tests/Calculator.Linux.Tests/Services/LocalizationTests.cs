// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 3, slice 1 (RED): LinuxLocalization must expose the same locale facts
// the ViewModels read from WinRT (separators, grouping, digits, RTL, week)
// but sourced from System.Globalization.CultureInfo.
using System.Globalization;
using Calculator.Linux.Services;
using Xunit;

namespace Calculator.Linux.Tests.Services;

public sealed class LocalizationTests
{
    [Fact]
    public void EnUs_HasExpectedSeparatorsAndIdentityDigits()
    {
        var loc = new LinuxLocalization(new CultureInfo("en-US"));

        Assert.Equal('.', loc.DecimalSeparator);
        Assert.Equal(',', loc.GroupSeparator);
        Assert.Equal("3;0", loc.NumberGrouping);
        Assert.Equal("en-US", loc.LocaleName);
        Assert.True(loc.IsDigitEnUsSetting);
        Assert.False(loc.IsRightToLeft);
        Assert.Equal("0", loc.DigitSymbolFromEnUsDigit('0').ToString());
        // Same well-known strings as WinRT CalendarIdentifiers ("gregorian", ...).
        Assert.Equal("gregorian", loc.CalendarIdentifier);
    }

    [Fact]
    public void DeDe_UsesCommaDecimalAndMondayFirst()
    {
        var loc = new LinuxLocalization(new CultureInfo("de-DE"));

        Assert.Equal(',', loc.DecimalSeparator);
        Assert.Equal('.', loc.GroupSeparator);
        Assert.Equal(DayOfWeek.Monday, loc.FirstDayOfWeek);
    }

    [Fact]
    public void EnUs_WeekStartsSunday()
    {
        var loc = new LinuxLocalization(new CultureInfo("en-US"));
        Assert.Equal(DayOfWeek.Sunday, loc.FirstDayOfWeek);
    }

    [Fact]
    public void ArEg_UsesLocalizedDigitsAndDelocalizes()
    {
        var loc = new LinuxLocalization(new CultureInfo("ar-EG"));

        Assert.False(loc.IsDigitEnUsSetting);
        Assert.True(loc.IsRightToLeft);
        // Eastern Arabic digits must round-trip through delocalization.
        string localizedTwo = loc.DigitSymbolFromEnUsDigit('2').ToString();
        Assert.NotEqual("2", localizedTwo);
        Assert.True(loc.IsLocalizedDigit(localizedTwo[0]));
        Assert.Equal("2", loc.EnglishValueFromLocalizedDigits(localizedTwo));
    }

    [Fact]
    public void EnUs_LocalizeIsNoOpAndDelocalizeIsIdentity()
    {
        var loc = new LinuxLocalization(new CultureInfo("en-US"));

        string value = "1,234.5";
        string localized = value;
        loc.LocalizeDisplayValue(ref localized);
        Assert.Equal(value, localized);
        Assert.Equal(value, loc.EnglishValueFromLocalizedDigits(value));
    }

    [Fact]
    public void DeDe_GroupSeparatorsRemoved()
    {
        var loc = new LinuxLocalization(new CultureInfo("de-DE"));

        // de-DE groups with '.'; both '.' and regular spaces must be stripped.
        Assert.Equal("1234", loc.RemoveGroupSeparators("1.234"));
    }

    [Fact]
    public void CurrencyFacts_ComeFromCulture()
    {
        var us = new LinuxLocalization(new CultureInfo("en-US"));
        Assert.Equal(2, us.CurrencyTrailingDigits);

        var jpn = new LinuxLocalization(new CultureInfo("ja-JP"));
        Assert.Equal(0, jpn.CurrencyTrailingDigits);
    }

    [Fact]
    public void ListSeparator_MatchesCulture()
    {
        var de = new LinuxLocalization(new CultureInfo("de-DE"));
        Assert.Equal(";", de.ListSeparator);
    }
}
