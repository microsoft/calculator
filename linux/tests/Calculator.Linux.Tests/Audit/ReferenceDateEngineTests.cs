// AUDIT — replay of the REFERENCE's own date-calculation unit tests.
//
// Source: src/Calculator.Tests/DateCalculatorTests.cs, class
// DateCalculatorUnitTests @ d125246a (unmodified reference). The test data
// arrays and every expectation below are transcribed verbatim; only the MSTest
// attributes are replaced with xUnit and the engine class is the port's
// Calculator.Linux.Common.DateCalculation.DateCalculationEngine, which exposes
// the same AddDuration / SubtractDuration / TryGetDateDifference contract.
//
// These are Microsoft-authored expectations for date arithmetic, which the port
// reimplements in C# (the shared CalcManager engine is calculator-only).

using Calculator.Linux.Common.DateCalculation;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class ReferenceDateEngineTests
{
    private static readonly DateTimeOffset[] s_date = new DateTimeOffset[15];
    private static readonly DateDifference[] s_dateDifference = new DateDifference[14];
    private static readonly DateTimeTestCase[] s_datetimeDifftest = new DateTimeTestCase[9];
    private static readonly DateTimeTestCase[] s_datetimeBoundAdd = new DateTimeTestCase[2];
    private static readonly DateTimeTestCase[] s_datetimeBoundSubtract = new DateTimeTestCase[2];
    private static readonly DateTimeTestCase[] s_datetimeAddCase = new DateTimeTestCase[3];
    private static readonly DateTimeTestCase[] s_datetimeSubtractCase = new DateTimeTestCase[3];

    private static DateTimeOffset MakeDate(int year, int month, int day) =>
        new(year, month, day, 0, 0, 0, TimeSpan.Zero);

    private static DateCalculationEngine s_engine = null!;

    public ReferenceDateEngineTests()
    {
        s_engine = new DateCalculationEngine("GregorianCalendar");

        s_date[0] = MakeDate(9999, 12, 31);
        s_date[1] = MakeDate(9999, 12, 30);
        s_date[2] = MakeDate(9998, 12, 31);
        s_date[3] = MakeDate(1601, 1, 1);
        s_date[4] = MakeDate(1601, 1, 2);
        s_date[5] = MakeDate(2008, 5, 10);
        s_date[6] = MakeDate(2008, 3, 10);
        s_date[7] = MakeDate(2008, 2, 29);
        s_date[8] = MakeDate(2007, 2, 28);
        s_date[9] = MakeDate(2007, 3, 10);
        s_date[10] = MakeDate(2007, 5, 10);
        s_date[11] = MakeDate(2008, 1, 29);
        s_date[12] = MakeDate(2007, 1, 28);
        s_date[13] = MakeDate(2008, 1, 31);
        s_date[14] = MakeDate(2008, 3, 31);

        s_dateDifference[0] = new DateDifference { Year = 1, Month = 1 };
        s_dateDifference[1] = new DateDifference { Month = 1, Day = 10 };
        s_dateDifference[2] = new DateDifference { Day = 2 };
        s_dateDifference[3] = new DateDifference { Week = 52, Day = 1 };
        s_dateDifference[4] = new DateDifference { Year = 1 };
        s_dateDifference[5] = new DateDifference { Day = 365 };
        s_dateDifference[6] = new DateDifference { Month = 1 };
        s_dateDifference[7] = new DateDifference { Month = 1, Day = 2 };
        s_dateDifference[8] = new DateDifference { Day = 31 };
        s_dateDifference[9] = new DateDifference { Month = 11, Day = 1 };
        s_dateDifference[10] = new DateDifference { Year = 8398, Month = 11, Day = 30 };
        s_dateDifference[11] = new DateDifference { Year = 2008 };
        s_dateDifference[12] = new DateDifference { Year = 7991, Month = 11 };
        s_dateDifference[13] = new DateDifference { Week = 416998, Day = 1 };

        s_datetimeDifftest[0] = new DateTimeTestCase { StartDate = s_date[0], EndDate = s_date[3], DateDiff = s_dateDifference[10] };
        s_datetimeDifftest[1] = new DateTimeTestCase { StartDate = s_date[0], EndDate = s_date[2], DateDiff = s_dateDifference[5] };
        s_datetimeDifftest[2] = new DateTimeTestCase { StartDate = s_date[0], EndDate = s_date[2], DateDiff = s_dateDifference[4] };
        s_datetimeDifftest[3] = new DateTimeTestCase { StartDate = s_date[0], EndDate = s_date[2], DateDiff = s_dateDifference[3] };
        s_datetimeDifftest[4] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7], DateDiff = s_dateDifference[7] };
        s_datetimeDifftest[5] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7], DateDiff = s_dateDifference[8] };
        s_datetimeDifftest[6] = new DateTimeTestCase { StartDate = s_date[11], EndDate = s_date[8], DateDiff = s_dateDifference[9] };
        s_datetimeDifftest[7] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0], DateDiff = s_dateDifference[12] };
        s_datetimeDifftest[8] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0], DateDiff = s_dateDifference[13] };

        s_datetimeBoundAdd[0] = new DateTimeTestCase { StartDate = s_date[1], EndDate = s_date[0], DateDiff = s_dateDifference[2] };
        s_datetimeBoundAdd[1] = new DateTimeTestCase { StartDate = s_date[2], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

        s_datetimeBoundSubtract[0] = new DateTimeTestCase { StartDate = s_date[3], EndDate = s_date[0], DateDiff = s_dateDifference[2] };
        s_datetimeBoundSubtract[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

        s_datetimeAddCase[0] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
        s_datetimeAddCase[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[5], DateDiff = s_dateDifference[1] };
        s_datetimeAddCase[2] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[6], DateDiff = s_dateDifference[1] };

        s_datetimeSubtractCase[0] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
        s_datetimeSubtractCase[1] = new DateTimeTestCase { StartDate = s_date[6], EndDate = s_date[11], DateDiff = s_dateDifference[1] };
        s_datetimeSubtractCase[2] = new DateTimeTestCase { StartDate = s_date[9], EndDate = s_date[12], DateDiff = s_dateDifference[1] };
    }

    // Mirrors the reference's `internal struct DateTimeTestCase`, which uses
    // public settable fields via object-initializer syntax.
    private struct DateTimeTestCase
    {
        public DateTimeOffset StartDate;
        public DateTimeOffset EndDate;
        public DateDifference DateDiff;
    }

    /// <summary>
    /// The request has to name the same units the expectation uses, or the engine
    /// reduces the answer into units the case never set. Verbatim from the
    /// reference.
    /// </summary>
    private static DateUnit UnitsPresentIn(DateDifference difference)
    {
        DateUnit units = 0;
        if (difference.Year != 0) units |= DateUnit.Year;
        if (difference.Month != 0) units |= DateUnit.Month;
        if (difference.Week != 0) units |= DateUnit.Week;
        if (difference.Day != 0) units |= DateUnit.Day;
        return units;
    }

    [Fact]
    public void TestDateDiff()
    {
        for (int testIndex = 0; testIndex < s_datetimeDifftest.Length; testIndex++)
        {
            var testCase = s_datetimeDifftest[testIndex];
            var requested = UnitsPresentIn(testCase.DateDiff);

            // The engine takes the dates in order; putting them the other way
            // round is the view model's job to normalize.
            var earlier = testCase.StartDate <= testCase.EndDate ? testCase.StartDate : testCase.EndDate;
            var later = testCase.StartDate <= testCase.EndDate ? testCase.EndDate : testCase.StartDate;

            var difference = s_engine.TryGetDateDifference(earlier, later, requested);

            Assert.NotNull(difference);
            Assert.Equal(testCase.DateDiff.Year, difference!.Value.Year);
            Assert.Equal(testCase.DateDiff.Month, difference.Value.Month);
            Assert.Equal(testCase.DateDiff.Week, difference.Value.Week);
            Assert.Equal(testCase.DateDiff.Day, difference.Value.Day);
        }
    }

    [Fact]
    public void OneYearDifferenceEndingAtTheTopOfRangeIsCalculated()
    {
        var difference = s_engine.TryGetDateDifference(
            MakeDate(9998, 12, 31), MakeDate(9999, 12, 31), DateUnit.Year);

        Assert.NotNull(difference);
        Assert.Equal(1, difference!.Value.Year);
        Assert.Equal(0, difference.Value.Day);
    }

    [Fact]
    public void FullSupportedRangeReturnsYearMonthDayDifference()
    {
        var difference = s_engine.TryGetDateDifference(
            MakeDate(1601, 1, 1), MakeDate(9999, 12, 31),
            DateUnit.Year | DateUnit.Month | DateUnit.Day);

        Assert.NotNull(difference);
        Assert.Equal(8398, difference!.Value.Year);
        Assert.Equal(11, difference.Value.Month);
        Assert.Equal(30, difference.Value.Day);
    }

    [Fact]
    public void RangeEndingInYear9998ReturnsYearMonthDayDifference()
    {
        var difference = s_engine.TryGetDateDifference(
            MakeDate(1601, 1, 1), MakeDate(9998, 12, 31),
            DateUnit.Year | DateUnit.Month | DateUnit.Day);

        Assert.NotNull(difference);
        Assert.Equal(8397, difference!.Value.Year);
        Assert.Equal(11, difference.Value.Month);
        Assert.Equal(30, difference.Value.Day);
    }

    [Fact]
    public void TestAddOob()
    {
        for (int testIndex = 0; testIndex < s_datetimeBoundAdd.Length; testIndex++)
        {
            var endDate = s_engine.AddDuration(
                s_datetimeBoundAdd[testIndex].StartDate,
                s_datetimeBoundAdd[testIndex].DateDiff);

            Assert.Null(endDate);
        }
    }

    [Fact]
    public void TestSubtractOob()
    {
        for (int testIndex = 0; testIndex < s_datetimeBoundSubtract.Length; testIndex++)
        {
            var endDate = s_engine.SubtractDuration(
                s_datetimeBoundSubtract[testIndex].StartDate,
                s_datetimeBoundSubtract[testIndex].DateDiff);

            Assert.Null(endDate);
        }
    }

    [Fact]
    public void TestAddition()
    {
        for (int testIndex = 0; testIndex < s_datetimeAddCase.Length; testIndex++)
        {
            var endDate = s_engine.AddDuration(
                s_datetimeAddCase[testIndex].StartDate,
                s_datetimeAddCase[testIndex].DateDiff);

            Assert.NotNull(endDate);
            Assert.Equal(s_datetimeAddCase[testIndex].EndDate, endDate!.Value);
        }
    }

    [Fact]
    public void TestSubtraction()
    {
        for (int testIndex = 0; testIndex < s_datetimeSubtractCase.Length; testIndex++)
        {
            var endDate = s_engine.SubtractDuration(
                s_datetimeSubtractCase[testIndex].StartDate,
                s_datetimeSubtractCase[testIndex].DateDiff);

            Assert.NotNull(endDate);
            Assert.Equal(s_datetimeSubtractCase[testIndex].EndDate, endDate!.Value);
        }
    }

    [Fact]
    public void EngineHandlesTheFullGregorianRangeWithoutThrowing()
    {
        // Not a reference case: a guard that the supported range the reference
        // asserts (1601-01-01 .. 9999-12-31) is actually inclusive on both ends.
        Assert.NotNull(s_engine.AddDuration(MakeDate(1601, 1, 1), new DateDifference { Day = 1 }));
        Assert.NotNull(s_engine.SubtractDuration(MakeDate(9999, 12, 31), new DateDifference { Day = 1 }));
        Assert.Equal(MakeDate(1601, 1, 2), s_engine.AddDuration(MakeDate(1601, 1, 1), new DateDifference { Day = 1 })!.Value);
        Assert.Equal(MakeDate(9999, 12, 30), s_engine.SubtractDuration(MakeDate(9999, 12, 31), new DateDifference { Day = 1 })!.Value);
    }
}