// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DateCalculator.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace CalculatorApp::Common::DateCalculation;

DateCalculationEngine::DateCalculationEngine(_In_ String^ calendarIdentifier)
{
    m_calendar = ref new Calendar();
    m_calendar->ChangeCalendarSystem(calendarIdentifier);
}

// Adding Duration to a Date 
// Returns: True if function succeeds to calculate the date else returns False
bool DateCalculationEngine::AddDuration(_In_ DateTime startDate, _In_ const DateDifference& duration, _Out_ DateTime *endDate)
{
    try
    {
        m_calendar->SetDateTime(startDate);

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

        *endDate = m_calendar->GetDateTime();
    }
    catch (Platform::InvalidArgumentException^ ex)
    {
        // Do nothing
        return false;
    }

    return true;
}

// Subtracting Duration from a Date
// Returns: True if function succeeds to calculate the date else returns False
bool DateCalculationEngine::SubtractDuration(_In_ DateTime startDate, _In_ const DateDifference& duration, _Out_ DateTime *endDate)
{
    // For Subtract the Algorithm is different than Add. Here the smaller units are subtracted first
    // and then the larger units. 
    try
    {
        m_calendar->SetDateTime(startDate);

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

        *endDate = m_calendar->GetDateTime();
    }
    catch (Platform::InvalidArgumentException^ ex)
    {
        // Do nothing
        return false;
    }

    // Check that the UniversalTime value is not negative
    return (endDate->UniversalTime >= 0);
}

// Calculate the difference between two dates
void DateCalculationEngine::GetDateDifference(_In_ DateTime date1, _In_ DateTime date2, _In_ DateUnit outputFormat, _Out_ DateDifference *difference)
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
        if (TryGetCalendarDaysInMonth(startDate, daysInMonth)
            && TryGetCalendarDaysInYear(endDate, approximateDaysInYear))
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
                        catch (Platform::InvalidArgumentException^)
                        {
                            // Operation failed due to out of bound result
                            // Do nothing
                            differenceInDates[unitIndex] = 0;
                        }
                    }

                    int tempDaysDiff;

                    do
                    {
                        tempDaysDiff = GetDifferenceInDays(pivotDate, endDate);

                        if (tempDaysDiff < 0)
                        {
                            // pivotDate has gone over the end date; start from the beginning of this unit
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
                                pivotDate = AdjustCalendarDate(pivotDate, dateUnit, 1);
                                differenceInDates[unitIndex] += 1;
                            }
                            catch (Platform::InvalidArgumentException^)
                            {
                                // handling for 31st Dec, 9999 last valid date
                                // Do nothing - break out
                                break;
                            }
                        }
                    } while (tempDaysDiff != 0); // dates are the same - exit the loop

                    tempPivotDate = AdjustCalendarDate(tempPivotDate, dateUnit, static_cast<int>(differenceInDates[unitIndex]));
                    pivotDate = tempPivotDate;
                    daysDiff = GetDifferenceInDays(pivotDate, endDate);
                }
            }
        }
    }

    differenceInDates[3] = daysDiff;

    difference->year = differenceInDates[0];
    difference->month = differenceInDates[1];
    difference->week = differenceInDates[2];
    difference->day = differenceInDates[3];
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

    switch (dateUnit)
    {
        case DateUnit::Year:
        {
            // In the Japanese calendar, transition years have 2 partial years.
            // It is not guaranteed that adding 1 year will always add 365 days in the Japanese Calendar.
            // To work around this quirk, we will change the calendar system to Gregorian before adding 1 year in the Japanese Calendar case only.
            // We will then return the calendar system back to the Japanese Calendar.
            auto currentCalendarSystem = m_calendar->GetCalendarSystem();
            if (currentCalendarSystem == CalendarIdentifiers::Japanese)
            {
                m_calendar->ChangeCalendarSystem(CalendarIdentifiers::Gregorian);
            }

            m_calendar->AddYears(difference);
            m_calendar->ChangeCalendarSystem(currentCalendarSystem);
            break;
        }
        case DateUnit::Month:
            m_calendar->AddMonths(difference);
            break;
        case DateUnit::Week:
            m_calendar->AddWeeks(difference);
            break;
    }

    return m_calendar->GetDateTime();
}
