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
        private readonly CalcManager.Interop.UnitConverterWrapper _model;
        private readonly DataLoaders.CurrencyDataLoader _currencyDataLoader;
        private readonly Windows.UI.Core.CoreDispatcher _dispatcher;
        private char _decimalSeparator;
#pragma warning disable CS0414 // assigned but value is never used — will be used when currency support is complete
        private bool _isInputBlocked;
#pragma warning restore CS0414
        private bool _isCategoryChanging;
        private bool _isCurrencyDataLoaded;

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
        private string _lastAnnouncedFrom;
        private string _lastAnnouncedTo;

        // Localized format strings
        private string _localizedValueFromFormat;
        private string _localizedValueToFormat;
        private string _localizedConversionResultFormat;

        private enum ConversionParameter { Source, Target }
#pragma warning disable CS0414
        private ConversionParameter _value1cp;
#pragma warning restore CS0414

        public UnitConverterViewModel()
        {
            // Create the real native engine via interop
            var dataLoader = new DataLoaders.UnitConverterDataLoader(new Windows.Globalization.GeographicRegion());
            _model = new CalcManager.Interop.UnitConverterWrapper(dataLoader);

            // Create currency data loader
            _currencyDataLoader = new DataLoaders.CurrencyDataLoader();
            _currencyDataLoader.SetViewModelCallback(new CurrencyVMCallback(this));

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

            // Capture the UI dispatcher for marshaling callbacks from background threads
            _dispatcher = Windows.UI.Core.CoreWindow.GetForCurrentThread()?.Dispatcher;

            // Set up VM callback
            var vmCallback = new UnitConverterVMCallback(this);
            _model.SetViewModelCallback(vmCallback);

            _decimalSeparator = LocalizationSettings.GetInstance().GetDecimalSeparator();

            // Initialize the native engine and populate data
            _model.Initialize();
            PopulateData();

            // Start loading currency data asynchronously
            _currencyDataLoader.LoadData();
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
        public RelayCommand<object> ButtonPressed => ButtonPressedCommand;
        public RelayCommand<object> CopyCommand => new RelayCommand<object>(OnCopyCommand);
        public RelayCommand<object> PasteCommand => new RelayCommand<object>(OnPasteCommand);

        #endregion

        #region Public Methods

        public void AnnounceConversionResult()
        {
            if ((_valueFromUnlocalized != _lastAnnouncedFrom || _valueToUnlocalized != _lastAnnouncedTo)
                && Unit1 != null && Unit2 != null)
            {
                _lastAnnouncedFrom = _valueFromUnlocalized;
                _lastAnnouncedTo = _valueToUnlocalized;

                var unitFrom = Value1Active ? Unit1 : Unit2;
                var unitTo = (unitFrom == Unit1) ? Unit2 : Unit1;
                _lastAnnouncedConversionResult = GetLocalizedConversionResultStringFormat(
                    Value1Active ? Value1 : Value2, unitFrom?.Name ?? string.Empty,
                    Value1Active ? Value2 : Value1, unitTo?.Name ?? string.Empty);

                Announcement = CalculatorAnnouncement.GetDisplayUpdatedAnnouncement(_lastAnnouncedConversionResult);
            }
        }

        public void OnPaste(string stringToPaste)
        {
            if (string.IsNullOrEmpty(stringToPaste))
            {
                return;
            }

            bool sendNegate = false;
            bool isFirstLegalChar = true;

            foreach (char ch in stringToPaste)
            {
                bool canSendNegate;
                var buttonId = MapCharacterToButtonId(ch, out canSendNegate);

                if (buttonId == NumbersAndOperatorsEnum.None)
                {
                    // Invalid character — reset negate tracking
                    sendNegate = false;
                    continue;
                }

                if (buttonId == NumbersAndOperatorsEnum.Negate)
                {
                    sendNegate = true;
                    continue;
                }

                if (isFirstLegalChar)
                {
                    // Clear display on first valid input
                    _model.SendCommand(CalcManager.Interop.UnitConverterCommand.Clear);
                    isFirstLegalChar = false;
                }

                _model.SendCommand(CommandFromButtonId(buttonId));

                if (sendNegate && canSendNegate)
                {
                    _model.SendCommand(CalcManager.Interop.UnitConverterCommand.Negate);
                    sendNegate = false;
                }
            }

            if (isFirstLegalChar)
            {
                // No legal characters found — show paste error
                DisplayPasteError();
            }
        }

        public void RefreshCurrencyRatios()
        {
            _isCurrencyDataLoaded = false;
            IsCurrencyLoadingVisible = true;

            string announcement = AppResourceProvider.GetInstance().GetResourceString("UpdatingCurrencyRates");
            Announcement = CalculatorAnnouncement.GetUpdateCurrencyRatesAnnouncement(announcement);

            _currencyDataLoader.LoadData();
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
            CopyPasteManager.CopyToClipboard(_valueFromUnlocalized);
        }

        public void OnPasteCommand(object parameter)
        {
            if (!CopyPasteManager.HasStringToPaste())
            {
                return;
            }

            _ = PasteAsync();
        }

        private async System.Threading.Tasks.Task PasteAsync()
        {
            string pastedString = await CopyPasteManager.GetStringToPaste(Mode, NavCategoryStates.GetGroupType(Mode), NumberBase.Unknown, BitLength.BitLengthUnknown);
            OnPaste(pastedString);
        }

        #endregion

        #region Internal Methods

        internal void ResetView()
        {
            _model.SendCommand(CalcManager.Interop.UnitConverterCommand.Reset);
            OnCategoryChanged(null);
        }

        internal void PopulateData()
        {
            var categories = _model.GetCategories();
            Categories.Clear();
            foreach (var cat in categories)
            {
                Categories.Add(new Category(cat.Id, cat.Name, cat.SupportsNegative));
            }

            RestoreUserPreferences();

            var currentCat = _model.GetCurrentCategory();
            CurrentCategory = new Category(currentCat.Id, currentCat.Name, currentCat.SupportsNegative);
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
            string errorMsg = AppResourceProvider.GetInstance().GetResourceString("InvalidInput");
            Value1 = errorMsg;
            Value2 = errorMsg;
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
            string announcement = AppResourceProvider.GetInstance().GetResourceString("Format_MaxDigitsReached");
            Announcement = CalculatorAnnouncement.GetMaxDigitsReachedAnnouncement(announcement);
        }

        internal void SaveUserPreferences()
        {
            if (!UnitsAreValid())
                return;

            if (!IsCurrencyCurrentCategory)
            {
                var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
                var userPreferences = _model.SaveUserPreferences();
                localSettings.Values["UnitConverterPreferences"] = userPreferences;
            }
        }

        internal void RestoreUserPreferences()
        {
            if (!IsCurrencyCurrentCategory)
            {
                var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
                if (localSettings.Values.ContainsKey("UnitConverterPreferences"))
                {
                    string userPreferences = (string)localSettings.Values["UnitConverterPreferences"];
                    _model.RestoreUserPreferences(userPreferences);
                }
            }
        }

        internal void HandleNetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
        {
            CurrencyDataLoadFailed = false;
            NetworkBehavior = newBehavior;
        }

        internal string GetValueFromUnlocalized() => _valueFromUnlocalized;
        internal string GetValueToUnlocalized() => _valueToUnlocalized;

        #endregion

        #region Private Methods

        private void InitializeView()
        {
            var categories = _model.GetCategories();
            Categories.Clear();
            foreach (var cat in categories)
            {
                Categories.Add(new Category(cat.Id, cat.Name, cat.SupportsNegative));
            }

            RestoreUserPreferences();

            var currentCat = _model.GetCurrentCategory();
            CurrentCategory = new Category(currentCat.Id, currentCat.Name, currentCat.SupportsNegative);
        }

        protected override void OnPropertyChanged(System.ComponentModel.PropertyChangedEventArgs e)
        {
            base.OnPropertyChanged(e);
            HandlePropertySideEffects(e.PropertyName);
        }

        private void HandlePropertySideEffects(string propertyName)
        {
            if (propertyName == nameof(CurrentCategory))
            {
                _isCategoryChanging = true;
                OnCategoryChanged(null);
                _isCategoryChanging = false;
            }
            else if (propertyName == nameof(Unit1) || propertyName == nameof(Unit2))
            {
                if (!_isCategoryChanging)
                {
                    OnUnitChanged(null);
                }
                if (propertyName == nameof(Unit1))
                    UpdateValue1AutomationName();
                else
                    UpdateValue2AutomationName();
            }
            else if (propertyName == nameof(Value1))
            {
                UpdateValue1AutomationName();
            }
            else if (propertyName == nameof(Value2))
            {
                UpdateValue2AutomationName();
            }
            else if (propertyName == nameof(Value1Active) || propertyName == nameof(Value2Active))
            {
                if (Value1Active && Value2Active)
                {
                    OnSwitchActive(null);
                }
                UpdateValue1AutomationName();
                UpdateValue2AutomationName();
            }
            else if (propertyName == nameof(SupplementaryResults))
            {
                OnPropertyChanged(nameof(SupplementaryVisibility));
            }
            else if (propertyName == nameof(CurrencySymbol1) || propertyName == nameof(CurrencySymbol2))
            {
                OnPropertyChanged(nameof(CurrencySymbolVisibility));
            }
        }

        private void OnCategoryChanged(object unused)
        {
            _model.SendCommand(CalcManager.Interop.UnitConverterCommand.Clear);
            ResetCategory();
        }

        private void OnUnitChanged(object unused)
        {
            if (Unit1 == null || Unit2 == null)
                return;

            if (IsCurrencyCurrentCategory)
            {
                // Update currency symbols
                var symbols = _currencyDataLoader.GetCurrencySymbols(Unit1.ModelUnitID(), Unit2.ModelUnitID());
                CurrencySymbol1 = Value1Active ? symbols.Symbol1 : symbols.Symbol2;
                CurrencySymbol2 = Value1Active ? symbols.Symbol2 : symbols.Symbol1;

                // Update ratio display
                var ratios = _currencyDataLoader.GetCurrencyRatioEquality(Unit1.ModelUnitID(), Unit2.ModelUnitID());
                CurrencyRatioEquality = ratios.Ratio1;
                CurrencyRatioEqualityAutomationName = ratios.Ratio2;
            }
            else
            {
                _model.SetCurrentUnitTypes(
                    new CalcManager.Interop.UnitWrapper { Id = Unit1.ModelUnitID(), Name = Unit1.Name, Abbreviation = Unit1.Abbreviation, AccessibleName = Unit1.AccessibleName },
                    new CalcManager.Interop.UnitWrapper { Id = Unit2.ModelUnitID(), Name = Unit2.Name, Abbreviation = Unit2.Abbreviation, AccessibleName = Unit2.AccessibleName });
            }

            SaveUserPreferences();
        }

        private void OnSwitchActive(object unused)
        {
            Value1Active = !Value1Active;
            Value2Active = !Value2Active;

            // Swap the unlocalized values
            var temp = _valueFromUnlocalized;
            _valueFromUnlocalized = _valueToUnlocalized;
            _valueToUnlocalized = temp;

            // Swap automation names
            var tempAutoName = Unit1AutomationName;
            Unit1AutomationName = Unit2AutomationName;
            Unit2AutomationName = tempAutoName;

            _isInputBlocked = false;
            _model.SwitchActive(_valueFromUnlocalized ?? "0");
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

            CalcManager.Interop.UnitConverterCommand command = CommandFromButtonId(numOp);

            if (command == CalcManager.Interop.UnitConverterCommand.Clear && IsDropDownOpen)
                return;

            _model.SendCommand(command);
        }

        private static CalcManager.Interop.UnitConverterCommand CommandFromButtonId(NumbersAndOperatorsEnum button)
        {
            switch (button)
            {
                case NumbersAndOperatorsEnum.Zero: return CalcManager.Interop.UnitConverterCommand.Zero;
                case NumbersAndOperatorsEnum.One: return CalcManager.Interop.UnitConverterCommand.One;
                case NumbersAndOperatorsEnum.Two: return CalcManager.Interop.UnitConverterCommand.Two;
                case NumbersAndOperatorsEnum.Three: return CalcManager.Interop.UnitConverterCommand.Three;
                case NumbersAndOperatorsEnum.Four: return CalcManager.Interop.UnitConverterCommand.Four;
                case NumbersAndOperatorsEnum.Five: return CalcManager.Interop.UnitConverterCommand.Five;
                case NumbersAndOperatorsEnum.Six: return CalcManager.Interop.UnitConverterCommand.Six;
                case NumbersAndOperatorsEnum.Seven: return CalcManager.Interop.UnitConverterCommand.Seven;
                case NumbersAndOperatorsEnum.Eight: return CalcManager.Interop.UnitConverterCommand.Eight;
                case NumbersAndOperatorsEnum.Nine: return CalcManager.Interop.UnitConverterCommand.Nine;
                case NumbersAndOperatorsEnum.Decimal: return CalcManager.Interop.UnitConverterCommand.Decimal;
                case NumbersAndOperatorsEnum.Negate: return CalcManager.Interop.UnitConverterCommand.Negate;
                case NumbersAndOperatorsEnum.Backspace: return CalcManager.Interop.UnitConverterCommand.Backspace;
                case NumbersAndOperatorsEnum.Clear: return CalcManager.Interop.UnitConverterCommand.Clear;
                default: return CalcManager.Interop.UnitConverterCommand.None;
            }
        }

        private void RefreshSupplementaryResults()
        {
            lock (_cacheMutex)
            {
                SupplementaryResults.Clear();
                var whimsicals = new List<SupplementaryResult>();

                if (_cachedSuggestedValues != null)
                {
                    foreach (var (Value, UnitId) in _cachedSuggestedValues)
                    {
                        var unit = FindUnitInList(new CalcManager.Interop.UnitWrapper { Id = UnitId });
                        if (unit != null)
                        {
                            var result = new SupplementaryResult(ConvertToLocalizedString(Value), unit);
                            if (unit.IsModelUnitWhimsical())
                            {
                                whimsicals.Add(result);
                            }
                            else
                            {
                                SupplementaryResults.Add(result);
                            }
                        }
                    }
                }

                if (whimsicals.Count > 0)
                {
                    SupplementaryResults.Add(whimsicals[0]);
                }
            }
            OnPropertyChanged(nameof(SupplementaryVisibility));
        }

        private void UpdateInputBlocked(string currencyInput)
        {
            // Currency input blocking — limit decimal places for currency
            _isInputBlocked = false;
        }

        private void UpdateCurrencyFormatter()
        {
            // Currency formatters only apply when in Currency mode
            if (!IsCurrencyCurrentCategory)
                return;
            UpdateIsDecimalEnabled();
        }

        private void UpdateIsDecimalEnabled()
        {
            // Decimal is always enabled for non-currency categories
            if (!IsCurrencyCurrentCategory)
                return;
            IsDecimalEnabled = true;
        }

        private bool UnitsAreValid()
        {
            return Unit1 != null && Unit2 != null;
        }

        private void ResetCategory()
        {
            _isInputBlocked = false;
            SetSelectedUnits();

            IsCurrencyLoadingVisible = IsCurrencyCurrentCategory && !_isCurrencyDataLoaded;
            IsDropDownEnabled = Units.Count > 0 && Units[0].ModelUnitID() != -1;

            OnUnitChanged(null);
        }

        private void SetSelectedUnits()
        {
            if (IsCurrencyCurrentCategory)
            {
                SetSelectedCurrencyUnits();
                return;
            }

            var result = _model.SetCurrentCategory(
                new CalcManager.Interop.CategoryWrapper
                {
                    Id = CurrentCategory.GetModelCategoryId(),
                    Name = CurrentCategory.Name,
                    SupportsNegative = CurrentCategory.NegateVisibility == Windows.UI.Xaml.Visibility.Visible
                });

            BuildUnitList(result.Units);
            Unit1 = FindUnitInList(result.FromUnit);
            Unit2 = FindUnitInList(result.ToUnit);
        }

        private void SetSelectedCurrencyUnits()
        {
            int currencyCatId = NavCategoryStates.Serialize(ViewMode.Currency);
            var currencyUnits = _currencyDataLoader.GetOrderedUnits(currencyCatId);

            Units.Clear();
            Unit fromUnit = null;
            Unit toUnit = null;
            foreach (var cu in currencyUnits)
            {
                var unit = new Unit(cu.Id, cu.Name, cu.Abbreviation, cu.Name, false);
                Units.Add(unit);

                if (cu.IsConversionSource) fromUnit = unit;
                if (cu.IsConversionTarget) toUnit = unit;
            }

            if (Units.Count == 0)
            {
                Units.Add(new Unit(-1, "", "", "", false));
            }

            Unit1 = fromUnit ?? (Units.Count > 0 ? Units[0] : null);
            Unit2 = toUnit ?? (Units.Count > 1 ? Units[1] : Units.Count > 0 ? Units[0] : null);
        }

        private void BuildUnitList(CalcManager.Interop.UnitWrapper[] modelUnits)
        {
            Units.Clear();
            foreach (var u in modelUnits)
            {
                if (!u.IsWhimsical)
                {
                    Units.Add(new Unit(u.Id, u.Name, u.Abbreviation, u.AccessibleName, u.IsWhimsical));
                }
            }

            if (Units.Count == 0)
            {
                Units.Add(new Unit(-1, "", "", "", false));
            }
        }

        private Unit FindUnitInList(CalcManager.Interop.UnitWrapper target)
        {
            foreach (var unit in Units)
            {
                if (unit.ModelUnitID() == target.Id)
                    return unit;
            }
            return Units.Count > 0 ? Units[0] : new Unit(-1, "", "", "", false);
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

        #region Callback

        private void RunOnUIThread(Windows.UI.Core.DispatchedHandler action)
        {
            if (_dispatcher != null && !_dispatcher.HasThreadAccess)
            {
                _ = _dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, action);
            }
            else
            {
                action();
            }
        }

        private sealed class UnitConverterVMCallback : CalcManager.Interop.UnitConverterVMCallbackBase
        {
            private readonly UnitConverterViewModel _vm;

            public UnitConverterVMCallback(UnitConverterViewModel vm) { _vm = vm; }

            protected override void DisplayCallback(string fromValue, string toValue)
            {
                _vm.RunOnUIThread(() => _vm.UpdateDisplay(fromValue, toValue));
            }

            protected override void SuggestedValueCallback(CalcManager.Interop.SuggestedValueWrapper[] suggestedValues)
            {
                var converted = new List<(string Value, int UnitId)>();
                if (suggestedValues != null)
                {
                    foreach (var sv in suggestedValues)
                    {
                        converted.Add((sv.Value, sv.Unit.Id));
                    }
                }
                _vm.RunOnUIThread(() => _vm.UpdateSupplementaryResults(converted));
            }

            protected override void MaxDigitsReached()
            {
                _vm.RunOnUIThread(() => _vm.OnMaxDigitsReached());
            }
        }

        private sealed class CurrencyVMCallback : DataLoaders.IViewModelCurrencyCallback
        {
            private readonly UnitConverterViewModel _vm;

            public CurrencyVMCallback(UnitConverterViewModel vm) { _vm = vm; }

            public void CurrencyDataLoadFinished(bool didLoad)
            {
                _vm.RunOnUIThread(() => _vm.OnCurrencyDataLoadFinished(didLoad));
            }

            public void CurrencyTimestampUpdated(string timestamp, bool isWeekOld)
            {
                _vm.RunOnUIThread(() => _vm.OnCurrencyTimestampUpdated(timestamp, isWeekOld));
            }

            public void NetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
            {
                _vm.RunOnUIThread(() => _vm.HandleNetworkBehaviorChanged(newBehavior));
            }
        }

        #endregion

        #region Currency Support

        internal void OnCurrencyDataLoadFinished(bool didLoad)
        {
            _isCurrencyDataLoaded = true;
            CurrencyDataLoadFailed = !didLoad;
            IsCurrencyLoadingVisible = false;

            if (didLoad && IsCurrencyCurrentCategory)
            {
                ResetCategory();
            }

            string key = didLoad ? "CurrencyRatesUpdated" : "CurrencyRatesUpdateFailed";
            string announcement = AppResourceProvider.GetInstance().GetResourceString(key);
            Announcement = CalculatorAnnouncement.GetUpdateCurrencyRatesAnnouncement(announcement);
        }

        internal void OnCurrencyTimestampUpdated(string timestamp, bool isWeekOld)
        {
            CurrencyDataIsWeekOld = isWeekOld;
            CurrencyTimestamp = timestamp;
        }

        #endregion

    }
}
