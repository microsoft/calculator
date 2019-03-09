// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "LocalizationService.h"

namespace CalculatorApp
{
    namespace Common
    {
        class LocalizationSettings
        {
        private:
            LocalizationSettings()
            {
                int result = 0;

                // Use DecimalFormatter as it respects the locale and the user setting
                Windows::Globalization::NumberFormatting::DecimalFormatter^ formatter;
                formatter = CalculatorApp::Common::LocalizationService::GetRegionalSettingsAwareDecimalFormatter();
                formatter->FractionDigits = 0;
                formatter->IsDecimalPointAlwaysDisplayed = false;

                for (unsigned int i = 0; i < 10; i++)
                {
                    m_digitSymbols.at(i) = formatter->FormatUInt(i)->Data()[0];
                }

                wchar_t resolvedName[LOCALE_NAME_MAX_LENGTH];
                result = ResolveLocaleName(formatter->ResolvedLanguage->Data(),
                    resolvedName,
                    LOCALE_NAME_MAX_LENGTH);
                if (result == 0)
                {
                    throw std::runtime_error("Unexpected error resolving locale name");
                }
                else
                {
                    m_resolvedName = resolvedName;
                    wchar_t decimalString[LocaleSettingBufferSize] = L"";
                    result = GetLocaleInfoEx(m_resolvedName.c_str(),
                        LOCALE_SDECIMAL,
                        decimalString,
                        ARRAYSIZE(decimalString));
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    wchar_t groupingSymbolString[LocaleSettingBufferSize] = L"";
                    result = GetLocaleInfoEx(m_resolvedName.c_str(),
                        LOCALE_STHOUSAND,
                        groupingSymbolString,
                        ARRAYSIZE(groupingSymbolString));
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    wchar_t numberGroupingString[LocaleSettingBufferSize] = L"";
                    result = GetLocaleInfoEx(m_resolvedName.c_str(),
                        LOCALE_SGROUPING,
                        numberGroupingString,
                        ARRAYSIZE(numberGroupingString));
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    // Get locale info for List Separator, eg. comma is used in many locales
                    wchar_t listSeparatorString[4] = L"";
                    result = ::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                        LOCALE_SLIST,
                        listSeparatorString,
                        ARRAYSIZE(listSeparatorString)); // Max length of the expected return value is 4
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    int currencyTrailingDigits = 0;
                    result = GetLocaleInfoEx(m_resolvedName.c_str(),
                        LOCALE_ICURRDIGITS | LOCALE_RETURN_NUMBER,
                        (LPWSTR)&currencyTrailingDigits,
                        sizeof(currencyTrailingDigits) / sizeof(WCHAR));
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    // Currency symbol precedence is either 0 or 1.
                    // A value of 0 indicates the symbol follows the currency value.
                    int currencySymbolPrecedence = 1;
                    result = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                        LOCALE_IPOSSYMPRECEDES | LOCALE_RETURN_NUMBER,
                        (LPWSTR)&currencySymbolPrecedence,
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
                ::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                    LOCALE_ICALENDARTYPE | LOCALE_RETURN_NUMBER,
                    reinterpret_cast<PWSTR>(&calId),
                    sizeof(calId));

                m_calendarIdentifier = GetCalendarIdentifierFromCalid(calId);

                // Get FirstDayOfWeek Date and Time setting
                wchar_t day[80] = L"";
                ::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                    LOCALE_IFIRSTDAYOFWEEK,                     // The first day in a week
                    reinterpret_cast<PWSTR>(day),               // Argument is of type PWSTR
                    ARRAYSIZE(day));                            // Max return size are 80 characters

                // The LOCALE_IFIRSTDAYOFWEEK integer value varies from 0, 1, .. 6 for Monday, Tuesday, ... Sunday
                // DayOfWeek enum value varies from 0, 1, .. 6 for Sunday, Monday, ... Saturday
                // Hence, DayOfWeek = (valueof(LOCALE_IFIRSTDAYOFWEEK) + 1) % 7
                m_firstDayOfWeek = static_cast<Windows::Globalization::DayOfWeek>((_wtoi(day) + 1) % 7);    // static cast int to DayOfWeek enum
            }

        public:
            // A LocalizationSettings object is not copyable.
            LocalizationSettings(const LocalizationSettings&) = delete;
            LocalizationSettings& operator=(const LocalizationSettings&) = delete;

            // A LocalizationSettings object is not moveable.
            LocalizationSettings(LocalizationSettings&&) = delete;
            LocalizationSettings& operator=(LocalizationSettings&&) = delete;

            // Provider of the singleton LocalizationSettings instance.
            static const LocalizationSettings& GetInstance()
            {
                static const LocalizationSettings localizationSettings;

                return localizationSettings;
            }

            Platform::String^ GetLocaleName() const
            {
                return ref new Platform::String(m_resolvedName.c_str());
            }

            bool IsDigitEnUsSetting() const
            {
                if (this->GetDigitSymbolFromEnUsDigit('0') == L'0')
                {
                    return true;
                }
                return false;
            }

            void LocalizeDisplayValue(_Inout_ std::wstring* stringToLocalize) const
            {
                if (IsDigitEnUsSetting())
                {
                    return;
                }

                for (wchar_t& ch : *stringToLocalize)
                {
                    if (IsEnUsDigit(ch))
                    {
                        ch = GetDigitSymbolFromEnUsDigit(ch);
                    }
                }
            }

            Platform::String^ GetEnglishValueFromLocalizedDigits(const std::wstring& localizedString) const
            {
                if (m_resolvedName == L"en-US")
                {
                    return ref new Platform::String(localizedString.c_str());
                }

                size_t i = 0;
                size_t length = localizedString.size();
                std::unique_ptr<wchar_t[]> englishString(new wchar_t[length + 1]); // +1 for the null termination

                for (; i < length; ++i)
                {
                    wchar_t ch = localizedString[i];
                    if (!IsEnUsDigit(ch))
                    {
                        for (int j = 0; j < 10; ++j)
                        {
                            if (ch == m_digitSymbols[j])
                            {
                                ch = j.ToString()->Data()[0];
                                break;
                                //ch = val - L'0';
                            }
                        }
                    }
                    if (ch == m_decimalSeparator)
                    {
                        ch = L'.';
                    }
                    englishString[i] = ch;
                }
                englishString[i] = '\0';

                return ref new Platform::String(englishString.get());
            }

            bool IsEnUsDigit(const wchar_t digit) const
            {
                if (digit >= L'0' && digit <= L'9')
                {
                    return true;
                }
                return false;
            }

            bool IsLocalizedDigit(const wchar_t digit) const
            {
                for (auto dig : m_digitSymbols)
                {
                    if (digit == dig)
                    {
                        return true;
                    }
                }
                return false;
            }

            bool IsLocalizedHexDigit(const wchar_t digit) const
            {
                if (IsLocalizedDigit(digit))
                {
                    return true;
                }

                for (auto dig : s_hexSymbols)
                {
                    if (digit == dig)
                    {
                        return true;
                    }
                }

                return false;
            }

            wchar_t GetDigitSymbolFromEnUsDigit(wchar_t digitSymbol) const
            {
                assert(digitSymbol >= L'0' && digitSymbol <= L'9');
                int digit = digitSymbol - L'0';
                return m_digitSymbols.at(digit); // throws on out of range
            }

            wchar_t GetDecimalSeparator() const
            {
                return m_decimalSeparator;
            }

            wchar_t GetNumberGroupSeparator() const
            {
                return m_numberGroupSeparator;
            }

            std::wstring GetDecimalSeparatorStr() const
            {
                std::wstring result;
                result.push_back(m_decimalSeparator);
                return result;
            }

            std::wstring GetNumberGroupingSeparatorStr() const
            {
                std::wstring result;
                result.push_back(m_numberGroupSeparator);
                return result;
            }

            std::wstring GetNumberGroupingStr() const
            {
                return m_numberGrouping;
            }

            void RemoveGroupSeparators(const wchar_t* value, const size_t length, std::wstring* rawValue) const
            {
                rawValue->clear();
                rawValue->reserve(length);

                for (size_t i = 0; i < length; i++)
                {
                    if (value[i] != L' ' && value[i] != m_numberGroupSeparator)
                    {
                        rawValue->append(1, value[i]);
                    }
                }
            }

            Platform::String^ GetCalendarIdentifier() const
            {
                return m_calendarIdentifier;
            }

            std::wstring GetListSeparator() const
            {
                return m_listSeparator;
            }

            Windows::Globalization::DayOfWeek GetFirstDayOfWeek() const
            {
                return m_firstDayOfWeek;
            }

            int GetCurrencyTrailingDigits() const
            {
                return m_currencyTrailingDigits;
            }

            int GetCurrencySymbolPrecedence() const
            {
                return m_currencySymbolPrecedence;
            }

        private:
            static Platform::String^ GetCalendarIdentifierFromCalid(CALID calId)
            {
                switch (calId)
                {
                case CAL_GREGORIAN:
                case CAL_GREGORIAN_ARABIC:
                case CAL_GREGORIAN_ME_FRENCH:
                case CAL_GREGORIAN_US:
                case CAL_GREGORIAN_XLIT_ENGLISH:
                case CAL_GREGORIAN_XLIT_FRENCH:
                    return Windows::Globalization::CalendarIdentifiers::Gregorian;

                case CAL_HEBREW:
                    return Windows::Globalization::CalendarIdentifiers::Hebrew;

                case CAL_HIJRI:
                case CAL_PERSIAN:
                    return Windows::Globalization::CalendarIdentifiers::Hijri;

                case CAL_JAPAN:
                    return Windows::Globalization::CalendarIdentifiers::Japanese;

                case CAL_KOREA:
                    return Windows::Globalization::CalendarIdentifiers::Korean;

                case CAL_TAIWAN:
                    return Windows::Globalization::CalendarIdentifiers::Taiwan;

                case CAL_THAI:
                    return Windows::Globalization::CalendarIdentifiers::Thai;

                case CAL_UMALQURA:
                    return Windows::Globalization::CalendarIdentifiers::UmAlQura;

                    // Gregorian will be the default Calendar Type
                default:
                    return Windows::Globalization::CalendarIdentifiers::Gregorian;
                }
            }

            wchar_t m_decimalSeparator;
            wchar_t m_numberGroupSeparator;
            std::wstring m_numberGrouping;
            std::array<wchar_t, 10> m_digitSymbols;
            // Hexadecimal characters are not currently localized
            static constexpr std::array<wchar_t, 6> s_hexSymbols{ L'A', L'B', L'C', L'D', L'E', L'F' };
            std::wstring m_listSeparator;
            Platform::String^ m_calendarIdentifier;
            Windows::Globalization::DayOfWeek m_firstDayOfWeek;
            int m_currencySymbolPrecedence;
            std::wstring m_resolvedName;
            int m_currencyTrailingDigits;
            static const unsigned int LocaleSettingBufferSize = 16;
        };
    }
}
