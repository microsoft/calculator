// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// DateCalculator.xaml.h
// Declaration of the DateCalculator class
//

#pragma once

#include "Converters/BooleanNegationConverter.h"
#include "Converters/VisibilityNegationConverter.h"
#include "Views/DateCalculator.g.h"

// We choose 2550 as the max year because CalendarDatePicker experiences clipping
// issues just after 2558.  We would like 9999 but will need to wait for a platform
// fix before we use a higher max year.  This platform issue is tracked by
// TODO: MSFT-9273247
const int c_maxYear = 2550;
const int c_minYear = 1601;

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DateCalculator sealed
    {
    public:
        DateCalculator();
        void CloseCalendarFlyout();
        void SetDefaultFocus();

    private:
        void FromDate_DateChanged(_In_ Windows::UI::Xaml::Controls::CalendarDatePicker^ sender, _In_ Windows::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs^ e);
        void ToDate_DateChanged(_In_ Windows::UI::Xaml::Controls::CalendarDatePicker^ sender, _In_ Windows::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs^ e);
        void AddSubtract_DateChanged(_In_ Windows::UI::Xaml::Controls::CalendarDatePicker^ sender, _In_ Windows::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs^ e);
        void OffsetValue_Changed(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void OnCopyMenuItemClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnLoaded(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void DateCalcOption_Changed(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void AddSubtractDateGrid_Loaded(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void AddSubtractOption_Checked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void ReselectCalendarDate(_In_ Windows::UI::Xaml::Controls::CalendarDatePicker^ calendarDatePicker, Windows::Foundation::DateTime dateTime);
        void OffsetDropDownClosed(_In_ Platform::Object^ sender, _In_ Platform::Object^ e);
        void CalendarFlyoutClosed(_In_ Platform::Object^ sender, _In_ Platform::Object^ e);
        void RaiseLiveRegionChangedAutomationEvent(_In_ bool isDateDiffMode);

        Windows::Foundation::EventRegistrationToken m_dateCalcOptionChangedEventToken;
    };
}
