// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class Category : ObservableObject
    {
        private readonly int _id;
        private readonly string _name;
        private readonly bool _supportsNegative;

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
    public sealed partial class Unit : ObservableObject
    {
        private readonly int _id;
        private readonly string _name;
        private readonly string _accessibleName;
        private readonly string _abbreviation;
        private readonly bool _isWhimsical;

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
    public sealed class SupplementaryResult : ObservableObject
    {
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
            private set => SetProperty(ref _value, value);
        }

        public Unit Unit
        {
            get => _unit;
            private set => SetProperty(ref _unit, value);
        }
    }

    public interface IActivatable
    {
        bool IsActive { get; set; }
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class UnitConverterViewModel : ObservableObject
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

        [ObservableProperty]
        private ViewMode _mode;

        private ObservableCollection<Unit> _units;

        [ObservableProperty]
        private string _currencySymbol1;

        [ObservableProperty]
        private Unit _unit1;

        [ObservableProperty]
        private string _value1;

        [ObservableProperty]
        private string _currencySymbol2;

        [ObservableProperty]
        private Unit _unit2;

        [ObservableProperty]
        private string _value2;

        private ObservableCollection<SupplementaryResult> _supplementaryResults;

        [ObservableProperty]
        private bool _value1Active;

        [ObservableProperty]
        private bool _value2Active;

        [ObservableProperty]
        private string _value1AutomationName = string.Empty;

        [ObservableProperty]
        private string _value2AutomationName = string.Empty;

        [ObservableProperty]
        private string _unit1AutomationName = string.Empty;

        [ObservableProperty]
        private string _unit2AutomationName = string.Empty;

        [ObservableProperty]
        private NarratorAnnouncement _announcement;

        [ObservableProperty]
        private bool _isDecimalEnabled;

        [ObservableProperty]
        private bool _isDropDownOpen;

        [ObservableProperty]
        private bool _isDropDownEnabled;

        [ObservableProperty]
        private bool _isCurrencyLoadingVisible;

        private bool _isCurrencyCurrentCategory;

        [ObservableProperty]
        private string _currencyRatioEquality;

        [ObservableProperty]
        private string _currencyRatioEqualityAutomationName;

        [ObservableProperty]
        private string _currencyTimestamp;

        [ObservableProperty]
        private NetworkAccessBehavior _networkBehavior;

        [ObservableProperty]
        private bool _currencyDataLoadFailed;

        [ObservableProperty]
        private bool _currencyDataIsWeekOld;

        private Category _currentCategory;

        // Internal state
        private List<(string Value, int UnitId)> _cachedSuggestedValues;
        private readonly object _cacheMutex = new object();
        private string _valueFromUnlocalized;
        private string _valueToUnlocalized;
        private string _lastAnnouncedConversionResult;
        private bool _isCurrencyDataLoaded;

        // Localized format strings
        private string _localizedValueFromFormat;
        private string _localizedValueToFormat;
        private string _localizedConversionResultFormat;

        private enum ConversionParameter { Source, Target }
        private ConversionParameter _value1cp;

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
            private set => SetProperty(ref _categories, value);
        }

        public ObservableCollection<Unit> Units
        {
            get => _units;
            private set => SetProperty(ref _units, value);
        }

        public ObservableCollection<SupplementaryResult> SupplementaryResults
        {
            get => _supplementaryResults;
            private set => SetProperty(ref _supplementaryResults, value);
        }

        public bool IsCurrencyCurrentCategory
        {
            get => _isCurrencyCurrentCategory;
            private set => SetProperty(ref _isCurrencyCurrentCategory, value);
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
                    OnPropertyChanged(nameof(CurrentCategory));
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

        public RelayCommand<object> CategoryChangedCommand => new RelayCommand<object>(OnCategoryChanged);
        public RelayCommand<object> UnitChangedCommand => new RelayCommand<object>(OnUnitChanged);
        public RelayCommand<object> SwitchActiveCommand => new RelayCommand<object>(OnSwitchActive);
        public RelayCommand<object> ButtonPressedCommand => new RelayCommand<object>(OnButtonPressed);
        public RelayCommand<object> CopyCommand => new RelayCommand<object>(OnCopyCommand);
        public RelayCommand<object> PasteCommand => new RelayCommand<object>(OnPasteCommand);

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

        internal void HandleNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
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

        protected override void OnPropertyChanged(System.ComponentModel.PropertyChangedEventArgs e)
        {
            base.OnPropertyChanged(e);
            HandlePropertySideEffects(e.PropertyName);
        }

        private void HandlePropertySideEffects(string propertyName)
        {
            if (propertyName == nameof(Mode))
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
