// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 5, slice 2: DateCalculatorViewModel behavior (diff/add/subtract,
// same-dates, out-of-bound, resource wiring).
using System.Globalization;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class DateCalculatorViewModelTests : IDisposable
{
    private readonly DateCalculatorViewModel _vm;

    public DateCalculatorViewModelTests()
    {
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("en-US"));
        _vm = new DateCalculatorViewModel();
    }

    public void Dispose()
    {
    }

    private static DateTimeOffset Day(int y, int m, int d) => new(y, m, d, 0, 0, 0, TimeSpan.Zero);

    [Fact]
    public void Diff_OneYearApart_ShowsYear()
    {
        _vm.IsDateDiffMode = true;
        _vm.FromDate = Day(2007, 3, 10);
        _vm.ToDate = Day(2008, 5, 10);
        Assert.Contains("year", _vm.StrDateDiffResult, StringComparison.OrdinalIgnoreCase);
        Assert.Contains("month", _vm.StrDateDiffResult, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Diff_SameDates_ShowsSameDatesMessage()
    {
        _vm.IsDateDiffMode = true;
        _vm.FromDate = Day(2026, 1, 5);
        _vm.ToDate = Day(2026, 1, 5);
        Assert.Equal(AppResources.GetString("Date_SameDates"), _vm.StrDateDiffResult);
    }

    [Fact]
    public void Diff_ReversedDates_MatchesForward()
    {
        _vm.IsDateDiffMode = true;
        _vm.FromDate = Day(2007, 3, 10);
        _vm.ToDate = Day(2008, 5, 10);
        string forward = _vm.StrDateDiffResult;

        _vm.FromDate = Day(2008, 5, 10);
        _vm.ToDate = Day(2007, 3, 10);
        Assert.Equal(forward, _vm.StrDateDiffResult);
    }

    [Fact]
    public void Add_OneYearToLeapDay_GivesFeb28()
    {
        _vm.IsDateDiffMode = false;
        _vm.IsAddMode = true;
        _vm.StartDate = Day(2008, 2, 29);
        _vm.YearsOffset = 1;
        _vm.MonthsOffset = 0;
        _vm.DaysOffset = 0;
        Assert.Contains("February 28, 2009", _vm.StrDateResult);
    }

    [Fact]
    public void Subtract_Basic()
    {
        _vm.IsDateDiffMode = false;
        _vm.IsAddMode = false;
        _vm.StartDate = Day(2008, 5, 10);
        _vm.YearsOffset = 1;
        _vm.MonthsOffset = 2;
        _vm.DaysOffset = 10;
        Assert.Contains("February 28, 2007", _vm.StrDateResult);
    }

    [Fact]
    public void Add_Overflow_ShowsOutOfBound()
    {
        _vm.IsDateDiffMode = false;
        _vm.IsAddMode = true;
        _vm.StartDate = Day(9999, 12, 31);
        _vm.YearsOffset = 0;
        _vm.MonthsOffset = 0;
        _vm.DaysOffset = 1;
        Assert.Equal(AppResources.GetString("Date_OutOfBoundMessage"), _vm.StrDateResult);
    }

    [Fact]
    public void Diff_AcrossDstBoundary_CountsCalendarDays()
    {
        // US DST 2026 starts Mar 8; UTC-clipped arithmetic is immune.
        // Sub-week differences render in StrDateDiffResult (not the InDays field).
        _vm.IsDateDiffMode = true;
        _vm.FromDate = Day(2026, 3, 7);
        _vm.ToDate = Day(2026, 3, 9);
        Assert.Equal("2 days", _vm.StrDateDiffResult);
    }

    [Fact]
    public void DisplayStrings_UseRealResources_NotFallbackKeys()
    {
        _vm.IsDateDiffMode = true;
        _vm.FromDate = Day(2007, 3, 10);
        _vm.ToDate = Day(2008, 5, 10);
        Assert.DoesNotContain("Date_", _vm.StrDateDiffResult);
        Assert.DoesNotContain("Date_", _vm.StrDateDiffResultAutomationName);
    }
}
