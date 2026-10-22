// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Text;
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
        private readonly object _modelLock = new object();
        private readonly DataLoaders.CurrencyDataLoader _currencyDataLoader;
        private readonly Windows.UI.Core.CoreDispatcher _dispatcher;
        private char _decimalSeparator;
        private bool _isInputBlocked;
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
        private List<(string Value, CalcManager.Interop.UnitWrapper Unit)> _cachedSuggestedValues;
        private readonly object _cacheMutex = new object();
        private string _valueFromUnlocalized;
        private string _valueToUnlocalized;
        private string _lastAnnouncedConversionResult;
        private string _lastAnnouncedFrom;
        private string _lastAnnouncedTo;

        // Currency formatters
        private Windows.Globalization.NumberFormatting.CurrencyFormatter _currencyFormatter;
        private Windows.Globalization.NumberFormatting.DecimalFormatter _decimalFormatter;
        private Windows.Globalization.NumberFormatting.CurrencyFormatter _currencyFormatter1;
        private Windows.Globalization.NumberFormatting.CurrencyFormatter _currencyFormatter2;

        // Localized format strings
        private string _localizedValueFromFormat;
        private string _localizedValueToFormat;
        private string _localizedConversionResultFormat;

        private enum ConversionParameter { Source, Target }
        private ConversionParameter _value1cp;

        private Windows.Globalization.NumberFormatting.CurrencyFormatter CurrencyFormatterFrom =>
            _value1cp == ConversionParameter.Source ? _currencyFormatter1 : _currencyFormatter2;

        private Windows.Globalization.NumberFormatting.CurrencyFormatter CurrencyFormatterTo =>
            _value1cp == ConversionParameter.Target ? _currencyFormatter1 : _currencyFormatter2;

        private string ValueFrom
        {
            get => _value1cp == ConversionParameter.Source ? Value1 : Value2;
            set
            {
                if (_value1cp == ConversionParameter.Source)
                {
                    Value1 = value;
                }
                else
                {
                    Value2 = value;
                }
            }
        }

        private string ValueTo
        {
            get => _value1cp == ConversionParameter.Target ? Value1 : Value2;
            set
            {
                if (_value1cp == ConversionParameter.Target)
                {
                    Value1 = value;
                }
                else
                {
                    Value2 = value;
                }
            }
        }

        private Unit UnitFrom
        {
            get => _value1cp == ConversionParameter.Source ? Unit1 : Unit2;
            set
            {
                if (_value1cp == ConversionParameter.Source)
                {
                    Unit1 = value;
                }
                else
                {
                    Unit2 = value;
                }
            }
        }

        private Unit UnitTo
        {
            get => _value1cp == ConversionParameter.Target ? Unit1 : Unit2;
            set
            {
                if (_value1cp == ConversionParameter.Target)
                {
                    Unit1 = value;
                }
                else
                {
                    Unit2 = value;
                }
            }
        }

        public UnitConverterViewModel()
        {
            // Create the real native engine via interop
            var dataLoader = new DataLoaders.UnitConverterDataLoader(new Windows.Globalization.GeographicRegion());
            _model = new CalcManager.Interop.UnitConverterWrapper(dataLoader);

            // Create currency data loader
            _currencyDataLoader = new DataLoaders.CurrencyDataLoader();
            _currencyDataLoader.SetViewModelCallback(new CurrencyVMCallback(this));

            // Wire currency data into the unit converter data loader so the native engine
            // can load currency units and ratios via GetOrderedUnits/LoadOrderedRatios
            dataLoader.CurrencyDataLoader = _currencyDataLoader;

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

            _decimalFormatter = LocalizationSettings.GetInstance().GetRegionalSettingsAwareDecimalFormatter();
            _decimalFormatter.FractionDigits = 0;
            _decimalFormatter.IsGrouped = true;

            // Initialize default currency formatter (uses user's currency or USD fallback)
            string userCurrency = Windows.System.UserProfile.GlobalizationPreferences.Currencies.Count > 0
                ? Windows.System.UserProfile.GlobalizationPreferences.Currencies[0]
                : "USD";
            _currencyFormatter = new Windows.Globalization.NumberFormatting.CurrencyFormatter(userCurrency);
            _currencyFormatter.IsGrouped = true;
            _currencyFormatter.Mode = Windows.Globalization.NumberFormatting.CurrencyFormatterMode.UseCurrencyCode;
            _currencyFormatter.ApplyRoundingForCurrency(Windows.Globalization.NumberFormatting.RoundingAlgorithm.RoundHalfDown);

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

        internal bool IsCurrencyDataLoaded => _isCurrencyDataLoaded;

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

        private RelayCommand<object> _categoryChangedCommand;
        private RelayCommand<object> _unitChangedCommand;
        private RelayCommand<object> _switchActiveCommand;
        private RelayCommand<object> _buttonPressedCommand;
        private RelayCommand<object> _copyCommand;
        private RelayCommand<object> _pasteCommand;

        public RelayCommand<object> CategoryChangedCommand =>
            _categoryChangedCommand ?? (_categoryChangedCommand = new RelayCommand<object>(OnCategoryChanged));
        public RelayCommand<object> UnitChangedCommand =>
            _unitChangedCommand ?? (_unitChangedCommand = new RelayCommand<object>(OnUnitChanged));
        public RelayCommand<object> SwitchActiveCommand =>
            _switchActiveCommand ?? (_switchActiveCommand = new RelayCommand<object>(OnSwitchActive));
        public RelayCommand<object> ButtonPressedCommand =>
            _buttonPressedCommand ?? (_buttonPressedCommand = new RelayCommand<object>(OnButtonPressed));
        public RelayCommand<object> ButtonPressed => ButtonPressedCommand;
        public RelayCommand<object> CopyCommand =>
            _copyCommand ?? (_copyCommand = new RelayCommand<object>(OnCopyCommand));
        public RelayCommand<object> PasteCommand =>
            _pasteCommand ?? (_pasteCommand = new RelayCommand<object>(OnPasteCommand));

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
            if (string.IsNullOrEmpty(stringToPaste) || CopyPasteManager.IsErrorMessage(stringToPaste))
            {
                DisplayPasteError();
                return;
            }

            bool isFirstLegalChar = true;
            bool sendNegate = false;
            var accumulation = new StringBuilder();

            lock (_modelLock)
            {
                foreach (char ch in stringToPaste)
                {
                    bool canSendNegate;
                    var buttonId = MapCharacterToButtonId(ch, out canSendNegate);

                    if (buttonId == NumbersAndOperatorsEnum.None)
                    {
                        sendNegate = false;
                        continue;
                    }

                    if (isFirstLegalChar)
                    {
                        // Send Clear before sending anything that will actually apply to the field.
                        _model.SendCommand(CalcManager.Interop.UnitConverterCommand.Clear);
                        isFirstLegalChar = false;

                        // A leading minus is a sign, but it has to follow the digit it applies to or
                        // the engine ignores it, so remember it rather than sending it now.
                        if (buttonId == NumbersAndOperatorsEnum.Negate)
                        {
                            sendNegate = true;
                        }
                    }

                    if (buttonId != NumbersAndOperatorsEnum.Negate)
                    {
                        _model.SendCommand(CommandFromButtonId(buttonId));

                        if (sendNegate)
                        {
                            if (canSendNegate)
                            {
                                _model.SendCommand(CalcManager.Interop.UnitConverterCommand.Negate);
                            }
                            sendNegate = false;
                        }
                    }

                    accumulation.Append(ch);
                    UpdateInputBlocked(accumulation.ToString());
                    if (_isInputBlocked)
                    {
                        break;
                    }
                }
            }

            if (isFirstLegalChar)
            {
                // No legal characters found — show paste error
                DisplayPasteError();
            }
        }

        public async System.Threading.Tasks.Task RefreshCurrencyRatiosAsync()
        {
            bool finished = false;
            try
            {
                _isCurrencyDataLoaded = false;
                CurrencyDataLoadFailed = false;
                IsCurrencyLoadingVisible = true;

                string announcement = AppResourceProvider.GetInstance().GetResourceString("UpdatingCurrencyRates");
                Announcement = CalculatorAnnouncement.GetUpdateCurrencyRatesAnnouncement(announcement);

                bool didLoad = await _currencyDataLoader.TryLoadDataFromWebOverrideAsync();
                finished = true;
                OnCurrencyDataLoadFinished(didLoad);
            }
            finally
            {
                if (!finished)
                {
                    OnCurrencyDataLoadFinished(false);
                }
            }
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
            lock (_modelLock)
            {
                _model.SendCommand(CalcManager.Interop.UnitConverterCommand.Reset);
            }
            OnCategoryChanged(null);
        }

        internal void PopulateData()
        {
            lock (_modelLock)
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
                canSendNegate = true;
                return NumbersAndOperatorsEnum.Decimal;
            }

            if (ch == '-')
            {
                return NumbersAndOperatorsEnum.Negate;
            }

            var localization = LocalizationSettings.GetInstance();
            if (localization.IsLocalizedDigit(ch))
            {
                canSendNegate = true;
                return NumbersAndOperatorsEnum.Zero + (ch - localization.GetDigitSymbolFromEnUsDigit('0'));
            }

            return NumbersAndOperatorsEnum.None;
        }

        internal void DisplayPasteError()
        {
            // The native code reads this from the engine's own string table, not from the app
            // resources (UnitConverterViewModel.cpp:428), and StandardCalculatorViewModel does the
            // same at DisplayPasteError. There is no "InvalidInput" app resource, so asking for one
            // returned nothing and a rejected paste blanked the display instead of explaining itself.
            const int IDS_ERRORS_FIRST = 99;
            const int IDS_DOMAIN = IDS_ERRORS_FIRST + 1;
            string errorMsg = AppResourceProvider.GetInstance().GetCEngineString(IDS_DOMAIN.ToString());
            Value1 = errorMsg;
            Value2 = errorMsg;
        }

        internal void UpdateDisplay(string from, string to)
        {
            _valueFromUnlocalized = from;
            _valueToUnlocalized = to;

            ValueFrom = ConvertToLocalizedString(from, true, CurrencyFormatterFrom);
            UpdateInputBlocked(from);
            ValueTo = ConvertToLocalizedString(to, true, CurrencyFormatterTo);

            UpdateValue1AutomationName();
            UpdateValue2AutomationName();
        }

        internal void UpdateSupplementaryResults(
            List<(string Value, CalcManager.Interop.UnitWrapper Unit)> suggestedValues)
        {
            lock (_cacheMutex)
            {
                _cachedSuggestedValues = suggestedValues;
            }
            RefreshSupplementaryResults();
        }

        internal void OnMaxDigitsReached()
        {
            string format = AppResourceProvider.GetInstance().GetResourceString("Format_MaxDigitsReached");
            string announcement = LocalizationStringUtil.GetLocalizedString(
                format, _lastAnnouncedConversionResult ?? string.Empty);
            Announcement = CalculatorAnnouncement.GetMaxDigitsReachedAnnouncement(announcement);
        }

        internal void SaveUserPreferences()
        {
            if (!UnitsAreValid())
                return;

            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            if (!IsCurrencyCurrentCategory)
            {
                string userPreferences;
                lock (_modelLock)
                {
                    userPreferences = _model.SaveUserPreferences();
                }
                localSettings.Values["UnitConverterPreferences"] = userPreferences;
            }
            else if (!string.IsNullOrEmpty(UnitFrom?.Abbreviation)
                && !string.IsNullOrEmpty(UnitTo?.Abbreviation))
            {
                localSettings.Values[DataLoaders.UnitConverterResourceKeys.CurrencyUnitFromKey] =
                    UnitFrom.Abbreviation;
                localSettings.Values[DataLoaders.UnitConverterResourceKeys.CurrencyUnitToKey] =
                    UnitTo.Abbreviation;
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
                    lock (_modelLock)
                    {
                        _model.RestoreUserPreferences(userPreferences);
                    }
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
            ResetCategory();
        }

        private void OnUnitChanged(object unused)
        {
            if (UnitFrom == null || UnitTo == null)
                return;

            UpdateCurrencyFormatter();

            if (IsCurrencyCurrentCategory)
            {
                // Update currency symbols
                var symbols = _currencyDataLoader.GetCurrencySymbols(
                    UnitFrom.ModelUnitID(), UnitTo.ModelUnitID());
                CurrencySymbol1 = _value1cp == ConversionParameter.Source
                    ? symbols.Symbol1
                    : symbols.Symbol2;
                CurrencySymbol2 = _value1cp == ConversionParameter.Source
                    ? symbols.Symbol2
                    : symbols.Symbol1;

                // Update ratio display
                var ratios = _currencyDataLoader.GetCurrencyRatioEquality(
                    UnitFrom.ModelUnitID(), UnitTo.ModelUnitID());
                CurrencyRatioEquality = ratios.Ratio1;
                CurrencyRatioEqualityAutomationName = ratios.Ratio2;
            }
            else
            {
                CurrencySymbol1 = string.Empty;
                CurrencySymbol2 = string.Empty;
                CurrencyRatioEquality = string.Empty;
                CurrencyRatioEqualityAutomationName = string.Empty;
            }

            // Always tell the native engine the current unit types (matches C++ behavior)
            lock (_modelLock)
            {
                _model.SetCurrentUnitTypes(
                    new CalcManager.Interop.UnitWrapper { Id = UnitFrom.ModelUnitID(), Name = UnitFrom.Name, Abbreviation = UnitFrom.Abbreviation, AccessibleName = UnitFrom.AccessibleName },
                    new CalcManager.Interop.UnitWrapper { Id = UnitTo.ModelUnitID(), Name = UnitTo.Name, Abbreviation = UnitTo.Abbreviation, AccessibleName = UnitTo.AccessibleName });
            }

            SaveUserPreferences();
        }

        private void OnSwitchActive(object unused)
        {
            // Switch conversion parameter mapping
            _value1cp = _value1cp == ConversionParameter.Source ? ConversionParameter.Target : ConversionParameter.Source;

            // The active side follows the source, and the side being turned off goes first.
            // Toggling both flags instead leaves them momentarily both true, which re-enters this
            // method through the property side effect below and undoes the switch.
            if (_value1cp == ConversionParameter.Source)
            {
                Value2Active = false;
                Value1Active = true;
            }
            else
            {
                Value1Active = false;
                Value2Active = true;
            }

            // Swap the unlocalized values
            var temp = _valueFromUnlocalized;
            _valueFromUnlocalized = _valueToUnlocalized;
            _valueToUnlocalized = temp;

            // Swap automation names
            var tempAutoName = Unit1AutomationName;
            Unit1AutomationName = Unit2AutomationName;
            Unit2AutomationName = tempAutoName;

            // The from/to formats follow the values, or Narrator would label the fields the wrong
            // way round once the active value has moved.
            EnsureValueFormatsLoaded();
            var tempFormat = _localizedValueFromFormat;
            _localizedValueFromFormat = _localizedValueToFormat;
            _localizedValueToFormat = tempFormat;

            _isInputBlocked = false;
            lock (_modelLock)
            {
                _model.SwitchActive(_valueFromUnlocalized ?? "0");
            }

            UpdateIsDecimalEnabled();
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

            lock (_modelLock)
            {
                // Block input if max decimal digits reached (except for clear/backspace)
                if (_isInputBlocked && !_model.IsSwitchedActive
                    && command != CalcManager.Interop.UnitConverterCommand.Clear
                    && command != CalcManager.Interop.UnitConverterCommand.Backspace)
                {
                    return;
                }

                _model.SendCommand(command);
            }
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
                    foreach (var (Value, ModelUnit) in _cachedSuggestedValues)
                    {
                        var unit = CreateUnit(ModelUnit);
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

                if (whimsicals.Count > 0)
                {
                    SupplementaryResults.Add(whimsicals[0]);
                }
            }
            OnPropertyChanged(nameof(SupplementaryVisibility));
        }

        private void UpdateInputBlocked(string currencyInput)
        {
            // currencyInput is in en-US and has the default decimal separator
            _isInputBlocked = false;
            var posOfDecimal = currencyInput.IndexOf('.');
            if (posOfDecimal >= 0 && IsCurrencyCurrentCategory)
            {
                var formatter = CurrencyFormatterFrom;
                if (formatter != null)
                {
                    _isInputBlocked = (posOfDecimal + formatter.FractionDigits + 1 == currencyInput.Length);
                }
            }
        }

        private void UpdateCurrencyFormatter()
        {
            if (!IsCurrencyCurrentCategory || Unit1 == null || Unit2 == null
                || string.IsNullOrEmpty(Unit1.Abbreviation) || string.IsNullOrEmpty(Unit2.Abbreviation))
                return;

            _currencyFormatter1 = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Unit1.Abbreviation);
            _currencyFormatter1.IsGrouped = true;
            _currencyFormatter1.Mode = Windows.Globalization.NumberFormatting.CurrencyFormatterMode.UseCurrencyCode;
            _currencyFormatter1.ApplyRoundingForCurrency(Windows.Globalization.NumberFormatting.RoundingAlgorithm.RoundHalfDown);

            _currencyFormatter2 = new Windows.Globalization.NumberFormatting.CurrencyFormatter(Unit2.Abbreviation);
            _currencyFormatter2.IsGrouped = true;
            _currencyFormatter2.Mode = Windows.Globalization.NumberFormatting.CurrencyFormatterMode.UseCurrencyCode;
            _currencyFormatter2.ApplyRoundingForCurrency(Windows.Globalization.NumberFormatting.RoundingAlgorithm.RoundHalfDown);

            UpdateIsDecimalEnabled();

            if (TryPrepareCurrencyInputForPaste(
                _valueFromUnlocalized,
                CurrencyFormatterFrom.FractionDigits,
                _decimalSeparator,
                out string preparedValue))
            {
                OnPaste(preparedValue);
            }
        }

        internal static bool TryPrepareCurrencyInputForPaste(
            string value,
            int fractionDigits,
            out string preparedValue)
        {
            return TryPrepareCurrencyInputForPaste(value, fractionDigits, '.', out preparedValue);
        }

        internal static bool TryPrepareCurrencyInputForPaste(
            string value,
            int fractionDigits,
            char decimalSeparator,
            out string preparedValue)
        {
            preparedValue = value;
            if (string.IsNullOrEmpty(value) || value.IndexOf('e') >= 0 || value.IndexOf('E') >= 0)
            {
                return false;
            }

            preparedValue = TruncateFractionDigits(value, fractionDigits);
            if (decimalSeparator != '.')
            {
                preparedValue = preparedValue.Replace('.', decimalSeparator);
            }
            return true;
        }

        private static string TruncateFractionDigits(string n, int digitCount)
        {
            if (string.IsNullOrEmpty(n))
                return n;

            var i = n.IndexOf('.');
            if (i < 0)
                return n;

            if (digitCount == 0)
                return n.Substring(0, i);

            int actualDigitCount = n.Length - i - 1;
            if (actualDigitCount <= digitCount)
                return n;

            return n.Substring(0, n.Length - (actualDigitCount - digitCount));
        }

        private void UpdateIsDecimalEnabled()
        {
            if (!IsCurrencyCurrentCategory)
                return;
            var formatter = CurrencyFormatterFrom;
            if (formatter == null)
                return;
            IsDecimalEnabled = formatter.FractionDigits > 0;
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
            lock (_modelLock)
            {
                if (IsCurrencyCurrentCategory)
                {
                    if (_isCurrencyDataLoaded && !CurrencyDataLoadFailed)
                    {
                        _model.ResetCategoriesAndRatios();
                    }

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
                UnitFrom = FindUnitInList(result.FromUnit);
                UnitTo = FindUnitInList(result.ToUnit);
            }
        }

        private void SetSelectedCurrencyUnits()
        {
            int currencyCatId = NavCategoryStates.Serialize(ViewMode.Currency);
            var currencyUnits = _currencyDataLoader.GetOrderedUnits(currencyCatId);

            var units = new ObservableCollection<Unit>();
            Unit fromUnit = null;
            Unit toUnit = null;
            foreach (var cu in currencyUnits)
            {
                // Match C++ Unit constructor: name = "countryName - currencyName", accessibleName = "countryName currencyName"
                var nameValue1 = cu.IsRtlLanguage ? cu.Name : cu.CountryName;
                var nameValue2 = cu.IsRtlLanguage ? cu.CountryName : cu.Name;
                var displayName = nameValue1 + " - " + nameValue2;
                var accessibleName = nameValue1 + " " + nameValue2;

                var unit = new Unit(cu.Id, displayName, cu.Abbreviation, accessibleName, false);
                units.Add(unit);

                if (cu.IsConversionSource) fromUnit = unit;
                if (cu.IsConversionTarget) toUnit = unit;
            }

            if (units.Count == 0)
            {
                units.Add(new Unit(-1, "", "", "", false));
            }

            // Publish a complete source before selected units so ComboBox never resolves them
            // against the previous category's items.
            Units = units;
            UnitFrom = fromUnit ?? (Units.Count > 0 ? Units[0] : null);
            UnitTo = toUnit ?? (Units.Count > 1 ? Units[1] : Units.Count > 0 ? Units[0] : null);
        }

        private void BuildUnitList(CalcManager.Interop.UnitWrapper[] modelUnits)
        {
            var units = new ObservableCollection<Unit>();
            foreach (var u in modelUnits)
            {
                if (!u.IsWhimsical)
                {
                    units.Add(CreateUnit(u));
                }
            }

            if (units.Count == 0)
            {
                units.Add(new Unit(-1, "", "", "", false));
            }

            // Replacing the collection makes the ItemsSource update atomic for the old ComboBox UI.
            Units = units;
        }

        private static Unit CreateUnit(CalcManager.Interop.UnitWrapper unit)
        {
            return new Unit(
                unit.Id,
                unit.Name,
                unit.Abbreviation,
                unit.AccessibleName,
                unit.IsWhimsical);
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
            return ConvertToLocalizedString(stringToLocalize, false, _currencyFormatter);
        }

        private string ConvertToLocalizedString(string stringToLocalize, bool allowPartialStrings, Windows.Globalization.NumberFormatting.CurrencyFormatter currencyFormatter)
        {
            if (string.IsNullOrEmpty(stringToLocalize))
            {
                return "0";
            }

            // If unit hasn't been set, formatter1/2 is null. Fallback to default.
            if (currencyFormatter == null)
            {
                currencyFormatter = _currencyFormatter;
            }

            if (currencyFormatter == null)
            {
                // No formatter available at all — just localize characters
                LocalizationSettings.GetInstance().LocalizeDisplayValue(ref stringToLocalize);
                return stringToLocalize;
            }

            int lastCurrencyFractionDigits = currencyFormatter.FractionDigits;
            bool lastIsDecimalPointAlwaysDisplayed = currencyFormatter.IsDecimalPointAlwaysDisplayed;

            _decimalFormatter.IsDecimalPointAlwaysDisplayed = false;
            _decimalFormatter.FractionDigits = 0;
            currencyFormatter.IsDecimalPointAlwaysDisplayed = false;
            currencyFormatter.FractionDigits = 0;

            string result;

            try
            {
                if (!double.TryParse(
                        stringToLocalize,
                        System.Globalization.NumberStyles.Float,
                        System.Globalization.CultureInfo.InvariantCulture,
                        out double parsedValue))
                {
                    LocalizationSettings.GetInstance().LocalizeDisplayValue(ref stringToLocalize);
                    return stringToLocalize;
                }

                // Handle scientific notation
                int posOfE = stringToLocalize.IndexOf('e');
                if (posOfE >= 0)
                {
                    int posOfSign = posOfE + 1;
                    char signOfE = stringToLocalize[posOfSign];
                    string significandStr = stringToLocalize.Substring(0, posOfE);
                    string exponentStr = stringToLocalize.Substring(posOfSign + 1);

                    result = ConvertToLocalizedString(significandStr, allowPartialStrings, currencyFormatter)
                        + "e" + signOfE
                        + ConvertToLocalizedString(exponentStr, allowPartialStrings, currencyFormatter);
                }
                else
                {
                    int posOfDecimal = stringToLocalize.IndexOf('.');
                    bool hasDecimal = posOfDecimal >= 0;

                    if (hasDecimal)
                    {
                        if (allowPartialStrings && lastCurrencyFractionDigits > 0)
                        {
                            // Allow "in progress" strings such as "3." that occur while a number is
                            // being composed, so the separator appears as soon as it is typed.
                            _decimalFormatter.IsDecimalPointAlwaysDisplayed = true;
                            currencyFormatter.IsDecimalPointAlwaysDisplayed = true;
                        }

                        // Force post-decimal digits so trailing zeroes aren't cut off
                        _decimalFormatter.FractionDigits = stringToLocalize.Length - (posOfDecimal + 1);
                        currencyFormatter.FractionDigits = lastCurrencyFractionDigits;
                    }

                    if (IsCurrencyCurrentCategory)
                    {
                        string currencyResult = currencyFormatter.Format(parsedValue);
                        string currencyCode = currencyFormatter.Currency;

                        // CurrencyFormatter always includes LangCode or Symbol. Remove the currency code.
                        int pos = currencyResult.IndexOf(currencyCode);
                        if (pos >= 0)
                        {
                            currencyResult = currencyResult.Remove(pos, currencyCode.Length);
                            // Trim any leading/trailing spaces (including non-breaking spaces)
                            currencyResult = currencyResult.Trim(' ', '\u00A0', '\u202F');
                        }

                        result = currencyResult;
                    }
                    else
                    {
                        result = _decimalFormatter.Format(parsedValue);
                    }

                    if (hasDecimal)
                    {
                        // GetLocaleInfoEx and DecimalFormatter disagree on the decimal separator for
                        // some locales, and the rest of the view model keys off the former, so bring
                        // the formatted result back in line with it.
                        string formattedSample = _decimalFormatter.Format(1.1);
                        int sepPos = result.IndexOf(formattedSample[1]);
                        if (sepPos >= 0)
                        {
                            result = result.Remove(sepPos, 1).Insert(sepPos, _decimalSeparator.ToString());
                        }
                    }
                }

                // A value that formats to zero still has to keep a leading minus, and some locales
                // put the sign at the end, which the display cannot use.
                if ((stringToLocalize[0] == '-' && parsedValue == 0)
                    || (result.Length > 0 && result[result.Length - 1] == '-'))
                {
                    if (result.Length > 0 && result[result.Length - 1] == '-')
                    {
                        result = result.Substring(0, result.Length - 1);
                    }
                    result = "-" + result;
                }
            }
            finally
            {
                // Restore formatter state
                currencyFormatter.FractionDigits = lastCurrencyFractionDigits;
                currencyFormatter.IsDecimalPointAlwaysDisplayed = lastIsDecimalPointAlwaysDisplayed;
            }

            return result;
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
            return LocalizationStringUtil.GetLocalizedString(
                _localizedConversionResultFormat,
                fromValue,
                fromUnit,
                toValue,
                toUnit);
        }

        internal void UpdateValue1AutomationName()
        {
            EnsureValueFormatsLoaded();
            Value1AutomationName = GetLocalizedAutomationName(
                Value1, Unit1?.AccessibleName ?? string.Empty, _localizedValueFromFormat);
        }

        internal void UpdateValue2AutomationName()
        {
            EnsureValueFormatsLoaded();
            Value2AutomationName = GetLocalizedAutomationName(
                Value2, Unit2?.AccessibleName ?? string.Empty, _localizedValueToFormat);
        }

        private void EnsureValueFormatsLoaded()
        {
            if (_localizedValueFromFormat == null || _localizedValueToFormat == null)
            {
                var resourceProvider = AppResourceProvider.GetInstance();
                _localizedValueFromFormat = resourceProvider.GetResourceString("Format_ValueFrom");
                _localizedValueToFormat = resourceProvider.GetResourceString("Format_ValueTo");
            }
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
                lock (_modelLock)
                {
                    action();
                }
            }
        }

        private sealed class UnitConverterVMCallback : CalcManager.Interop.UnitConverterVMCallbackBase
        {
            // Held weakly on purpose. The native UnitConverterWrapper keeps m_vmCallbackBridge,
            // and that bridge stores its receiver by value (UnitConverterInterop.h:80), which is a
            // strong COM reference to this object. A strong _vm here would close the loop
            // VM -> _model -> bridge -> callback -> VM across the interop boundary, and neither the
            // collector nor COM reference counting can break a cycle that spans it. The view model
            // is owned by ApplicationViewModel for as long as the converter is in use, so a weak
            // reference is enough for every callback that matters.
            private readonly WeakReference<UnitConverterViewModel> _vm;

            public UnitConverterVMCallback(UnitConverterViewModel vm)
            {
                _vm = new WeakReference<UnitConverterViewModel>(vm);
            }

            private UnitConverterViewModel ViewModel =>
                _vm.TryGetTarget(out UnitConverterViewModel vm) ? vm : null;

            public override void DisplayCallback(string fromValue, string toValue)
            {
                UnitConverterViewModel vm = ViewModel;
                vm?.RunOnUIThread(() => vm.UpdateDisplay(fromValue, toValue));
            }

            public override void SuggestedValueCallback(CalcManager.Interop.SuggestedValueWrapper[] suggestedValues)
            {
                UnitConverterViewModel vm = ViewModel;
                if (vm == null)
                {
                    return;
                }

                var converted = new List<(string Value, CalcManager.Interop.UnitWrapper Unit)>();
                if (suggestedValues != null)
                {
                    foreach (var sv in suggestedValues)
                    {
                        converted.Add((sv.Value, sv.Unit));
                    }
                }
                vm.RunOnUIThread(() => vm.UpdateSupplementaryResults(converted));
            }

            public override void MaxDigitsReached()
            {
                UnitConverterViewModel vm = ViewModel;
                vm?.RunOnUIThread(() => vm.OnMaxDigitsReached());
            }
        }

        private sealed class CurrencyVMCallback : DataLoaders.IViewModelCurrencyCallback
        {
            // Weak for the same reason as UnitConverterVMCallback: the currency loader is reachable
            // from the native data-loader bridge, so a strong reference here would root the view
            // model through the interop boundary too.
            private readonly WeakReference<UnitConverterViewModel> _vm;

            public CurrencyVMCallback(UnitConverterViewModel vm)
            {
                _vm = new WeakReference<UnitConverterViewModel>(vm);
            }

            private UnitConverterViewModel ViewModel =>
                _vm.TryGetTarget(out UnitConverterViewModel vm) ? vm : null;

            public void CurrencyDataLoadFinished(bool didLoad)
            {
                UnitConverterViewModel vm = ViewModel;
                vm?.RunOnUIThread(() => vm.OnCurrencyDataLoadFinished(didLoad));
            }

            public void CurrencyTimestampUpdated(string timestamp, bool isWeekOld)
            {
                UnitConverterViewModel vm = ViewModel;
                vm?.RunOnUIThread(() => vm.OnCurrencyTimestampUpdated(timestamp, isWeekOld));
            }

            public void NetworkBehaviorChanged(NetworkAccessBehavior newBehavior)
            {
                UnitConverterViewModel vm = ViewModel;
                vm?.RunOnUIThread(() => vm.HandleNetworkBehaviorChanged(newBehavior));
            }
        }

        #endregion

        #region Currency Support

        internal void OnCurrencyDataLoadFinished(bool didLoad)
        {
            _isCurrencyDataLoaded = true;
            try
            {
                if (didLoad && IsCurrencyCurrentCategory)
                {
                    lock (_modelLock)
                    {
                        _model.Calculate();
                        ResetCategory();
                    }
                }
            }
            finally
            {
                try
                {
                    IsCurrencyLoadingVisible = false;
                }
                finally
                {
                    CurrencyDataLoadFailed = !didLoad;
                }
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
