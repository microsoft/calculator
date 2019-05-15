// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using Windows.Globalization;

namespace CalculatorApp.Common.DateCalculation
{
	[Flags]
	public enum DateUnit
	{
		Year = 0x01,
		Month = 0x02,
		Week = 0x04,
		Day = 0x08
	};

    // Struct to store the difference between two Dates in the form of Years, Months , Weeks
    public struct DateDifference
	{
		public int year;
        public int month;
        public int week;
        public int day;
    };

	public class DateCalculationEngine
	{
		const ulong c_millisecond = 10000;
		const ulong c_second = 1000 * c_millisecond;
		const ulong c_minute = 60 * c_second;
		const ulong c_hour = 60 * c_minute;
		const ulong c_day = 24 * c_hour;

		const int c_unitsOfDate = 4;          // Units Year,Month,Week,Day
		const int c_unitsGreaterThanDays = 3; // Units Greater than Days (Year/Month/Week) 3
		const int c_daysInWeek = 7;

		private Calendar m_calendar;

		public DateCalculationEngine(string calendarIdentifier)
		{
			m_calendar = new Calendar();
			m_calendar.ChangeTimeZone("UTC");
			m_calendar.ChangeCalendarSystem(calendarIdentifier);
		}

		// Adding Duration to a Date
		// Returns: True if function succeeds to calculate the date else returns False
		public bool AddDuration( DateTime startDate,  DateDifference duration, out DateTime endDate)
		{
		    var currentCalendarSystem = m_calendar.GetCalendarSystem();

		    try
		    {
		        m_calendar.SetDateTime(startDate);

		        // The Japanese Era system can have multiple year partitions within the same year.
		        // For example, April 30, 2019 is denoted April 30, Heisei 31; May 1, 2019 is denoted as May 1, Reiwa 1.
		        // The Calendar treats Heisei 31 and Reiwa 1 as separate years, which results in some unexpected behaviors where subtracting a year from Reiwa 1 results
		        // in a date in Heisei 31. To provide the expected result across era boundaries, we first convert the Japanese era system to a Gregorian system, do date
		        // math, and then convert back to the Japanese era system. This works because the Japanese era system maintains the same year/month boundaries and
		        // durations as the Gregorian system and is only different in display value.
		        if (currentCalendarSystem == CalendarIdentifiers.Japanese)
		        {
		            m_calendar.ChangeCalendarSystem(CalendarIdentifiers.Gregorian);
		        }

		        if (duration.year != 0)
		        {
		            m_calendar.AddYears(duration.year);
		        }
		        if (duration.month != 0)
		        {
		            m_calendar.AddMonths(duration.month);
		        }
		        if (duration.day != 0)
		        {
		            m_calendar.AddDays(duration.day);
		        }

		        endDate = m_calendar.GetDateTime().DateTime;
		    }
		    catch (ArgumentException ex)
		    {
		        // ensure that we revert to the correct calendar system
		        m_calendar.ChangeCalendarSystem(currentCalendarSystem);

		        // Do nothing
				endDate = default(DateTime);
		        return false;
		    }

		    m_calendar.ChangeCalendarSystem(currentCalendarSystem);

		    return true;
		}

		// Subtracting Duration from a Date
		// Returns: True if function succeeds to calculate the date else returns False
		public bool SubtractDuration( DateTime startDate,  DateDifference duration, out DateTime endDate)
		{
		    var currentCalendarSystem = m_calendar.GetCalendarSystem();

		    // For Subtract the Algorithm is different than Add. Here the smaller units are subtracted first
		    // and then the larger units.
		    try
		    {
		        m_calendar.SetDateTime(startDate);

		        // The Japanese Era system can have multiple year partitions within the same year.
		        // For example, April 30, 2019 is denoted April 30, Heisei 31; May 1, 2019 is denoted as May 1, Reiwa 1.
		        // The Calendar treats Heisei 31 and Reiwa 1 as separate years, which results in some unexpected behaviors where subtracting a year from Reiwa 1 results
		        // in a date in Heisei 31. To provide the expected result across era boundaries, we first convert the Japanese era system to a Gregorian system, do date
		        // math, and then convert back to the Japanese era system. This works because the Japanese era system maintains the same year/month boundaries and
		        // durations as the Gregorian system and is only different in display value.
		        if (currentCalendarSystem == CalendarIdentifiers.Japanese)
		        {
		            m_calendar.ChangeCalendarSystem(CalendarIdentifiers.Gregorian);
		        }

		        if (duration.day != 0)
		        {
		            m_calendar.AddDays(-duration.day);
		        }
		        if (duration.month != 0)
		        {
		            m_calendar.AddMonths(-duration.month);
		        }
		        if (duration.year != 0)
		        {
		            m_calendar.AddYears(-duration.year);
		        }
		        endDate = m_calendar.GetDateTime().DateTime;
		    }
		    catch (ArgumentException ex)
		    {
		        // ensure that we revert to the correct calendar system
		        m_calendar.ChangeCalendarSystem(currentCalendarSystem);

		        // Do nothing
				endDate = default(DateTime);
		        return false;
		    }

		    m_calendar.ChangeCalendarSystem(currentCalendarSystem);

		    // Check that the UniversalTime value is not negative
		    return (endDate.ToUniversalTime().Ticks >= 0);
		}

		// Calculate the difference between two dates
		public void GetDateDifference( DateTime date1,  DateTime date2,  DateUnit outputFormat, out DateDifference difference)
		{
		    DateTime startDate;
		    DateTime endDate;
		    DateTime pivotDate;
		    DateTime tempPivotDate;
		    uint daysDiff = 0;
		    uint[] differenceInDates = new uint[c_unitsOfDate];

		    if (date1.ToUniversalTime() < date2.ToUniversalTime())
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

		    daysDiff = (uint)GetDifferenceInDays(startDate, endDate);

		    // If output has units other than days
		    // 0th bit: Year, 1st bit: Month, 2nd bit: Week, 3rd bit: Day
		    if (((int)(outputFormat) & 7) != 0)
		    {
		        uint daysInMonth;
		        uint approximateDaysInYear;

		        // If we're unable to calculate the days-in-month or days-in-year, we'll leave the values at 0.
		        if (TryGetCalendarDaysInMonth(startDate, out daysInMonth) && TryGetCalendarDaysInYear(endDate, out approximateDaysInYear))
		        {
		            uint[] daysIn = new uint[c_unitsOfDate] { approximateDaysInYear, daysInMonth, c_daysInWeek, 1 };

		            for (int unitIndex = 0; unitIndex < c_unitsGreaterThanDays; unitIndex++)
		            {
		                tempPivotDate = pivotDate;

		                // Check if the bit flag is set for the date unit
		                DateUnit dateUnit = (DateUnit)(1 << unitIndex);

		                if ((int)(outputFormat & dateUnit) != 0)
		                {
		                    bool isEndDateHit = false;
		                    differenceInDates[unitIndex] = (daysDiff / daysIn[unitIndex]);

		                    if (differenceInDates[unitIndex] != 0)
		                    {
		                        try
		                        {
		                            pivotDate = AdjustCalendarDate(pivotDate, dateUnit, (int)(differenceInDates[unitIndex]));
		                        }
		                        catch (ArgumentException)
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
		                            pivotDate = AdjustCalendarDate(pivotDate, dateUnit, (int)(differenceInDates[unitIndex]));
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
		                            catch (ArgumentException)
		                            {
		                                // handling for 31st Dec, 9999 last valid date
		                                // Do nothing - break out
		                                break;
		                            }
		                        }
		                    } while (tempDaysDiff != 0); // dates are the same - exit the loop

		                    tempPivotDate = AdjustCalendarDate(tempPivotDate, dateUnit, (int)(differenceInDates[unitIndex]));
		                    pivotDate = tempPivotDate;
		                    daysDiff = (uint)GetDifferenceInDays(pivotDate, endDate);
		                }
		            }
		        }
		    }

		    differenceInDates[3] = daysDiff;

		    difference.year = (int)differenceInDates[0];
		    difference.month = (int)differenceInDates[1];
		    difference.week = (int)differenceInDates[2];
		    difference.day = (int)differenceInDates[3];
		}

		// Private Methods

		// Gets number of days between the two date time values
		int GetDifferenceInDays(DateTime date1, DateTime date2)
		{
		    // A tick is defined as the number of 100 nanoseconds
		    long ticksDifference = date2.ToUniversalTime().Ticks - date1.ToUniversalTime().Ticks;
		    return (int)(ticksDifference / (long)(c_day));
		}

		// Gets number of Calendar days in the month in which this date falls.
		// Returns true if successful, false otherwise.
		bool TryGetCalendarDaysInMonth( DateTime date, out uint daysInMonth)
		{
			daysInMonth = 0;
		    bool result = false;
		    m_calendar.SetDateTime(date);

		    // NumberOfDaysInThisMonth returns -1 if unknown.
		    int daysInThisMonth = m_calendar.NumberOfDaysInThisMonth;
		    if (daysInThisMonth != -1)
		    {
		        daysInMonth = (uint)(daysInThisMonth);
		        result = true;
		    }

		    return result;
		}

		// Gets number of Calendar days in the year in which this date falls.
		// Returns true if successful, false otherwise.
		bool TryGetCalendarDaysInYear( DateTime date, out uint daysInYear)
		{
			daysInYear = 0;
		    bool result = false;
		    uint days = 0;

		    m_calendar.SetDateTime(date);

		    // NumberOfMonthsInThisYear returns -1 if unknown.
		    int monthsInYear = m_calendar.NumberOfMonthsInThisYear;
		    if (monthsInYear != -1)
		    {
		        bool monthResult = true;

		        // Not all years begin with Month 1.
		        int firstMonthThisYear = m_calendar.FirstMonthInThisYear;
		        for (int month = 0; month < monthsInYear; month++)
		        {
		            m_calendar.Month = firstMonthThisYear + month;

		            // NumberOfDaysInThisMonth returns -1 if unknown.
		            int daysInMonth = m_calendar.NumberOfDaysInThisMonth;
		            if (daysInMonth == -1)
		            {
		                monthResult = false;
		                break;
		            }

		            days += (uint)daysInMonth;
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
		DateTime AdjustCalendarDate(DateTime date, DateUnit dateUnit, int difference)
		{
		    m_calendar.SetDateTime(date);

		    // The Japanese Era system can have multiple year partitions within the same year.
		    // For example, April 30, 2019 is denoted April 30, Heisei 31; May 1, 2019 is denoted as May 1, Reiwa 1.
		    // The Calendar treats Heisei 31 and Reiwa 1 as separate years, which results in some unexpected behaviors where subtracting a year from Reiwa 1 results in
		    // a date in Heisei 31. To provide the expected result across era boundaries, we first convert the Japanese era system to a Gregorian system, do date math,
		    // and then convert back to the Japanese era system. This works because the Japanese era system maintains the same year/month boundaries and durations as
		    // the Gregorian system and is only different in display value.
		    var currentCalendarSystem = m_calendar.GetCalendarSystem();
		    if (currentCalendarSystem == CalendarIdentifiers.Japanese)
		    {
		        m_calendar.ChangeCalendarSystem(CalendarIdentifiers.Gregorian);
		    }

		    switch (dateUnit)
		    {
		    case DateUnit.Year:
		        m_calendar.AddYears(difference);
		        break;
		    case DateUnit.Month:
		        m_calendar.AddMonths(difference);
		        break;
		    case DateUnit.Week:
		        m_calendar.AddWeeks(difference);
		        break;
		    }

		    m_calendar.ChangeCalendarSystem(currentCalendarSystem);

		    return m_calendar.GetDateTime().DateTime;
		}
	}
}
