// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

namespace DateCalculationUnitTests
{
    class DateUtils
    {
    public:
        // Converts SYSTEMTIME structure to DateTime value
        // Converts: SYSTEMTIME -> FILETIME -> DateTime
        static Windows::Foundation::DateTime SystemTimeToDateTime(SYSTEMTIME systemTime)
        {
            LPFILETIME lpFileTime = new FILETIME();
            SystemTimeToFileTime(&systemTime, lpFileTime);

            Windows::Foundation::DateTime dateTime;
            dateTime.UniversalTime = (DWORD)lpFileTime->dwHighDateTime;
            dateTime.UniversalTime <<= 32;
            dateTime.UniversalTime |= (DWORD)lpFileTime->dwLowDateTime;

            return dateTime;
        }

        // Converts DateTime value to SYSTEMTIME structure
        // Converts: DateTime -> FILETIME -> SYSTEMTIME
        static SYSTEMTIME DateTimeToSystemTime(Windows::Foundation::DateTime dateTime)
        {
            FILETIME fileTime;
            fileTime.dwLowDateTime = (DWORD)(dateTime.UniversalTime & 0xffffffff);
            fileTime.dwHighDateTime = (DWORD)(dateTime.UniversalTime >> 32);

            SYSTEMTIME systemTime;
            FileTimeToSystemTime(&fileTime, &systemTime);

            return systemTime;
        }

        // Returns long date format for a date
        static Platform::String ^ GetLongDate(SYSTEMTIME systemTime) {
            auto formatter = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter(
                L"longdate",
                Windows::Globalization::ApplicationLanguages::Languages,
                Windows::System::UserProfile::GlobalizationPreferences::HomeGeographicRegion,
                Windows::Globalization::CalendarIdentifiers::Gregorian,
                Windows::Globalization::ClockIdentifiers::TwentyFourHour);

            Windows::Foundation::DateTime dateTime = SystemTimeToDateTime(systemTime);
            return formatter->Format(dateTime);
        }
    };
}
