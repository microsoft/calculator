// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DateCalculatorViewModel.h"
#include "Common/TraceLogger.h"
#include "Common/LocalizationStringUtil.h"
#include "Common/LocalizationService.h"
#include "Common/LocalizationSettings.h"
#include "Common/CopyPasteManager.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel::Common::DateCalculation;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::System::UserProfile;

namespace
{
    StringReference StrDateDiffResultPropertyName(L"StrDateDiffResult");
    StringReference StrDateDiffResultAutomationNamePropertyName(L"StrDateDiffResultAutomationName");
    StringReference StrDateDiffResultInDaysPropertyName(L"StrDateDiffResultInDays");
    StringReference StrDateResultPropertyName(L"StrDateResult");
    StringReference StrDateResultAutomationNamePropertyName(L"StrDateResultAutomationName");
    StringReference IsDiffInDaysPropertyName(L"IsDiffInDays");
}

DateCalculatorViewModel::DateCalculatorViewModel()
    : m_IsDateDiffMode(true)
    , m_IsAddMode(true)
    , m_isOutOfBound(false)
    , m_DaysOffset(0)
    , m_MonthsOffset(0)
    , m_YearsOffset(0)
    , m_StrDateDiffResult(L"")
    , m_StrDateDiffResultAutomationName(L"")
    , m_StrDateDiffResultInDays(L"")
    , m_StrDateResult(L"")
    , m_StrDateResultAutomationName(L"")
{
    LocalizationSettings^ localizationSettings = LocalizationSettings::GetInstance();

    // Initialize Date Output format instances
    InitializeDateOutputFormats(localizationSettings->GetCalendarIdentifier());

    // Initialize Date Calc engine
    m_dateCalcEngine = ref new DateCalculationEngine(localizationSettings->GetCalendarIdentifier());
    // Initialize dates of DatePicker controls to today's date
    auto calendar = ref new Calendar();
    // We force the timezone to UTC, in order to avoid being affected by Daylight Saving Time
    // when we calculate the difference between 2 dates.
    calendar->ChangeTimeZone("UTC");
    auto today = calendar->GetDateTime();

    // FromDate and ToDate should be clipped (adjusted to a consistent hour in UTC)
    m_fromDate = m_toDate = ClipTime(today);

    // StartDate should not be clipped
    m_startDate = today;
    m_dateResult = today;

    // Initialize the list separator delimiter appended with a space at the end, e.g. ", "
    // This will be used for date difference formatting: Y years, M months, W weeks, D days
    m_listSeparator = localizationSettings->GetListSeparator() + L" ";

    // Initialize the output results
    UpdateDisplayResult();

    m_offsetValues = ref new Vector<String ^>();
    for (int i = 0; i <= c_maxOffsetValue; i++)
    {
        wstring numberStr(to_wstring(i));
        localizationSettings->LocalizeDisplayValue(&numberStr);
        m_offsetValues->Append(ref new String(numberStr.c_str()));
    }

    DayOfWeek trueDayOfWeek = calendar->DayOfWeek;

    DateTime clippedTime = ClipTime(today);
    calendar->SetDateTime(clippedTime);
    if (calendar->DayOfWeek != trueDayOfWeek)
    {
        calendar->SetDateTime(today);
    }
}

void DateCalculatorViewModel::OnPropertyChanged(_In_ String ^ prop)
{
    if (prop == StrDateDiffResultPropertyName)
    {
        UpdateStrDateDiffResultAutomationName();
    }
    else if (prop == StrDateResultPropertyName)
    {
        UpdateStrDateResultAutomationName();
    }
    else if (
        prop != StrDateDiffResultAutomationNamePropertyName && prop != StrDateDiffResultInDaysPropertyName && prop != StrDateResultAutomationNamePropertyName
        && prop != IsDiffInDaysPropertyName)
    {
        OnInputsChanged();
    }
}

void DateCalculatorViewModel::OnInputsChanged()
{
    if (m_IsDateDiffMode)
    {
        DateTime clippedFromDate = ClipTime(FromDate);
        DateTime clippedToDate = ClipTime(ToDate);

        // Calculate difference between two dates
        auto dateDiff = m_dateCalcEngine->TryGetDateDifference(clippedFromDate, clippedToDate, m_daysOutputFormat);
        if (dateDiff != nullptr)
        {
            DateDiffResultInDays = dateDiff->Value;
            dateDiff = m_dateCalcEngine->TryGetDateDifference(clippedFromDate, clippedToDate, m_allDateUnitsOutputFormat);
            if (dateDiff != nullptr)
            {
                DateDiffResult = dateDiff->Value;
            }
            else
            {
                // TryGetDateDifference wasn't able to calculate the difference in days/weeks/months/years, we will instead display the difference in days.
                DateDiffResult = DateDiffResultInDays;
            }
        }
        else
        {
            DateDiffResult = DateDifferenceUnknown;
            DateDiffResultInDays = DateDifferenceUnknown;
        }
    }
    else
    {
        DateDifference dateDiff;
        dateDiff.day = DaysOffset;
        dateDiff.month = MonthsOffset;
        dateDiff.year = YearsOffset;

        IBox<DateTime> ^ dateTimeResult;

        if (m_IsAddMode)
        {
            // Add number of Days, Months and Years to a Date
            dateTimeResult = m_dateCalcEngine->AddDuration(StartDate, dateDiff);
        }
        else
        {
            // Subtract number of Days, Months and Years from a Date
            dateTimeResult = m_dateCalcEngine->SubtractDuration(StartDate, dateDiff);
        }
        IsOutOfBound = dateTimeResult == nullptr;

        if (!m_isOutOfBound)
        {
            DateResult = dateTimeResult->Value;
        }
    }
}

void DateCalculatorViewModel::UpdateDisplayResult()
{
    if (m_IsDateDiffMode)
    {
        if (m_dateDiffResultInDays == DateDifferenceUnknown)
        {
            IsDiffInDays = false;
            StrDateDiffResultInDays = L"";
            StrDateDiffResult = AppResourceProvider::GetInstance()->GetResourceString(L"CalculationFailed");
        }
        else if (m_dateDiffResultInDays.day == 0)
        {
            // to and from dates the same
            IsDiffInDays = true;
            StrDateDiffResultInDays = L"";
            StrDateDiffResult = AppResourceProvider::GetInstance()->GetResourceString(L"Date_SameDates");
        }
        else if (m_dateDiffResult == DateDifferenceUnknown || (m_dateDiffResult.year == 0 && m_dateDiffResult.month == 0 && m_dateDiffResult.week == 0))
        {
            IsDiffInDays = true;
            StrDateDiffResultInDays = L"";

            // Display result in number of days
            StrDateDiffResult = GetDateDiffStringInDays();
        }
        else
        {
            IsDiffInDays = false;

            // Display result in days, weeks, months and years
            StrDateDiffResult = GetDateDiffString();

            // Display result in number of days
            StrDateDiffResultInDays = GetDateDiffStringInDays();
        }
    }
    else
    {
        if (m_isOutOfBound)
        {
            // Display Date out of bound message
            StrDateResult = AppResourceProvider::GetInstance()->GetResourceString(L"Date_OutOfBoundMessage");
        }
        else
        {
            // Display the resulting date in long format
            StrDateResult = m_dateTimeFormatter->Format(DateResult);
        }
    }
}

void DateCalculatorViewModel::UpdateStrDateDiffResultAutomationName()
{
    String ^ automationFormat = AppResourceProvider::GetInstance()->GetResourceString(L"Date_DifferenceResultAutomationName");
    StrDateDiffResultAutomationName = LocalizationStringUtil::GetLocalizedString(automationFormat, StrDateDiffResult);
}

void DateCalculatorViewModel::UpdateStrDateResultAutomationName()
{
    String ^ automationFormat = AppResourceProvider::GetInstance()->GetResourceString(L"Date_ResultingDateAutomationName");
    StrDateResultAutomationName = LocalizationStringUtil::GetLocalizedString(automationFormat, StrDateResult);
}

void DateCalculatorViewModel::InitializeDateOutputFormats(_In_ String ^ calendarIdentifier)
{
    // Format for Add/Subtract days
    m_dateTimeFormatter = LocalizationService::GetInstance()->GetRegionalSettingsAwareDateTimeFormatter(
        L"longdate",
        calendarIdentifier,
        ClockIdentifiers::TwentyFourHour); // Clock Identifier is not used

    // Format for Date Difference
    m_allDateUnitsOutputFormat = DateUnit::Year | DateUnit::Month | DateUnit::Week | DateUnit::Day;
    m_daysOutputFormat = DateUnit::Day;
}

String ^ DateCalculatorViewModel::GetDateDiffString() const
{
    wstring result;
    bool addDelimiter = false;
    AppResourceProvider ^ resourceLoader = AppResourceProvider::GetInstance();

    auto yearCount = m_dateDiffResult.year;
    if (yearCount > 0)
    {
        result += GetLocalizedNumberString(yearCount)->Data();
        result += L' ';

        if (yearCount > 1)
        {
            result += resourceLoader->GetResourceString(L"Date_Years")->Data();
        }
        else
        {
            result += resourceLoader->GetResourceString(L"Date_Year")->Data();
        }

        // set the flags to add a delimiter whenever the next unit is added
        addDelimiter = true;
    }

    auto monthCount = m_dateDiffResult.month;
    if (monthCount > 0)
    {
        if (addDelimiter)
        {
            result += m_listSeparator;
        }
        else
        {
            addDelimiter = true;
        }

        result += GetLocalizedNumberString(monthCount)->Data();
        result += L' ';

        if (monthCount > 1)
        {
            result += resourceLoader->GetResourceString(L"Date_Months")->Data();
        }
        else
        {
            result += resourceLoader->GetResourceString(L"Date_Month")->Data();
        }
    }

    auto weekCount = m_dateDiffResult.week;
    if (weekCount > 0)
    {
        if (addDelimiter)
        {
            result += m_listSeparator;
        }
        else
        {
            addDelimiter = true;
        }

        result += GetLocalizedNumberString(weekCount)->Data();
        result += L' ';

        if (weekCount > 1)
        {
            result += resourceLoader->GetResourceString(L"Date_Weeks")->Data();
        }
        else
        {
            result += resourceLoader->GetResourceString(L"Date_Week")->Data();
        }
    }

    auto dayCount = m_dateDiffResult.day;
    if (dayCount > 0)
    {
        if (addDelimiter)
        {
            result += m_listSeparator;
        }
        else
        {
            addDelimiter = true;
        }

        result += GetLocalizedNumberString(dayCount)->Data();
        result += L' ';

        if (dayCount > 1)
        {
            result += resourceLoader->GetResourceString(L"Date_Days")->Data();
        }
        else
        {
            result += resourceLoader->GetResourceString(L"Date_Day")->Data();
        }
    }

    return ref new String(result.data());
}

String ^ DateCalculatorViewModel::GetDateDiffStringInDays() const
{
    wstring result = GetLocalizedNumberString(m_dateDiffResultInDays.day)->Data();
    result += L' ';

    // Display the result as '1 day' or 'N days'
    if (m_dateDiffResultInDays.day > 1)
    {
        result += AppResourceProvider::GetInstance()->GetResourceString(L"Date_Days")->Data();
    }
    else
    {
        result += AppResourceProvider::GetInstance()->GetResourceString(L"Date_Day")->Data();
    }

    return ref new String(result.data());
}

void DateCalculatorViewModel::OnCopyCommand(Platform::Object ^ parameter)
{
    if (m_IsDateDiffMode)
    {
        CopyPasteManager::CopyToClipboard(m_StrDateDiffResult);
    }
    else
    {
        CopyPasteManager::CopyToClipboard(m_StrDateResult);
    }
}

String ^ DateCalculatorViewModel::GetLocalizedNumberString(int value) const
{
    wstring numberStr(to_wstring(value));
    LocalizationSettings::GetInstance()->LocalizeDisplayValue(&numberStr);
    return ref new String(numberStr.c_str());
}

/// <summary>
/// Adjusts the given DateTime to 12AM of the same day
/// </summary>
/// <param name="dateTime">DateTime to clip</param>
/// <param name="adjustUsingLocalTime">Adjust the datetime using local time (by default adjust using UTC time)</param>
DateTime DateCalculatorViewModel::ClipTime(DateTime dateTime, bool adjustUsingLocalTime)
{
    DateTime referenceDateTime;
    if (adjustUsingLocalTime)
    {
        FILETIME fileTime;
        fileTime.dwLowDateTime = (DWORD)(dateTime.UniversalTime & 0xffffffff);
        fileTime.dwHighDateTime = (DWORD)(dateTime.UniversalTime >> 32);

        FILETIME localFileTime;
        FileTimeToLocalFileTime(&fileTime, &localFileTime);

        referenceDateTime.UniversalTime = (DWORD)localFileTime.dwHighDateTime;
        referenceDateTime.UniversalTime <<= 32;
        referenceDateTime.UniversalTime |= (DWORD)localFileTime.dwLowDateTime;
    }
    else
    {
        referenceDateTime = dateTime;
    }
    auto calendar = ref new Calendar();
    calendar->ChangeTimeZone("UTC");
    calendar->SetDateTime(referenceDateTime);
    calendar->Period = calendar->FirstPeriodInThisDay;
    calendar->Hour = calendar->FirstHourInThisPeriod;
    calendar->Minute = 0;
    calendar->Second = 0;
    calendar->Nanosecond = 0;

    return calendar->GetDateTime();
}
