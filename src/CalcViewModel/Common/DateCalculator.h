// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

const uint64_t c_millisecond = 10000;
const uint64_t c_second = 1000 * c_millisecond;
const uint64_t c_minute = 60 * c_second;
const uint64_t c_hour = 60 * c_minute;
const uint64_t c_day = 24 * c_hour;

const int c_unitsOfDate = 4;          // Units Year,Month,Week,Day
const int c_unitsGreaterThanDays = 3; // Units Greater than Days (Year/Month/Week) 3
const int c_daysInWeek = 7;

namespace CalculatorApp
{
    namespace Common
    {
        namespace DateCalculation
        {
        public
            enum class _Enum_is_bitflag_ DateUnit
            {
                Year = 0x01,
                Month = 0x02,
                Week = 0x04,
                Day = 0x08
            };

            // Struct to store the difference between two Dates in the form of Years, Months , Weeks
            struct DateDifference
            {
                int year = 0;
                int month = 0;
                int week = 0;
                int day = 0;

                bool operator==(const DateDifference& dd) const
                {
                    return year == dd.year && month == dd.month && week == dd.week && day == day;
                }
            };

            const DateDifference DateDifferenceUnknown{ INT_MIN, INT_MIN, INT_MIN, INT_MIN };

            class DateCalculationEngine
            {
            public:
                // Constructor
                DateCalculationEngine(_In_ Platform::String ^ calendarIdentifier);

                // Public Methods
                bool __nothrow
                AddDuration(_In_ Windows::Foundation::DateTime startDate, _In_ const DateDifference& duration, _Out_ Windows::Foundation::DateTime* endDate);
                bool __nothrow SubtractDuration(
                    _In_ Windows::Foundation::DateTime startDate,
                    _In_ const DateDifference& duration,
                    _Out_ Windows::Foundation::DateTime* endDate);
                bool __nothrow TryGetDateDifference(
                    _In_ Windows::Foundation::DateTime date1,
                    _In_ Windows::Foundation::DateTime date2,
                    _In_ DateUnit outputFormat,
                    _Out_ DateDifference* difference);

            private:
                // Private Variables
                Windows::Globalization::Calendar ^ m_calendar;

                // Private Methods
                int GetDifferenceInDays(Windows::Foundation::DateTime date1, Windows::Foundation::DateTime date2);
                bool TryGetCalendarDaysInMonth(_In_ Windows::Foundation::DateTime date, _Out_ UINT& daysInMonth);
                bool TryGetCalendarDaysInYear(_In_ Windows::Foundation::DateTime date, _Out_ UINT& daysInYear);
                Windows::Foundation::DateTime AdjustCalendarDate(Windows::Foundation::DateTime date, DateUnit dateUnit, int difference);
            };
        }
    }
}
