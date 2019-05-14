// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using System;
using System.Diagnostics;
using System.Globalization;
using System.Text;

namespace CalculatorApp
{
    namespace Common
    {
        public class LocalizationSettings
        {
            const int LOCALE_NAME_MAX_LENGTH = 85;
            const int LocaleSettingBufferSize = 16;

            public LocalizationSettings()
            {
#if false
                int result = 0;

                // Use DecimalFormatter as it respects the locale and the user setting
                Windows.Globalization.NumberFormatting.DecimalFormatter formatter;
                formatter = CalculatorApp.Common.LocalizationService.GetRegionalSettingsAwareDecimalFormatter();
                formatter.FractionDigits = 0;
                formatter.IsDecimalPointAlwaysDisplayed = false;

                for (uint i = 0; i < 10; i++)
                {
                    m_digitSymbols[i] = formatter.FormatUInt(i)[0];
                }

                char[] resolvedName = new char[LOCALE_NAME_MAX_LENGTH];
                result = ResolveLocaleName(formatter.ResolvedLanguage.Data(), resolvedName, LOCALE_NAME_MAX_LENGTH);
                if (result == 0)
                {
                    throw new Exception("Unexpected error resolving locale name");
                }
                else
                {
                    m_resolvedName = resolvedName;
                    char[] decimalString = new char[LocaleSettingBufferSize];
                    result = GetLocaleInfoEx(m_resolvedName, LOCALE_SDECIMAL, decimalString, (int)(std.size(decimalString)));
                    if (result == 0)
                    {
                        throw new Exception("Unexpected error while getting locale info");
                    }

                    char[] groupingSymbolString = new char[LocaleSettingBufferSize];
                    result = GetLocaleInfoEx(m_resolvedName, LOCALE_STHOUSAND, groupingSymbolString, (int)(std.size(groupingSymbolString)));
                    if (result == 0)
                    {
                        throw new Exception("Unexpected error while getting locale info");
                    }

                    char[] numberGroupingString = new char[LocaleSettingBufferSize];
                    result = GetLocaleInfoEx(m_resolvedName, LOCALE_SGROUPING, numberGroupingString, (int)(std.size(numberGroupingString)));
                    if (result == 0)
                    {
                        throw new Exception("Unexpected error while getting locale info");
                    }

                    // Get locale info for List Separator, eg. comma is used in many locales
                    char[] listSeparatorString = new char[4];
                    result = GetLocaleInfoEx(
                        LOCALE_NAME_USER_DEFAULT,
                        LOCALE_SLIST,
                        listSeparatorString,
                        (int)(std.size(listSeparatorString))); // Max length of the expected return value is 4
                    if (result == 0)
                    {
                        throw new Exception("Unexpected error while getting locale info");
                    }

                    int currencyTrailingDigits = 0;
                    result = GetLocaleInfoEx(
                        m_resolvedName,
                        LOCALE_ICURRDIGITS | LOCALE_RETURN_NUMBER,
                        (LPWSTR) & currencyTrailingDigits,
                        sizeof(currencyTrailingDigits) / sizeof(WCHAR));
                    if (result == 0)
                    {
                        throw new Exception("Unexpected error while getting locale info");
                    }

                    // Currency symbol precedence is either 0 or 1.
                    // A value of 0 indicates the symbol follows the currency value.
                    int currencySymbolPrecedence = 1;
                    result = GetLocaleInfoEx(
                        LOCALE_NAME_USER_DEFAULT,
                        LOCALE_IPOSSYMPRECEDES | LOCALE_RETURN_NUMBER,
                        (LPWSTR) & currencySymbolPrecedence,
                        sizeof(currencySymbolPrecedence) / sizeof(WCHAR));

                    // As CalcEngine only supports the first character of the decimal separator,
                    // Only first character of the decimal separator string is supported.
                    m_decimalSeparator = decimalString[0];
                    m_numberGroupSeparator = groupingSymbolString[0];
                    m_numberGrouping = numberGroupingString;
                    m_listSeparator = listSeparatorString;
                    m_currencyTrailingDigits = currencyTrailingDigits;
                    m_currencySymbolPrecedence = currencySymbolPrecedence;
                }

                // Get the system calendar type
                // Note: This function returns 0 on failure.
                // We'll ignore the failure in that case and the CalendarIdentifier would get set to GregorianCalendar.
                CALID calId;
                GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_ICALENDARTYPE | LOCALE_RETURN_NUMBER, reinterpret_cast<PWSTR>(&calId), sizeof(calId));

                m_calendarIdentifier = GetCalendarIdentifierFromCalid(calId);

                // Get FirstDayOfWeek Date and Time setting
                char day[80] = "";
                GetLocaleInfoEx(
                    LOCALE_NAME_USER_DEFAULT,
                    LOCALE_IFIRSTDAYOFWEEK,            // The first day in a week
                    reinterpret_cast<PWSTR>(day),      // Argument is of type PWSTR
                    (int)(std.size(day))); // Max return size are 80 characters

                // The LOCALE_IFIRSTDAYOFWEEK integer value varies from 0, 1, .. 6 for Monday, Tuesday, ... Sunday
                // DayOfWeek enum value varies from 0, 1, .. 6 for Sunday, Monday, ... Saturday
                // Hence, DayOfWeek = (valueof(LOCALE_IFIRSTDAYOFWEEK) + 1) % 7
                m_firstDayOfWeek = (Windows.Globalization.DayOfWeek)((_wtoi(day) + 1) % 7); // static cast int to DayOfWeek enum
#else
                for (uint i = 0; i < 10; i++)
                {
                    m_digitSymbols[i] = i.ToString(NumberFormatInfo.CurrentInfo)[0];
                }

                // As CalcEngine only supports the first character of the decimal separator,
                // Only first character of the decimal separator string is supported.
                m_decimalSeparator = NumberFormatInfo.CurrentInfo.NumberDecimalSeparator[0];
                m_numberGroupSeparator = NumberFormatInfo.CurrentInfo.NumberGroupSeparator[0];
                m_numberGrouping = ""; // UNO TODO https://docs.microsoft.com/en-us/windows/desktop/Intl/locale-sgrouping
                m_listSeparator = CultureInfo.CurrentCulture.TextInfo.ListSeparator;
                m_currencyTrailingDigits = NumberFormatInfo.CurrentInfo.CurrencyDecimalDigits;
                m_currencySymbolPrecedence = ~(NumberFormatInfo.CurrentInfo.CurrencyPositivePattern) & 1;

                m_calendarIdentifier = "";//CultureInfo.CurrentCulture.DateTimeFormat.Calendar.ToString();

                m_firstDayOfWeek = (Windows.Globalization.DayOfWeek)CultureInfo.CurrentCulture.DateTimeFormat.FirstDayOfWeek;
#endif
            }

            static LocalizationSettings m_localizationSettings;

            // Provider of the singleton LocalizationSettings instance.
            public static LocalizationSettings GetInstance()
            {
                if(m_localizationSettings== null)
                {
                    m_localizationSettings = new LocalizationSettings();
                }
                return m_localizationSettings;
            }

            string GetLocaleName()
            {
                return m_resolvedName;
            }

            bool IsDigitEnUsSetting()
            {
                if (this.GetDigitSymbolFromEnUsDigit('0') == '0')
                {
                    return true;
                }
                return false;
            }

            public void LocalizeDisplayValue(ref string stringToLocalize)
            {
                var b = new StringBuilder();

                if (IsDigitEnUsSetting())
                {
                    return;
                }

                foreach (char ch in stringToLocalize)
                {
                    if (IsEnUsDigit(ch))
                    {
                        b.Append(GetDigitSymbolFromEnUsDigit(ch));
                    }
                    else
                    {
                        b.Append(ch);
                    }
                }

                stringToLocalize = b.ToString();
            }

            public string GetEnglishValueFromLocalizedDigits(string localizedString)
            {
                if (m_resolvedName == "en-US")
                {
                    return localizedString;
                }

                int i = 0;
                int length = localizedString.Length;
                char[] englishString = new char[length + 1]; // +1 for the null termination

                for (; i < length; ++i)
                {
                    char ch = localizedString[i];
                    if (!IsEnUsDigit(ch))
                    {
                        for (int j = 0; j < 10; ++j)
                        {
                            if (ch == m_digitSymbols[j])
                            {
                                ch = j.ToString()[0];
                                break;
                                // ch = val - '0';
                            }
                        }
                    }
                    if (ch == m_decimalSeparator)
                    {
                        ch = '.';
                    }
                    englishString[i] = ch;
                }
                englishString[i] = '\0';

                return new string(englishString);
            }

            public bool IsEnUsDigit(char digit)
            {
                if (digit >= '0' && digit <= '9')
                {
                    return true;
                }
                return false;
            }

            public bool IsLocalizedDigit(char digit)
            {
                foreach(var dig in m_digitSymbols)
                {
                    if (digit == dig)
                    {
                        return true;
                    }
                }
                return false;
            }

            public bool IsLocalizedHexDigit(char digit)
            {
                if (IsLocalizedDigit(digit))
                {
                    return true;
                }

                foreach(var dig in s_hexSymbols)
                {
                    if (digit == dig)
                    {
                        return true;
                    }
                }

                return false;
            }

            public char GetDigitSymbolFromEnUsDigit(char digitSymbol)
            {
                Debug.Assert(digitSymbol >= '0' && digitSymbol <= '9');
                int digit = digitSymbol - '0';
                return m_digitSymbols[digit]; // throws on out of range
            }

            public char GetDecimalSeparator()
            {
                return m_decimalSeparator;
            }

            char GetNumberGroupSeparator()
            {
                return m_numberGroupSeparator;
            }

            public string GetDecimalSeparatorStr()
            {
                return m_decimalSeparator.ToString();
            }

            public string GetNumberGroupingSeparatorStr() 
            {
                return m_numberGroupSeparator.ToString();
            }

            public string GetNumberGroupingStr()
            {
                return m_numberGrouping;
            }

            public void RemoveGroupSeparators(string value, int length, ref string rawValue)
            {
                StringBuilder b = new StringBuilder(length);

                for (int i = 0; i < length; i++)
                {
                    if (value[i] != ' ' && value[i] != m_numberGroupSeparator)
                    {
                        b.Append(value[i]);
                    }
                }

                rawValue = b.ToString();
            }

            public string GetCalendarIdentifier()
            {
                return m_calendarIdentifier;
            }

            public string GetListSeparator()
            {
                return m_listSeparator;
            }

            Windows.Globalization.DayOfWeek GetFirstDayOfWeek()
            {
                return m_firstDayOfWeek;
            }

            public int GetCurrencyTrailingDigits()
            {
                return m_currencyTrailingDigits;
            }

            int GetCurrencySymbolPrecedence()
            {
                return m_currencySymbolPrecedence;
            }

            public static string GetCalendarIdentifierFromCalid(int calId)
            {
                switch (calId)
                {
                //case CAL_GREGORIAN:
                //case CAL_GREGORIAN_ARABIC:
                //case CAL_GREGORIAN_ME_FRENCH:
                //case CAL_GREGORIAN_US:
                //case CAL_GREGORIAN_XLIT_ENGLISH:
                //case CAL_GREGORIAN_XLIT_FRENCH:
                //    return Windows.Globalization.CalendarIdentifiers.Gregorian;

                //case CAL_HEBREW:
                //    return Windows.Globalization.CalendarIdentifiers.Hebrew;

                //case CAL_HIJRI:
                //case CAL_PERSIAN:
                //    return Windows.Globalization.CalendarIdentifiers.Hijri;

                //case CAL_JAPAN:
                //    return Windows.Globalization.CalendarIdentifiers.Japanese;

                //case CAL_KOREA:
                //    return Windows.Globalization.CalendarIdentifiers.Korean;

                //case CAL_TAIWAN:
                //    return Windows.Globalization.CalendarIdentifiers.Taiwan;

                //case CAL_THAI:
                //    return Windows.Globalization.CalendarIdentifiers.Thai;

                //case CAL_UMALQURA:
                //    return Windows.Globalization.CalendarIdentifiers.UmAlQura;

                    // Gregorian will be the default Calendar Type
                default:
                    return Windows.Globalization.CalendarIdentifiers.Gregorian;
                }
            }

            char m_decimalSeparator;
            char m_numberGroupSeparator;
            string m_numberGrouping;
            char[] m_digitSymbols = new char[10];
            // Hexadecimal characters are not currently localized
            static char[] s_hexSymbols = { 'A', 'B', 'C', 'D', 'E', 'F' };
            string m_listSeparator;
            string m_calendarIdentifier;
            Windows.Globalization.DayOfWeek m_firstDayOfWeek;
            int m_currencySymbolPrecedence;
            string m_resolvedName;
            int m_currencyTrailingDigits;
        }
    }
}
