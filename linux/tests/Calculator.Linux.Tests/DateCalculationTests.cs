// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 5, slice 1 (RED): date-difference and add/subtract vectors ported
// from src/Calculator.Tests/DateCalculatorTests.cs (Gregorian).
using System;
using Calculator.Linux.Common.DateCalculation;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class DateCalculationTests
{
    private static readonly DateCalculationEngine s_engine = new("gregorian");

    private static DateTimeOffset MakeDate(int year, int month, int day) =>
        new(year, month, day, 0, 0, 0, TimeSpan.Zero);

    [Fact]
    public void FullRange_Returns8398Years11Months30Days()
    {
        var diff = s_engine.TryGetDateDifference(
            MakeDate(1601, 1, 1), MakeDate(9999, 12, 31),
            DateUnit.Year | DateUnit.Month | DateUnit.Day);
        Assert.NotNull(diff);
        Assert.Equal(8398, diff.Value.Year);
        Assert.Equal(11, diff.Value.Month);
        Assert.Equal(30, diff.Value.Day);
    }

    [Fact]
    public void DayOnlyFormat_Returns365Days()
    {
        var diff = s_engine.TryGetDateDifference(
            MakeDate(9998, 12, 31), MakeDate(9999, 12, 31), DateUnit.Day);
        Assert.Equal(new DateDifference { Day = 365 }, diff);
    }

    [Fact]
    public void YearOnlyFormat_ReturnsOneYear()
    {
        var diff = s_engine.TryGetDateDifference(
            MakeDate(9998, 12, 31), MakeDate(9999, 12, 31), DateUnit.Year);
        Assert.NotNull(diff);
        Assert.Equal(1, diff.Value.Year);
        Assert.Equal(0, diff.Value.Day);
    }

    [Fact]
    public void LeapDay_ToNextYear_IsOneYear()
    {
        var diff = s_engine.TryGetDateDifference(
            MakeDate(2008, 2, 29), MakeDate(2009, 2, 28),
            DateUnit.Year | DateUnit.Month | DateUnit.Week | DateUnit.Day);
        Assert.Equal(new DateDifference { Year = 1 }, diff);
    }

    [Fact]
    public void ReversedDates_GiveSameDifference()
    {
        var forward = s_engine.TryGetDateDifference(
            MakeDate(2007, 3, 10), MakeDate(2008, 5, 10),
            DateUnit.Year | DateUnit.Month | DateUnit.Week | DateUnit.Day);
        var reversed = s_engine.TryGetDateDifference(
            MakeDate(2008, 5, 10), MakeDate(2007, 3, 10),
            DateUnit.Year | DateUnit.Month | DateUnit.Week | DateUnit.Day);
        Assert.Equal(forward, reversed);
    }

    [Fact]
    public void AddDuration_MonthEndClamps()
    {
        // 2008-01-31 + 1 month -> 2008-02-29 (leap year clamp).
        var result = s_engine.AddDuration(MakeDate(2008, 1, 31), new DateDifference { Month = 1 });
        Assert.Equal(MakeDate(2008, 2, 29), result);
    }

    [Fact]
    public void AddDuration_LeapDayPlusOneYear()
    {
        // 2008-02-29 + 1 year -> 2009-02-28.
        var result = s_engine.AddDuration(MakeDate(2008, 2, 29), new DateDifference { Year = 1 });
        Assert.Equal(MakeDate(2009, 2, 28), result);
    }

    [Fact]
    public void SubtractDuration_Basic()
    {
        var result = s_engine.SubtractDuration(MakeDate(2008, 5, 10), new DateDifference { Year = 1, Month = 2, Day = 10 });
        Assert.Equal(MakeDate(2007, 2, 28), result);
    }

    [Fact]
    public void AddDuration_Overflow_ReturnsNull()
    {
        var result = s_engine.AddDuration(MakeDate(9999, 12, 31), new DateDifference { Day = 1 });
        Assert.Null(result);
    }

    [Fact]
    public void SubtractDuration_BelowFloor_ReturnsNull()
    {
        var result = s_engine.SubtractDuration(MakeDate(1601, 1, 1), new DateDifference { Day = 1 });
        Assert.Null(result);
    }
}
