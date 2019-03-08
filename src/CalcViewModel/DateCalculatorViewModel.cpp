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
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::DateCalculation;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::System::UserProfile;

namespace CalculatorApp::ViewModel::DateCalculatorViewModelProperties
{
    StringReference StrDateDiffResult(L"StrDateDiffResult");
    StringReference StrDateDiffResultAutomationName(L"StrDateDiffResultAutomationName");
    StringReference StrDateDiffResultInDays(L"StrDateDiffResultInDays");
    StringReference StrDateResult(L"StrDateResult");
    StringReference StrDateResultAutomationName(L"StrDateResultAutomationName");
    StringReference IsDiffInDays(L"IsDiffInDays");
}

DateCalculatorViewModel::DateCalculatorViewModel() :
    m_IsDateDiffMode(true),
    m_IsAddMode(true),
    m_isOutOfBound(false),
    m_DaysOffset(0),
    m_MonthsOffset(0),
    m_YearsOffset(0),
    m_StrDateDiffResult(L""),
    m_StrDateDiffResultAutomationName(L""),
    m_StrDateDiffResultInDays(L""),
    m_StrDateResult(L""),
    m_StrDateResultAutomationName(L""),
    m_fromDate({ 0 }),
    m_toDate({ 0 }),
    m_startDate({ 0 }),
    m_dateResult({ 0 })
{
    const auto& localizationSettings = LocalizationSettings::GetInstance();

    // Initialize Date Output format instances
    InitializeDateOutputFormats(localizationSettings.GetCalendarIdentifier());

    // Initialize Date Calc engine
    m_dateCalcEngine = make_shared<DateCalculationEngine>(localizationSettings.GetCalendarIdentifier());

    // Initialize dates of DatePicker controls to today's date
    auto calendar = ref new Calendar();
    auto today = calendar->GetDateTime();

    // FromDate and ToDate should be clipped (adjusted to a consistent hour in UTC)
    m_fromDate = today;
    m_toDate = today;
    FromDate = ClipTime(today);
    ToDate = ClipTime(today);

    // StartDate should not be clipped
    StartDate = today;
    m_dateResult = today;

    // Initialize the list separator delimiter appended with a space at the end, e.g. ", "
    // This will be used for date difference formatting: Y years, M months, W weeks, D days
    m_listSeparator = ref new String((localizationSettings.GetListSeparator() + L" ").c_str());

    // Initialize the output results
    UpdateDisplayResult();

    m_offsetValues = ref new Vector<String^>();
    for (int i = 0; i <= c_maxOffsetValue; i++)
    {
        wstring numberStr(to_wstring(i));
        localizationSettings.LocalizeDisplayValue(&numberStr);
        m_offsetValues->Append(ref new String(numberStr.c_str()));
    }

    /* In the ClipTime function, we used to change timezone to UTC before clipping the time.
       The comment from the previous developers said this was done to eliminate the effects of
       Daylight Savings Time. We can't think of a good reason why this change in timezone is
       necessary and did find bugs related to the change, therefore, we have removed the
       change. Just in case, we will see if the clipped time is ever a different day from the
       original day, which would hopefully indicate the change in timezone was actually
       necessary. We will collect telemetry if we find this case. If we don't see any
       telemetry events after the application has been used for some time, we will feel safe
       and can remove this function. */
    DayOfWeek trueDayOfWeek = calendar->DayOfWeek;

    DateTime clippedTime = ClipTime(today);
    calendar->SetDateTime(clippedTime);
    if (calendar->DayOfWeek != trueDayOfWeek)
    {
        calendar->SetDateTime(today);
        TraceLogger::GetInstance().LogDateClippedTimeDifferenceFound(
            from_cx<winrt::Windows::Globalization::Calendar>(calendar),
            winrt::Windows::Foundation::DateTime{ winrt::Windows::Foundation::TimeSpan{ clippedTime.UniversalTime } });
    }
}

void DateCalculatorViewModel::OnPropertyChanged(_In_ String^ prop)
{
    if (prop == DateCalculatorViewModelProperties::StrDateDiffResult)
    {
        UpdateStrDateDiffResultAutomationName();
    }
    else if (prop == DateCalculatorViewModelProperties::StrDateResult)
    {
        UpdateStrDateResultAutomationName();
    }
    else if (prop != DateCalculatorViewModelProperties::StrDateDiffResultAutomationName
        && prop != DateCalculatorViewModelProperties::StrDateDiffResultInDays
        && prop != DateCalculatorViewModelProperties::StrDateResultAutomationName
        && prop != DateCalculatorViewModelProperties::IsDiffInDays)
    {
        OnInputsChanged();
    }
}

void DateCalculatorViewModel::OnInputsChanged()
{
    DateDifference dateDiff;

    if (m_IsDateDiffMode)
    {
        DateTime clippedFromDate = ClipTime(FromDate);
        DateTime clippedToDate = ClipTime(ToDate);

        // Calculate difference between two dates
        m_dateCalcEngine->GetDateDifference(clippedFromDate, clippedToDate, m_allDateUnitsOutputFormat, &dateDiff);
        DateDiffResult = dateDiff;

        m_dateCalcEngine->GetDateDifference(clippedFromDate, clippedToDate, m_daysOutputFormat, &dateDiff);
        DateDiffResultInDays = dateDiff;
    }
    else
    {
        dateDiff.day = DaysOffset;
        dateDiff.month = MonthsOffset;
        dateDiff.year = YearsOffset;

        DateTime dateTimeResult;

        if (m_IsAddMode)
        {
            // Add number of Days, Months and Years to a Date
            IsOutOfBound = !m_dateCalcEngine->AddDuration(StartDate, dateDiff, &dateTimeResult);
        }
        else
        {
            // Subtract number of Days, Months and Years from a Date
            IsOutOfBound = !m_dateCalcEngine->SubtractDuration(StartDate, dateDiff, &dateTimeResult);
        }

        if (!m_isOutOfBound)
        {
            DateResult = dateTimeResult;
        }
    }
}

void DateCalculatorViewModel::UpdateDisplayResult()
{
    if (m_IsDateDiffMode)
    {
        // Are to and from dates the same
        if (m_dateDiffResultInDays.day == 0)
        {
            IsDiffInDays = true;
            StrDateDiffResultInDays = L"";
            StrDateDiffResult = AppResourceProvider::GetInstance().GetResourceString(L"Date_SameDates");
        }
        else if ((m_dateDiffResult.year == 0) && 
                (m_dateDiffResult.month == 0) && 
                (m_dateDiffResult.week == 0))
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
            StrDateResult = AppResourceProvider::GetInstance().GetResourceString(L"Date_OutOfBoundMessage");
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
    String^ automationFormat = AppResourceProvider::GetInstance().GetResourceString(L"Date_DifferenceResultAutomationName");
    wstring localizedAutomationName = LocalizationStringUtil::GetLocalizedString(automationFormat->Data(), StrDateDiffResult->Data());
    StrDateDiffResultAutomationName = ref new String(localizedAutomationName.c_str());
}

void DateCalculatorViewModel::UpdateStrDateResultAutomationName()
{
    String^ automationFormat = AppResourceProvider::GetInstance().GetResourceString(L"Date_ResultingDateAutomationName");
    wstring localizedAutomationName = LocalizationStringUtil::GetLocalizedString(automationFormat->Data(), StrDateResult->Data());
    StrDateResultAutomationName = ref new String(localizedAutomationName.c_str());
}

void DateCalculatorViewModel::InitializeDateOutputFormats(_In_ String^ calendarIdentifier)
{
    // Format for Add/Subtract days
    m_dateTimeFormatter = LocalizationService::GetRegionalSettingsAwareDateTimeFormatter(
        L"longdate",
        calendarIdentifier,
        ClockIdentifiers::TwentyFourHour); // Clock Identifier is not used

    // Format for Date Difference
    m_allDateUnitsOutputFormat = DateUnit::Year | DateUnit::Month | DateUnit::Week | DateUnit::Day;
    m_daysOutputFormat = DateUnit::Day;
}

String^ DateCalculatorViewModel::GetDateDiffString() const
{
    String^ result = L"";
    bool addDelimiter = false;
    AppResourceProvider resourceLoader = AppResourceProvider::GetInstance();

    auto yearCount = m_dateDiffResult.year;
    if (yearCount > 0)
    {
        result = String::Concat(GetLocalizedNumberString(yearCount), L" ");

        if (yearCount > 1)
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Years"));
        }
        else
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Year"));
        }

        // set the flags to add a delimiter whenever the next unit is added
        addDelimiter = true;
    }

    auto monthCount = m_dateDiffResult.month;
    if (monthCount > 0)
    {
        if (addDelimiter)
        {
            result = String::Concat(result, m_listSeparator);
        }
        else
        {
            addDelimiter = true;
        }

        result = String::Concat(result, String::Concat(GetLocalizedNumberString(monthCount), L" "));

        if (monthCount > 1)
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Months"));
        }
        else
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Month"));
        }
    }

    auto weekCount = m_dateDiffResult.week;
    if (weekCount > 0)
    {
        if (addDelimiter)
        {
            result = String::Concat(result, m_listSeparator);
        }
        else
        {
            addDelimiter = true;
        }

        result = String::Concat(result, String::Concat(GetLocalizedNumberString(weekCount), L" "));

        if (weekCount > 1)
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Weeks"));
        }
        else
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Week"));
        }
    }

    auto dayCount = m_dateDiffResult.day;
    if (dayCount > 0)
    {
        if (addDelimiter)
        {
            result = String::Concat(result, m_listSeparator);
        }
        else
        {
            addDelimiter = true;
        }

        result = String::Concat(result, String::Concat(GetLocalizedNumberString(dayCount), L" "));

        if (dayCount > 1)
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Days"));
        }
        else
        {
            result = String::Concat(result, resourceLoader.GetResourceString(L"Date_Day"));
        }
    }

    return result;
}

String^ DateCalculatorViewModel::GetDateDiffStringInDays() const
{
    String^ strDateUnit;

    // Display the result as '1 day' or 'N days'
    if (m_dateDiffResultInDays.day > 1)
    {
        strDateUnit = AppResourceProvider::GetInstance().GetResourceString(L"Date_Days");
    }
    else
    {
        strDateUnit = AppResourceProvider::GetInstance().GetResourceString(L"Date_Day");
    }

    return String::Concat(GetLocalizedNumberString(m_dateDiffResultInDays.day), String::Concat(L" ", strDateUnit));
}

void DateCalculatorViewModel::OnCopyCommand(Platform::Object^ parameter)
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

String^ DateCalculatorViewModel::GetLocalizedNumberString(int value) const
{
    wstring numberStr(to_wstring(value));
    LocalizationSettings::GetInstance().LocalizeDisplayValue(&numberStr);
    return ref new String(numberStr.c_str());
}

// Adjusts the given DateTime to 12AM of the same day
DateTime DateCalculatorViewModel::ClipTime(DateTime dateTime)
{
    auto calendar = ref new Calendar();
    calendar->SetDateTime(dateTime);
    calendar->Period = 1;
    calendar->Hour = 12;
    calendar->Minute = 0;
    calendar->Second = 0;
    calendar->Nanosecond = 0;

    return calendar->GetDateTime();
}
