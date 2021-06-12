// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "LocalizationService.h"

#include <iterator>

namespace CalculatorApp
{
    namespace Common
    {
        class LocalizationSettings
        {
        private:
            LocalizationSettings()
                // Use DecimalFormatter as it respects the locale and the user setting
                : LocalizationSettings(LocalizationService::GetInstance()->GetRegionalSettingsAwareDecimalFormatter())
            {
            }

        public:
            // This is only public for unit testing purposes.
            LocalizationSettings(Windows::Globalization::NumberFormatting::DecimalFormatter ^ formatter)
            {
                formatter->FractionDigits = 0;
                formatter->IsDecimalPointAlwaysDisplayed = false;

                for (unsigned int i = 0; i < m_digitSymbols.size(); i++)
                {
                    m_digitSymbols[i] = formatter->FormatUInt(i)->Data()[0];
                }

                wchar_t resolvedName[LOCALE_NAME_MAX_LENGTH];
                int result = ResolveLocaleName(formatter->ResolvedLanguage->Data(), resolvedName, LOCALE_NAME_MAX_LENGTH);
                if (result == 0)
                {
                    throw std::runtime_error("Unexpected error resolving locale name");
                }
                else
                {
                    m_resolvedName = ref new Platform::String(resolvedName);
                    wchar_t decimalString[LocaleSettingBufferSize] = L"";
                    result = GetLocaleInfoEx(m_resolvedName->Data(), LOCALE_SDECIMAL, decimalString, static_cast<int>(std::size(decimalString)));
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    wchar_t groupingSymbolString[LocaleSettingBufferSize] = L"";
                    result = GetLocaleInfoEx(m_resolvedName->Data(), LOCALE_STHOUSAND, groupingSymbolString, static_cast<int>(std::size(groupingSymbolString)));
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    wchar_t numberGroupingString[LocaleSettingBufferSize] = L"";
                    result = GetLocaleInfoEx(m_resolvedName->Data(), LOCALE_SGROUPING, numberGroupingString, static_cast<int>(std::size(numberGroupingString)));
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    // Get locale info for List Separator, eg. comma is used in many locales
                    wchar_t listSeparatorString[4] = L"";
                    result = ::GetLocaleInfoEx(
                        m_resolvedName->Data(),
                        LOCALE_SLIST,
                        listSeparatorString,
                        static_cast<int>(std::size(listSeparatorString))); // Max length of the expected return value is 4
                    if (result == 0)
                    {
                        throw std::runtime_error("Unexpected error while getting locale info");
                    }

                    int currencyTrailingDigits = 0;
                    result = GetLocaleInfoEx(
                        m_resolvedName->Data(),
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
                    result = GetLocaleInfoEx(
                        m_resolvedName->Data(),
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
                ::GetLocaleInfoEx(m_resolvedName->Data(), LOCALE_ICALENDARTYPE | LOCALE_RETURN_NUMBER, reinterpret_cast<PWSTR>(&calId), sizeof(calId));

                m_calendarIdentifier = GetCalendarIdentifierFromCalid(calId);

                // Get FirstDayOfWeek Date and Time setting
                wchar_t day[80] = L"";
                ::GetLocaleInfoEx(
                    m_resolvedName->Data(),
                    LOCALE_IFIRSTDAYOFWEEK,            // The first day in a week
                    reinterpret_cast<PWSTR>(day),      // Argument is of type PWSTR
                    static_cast<int>(std::size(day))); // Max return size are 80 characters

                // The LOCALE_IFIRSTDAYOFWEEK integer value varies from 0, 1, .. 6 for Monday, Tuesday, ... Sunday
                // DayOfWeek enum value varies from 0, 1, .. 6 for Sunday, Monday, ... Saturday
                // Hence, DayOfWeek = (valueof(LOCALE_IFIRSTDAYOFWEEK) + 1) % 7
                m_firstDayOfWeek = static_cast<Windows::Globalization::DayOfWeek>((_wtoi(day) + 1) % 7); // static cast int to DayOfWeek enum
            }

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

            Platform::String ^ GetLocaleName() const
            {
                return m_resolvedName;
            }

            bool IsDigitEnUsSetting() const
            {
                return (this->GetDigitSymbolFromEnUsDigit('0') == L'0');
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

            Platform::String ^ GetEnglishValueFromLocalizedDigits(Platform::String ^ localizedString) const
            {
                if (m_resolvedName == L"en-US")
                {
                    return localizedString;
                }

                std::wstring englishString;
                englishString.reserve(localizedString->Length());

                for (wchar_t ch : localizedString)
                {
                    if (!IsEnUsDigit(ch))
                    {
                        auto it = std::find(m_digitSymbols.begin(), m_digitSymbols.end(), ch);

                        if (it != m_digitSymbols.end())
                        {
                            auto index = std::distance(m_digitSymbols.begin(), it);
                            ch = index.ToString()->Data()[0];
                        }
                    }
                    if (ch == m_decimalSeparator)
                    {
                        ch = L'.';
                    }
                    englishString += ch;
                }

                return ref new Platform::String(englishString.c_str());
            }

            bool IsEnUsDigit(const wchar_t digit) const
            {
                return (digit >= L'0' && digit <= L'9');
            }

            bool IsLocalizedDigit(const wchar_t digit) const
            {
                return std::find(m_digitSymbols.begin(), m_digitSymbols.end(), digit) != m_digitSymbols.end();
            }

            bool IsLocalizedHexDigit(const wchar_t digit) const
            {
                if (IsLocalizedDigit(digit))
                {
                    return true;
                }

                return std::find(s_hexSymbols.begin(), s_hexSymbols.end(), digit) != s_hexSymbols.end();
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
                return std::wstring(1, m_decimalSeparator);
            }

            std::wstring GetNumberGroupingSeparatorStr() const
            {
                return std::wstring(1, m_numberGroupSeparator);
            }

            std::wstring GetNumberGroupingStr() const
            {
                return m_numberGrouping;
            }

            Platform::String ^ RemoveGroupSeparators(Platform::String ^ source) const
            {
                std::wstring destination;
                std::copy_if(
                    begin(source), end(source), std::back_inserter(destination), [this](auto const c) { return c != L' ' && c != m_numberGroupSeparator; });

                return ref new Platform::String(destination.c_str());
            }

            Platform::String ^ GetCalendarIdentifier() const
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
            Platform::String ^ m_calendarIdentifier;
            Windows::Globalization::DayOfWeek m_firstDayOfWeek;
            int m_currencySymbolPrecedence;
            Platform::String ^ m_resolvedName;
            int m_currencyTrailingDigits;
            static const unsigned int LocaleSettingBufferSize = 16;
        };
    }
}
