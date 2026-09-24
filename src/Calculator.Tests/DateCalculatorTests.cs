// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.DateCalculation;

namespace Calculator.Tests
{
    #region Test Data

    internal struct DateTimeTestCase
    {
        public DateTimeOffset StartDate;
        public DateTimeOffset EndDate;
        public DateDifference DateDiff;
    }

    #endregion

    [TestClass]
    public class DateCalculatorUnitTests
    {
        private static DateCalculationEngine s_dateCalcEngine;

        private static readonly DateTimeOffset[] s_date = new DateTimeOffset[15];
        private static readonly DateDifference[] s_dateDifference = new DateDifference[14];
        private static readonly DateTimeTestCase[] s_datetimeDifftest = new DateTimeTestCase[9];
        private static readonly DateTimeTestCase[] s_datetimeBoundAdd = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeBoundSubtract = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeAddCase = new DateTimeTestCase[3];
        private static readonly DateTimeTestCase[] s_datetimeSubtractCase = new DateTimeTestCase[3];

        private static DateTimeOffset MakeDate(int year, int month, int day) =>
            new DateTimeOffset(year, month, day, 0, 0, 0, TimeSpan.Zero);

        [ClassInitialize]
        public static void TestClassSetup(TestContext context)
        {
            s_dateCalcEngine = new DateCalculationEngine("GregorianCalendar");

            // Dates - DD.MM.YYYY
            s_date[0]  = MakeDate(9999, 12, 31);
            s_date[1]  = MakeDate(9999, 12, 30);
            s_date[2]  = MakeDate(9998, 12, 31);
            s_date[3]  = MakeDate(1601,  1,  1);
            s_date[4]  = MakeDate(1601,  1,  2);
            s_date[5]  = MakeDate(2008,  5, 10);
            s_date[6]  = MakeDate(2008,  3, 10);
            s_date[7]  = MakeDate(2008,  2, 29);
            s_date[8]  = MakeDate(2007,  2, 28);
            s_date[9]  = MakeDate(2007,  3, 10);
            s_date[10] = MakeDate(2007,  5, 10);
            s_date[11] = MakeDate(2008,  1, 29);
            s_date[12] = MakeDate(2007,  1, 28);
            s_date[13] = MakeDate(2008,  1, 31);
            s_date[14] = MakeDate(2008,  3, 31);

            // Date Differences
            s_dateDifference[0]  = new DateDifference { Year = 1, Month = 1 };
            s_dateDifference[1]  = new DateDifference { Month = 1, Day = 10 };
            s_dateDifference[2]  = new DateDifference { Day = 2 };
            s_dateDifference[3]  = new DateDifference { Week = 52, Day = 1 };
            s_dateDifference[4]  = new DateDifference { Year = 1 };
            s_dateDifference[5]  = new DateDifference { Day = 365 };
            s_dateDifference[6]  = new DateDifference { Month = 1 };
            s_dateDifference[7]  = new DateDifference { Month = 1, Day = 2 };
            s_dateDifference[8]  = new DateDifference { Day = 31 };
            s_dateDifference[9]  = new DateDifference { Month = 11, Day = 1 };
            s_dateDifference[10] = new DateDifference { Year = 8398, Month = 11, Day = 30 };
            s_dateDifference[11] = new DateDifference { Year = 2008 };
            s_dateDifference[12] = new DateDifference { Year = 7991, Month = 11 };
            s_dateDifference[13] = new DateDifference { Week = 416998, Day = 1 };

            // Date Difference test cases
            s_datetimeDifftest[0] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[3],  DateDiff = s_dateDifference[10] };
            s_datetimeDifftest[1] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[5] };
            s_datetimeDifftest[2] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[4] };
            s_datetimeDifftest[3] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[3] };
            s_datetimeDifftest[4] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[7] };
            s_datetimeDifftest[5] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[8] };
            s_datetimeDifftest[6] = new DateTimeTestCase { StartDate = s_date[11], EndDate = s_date[8],  DateDiff = s_dateDifference[9] };
            s_datetimeDifftest[7] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[12] };
            s_datetimeDifftest[8] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[13] };

            // Date Add Out of Bound test cases
            s_datetimeBoundAdd[0] = new DateTimeTestCase { StartDate = s_date[1], EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundAdd[1] = new DateTimeTestCase { StartDate = s_date[2], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Subtract Out of Bound test cases
            s_datetimeBoundSubtract[0] = new DateTimeTestCase { StartDate = s_date[3],  EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundSubtract[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Add test cases
            s_datetimeAddCase[0] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeAddCase[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[5], DateDiff = s_dateDifference[1] };
            s_datetimeAddCase[2] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[6], DateDiff = s_dateDifference[1] };

            // Date Subtract test cases
            s_datetimeSubtractCase[0] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeSubtractCase[1] = new DateTimeTestCase { StartDate = s_date[6],  EndDate = s_date[11], DateDiff = s_dateDifference[1] };
            s_datetimeSubtractCase[2] = new DateTimeTestCase { StartDate = s_date[9],  EndDate = s_date[12], DateDiff = s_dateDifference[1] };
        }

        [TestMethod]
        public void TestDateDiff()
        {
            for (int testIndex = 0; testIndex < s_datetimeDifftest.Length; testIndex++)
            {
                var testCase = s_datetimeDifftest[testIndex];
                var requested = UnitsPresentIn(testCase.DateDiff);

                // The engine takes the dates in order; putting them the other way round is the view
                // model's job to normalize, which DateCalcViewModelDateDiffIgnoreSignTest covers.
                var earlier = testCase.StartDate <= testCase.EndDate ? testCase.StartDate : testCase.EndDate;
                var later = testCase.StartDate <= testCase.EndDate ? testCase.EndDate : testCase.StartDate;

                var difference = s_dateCalcEngine.TryGetDateDifference(earlier, later, requested);

                Assert.IsNotNull(difference, $"TryGetDateDifference returned null for case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Year, difference.Value.Year, $"year, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Month, difference.Value.Month, $"month, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Week, difference.Value.Week, $"week, case {testIndex}");
                Assert.AreEqual(testCase.DateDiff.Day, difference.Value.Day, $"day, case {testIndex}");
            }
        }

        [TestMethod]
        public void OneYearDifferenceEndingAtTheTopOfRangeIsCalculated()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(9998, 12, 31),
                MakeDate(9999, 12, 31),
                DateUnit.Year);

            Assert.IsNotNull(difference);
            Assert.AreEqual(1, difference.Value.Year);
            Assert.AreEqual(0, difference.Value.Day);
        }

        [TestMethod]
        public void FullSupportedRangeReturnsYearMonthDayDifference()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(1601, 1, 1),
                MakeDate(9999, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(8398, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(30, difference.Value.Day);
        }

        [TestMethod]
        public void RangeEndingInYear9998ReturnsYearMonthDayDifference()
        {
            var difference = s_dateCalcEngine.TryGetDateDifference(
                MakeDate(1601, 1, 1),
                MakeDate(9998, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(8397, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(30, difference.Value.Day);
        }

        // The request has to name the same units the expectation uses, or the engine reduces the
        // answer into ones the case never set.
        private static DateUnit UnitsPresentIn(DateDifference difference)
        {
            DateUnit units = 0;
            if (difference.Year != 0) units |= DateUnit.Year;
            if (difference.Month != 0) units |= DateUnit.Month;
            if (difference.Week != 0) units |= DateUnit.Week;
            if (difference.Day != 0) units |= DateUnit.Day;
            return units;
        }

        [TestMethod]
        public void TestAddOob()
        {
            for (int testIndex = 0; testIndex < s_datetimeBoundAdd.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.AddDuration(
                    s_datetimeBoundAdd[testIndex].StartDate,
                    s_datetimeBoundAdd[testIndex].DateDiff);

                Assert.IsNull(endDate, $"AddDuration should return null for out-of-bound case {testIndex}");
            }
        }

        [TestMethod]
        public void TestSubtractOob()
        {
            for (int testIndex = 0; testIndex < s_datetimeBoundSubtract.Length; testIndex++)
            {
                // Subtract Duration
                var endDate = s_dateCalcEngine.SubtractDuration(
                    s_datetimeBoundSubtract[testIndex].StartDate,
                    s_datetimeBoundSubtract[testIndex].DateDiff);

                // Assert for the result
                Assert.IsNull(endDate, $"SubtractDuration should return null for out-of-bound case {testIndex}");
            }
        }

        [TestMethod]
        public void TestAddition()
        {
            for (int testIndex = 0; testIndex < s_datetimeAddCase.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.AddDuration(
                    s_datetimeAddCase[testIndex].StartDate,
                    s_datetimeAddCase[testIndex].DateDiff);

                Assert.IsNotNull(endDate, $"AddDuration returned null for case {testIndex}");
                Assert.AreEqual(
                    s_datetimeAddCase[testIndex].EndDate,
                    endDate.Value,
                    $"AddDuration produced the wrong date for case {testIndex}");
            }
        }

        [TestMethod]
        public void TestSubtraction()
        {
            for (int testIndex = 0; testIndex < s_datetimeSubtractCase.Length; testIndex++)
            {
                var endDate = s_dateCalcEngine.SubtractDuration(
                    s_datetimeSubtractCase[testIndex].StartDate,
                    s_datetimeSubtractCase[testIndex].DateDiff);

                Assert.IsNotNull(endDate, $"SubtractDuration returned null for case {testIndex}");
                Assert.AreEqual(
                    s_datetimeSubtractCase[testIndex].EndDate,
                    endDate.Value,
                    $"SubtractDuration produced the wrong date for case {testIndex}");
            }
        }
    }

    [TestClass]
    public class DateCalculatorViewModelTests
    {
        private static readonly DateTimeOffset[] s_date = new DateTimeOffset[15];
        private static readonly DateDifference[] s_dateDifference = new DateDifference[14];
        private static readonly DateTimeTestCase[] s_datetimeDifftest = new DateTimeTestCase[9];
        private static readonly DateTimeTestCase[] s_datetimeBoundAdd = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeBoundSubtract = new DateTimeTestCase[2];
        private static readonly DateTimeTestCase[] s_datetimeAddCase = new DateTimeTestCase[3];
        private static readonly DateTimeTestCase[] s_datetimeSubtractCase = new DateTimeTestCase[3];

        private static DateTimeOffset MakeDate(int year, int month, int day) =>
            new DateTimeOffset(year, month, day, 0, 0, 0, TimeSpan.Zero);

        private static string LocalizeNumber(int value)
        {
            string result = value.ToString();
            LocalizationSettings.GetInstance().LocalizeDisplayValue(ref result);
            return result;
        }

        [ClassInitialize]
        public static void TestClassSetup(TestContext context)
        {
            // Dates - DD.MM.YYYY
            s_date[0]  = MakeDate(9999, 12, 31);
            s_date[1]  = MakeDate(9999, 12, 30);
            s_date[2]  = MakeDate(9998, 12, 31);
            s_date[3]  = MakeDate(1601,  1,  1);
            s_date[4]  = MakeDate(1601,  1,  2);
            s_date[5]  = MakeDate(2008,  5, 10);
            s_date[6]  = MakeDate(2008,  3, 10);
            s_date[7]  = MakeDate(2008,  2, 29);
            s_date[8]  = MakeDate(2007,  2, 28);
            s_date[9]  = MakeDate(2007,  3, 10);
            s_date[10] = MakeDate(2007,  5, 10);
            s_date[11] = MakeDate(2008,  1, 29);
            s_date[12] = MakeDate(2007,  1, 28);
            s_date[13] = MakeDate(2008,  1, 31);
            s_date[14] = MakeDate(2008,  3, 31);

            // Date Differences
            s_dateDifference[0]  = new DateDifference { Year = 1, Month = 1 };
            s_dateDifference[1]  = new DateDifference { Month = 1, Day = 10 };
            s_dateDifference[2]  = new DateDifference { Day = 2 };
            s_dateDifference[3]  = new DateDifference { Week = 52, Day = 1 };
            s_dateDifference[4]  = new DateDifference { Year = 1 };
            s_dateDifference[5]  = new DateDifference { Day = 365 };
            s_dateDifference[6]  = new DateDifference { Month = 1 };
            s_dateDifference[7]  = new DateDifference { Month = 1, Day = 2 };
            s_dateDifference[8]  = new DateDifference { Day = 31 };
            s_dateDifference[9]  = new DateDifference { Month = 11, Day = 1 };
            s_dateDifference[10] = new DateDifference { Year = 8398, Month = 11, Day = 30 };
            s_dateDifference[11] = new DateDifference { Year = 2008 };
            s_dateDifference[12] = new DateDifference { Year = 7991, Month = 11 };
            s_dateDifference[13] = new DateDifference { Week = 416998, Day = 1 };

            // Date Difference test cases
            s_datetimeDifftest[0] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[3],  DateDiff = s_dateDifference[10] };
            s_datetimeDifftest[1] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[5] };
            s_datetimeDifftest[2] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[4] };
            s_datetimeDifftest[3] = new DateTimeTestCase { StartDate = s_date[0],  EndDate = s_date[2],  DateDiff = s_dateDifference[3] };
            s_datetimeDifftest[4] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[7] };
            s_datetimeDifftest[5] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7],  DateDiff = s_dateDifference[8] };
            s_datetimeDifftest[6] = new DateTimeTestCase { StartDate = s_date[11], EndDate = s_date[8],  DateDiff = s_dateDifference[9] };
            s_datetimeDifftest[7] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[12] };
            s_datetimeDifftest[8] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[0],  DateDiff = s_dateDifference[13] };

            // Date Add Out of Bound test cases
            s_datetimeBoundAdd[0] = new DateTimeTestCase { StartDate = s_date[1], EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundAdd[1] = new DateTimeTestCase { StartDate = s_date[2], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Subtract Out of Bound test cases
            s_datetimeBoundSubtract[0] = new DateTimeTestCase { StartDate = s_date[3],  EndDate = s_date[0], DateDiff = s_dateDifference[2] };
            s_datetimeBoundSubtract[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[0], DateDiff = s_dateDifference[11] };

            // Date Add test cases
            s_datetimeAddCase[0] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeAddCase[1] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[5], DateDiff = s_dateDifference[1] };
            s_datetimeAddCase[2] = new DateTimeTestCase { StartDate = s_date[13], EndDate = s_date[6], DateDiff = s_dateDifference[1] };

            // Date Subtract test cases
            s_datetimeSubtractCase[0] = new DateTimeTestCase { StartDate = s_date[14], EndDate = s_date[7], DateDiff = s_dateDifference[6] };
            s_datetimeSubtractCase[1] = new DateTimeTestCase { StartDate = s_date[6],  EndDate = s_date[11], DateDiff = s_dateDifference[1] };
            s_datetimeSubtractCase[2] = new DateTimeTestCase { StartDate = s_date[9],  EndDate = s_date[12], DateDiff = s_dateDifference[1] };
        }

        [TestMethod]
        public void DateCalcViewModelInitializationTest()
        {
            var viewModel = new DateCalculatorViewModel();

            Assert.IsTrue(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            Assert.AreNotEqual(default(DateTimeOffset), viewModel.FromDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.ToDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.StartDate);

            Assert.AreEqual(0, viewModel.DaysOffset);
            Assert.AreEqual(0, viewModel.MonthsOffset);
            Assert.AreEqual(0, viewModel.YearsOffset);

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("Same dates", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
            Assert.AreEqual(string.Empty, viewModel.StrDateResult);
        }

        [TestMethod]
        public void DateCalcViewModelAddSubtractInitTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = false;

            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            Assert.AreNotEqual(default(DateTimeOffset), viewModel.FromDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.ToDate);
            Assert.AreNotEqual(default(DateTimeOffset), viewModel.StartDate);

            Assert.AreEqual(0, viewModel.DaysOffset);
            Assert.AreEqual(0, viewModel.MonthsOffset);
            Assert.AreEqual(0, viewModel.YearsOffset);

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("Same dates", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);

            Assert.IsNotNull(viewModel.StrDateResult);
            Assert.AreNotEqual("", viewModel.StrDateResult);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffDaylightSavingTimeTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            // 31.03.2008 -> 29.02.2008
            viewModel.FromDate = s_datetimeDifftest[5].StartDate;
            viewModel.ToDate = s_datetimeDifftest[5].EndDate;

            // Assert for the result
            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("31 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("1 month, 2 days", viewModel.StrDateDiffResult);

            // Daylight Saving Time - Clock Forward
            // 10.03.2019 -> 11.03.2019
            viewModel.FromDate = MakeDate(2019, 3, 10);
            viewModel.ToDate = MakeDate(2019, 3, 11);
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);

            // 10.03.2019 -> 17.03.2019
            viewModel.ToDate = MakeDate(2019, 3, 17);
            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("1 week", viewModel.StrDateDiffResult);

            // Daylight Saving Time - Clock Backward
            // 03.11.2019 -> 04.11.2019
            viewModel.FromDate = MakeDate(2019, 11, 3);
            viewModel.ToDate = MakeDate(2019, 11, 4);
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
        }

        [TestMethod]
        public void DateCalcViewModelAddTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;

            for (int testIndex = 0; testIndex < s_datetimeAddCase.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeAddCase[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeAddCase[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeAddCase[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeAddCase[testIndex].DateDiff.Year;

                Assert.AreEqual(
                    ExpectedDateString(s_datetimeAddCase[testIndex].EndDate),
                    viewModel.StrDateResult,
                    $"Add mode produced the wrong date for case {testIndex}");
            }
        }

        [TestMethod]
        public void DateCalcViewModelSubtractTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = false;

            for (int testIndex = 0; testIndex < s_datetimeSubtractCase.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeSubtractCase[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeSubtractCase[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeSubtractCase[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeSubtractCase[testIndex].DateDiff.Year;

                Assert.AreEqual(
                    ExpectedDateString(s_datetimeSubtractCase[testIndex].EndDate),
                    viewModel.StrDateResult,
                    $"Subtract mode produced the wrong date for case {testIndex}");
            }
        }

        [TestMethod]
        public void DateCalcViewModelAddOobTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;
            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsTrue(viewModel.IsAddMode);

            for (int testIndex = 0; testIndex < s_datetimeBoundAdd.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeBoundAdd[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeBoundAdd[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeBoundAdd[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeBoundAdd[testIndex].DateDiff.Year;

                // Assert for the result
                Assert.AreEqual("Date out of Bound", viewModel.StrDateResult);
            }
        }

        // Built through the regional long-date formatter, the same way the view model renders it,
        // rather than hard-coded to one locale.
        private static string ExpectedDateString(DateTimeOffset date)
        {
            return LocalizationSettings.GetInstance()
                .GetRegionalSettingsAwareDateTimeFormatter("longdate")
                .Format(date);
        }

        [TestMethod]
        public void DateCalcViewModelSubtractOobTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = false;
            Assert.IsFalse(viewModel.IsDateDiffMode);
            Assert.IsFalse(viewModel.IsAddMode);

            for (int testIndex = 0; testIndex < s_datetimeBoundSubtract.Length; testIndex++)
            {
                viewModel.StartDate = s_datetimeBoundSubtract[testIndex].StartDate;
                viewModel.DaysOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Day;
                viewModel.MonthsOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Month;
                viewModel.YearsOffset = s_datetimeBoundSubtract[testIndex].DateDiff.Year;

                // Assert for the result
                Assert.AreEqual("Date out of Bound", viewModel.StrDateResult);
            }
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffIgnoreSignTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[10]; // 10.05.2007
            viewModel.ToDate = s_date[6];    // 10.03.2008

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("305 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("10 months", viewModel.StrDateDiffResult);

            viewModel.FromDate = s_date[6];  // 10.03.2008
            viewModel.ToDate = s_date[10];   // 10.05.2007

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("305 days", viewModel.StrDateDiffResultInDays);
            Assert.AreEqual("10 months", viewModel.StrDateDiffResult);
        }

        [TestMethod]
        public void DateCalcViewModelRangeEndingAtMaximumShowsYearsMonthsAndTotalDays()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = true;

            viewModel.FromDate = s_date[13];  // 31.01.2008
            viewModel.ToDate = s_date[0];     // 31.12.9999

            var resources = AppResourceProvider.GetInstance();
            string expectedBreakdown = LocalizeNumber(7991) + " " + resources.GetResourceString("Date_Years")
                + LocalizationSettings.GetInstance().GetListSeparator() + " "
                + LocalizeNumber(11) + " " + resources.GetResourceString("Date_Months");
            string expectedDays = LocalizeNumber(2918987) + " " + resources.GetResourceString("Date_Days");

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual(expectedBreakdown, viewModel.StrDateDiffResult);
            Assert.AreEqual(expectedDays, viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffResultInPositiveDaysTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[1]; // 30.12.9999
            viewModel.ToDate = s_date[0];   // 31.12.9999

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffAutomationNameUsesLocalizedFormat()
        {
            var viewModel = new DateCalculatorViewModel
            {
                IsDateDiffMode = true,
                FromDate = s_date[1],
                ToDate = s_date[0]
            };

            Assert.AreEqual(
                "Difference " + viewModel.StrDateDiffResult,
                viewModel.StrDateDiffResultAutomationName);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffFromDateHigherThanToDate()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[0]; // 31.12.9999
            viewModel.ToDate = s_date[1];   // 30.12.9999

            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.AreEqual(string.Empty, viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        public void DateCalcViewModelPreservesCalendarDateForOffsetPickerValues()
        {
            var viewModel = new DateCalculatorViewModel
            {
                IsDateDiffMode = true,
                FromDate = new DateTimeOffset(2024, 3, 1, 0, 0, 0, TimeSpan.FromHours(2)),
                ToDate = new DateTimeOffset(2024, 4, 1, 0, 0, 0, TimeSpan.FromHours(2))
            };

            Assert.IsFalse(viewModel.IsDiffInDays);
            Assert.AreEqual("1 month", viewModel.StrDateDiffResult);
        }

        [TestMethod]
        public void DateCalcViewModelAddSubtractResultAutomationNameTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = false;
            viewModel.IsAddMode = true;
            viewModel.StartDate = s_date[13];
            viewModel.DaysOffset = 1;
            viewModel.MonthsOffset = 0;
            viewModel.YearsOffset = 0;

            var automationName = viewModel.StrDateResultAutomationName;

            Assert.AreEqual("Resulting date " + viewModel.StrDateResult, automationName);
        }

        [TestMethod]
        public void JaEraTransitionAddition()
        {
            // Showa ends 1989-01-07 and Heisei begins on the 8th. Date arithmetic must not notice.
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var result = engine.AddDuration(MakeDate(1989, 1, 7), new DateDifference { Day = 1 });

            Assert.IsNotNull(result, "AddDuration returned null across the Showa/Heisei boundary");
            Assert.AreEqual(MakeDate(1989, 1, 8), result.Value);
        }

        [TestMethod]
        public void JaEraTransitionSubtraction()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var result = engine.SubtractDuration(MakeDate(1989, 1, 8), new DateDifference { Day = 1 });

            Assert.IsNotNull(result, "SubtractDuration returned null across the Showa/Heisei boundary");
            Assert.AreEqual(MakeDate(1989, 1, 7), result.Value);
        }

        [TestMethod]
        public void JaEraTransitionDifference()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var showaToHeisei = engine.TryGetDateDifference(
                MakeDate(1989, 1, 7), MakeDate(1989, 1, 8), DateUnit.Day);
            Assert.IsNotNull(showaToHeisei, "TryGetDateDifference returned null across Showa/Heisei");
            Assert.AreEqual(1, showaToHeisei.Value.Day);

            // Heisei ends 2019-04-30 and Reiwa begins on the 1st; the same must hold there.
            var heiseiToReiwa = engine.TryGetDateDifference(
                MakeDate(2019, 4, 30), MakeDate(2019, 5, 1), DateUnit.Day);
            Assert.IsNotNull(heiseiToReiwa, "TryGetDateDifference returned null across Heisei/Reiwa");
            Assert.AreEqual(1, heiseiToReiwa.Value.Day);
        }

        [TestMethod]
        public void JapaneseCalendarSubtractionBelowSupportedRangeReturnsNullAndRecovers()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            var outOfRange = engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 });

            Assert.IsNull(outOfRange);

            Assert.IsNull(engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 }));
        }

        [TestMethod]
        public void UmAlQuraRangeAtCalendarLimitReturnsYearMonthDayDifference()
        {
            var engine = new DateCalculationEngine("UmAlQuraCalendar");

            var difference = engine.TryGetDateDifference(
                MakeDate(2075, 1, 1),
                MakeDate(2077, 11, 16),
                DateUnit.Year | DateUnit.Month | DateUnit.Day);

            Assert.IsNotNull(difference);
            Assert.AreEqual(2, difference.Value.Year);
            Assert.AreEqual(11, difference.Value.Month);
            Assert.AreEqual(16, difference.Value.Day);
        }

        [TestMethod]
        public void JapaneseDifferenceRecoveryPreservesTheCalendarSystem()
        {
            var engine = new DateCalculationEngine("JapaneseCalendar");

            Assert.IsNotNull(engine.TryGetDateDifference(
                MakeDate(1900, 1, 1),
                MakeDate(9998, 12, 31),
                DateUnit.Year | DateUnit.Month | DateUnit.Day));

            Assert.IsNull(engine.SubtractDuration(
                MakeDate(1900, 1, 1),
                new DateDifference { Year = 100 }));
        }
    }
}