// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// DateCalculator.xaml.h
// Declaration of the DateCalculator class
//

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using System;

using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class DateCalculator
    {
        public DateCalculator()
        {
            InitializeComponent();

            // Set Calendar Identifier
            DateDiff_FromDate.CalendarIdentifier = localizationSettings.GetCalendarIdentifier();
            DateDiff_ToDate.CalendarIdentifier = localizationSettings.GetCalendarIdentifier();

            // Setting the FirstDayofWeek
            DateDiff_FromDate.FirstDayOfWeek = localizationSettings.GetFirstDayOfWeek();
            DateDiff_ToDate.FirstDayOfWeek = localizationSettings.GetFirstDayOfWeek();

            // Setting the Language explicitly is not required,
            // this is a workaround for the bug in the control due to which
            // the displayed date is incorrect for non Gregorian Calendar Systems
            // The displayed date doesn't honor the shortdate format, on setting the Language the format is refreshed
            DateDiff_FromDate.Language = localizationSettings.GetLocaleName();
            DateDiff_ToDate.Language = localizationSettings.GetLocaleName();

            // Set Min and Max Dates according to the Gregorian Calendar(1601 & 9999)
            var calendar = new Calendar();
            var today = calendar.GetDateTime();

            calendar.ChangeCalendarSystem(CalendarIdentifiers.Gregorian);
            calendar.Day = 1;
            calendar.Month = 1;
            calendar.Year = c_minYear;
            var minYear = calendar.GetDateTime(); // 1st January, 1601
            DateDiff_FromDate.MinDate = minYear;
            DateDiff_ToDate.MinDate = minYear;

            calendar.Day = 31;
            calendar.Month = 12;
            calendar.Year = c_maxYear;
            var maxYear = calendar.GetDateTime(); // 31st December, 9878
            DateDiff_FromDate.MaxDate = maxYear;
            DateDiff_ToDate.MaxDate = maxYear;

            // Set the PlaceHolderText for CalendarDatePicker
            DateTimeFormatter dateTimeFormatter = LocalizationService.GetInstance().GetRegionalSettingsAwareDateTimeFormatter(
                "day month year",
                localizationSettings.GetCalendarIdentifier(),
                ClockIdentifiers.TwentyFourHour); // Clock Identifier is not used

            DateDiff_FromDate.DateFormat = "day month year";
            DateDiff_ToDate.DateFormat = "day month year";

            var placeholderText = dateTimeFormatter.Format(today);

            DateDiff_FromDate.PlaceholderText = placeholderText;
            DateDiff_ToDate.PlaceholderText = placeholderText;

            CopyMenuItem.Text = AppResourceProvider.GetInstance().GetResourceString("copyMenuItem");
            DateCalculationOption.SelectionChanged += DateCalcOption_Changed;
        }

        public void CloseCalendarFlyout()
        {
            if (DateDiff_FromDate.IsCalendarOpen)
            {
                DateDiff_FromDate.IsCalendarOpen = false;
            }

            if (DateDiff_ToDate.IsCalendarOpen)
            {
                DateDiff_ToDate.IsCalendarOpen = false;
            }

            if ((AddSubtract_FromDate != null) && (AddSubtract_FromDate.IsCalendarOpen))
            {
                AddSubtract_FromDate.IsCalendarOpen = false;
            }
        }

        public void SetDefaultFocus()
        {
            DateCalculationOption.Focus(FocusState.Programmatic);
        }

        private void FromDate_DateChanged(CalendarDatePicker sender, CalendarDatePickerDateChangedEventArgs e)
        {
            if (e.NewDate != null)
            {
                var dateCalcViewModel = (DateCalculatorViewModel)DataContext;
                dateCalcViewModel.FromDate = e.NewDate.Value;
                TraceLogger.GetInstance().LogDateCalculationModeUsed(false);
            }
            else
            {
                ReselectCalendarDate(sender, e.OldDate.Value);
            }
        }

        private void ToDate_DateChanged(CalendarDatePicker sender, CalendarDatePickerDateChangedEventArgs e)
        {
            if (e.NewDate != null)
            {
                var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
                dateCalcViewModel.ToDate = e.NewDate.Value;
                TraceLogger.GetInstance().LogDateCalculationModeUsed(false);
            }
            else
            {
                ReselectCalendarDate(sender, e.OldDate.Value);
            }
        }

        private void AddSubtract_DateChanged(CalendarDatePicker sender, CalendarDatePickerDateChangedEventArgs e)
        {
            if (e.NewDate != null)
            {
                var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
                dateCalcViewModel.StartDate = e.NewDate.Value;
                TraceLogger.GetInstance().LogDateCalculationModeUsed(true);
            }
            else
            {
                ReselectCalendarDate(sender, e.OldDate.Value);
            }
        }

        private void OffsetValue_Changed(object sender, SelectionChangedEventArgs e)
        {
            var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
            // do not log diagnostics for no-ops and initialization of combo boxes
            if (dateCalcViewModel.DaysOffset != 0 || dateCalcViewModel.MonthsOffset != 0 || dateCalcViewModel.YearsOffset != 0)
            {
                TraceLogger.GetInstance().LogDateCalculationModeUsed(true);
            }
        }

        private void OnCopyMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var calcResult = (TextBlock)ResultsContextMenu.Target;

            CopyPasteManager.CopyToClipboard(calcResult.Text);
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
        }

        private void DateCalcOption_Changed(object sender, SelectionChangedEventArgs e)
        {
            FindName("AddSubtractDateGrid");
            var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;

            // From Date Field needs to persist across Date Difference and Add Substract Date Mode.
            // So when the mode dropdown changes, update the other datepicker with the latest date.
            if (dateCalcViewModel.IsDateDiffMode)
            {
                if (AddSubtract_FromDate.Date == null)
                {
                    return;
                }
                DateDiff_FromDate.Date = AddSubtract_FromDate.Date.Value;
            }
            else
            {
                if (DateDiff_FromDate.Date == null)
                {
                    // If no date has been picked, then this can be null.
                    return;
                }
                AddSubtract_FromDate.Date = DateDiff_FromDate.Date.Value;
            }
        }

        private void AddSubtractDateGrid_Loaded(object sender, RoutedEventArgs e)
        {
            AddSubtract_FromDate.PlaceholderText = DateDiff_FromDate.PlaceholderText;
            AddSubtract_FromDate.CalendarIdentifier = localizationSettings.GetCalendarIdentifier();
            AddSubtract_FromDate.FirstDayOfWeek = localizationSettings.GetFirstDayOfWeek();
            AddSubtract_FromDate.Language = localizationSettings.GetLocaleName();

            AddSubtract_FromDate.MinDate = DateDiff_FromDate.MinDate;
            AddSubtract_FromDate.MaxDate = DateDiff_FromDate.MaxDate;
            AddSubtract_FromDate.DateFormat = "day month year";
        }

        private void AddSubtractOption_Checked(object sender, RoutedEventArgs e)
        {
            RaiseLiveRegionChangedAutomationEvent(false);
        }

        private void ReselectCalendarDate(CalendarDatePicker calendarDatePicker, DateTimeOffset? dateTime)
        {
            // Reselect the unselected Date
            calendarDatePicker.Date = dateTime;

            // Dismiss the Calendar flyout
            calendarDatePicker.IsCalendarOpen = false;
        }

        private void OffsetDropDownClosed(object sender, object e)
        {
            RaiseLiveRegionChangedAutomationEvent(false);
        }

        private void CalendarFlyoutClosed(object sender, object e)
        {
            var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
            RaiseLiveRegionChangedAutomationEvent(dateCalcViewModel.IsDateDiffMode);
        }

        private void RaiseLiveRegionChangedAutomationEvent(bool isDateDiffMode)
        {
            TextBlock resultTextBlock = isDateDiffMode ? DateDiffAllUnitsResultLabel : DateResultLabel;
            string automationName = AutomationProperties.GetName(resultTextBlock);
            TextBlockAutomationPeer.FromElement(resultTextBlock).RaiseAutomationEvent(AutomationEvents.LiveRegionChanged);
        }

        private void OnVisualStateChanged(object sender, VisualStateChangedEventArgs e)
        {
            TraceLogger.GetInstance().LogVisualStateChanged(ViewMode.Date, e.NewState.Name, false);
        }

        // We choose 2550 as the max year because CalendarDatePicker experiences clipping
        // issues just after 2558.  We would like 9999 but will need to wait for a platform
        // fix before we use a higher max year.  This platform issue is tracked by
        // TODO: MSFT-9273247
        private const int c_maxYear = 2550;
        private const int c_minYear = 1601;

        private static readonly LocalizationSettings localizationSettings = LocalizationSettings.GetInstance();
    }
}
