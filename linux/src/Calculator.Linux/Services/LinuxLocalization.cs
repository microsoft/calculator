// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for LocalizationSettings (WinRT DecimalFormatter +
// kernel32 GetLocaleInfoEx). All locale facts come from
// System.Globalization.CultureInfo, which on Linux is backed by ICU and
// matches WinRT behavior for separators, digits, week start and RTL.
//
// Behavior notes vs upstream:
//   - DayOfWeek is System.DayOfWeek (same Sunday=0 numbering as WinRT).
//   - CalendarIdentifier uses the same well-known strings as WinRT
//     CalendarIdentifiers ("gregorian", "hijri", ...).
//   - CurrencySymbolPrecedence is approximated from CurrencyPositivePattern
//     (WinRT reads LOCALE_IPOSSYMPRECEDES, which has no .NET equivalent).
//   - Font APIs (LanguageFontGroup) and WinRT formatter factories are UI-layer
//     concerns and live with the Avalonia shell (Task 9), not here.
using System.Globalization;
using System.Text;

namespace Calculator.Linux.Services;

public sealed class LinuxLocalization
{
    private readonly char[] _digitSymbols = new char[10];

    public LinuxLocalization(CultureInfo? culture = null)
    {
        CultureInfo cultureInfo = culture ?? CultureInfo.CurrentCulture;
        Initialize(cultureInfo);
    }

    public string LocaleName { get; private set; } = "en-US";
    public char DecimalSeparator { get; private set; } = '.';
    public char GroupSeparator { get; private set; } = ',';
    public string NumberGrouping { get; private set; } = "3;0";
    public string ListSeparator { get; private set; } = ",";
    public string CalendarIdentifier { get; private set; } = "gregorian";
    public DayOfWeek FirstDayOfWeek { get; private set; } = DayOfWeek.Sunday;
    public int CurrencyTrailingDigits { get; private set; } = 2;
    public int CurrencySymbolPrecedence { get; private set; } = 1;
    public bool IsRightToLeft { get; private set; }

    public bool IsDigitEnUsSetting => DigitSymbolFromEnUsDigit('0') == '0';

    public char DigitSymbolFromEnUsDigit(char digit)
    {
        int index = digit - '0';
        if (index < 0 || index > 9)
        {
            throw new ArgumentOutOfRangeException(nameof(digit));
        }
        return _digitSymbols[index];
    }

    public bool IsEnUsDigit(char c) => c >= '0' && c <= '9';
    public bool IsLocalizedDigit(char c) => Array.IndexOf(_digitSymbols, c) >= 0;

    public bool IsLocalizedHexDigit(char c) =>
        IsLocalizedDigit(c) || (c >= 'A' && c <= 'F');

    public string DecimalSeparatorString => new(DecimalSeparator, 1);
    public string GroupSeparatorString => new(GroupSeparator, 1);

    public string EnglishValueFromLocalizedDigits(string localized)
    {
        if (LocaleName == "en-US")
        {
            return localized;
        }

        var sb = new StringBuilder(localized.Length);
        foreach (char ch in localized)
        {
            char result = ch;
            if (!IsEnUsDigit(ch))
            {
                int index = Array.IndexOf(_digitSymbols, ch);
                if (index >= 0)
                {
                    result = (char)('0' + index);
                }
            }
            if (result == DecimalSeparator)
            {
                result = '.';
            }
            sb.Append(result);
        }
        return sb.ToString();
    }

    public string RemoveGroupSeparators(string source)
    {
        var sb = new StringBuilder(source.Length);
        foreach (char c in source)
        {
            if (c != ' ' && c != GroupSeparator)
            {
                sb.Append(c);
            }
        }
        return sb.ToString();
    }

    public void LocalizeDisplayValue(ref string value)
    {
        if (IsDigitEnUsSetting)
        {
            return;
        }

        var sb = new StringBuilder(value.Length);
        foreach (char ch in value)
        {
            sb.Append(IsEnUsDigit(ch) ? DigitSymbolFromEnUsDigit(ch) : ch);
        }
        value = sb.ToString();
    }

    private void Initialize(CultureInfo culture)
    {
        NumberFormatInfo number = culture.NumberFormat;

        for (int i = 0; i < 10; i++)
        {
            string native = (number.NativeDigits != null && number.NativeDigits.Length > i)
                ? number.NativeDigits[i]
                : ((char)('0' + i)).ToString();
            _digitSymbols[i] = string.IsNullOrEmpty(native) ? (char)('0' + i) : native[0];
        }

        LocaleName = culture.Name.Length == 0 ? "en-US" : culture.Name;
        DecimalSeparator = FirstCharOr(number.NumberDecimalSeparator, '.');
        GroupSeparator = FirstCharOr(number.NumberGroupSeparator, ',');
        NumberGrouping = GroupSizesToGrouping(number.NumberGroupSizes);
        ListSeparator = string.IsNullOrEmpty(culture.TextInfo.ListSeparator) ? "," : culture.TextInfo.ListSeparator;
        CalendarIdentifier = CalendarToIdentifier(culture.Calendar);
        FirstDayOfWeek = culture.DateTimeFormat.FirstDayOfWeek;
        CurrencyTrailingDigits = number.CurrencyDecimalDigits;
        // Patterns 0 ("$n") and 2 ("$ n"): symbol precedes the value.
        CurrencySymbolPrecedence = culture.NumberFormat.CurrencyPositivePattern is 0 or 2 ? 1 : 0;
        IsRightToLeft = culture.TextInfo.IsRightToLeft;
    }

    private static char FirstCharOr(string? value, char fallback) =>
        string.IsNullOrEmpty(value) ? fallback : value[0];

    private static string GroupSizesToGrouping(int[] sizes)
    {
        if (sizes == null || sizes.Length == 0 || sizes[0] == 0)
        {
            return "0";
        }
        // Windows LOCALE_SGROUPING shape, e.g. [3] -> "3;0", [3,2] -> "3;2;0".
        return string.Join(";", sizes) + ";0";
    }

    private static string CalendarToIdentifier(Calendar calendar) =>
        calendar switch
        {
            HebrewCalendar => "hebrew",
            HijriCalendar => "hijri",
            JapaneseCalendar => "japanese",
            KoreanCalendar => "korean",
            TaiwanCalendar => "taiwan",
            ThaiBuddhistCalendar => "thai",
            UmAlQuraCalendar => "umalqura",
            _ => "gregorian",
        };
}
