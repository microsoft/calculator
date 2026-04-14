// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.Globalization.NumberFormatting;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.ViewModel.Common
{
    public enum LanguageFontType
    {
        UIText,
        UICaption,
        UITitle,
        UIHeading,
        UIBody
    }

    /// <summary>
    /// Provides locale-aware settings for number formatting, digit symbols, separators, etc.
    /// This is the C# port of the C++ LocalizationSettings class.
    /// </summary>
    public sealed class LocalizationSettings
    {
        private static readonly Lazy<LocalizationSettings> s_instance = new Lazy<LocalizationSettings>(() =>
        {
            try
            {
                var locService = new DecimalFormatter();
                return new LocalizationSettings(locService);
            }
            catch
            {
                // Fallback: create with a default formatter for en-US
                return new LocalizationSettings(null);
            }
        });

        private readonly char[] _digitSymbols = new char[10];
        private static readonly char[] s_hexSymbols = { 'A', 'B', 'C', 'D', 'E', 'F' };

        private char _decimalSeparator;
        private char _numberGroupSeparator;
        private string _numberGrouping;
        private string _listSeparator;
        private string _calendarIdentifier;
        private Windows.Globalization.DayOfWeek _firstDayOfWeek;
        private int _currencySymbolPrecedence;
        private string _resolvedName;
        private int _currencyTrailingDigits;

        public LocalizationSettings(DecimalFormatter formatter)
        {
            if (formatter != null)
            {
                Initialize(formatter);
            }
            else
            {
                // Fallback: en-US defaults
                for (int i = 0; i < 10; i++) _digitSymbols[i] = (char)('0' + i);
                _decimalSeparator = '.';
                _numberGroupSeparator = ',';
                _numberGrouping = "3;0";
                _listSeparator = ",";
                _calendarIdentifier = "GregorianCalendar";
                _firstDayOfWeek = Windows.Globalization.DayOfWeek.Sunday;
                _resolvedName = "en-US";
            }
        }

        public static LocalizationSettings GetInstance() => s_instance.Value;

        public string GetLocaleName() => _resolvedName;

        public bool IsDigitEnUsSetting() => GetDigitSymbolFromEnUsDigit('0') == '0';

        public string GetEnglishValueFromLocalizedDigits(string localizedString)
        {
            if (_resolvedName == "en-US")
            {
                return localizedString;
            }

            var sb = new StringBuilder(localizedString.Length);
            foreach (char ch in localizedString)
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
                if (result == _decimalSeparator)
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
                if (c != ' ' && c != _numberGroupSeparator)
                {
                    sb.Append(c);
                }
            }
            return sb.ToString();
        }

        public string GetCalendarIdentifier() => _calendarIdentifier;
        public Windows.Globalization.DayOfWeek GetFirstDayOfWeek() => _firstDayOfWeek;
        public int GetCurrencyTrailingDigits() => _currencyTrailingDigits;
        public int GetCurrencySymbolPrecedence() => _currencySymbolPrecedence;
        public char GetDecimalSeparator() => _decimalSeparator;
        public char GetNumberGroupSeparator() => _numberGroupSeparator;

        public char GetDigitSymbolFromEnUsDigit(char digitSymbol)
        {
            int digit = digitSymbol - '0';
            if (digit < 0 || digit > 9)
                throw new ArgumentOutOfRangeException(nameof(digitSymbol));
            return _digitSymbols[digit];
        }

        public bool IsEnUsDigit(char digit) => digit >= '0' && digit <= '9';

        public bool IsLocalizedDigit(char digit) => Array.IndexOf(_digitSymbols, digit) >= 0;

        public bool IsLocalizedHexDigit(char digit)
        {
            return IsLocalizedDigit(digit) || Array.IndexOf(s_hexSymbols, digit) >= 0;
        }

        public string GetListSeparator() => _listSeparator;
        public string GetDecimalSeparatorStr() => new string(_decimalSeparator, 1);
        public string GetNumberGroupingSeparatorStr() => new string(_numberGroupSeparator, 1);
        public string GetNumberGroupingStr() => _numberGrouping;

        public FlowDirection GetFlowDirection()
        {
            return IsRtlLayout() ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;
        }

        public string GetLanguage()
        {
            return _resolvedName;
        }

        public FontFamily GetLanguageFontFamilyForType(LanguageFontType type)
        {
            var languageFontGroup = new Windows.Globalization.Fonts.LanguageFontGroup(_resolvedName);
            Windows.Globalization.Fonts.LanguageFont languageFont;
            switch (type)
            {
                case LanguageFontType.UICaption:
                    languageFont = languageFontGroup.UICaptionFont;
                    break;
                case LanguageFontType.UITitle:
                    languageFont = languageFontGroup.UITitleFont;
                    break;
                case LanguageFontType.UIHeading:
                    languageFont = languageFontGroup.UIHeadingFont;
                    break;
                case LanguageFontType.UIText:
                default:
                    languageFont = languageFontGroup.UITextFont;
                    break;
            }
            return new FontFamily(languageFont.FontFamily);
        }

        public DateTimeFormatter GetRegionalSettingsAwareDateTimeFormatter(string format)
        {
            return new DateTimeFormatter(format, new[] { _resolvedName });
        }

        public DateTimeFormatter GetRegionalSettingsAwareDateTimeFormatter(string format, string calendarIdentifier, string clockIdentifier)
        {
            return new DateTimeFormatter(format, new[] { _resolvedName }, "ZZ", calendarIdentifier, clockIdentifier);
        }

        public DecimalFormatter GetRegionalSettingsAwareDecimalFormatter()
        {
            return new DecimalFormatter(new[] { _resolvedName }, "ZZ");
        }

        public string GetListSeparatorWinRT()
        {
            return _listSeparator;
        }

        public string GetDecimalSeparatorStrWinRT()
        {
            return new string(_decimalSeparator, 1);
        }

        public bool IsRtlLayout()
        {
            try
            {
                var flowDirection = Windows.ApplicationModel.Resources.Core.ResourceContext.GetForCurrentView()
                    .QualifierValues["LayoutDirection"];
                return flowDirection == "RTL";
            }
            catch
            {
                return false;
            }
        }

        public static string GetNarratorReadableToken(string token)
        {
            return token;
        }

        public static string GetNarratorReadableString(string value)
        {
            return value;
        }

        public void LocalizeDisplayValue(ref string stringToLocalize)
        {
            if (IsDigitEnUsSetting())
            {
                return;
            }

            var sb = new StringBuilder(stringToLocalize.Length);
            foreach (char ch in stringToLocalize)
            {
                if (IsEnUsDigit(ch))
                {
                    sb.Append(GetDigitSymbolFromEnUsDigit(ch));
                }
                else
                {
                    sb.Append(ch);
                }
            }
            stringToLocalize = sb.ToString();
        }

        private void Initialize(DecimalFormatter formatter)
        {
            formatter.FractionDigits = 0;
            formatter.IsDecimalPointAlwaysDisplayed = false;

            for (uint i = 0; i < 10; i++)
            {
                string formatted = formatter.FormatUInt(i);
                _digitSymbols[i] = formatted[0];
            }

            string resolvedLanguage = formatter.ResolvedLanguage;

            // ResolveLocaleName
            var buffer = new char[85]; // LOCALE_NAME_MAX_LENGTH
            int result = ResolveLocaleName(resolvedLanguage, buffer, buffer.Length);
            if (result == 0)
            {
                throw new InvalidOperationException("Unexpected error resolving locale name");
            }

            _resolvedName = new string(buffer, 0, result - 1); // Remove null terminator

            // Get decimal separator
            var decimalString = new char[16];
            result = GetLocaleInfoEx(_resolvedName, 0x0000000E /* LOCALE_SDECIMAL */, decimalString, decimalString.Length);
            if (result == 0)
            {
                throw new InvalidOperationException("Unexpected error while getting locale info (decimal)");
            }
            _decimalSeparator = decimalString[0];

            // Get grouping symbol
            var groupingSymbolString = new char[16];
            result = GetLocaleInfoEx(_resolvedName, 0x0000000F /* LOCALE_STHOUSAND */, groupingSymbolString, groupingSymbolString.Length);
            if (result == 0)
            {
                throw new InvalidOperationException("Unexpected error while getting locale info (thousand)");
            }
            _numberGroupSeparator = groupingSymbolString[0];

            // Get number grouping
            var numberGroupingString = new char[16];
            result = GetLocaleInfoEx(_resolvedName, 0x00000010 /* LOCALE_SGROUPING */, numberGroupingString, numberGroupingString.Length);
            if (result == 0)
            {
                throw new InvalidOperationException("Unexpected error while getting locale info (grouping)");
            }
            _numberGrouping = new string(numberGroupingString, 0, result - 1);

            // Get list separator
            var listSeparatorString = new char[4];
            result = GetLocaleInfoEx(_resolvedName, 0x0000000C /* LOCALE_SLIST */, listSeparatorString, listSeparatorString.Length);
            if (result == 0)
            {
                throw new InvalidOperationException("Unexpected error while getting locale info (list separator)");
            }
            _listSeparator = new string(listSeparatorString, 0, result - 1);

            // Get currency trailing digits
            int currencyTrailingDigits = 0;
            result = GetLocaleInfoEx(_resolvedName, 0x00000019 | 0x20000000 /* LOCALE_ICURRDIGITS | LOCALE_RETURN_NUMBER */, ref currencyTrailingDigits, sizeof(int) / sizeof(char));
            if (result == 0)
            {
                throw new InvalidOperationException("Unexpected error while getting locale info (currency digits)");
            }
            _currencyTrailingDigits = currencyTrailingDigits;

            // Currency symbol precedence
            int currencySymbolPrecedence = 1;
            GetLocaleInfoEx(_resolvedName, 0x00000034 | 0x20000000 /* LOCALE_IPOSSYMPRECEDES | LOCALE_RETURN_NUMBER */, ref currencySymbolPrecedence, sizeof(int) / sizeof(char));
            _currencySymbolPrecedence = currencySymbolPrecedence;

            // Calendar type
            int calId = 0;
            GetLocaleInfoEx(_resolvedName, 0x00001009 | 0x20000000 /* LOCALE_ICALENDARTYPE | LOCALE_RETURN_NUMBER */, ref calId, sizeof(int) / sizeof(char));
            _calendarIdentifier = GetCalendarIdentifierFromCalid(calId);

            // First day of week
            var dayBuffer = new char[80];
            result = GetLocaleInfoEx(_resolvedName, 0x0000100C /* LOCALE_IFIRSTDAYOFWEEK */, dayBuffer, dayBuffer.Length);
            int dayValue = 0;
            if (result > 0)
            {
                int.TryParse(new string(dayBuffer, 0, result - 1), out dayValue);
            }
            // LOCALE_IFIRSTDAYOFWEEK: 0=Monday ... 6=Sunday
            // Windows.Globalization.DayOfWeek: 0=Sunday, 1=Monday ... 6=Saturday
            _firstDayOfWeek = (Windows.Globalization.DayOfWeek)((dayValue + 1) % 7);
        }

        private static string GetCalendarIdentifierFromCalid(int calId)
        {
            switch (calId)
            {
                case 1: // CAL_GREGORIAN
                case 10: // CAL_GREGORIAN_ARABIC
                case 9: // CAL_GREGORIAN_ME_FRENCH
                case 2: // CAL_GREGORIAN_US
                case 11: // CAL_GREGORIAN_XLIT_ENGLISH
                case 12: // CAL_GREGORIAN_XLIT_FRENCH
                    return CalendarIdentifiers.Gregorian;
                case 8: // CAL_HEBREW
                    return CalendarIdentifiers.Hebrew;
                case 6: // CAL_HIJRI
                case 22: // CAL_PERSIAN
                    return CalendarIdentifiers.Hijri;
                case 3: // CAL_JAPAN
                    return CalendarIdentifiers.Japanese;
                case 5: // CAL_KOREA
                    return CalendarIdentifiers.Korean;
                case 4: // CAL_TAIWAN
                    return CalendarIdentifiers.Taiwan;
                case 7: // CAL_THAI
                    return CalendarIdentifiers.Thai;
                case 23: // CAL_UMALQURA
                    return CalendarIdentifiers.UmAlQura;
                default:
                    return CalendarIdentifiers.Gregorian;
            }
        }

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern int ResolveLocaleName(string lpNameToResolve, [Out] char[] lpLocaleName, int cchLocaleName);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern int GetLocaleInfoEx(string lpLocaleName, uint LCType, [Out] char[] lpLCData, int cchData);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern int GetLocaleInfoEx(string lpLocaleName, uint LCType, ref int lpLCData, int cchData);
    }
}
