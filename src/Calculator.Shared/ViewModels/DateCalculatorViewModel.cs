//using System;
//using System.Collections.Generic;
//using System.ComponentModel;
//using System.Text;
//using Windows.UI.Xaml.Data;
//using Windows.Globalization;
//using CalculatorApp.Common;
//using Windows.Globalization.DateTimeFormatting;
//using CalculatorApp.Common.DateCalculation;
//using Windows.Foundation;
//using Platform.Collections;
//using System.Numerics;

//namespace WindowsCalculator.Shared.ViewModels
//{
//	/* MAX?
//	{
//    StringReference StrDateDiffResult(L"StrDateDiffResult");
//	StringReference StrDateDiffResultAutomationName(L"StrDateDiffResultAutomationName");
//	StringReference StrDateDiffResultInDays(L"StrDateDiffResultInDays");
//	StringReference StrDateResult(L"StrDateResult");
//	StringReference StrDateResultAutomationName(L"StrDateResultAutomationName");
//	StringReference IsDiffInDays(L"IsDiffInDays");
//	}
//	*/

//	[Bindable]
//	public sealed class DateCalculatorViewModel : INotifyPropertyChanged
//	{
//		// Private members
//		private DateCalculationEngine m_dateCalcEngine;
//		private DateUnit m_daysOutputFormat;
//		private DateUnit m_allDateUnitsOutputFormat;
//		private DateTimeFormatter m_dateTimeFormatter;
//		private string m_listSeparator;

//		private bool m_isOutOfBound;
//		private Vector<string> m_offsetValues;
//		private DateTime m_fromDate;
//		private DateTime m_toDate;
//		private DateTime m_startDate;
//		private DateTime m_dateResult;
//		private DateDifference m_dateDiffResult;
//		private DateDifference m_dateDiffResultInDays;

//		public DateCalculatorViewModel()
//		{
//			m_IsDateDiffMode = true;
//			m_IsAddMode = true;
//			m_isOutOfBound = false;
//			m_DaysOffset = 0;
//			m_MonthsOffset = 0;
//			m_YearsOffset = 0;
//			m_StrDateDiffResult = "";
//			m_StrDateDiffResultAutomationName = "";
//			m_StrDateDiffResultInDays = "";
//			m_StrDateResult = "";
//			m_StrDateResultAutomationName = "";
//			m_fromDate = 0;
//			m_toDate = 0;
//			m_startDate = 0;
//			m_dateResult = 0;

//			var localizationSettings = LocalizationSettings.GetInstance();

//			// Initialize Date Output format instances
//			InitializeDateOutputFormats(localizationSettings.GetCalendarIdentifier());

//			// Initialize Date Calc engine
//			m_dateCalcEngine = make_shared<DateCalculationEngine>(localizationSettings.GetCalendarIdentifier());

//			// Initialize dates of DatePicker controls to today's date
//			var calendar = new Calendar();
//			var today = calendar.GetDateTime();

//			// FromDate and ToDate should be clipped (adjusted to a consistent hour in UTC)
//			m_fromDate = today;
//			m_toDate = today;
//			FromDate = ClipTime(today);
//			ToDate = ClipTime(today);

//			// StartDate should not be clipped
//			StartDate = today;
//			m_dateResult = today;

//			// Initialize the list separator delimiter appended with a space at the end, e.g. ", "
//			// This will be used for date difference formatting: Y years, M months, W weeks, D days
//			m_listSeparator = new String((localizationSettings.GetListSeparator() + " ").ToString());

//			// Initialize the output results
//			UpdateDisplayResult();

//			m_offsetValues = new Vector<String> ();

//			for (int i = 0; i <= c_maxOffsetValue; i++)
//			{
//				// MAX?
//				string numberStr = i.ToString();
//				localizationSettings.LocalizeDisplayValue(numberStr);
//				m_offsetValues.Append(numberStr.ToString());
//			}

//			/* In the ClipTime function, we used to change timezone to UTC before clipping the time.
//				The comment from the previous delopers said this was done to eliminate the effects of
//				Daylight Savings Time. We can't think of a good reason why this change in timezone is
//				necessary and did find bugs related to the change, therefore, we have removed the
//				change. Just in case, we will see if the clipped time is ever a different day from the
//				original day, which would hopefully indicate the change in timezone was actually
//				necessary. We will collect telemetry if we find this case. If we don't see any
//				telemetry events after the application has been used for some time, we will feel safe
//				and can remove this function. */
//			DayOfWeek trueDayOfWeek = calendar->DayOfWeek;

//			DateTime clippedTime = ClipTime(today);
//			calendar.SetDateTime(clippedTime);

//			if (calendar->DayOfWeek != trueDayOfWeek)
//			{
//				// MAX?
//				calendar.SetDateTime(today);
//				TraceLogger.GetInstance().LogDateClippedTimeDifferenceFound(
//					from_cx<Calendar>(calendar),
//					DateTime{ TimeSpan{ clippedTime.UniversalTime } });
//			}
//		}

//		static void CheckClipTimeSameDay(Calendar reference)
//		{

//		}

//		private bool IsOutOfBound
//		{
//			get { return m_isOutOfBound; }
//			set
//			{
//				m_isOutOfBound = value;
//				UpdateDisplayResult();
//			}
//		}

//		private DateDifference DateDiffResult
//		{
//			get { return m_dateDiffResult; }
//			set {
//				m_dateDiffResult = value;
//				UpdateDisplayResult();
//			}
//		}

//		private DateDifference DateDiffResultInDays
//		{
//			get { return m_dateDiffResultInDays; }
//			set
//			{
//				m_dateDiffResultInDays = value;
//				UpdateDisplayResult();
//			}
//		}

//		private DateTime DateResult
//		{
//			get { return m_dateResult; }
//			set {
//				m_dateResult = value;
//				UpdateDisplayResult();
//			}
//		}

//		// PUBLIC

//		// Input Properties
//		public string IsDateDiffMode { get; set; }
//		public string IsAddMode { get; set; }
//		public string IsDiffInDays { get; set; } // If diff is only in days or the dates are the same,
//												 // then show only one result and avoid redundancy

//		public string DaysOffset { get; set; }
//		public string MonthsOffset { get; set; }
//		public string YearsOffset { get; set; }

//		// Read only property for offset values
//		public IVector<String> OffsetValues
//		{
//			get { return m_offsetValues; }
//		}

//		// From date for Date Diff
//		public DateTime FromDate
//		{
//			DateTime get() { return m_fromDate; }
//			set
//			{
//				if (m_fromDate.UniversalTime != value.UniversalTime)
//				{
//					m_fromDate = value;
//					RaisePropertyChanged("FromDate");
//				}
//			}
//		}

//		// To date for Date Diff
//		public DateTime ToDate
//		{
//			get { return m_toDate; }
//			set
//			{
//				if (m_toDate.UniversalTime != value.UniversalTime)
//				{
//					m_toDate = value;
//					RaisePropertyChanged("ToDate");
//				}
//			}
//		}

//		// Start date for Add/Subtract date
//		public DateTime StartDate
//		{
//			get { return m_startDate; }
//			set
//			{
//				if (m_startDate.UniversalTime != value.UniversalTime)
//				{
//					m_startDate = value;
//					RaisePropertyChanged("StartDate");
//				}
//			}
//		}

//		// Output Properties
//		public string StrDateDiffResult { get; set; }

//		public string StrDateDiffResultAutomationName { get; set; }

//		public string StrDateDiffResultInDays { get; set; }

//		public string StrDateResult { get; set; }

//		public string StrDateResultAutomationName { get; set; }

//		public void OnCopyCommand(Object parameter)
//		{

//		}

//		// PRIVATE
//		private void OnPropertyChanged(string prop)
//		{
//			if (prop == DateCalculatorViewModelProperties.StrDateDiffResult)
//			{
//				UpdateStrDateDiffResultAutomationName();
//			}
//			else if (prop == DateCalculatorViewModelProperties.StrDateResult)
//			{
//				UpdateStrDateResultAutomationName();
//			}
//			else if (prop != DateCalculatorViewModelProperties.StrDateDiffResultAutomationName
//				&& prop != DateCalculatorViewModelProperties.StrDateDiffResultInDays
//				&& prop != DateCalculatorViewModelProperties.StrDateResultAutomationName
//				&& prop != DateCalculatorViewModelProperties.IsDiffInDays)
//			{
//				OnInputsChanged();
//			}
//		}

//		private void OnInputsChanged()
//		{
//			DateDifference dateDiff;

//			if (m_IsDateDiffMode)
//			{
//				DateTime clippedFromDate = ClipTime(FromDate);
//				DateTime clippedToDate = ClipTime(ToDate);

//				// Calculate difference between two dates
//				m_dateCalcEngine->GetDateDifference(clippedFromDate, clippedToDate, m_allDateUnitsOutputFormat, &dateDiff);
//				DateDiffResult = dateDiff;

//				m_dateCalcEngine->GetDateDifference(clippedFromDate, clippedToDate, m_daysOutputFormat, &dateDiff);
//				DateDiffResultInDays = dateDiff;
//			}
//			else
//			{
//				dateDiff.day = DaysOffset;
//				dateDiff.month = MonthsOffset;
//				dateDiff.year = YearsOffset;

//				DateTime dateTimeResult;

//				if (m_IsAddMode)
//				{
//					// Add number of Days, Months and Years to a Date
//					IsOutOfBound = !m_dateCalcEngine->AddDuration(StartDate, dateDiff, &dateTimeResult);
//				}
//				else
//				{
//					// Subtract number of Days, Months and Years from a Date
//					IsOutOfBound = !m_dateCalcEngine->SubtractDuration(StartDate, dateDiff, &dateTimeResult);
//				}

//				if (!m_isOutOfBound)
//				{
//					DateResult = dateTimeResult;
//				}
//			}
//		}

//		private void UpdateDisplayResult()
//		{
//			if (m_IsDateDiffMode)
//			{
//				// Are to and from dates the same
//				if (m_dateDiffResultInDays.day == 0)
//				{
//					IsDiffInDays = true;
//					StrDateDiffResultInDays = "";
//					StrDateDiffResult = AppResourceProvider.GetInstance().GetResourceString("Date_SameDates");
//				}
//				else if ((m_dateDiffResult.year == 0) &&
//						(m_dateDiffResult.month == 0) &&
//						(m_dateDiffResult.week == 0))
//				{
//					IsDiffInDays = true;
//					StrDateDiffResultInDays = "";

//					// Display result in number of days
//					StrDateDiffResult = GetDateDiffStringInDays();
//				}
//				else
//				{
//					IsDiffInDays = false;

//					// Display result in days, weeks, months and years
//					StrDateDiffResult = GetDateDiffString();

//					// Display result in number of days
//					StrDateDiffResultInDays = GetDateDiffStringInDays();
//				}
//			}
//			else
//			{
//				if (m_isOutOfBound)
//				{
//					// Display Date out of bound message
//					StrDateResult = AppResourceProvider.GetInstance().GetResourceString("Date_OutOfBoundMessage");
//				}
//				else
//				{
//					// Display the resulting date in long format
//					StrDateResult = m_dateTimeFormatter.Format(DateResult);
//				}
//			}
//		}

//		private void UpdateStrDateDiffResultAutomationName()
//		{
//			String automationFormat = AppResourceProvider.GetInstance().GetResourceString("Date_DifferenceResultAutomationName");
//			String localizedAutomationName = LocalizationStringUtil.GetLocalizedString(automationFormat->Data(), StrDateDiffResult.Data());

//			StrDateDiffResultAutomationName = new String(localizedAutomationName.ToString());
//		}

//		private void UpdateStrDateResultAutomationName()
//		{
//			String automationFormat = AppResourceProvider.GetInstance().GetResourceString("Date_ResultingDateAutomationName");
//			String localizedAutomationName = LocalizationStringUtil.GetLocalizedString(automationFormat.Data(), StrDateResult.Data());

//			StrDateResultAutomationName = new String(localizedAutomationName.ToString());
//		}

//		private void InitializeDateOutputFormats(string calendarIdentifer)
//		{
//			// Format for Add/Subtract days
//			m_dateTimeFormatter = LocalizationService.GetRegionalSettingsAwareDateTimeFormatter(
//				"longdate",
//				calendarIdentifier,
//				ClockIdentifiers.TwentyFourHour); // Clock Identifier is not used

//			// Format for Date Difference
//			m_allDateUnitsOutputFormat = DateUnit.Year | DateUnit.Month | DateUnit.Week | DateUnit.Day;
//			m_daysOutputFormat = DateUnit.Day;
//		}

//		private string GetDateDiffString()
//		{
//			String result = "";
//			bool addDelimiter = false;
//			AppResourceProvider resourceLoader = AppResourceProvider.GetInstance();

//			var yearCount = m_dateDiffResult.year;

//			if (yearCount > 0)
//			{
//				result = String.Concat(GetLocalizedNumberString(yearCount), " ");

//				if (yearCount > 1)
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Years"));
//				}
//				else
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Year"));
//				}

//				// set the flags to add a delimiter whenever the next unit is added
//				addDelimiter = true;
//			}

//			var monthCount = m_dateDiffResult.month;

//			if (monthCount > 0)
//			{
//				if (addDelimiter)
//				{
//					result = String.Concat(result, m_listSeparator);
//				}
//				else
//				{
//					addDelimiter = true;
//				}

//				result = String.Concat(result, String.Concat(GetLocalizedNumberString(monthCount), " "));

//				if (monthCount > 1)
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Months"));
//				}
//				else
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Month"));
//				}
//			}

//			var weekCount = m_dateDiffResult.week;

//			if (weekCount > 0)
//			{
//				if (addDelimiter)
//				{
//					result = String.Concat(result, m_listSeparator);
//				}
//				else
//				{
//					addDelimiter = true;
//				}

//				result = String.Concat(result, String.Concat(GetLocalizedNumberString(weekCount), " "));

//				if (weekCount > 1)
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Weeks"));
//				}
//				else
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Week"));
//				}
//			}

//			var dayCount = m_dateDiffResult.day;

//			if (dayCount > 0)
//			{
//				if (addDelimiter)
//				{
//					result = String.Concat(result, m_listSeparator);
//				}
//				else
//				{
//					addDelimiter = true;
//				}

//				result = String.Concat(result, String.Concat(GetLocalizedNumberString(dayCount), " "));

//				if (dayCount > 1)
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Days"));
//				}
//				else
//				{
//					result = String.Concat(result, resourceLoader.GetResourceString("Date_Day"));
//				}
//			}

//			return result;
//		}

//		private string GetDateDiffStringInDays()
//		{
//			String strDateUnit;

//			// Display the result as '1 day' or 'N days'
//			if (m_dateDiffResultInDays.day > 1)
//			{
//				strDateUnit = AppResourceProvider.GetInstance().GetResourceString("Date_Days");
//			}
//			else
//			{
//				strDateUnit = AppResourceProvider.GetInstance().GetResourceString("Date_Day");
//			}

//			return String.Concat(GetLocalizedNumberString(m_dateDiffResultInDays.day), String.Concat(" ", strDateUnit));
//		}

//		private string GetLocalizedNumberString(int value)
//		{
//			string numberStr = value.ToString();
//			LocalizationSettings.GetInstance().LocalizeDisplayValue(numberStr);

//			return numberStr.ToString();
//		}

//		private static DateTime ClipTime(DateTime dateTime)
//		{
//			var calendar = new Calendar();
//			calendar.SetDateTime(dateTime);
//			calendar.Period = 1;
//			calendar.Hour = 12;
//			calendar.Minute = 0;
//			calendar.Second = 0;
//			calendar.Nanosecond = 0;

//			return calendar.GetDateTime();
//		}

//		// MAX?
//		private static void CheckClipTimeSameDay(Calendar reference)
//		{

//		}
//	}
//}