// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
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
        [Ignore("TODO - MSFT 10331900, fix this test")]
        public void TestDateDiff()
        {
            // Ported from C++ - originally commented out
            // Tests TryGetDateDifference for various date output formats
        }

        [TestMethod]
        [Ignore("TODO - MSFT 10331900, fix this test")]
        public void TestAddOob()
        {
            // Ported from C++ - originally commented out
            // Tests AddDuration for out-of-bound cases
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
        [Ignore("TODO - MSFT 10331900, fix this test")]
        public void TestAddition()
        {
            // Ported from C++ - originally commented out
            // Tests AddDuration and verifies the resulting date
        }

        [TestMethod]
        [Ignore("TODO - MSFT 10331900, fix this test")]
        public void TestSubtraction()
        {
            // Ported from C++ - originally commented out
            // Tests SubtractDuration and verifies the resulting date
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

            // Check for the initialized values
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
            Assert.IsNull(viewModel.StrDateDiffResultInDays);

            Assert.IsNull(viewModel.StrDateResult);
        }

        [TestMethod]
        public void DateCalcViewModelAddSubtractInitTest()
        {
            var viewModel = new DateCalculatorViewModel();
            viewModel.IsDateDiffMode = false;

            // Check for the initialized values
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
            Assert.IsNull(viewModel.StrDateDiffResultInDays);

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
        [Ignore("TODO - MSFT 10331900, fix this test")]
        public void DateCalcViewModelAddTest()
        {
            // Ported from C++ - originally commented out
            // Tests Add mode with specific dates and verifies StrDateResult
        }

        [TestMethod]
        [Ignore("TODO - MSFT 10331900, fix this test")]
        public void DateCalcViewModelSubtractTest()
        {
            // Ported from C++ - originally commented out
            // Tests Subtract mode with specific dates and verifies StrDateResult
        }

        [TestMethod]
        [Ignore("TODO - MSFT 10331900, fix this test")]
        public void DateCalcViewModelAddOobTest()
        {
            // Ported from C++ - originally commented out
            // Tests Add mode for out-of-bound cases, verifies "Date out of Bound"
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
        [Ignore("TODO - MSFT 10331900, fix this test - viewmodel reduction algorithm issue")]
        public void DateCalcViewModelDateDiffTest()
        {
            // Ported from C++ - originally commented out
            // Tests date diff between 31.01.2008 and 31.12.9999
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffResultInPositiveDaysTest()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[1]; // 30.12.9999
            viewModel.ToDate = s_date[0];   // 31.12.9999

            // Assert for the result
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.IsNull(viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        public void DateCalcViewModelDateDiffFromDateHigherThanToDate()
        {
            var viewModel = new DateCalculatorViewModel();

            viewModel.IsDateDiffMode = true;
            Assert.IsTrue(viewModel.IsDateDiffMode);

            viewModel.FromDate = s_date[0]; // 31.12.9999
            viewModel.ToDate = s_date[1];   // 30.12.9999

            // Assert for the result
            Assert.IsTrue(viewModel.IsDiffInDays);
            Assert.AreEqual("1 day", viewModel.StrDateDiffResult);
            Assert.IsNull(viewModel.StrDateDiffResultInDays);
        }

        [TestMethod]
        [Ignore("Requires UWP APIs (Calendar, DateTimeFormatter)")]
        public void DateCalcViewModelAddSubtractResultAutomationNameTest()
        {
            // Ported from C++ - requires UWP Calendar and DateTimeFormatter APIs
            // Tests that the automation name for the resulting date in Add Mode
            // contains the DayOfWeek, Day, Month, and Year components
        }

        [TestMethod]
        [Ignore("Requires UWP Calendar API")]
        public void JaEraTransitionAddition()
        {
            // Ported from C++ - requires UWP Calendar API for Japanese era date construction
            // Tests addition across Japanese era boundary (Showa ended Jan 1989)
        }

        [TestMethod]
        [Ignore("Requires UWP Calendar API")]
        public void JaEraTransitionSubtraction()
        {
            // Ported from C++ - requires UWP Calendar API for Japanese era date construction
            // Tests subtraction across Japanese era boundary (Showa ended Jan 1989)
        }

        [TestMethod]
        [Ignore("Requires UWP Calendar API")]
        public void JaEraTransitionDifference()
        {
            // Ported from C++ - requires UWP Calendar API for Japanese era date construction
            // Tests date difference across Japanese era boundary (Showa ended Jan 8, 1989)
        }
    }
}
