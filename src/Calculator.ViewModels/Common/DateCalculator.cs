// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Globalization;

namespace CalculatorApp.ViewModel.Common.DateCalculation
{
    [Flags]
    public enum DateUnit
    {
        Year = 0x01,
        Month = 0x02,
        Week = 0x04,
        Day = 0x08
    }

    public struct DateDifference : IEquatable<DateDifference>
    {
        public int Year;
        public int Month;
        public int Week;
        public int Day;

        public bool Equals(DateDifference other)
        {
            return Year == other.Year && Month == other.Month && Week == other.Week && Day == other.Day;
        }

        public override bool Equals(object obj) => obj is DateDifference other && Equals(other);
        public override int GetHashCode() => Year ^ (Month << 8) ^ (Week << 16) ^ (Day << 24);
        public static bool operator ==(DateDifference left, DateDifference right) => left.Equals(right);
        public static bool operator !=(DateDifference left, DateDifference right) => !left.Equals(right);
    }

    public sealed class DateCalculationEngine
    {
        private const long c_millisecond = 10000;
        private const long c_second = 1000 * c_millisecond;
        private const long c_minute = 60 * c_second;
        private const long c_hour = 60 * c_minute;
        private const long c_day = 24 * c_hour;

        private const int c_unitsOfDate = 4;
        private const int c_unitsGreaterThanDays = 3;
        private const int c_daysInWeek = 7;

        // Dates are round-tripped through Windows.Foundation.DateTime, whose UniversalTime field
        // counts from the FILETIME epoch, so anything earlier than 1601 cannot be represented.
        // Note this is not the same floor as DateTimeOffset.MinValue, which is year 1.
        private static readonly DateTimeOffset s_minSupportedDate = new DateTimeOffset(1601, 1, 1, 0, 0, 0, TimeSpan.Zero);

        public static readonly DateDifference DateDifferenceUnknown = new DateDifference
        {
            Year = int.MinValue, Month = int.MinValue, Week = int.MinValue, Day = int.MinValue
        };

        private Calendar _calendar;

        public DateCalculationEngine(string calendarIdentifier)
        {
            _calendar = CreateCalendar(calendarIdentifier);
        }

        private static Calendar CreateCalendar(string calendarIdentifier)
        {
            var calendar = new Calendar();
            calendar.ChangeTimeZone("UTC");
            calendar.ChangeCalendarSystem(calendarIdentifier);
            return calendar;
        }

        public DateTimeOffset? AddDuration(DateTimeOffset startDate, DateDifference duration)
        {
            var currentCalendarSystem = _calendar.GetCalendarSystem();
            try
            {
                _calendar.SetDateTime(startDate);

                // Handle Japanese era system by converting to Gregorian for date math
                if (currentCalendarSystem == CalendarIdentifiers.Japanese)
                {
                    _calendar.ChangeCalendarSystem(CalendarIdentifiers.Gregorian);
                }

                if (duration.Year != 0) _calendar.AddYears(duration.Year);
                if (duration.Month != 0) _calendar.AddMonths(duration.Month);
                if (duration.Day != 0) _calendar.AddDays(duration.Day);

                _calendar.ChangeCalendarSystem(currentCalendarSystem);
                return _calendar.GetDateTime();
            }
            catch (ArgumentException)
            {
                _calendar = CreateCalendar(currentCalendarSystem);
                return null;
            }
        }

        public DateTimeOffset? SubtractDuration(DateTimeOffset startDate, DateDifference duration)
        {
            var currentCalendarSystem = _calendar.GetCalendarSystem();
            try
            {
                _calendar.SetDateTime(startDate);

                if (currentCalendarSystem == CalendarIdentifiers.Japanese)
                {
                    _calendar.ChangeCalendarSystem(CalendarIdentifiers.Gregorian);
                }

                // For Subtract the algorithm is different than Add: smaller units first, then larger.
                if (duration.Day != 0) _calendar.AddDays(-duration.Day);
                if (duration.Month != 0) _calendar.AddMonths(-duration.Month);
                if (duration.Year != 0) _calendar.AddYears(-duration.Year);

                _calendar.ChangeCalendarSystem(currentCalendarSystem);

                var dateTime = _calendar.GetDateTime();
                if (dateTime.ToUniversalTime() >= s_minSupportedDate)
                {
                    return dateTime;
                }
                return null;
            }
            catch (ArgumentException)
            {
                _calendar = CreateCalendar(currentCalendarSystem);
                return null;
            }
        }

        public DateDifference? TryGetDateDifference(DateTimeOffset date1, DateTimeOffset date2, DateUnit outputFormat)
        {
            DateTimeOffset startDate, endDate;

            if (date1 < date2)
            {
                startDate = date1;
                endDate = date2;
            }
            else
            {
                startDate = date2;
                endDate = date1;
            }
            var pivotDate = startDate;
            uint daysDiff = (uint)GetDifferenceInDays(startDate, endDate);
            uint[] differenceInDates = new uint[c_unitsOfDate];

            // If output has units other than days (bits 0-2: Year, Month, Week)
            if (((int)outputFormat & 7) != 0)
            {
                if (TryGetCalendarDaysInMonth(startDate, out uint daysInMonth) &&
                    TryGetCalendarDaysInYear(endDate, out uint approximateDaysInYear))
                {
                    uint[] daysIn = { approximateDaysInYear, daysInMonth, c_daysInWeek, 1 };

                    for (int unitIndex = 0; unitIndex < c_unitsGreaterThanDays; unitIndex++)
                    {
                        var tempPivotDate = pivotDate;
                        DateUnit dateUnit = (DateUnit)(1 << unitIndex);

                        if (((int)outputFormat & (int)dateUnit) != 0)
                        {
                            bool isEndDateHit = false;
                            differenceInDates[unitIndex] = daysDiff / daysIn[unitIndex];

                            if (differenceInDates[unitIndex] != 0)
                            {
                                while (differenceInDates[unitIndex] != 0)
                                {
                                    try
                                    {
                                        pivotDate = AdjustCalendarDate(tempPivotDate, dateUnit, (int)differenceInDates[unitIndex]);
                                        break;
                                    }
                                    catch (ArgumentException)
                                    {
                                        // The day-based estimate can overshoot the calendar's upper bound.
                                        differenceInDates[unitIndex] -= 1;
                                    }
                                }
                            }

                            int tempDaysDiff;
                            do
                            {
                                tempDaysDiff = GetDifferenceInDays(pivotDate, endDate);

                                if (tempDaysDiff < 0)
                                {
                                    if (differenceInDates[unitIndex] == 0) return null;
                                    differenceInDates[unitIndex] -= 1;
                                    pivotDate = tempPivotDate;
                                    pivotDate = AdjustCalendarDate(pivotDate, dateUnit, (int)differenceInDates[unitIndex]);
                                    isEndDateHit = true;
                                }
                                else if (tempDaysDiff > 0)
                                {
                                    if (isEndDateHit) break;

                                    try
                                    {
                                        pivotDate = AdjustCalendarDate(tempPivotDate, dateUnit, (int)(differenceInDates[unitIndex] + 1));
                                        differenceInDates[unitIndex] += 1;
                                    }
                                    catch (ArgumentException)
                                    {
                                        // The current pivot is valid; finish with smaller units.
                                        break;
                                    }
                                }
                            } while (tempDaysDiff != 0);

                            int signedDaysDiff = GetDifferenceInDays(pivotDate, endDate);
                            if (signedDaysDiff < 0) return null;
                            daysDiff = (uint)signedDaysDiff;
                        }
                    }
                }
            }

            differenceInDates[3] = daysDiff;

            return new DateDifference
            {
                Year = (int)differenceInDates[0],
                Month = (int)differenceInDates[1],
                Week = (int)differenceInDates[2],
                Day = (int)differenceInDates[3]
            };
        }

        private int GetDifferenceInDays(DateTimeOffset date1, DateTimeOffset date2)
        {
            long ticksDifference = date2.ToUniversalTime().Ticks - date1.ToUniversalTime().Ticks;
            return (int)(ticksDifference / c_day);
        }

        private bool TryGetCalendarDaysInMonth(DateTimeOffset date, out uint daysInMonth)
        {
            daysInMonth = 0;
            _calendar.SetDateTime(date);
            int days = _calendar.NumberOfDaysInThisMonth;
            if (days != -1)
            {
                daysInMonth = (uint)days;
                return true;
            }
            return false;
        }

        private bool TryGetCalendarDaysInYear(DateTimeOffset date, out uint daysInYear)
        {
            daysInYear = 0;
            uint days = 0;
            _calendar.SetDateTime(date);

            int monthsInYear = _calendar.NumberOfMonthsInThisYear;
            if (monthsInYear == -1) return false;

            int firstMonth = _calendar.FirstMonthInThisYear;
            for (int month = 0; month < monthsInYear; month++)
            {
                _calendar.Month = firstMonth + month;
                int daysInMonth = _calendar.NumberOfDaysInThisMonth;
                if (daysInMonth == -1) return false;
                days += (uint)daysInMonth;
            }

            daysInYear = days;
            return true;
        }

        private DateTimeOffset AdjustCalendarDate(DateTimeOffset date, DateUnit dateUnit, int difference)
        {
            _calendar.SetDateTime(date);

            var currentCalendarSystem = _calendar.GetCalendarSystem();
            try
            {
                if (currentCalendarSystem == CalendarIdentifiers.Japanese)
                {
                    _calendar.ChangeCalendarSystem(CalendarIdentifiers.Gregorian);
                }

                switch (dateUnit)
                {
                    case DateUnit.Year:
                        _calendar.AddYears(difference);
                        break;
                    case DateUnit.Month:
                        _calendar.AddMonths(difference);
                        break;
                    case DateUnit.Week:
                        _calendar.AddWeeks(difference);
                        break;
                }

                _calendar.ChangeCalendarSystem(currentCalendarSystem);
                return _calendar.GetDateTime();
            }
            finally
            {
                if (_calendar.GetCalendarSystem() != currentCalendarSystem)
                {
                    _calendar.SetDateTime(date);
                    _calendar.ChangeCalendarSystem(currentCalendarSystem);
                }
            }
        }
    }
}
