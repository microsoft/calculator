// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for WinRT Windows.Globalization.Calendar, shaped to the
// subset DateCalculationEngine needs. Backed by System.Globalization.Calendar
// (ICU-independent; Gregorian and alternative calendars included in .NET).
//
// Behavior notes vs WinRT:
//   - Dates are UTC date-only values; ChangeCalendarSystem keeps the instant
//     (only field interpretation changes), matching WinRT.
//   - Identifiers accept both WinRT spellings ("gregorian") and the legacy
//     "XxxCalendar" forms the old tests use; comparison is case-insensitive.
//   - WinRT calendars cannot represent anything before 1601-01-01
//     (FILETIME epoch); operations that would leave that floor throw
//     ArgumentException, which the engine already handles as out-of-range.
using System.Globalization;

namespace Calculator.Linux.Services;

public sealed class LinuxCalendar
{
    private static readonly DateTime s_minSupportedDate = new(1601, 1, 1, 0, 0, 0, DateTimeKind.Utc);

    private Calendar _calendar;
    private string _calendarSystem;
    private DateTime _date;

    public LinuxCalendar(string calendarSystem = "gregorian")
    {
        _calendarSystem = Normalize(calendarSystem);
        _calendar = Create(_calendarSystem);
        _date = DateTime.UtcNow.Date;
    }

    public string GetCalendarSystem() => _calendarSystem;

    public void ChangeCalendarSystem(string calendarSystem)
    {
        _calendarSystem = Normalize(calendarSystem);
        _calendar = Create(_calendarSystem);
    }

    public void SetDateTime(DateTimeOffset date)
    {
        DateTime utc = date.UtcDateTime.Date;
        if (utc < s_minSupportedDate)
        {
            throw new ArgumentException("Date precedes the supported calendar floor (1601-01-01).");
        }
        _date = utc;
    }

    public DateTimeOffset GetDateTime() => new(_date, TimeSpan.Zero);

    public void AddYears(int value) => Add(_calendar.AddYears(_date, value));
    public void AddMonths(int value) => Add(_calendar.AddMonths(_date, value));
    public void AddDays(int value) => Add(_calendar.AddDays(_date, value));
    public void AddWeeks(int value) => Add(_calendar.AddDays(_date, value * 7));

    public int NumberOfDaysInThisMonth
    {
        get
        {
            try
            {
                return _calendar.GetDaysInMonth(_date.Year, _date.Month, CurrentEra);
            }
            catch (ArgumentOutOfRangeException)
            {
                return -1;
            }
        }
    }

    public int NumberOfMonthsInThisYear
    {
        get
        {
            try
            {
                return _calendar.GetMonthsInYear(_date.Year, CurrentEra);
            }
            catch (ArgumentOutOfRangeException)
            {
                return -1;
            }
        }
    }

    public int FirstMonthInThisYear => 1;

    public int Month
    {
        get => _calendar.GetMonth(_date);
        set
        {
            int days = _calendar.GetDaysInMonth(_date.Year, value, CurrentEra);
            _date = new DateTime(_date.Year, value, Math.Min(_date.Day, days), 0, 0, 0, DateTimeKind.Utc);
        }
    }

    private int CurrentEra => _calendar.GetEra(_date);

    private void Add(DateTime next)
    {
        // .NET throws ArgumentOutOfRangeException (an ArgumentException) past
        // DateTime.Min/MaxValue; the pre-1601 floor is enforced explicitly.
        if (next < s_minSupportedDate)
        {
            throw new ArgumentException("Result precedes the supported calendar floor (1601-01-01).");
        }
        _date = next;
    }

    private static string Normalize(string identifier)
    {
        string lower = (identifier ?? string.Empty).Trim().ToLowerInvariant();
        if (lower.EndsWith("calendar", StringComparison.Ordinal))
        {
            lower = lower.Substring(0, lower.Length - "calendar".Length);
        }
        return lower;
    }

    private static Calendar Create(string system) =>
        system switch
        {
            "hebrew" => new HebrewCalendar(),
            "hijri" => new HijriCalendar(),
            "japanese" => new JapaneseCalendar(),
            "korean" => new KoreanCalendar(),
            "taiwan" => new TaiwanCalendar(),
            "thai" => new ThaiBuddhistCalendar(),
            "umalqura" => new UmAlQuraCalendar(),
            // Upstream maps Persian to Hijri (CAL_PERSIAN); keep that mapping.
            "persian" => new HijriCalendar(),
            _ => new GregorianCalendar(),
        };
}
