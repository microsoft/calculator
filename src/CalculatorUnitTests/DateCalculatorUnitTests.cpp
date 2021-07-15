// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include "DateUtils.h"

#include "CalcViewModel/Common/DateCalculator.h"
#include "CalcViewModel/DateCalculatorViewModel.h"

using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace CalculatorApp::ViewModel::Common::DateCalculation;
using namespace CalculatorApp::ViewModel;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DateCalculationUnitTests
{
    const int c_numDate = 15;
    const int c_diffTestCase = 9;
    const int c_numAddOobDate = 2;
    const int c_numSubtractOobDate = 2;
    const int c_addCases = 3;
    const int c_subtractCases = 3;
    const int c_dateDiff = 14;

    typedef struct
    {
        SYSTEMTIME startDate;
        SYSTEMTIME endDate;
        DateDifference dateDiff;
    } DateTimeTestCase;

    SYSTEMTIME date[c_numDate];
    DateDifference dateDifference[c_dateDiff];
    DateTimeTestCase datetimeDifftest[c_diffTestCase];
    DateTimeTestCase datetimeBoundAdd[c_numAddOobDate];
    DateTimeTestCase datetimeBoundSubtract[c_numSubtractOobDate];
    DateTimeTestCase datetimeAddCase[c_addCases];
    DateTimeTestCase datetimeSubtractCase[c_subtractCases];

    // Test Class
    TEST_CLASS(DateCalculatorUnitTests)
    {
        static DateCalculationEngine ^ m_DateCalcEngine;

    public:
        TEST_CLASS_INITIALIZE(TestClassSetup)
        {
            m_DateCalcEngine = ref new DateCalculationEngine(CalendarIdentifiers::Gregorian);

            /* Test Case Data */

            // Dates - DD.MM.YYYY
            /*31.12.9999*/
            date[0].wYear = 9999;
            date[0].wMonth = 12;
            date[0].wDayOfWeek = 5;
            date[0].wDay = 31;
            date[0].wHour = 0;
            date[0].wMinute = 0;
            date[0].wSecond = 0;
            date[0].wMilliseconds = 0;
            /*30.12.9999*/ date[1].wYear = 9999;
            date[1].wMonth = 12;
            date[1].wDayOfWeek = 4;
            date[1].wDay = 30;
            date[1].wHour = 0;
            date[1].wMinute = 0;
            date[1].wSecond = 0;
            date[1].wMilliseconds = 0;
            /*31.12.9998*/ date[2].wYear = 9998;
            date[2].wMonth = 12;
            date[2].wDayOfWeek = 4;
            date[2].wDay = 31;
            date[2].wHour = 0;
            date[2].wMinute = 0;
            date[2].wSecond = 0;
            date[2].wMilliseconds = 0;
            /*01.01.1601*/ date[3].wYear = 1601;
            date[3].wMonth = 1;
            date[3].wDayOfWeek = 1;
            date[3].wDay = 1;
            date[3].wHour = 0;
            date[3].wMinute = 0;
            date[3].wSecond = 0;
            date[3].wMilliseconds = 0;
            /*02.01.1601*/ date[4].wYear = 1601;
            date[4].wMonth = 1;
            date[4].wDayOfWeek = 2;
            date[4].wDay = 2;
            date[4].wHour = 0;
            date[4].wMinute = 0;
            date[4].wSecond = 0;
            date[4].wMilliseconds = 0;
            /*10.05.2008*/ date[5].wYear = 2008;
            date[5].wMonth = 5;
            date[5].wDayOfWeek = 6;
            date[5].wDay = 10;
            date[5].wHour = 0;
            date[5].wMinute = 0;
            date[5].wSecond = 0;
            date[5].wMilliseconds = 0;
            /*10.03.2008*/ date[6].wYear = 2008;
            date[6].wMonth = 3;
            date[6].wDayOfWeek = 1;
            date[6].wDay = 10;
            date[6].wHour = 0;
            date[6].wMinute = 0;
            date[6].wSecond = 0;
            date[6].wMilliseconds = 0;
            /*29.02.2008*/ date[7].wYear = 2008;
            date[7].wMonth = 2;
            date[7].wDayOfWeek = 5;
            date[7].wDay = 29;
            date[7].wHour = 0;
            date[7].wMinute = 0;
            date[7].wSecond = 0;
            date[7].wMilliseconds = 0;
            /*28.02.2007*/ date[8].wYear = 2007;
            date[8].wMonth = 2;
            date[8].wDayOfWeek = 3;
            date[8].wDay = 28;
            date[8].wHour = 0;
            date[8].wMinute = 0;
            date[8].wSecond = 0;
            date[8].wMilliseconds = 0;
            /*10.03.2007*/ date[9].wYear = 2007;
            date[9].wMonth = 3;
            date[9].wDayOfWeek = 6;
            date[9].wDay = 10;
            date[9].wHour = 0;
            date[9].wMinute = 0;
            date[9].wSecond = 0;
            date[9].wMilliseconds = 0;
            /*10.05.2007*/ date[10].wYear = 2007;
            date[10].wMonth = 5;
            date[10].wDayOfWeek = 4;
            date[10].wDay = 10;
            date[10].wHour = 0;
            date[10].wMinute = 0;
            date[10].wSecond = 0;
            date[10].wMilliseconds = 0;
            /*29.01.2008*/ date[11].wYear = 2008;
            date[11].wMonth = 1;
            date[11].wDayOfWeek = 2;
            date[11].wDay = 29;
            date[11].wHour = 0;
            date[11].wMinute = 0;
            date[11].wSecond = 0;
            date[11].wMilliseconds = 0;
            /*28.01.2007*/ date[12].wYear = 2007;
            date[12].wMonth = 1;
            date[12].wDayOfWeek = 0;
            date[12].wDay = 28;
            date[12].wHour = 0;
            date[12].wMinute = 0;
            date[12].wSecond = 0;
            date[12].wMilliseconds = 0;
            /*31.01.2008*/ date[13].wYear = 2008;
            date[13].wMonth = 1;
            date[13].wDayOfWeek = 4;
            date[13].wDay = 31;
            date[13].wHour = 0;
            date[13].wMinute = 0;
            date[13].wSecond = 0;
            date[13].wMilliseconds = 0;
            /*31.03.2008*/ date[14].wYear = 2008;
            date[14].wMonth = 3;
            date[14].wDayOfWeek = 1;
            date[14].wDay = 31;
            date[14].wHour = 0;
            date[14].wMinute = 0;
            date[14].wSecond = 0;
            date[14].wMilliseconds = 0;

            // Date Differences
            dateDifference[0].year = 1;
            dateDifference[0].month = 1;
            dateDifference[1].month = 1;
            dateDifference[1].day = 10;
            dateDifference[2].day = 2;
            /*date[2]-[0]*/ dateDifference[3].week = 52;
            dateDifference[3].day = 1;
            /*date[2]-[0]*/ dateDifference[4].year = 1;
            dateDifference[5].day = 365;
            dateDifference[6].month = 1;
            dateDifference[7].month = 1;
            dateDifference[7].day = 2;
            dateDifference[8].day = 31;
            dateDifference[9].month = 11;
            dateDifference[9].day = 1;
            dateDifference[10].year = 8398;
            dateDifference[10].month = 11;
            dateDifference[10].day = 30;
            dateDifference[11].year = 2008;
            dateDifference[12].year = 7991;
            dateDifference[12].month = 11;
            dateDifference[13].week = 416998;
            dateDifference[13].day = 1;

            /* Test Cases */

            // Date Difference test cases
            datetimeDifftest[0].startDate = date[0];
            datetimeDifftest[0].endDate = date[3];
            datetimeDifftest[0].dateDiff = dateDifference[10];
            datetimeDifftest[1].startDate = date[0];
            datetimeDifftest[1].endDate = date[2];
            datetimeDifftest[1].dateDiff = dateDifference[5];
            datetimeDifftest[2].startDate = date[0];
            datetimeDifftest[2].endDate = date[2];
            datetimeDifftest[2].dateDiff = dateDifference[4];
            datetimeDifftest[3].startDate = date[0];
            datetimeDifftest[3].endDate = date[2];
            datetimeDifftest[3].dateDiff = dateDifference[3];
            datetimeDifftest[4].startDate = date[14];
            datetimeDifftest[4].endDate = date[7];
            datetimeDifftest[4].dateDiff = dateDifference[7];
            datetimeDifftest[5].startDate = date[14];
            datetimeDifftest[5].endDate = date[7];
            datetimeDifftest[5].dateDiff = dateDifference[8];
            datetimeDifftest[6].startDate = date[11];
            datetimeDifftest[6].endDate = date[8];
            datetimeDifftest[6].dateDiff = dateDifference[9];
            datetimeDifftest[7].startDate = date[13];
            datetimeDifftest[7].endDate = date[0];
            datetimeDifftest[7].dateDiff = dateDifference[12];
            datetimeDifftest[8].startDate = date[13];
            datetimeDifftest[8].endDate = date[0];
            datetimeDifftest[8].dateDiff = dateDifference[13];

            // Date Add Out of Bound test cases (Negative tests)
            /*OutofBound*/ datetimeBoundAdd[0].startDate = date[1];
            datetimeBoundAdd[0].endDate = date[0];
            datetimeBoundAdd[0].dateDiff = dateDifference[2]; // on Add date[0] not used
            /*OutofBound*/ datetimeBoundAdd[1].startDate = date[2];
            datetimeBoundAdd[1].endDate = date[0];
            datetimeBoundAdd[1].dateDiff = dateDifference[11]; // on Add date[0] not used

            // Date Subtract Out of Bound test cases (Negative tests)
            /*OutofBound*/ datetimeBoundSubtract[0].startDate = date[3];
            datetimeBoundSubtract[0].endDate = date[0];
            datetimeBoundSubtract[0].dateDiff = dateDifference[2]; // on subtract date[0] not used
            /*OutofBound*/ datetimeBoundSubtract[1].startDate = date[14];
            datetimeBoundSubtract[1].endDate = date[0];
            datetimeBoundSubtract[1].dateDiff = dateDifference[11]; // on subtract date[0] not used

            // Date Add test cases (Positive tests)
            datetimeAddCase[0].startDate = date[13];
            datetimeAddCase[0].endDate = date[7];
            datetimeAddCase[0].dateDiff = dateDifference[6]; // add
            datetimeAddCase[1].startDate = date[14];
            datetimeAddCase[1].endDate = date[5];
            datetimeAddCase[1].dateDiff = dateDifference[1]; // add
            datetimeAddCase[2].startDate = date[13];
            datetimeAddCase[2].endDate = date[6];
            datetimeAddCase[2].dateDiff = dateDifference[1]; // add

            // Date Subtract test cases (Positive tests)
            datetimeSubtractCase[0].startDate = date[14];
            datetimeSubtractCase[0].endDate = date[7];
            datetimeSubtractCase[0].dateDiff = dateDifference[6]; // subtract
            datetimeSubtractCase[1].startDate = date[6];
            datetimeSubtractCase[1].endDate = date[11];
            datetimeSubtractCase[1].dateDiff = dateDifference[1]; // subtract
            datetimeSubtractCase[2].startDate = date[9];
            datetimeSubtractCase[2].endDate = date[12];
            datetimeSubtractCase[2].dateDiff = dateDifference[1]; // subtract
        }

        /* Duration Between Two Date Tests -- Timediff obtained after calculation should be checked to be identical */
        TEST_METHOD(TestDateDiff)
        {
            // TODO - MSFT 10331900, fix this test

            // for (int testIndex = 0; testIndex < c_diffTestCase; testIndex++)
            //{
            //    DateDifference diff;
            //    DateUnit dateOutputFormat;

            //    switch (testIndex)
            //    {
            //    case 0:
            //    case 2:
            //        dateOutputFormat = DateUnit::Year | DateUnit::Month | DateUnit::Day;
            //        break;
            //    case 1:
            //        dateOutputFormat = DateUnit::Day;
            //        break;
            //    case 3:
            //    case 8:
            //        dateOutputFormat = DateUnit::Week | DateUnit::Day;
            //        break;
            //    case 7:
            //        dateOutputFormat = DateUnit::Year | DateUnit::Month | DateUnit::Day;
            //        break;
            //    case 4:
            //    case 6:
            //        dateOutputFormat = DateUnit::Month | DateUnit::Day;
            //        break;
            //    case 5:
            //        dateOutputFormat = DateUnit::Day;
            //        break;
            //    }

            //    // Calculate the difference
            //    m_DateCalcEngine->TryGetDateDifference(DateUtils::SystemTimeToDateTime(datetimeDifftest[testIndex].startDate),
            //    DateUtils::SystemTimeToDateTime(datetimeDifftest[testIndex].endDate), dateOutputFormat, &diff);

            //    // Assert for the result
            //    bool areIdentical = true;
            //    if (diff.year != datetimeDifftest[testIndex].dateDiff.year ||
            //        diff.month != datetimeDifftest[testIndex].dateDiff.month ||
            //        diff.week != datetimeDifftest[testIndex].dateDiff.week ||
            //        diff.day != datetimeDifftest[testIndex].dateDiff.day)
            //    {
            //        areIdentical = false;
            //    }

            //    VERIFY_IS_TRUE(areIdentical);
            //}
        }

        /*Add Out of bound Tests*/
        TEST_METHOD(TestAddOob)
        {
            // TODO - MSFT 10331900, fix this test

            // for (int testIndex = 0; testIndex< c_numAddOobDate; testIndex++)
            //{
            //    DateTime endDate;

            //    // Add Duration
            //    bool isValid = m_DateCalcEngine->AddDuration(DateUtils::SystemTimeToDateTime(datetimeBoundAdd[testIndex].startDate),
            //    datetimeBoundAdd[testIndex].dateDiff, &endDate);

            //    // Assert for the result
            //    VERIFY_IS_FALSE(isValid);
            //}
        }

        /*Subtract Out of bound Tests*/
        TEST_METHOD(TestSubtractOob)
        {
            for (int testIndex = 0; testIndex < c_numSubtractOobDate; testIndex++)
            {
                // Subtract Duration
                auto endDate = m_DateCalcEngine->SubtractDuration(
                    DateUtils::SystemTimeToDateTime(datetimeBoundSubtract[testIndex].startDate), datetimeBoundSubtract[testIndex].dateDiff);

                // Assert for the result
                VERIFY_IS_NULL(endDate);
            }
        }

        // Add Tests
        TEST_METHOD(TestAddition)
        {
            // TODO - MSFT 10331900, fix this test

            // for (int testIndex = 0; testIndex < c_addCases; testIndex++)
            //{
            //    DateTime endDate;

            //    // Add Duration
            //    bool isValid = m_DateCalcEngine->AddDuration(DateUtils::SystemTimeToDateTime(datetimeAddCase[testIndex].startDate),
            //    datetimeAddCase[testIndex].dateDiff, &endDate);

            //    // Assert for the result
            //    VERIFY_IS_TRUE(isValid);

            //    SYSTEMTIME systemTime = DateUtils::DateTimeToSystemTime(endDate);
            //    if (systemTime.wYear != datetimeAddCase[testIndex].endDate.wYear ||
            //        systemTime.wMonth != datetimeAddCase[testIndex].endDate.wMonth ||
            //        systemTime.wDay != datetimeAddCase[testIndex].endDate.wDay ||
            //        systemTime.wDayOfWeek != datetimeAddCase[testIndex].endDate.wDayOfWeek)
            //    {
            //        isValid = false;
            //    }

            //    VERIFY_IS_TRUE(isValid);
            //}
        }

        // Subtract Tests
        TEST_METHOD(TestSubtraction)
        {
            // TODO - MSFT 10331900, fix this test

            // for (int testIndex = 0; testIndex < c_subtractCases; testIndex++)
            //{
            //    DateTime endDate;

            //    // Subtract Duration
            //    bool isValid = m_DateCalcEngine->SubtractDuration(DateUtils::SystemTimeToDateTime(datetimeSubtractCase[testIndex].startDate),
            //    datetimeSubtractCase[testIndex].dateDiff, &endDate);

            //    // assert for the result
            //    VERIFY_IS_TRUE(isValid);

            //    SYSTEMTIME systemTime = DateUtils::DateTimeToSystemTime(endDate);
            //    if (systemTime.wYear != datetimeSubtractCase[testIndex].endDate.wYear ||
            //        systemTime.wMonth != datetimeSubtractCase[testIndex].endDate.wMonth ||
            //        systemTime.wDay != datetimeSubtractCase[testIndex].endDate.wDay ||
            //        systemTime.wDayOfWeek != datetimeSubtractCase[testIndex].endDate.wDayOfWeek)
            //    {
            //        isValid = false;
            //    }

            //    VERIFY_IS_TRUE(isValid);
            //}
        }
    };

    TEST_CLASS(DateCalculatorViewModelTests){ public: TEST_CLASS_INITIALIZE(TestClassSetup){ /* Test Case Data */
                                                                                             // Dates - DD.MM.YYYY
                                                                                             /*31.12.9999*/ date[0].wYear = 9999;
    date[0].wMonth = 12;
    date[0].wDayOfWeek = 5;
    date[0].wDay = 31;
    date[0].wHour = 0;
    date[0].wMinute = 0;
    date[0].wSecond = 0;
    date[0].wMilliseconds = 0;
    /*30.12.9999*/ date[1].wYear = 9999;
    date[1].wMonth = 12;
    date[1].wDayOfWeek = 4;
    date[1].wDay = 30;
    date[1].wHour = 0;
    date[1].wMinute = 0;
    date[1].wSecond = 0;
    date[1].wMilliseconds = 0;
    /*31.12.9998*/ date[2].wYear = 9998;
    date[2].wMonth = 12;
    date[2].wDayOfWeek = 4;
    date[2].wDay = 31;
    date[2].wHour = 0;
    date[2].wMinute = 0;
    date[2].wSecond = 0;
    date[2].wMilliseconds = 0;
    /*01.01.1601*/ date[3].wYear = 1601;
    date[3].wMonth = 1;
    date[3].wDayOfWeek = 1;
    date[3].wDay = 1;
    date[3].wHour = 0;
    date[3].wMinute = 0;
    date[3].wSecond = 0;
    date[3].wMilliseconds = 0;
    /*02.01.1601*/ date[4].wYear = 1601;
    date[4].wMonth = 1;
    date[4].wDayOfWeek = 2;
    date[4].wDay = 2;
    date[4].wHour = 0;
    date[4].wMinute = 0;
    date[4].wSecond = 0;
    date[4].wMilliseconds = 0;
    /*10.05.2008*/ date[5].wYear = 2008;
    date[5].wMonth = 5;
    date[5].wDayOfWeek = 6;
    date[5].wDay = 10;
    date[5].wHour = 0;
    date[5].wMinute = 0;
    date[5].wSecond = 0;
    date[5].wMilliseconds = 0;
    /*10.03.2008*/ date[6].wYear = 2008;
    date[6].wMonth = 3;
    date[6].wDayOfWeek = 1;
    date[6].wDay = 10;
    date[6].wHour = 0;
    date[6].wMinute = 0;
    date[6].wSecond = 0;
    date[6].wMilliseconds = 0;
    /*29.02.2008*/ date[7].wYear = 2008;
    date[7].wMonth = 2;
    date[7].wDayOfWeek = 5;
    date[7].wDay = 29;
    date[7].wHour = 0;
    date[7].wMinute = 0;
    date[7].wSecond = 0;
    date[7].wMilliseconds = 0;
    /*28.02.2007*/ date[8].wYear = 2007;
    date[8].wMonth = 2;
    date[8].wDayOfWeek = 3;
    date[8].wDay = 28;
    date[8].wHour = 0;
    date[8].wMinute = 0;
    date[8].wSecond = 0;
    date[8].wMilliseconds = 0;
    /*10.03.2007*/ date[9].wYear = 2007;
    date[9].wMonth = 3;
    date[9].wDayOfWeek = 6;
    date[9].wDay = 10;
    date[9].wHour = 0;
    date[9].wMinute = 0;
    date[9].wSecond = 0;
    date[9].wMilliseconds = 0;
    /*10.05.2007*/ date[10].wYear = 2007;
    date[10].wMonth = 5;
    date[10].wDayOfWeek = 4;
    date[10].wDay = 10;
    date[10].wHour = 0;
    date[10].wMinute = 0;
    date[10].wSecond = 0;
    date[10].wMilliseconds = 0;
    /*29.01.2008*/ date[11].wYear = 2008;
    date[11].wMonth = 1;
    date[11].wDayOfWeek = 2;
    date[11].wDay = 29;
    date[11].wHour = 0;
    date[11].wMinute = 0;
    date[11].wSecond = 0;
    date[11].wMilliseconds = 0;
    /*28.01.2007*/ date[12].wYear = 2007;
    date[12].wMonth = 1;
    date[12].wDayOfWeek = 0;
    date[12].wDay = 28;
    date[12].wHour = 0;
    date[12].wMinute = 0;
    date[12].wSecond = 0;
    date[12].wMilliseconds = 0;
    /*31.01.2008*/ date[13].wYear = 2008;
    date[13].wMonth = 1;
    date[13].wDayOfWeek = 4;
    date[13].wDay = 31;
    date[13].wHour = 0;
    date[13].wMinute = 0;
    date[13].wSecond = 0;
    date[13].wMilliseconds = 0;
    /*31.03.2008*/ date[14].wYear = 2008;
    date[14].wMonth = 3;
    date[14].wDayOfWeek = 1;
    date[14].wDay = 31;
    date[14].wHour = 0;
    date[14].wMinute = 0;
    date[14].wSecond = 0;
    date[14].wMilliseconds = 0;

    // Date Differences
    dateDifference[0].year = 1;
    dateDifference[0].month = 1;
    dateDifference[1].month = 1;
    dateDifference[1].day = 10;
    dateDifference[2].day = 2;
    /*date[2]-[0]*/ dateDifference[3].week = 52;
    dateDifference[3].day = 1;
    /*date[2]-[0]*/ dateDifference[4].year = 1;
    dateDifference[5].day = 365;
    dateDifference[6].month = 1;
    dateDifference[7].month = 1;
    dateDifference[7].day = 2;
    dateDifference[8].day = 31;
    dateDifference[9].month = 11;
    dateDifference[9].day = 1;
    dateDifference[10].year = 8398;
    dateDifference[10].month = 11;
    dateDifference[10].day = 30;
    dateDifference[11].year = 2008;
    dateDifference[12].year = 7991;
    dateDifference[12].month = 11;
    dateDifference[13].week = 416998;
    dateDifference[13].day = 1;

    /* Test Cases */

    // Date Difference test cases
    datetimeDifftest[0].startDate = date[0];
    datetimeDifftest[0].endDate = date[3];
    datetimeDifftest[0].dateDiff = dateDifference[10];
    datetimeDifftest[1].startDate = date[0];
    datetimeDifftest[1].endDate = date[2];
    datetimeDifftest[1].dateDiff = dateDifference[5];
    datetimeDifftest[2].startDate = date[0];
    datetimeDifftest[2].endDate = date[2];
    datetimeDifftest[2].dateDiff = dateDifference[4];
    datetimeDifftest[3].startDate = date[0];
    datetimeDifftest[3].endDate = date[2];
    datetimeDifftest[3].dateDiff = dateDifference[3];
    datetimeDifftest[4].startDate = date[14];
    datetimeDifftest[4].endDate = date[7];
    datetimeDifftest[4].dateDiff = dateDifference[7];
    datetimeDifftest[5].startDate = date[14];
    datetimeDifftest[5].endDate = date[7];
    datetimeDifftest[5].dateDiff = dateDifference[8];
    datetimeDifftest[6].startDate = date[11];
    datetimeDifftest[6].endDate = date[8];
    datetimeDifftest[6].dateDiff = dateDifference[9];
    datetimeDifftest[7].startDate = date[13];
    datetimeDifftest[7].endDate = date[0];
    datetimeDifftest[7].dateDiff = dateDifference[12];
    datetimeDifftest[8].startDate = date[13];
    datetimeDifftest[8].endDate = date[0];
    datetimeDifftest[8].dateDiff = dateDifference[13];

    // Date Add Out of Bound test cases (Negative tests)
    /*OutofBound*/ datetimeBoundAdd[0].startDate = date[1];
    datetimeBoundAdd[0].endDate = date[0];
    datetimeBoundAdd[0].dateDiff = dateDifference[2]; // on Add date[0] not used
    /*OutofBound*/ datetimeBoundAdd[1].startDate = date[2];
    datetimeBoundAdd[1].endDate = date[0];
    datetimeBoundAdd[1].dateDiff = dateDifference[11]; // on Add date[0] not used

    // Date Subtract Out of Bound test cases (Negative tests)
    /*OutofBound*/ datetimeBoundSubtract[0].startDate = date[3];
    datetimeBoundSubtract[0].endDate = date[0];
    datetimeBoundSubtract[0].dateDiff = dateDifference[2]; // on subtract date[0] not used
    /*OutofBound*/ datetimeBoundSubtract[1].startDate = date[14];
    datetimeBoundSubtract[1].endDate = date[0];
    datetimeBoundSubtract[1].dateDiff = dateDifference[11]; // on subtract date[0] not used

    // Date Add test cases (Positive tests)
    datetimeAddCase[0].startDate = date[13];
    datetimeAddCase[0].endDate = date[7];
    datetimeAddCase[0].dateDiff = dateDifference[6]; // add
    datetimeAddCase[1].startDate = date[14];
    datetimeAddCase[1].endDate = date[5];
    datetimeAddCase[1].dateDiff = dateDifference[1]; // add
    datetimeAddCase[2].startDate = date[13];
    datetimeAddCase[2].endDate = date[6];
    datetimeAddCase[2].dateDiff = dateDifference[1]; // add

    // Date Subtract test cases (Positive tests)
    datetimeSubtractCase[0].startDate = date[14];
    datetimeSubtractCase[0].endDate = date[7];
    datetimeSubtractCase[0].dateDiff = dateDifference[6]; // subtract
    datetimeSubtractCase[1].startDate = date[6];
    datetimeSubtractCase[1].endDate = date[11];
    datetimeSubtractCase[1].dateDiff = dateDifference[1]; // subtract
    datetimeSubtractCase[2].startDate = date[9];
    datetimeSubtractCase[2].endDate = date[12];
    datetimeSubtractCase[2].dateDiff = dateDifference[1]; // subtract
}

TEST_METHOD(DateCalcViewModelInitializationTest)
{
    auto viewModel = ref new DateCalculatorViewModel();

    // Check for the initialized values
    VERIFY_IS_TRUE(viewModel->IsDateDiffMode);
    VERIFY_IS_TRUE(viewModel->IsAddMode);

    VERIFY_IS_TRUE(0 != viewModel->FromDate.UniversalTime);
    VERIFY_IS_TRUE(0 != viewModel->ToDate.UniversalTime);
    VERIFY_IS_TRUE(0 != viewModel->StartDate.UniversalTime);

    VERIFY_ARE_EQUAL(0, viewModel->DaysOffset);
    VERIFY_ARE_EQUAL(0, viewModel->MonthsOffset);
    VERIFY_ARE_EQUAL(0, viewModel->YearsOffset);

    VERIFY_IS_TRUE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"Same dates"), viewModel->StrDateDiffResult);
    VERIFY_IS_NULL(viewModel->StrDateDiffResultInDays);

    VERIFY_IS_NULL(viewModel->StrDateResult);
}

TEST_METHOD(DateCalcViewModelAddSubtractInitTest)
{
    auto viewModel = ref new DateCalculatorViewModel();
    viewModel->IsDateDiffMode = false;

    // Check for the initialized values
    VERIFY_IS_FALSE(viewModel->IsDateDiffMode);
    VERIFY_IS_TRUE(viewModel->IsAddMode);

    VERIFY_IS_TRUE(0 != viewModel->FromDate.UniversalTime);
    VERIFY_IS_TRUE(0 != viewModel->ToDate.UniversalTime);
    VERIFY_IS_TRUE(0 != viewModel->StartDate.UniversalTime);

    VERIFY_ARE_EQUAL(0, viewModel->DaysOffset);
    VERIFY_ARE_EQUAL(0, viewModel->MonthsOffset);
    VERIFY_ARE_EQUAL(0, viewModel->YearsOffset);

    VERIFY_IS_TRUE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"Same dates"), viewModel->StrDateDiffResult);
    VERIFY_IS_NULL(viewModel->StrDateDiffResultInDays);

    VERIFY_IS_NOT_NULL(viewModel->StrDateResult);
    VERIFY_IS_TRUE(StringReference(L"") != viewModel->StrDateResult);
}

TEST_METHOD(DateCalcViewModelDateDiffDaylightSavingTimeTest)
{
    auto viewModel = ref new DateCalculatorViewModel();
    viewModel->IsDateDiffMode = true;
    VERIFY_IS_TRUE(viewModel->IsDateDiffMode);

    // 29.02.2008
    viewModel->FromDate = DateUtils::SystemTimeToDateTime(datetimeDifftest[5].startDate);
    // 31.03.2008
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(datetimeDifftest[5].endDate);

    //// Assert for the result
    VERIFY_IS_FALSE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"31 days"), viewModel->StrDateDiffResultInDays);
    VERIFY_ARE_EQUAL(StringReference(L"1 month, 2 days"), viewModel->StrDateDiffResult);

    // Daylight Saving Time - Clock Forward
    // 10.03.2019
    SYSTEMTIME startDate;
    startDate.wYear = 2019;
    startDate.wMonth = 03;
    startDate.wDay = 10;
    startDate.wDayOfWeek = 0;
    startDate.wHour = startDate.wMinute = 0;
    startDate.wSecond = startDate.wMilliseconds = 0;
    viewModel->FromDate = DateUtils::SystemTimeToDateTime(startDate);
    // 11.03.2019
    SYSTEMTIME endDate;
    endDate.wYear = 2019;
    endDate.wMonth = 03;
    endDate.wDay = 11;
    endDate.wDayOfWeek = 0;
    endDate.wHour = endDate.wMinute = 0;
    endDate.wSecond = endDate.wMilliseconds = 0;
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(endDate);
    VERIFY_IS_TRUE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"1 day"), viewModel->StrDateDiffResult);

    endDate.wDay += 6;
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(endDate);
    VERIFY_IS_FALSE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"1 week"), viewModel->StrDateDiffResult);

    // Daylight Saving Time - Clock Backward
    // 03.11.2019
    startDate.wMonth = 11;
    startDate.wDay = 03;
    viewModel->FromDate = DateUtils::SystemTimeToDateTime(startDate);
    // 04.11.2019
    endDate.wMonth = 11;
    endDate.wDay = 04;
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(endDate);
    VERIFY_IS_TRUE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"1 day"), viewModel->StrDateDiffResult);
}

TEST_METHOD(DateCalcViewModelAddTest)
{
    // TODO - MSFT 10331900, fix this test
    // A few issues to be investigated..
    //     The date returned by DateUtils::GetLongDate can be a different string than expected
    //         based on the values of date[7].  This is because date[7] is in UTC but GetLongDate
    //         doesn't format according to UTC.  If it did, the test would still be incorrect because
    //         the ViewModel is not necessarily in UTC.
    //
    //     The DateTime value assigned to StartDate after the conversion SystemTimeToDateTime is not
    //         the same DateTime value as if the user were to select the same date from the CalendarDatePicker.
    //         This means testing a specific date here, is *not* the same as selecting that date in the app.

    // auto viewModel = ref new DateCalculatorViewModel();
    // viewModel->Initialize();

    // viewModel->IsDateDiffMode = false;
    // viewModel->IsAddMode = true;
    // VERIFY_IS_FALSE(viewModel->IsDateDiffMode);
    // VERIFY_IS_TRUE(viewModel->IsAddMode);

    // viewModel->StartDate = DateUtils::SystemTimeToDateTime(datetimeAddCase[0].startDate);
    // viewModel->DaysOffset = datetimeAddCase[0].dateDiff.day;
    // viewModel->MonthsOffset = datetimeAddCase[0].dateDiff.month;
    // viewModel->YearsOffset = datetimeAddCase[0].dateDiff.year;

    //// Assert for the result
    // VERIFY_ARE_EQUAL(DateUtils::GetLongDate(date[7]), viewModel->StrDateResult);
}

TEST_METHOD(DateCalcViewModelSubtractTest)
{
    // TODO - MSFT 10331900, fix this test
    // A few issues to be investigated..
    //     The date returned by DateUtils::GetLongDate can be a different string than expected
    //         based on the values of date[7].  This is because date[7] is in UTC but GetLongDate
    //         doesn't format according to UTC.  If it did, the test would still be incorrect because
    //         the ViewModel is not necessarily in UTC.
    //
    //     The DateTime value assigned to StartDate after the conversion SystemTimeToDateTime is not
    //         the same DateTime value as if the user were to select the same date from the CalendarDatePicker.
    //         This means testing a specific date here, is *not* the same as selecting that date in the app.

    // auto viewModel = ref new DateCalculatorViewModel();
    // viewModel->Initialize();

    // viewModel->IsDateDiffMode = false;
    // viewModel->IsAddMode = false;
    // VERIFY_IS_FALSE(viewModel->IsDateDiffMode);
    // VERIFY_IS_FALSE(viewModel->IsAddMode);

    // viewModel->StartDate = DateUtils::SystemTimeToDateTime(datetimeSubtractCase[0].startDate);
    // viewModel->DaysOffset = datetimeSubtractCase[0].dateDiff.day;
    // viewModel->MonthsOffset = datetimeSubtractCase[0].dateDiff.month;
    // viewModel->YearsOffset = datetimeSubtractCase[0].dateDiff.year;

    //// Assert for the result
    // VERIFY_ARE_EQUAL(DateUtils::GetLongDate(date[7]), viewModel->StrDateResult);
}

TEST_METHOD(DateCalcViewModelAddOobTest)
{
    // TODO - MSFT 10331900, fix this test
    // Curiously enough, this test fails because it fails to go Oob.
    // Possibly need to update test to use a new max date.

    // auto viewModel = ref new DateCalculatorViewModel();
    // viewModel->Initialize();

    // viewModel->IsDateDiffMode = false;
    // viewModel->IsAddMode = true;
    // VERIFY_IS_FALSE(viewModel->IsDateDiffMode);
    // VERIFY_IS_TRUE(viewModel->IsAddMode);

    // for (int testIndex = 0; testIndex< c_numAddOobDate; testIndex++)
    //{
    //    viewModel->StartDate = DateUtils::SystemTimeToDateTime(datetimeBoundAdd[testIndex].startDate);
    //    viewModel->DaysOffset = datetimeBoundAdd[testIndex].dateDiff.day;
    //    viewModel->MonthsOffset = datetimeBoundAdd[testIndex].dateDiff.month;
    //    viewModel->YearsOffset = datetimeBoundAdd[testIndex].dateDiff.year;

    //    // Assert for the result
    //    VERIFY_ARE_EQUAL(StringReference(L"Date out of Bound"), viewModel->StrDateResult);
    //}
}

TEST_METHOD(DateCalcViewModelSubtractOobTest)
{
    auto viewModel = ref new DateCalculatorViewModel();

    viewModel->IsDateDiffMode = false;
    viewModel->IsAddMode = false;
    VERIFY_IS_FALSE(viewModel->IsDateDiffMode);
    VERIFY_IS_FALSE(viewModel->IsAddMode);

    for (int testIndex = 0; testIndex < c_numSubtractOobDate; testIndex++)
    {
        viewModel->StartDate = DateUtils::SystemTimeToDateTime(datetimeBoundSubtract[testIndex].startDate);
        viewModel->DaysOffset = datetimeBoundSubtract[testIndex].dateDiff.day;
        viewModel->MonthsOffset = datetimeBoundSubtract[testIndex].dateDiff.month;
        viewModel->YearsOffset = datetimeBoundSubtract[testIndex].dateDiff.year;

        // Assert for the result
        VERIFY_ARE_EQUAL(StringReference(L"Date out of Bound"), viewModel->StrDateResult);
    }
}

TEST_METHOD(DateCalcViewModelDateDiffIgnoreSignTest)
{
    auto viewModel = ref new DateCalculatorViewModel();

    viewModel->IsDateDiffMode = true;
    VERIFY_IS_TRUE(viewModel->IsDateDiffMode);

    viewModel->FromDate = DateUtils::SystemTimeToDateTime(date[10]);
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(date[6]);

    VERIFY_IS_FALSE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"305 days"), viewModel->StrDateDiffResultInDays);
    VERIFY_ARE_EQUAL(StringReference(L"10 months"), viewModel->StrDateDiffResult);
    viewModel->FromDate = DateUtils::SystemTimeToDateTime(date[6]);
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(date[10]);
    VERIFY_IS_FALSE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"305 days"), viewModel->StrDateDiffResultInDays);
    VERIFY_ARE_EQUAL(StringReference(L"10 months"), viewModel->StrDateDiffResult);
}

TEST_METHOD(DateCalcViewModelDateDiffTest)
{
    // TODO - MSFT 10331900, fix this test
    // The last VERIFY checks with expected value "8398 years, 11 months, 4 weeks, 2 days"
    // The viewmodel result is something like "8398 years, 12 months, 6568892 weeks, 1 day",
    // which shows there is a problem with the viewmodel's reduction algorithm.

    // auto viewModel = ref new DateCalculatorViewModel();
    // viewModel->Initialize();

    // viewModel->IsDateDiffMode = true;
    // VERIFY_IS_TRUE(viewModel->IsDateDiffMode);

    // viewModel->FromDate = DateUtils::SystemTimeToDateTime(datetimeDifftest[0].startDate);
    // viewModel->ToDate = DateUtils::SystemTimeToDateTime(datetimeDifftest[0].endDate);

    //// Assert for the result
    // VERIFY_IS_FALSE(viewModel->IsDiffInDays);
    // VERIFY_ARE_EQUAL(StringReference(L"3067670 days"), viewModel->StrDateDiffResultInDays);
    // VERIFY_ARE_EQUAL(StringReference(L"8398 years, 11 months, 4 weeks, 2 days"), viewModel->StrDateDiffResult);
}

TEST_METHOD(DateCalcViewModelDateDiffResultInPositiveDaysTest)
{
    auto viewModel = ref new DateCalculatorViewModel();

    viewModel->IsDateDiffMode = true;
    VERIFY_IS_TRUE(viewModel->IsDateDiffMode);

    viewModel->FromDate = DateUtils::SystemTimeToDateTime(date[1]);
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(date[0]);

    // Assert for the result
    VERIFY_IS_TRUE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"1 day"), viewModel->StrDateDiffResult);
    VERIFY_IS_NULL(viewModel->StrDateDiffResultInDays);
}

TEST_METHOD(DateCalcViewModelDateDiffFromDateHigherThanToDate)
{
    auto viewModel = ref new DateCalculatorViewModel();

    viewModel->IsDateDiffMode = true;
    VERIFY_IS_TRUE(viewModel->IsDateDiffMode);

    viewModel->FromDate = DateUtils::SystemTimeToDateTime(date[0]);
    viewModel->ToDate = DateUtils::SystemTimeToDateTime(date[1]);

    // Assert for the result
    VERIFY_IS_TRUE(viewModel->IsDiffInDays);
    VERIFY_ARE_EQUAL(StringReference(L"1 day"), viewModel->StrDateDiffResult);
    VERIFY_IS_NULL(viewModel->StrDateDiffResultInDays);
}

// Tests that the automation name for the resulting date in Add Mode
// contains the DayOfWeek, Day, Month, and Year
TEST_METHOD(DateCalcViewModelAddSubtractResultAutomationNameTest)
{
    auto cal = ref new Calendar();
    cal->Year = 2007;
    cal->Month = 5;
    cal->Day = 10;
    cal->Hour = 12;
    cal->Period = 2;
    cal->Nanosecond = 0;
    cal->Second = 0;

    DateTime startDate = cal->GetDateTime();
    auto viewModel = ref new DateCalculatorViewModel();

    viewModel->StartDate = startDate;

    viewModel->IsDateDiffMode = false;
    viewModel->IsAddMode = true;

    wstring actualValue = viewModel->StrDateResultAutomationName->Data();

    // Verify each component is present in the result
    wstring components[] = { L"dayofweek.full", L"month.full", L"year.full", L"day" };

    for (const wstring& component : components)
    {
        auto formatter = ref new DateTimeFormatter(ref new String(component.c_str()));
        wstring expectedValue = formatter->Format(startDate)->Data();
        wstring message = L"Verifying " + component + L" is present in the result";
        VERIFY_IS_TRUE(actualValue.find(expectedValue) != wstring::npos, message.c_str());
    }
}

TEST_METHOD(JaEraTransitionAddition)
{
    auto engine = ref new DateCalculationEngine(CalendarIdentifiers::Japanese);
    auto cal = ref new Calendar();

    // The Showa period ended in Jan 1989.
    cal->Year = 1989;
    cal->Month = 1;
    cal->Day = 1;
    auto startTime = cal->GetDateTime();

    cal->Year = 1990;
    cal->Month = 1;
    cal->Day = 1;

    // Expect that adding a year across boundaries adds the equivalent in the Gregorian calendar.
    auto expectedYearResult = cal->GetDateTime();
    DateDifference yearDuration;
    yearDuration.year = 1;

    auto actualYearResult = engine->AddDuration(startTime, yearDuration);

    VERIFY_ARE_EQUAL(expectedYearResult.UniversalTime, actualYearResult->Value.UniversalTime);

    cal->Year = 1989;
    cal->Month = 2;
    cal->Day = 1;

    // Expect that adding a month across boundaries adds the equivalent in the Gregorian calendar.
    auto expectedMonthResult = cal->GetDateTime();
    DateDifference monthDuration;
    monthDuration.month = 1;

    auto actualMonthResult = engine->AddDuration(startTime, monthDuration);

    VERIFY_ARE_EQUAL(expectedMonthResult.UniversalTime, actualMonthResult->Value.UniversalTime);
}

TEST_METHOD(JaEraTransitionSubtraction)
{
    auto engine = ref new DateCalculationEngine(CalendarIdentifiers::Japanese);
    auto cal = ref new Calendar();

    // The Showa period ended in Jan 1989.
    cal->Year = 1989;
    cal->Month = 2;
    cal->Day = 1;
    auto startTime = cal->GetDateTime();

    cal->Year = 1988;
    cal->Month = 2;
    cal->Day = 1;

    // Expect that adding a year across boundaries adds the equivalent in the Gregorian calendar.
    auto expectedYearResult = cal->GetDateTime();
    DateDifference yearDuration;
    yearDuration.year = 1;

    auto actualYearResult = engine->SubtractDuration(startTime, yearDuration);

    VERIFY_ARE_EQUAL(expectedYearResult.UniversalTime, actualYearResult->Value.UniversalTime);

    cal->Year = 1989;
    cal->Month = 1;
    cal->Day = 1;

    // Expect that adding a month across boundaries adds the equivalent in the Gregorian calendar.
    auto expectedMonthResult = cal->GetDateTime();
    DateDifference monthDuration;
    monthDuration.month = 1;

    auto actualMonthResult = engine->SubtractDuration(startTime, monthDuration);

    VERIFY_ARE_EQUAL(expectedMonthResult.UniversalTime, actualMonthResult->Value.UniversalTime);
}

TEST_METHOD(JaEraTransitionDifference)
{
    auto engine = ref new DateCalculationEngine(CalendarIdentifiers::Japanese);
    auto cal = ref new Calendar();

    // The Showa period ended in Jan 8, 1989.  Pick 2 days across that boundary
    cal->Year = 1989;
    cal->Month = 1;
    cal->Day = 1;
    auto startTime = cal->GetDateTime();

    cal->Year = 1989;
    cal->Month = 1;
    cal->Day = 20;
    auto endTime = cal->GetDateTime();

    auto diff = engine->TryGetDateDifference(startTime, endTime, DateUnit::Day);
    VERIFY_IS_NOT_NULL(diff);
    VERIFY_ARE_EQUAL(diff->Value.day, 19);
}
}
;

DateCalculationEngine ^ DateCalculatorUnitTests::m_DateCalcEngine;
}
