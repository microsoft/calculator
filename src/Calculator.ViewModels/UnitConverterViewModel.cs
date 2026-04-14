// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Threading;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class Category : INotifyPropertyChanged
    {
        private readonly int _id;
        private readonly string _name;
        private readonly bool _supportsNegative;

        public event PropertyChangedEventHandler PropertyChanged;

        internal Category(int id, string name, bool supportsNegative)
        {
            _id = id;
            _name = name;
            _supportsNegative = supportsNegative;
        }

        public string Name => _name;

        public Windows.UI.Xaml.Visibility NegateVisibility =>
            _supportsNegative ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;

        public int GetModelCategoryId() => _id;
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class Unit : INotifyPropertyChanged
    {
        private readonly int _id;
        private readonly string _name;
        private readonly string _accessibleName;
        private readonly string _abbreviation;
        private readonly bool _isWhimsical;

        public event PropertyChangedEventHandler PropertyChanged;

        internal Unit(int id, string name, string abbreviation, string accessibleName, bool isWhimsical = false)
        {
            _id = id;
            _name = name;
            _abbreviation = abbreviation;
            _accessibleName = accessibleName;
            _isWhimsical = isWhimsical;
        }

        public string Name => _name;
        public string AccessibleName => _accessibleName;
        public string Abbreviation => _abbreviation;

        public override string ToString() => AccessibleName;

        public bool IsModelUnitWhimsical() => _isWhimsical;
        public int ModelUnitID() => _id;
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class SupplementaryResult : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private string _value;
        private Unit _unit;

        internal SupplementaryResult(string value, Unit unit)
        {
            _value = value;
            _unit = unit;
        }

        public bool IsWhimsical() => _unit?.IsModelUnitWhimsical() ?? false;

        public string GetLocalizedAutomationName()
        {
            return $"{_value} {_unit?.Name}";
        }

        public string Value
        {
            get => _value;
            private set
            {
                if (_value != value)
                {
                    _value = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Value)));
                }
            }
        }

        public Unit Unit
        {
            get => _unit;
            private set
            {
                if (_unit != value)
                {
                    _unit = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Unit)));
                }
            }
        }
    }

    public interface IActivatable
    {
        bool IsActive { get; set; }
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class UnitConverterViewModel : INotifyPropertyChanged
    {
        public const string NetworkBehaviorPropertyName = "NetworkBehavior";
        public const string CurrencyDataLoadFailedPropertyName = "CurrencyDataLoadFailed";
        public const string CurrencyDataIsWeekOldPropertyName = "CurrencyDataIsWeekOld";
        public const string IsCurrencyLoadingVisiblePropertyName = "IsCurrencyLoadingVisible";
        public const string IsCurrencyCurrentCategoryPropertyName = "IsCurrencyCurrentCategory";

        // Model
        private readonly IUnitConverter _model;
        private char _decimalSeparator;

        // Observable properties backing fields
        private ObservableCollection<Category> _categories;
        private ViewMode _mode;
        private ObservableCollection<Unit> _units;
        private string _currencySymbol1;
        private Unit _unit1;
        private string _value1;
        private string _currencySymbol2;
        private Unit _unit2;
        private string _value2;
        private ObservableCollection<SupplementaryResult> _supplementaryResults;
        private bool _value1Active;
        private bool _value2Active;
        private string _value1AutomationName = string.Empty;
        private string _value2AutomationName = string.Empty;
        private string _unit1AutomationName = string.Empty;
        private string _unit2AutomationName = string.Empty;
        private NarratorAnnouncement _announcement;
        private bool _isDecimalEnabled;
        private bool _isDropDownOpen;
        private bool _isDropDownEnabled;
        private bool _isCurrencyLoadingVisible;
        private bool _isCurrencyCurrentCategory;
        private string _currencyRatioEquality;
        private string _currencyRatioEqualityAutomationName;
        private string _currencyTimestamp;
        private NetworkAccessBehavior _networkBehavior;
        private bool _currencyDataLoadFailed;
        private bool _currencyDataIsWeekOld;
        private Category _currentCategory;

        // Internal state
        private bool _isInputBlocked;
        private Timer _supplementaryResultsTimer;
        private bool _resettingTimer;
        private List<(string Value, int UnitId)> _cachedSuggestedValues;
        private readonly object _cacheMutex = new object();
        private string _valueFromUnlocalized;
        private string _valueToUnlocalized;
        private bool _relocalizeStringOnSwitch;
        private bool _isValue1Updating;
        private bool _isValue2Updating;
        private string _lastAnnouncedFrom;
        private string _lastAnnouncedTo;
        private string _lastAnnouncedConversionResult;
        private bool _isCurrencyDataLoaded;

        // Localized format strings
        private string _localizedValueFromFormat;
        private string _localizedValueFromDecimalFormat;
        private string _localizedValueToFormat;
        private string _localizedConversionResultFormat;
        private string _localizedInputUnitName;
        private string _localizedOutputUnitName;

        private enum ConversionParameter { Source, Target }
        private ConversionParameter _value1cp;

        public event PropertyChangedEventHandler PropertyChanged;

        public UnitConverterViewModel()
            : this(null)
        {
        }

        internal UnitConverterViewModel(IUnitConverter model)
        {
            _model = model;
            _categories = new ObservableCollection<Category>();
            _units = new ObservableCollection<Unit>();
            _supplementaryResults = new ObservableCollection<SupplementaryResult>();
            _value1 = "0";
            _value2 = "0";
            _currencySymbol1 = string.Empty;
            _currencySymbol2 = string.Empty;
            _value1Active = true;
            _value2Active = false;
            _isDecimalEnabled = true;
            _isDropDownEnabled = true;
            _currencyRatioEquality = string.Empty;
            _currencyRatioEqualityAutomationName = string.Empty;
            _currencyTimestamp = string.Empty;
            _value1cp = ConversionParameter.Source;
            _decimalSeparator = LocalizationSettings.GetInstance().GetDecimalSeparator();
        }

        #region Observable Properties

        public ObservableCollection<Category> Categories
        {
            get => _categories;
            private set { if (_categories != value) { _categories = value; RaisePropertyChanged(nameof(Categories)); } }
        }

        public ViewMode Mode
        {
            get => _mode;
            set { if (_mode != value) { _mode = value; RaisePropertyChanged(nameof(Mode)); } }
        }

        public ObservableCollection<Unit> Units
        {
            get => _units;
            private set { if (_units != value) { _units = value; RaisePropertyChanged(nameof(Units)); } }
        }

        public string CurrencySymbol1
        {
            get => _currencySymbol1;
            set { if (_currencySymbol1 != value) { _currencySymbol1 = value; RaisePropertyChanged(nameof(CurrencySymbol1)); } }
        }

        public Unit Unit1
        {
            get => _unit1;
            set { if (_unit1 != value) { _unit1 = value; RaisePropertyChanged(nameof(Unit1)); } }
        }

        public string Value1
        {
            get => _value1;
            set { if (_value1 != value) { _value1 = value; RaisePropertyChanged(nameof(Value1)); } }
        }

        public string CurrencySymbol2
        {
            get => _currencySymbol2;
            set { if (_currencySymbol2 != value) { _currencySymbol2 = value; RaisePropertyChanged(nameof(CurrencySymbol2)); } }
        }

        public Unit Unit2
        {
            get => _unit2;
            set { if (_unit2 != value) { _unit2 = value; RaisePropertyChanged(nameof(Unit2)); } }
        }

        public string Value2
        {
            get => _value2;
            set { if (_value2 != value) { _value2 = value; RaisePropertyChanged(nameof(Value2)); } }
        }

        public ObservableCollection<SupplementaryResult> SupplementaryResults
        {
            get => _supplementaryResults;
            private set { if (_supplementaryResults != value) { _supplementaryResults = value; RaisePropertyChanged(nameof(SupplementaryResults)); } }
        }

        public bool Value1Active
        {
            get => _value1Active;
            set { if (_value1Active != value) { _value1Active = value; RaisePropertyChanged(nameof(Value1Active)); } }
        }

        public bool Value2Active
        {
            get => _value2Active;
            set { if (_value2Active != value) { _value2Active = value; RaisePropertyChanged(nameof(Value2Active)); } }
        }

        public string Value1AutomationName
        {
            get => _value1AutomationName;
            set { if (_value1AutomationName != value) { _value1AutomationName = value; RaisePropertyChanged(nameof(Value1AutomationName)); } }
        }

        public string Value2AutomationName
        {
            get => _value2AutomationName;
            set { if (_value2AutomationName != value) { _value2AutomationName = value; RaisePropertyChanged(nameof(Value2AutomationName)); } }
        }

        public string Unit1AutomationName
        {
            get => _unit1AutomationName;
            set { if (_unit1AutomationName != value) { _unit1AutomationName = value; RaisePropertyChanged(nameof(Unit1AutomationName)); } }
        }

        public string Unit2AutomationName
        {
            get => _unit2AutomationName;
            set { if (_unit2AutomationName != value) { _unit2AutomationName = value; RaisePropertyChanged(nameof(Unit2AutomationName)); } }
        }

        public NarratorAnnouncement Announcement
        {
            get => _announcement;
            set { if (_announcement != value) { _announcement = value; RaisePropertyChanged(nameof(Announcement)); } }
        }

        public bool IsDecimalEnabled
        {
            get => _isDecimalEnabled;
            set { if (_isDecimalEnabled != value) { _isDecimalEnabled = value; RaisePropertyChanged(nameof(IsDecimalEnabled)); } }
        }

        public bool IsDropDownOpen
        {
            get => _isDropDownOpen;
            set { if (_isDropDownOpen != value) { _isDropDownOpen = value; RaisePropertyChanged(nameof(IsDropDownOpen)); } }
        }

        public bool IsDropDownEnabled
        {
            get => _isDropDownEnabled;
            set { if (_isDropDownEnabled != value) { _isDropDownEnabled = value; RaisePropertyChanged(nameof(IsDropDownEnabled)); } }
        }

        public bool IsCurrencyLoadingVisible
        {
            get => _isCurrencyLoadingVisible;
            set { if (_isCurrencyLoadingVisible != value) { _isCurrencyLoadingVisible = value; RaisePropertyChanged(nameof(IsCurrencyLoadingVisible)); } }
        }

        public bool IsCurrencyCurrentCategory
        {
            get => _isCurrencyCurrentCategory;
            private set { if (_isCurrencyCurrentCategory != value) { _isCurrencyCurrentCategory = value; RaisePropertyChanged(nameof(IsCurrencyCurrentCategory)); } }
        }

        public string CurrencyRatioEquality
        {
            get => _currencyRatioEquality;
            set { if (_currencyRatioEquality != value) { _currencyRatioEquality = value; RaisePropertyChanged(nameof(CurrencyRatioEquality)); } }
        }

        public string CurrencyRatioEqualityAutomationName
        {
            get => _currencyRatioEqualityAutomationName;
            set { if (_currencyRatioEqualityAutomationName != value) { _currencyRatioEqualityAutomationName = value; RaisePropertyChanged(nameof(CurrencyRatioEqualityAutomationName)); } }
        }

        public string CurrencyTimestamp
        {
            get => _currencyTimestamp;
            set { if (_currencyTimestamp != value) { _currencyTimestamp = value; RaisePropertyChanged(nameof(CurrencyTimestamp)); } }
        }

        public NetworkAccessBehavior NetworkBehavior
        {
            get => _networkBehavior;
            set { if (_networkBehavior != value) { _networkBehavior = value; RaisePropertyChanged(nameof(NetworkBehavior)); } }
        }

        public bool CurrencyDataLoadFailed
        {
            get => _currencyDataLoadFailed;
            set { if (_currencyDataLoadFailed != value) { _currencyDataLoadFailed = value; RaisePropertyChanged(nameof(CurrencyDataLoadFailed)); } }
        }

        public bool CurrencyDataIsWeekOld
        {
            get => _currencyDataIsWeekOld;
            set { if (_currencyDataIsWeekOld != value) { _currencyDataIsWeekOld = value; RaisePropertyChanged(nameof(CurrencyDataIsWeekOld)); } }
        }

        public Category CurrentCategory
        {
            get => _currentCategory;
            set
            {
                if (_currentCategory != value)
                {
                    _currentCategory = value;
                    if (value != null)
                    {
                        IsCurrencyCurrentCategory = value.GetModelCategoryId() ==
                            NavCategoryStates.Serialize(ViewMode.Currency);
                    }
                    RaisePropertyChanged(nameof(CurrentCategory));
                }
            }
        }

        public Windows.UI.Xaml.Visibility SupplementaryVisibility =>
            SupplementaryResults?.Count > 0 ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;

        public Windows.UI.Xaml.Visibility CurrencySymbolVisibility =>
            string.IsNullOrEmpty(CurrencySymbol1) || string.IsNullOrEmpty(CurrencySymbol2)
                ? Windows.UI.Xaml.Visibility.Collapsed
                : Windows.UI.Xaml.Visibility.Visible;

        #endregion

        #region Commands

        public DelegateCommand CategoryChangedCommand => new DelegateCommand(OnCategoryChanged);
        public DelegateCommand UnitChangedCommand => new DelegateCommand(OnUnitChanged);
        public DelegateCommand SwitchActiveCommand => new DelegateCommand(OnSwitchActive);
        public DelegateCommand ButtonPressedCommand => new DelegateCommand(OnButtonPressed);
        public DelegateCommand CopyCommand => new DelegateCommand(OnCopyCommand);
        public DelegateCommand PasteCommand => new DelegateCommand(OnPasteCommand);

        #endregion

        #region Public Methods

        public void AnnounceConversionResult()
        {
            string localizedResult = GetLocalizedConversionResultStringFormat(
                Value1, Unit1?.Name ?? string.Empty,
                Value2, Unit2?.Name ?? string.Empty);

            if (localizedResult != _lastAnnouncedConversionResult)
            {
                _lastAnnouncedConversionResult = localizedResult;
                Announcement = CalculatorAnnouncement.GetDisplayUpdatedAnnouncement(localizedResult);
            }
        }

        public void OnPaste(string stringToPaste)
        {
            if (string.IsNullOrEmpty(stringToPaste))
            {
                return;
            }

            // Validate and set the pasted value
            _model?.SetCurrentUnitValue(stringToPaste);
        }

        public void RefreshCurrencyRatios()
        {
            // Trigger a re-fetch of currency data
            _model?.RefreshCurrencyRatios();
        }

        public void OnValueActivated(IActivatable control)
        {
            if (control != null)
            {
                control.IsActive = true;
            }
        }

        public void OnCopyCommand(object parameter)
        {
            string valueToCopy = Value1Active ? Value1 : Value2;
            CopyPasteManager.CopyToClipboard(valueToCopy);
        }

        public void OnPasteCommand(object parameter)
        {
            // Paste from clipboard
        }

        #endregion

        #region Internal Methods

        internal void ResetView()
        {
            Value1 = "0";
            Value2 = "0";
        }

        internal void PopulateData()
        {
            // Load categories and units from the model
        }

        internal NumbersAndOperatorsEnum MapCharacterToButtonId(char ch, out bool canSendNegate)
        {
            canSendNegate = false;

            if (ch >= '0' && ch <= '9')
            {
                canSendNegate = true;
                return NumbersAndOperatorsEnum.Zero + (ch - '0');
            }

            if (ch == _decimalSeparator)
            {
                return NumbersAndOperatorsEnum.Decimal;
            }

            if (ch == '-')
            {
                return NumbersAndOperatorsEnum.Negate;
            }

            return NumbersAndOperatorsEnum.None;
        }

        internal void DisplayPasteError()
        {
            // Display an error for invalid paste
        }

        internal void UpdateDisplay(string from, string to)
        {
            _valueFromUnlocalized = from;
            _valueToUnlocalized = to;

            Value1 = ConvertToLocalizedString(from);
            Value2 = ConvertToLocalizedString(to);

            UpdateValue1AutomationName();
            UpdateValue2AutomationName();
        }

        internal void UpdateSupplementaryResults(List<(string Value, int UnitId)> suggestedValues)
        {
            lock (_cacheMutex)
            {
                _cachedSuggestedValues = suggestedValues;
            }
            RefreshSupplementaryResults();
        }

        internal void OnMaxDigitsReached()
        {
            // Announce max digits reached
        }

        internal void SaveUserPreferences()
        {
            // Save category and unit preferences to local settings
        }

        internal void RestoreUserPreferences()
        {
            // Restore category and unit preferences from local settings
        }

        internal void OnCurrencyDataLoadFinished(bool didLoad)
        {
            _isCurrencyDataLoaded = didLoad;
            IsCurrencyLoadingVisible = false;
            CurrencyDataLoadFailed = !didLoad;
        }

        internal void OnCurrencyTimestampUpdated(string timestamp, bool isWeekOld)
        {
            CurrencyTimestamp = timestamp;
            CurrencyDataIsWeekOld = isWeekOld;
        }

        internal void OnNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
        {
            NetworkBehavior = newBehavior;
        }

        internal string GetValueFromUnlocalized() => _valueFromUnlocalized;
        internal string GetValueToUnlocalized() => _valueToUnlocalized;

        #endregion

        #region Private Methods

        private void InitializeView()
        {
            PopulateData();
            RestoreUserPreferences();
        }

        private void OnPropertyChanged(string prop)
        {
            if (prop == nameof(Mode))
            {
                InitializeView();
            }
        }

        private void OnCategoryChanged(object unused)
        {
            if (CurrentCategory != null)
            {
                _model?.SwitchCategory(CurrentCategory.GetModelCategoryId());
            }
        }

        private void OnUnitChanged(object unused)
        {
            // Update conversion when units change
        }

        private void OnSwitchActive(object unused)
        {
            Value1Active = !Value1Active;
            Value2Active = !Value2Active;
            _value1cp = _value1cp == ConversionParameter.Source ? ConversionParameter.Target : ConversionParameter.Source;
        }

        private void OnButtonPressed(object parameter)
        {
            NumbersAndOperatorsEnum numOp;
            if (parameter is CalculatorButtonPressedEventArgs eventArgs)
            {
                numOp = eventArgs.Operation;
            }
            else if (parameter is NumbersAndOperatorsEnum numOpDirect)
            {
                numOp = numOpDirect;
            }
            else
            {
                return;
            }
            _model?.SendCommand((int)numOp);
        }

        private void RefreshSupplementaryResults()
        {
            // Update supplementary results collection on UI thread
        }

        private void UpdateInputBlocked(string currencyInput)
        {
            // Block input during currency loading
        }

        private void UpdateCurrencyFormatter()
        {
            // Update currency formatters based on current units
        }

        private void UpdateIsDecimalEnabled()
        {
            // Enable/disable decimal based on current state
        }

        private bool UnitsAreValid()
        {
            return Unit1 != null && Unit2 != null;
        }

        private void ResetCategory()
        {
            // Reset category to default
        }

        private string ConvertToLocalizedString(string stringToLocalize)
        {
            if (string.IsNullOrEmpty(stringToLocalize))
            {
                return "0";
            }
            LocalizationSettings.GetInstance().LocalizeDisplayValue(ref stringToLocalize);
            return stringToLocalize;
        }

        internal string GetLocalizedAutomationName(string displayValue, string unitName, string format)
        {
            return LocalizationStringUtil.GetLocalizedString(format, displayValue, unitName);
        }

        internal string GetLocalizedConversionResultStringFormat(string fromValue, string fromUnit, string toValue, string toUnit)
        {
            if (_localizedConversionResultFormat == null)
            {
                _localizedConversionResultFormat = AppResourceProvider.GetInstance().GetResourceString("Format_ConversionResult");
            }
            return string.Format(_localizedConversionResultFormat, fromValue, fromUnit, toValue, toUnit);
        }

        internal void UpdateValue1AutomationName()
        {
            if (_localizedValueFromFormat == null)
            {
                _localizedValueFromFormat = AppResourceProvider.GetInstance().GetResourceString("Format_ValueFrom");
            }
            Value1AutomationName = GetLocalizedAutomationName(Value1, Unit1?.Name ?? string.Empty, _localizedValueFromFormat);
        }

        internal void UpdateValue2AutomationName()
        {
            if (_localizedValueToFormat == null)
            {
                _localizedValueToFormat = AppResourceProvider.GetInstance().GetResourceString("Format_ValueTo");
            }
            Value2AutomationName = GetLocalizedAutomationName(Value2, Unit2?.Name ?? string.Empty, _localizedValueToFormat);
        }

        #endregion

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            OnPropertyChanged(propertyName);
        }
    }

    /// <summary>
    /// Interface for the unit converter model.
    /// </summary>
    public interface IUnitConverter
    {
        void SwitchCategory(int categoryId);
        void SetCurrentUnitValue(string value);
        void RefreshCurrencyRatios();
        void SendCommand(int command);
    }
}
