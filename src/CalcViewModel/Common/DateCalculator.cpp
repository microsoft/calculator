// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DateCalculator.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace CalculatorApp::ViewModel::Common::DateCalculation;

bool operator==(const DateDifference& l, const DateDifference& r)
{
    return l.year == r.year && l.month == r.month && l.week == r.week && l.day == r.day;
}

DateCalculationEngine::DateCalculationEngine(_In_ String ^ calendarIdentifier)
{
    m_calendar = ref new Calendar();
    m_calendar->ChangeTimeZone("UTC");
    m_calendar->ChangeCalendarSystem(calendarIdentifier);
}

// Adding Duration to a Date
// Returns: True if function succeeds to calculate the date else returns False
IBox<DateTime> ^ DateCalculationEngine::AddDuration(DateTime startDate, DateDifference duration)
{
    auto currentCalendarSystem = m_calendar->GetCalendarSystem();
    try
    {
        m_calendar->SetDateTime(startDate);

        // The Japanese Era system can have multiple year partitions within the same year.
        // For example, April 30, 2019 is denoted April 30, Heisei 31; May 1, 2019 is denoted as May 1, Reiwa 1.
        // The Calendar treats Heisei 31 and Reiwa 1 as separate years, which results in some unexpected behaviors where subtracting a year from Reiwa 1 results
        // in a date in Heisei 31. To provide the expected result across era boundaries, we first convert the Japanese era system to a Gregorian system, do date
        // math, and then convert back to the Japanese era system. This works because the Japanese era system maintains the same year/month boundaries and
        // durations as the Gregorian system and is only different in display value.
        if (currentCalendarSystem == CalendarIdentifiers::Japanese)
        {
            m_calendar->ChangeCalendarSystem(CalendarIdentifiers::Gregorian);
        }

        if (duration.year != 0)
        {
            m_calendar->AddYears(duration.year);
        }
        if (duration.month != 0)
        {
            m_calendar->AddMonths(duration.month);
        }
        if (duration.day != 0)
        {
            m_calendar->AddDays(duration.day);
        }

        m_calendar->ChangeCalendarSystem(currentCalendarSystem);
        return m_calendar->GetDateTime();
    }
    catch (Platform::InvalidArgumentException ^ ex)
    {
        // ensure that we revert to the correct calendar system
        m_calendar->ChangeCalendarSystem(currentCalendarSystem);

        // Do nothing
        return nullptr;
    }
}

// Subtracting Duration from a Date
// Returns: True if function succeeds to calculate the date else returns False
IBox<DateTime> ^ DateCalculationEngine::SubtractDuration(_In_ DateTime startDate, _In_ DateDifference duration)
{
    auto currentCalendarSystem = m_calendar->GetCalendarSystem();

    // For Subtract the Algorithm is different than Add. Here the smaller units are subtracted first
    // and then the larger units.
    try
    {
        m_calendar->SetDateTime(startDate);

        // The Japanese Era system can have multiple year partitions within the same year.
        // For example, April 30, 2019 is denoted April 30, Heisei 31; May 1, 2019 is denoted as May 1, Reiwa 1.
        // The Calendar treats Heisei 31 and Reiwa 1 as separate years, which results in some unexpected behaviors where subtracting a year from Reiwa 1 results
        // in a date in Heisei 31. To provide the expected result across era boundaries, we first convert the Japanese era system to a Gregorian system, do date
        // math, and then convert back to the Japanese era system. This works because the Japanese era system maintains the same year/month boundaries and
        // durations as the Gregorian system and is only different in display value.
        if (currentCalendarSystem == CalendarIdentifiers::Japanese)
        {
            m_calendar->ChangeCalendarSystem(CalendarIdentifiers::Gregorian);
        }

        if (duration.day != 0)
        {
            m_calendar->AddDays(-duration.day);
        }
        if (duration.month != 0)
        {
            m_calendar->AddMonths(-duration.month);
        }
        if (duration.year != 0)
        {
            m_calendar->AddYears(-duration.year);
        }
        m_calendar->ChangeCalendarSystem(currentCalendarSystem);

        auto dateTime = m_calendar->GetDateTime();
        // Check that the UniversalTime value is not negative
        if (dateTime.UniversalTime >= 0)
        {
            return dateTime;
        }
        else
        {
            return nullptr;
        }
    }
    catch (Platform::InvalidArgumentException ^ ex)
    {
        // ensure that we revert to the correct calendar system
        m_calendar->ChangeCalendarSystem(currentCalendarSystem);

        // Do nothing
        return nullptr;
    }
}

// Calculate the difference between two dates
IBox<DateDifference> ^ DateCalculationEngine::TryGetDateDifference(_In_ DateTime date1, _In_ DateTime date2, _In_ DateUnit outputFormat)
{
    DateTime startDate;
    DateTime endDate;
    DateTime pivotDate;
    DateTime tempPivotDate;
    UINT daysDiff = 0;
    UINT differenceInDates[c_unitsOfDate] = { 0 };

    if (date1.UniversalTime < date2.UniversalTime)
    {
        startDate = date1;
        endDate = date2;
    }
    else
    {
        startDate = date2;
        endDate = date1;
    }

    pivotDate = startDate;

    daysDiff = GetDifferenceInDays(startDate, endDate);

    // If output has units other than days
    // 0th bit: Year, 1st bit: Month, 2nd bit: Week, 3rd bit: Day
    if (static_cast<int>(outputFormat) & 7)
    {
        UINT daysInMonth;
        UINT approximateDaysInYear;

        // If we're unable to calculate the days-in-month or days-in-year, we'll leave the values at 0.
        if (TryGetCalendarDaysInMonth(startDate, daysInMonth) && TryGetCalendarDaysInYear(endDate, approximateDaysInYear))
        {
            UINT daysIn[c_unitsOfDate] = { approximateDaysInYear, daysInMonth, c_daysInWeek, 1 };

            for (int unitIndex = 0; unitIndex < c_unitsGreaterThanDays; unitIndex++)
            {
                tempPivotDate = pivotDate;

                // Check if the bit flag is set for the date unit
                DateUnit dateUnit = static_cast<DateUnit>(1 << unitIndex);

                if (static_cast<int>(outputFormat & dateUnit))
                {
                    bool isEndDateHit = false;
                    differenceInDates[unitIndex] = (daysDiff / daysIn[unitIndex]);

                    if (differenceInDates[unitIndex] != 0)
                    {
                        try
                        {
                            pivotDate = AdjustCalendarDate(pivotDate, dateUnit, static_cast<int>(differenceInDates[unitIndex]));
                        }
                        catch (Platform::InvalidArgumentException ^)
                        {
                            // Operation failed due to out of bound result
                            // For example: 31st Dec, 9999 - last valid date
                            return nullptr;
                        }
                    }

                    int tempDaysDiff;

                    do
                    {
                        tempDaysDiff = GetDifferenceInDays(pivotDate, endDate);

                        if (tempDaysDiff < 0)
                        {
                            // pivotDate has gone over the end date; start from the beginning of this unit
                            if (differenceInDates[unitIndex] == 0)
                            {
                                // differenceInDates[unitIndex] is unsigned, the value can't be negative
                                return nullptr;
                            }
                            differenceInDates[unitIndex] -= 1;
                            pivotDate = tempPivotDate;
                            pivotDate = AdjustCalendarDate(pivotDate, dateUnit, static_cast<int>(differenceInDates[unitIndex]));
                            isEndDateHit = true;
                        }
                        else if (tempDaysDiff > 0)
                        {
                            if (isEndDateHit)
                            {
                                // This is the closest the pivot can get to the end date for this unit
                                break;
                            }

                            // pivotDate is still below the end date
                            try
                            {
                                pivotDate = AdjustCalendarDate(tempPivotDate, dateUnit, static_cast<int>(differenceInDates[unitIndex] + 1));
                                differenceInDates[unitIndex] += 1;
                            }
                            catch (Platform::InvalidArgumentException ^)
                            {
                                // Operation failed due to out of bound result
                                // For example: 31st Dec, 9999 - last valid date
                                return nullptr;
                            }
                        }
                    } while (tempDaysDiff != 0); // dates are the same - exit the loop

                    tempPivotDate = AdjustCalendarDate(tempPivotDate, dateUnit, static_cast<int>(differenceInDates[unitIndex]));
                    pivotDate = tempPivotDate;
                    int signedDaysDiff = GetDifferenceInDays(pivotDate, endDate);
                    if (signedDaysDiff < 0)
                    {
                        // daysDiff is unsigned, the value can't be negative
                        return nullptr;
                    }

                    daysDiff = signedDaysDiff;
                }
            }
        }
    }

    differenceInDates[3] = daysDiff;

    DateDifference result;
    result.year = differenceInDates[0];
    result.month = differenceInDates[1];
    result.week = differenceInDates[2];
    result.day = differenceInDates[3];
    return result;
}

// Private Methods

// Gets number of days between the two date time values
int DateCalculationEngine::GetDifferenceInDays(DateTime date1, DateTime date2)
{
    // A tick is defined as the number of 100 nanoseconds
    long long ticksDifference = date2.UniversalTime - date1.UniversalTime;
    return static_cast<int>(ticksDifference / static_cast<long long>(c_day));
}

// Gets number of Calendar days in the month in which this date falls.
// Returns true if successful, false otherwise.
bool DateCalculationEngine::TryGetCalendarDaysInMonth(_In_ DateTime date, _Out_ UINT& daysInMonth)
{
    bool result = false;
    m_calendar->SetDateTime(date);

    // NumberOfDaysInThisMonth returns -1 if unknown.
    int daysInThisMonth = m_calendar->NumberOfDaysInThisMonth;
    if (daysInThisMonth != -1)
    {
        daysInMonth = static_cast<UINT>(daysInThisMonth);
        result = true;
    }

    return result;
}

// Gets number of Calendar days in the year in which this date falls.
// Returns true if successful, false otherwise.
bool DateCalculationEngine::TryGetCalendarDaysInYear(_In_ DateTime date, _Out_ UINT& daysInYear)
{
    bool result = false;
    UINT days = 0;

    m_calendar->SetDateTime(date);

    // NumberOfMonthsInThisYear returns -1 if unknown.
    int monthsInYear = m_calendar->NumberOfMonthsInThisYear;
    if (monthsInYear != -1)
    {
        bool monthResult = true;

        // Not all years begin with Month 1.
        int firstMonthThisYear = m_calendar->FirstMonthInThisYear;
        for (int month = 0; month < monthsInYear; month++)
        {
            m_calendar->Month = firstMonthThisYear + month;

            // NumberOfDaysInThisMonth returns -1 if unknown.
            int daysInMonth = m_calendar->NumberOfDaysInThisMonth;
            if (daysInMonth == -1)
            {
                monthResult = false;
                break;
            }

            days += daysInMonth;
        }

        if (monthResult)
        {
            daysInYear = days;
            result = true;
        }
    }

    return result;
}

// Adds/Subtracts certain value for a particular date unit
DateTime DateCalculationEngine::AdjustCalendarDate(Windows::Foundation::DateTime date, DateUnit dateUnit, int difference)
{
    m_calendar->SetDateTime(date);

    // The Japanese Era system can have multiple year partitions within the same year.
    // For example, April 30, 2019 is denoted April 30, Heisei 31; May 1, 2019 is denoted as May 1, Reiwa 1.
    // The Calendar treats Heisei 31 and Reiwa 1 as separate years, which results in some unexpected behaviors where subtracting a year from Reiwa 1 results in
    // a date in Heisei 31. To provide the expected result across era boundaries, we first convert the Japanese era system to a Gregorian system, do date math,
    // and then convert back to the Japanese era system. This works because the Japanese era system maintains the same year/month boundaries and durations as
    // the Gregorian system and is only different in display value.
    auto currentCalendarSystem = m_calendar->GetCalendarSystem();
    if (currentCalendarSystem == CalendarIdentifiers::Japanese)
    {
        m_calendar->ChangeCalendarSystem(CalendarIdentifiers::Gregorian);
    }

    switch (dateUnit)
    {
    case DateUnit::Year:
        m_calendar->AddYears(difference);
        break;
    case DateUnit::Month:
        m_calendar->AddMonths(difference);
        break;
    case DateUnit::Week:
        m_calendar->AddWeeks(difference);
        break;
    }

    m_calendar->ChangeCalendarSystem(currentCalendarSystem);

    return m_calendar->GetDateTime();
}
