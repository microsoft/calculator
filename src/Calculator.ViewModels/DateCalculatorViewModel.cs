// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.DateCalculation;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class DateCalculatorViewModel : INotifyPropertyChanged
    {
        private const int MaxOffsetValue = 999;

        // Input property backing fields
        private bool _isDateDiffMode;
        private bool _isAddMode;
        private bool _isDiffInDays;
        private int _daysOffset;
        private int _monthsOffset;
        private int _yearsOffset;

        // Date fields
        private DateTimeOffset _fromDate;
        private DateTimeOffset _toDate;
        private DateTimeOffset _startDate;
        private DateTimeOffset _dateResult;

        // Output property backing fields
        private string _strDateDiffResult = string.Empty;
        private string _strDateDiffResultAutomationName = string.Empty;
        private string _strDateDiffResultInDays = string.Empty;
        private string _strDateResult = string.Empty;
        private string _strDateResultAutomationName = string.Empty;

        // Private members
        private bool _isOutOfBound;
        private IList<string> _offsetValues;
        private DateDifference _dateDiffResult;
        private DateDifference _dateDiffResultInDays;
        private DateCalculationEngine _dateCalcEngine;
        private DateUnit _daysOutputFormat;
        private DateUnit _allDateUnitsOutputFormat;
        private Windows.Globalization.DateTimeFormatting.DateTimeFormatter _dateTimeFormatter;
        private string _listSeparator;

        public event PropertyChangedEventHandler PropertyChanged;

        public DateCalculatorViewModel()
        {
            _isDateDiffMode = true;
            _isAddMode = true;
            _isOutOfBound = false;
            _daysOffset = 0;
            _monthsOffset = 0;
            _yearsOffset = 0;
            _strDateDiffResult = string.Empty;
            _strDateDiffResultAutomationName = string.Empty;
            _strDateDiffResultInDays = string.Empty;
            _strDateResult = string.Empty;
            _strDateResultAutomationName = string.Empty;

            var localizationSettings = LocalizationSettings.GetInstance();

            // Initialize Date Output format instances
            InitializeDateOutputFormats(localizationSettings.GetCalendarIdentifier());

            // Initialize Date Calc engine
            _dateCalcEngine = new DateCalculationEngine(localizationSettings.GetCalendarIdentifier());

            // Initialize dates to today's date
            var calendar = new Windows.Globalization.Calendar();
            calendar.ChangeTimeZone("UTC");
            var today = calendar.GetDateTime();

            _fromDate = _toDate = ClipTime(today);
            _startDate = today;
            _dateResult = today;

            _listSeparator = localizationSettings.GetListSeparator() + " ";

            UpdateDisplayResult();

            _offsetValues = new List<string>();
            for (int i = 0; i <= MaxOffsetValue; i++)
            {
                string numberStr = i.ToString();
                localizationSettings.LocalizeDisplayValue(ref numberStr);
                _offsetValues.Add(numberStr);
            }
        }

        #region Properties

        public bool IsDateDiffMode
        {
            get => _isDateDiffMode;
            set
            {
                if (_isDateDiffMode != value)
                {
                    _isDateDiffMode = value;
                    RaisePropertyChanged(nameof(IsDateDiffMode));
                }
            }
        }

        public bool IsAddMode
        {
            get => _isAddMode;
            set
            {
                if (_isAddMode != value)
                {
                    _isAddMode = value;
                    RaisePropertyChanged(nameof(IsAddMode));
                }
            }
        }

        public bool IsDiffInDays
        {
            get => _isDiffInDays;
            private set
            {
                if (_isDiffInDays != value)
                {
                    _isDiffInDays = value;
                    RaisePropertyChanged(nameof(IsDiffInDays));
                }
            }
        }

        public int DaysOffset
        {
            get => _daysOffset;
            set
            {
                if (_daysOffset != value)
                {
                    _daysOffset = value;
                    RaisePropertyChanged(nameof(DaysOffset));
                }
            }
        }

        public int MonthsOffset
        {
            get => _monthsOffset;
            set
            {
                if (_monthsOffset != value)
                {
                    _monthsOffset = value;
                    RaisePropertyChanged(nameof(MonthsOffset));
                }
            }
        }

        public int YearsOffset
        {
            get => _yearsOffset;
            set
            {
                if (_yearsOffset != value)
                {
                    _yearsOffset = value;
                    RaisePropertyChanged(nameof(YearsOffset));
                }
            }
        }

        public IList<string> OffsetValues => _offsetValues;

        public DateTimeOffset FromDate
        {
            get => _fromDate;
            set
            {
                if (_fromDate != value)
                {
                    _fromDate = value;
                    RaisePropertyChanged(nameof(FromDate));
                }
            }
        }

        public DateTimeOffset ToDate
        {
            get => _toDate;
            set
            {
                if (_toDate != value)
                {
                    _toDate = value;
                    RaisePropertyChanged(nameof(ToDate));
                }
            }
        }

        public DateTimeOffset StartDate
        {
            get => _startDate;
            set
            {
                if (_startDate != value)
                {
                    _startDate = value;
                    RaisePropertyChanged(nameof(StartDate));
                }
            }
        }

        public string StrDateDiffResult
        {
            get => _strDateDiffResult;
            private set
            {
                if (_strDateDiffResult != value)
                {
                    _strDateDiffResult = value;
                    RaisePropertyChanged(nameof(StrDateDiffResult));
                }
            }
        }

        public string StrDateDiffResultAutomationName
        {
            get => _strDateDiffResultAutomationName;
            private set
            {
                if (_strDateDiffResultAutomationName != value)
                {
                    _strDateDiffResultAutomationName = value;
                    RaisePropertyChanged(nameof(StrDateDiffResultAutomationName));
                }
            }
        }

        public string StrDateDiffResultInDays
        {
            get => _strDateDiffResultInDays;
            private set
            {
                if (_strDateDiffResultInDays != value)
                {
                    _strDateDiffResultInDays = value;
                    RaisePropertyChanged(nameof(StrDateDiffResultInDays));
                }
            }
        }

        public string StrDateResult
        {
            get => _strDateResult;
            private set
            {
                if (_strDateResult != value)
                {
                    _strDateResult = value;
                    RaisePropertyChanged(nameof(StrDateResult));
                }
            }
        }

        public string StrDateResultAutomationName
        {
            get => _strDateResultAutomationName;
            private set
            {
                if (_strDateResultAutomationName != value)
                {
                    _strDateResultAutomationName = value;
                    RaisePropertyChanged(nameof(StrDateResultAutomationName));
                }
            }
        }

        #endregion

        #region Commands

        public DelegateCommand CopyCommand => new DelegateCommand(OnCopyCommand);

        #endregion

        #region Public Methods

        public void OnCopyCommand(object parameter)
        {
            if (IsDateDiffMode)
            {
                CopyPasteManager.CopyToClipboard(StrDateDiffResult);
            }
            else
            {
                CopyPasteManager.CopyToClipboard(StrDateResult);
            }
        }

        #endregion

        #region Private Methods

        private void OnPropertyChanged(string prop)
        {
            if (prop == nameof(IsDateDiffMode) || prop == nameof(IsAddMode)
                || prop == nameof(DaysOffset) || prop == nameof(MonthsOffset) || prop == nameof(YearsOffset))
            {
                OnInputsChanged();
            }

            if (prop == nameof(FromDate) || prop == nameof(ToDate) || prop == nameof(StartDate))
            {
                OnInputsChanged();
            }
        }

        private void OnInputsChanged()
        {
            if (IsDateDiffMode)
            {
                var clippedFrom = ClipTime(FromDate);
                var clippedTo = ClipTime(ToDate);

                // Calculate difference
                _dateDiffResult = _dateCalcEngine.TryGetDateDifference(clippedFrom, clippedTo, _allDateUnitsOutputFormat) ?? default;
                _dateDiffResultInDays = _dateCalcEngine.TryGetDateDifference(clippedFrom, clippedTo, _daysOutputFormat) ?? default;

                IsDiffInDays = (_dateDiffResult.Year == 0 && _dateDiffResult.Month == 0 && _dateDiffResult.Week == 0)
                    || (FromDate == ToDate);
            }
            else
            {
                _isOutOfBound = false;
                try
                {
                    var duration = new DateDifference
                    {
                        Year = IsAddMode ? YearsOffset : -YearsOffset,
                        Month = IsAddMode ? MonthsOffset : -MonthsOffset,
                        Day = IsAddMode ? DaysOffset : -DaysOffset
                    };
                    _dateResult = _dateCalcEngine.AddDuration(StartDate, duration) ?? StartDate;
                }
                catch
                {
                    _isOutOfBound = true;
                }
            }

            UpdateDisplayResult();
        }

        private void UpdateDisplayResult()
        {
            if (IsDateDiffMode)
            {
                StrDateDiffResult = _isOutOfBound ? string.Empty : GetDateDiffString();
                StrDateDiffResultInDays = _isOutOfBound ? string.Empty : GetDateDiffStringInDays();
                UpdateStrDateDiffResultAutomationName();
            }
            else
            {
                if (_isOutOfBound)
                {
                    StrDateResult = AppResourceProvider.GetInstance().GetResourceString("Date_OutOfBoundMessage");
                }
                else
                {
                    StrDateResult = _dateTimeFormatter?.Format(_dateResult) ?? _dateResult.ToString();
                }
                UpdateStrDateResultAutomationName();
            }
        }

        private void UpdateStrDateDiffResultAutomationName()
        {
            StrDateDiffResultAutomationName = StrDateDiffResult;
        }

        private void UpdateStrDateResultAutomationName()
        {
            StrDateResultAutomationName = StrDateResult;
        }

        private void InitializeDateOutputFormats(string calendarIdentifier)
        {
            _allDateUnitsOutputFormat = DateUnit.Year | DateUnit.Month | DateUnit.Week | DateUnit.Day;
            _daysOutputFormat = DateUnit.Day;

            _dateTimeFormatter = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter(
                "longdate",
                new[] { Windows.Globalization.Language.CurrentInputMethodLanguageTag },
                new Windows.Globalization.GeographicRegion().CodeTwoLetter ?? "US",
                calendarIdentifier,
                Windows.Globalization.ClockIdentifiers.TwentyFourHour);
        }

        private string GetDateDiffString()
        {

            var parts = new List<string>();
            if (_dateDiffResult.Year > 0)
                parts.Add(GetLocalizedNumberString(_dateDiffResult.Year) + " " +
                    AppResourceProvider.GetInstance().GetResourceString(_dateDiffResult.Year == 1 ? "Date_Year" : "Date_Years"));
            if (_dateDiffResult.Month > 0)
                parts.Add(GetLocalizedNumberString(_dateDiffResult.Month) + " " +
                    AppResourceProvider.GetInstance().GetResourceString(_dateDiffResult.Month == 1 ? "Date_Month" : "Date_Months"));
            if (_dateDiffResult.Week > 0)
                parts.Add(GetLocalizedNumberString(_dateDiffResult.Week) + " " +
                    AppResourceProvider.GetInstance().GetResourceString(_dateDiffResult.Week == 1 ? "Date_Week" : "Date_Weeks"));
            if (_dateDiffResult.Day > 0 || parts.Count == 0)
                parts.Add(GetLocalizedNumberString(_dateDiffResult.Day) + " " +
                    AppResourceProvider.GetInstance().GetResourceString(_dateDiffResult.Day == 1 ? "Date_Day" : "Date_Days"));

            return string.Join(_listSeparator, parts);
        }

        private string GetDateDiffStringInDays()
        {

            return GetLocalizedNumberString(_dateDiffResultInDays.Day) + " " +
                AppResourceProvider.GetInstance().GetResourceString(_dateDiffResultInDays.Day == 1 ? "Date_Day" : "Date_Days");
        }

        private string GetLocalizedNumberString(int value)
        {
            string result = value.ToString();
            LocalizationSettings.GetInstance().LocalizeDisplayValue(ref result);
            return result;
        }

        private static DateTimeOffset ClipTime(DateTimeOffset dateTime, bool adjustUsingLocalTime = false)
        {
            // Clip to midnight UTC to avoid DST issues
            var utc = dateTime.ToUniversalTime();
            return new DateTimeOffset(utc.Year, utc.Month, utc.Day, 0, 0, 0, TimeSpan.Zero);
        }

        #endregion

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            OnPropertyChanged(propertyName);
        }
    }
}
