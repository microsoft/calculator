// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using Windows.UI.Xaml.Data;
using Windows.Globalization;
using CalculatorApp.Common;
using Windows.Globalization.DateTimeFormatting;
using CalculatorApp.Common.DateCalculation;
using Windows.Foundation;
using System.Numerics;
using CalculatorApp;
using System.Globalization;
using Windows.System;

namespace CalculatorApp.ViewModel
{
	[Windows.UI.Xaml.Data.Bindable]
	public sealed class DateCalculatorViewModel : INotifyPropertyChanged
	{
		const int c_maxOffsetValue = 999;

		const string StrDateDiffResultPropertyName="StrDateDiffResult";
		const string StrDateDiffResultAutomationNamePropertyName="StrDateDiffResultAutomationName";
		const string StrDateDiffResultInDaysPropertyName="StrDateDiffResultInDays";
		const string StrDateResultPropertyName="StrDateResult";
		const string StrDateResultAutomationNamePropertyName="StrDateResultAutomationName";
		const string IsDiffInDaysPropertyName="IsDiffInDays";

		public event PropertyChangedEventHandler PropertyChanged;
		private void RaisePropertyChanged([System.Runtime.CompilerServices.CallerMemberName]string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));

		// Private members
		private DateCalculationEngine m_dateCalcEngine;
		private DateUnit m_daysOutputFormat;
		private DateUnit m_allDateUnitsOutputFormat;
		private DateTimeFormatter m_dateTimeFormatter;
		private string m_listSeparator;

		private CalculatorList<string> m_offsetValues;
		private DateTime m_fromDate;
		private DateTime m_toDate;
		private DateTime m_startDate;
		private DateTime m_dateResult;
		private DateDifference m_dateDiffResult;
		private DateDifference m_dateDiffResultInDays;

		private bool m_IsDateDiffMode = true;
		private bool m_IsAddMode = true;
		private bool m_IsDiffInDays = true;
		private bool m_isOutOfBound = false;
		private int m_DaysOffset = 0;
		private int m_MonthsOffset = 0;
		private int m_YearsOffset = 0;
		private string m_StrDateDiffResult = "";
		private string m_StrDateDiffResultAutomationName = "";
		private string m_StrDateDiffResultInDays = "";
		private string m_StrDateResult = "";
		private string m_StrDateResultAutomationName = "";

		public DateCalculatorViewModel()
		{
			var localizationSettings = LocalizationSettings.GetInstance();

			// Initialize Date Output format instances
			InitializeDateOutputFormats(localizationSettings.GetCalendarIdentifier());

			// Initialize Date Calc engine
			m_dateCalcEngine = new DateCalculationEngine(localizationSettings.GetCalendarIdentifier());

			// Initialize dates of DatePicker controls to today's date
			var calendar = new Windows.Globalization.Calendar();
			var today = calendar.GetDateTime().DateTime;

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
			m_listSeparator = localizationSettings.GetListSeparator() + " ";

			// Initialize the output results
			UpdateDisplayResult();

			m_offsetValues = new CalculatorList<string>();

			for (int i = 0; i <= c_maxOffsetValue; i++)
			{
				// MAX?
				string numberStr = i.ToString();
				localizationSettings.LocalizeDisplayValue(ref numberStr);
				m_offsetValues.Append(numberStr.ToString());
			}

			/* In the ClipTime function, we used to change timezone to UTC before clipping the time.
				The comment from the previous delopers said this was done to eliminate the effects of
				Daylight Savings Time. We can't think of a good reason why this change in timezone is
				necessary and did find bugs related to the change, therefore, we have removed the
				change. Just in case, we will see if the clipped time is ever a different day from the
				original day, which would hopefully indicate the change in timezone was actually
				necessary. We will collect telemetry if we find this case. If we don't see any
				telemetry events after the application has been used for some time, we will feel safe
				and can remove this function. */
			Windows.Globalization.DayOfWeek trueDayOfWeek = calendar.DayOfWeek;

			DateTime clippedTime = ClipTime(today);
			calendar.SetDateTime(clippedTime);

			if (calendar.DayOfWeek != trueDayOfWeek)
			{
				// MAX?
				calendar.SetDateTime(today);
				TraceLogger.GetInstance().LogDateClippedTimeDifferenceFound(
					calendar,
					clippedTime.ToUniversalTime());
			}

			PropertyChanged += (snd, e) => ((DateCalculatorViewModel)snd).OnPropertyChanged(e.PropertyName);
		}

		static void CheckClipTimeSameDay(Windows.Globalization.Calendar reference)
		{

		}

		private bool IsOutOfBound
		{
			get { return m_isOutOfBound; }
			set
			{
				m_isOutOfBound = value;
				UpdateDisplayResult();
			}
		}

		private DateDifference DateDiffResult
		{
			get { return m_dateDiffResult; }
			set {
				m_dateDiffResult = value;
				UpdateDisplayResult();
			}
		}

		private DateDifference DateDiffResultInDays
		{
			get { return m_dateDiffResultInDays; }
			set
			{
				m_dateDiffResultInDays = value;
				UpdateDisplayResult();
			}
		}

		private DateTime DateResult
		{
			get { return m_dateResult; }
			set {
				m_dateResult = value;
				UpdateDisplayResult();
			}
		}

		// PUBLIC

		// Input Properties
		public bool IsDateDiffMode
		{
			get => m_IsDateDiffMode;
			set
			{
				if (m_IsDateDiffMode != value)
				{
					m_IsDateDiffMode = value;
					RaisePropertyChanged();
				}
			}
		}
		public bool IsAddMode
		{
			get => m_IsAddMode;
			set
			{
				if (m_IsAddMode != value)
				{
					m_IsAddMode = value;
					RaisePropertyChanged();
				}
			}
		}
		public bool IsDiffInDays
		{
			get => m_IsDiffInDays;
			private set
			{
				if (m_IsDiffInDays != value)
				{
					m_IsDiffInDays = value;
					RaisePropertyChanged();
				}
			}
		} // If diff is only in days or the dates are the same,
		  // then show only one result and avoid redundancy

		public int DaysOffset
		{
			get => m_DaysOffset;
			set
			{
				if (m_DaysOffset != value)
				{
					m_DaysOffset = value;
					RaisePropertyChanged();
				}
			}
		}

		public int MonthsOffset
		{
			get => m_MonthsOffset;
			set
			{
				if (m_MonthsOffset != value)
				{
					m_MonthsOffset = value;
					RaisePropertyChanged();
				}
			}
		}

		public int YearsOffset
		{
			get => m_YearsOffset;
			set
			{
				if (m_YearsOffset != value)
				{
					m_YearsOffset = value;
					RaisePropertyChanged();
				}
			}
		}

		// Read only property for offset values
		public CalculatorList<string> OffsetValues
		{
			get { return m_offsetValues; }
		}

		// From date for Date Diff
		public DateTime FromDate
		{
			get { return m_fromDate; }
			set
			{
				if (m_fromDate.ToUniversalTime() != value.ToUniversalTime())
				{
					m_fromDate = value;
					RaisePropertyChanged("FromDate");
				}
			}
		}

		// To date for Date Diff
		public DateTime ToDate
		{
			get { return m_toDate; }
			set
			{
				if (m_toDate.ToUniversalTime() != value.ToUniversalTime())
				{
					m_toDate = value;
					RaisePropertyChanged("ToDate");
				}
			}
		}

		// Start date for Add/Subtract date
		public DateTime StartDate
		{
			get { return m_startDate; }
			set
			{
				if (m_startDate.ToUniversalTime() != value.ToUniversalTime())
				{
					m_startDate = value;
					RaisePropertyChanged("StartDate");
				}
			}
		}

		// Output Properties
		public string StrDateDiffResult
		{
			get => m_StrDateDiffResult;
			private set
			{
				if (m_StrDateDiffResult != value)
				{
					m_StrDateDiffResult = value;
					RaisePropertyChanged();
				}
			}
		}

		public string StrDateDiffResultAutomationName
		{
			get => m_StrDateDiffResultAutomationName;
			private set
			{
				if (m_StrDateDiffResultAutomationName != value)
				{
					m_StrDateDiffResultAutomationName = value;
					RaisePropertyChanged();
				}
			}
		}

		public string StrDateDiffResultInDays
		{
			get => m_StrDateDiffResultInDays;
			private set
			{
				if (m_StrDateDiffResultInDays != value)
				{
					m_StrDateDiffResultInDays = value;
					RaisePropertyChanged();
				}
			}
		}

		public string StrDateResult
		{
			get => m_StrDateResult;
			private set
			{
				if (m_StrDateResult != value)
				{
					m_StrDateResult = value;
					RaisePropertyChanged();
				}
			}
		}

		public string StrDateResultAutomationName
		{
			get => m_StrDateResultAutomationName;
			private set
			{
				if (m_StrDateResultAutomationName != value)
				{
					m_StrDateResultAutomationName = value;
					RaisePropertyChanged();
				}
			}
		}

		public void OnCopyCommand(Object parameter)
		{

		}


		// TODO UNO: KeyboardShortcutManager
		public void OnKeyPress(VirtualKey key)
		{
		}

		// PRIVATE
		private void OnPropertyChanged(string prop)
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
				prop != StrDateDiffResultAutomationNamePropertyName
				&& prop != StrDateDiffResultInDaysPropertyName
				&& prop != StrDateResultAutomationNamePropertyName
				&& prop != IsDiffInDaysPropertyName)
			{
				OnInputsChanged();
			}
		}

		private void OnInputsChanged()
		{
			DateDifference dateDiff = new DateDifference();

			if (m_IsDateDiffMode)
			{
				DateTime clippedFromDate = ClipTime(FromDate);
				DateTime clippedToDate = ClipTime(ToDate);

				// Calculate difference between two dates
				m_dateCalcEngine.GetDateDifference(clippedFromDate, clippedToDate, m_allDateUnitsOutputFormat, out dateDiff);
				DateDiffResult = dateDiff;

				m_dateCalcEngine.GetDateDifference(clippedFromDate, clippedToDate, m_daysOutputFormat, out dateDiff);
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
					IsOutOfBound = !m_dateCalcEngine.AddDuration(StartDate, dateDiff, out dateTimeResult);
				}
				else
				{
					// Subtract number of Days, Months and Years from a Date
					IsOutOfBound = !m_dateCalcEngine.SubtractDuration(StartDate, dateDiff, out dateTimeResult);
				}

				if (!m_isOutOfBound)
				{
					DateResult = dateTimeResult;
				}
			}
		}

		private void UpdateDisplayResult()
		{
			if (m_IsDateDiffMode)
			{
				// Are to and from dates the same
				if (m_dateDiffResultInDays.day == 0)
				{
					IsDiffInDays = true;
					StrDateDiffResultInDays = "";
					StrDateDiffResult = AppResourceProvider.GetInstance().GetResourceString("Date_SameDates");
				}
				else if ((m_dateDiffResult.year == 0) &&
						(m_dateDiffResult.month == 0) &&
						(m_dateDiffResult.week == 0))
				{
					IsDiffInDays = true;
					StrDateDiffResultInDays = "";

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
					StrDateResult = AppResourceProvider.GetInstance().GetResourceString("Date_OutOfBoundMessage");
				}
				else
				{
					// Display the resulting date in long format
					//TODO UNO: StrDateResult = m_dateTimeFormatter.Format(DateResult);
					StrDateResult = DateResult.ToString(CultureInfo.CurrentCulture);
				}
			}
		}

		private void UpdateStrDateDiffResultAutomationName()
		{
			String automationFormat = AppResourceProvider.GetInstance().GetResourceString("Date_DifferenceResultAutomationName");
			String localizedAutomationName = LocalizationStringUtil.GetLocalizedString(automationFormat, StrDateDiffResult);

			StrDateDiffResultAutomationName = localizedAutomationName.ToString();
		}

		private void UpdateStrDateResultAutomationName()
		{
			String automationFormat = AppResourceProvider.GetInstance().GetResourceString("Date_ResultingDateAutomationName");
			String localizedAutomationName = LocalizationStringUtil.GetLocalizedString(automationFormat, StrDateResult);

			StrDateResultAutomationName = localizedAutomationName.ToString();
		}

		private void InitializeDateOutputFormats(string calendarIdentifier)
		{
			// Format for Add/Subtract days
			m_dateTimeFormatter = LocalizationService.GetRegionalSettingsAwareDateTimeFormatter(
				"longdate",
				calendarIdentifier,
				ClockIdentifiers.TwentyFourHour); // Clock Identifier is not used

			// Format for Date Difference
			m_allDateUnitsOutputFormat = DateUnit.Year | DateUnit.Month | DateUnit.Week | DateUnit.Day;
			m_daysOutputFormat = DateUnit.Day;
		}

		private string GetDateDiffString()
		{
			String result = "";
			bool addDelimiter = false;
			AppResourceProvider resourceLoader = AppResourceProvider.GetInstance();

			var yearCount = m_dateDiffResult.year;

			if (yearCount > 0)
			{
				result = String.Concat(GetLocalizedNumberString(yearCount), " ");

				if (yearCount > 1)
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Years"));
				}
				else
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Year"));
				}

				// set the flags to add a delimiter whenever the next unit is added
				addDelimiter = true;
			}

			var monthCount = m_dateDiffResult.month;

			if (monthCount > 0)
			{
				if (addDelimiter)
				{
					result = String.Concat(result, m_listSeparator);
				}
				else
				{
					addDelimiter = true;
				}

				result = String.Concat(result, String.Concat(GetLocalizedNumberString(monthCount), " "));

				if (monthCount > 1)
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Months"));
				}
				else
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Month"));
				}
			}

			var weekCount = m_dateDiffResult.week;

			if (weekCount > 0)
			{
				if (addDelimiter)
				{
					result = String.Concat(result, m_listSeparator);
				}
				else
				{
					addDelimiter = true;
				}

				result = String.Concat(result, String.Concat(GetLocalizedNumberString(weekCount), " "));

				if (weekCount > 1)
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Weeks"));
				}
				else
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Week"));
				}
			}

			var dayCount = m_dateDiffResult.day;

			if (dayCount > 0)
			{
				if (addDelimiter)
				{
					result = String.Concat(result, m_listSeparator);
				}
				else
				{
					addDelimiter = true;
				}

				result = String.Concat(result, String.Concat(GetLocalizedNumberString(dayCount), " "));

				if (dayCount > 1)
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Days"));
				}
				else
				{
					result = String.Concat(result, resourceLoader.GetResourceString("Date_Day"));
				}
			}

			return result;
		}

		private string GetDateDiffStringInDays()
		{
			String strDateUnit;

			// Display the result as '1 day' or 'N days'
			if (m_dateDiffResultInDays.day > 1)
			{
				strDateUnit = AppResourceProvider.GetInstance().GetResourceString("Date_Days");
			}
			else
			{
				strDateUnit = AppResourceProvider.GetInstance().GetResourceString("Date_Day");
			}

			return String.Concat(GetLocalizedNumberString(m_dateDiffResultInDays.day), String.Concat(" ", strDateUnit));
		}

		private string GetLocalizedNumberString(int value)
		{
			string numberStr = value.ToString();
			LocalizationSettings.GetInstance().LocalizeDisplayValue(ref numberStr);

			return numberStr.ToString();
		}

		private static DateTime ClipTime(DateTime dateTime)
		{
			var calendar = new Windows.Globalization.Calendar();
			calendar.SetDateTime(dateTime);
			calendar.Period = 1;
			calendar.Hour = 12;
			calendar.Minute = 0;
			calendar.Second = 0;
			calendar.Nanosecond = 0;

			return calendar.GetDateTime().DateTime;
		}
	}
}
