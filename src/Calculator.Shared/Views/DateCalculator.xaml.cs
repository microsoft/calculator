// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using System.Globalization;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
	public sealed partial class DateCalculator : UserControl
	{
		// We choose 2550 as the max year because CalendarDatePicker experiences clipping
		// issues just after 2558.  We would like 9999 but will need to wait for a platform
		// fix before we use a higher max year.  This platform issue is tracked by
		// TODO: MSFT-9273247
		const int c_maxYear = 2550;
		const int c_minYear = 1601;

		public DateCalculator()
		{
			this.InitializeComponent();

			var localizationSettings = LocalizationSettings.GetInstance();

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
		    var calendar = new Windows.Globalization.Calendar();
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
		    DateTimeFormatter dateTimeFormatter = LocalizationService.GetRegionalSettingsAwareDateTimeFormatter(
		        "day month year",
		        localizationSettings.GetCalendarIdentifier(),
		        ClockIdentifiers.TwentyFourHour); // Clock Identifier is not used

		    DateDiff_FromDate.DateFormat = "day month year";
		    DateDiff_ToDate.DateFormat = "day month year";

			//TODO UNO: var placeholderText = dateTimeFormatter.Format(today);
			var placeholderText = today.ToString("day month year", CultureInfo.CurrentCulture);

			DateDiff_FromDate.PlaceholderText = placeholderText;
		    DateDiff_ToDate.PlaceholderText = placeholderText;

		    // TODO UNO
		    // CopyMenuItem.Text = AppResourceProvider.GetInstance().GetResourceString("copyMenuItem");
		    DateCalculationOption.SelectionChanged += new SelectionChangedEventHandler(DateCalcOption_Changed);
		}

		void FromDate_DateChanged( CalendarDatePicker sender,  CalendarDatePickerDateChangedEventArgs e)
		{
		    if (e.NewDate != null)
		    {
		        var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
		        dateCalcViewModel.FromDate = e.NewDate.Value.DateTime;
		        TraceLogger.GetInstance().LogDateDifferenceModeUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
		    }
		    else
		    {
		        ReselectCalendarDate(sender, e.OldDate.Value.DateTime);
		    }
		}

		void ToDate_DateChanged( CalendarDatePicker sender,  CalendarDatePickerDateChangedEventArgs e)
		{
		    if (e.NewDate != null)
		    {
		        var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
		        dateCalcViewModel.ToDate = e.NewDate.Value.DateTime;
		        TraceLogger.GetInstance().LogDateDifferenceModeUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
		    }
		    else
		    {
		        ReselectCalendarDate(sender, e.OldDate.Value.DateTime);
		    }
		}

		void AddSubtract_DateChanged( CalendarDatePicker sender,  CalendarDatePickerDateChangedEventArgs e)
		{
		    if (e.NewDate != null)
		    {
		        var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
		        dateCalcViewModel.StartDate = e.NewDate.Value.DateTime;
		        TraceLogger.GetInstance().LogDateAddSubtractModeUsed(
		            ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()), dateCalcViewModel.IsAddMode);
		    }
		    else
		    {
		        ReselectCalendarDate(sender, e.OldDate.Value.DateTime);
		    }
		}

		void OffsetValue_Changed( object sender,  SelectionChangedEventArgs e)
		{
		    var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
		    TraceLogger.GetInstance().LogDateAddSubtractModeUsed(
		        ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()), dateCalcViewModel.IsAddMode);
		}

		void OnCopyMenuItemClicked( object sender,  RoutedEventArgs e)
		{
			// TODO UNO
		    //var calcResult = (TextBlock)ResultsContextMenu.Target;

		    //CopyPasteManager.CopyToClipboard(calcResult.Text);
		}

		void OnLoaded( object sender,  Windows.UI.Xaml.RoutedEventArgs e)
		{
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

		void DateCalcOption_Changed( object sender,  Windows.UI.Xaml.Controls.SelectionChangedEventArgs e)
		{
		    FindName("AddSubtractDateGrid");
			DateCalculationOption.SelectionChanged -= DateCalcOption_Changed;
		}

		void AddSubtractDateGrid_Loaded( object sender,  Windows.UI.Xaml.RoutedEventArgs e)
		{
		    var localizationSettings = LocalizationSettings.GetInstance();

		    AddSubtract_FromDate.PlaceholderText = DateDiff_FromDate.PlaceholderText;
		    AddSubtract_FromDate.CalendarIdentifier = localizationSettings.GetCalendarIdentifier();
		    AddSubtract_FromDate.FirstDayOfWeek = localizationSettings.GetFirstDayOfWeek();
		    AddSubtract_FromDate.Language = localizationSettings.GetLocaleName();

		    AddSubtract_FromDate.MinDate = DateDiff_FromDate.MinDate;
		    AddSubtract_FromDate.MaxDate = DateDiff_FromDate.MaxDate;
		    AddSubtract_FromDate.DateFormat = "day month year";
		}

		void ReselectCalendarDate( Windows.UI.Xaml.Controls.CalendarDatePicker calendarDatePicker, DateTime dateTime)
		{
		    // Reselect the unselected Date
		    calendarDatePicker.Date = dateTime;

		    // Dismiss the Calendar flyout
		    calendarDatePicker.IsCalendarOpen = false;
		}

		void OffsetDropDownClosed( object sender,  object e)
		{
		    RaiseLiveRegionChangedAutomationEvent(/* DateDiff mode */ false);
		}

		void CalendarFlyoutClosed( object sender,  object e)
		{
		    var dateCalcViewModel = (DateCalculatorViewModel)this.DataContext;
		    RaiseLiveRegionChangedAutomationEvent(dateCalcViewModel.IsDateDiffMode);
		}

		void RaiseLiveRegionChangedAutomationEvent( bool isDateDiffMode)
		{
		    TextBlock resultTextBlock = (isDateDiffMode ? DateDiffAllUnitsResultLabel : DateResultLabel);
		    String automationName = AutomationProperties.GetName(resultTextBlock);
		    TextBlockAutomationPeer.FromElement(resultTextBlock).RaiseAutomationEvent(AutomationEvents.LiveRegionChanged);
		}

		void AddSubtractOption_Checked( object sender,  RoutedEventArgs e)
		{
		    RaiseLiveRegionChangedAutomationEvent(/* DateDiff mode */ false);
		}
	}
}
