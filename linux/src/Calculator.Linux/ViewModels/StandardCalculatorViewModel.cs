// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Threading.Tasks;
using Calculator.Linux.Common;
using Calculator.Linux.Interop;
using Calculator.Linux.Services;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace Calculator.Linux.ViewModels
{
    public delegate void HideMemoryClickedHandler();

    public struct ButtonInfo
    {
        public NumbersAndOperatorsEnum ButtonId;
        public bool CanSendNegate;
    }

    public sealed partial class StandardCalculatorViewModel : ObservableObject, ICalcDisplayTarget, IDisposable
    {
        public const string BinaryDigitsPropertyName = "BinaryDigits";
        public const string IsBitFlipCheckedPropertyName = "IsBitFlipChecked";
        public const string IsProgrammerPropertyName = "IsProgrammer";
        public const string IsMemoryEmptyPropertyName = "IsMemoryEmpty";
        public const string IsInErrorPropertyName = "IsInError";

        private const int ASCII_0 = 48;
        private const int StandardModePrecision = 16;
        private const int ScientificModePrecision = 32;
        private const int ProgrammerModePrecision = 64;

        // Engine objects
        private readonly CalculatorDisplay _calculatorDisplay;
        private readonly CalcEngine _engine;

        // Localized format strings
        private string _expressionAutomationNameFormat;
        private string _localizedCalculationResultAutomationFormat;
        private string _localizedCalculationResultDecimalAutomationFormat;
        private string _localizedHexaDecimalAutomationFormat;
        private string _localizedDecimalAutomationFormat;
        private string _localizedOctalAutomationFormat;
        private string _localizedBinaryAutomationFormat;
        private string _localizedMaxDigitsReachedAutomationFormat;
        private string _localizedButtonPressFeedbackAutomationFormat;
        private string _localizedMemorySavedAutomationFormat;
        private string _localizedMemoryItemChangedAutomationFormat;
        private string _localizedMemoryItemClearedAutomationFormat;
        private string _localizedMemoryCleared;
        private string _localizedOpenParenthesisCountChangedAutomationFormat;
        private string _localizedNoRightParenthesisAddedFormat;

        // State fields
        private NumbersAndOperatorsEnum _currentAngleType;
        private char _decimalSeparator;
        private bool _isOperandEnabled;
        private bool _isEditingEnabled;
        private bool _isStandard;
        private bool _isScientific;
        private bool _isProgrammer;
        private bool _isBitFlipChecked;
        private bool _isRtlLanguage;

        private bool _isLastOperationHistoryLoad;
        private BitLength _valueBitLength;
        private string _selectedExpressionLastData;
        private DisplayExpressionToken _selectedExpressionToken = null;
        private string _feedbackForButtonPress;

        // Expression data
        private List<(string Token, int CommandIndex, int CommandType)> _tokens;
        private List<int> _commands;

        // Backing fields for observable properties
        [ObservableProperty]
        private string _displayValue;

        private HistoryViewModel _historyVM;

        [ObservableProperty]
        private bool _isAlwaysOnTop;

        private bool _isBinaryBitFlippingEnabled;
        private bool _isInError;
        private bool _isOperatorCommand;
        private ObservableCollection<DisplayExpressionToken> _expressionTokens;
        private string _decimalDisplayValue;
        private string _hexDisplayValue;
        private string _octalDisplayValue;
        private string _binaryDisplayValue;
        private IList<bool> _binaryDigits;
        private string _hexDisplayValue_AutomationName = string.Empty;
        private string _decDisplayValue_AutomationName = string.Empty;
        private string _octDisplayValue_AutomationName = string.Empty;
        private string _binDisplayValue_AutomationName = string.Empty;

        [ObservableProperty]
        private bool _isBinaryOperatorEnabled;

        [ObservableProperty]
        private bool _isUnaryOperatorEnabled;

        [ObservableProperty]
        private bool _isNegateEnabled;

        [ObservableProperty]
        private bool _isDecimalEnabled;

        private ObservableCollection<MemoryItemViewModel> _memorizedNumbers;

        [ObservableProperty]
        private bool _isMemoryEmpty;
        private bool _isFToEChecked;
        private bool _isFToEEnabled;
        private bool _areHEXButtonsEnabled;
        private string _calculationResultAutomationName = string.Empty;
        private string _calculationExpressionAutomationName = string.Empty;
        private bool _isShiftProgrammerChecked;
        private NumberBase _currentRadixType;
        private bool _areTokensUpdated;
        private bool _areAlwaysOnTopResultsUpdated;
        private bool _areProgrammerRadixOperatorsVisible;
        private bool _isInputEmpty;
        private NarratorAnnouncement _announcement;
        private uint _openParenthesisCount;

        // Non-observable properties
        private bool _isOperandUpdatedUsingViewModel;
        private int _tokenPosition;
        private bool _isOperandTextCompletelySelected;
        private bool _keyPressed;

        public event HideMemoryClickedHandler HideMemoryClicked;

        public StandardCalculatorViewModel()
        {
            _displayValue = "0";
            _decimalDisplayValue = "0";
            _hexDisplayValue = "0";
            _binaryDisplayValue = "0";
            _octalDisplayValue = "0";
            _binaryDigits = new List<bool>(new bool[64]);
            _expressionTokens = new ObservableCollection<DisplayExpressionToken>();
            _memorizedNumbers = new ObservableCollection<MemoryItemViewModel>();
            _isMemoryEmpty = true;
            _isFToEChecked = false;
            _isShiftProgrammerChecked = false;
            _valueBitLength = BitLength.BitLengthQWord;
            _isBitFlipChecked = false;
            _isBinaryBitFlippingEnabled = false;
            _currentRadixType = NumberBase.DecBase;
            _currentAngleType = NumbersAndOperatorsEnum.Degree;
            _openParenthesisCount = 0;
            _tokenPosition = -1;
            _isLastOperationHistoryLoad = false;
            _isRtlLanguage = false;
            _tokens = new List<(string, int, int)>();
            _commands = new List<int>();

            _engine = new CalcEngine();
            _calculatorDisplay = new CalculatorDisplay(_engine);
            _calculatorDisplay.SetCallback(this);

            var appResourceProvider = AppResources.Instance;
            _expressionAutomationNameFormat = appResourceProvider.GetResourceString("Format_CalculatorExpression");
            _localizedCalculationResultAutomationFormat = appResourceProvider.GetResourceString("Format_CalculatorResults");
            _localizedCalculationResultDecimalAutomationFormat = appResourceProvider.GetResourceString("Format_CalculatorResults_Decimal");
            _localizedHexaDecimalAutomationFormat = appResourceProvider.GetResourceString("Format_HexButtonValue");
            _localizedDecimalAutomationFormat = appResourceProvider.GetResourceString("Format_DecButtonValue");
            _localizedOctalAutomationFormat = appResourceProvider.GetResourceString("Format_OctButtonValue");
            _localizedBinaryAutomationFormat = appResourceProvider.GetResourceString("Format_BinButtonValue");

            CalculationResultAutomationName = GetLocalizedStringFormat(_localizedCalculationResultAutomationFormat, _displayValue);
            CalculationExpressionAutomationName = GetLocalizedStringFormat(_expressionAutomationNameFormat, string.Empty);

            _historyVM = new HistoryViewModel(_engine);
            _calculatorDisplay.SetHistoryCallback(_historyVM);

            _decimalSeparator = LinuxLocalization.Current.GetDecimalSeparator();
            _isRtlLanguage = LinuxLocalization.Current.IsRightToLeft;

            IsEditingEnabled = false;
            IsUnaryOperatorEnabled = true;
            IsBinaryOperatorEnabled = true;
            IsOperandEnabled = true;
            IsNegateEnabled = true;
            IsDecimalEnabled = true;
            AreProgrammerRadixOperatorsVisible = false;
        }

        public HistoryViewModel HistoryVM
        {
            get => _historyVM;
            private set => SetProperty(ref _historyVM, value);
        }

        public bool IsBinaryBitFlippingEnabled
        {
            get => _isBinaryBitFlippingEnabled;
            private set => SetProperty(ref _isBinaryBitFlippingEnabled, value);
        }

        public bool IsInError
        {
            get => _isInError;
            private set => SetProperty(ref _isInError, value);
        }

        public bool IsOperatorCommand
        {
            get => _isOperatorCommand;
            private set => SetProperty(ref _isOperatorCommand, value);
        }

        public ObservableCollection<DisplayExpressionToken> ExpressionTokens
        {
            get => _expressionTokens;
            private set => SetProperty(ref _expressionTokens, value);
        }

        public string DecimalDisplayValue
        {
            get => _decimalDisplayValue;
            private set => SetProperty(ref _decimalDisplayValue, value);
        }

        public string HexDisplayValue
        {
            get => _hexDisplayValue;
            private set => SetProperty(ref _hexDisplayValue, value);
        }

        public string OctalDisplayValue
        {
            get => _octalDisplayValue;
            private set => SetProperty(ref _octalDisplayValue, value);
        }

        public string BinaryDisplayValue
        {
            get => _binaryDisplayValue;
            private set => SetProperty(ref _binaryDisplayValue, value);
        }

        public IList<bool> BinaryDigits
        {
            get => _binaryDigits;
            private set => SetProperty(ref _binaryDigits, value);
        }

        public string HexDisplayValue_AutomationName
        {
            get => _hexDisplayValue_AutomationName;
            private set => SetProperty(ref _hexDisplayValue_AutomationName, value);
        }

        public string DecDisplayValue_AutomationName
        {
            get => _decDisplayValue_AutomationName;
            private set => SetProperty(ref _decDisplayValue_AutomationName, value);
        }

        public string OctDisplayValue_AutomationName
        {
            get => _octDisplayValue_AutomationName;
            private set => SetProperty(ref _octDisplayValue_AutomationName, value);
        }

        public string BinDisplayValue_AutomationName
        {
            get => _binDisplayValue_AutomationName;
            private set => SetProperty(ref _binDisplayValue_AutomationName, value);
        }

        public ObservableCollection<MemoryItemViewModel> MemorizedNumbers
        {
            get => _memorizedNumbers;
            private set => SetProperty(ref _memorizedNumbers, value);
        }

        public bool IsFToEChecked
        {
            get => _isFToEChecked;
            set => SetProperty(ref _isFToEChecked, value);
        }

        public bool IsFToEEnabled
        {
            get => _isFToEEnabled;
            private set => SetProperty(ref _isFToEEnabled, value);
        }

        public bool AreHEXButtonsEnabled
        {
            get => _areHEXButtonsEnabled;
            private set => SetProperty(ref _areHEXButtonsEnabled, value);
        }

        public string CalculationResultAutomationName
        {
            get => _calculationResultAutomationName;
            private set => SetProperty(ref _calculationResultAutomationName, value);
        }

        public string CalculationExpressionAutomationName
        {
            get => _calculationExpressionAutomationName;
            private set => SetProperty(ref _calculationExpressionAutomationName, value);
        }

        public bool IsShiftProgrammerChecked
        {
            get => _isShiftProgrammerChecked;
            private set => SetProperty(ref _isShiftProgrammerChecked, value);
        }

        public NumberBase CurrentRadixType
        {
            get => _currentRadixType;
            private set => SetProperty(ref _currentRadixType, value);
        }

        public bool AreTokensUpdated
        {
            get => _areTokensUpdated;
            private set => SetProperty(ref _areTokensUpdated, value);
        }

        public bool AreAlwaysOnTopResultsUpdated
        {
            get => _areAlwaysOnTopResultsUpdated;
            private set => SetProperty(ref _areAlwaysOnTopResultsUpdated, value);
        }

        public bool AreProgrammerRadixOperatorsVisible
        {
            get => _areProgrammerRadixOperatorsVisible;
            private set => SetProperty(ref _areProgrammerRadixOperatorsVisible, value);
        }

        public bool IsInputEmpty
        {
            get => _isInputEmpty;
            private set => SetProperty(ref _isInputEmpty, value);
        }

        public NarratorAnnouncement Announcement
        {
            get => _announcement;
            private set => SetProperty(ref _announcement, value);
        }

        public uint OpenParenthesisCount
        {
            get => _openParenthesisCount;
            private set => SetProperty(ref _openParenthesisCount, value);
        }

        public bool IsOperandUpdatedUsingViewModel
        {
            get => _isOperandUpdatedUsingViewModel;
            internal set => _isOperandUpdatedUsingViewModel = value;
        }

        public int TokenPosition
        {
            get => _tokenPosition;
            internal set => _tokenPosition = value;
        }

        public bool IsOperandTextCompletelySelected
        {
            get => _isOperandTextCompletelySelected;
            internal set => _isOperandTextCompletelySelected = value;
        }

        public bool KeyPressed
        {
            get => _keyPressed;
            internal set => _keyPressed = value;
        }

        public string SelectedExpressionLastData
        {
            get => _selectedExpressionLastData;
            internal set => _selectedExpressionLastData = value;
        }

        #region Mode properties

        public bool IsBitFlipChecked
        {
            get => _isBitFlipChecked;
            set
            {
                if (_isBitFlipChecked != value)
                {
                    _isBitFlipChecked = value;
                    IsBinaryBitFlippingEnabled = IsProgrammer && _isBitFlipChecked;
                    AreProgrammerRadixOperatorsVisible = IsProgrammer && !_isBitFlipChecked;
                    OnPropertyChanged(nameof(IsBitFlipChecked));
                }
            }
        }

        public BitLength ValueBitLength
        {
            get => _valueBitLength;
            set
            {
                if (_valueBitLength != value)
                {
                    _valueBitLength = value;
                    OnPropertyChanged(nameof(ValueBitLength));

                    switch (value)
                    {
                        case BitLength.BitLengthQWord:
                            OnButtonPressed(NumbersAndOperatorsEnum.Qword);
                            break;
                        case BitLength.BitLengthDWord:
                            OnButtonPressed(NumbersAndOperatorsEnum.Dword);
                            break;
                        case BitLength.BitLengthWord:
                            OnButtonPressed(NumbersAndOperatorsEnum.Word);
                            break;
                        case BitLength.BitLengthByte:
                            OnButtonPressed(NumbersAndOperatorsEnum.Byte);
                            break;
                    }

                    SetMemorizedNumbersString();
                }
            }
        }

        public bool IsStandard
        {
            get => _isStandard;
            set
            {
                if (_isStandard != value)
                {
                    _isStandard = value;
                    if (value)
                    {
                        IsScientific = false;
                        IsProgrammer = false;
                    }
                    OnPropertyChanged(nameof(IsStandard));
                }
            }
        }

        public bool IsScientific
        {
            get => _isScientific;
            set
            {
                if (_isScientific != value)
                {
                    _isScientific = value;
                    if (value)
                    {
                        IsStandard = false;
                        IsProgrammer = false;
                    }
                    OnPropertyChanged(nameof(IsScientific));
                }
            }
        }

        public bool IsProgrammer
        {
            get => _isProgrammer;
            set
            {
                if (_isProgrammer != value)
                {
                    _isProgrammer = value;
                    if (!_isProgrammer)
                    {
                        IsBitFlipChecked = false;
                    }
                    IsBinaryBitFlippingEnabled = _isProgrammer && IsBitFlipChecked;
                    AreProgrammerRadixOperatorsVisible = _isProgrammer && !IsBitFlipChecked;
                    if (value)
                    {
                        IsStandard = false;
                        IsScientific = false;
                    }
                    OnPropertyChanged(nameof(IsProgrammer));
                }
            }
        }

        public bool IsEditingEnabled
        {
            get => _isEditingEnabled;
            set
            {
                if (_isEditingEnabled != value)
                {
                    _isEditingEnabled = value;
                    bool currentEditToggleValue = !_isEditingEnabled;
                    IsBinaryOperatorEnabled = currentEditToggleValue;
                    IsUnaryOperatorEnabled = currentEditToggleValue;
                    IsOperandEnabled = currentEditToggleValue;
                    IsNegateEnabled = currentEditToggleValue;
                    IsDecimalEnabled = currentEditToggleValue;
                    OnPropertyChanged(nameof(IsEditingEnabled));
                }
            }
        }

        public bool IsEngineRecording => _engine.IsEngineRecording;

        public bool IsOperandEnabled
        {
            get => _isOperandEnabled;
            set
            {
                if (_isOperandEnabled != value)
                {
                    _isOperandEnabled = value;
                    IsDecimalEnabled = value;
                    AreHEXButtonsEnabled = IsProgrammer;
                    IsFToEEnabled = value;
                    OnPropertyChanged(nameof(IsOperandEnabled));
                }
            }
        }

        #endregion

                // PORT NOTE (Task 9): multi-window Snapshot save/restore omitted;
        // history recall works through flat engine-command replay (see Recalculate).


        private RelayCommand<object> _copyCommand;
        private RelayCommand<object> _pasteCommand;
        private RelayCommand<object> _buttonPressedCommand;
        private RelayCommand<object> _clearMemoryCommand;
        private RelayCommand<object> _memoryItemPressedCommand;
        private RelayCommand<object> _memoryAddCommand;
        private RelayCommand<object> _memorySubtractCommand;

        public RelayCommand<object> CopyCommand => _copyCommand ?? (_copyCommand = new RelayCommand<object>(OnCopyCommand));
        public RelayCommand<object> PasteCommand => _pasteCommand ?? (_pasteCommand = new RelayCommand<object>(OnPasteCommand));
        public RelayCommand<object> ButtonPressedCommand => _buttonPressedCommand ?? (_buttonPressedCommand = new RelayCommand<object>(OnButtonPressed));
        public RelayCommand<object> ClearMemoryCommand => _clearMemoryCommand ?? (_clearMemoryCommand = new RelayCommand<object>(OnClearMemoryCommand));
        public RelayCommand<object> MemoryItemPressedCommand => _memoryItemPressedCommand ?? (_memoryItemPressedCommand = new RelayCommand<object>(OnMemoryItemPressed));
        public RelayCommand<object> MemoryAddCommand => _memoryAddCommand ?? (_memoryAddCommand = new RelayCommand<object>(OnMemoryAdd));
        public RelayCommand<object> MemorySubtractCommand => _memorySubtractCommand ?? (_memorySubtractCommand = new RelayCommand<object>(OnMemorySubtract));

        public RelayCommand<object> ButtonPressed => ButtonPressedCommand;
        public RelayCommand<object> MemoryItemPressed => MemoryItemPressedCommand;
        public RelayCommand<object> MemoryAdd => MemoryAddCommand;
        public RelayCommand<object> MemorySubtract => MemorySubtractCommand;

        public void UpdateOperand(int pos, string text)
        {
            if (_tokens == null || pos < 0 || pos >= _tokens.Count)
            {
                return;
            }

            var p = _tokens[pos];
            string englishString = LinuxLocalization.Current.GetEnglishValueFromLocalizedDigits(text);
            p.Token = englishString;

            // UpdateOperand not available via interop - operand updates handled through command replay
        }

        public void SetCalculatorType(ViewMode targetState)
        {
            IsInError = false;

            switch (targetState)
            {
                case ViewMode.Standard:
                    IsStandard = true;
                    ResetRadixAndUpdateMemory(true);
                    SetPrecision(StandardModePrecision);
                    UpdateMaxIntDigits();
                    break;

                case ViewMode.Scientific:
                    IsScientific = true;
                    ResetRadixAndUpdateMemory(true);
                    SetPrecision(ScientificModePrecision);
                    break;

                case ViewMode.Programmer:
                    IsProgrammer = true;
                    ResetRadixAndUpdateMemory(false);
                    SetPrecision(ProgrammerModePrecision);
                    break;
            }
        }

        public void OnCopyCommand(object parameter)
        {
            CopyPasteManager.CopyToClipboard(GetRawDisplayValue());

            string announcement = AppResources.Instance.GetResourceString("Display_Copied");
            Announcement = CalculatorAnnouncement.GetDisplayCopiedAnnouncement(announcement);
        }

        public void OnPasteCommand(object parameter)
        {
            ViewMode mode;
            BitLength bitLengthType = BitLength.BitLengthUnknown;
            NumberBase numberBase = NumberBase.Unknown;

            if (IsScientific)
            {
                mode = ViewMode.Scientific;
            }
            else if (IsProgrammer)
            {
                mode = ViewMode.Programmer;
                bitLengthType = _valueBitLength;
                numberBase = CurrentRadixType;
            }
            else
            {
                mode = ViewMode.Standard;
            }

            if (IsEditingEnabled || !CopyPasteManager.HasStringToPaste())
            {
                return;
            }

            _ = PasteAsync(mode, numberBase, bitLengthType);
        }

        private async Task PasteAsync(ViewMode mode, NumberBase numberBase, BitLength bitLengthType)
        {
            string pastedString = await CopyPasteManager.GetStringToPaste(mode, NavCategoryStates.GetGroupType(mode), numberBase, bitLengthType);
            OnPaste(pastedString);
        }

        public void OnMemoryButtonPressed()
        {
            _engine.Memorize();
            LinuxTraceLogger.Instance.UpdateButtonUsage(NumbersAndOperatorsEnum.Memory, GetCalculatorMode());

            if (_localizedMemorySavedAutomationFormat == null)
            {
                _localizedMemorySavedAutomationFormat = AppResources.Instance.GetResourceString("Format_MemorySave");
            }
            string announcement = LocalizationStringUtil.GetLocalizedString(_localizedMemorySavedAutomationFormat, DisplayValue);
            Announcement = CalculatorAnnouncement.GetMemoryItemAddedAnnouncement(announcement);
        }

        public void OnMemoryItemPressed(object memoryItemPosition)
        {
            if (MemorizedNumbers != null && MemorizedNumbers.Count > 0)
            {
                int position = (int)memoryItemPosition;
                _engine.MemoryLoad((uint)position);
                HideMemoryClicked?.Invoke();

                var mode = IsStandard ? ViewMode.Standard : IsScientific ? ViewMode.Scientific : ViewMode.Programmer;
                LinuxTraceLogger.Instance.LogMemoryItemLoad(mode, MemorizedNumbers.Count, position);
            }
        }

        public void OnMemoryAdd(object memoryItemPosition)
        {
            if (MemorizedNumbers != null)
            {
                int position = (int)memoryItemPosition;
                LinuxTraceLogger.Instance.UpdateButtonUsage(NumbersAndOperatorsEnum.MemoryAdd, GetCalculatorMode());
                _engine.MemoryAdd((uint)position);
            }
        }

        public void OnMemorySubtract(object memoryItemPosition)
        {
            if (MemorizedNumbers != null)
            {
                int position = (int)memoryItemPosition;
                LinuxTraceLogger.Instance.UpdateButtonUsage(NumbersAndOperatorsEnum.MemorySubtract, GetCalculatorMode());
                _engine.MemorySubtract((uint)position);
            }
        }

        public void OnMemoryClear(object memoryItemPosition)
        {
            if (MemorizedNumbers != null && MemorizedNumbers.Count > 0)
            {
                int position = (int)memoryItemPosition;

                if (position >= 0)
                {
                    uint unsignedPosition = (uint)position;
                    _engine.MemoryClear(unsignedPosition);

                    MemorizedNumbers.RemoveAt(position);
                    for (int i = 0; i < MemorizedNumbers.Count; i++)
                    {
                        MemorizedNumbers[i].Position = i;
                    }

                    if (MemorizedNumbers.Count == 0)
                    {
                        IsMemoryEmpty = true;
                    }
                    LinuxTraceLogger.Instance.UpdateButtonUsage(NumbersAndOperatorsEnum.MemoryClear, GetCalculatorMode());

                    string localizedIndex = (position + 1).ToString();
                    LinuxLocalization.Current.LocalizeDisplayValue(ref localizedIndex);

                    if (_localizedMemoryItemClearedAutomationFormat == null)
                    {
                        _localizedMemoryItemClearedAutomationFormat = AppResources.Instance.GetResourceString("Format_MemorySlotCleared");
                    }
                    string announcement = LocalizationStringUtil.GetLocalizedString(_localizedMemoryItemClearedAutomationFormat, localizedIndex);
                    Announcement = CalculatorAnnouncement.GetMemoryClearedAnnouncement(announcement);
                }
            }
        }

        public void SelectHistoryItem(HistoryItemViewModel item)
        {
            SetHistoryExpressionDisplay(item.GetTokens(), item.GetCommands());
            SetExpressionDisplay(item.GetTokens());
            SetPrimaryDisplay(item.Result, false);
            IsFToEEnabled = false;
        }

        public void SwitchProgrammerModeBase(NumberBase numberBase)
        {
            if (IsInError)
            {
                _engine.Send((int)CalcCommand.CommandCLEAR);
            }

            AreHEXButtonsEnabled = numberBase == NumberBase.HexBase;
            CurrentRadixType = numberBase;
            _engine.SetRadix((int)GetRadixTypeFromNumberBase(numberBase));
        }

        public void SetBitshiftRadioButtonCheckedAnnouncement(string announcement)
        {
            Announcement = CalculatorAnnouncement.GetBitShiftRadioButtonCheckedAnnouncement(announcement);
        }

        public void SetOpenParenthesisCountNarratorAnnouncement()
        {
            string localizedParenthesisCount = _openParenthesisCount.ToString();
            LinuxLocalization.Current.LocalizeDisplayValue(ref localizedParenthesisCount);

            if (_localizedOpenParenthesisCountChangedAutomationFormat == null)
            {
                _localizedOpenParenthesisCountChangedAutomationFormat =
                    AppResources.Instance.GetResourceString("Format_OpenParenthesisCountAutomationNamePrefix");
            }
            string announcement = LocalizationStringUtil.GetLocalizedString(
                _localizedOpenParenthesisCountChangedAutomationFormat, localizedParenthesisCount);
            Announcement = CalculatorAnnouncement.GetOpenParenthesisCountChangedAnnouncement(announcement);
        }

        public void SwitchAngleType(NumbersAndOperatorsEnum num)
        {
            OnButtonPressed(num);
        }

        public void FtoEButtonToggled()
        {
            OnButtonPressed(NumbersAndOperatorsEnum.FToE);
        }

        public void ResetCalcManager(bool clearMemory)
        {
            _engine.Reset(clearMemory);
        }



        private void ResetManagedCalculatorSubmodes()
        {
            _currentAngleType = NumbersAndOperatorsEnum.Degree;
            IsFToEChecked = false;
            IsShiftProgrammerChecked = false;
            IsBitFlipChecked = false;
            if (_valueBitLength != BitLength.BitLengthQWord)
            {
                _valueBitLength = BitLength.BitLengthQWord;
                OnPropertyChanged(nameof(ValueBitLength));
            }
            CurrentRadixType = NumberBase.DecBase;
            AreHEXButtonsEnabled = false;
        }

        private void SetNativeCalculatorMode(ViewMode mode)
        {
            switch (mode)
            {
                case ViewMode.Standard:
                    _engine.SetStandardMode();
                    break;
                case ViewMode.Scientific:
                    _engine.SetScientificMode();
                    break;
                case ViewMode.Programmer:
                    _engine.SetProgrammerMode();
                    break;
            }
        }

        public void SendCommandToCalcManager(int command)
        {
            _engine.Send((int)command);
        }

        #region Internal callback methods (called by CalculatorDisplay)

        public void SetPrimaryDisplay(string displayStringValue, bool isError)
        {
            string localizedDisplayStringValue = LocalizeDisplayValue(displayStringValue);

            _calculationResultAutomationName = CalculateNarratorDisplayValue(displayStringValue, localizedDisplayStringValue);

            AreAlwaysOnTopResultsUpdated = false;
            if (DisplayValue != localizedDisplayStringValue)
            {
                DisplayValue = localizedDisplayStringValue;
                AreAlwaysOnTopResultsUpdated = true;
            }

            IsInError = isError;

            if (IsProgrammer)
            {
                UpdateProgrammerPanelDisplay();
            }
        }

        public void SetExpressionDisplay(List<(string Token, int CommandIndex, int CommandType)> tokens)
        {
            SetExpressionDisplay(tokens, _engine.DisplayCommands());
        }

        internal void SetExpressionDisplay(
            List<(string Token, int CommandIndex, int CommandType)> tokens,
            List<int> commands)
        {
            _tokens = new List<(string Token, int CommandIndex, int CommandType)>(tokens);
            _commands = new List<int>(commands);
            if (!IsEditingEnabled)
            {
                SetTokens(tokens);
            }

            CalculationExpressionAutomationName = GetCalculatorExpressionAutomationName();
            AreTokensUpdated = true;
        }

        public void SetParenthesisCount(uint parenthesisCount)
        {
            if (_openParenthesisCount == parenthesisCount)
            {
                return;
            }

            OpenParenthesisCount = parenthesisCount;
            if (IsProgrammer || IsScientific)
            {
                SetOpenParenthesisCountNarratorAnnouncement();
            }
        }

        public void OnNoRightParenAdded()
        {
            SetNoParenAddedNarratorAnnouncement();
        }

        public void SetIsInError(bool isError)
        {
            IsInError = isError;
        }

        public void SyncMemorizedNumbers(List<string> newMemorizedNumbers)
        {
            var localizer = LinuxLocalization.Current;

            if (newMemorizedNumbers.Count == 0)
            {
                MemorizedNumbers.Clear();
                IsMemoryEmpty = true;
            }
            else if (newMemorizedNumbers.Count > MemorizedNumbers.Count)
            {
                while (newMemorizedNumbers.Count > MemorizedNumbers.Count)
                {
                    int newValuePosition = newMemorizedNumbers.Count - MemorizedNumbers.Count - 1;
                    string stringValue = newMemorizedNumbers[newValuePosition];

                    var memorySlot = new MemoryItemViewModel(this);
                    memorySlot.Position = 0;
                    localizer.LocalizeDisplayValue(ref stringValue);
                    memorySlot.Value = stringValue;

                    MemorizedNumbers.Insert(0, memorySlot);
                    IsMemoryEmpty = IsAlwaysOnTop;

                    for (int i = 1; i < MemorizedNumbers.Count; i++)
                    {
                        MemorizedNumbers[i].Position++;
                    }
                }
            }
            else if (newMemorizedNumbers.Count == MemorizedNumbers.Count)
            {
                for (int i = 0; i < MemorizedNumbers.Count; i++)
                {
                    string newStringValue = newMemorizedNumbers[i];
                    localizer.LocalizeDisplayValue(ref newStringValue);

                    if (MemorizedNumbers[i].Value != newStringValue)
                    {
                        MemorizedNumbers[i].Value = newStringValue;
                    }
                }
            }
        }

        public void OnMaxDigitsReached()
        {
            if (_localizedMaxDigitsReachedAutomationFormat == null)
            {
                _localizedMaxDigitsReachedAutomationFormat =
                    AppResources.Instance.GetResourceString("Format_MaxDigitsReached");
            }
            string announcement = LocalizationStringUtil.GetLocalizedString(
                _localizedMaxDigitsReachedAutomationFormat, _calculationResultAutomationName);
            Announcement = CalculatorAnnouncement.GetMaxDigitsReachedAnnouncement(announcement);
        }

        public void OnBinaryOperatorReceived()
        {
            Announcement = GetDisplayUpdatedNarratorAnnouncement();
        }

        public void OnMemoryItemChanged(uint indexOfMemory)
        {
            if (indexOfMemory < (uint)MemorizedNumbers.Count)
            {
                var memSlot = MemorizedNumbers[(int)indexOfMemory];
                string localizedValue = memSlot.Value;

                string localizedIndex = (indexOfMemory + 1).ToString();
                LinuxLocalization.Current.LocalizeDisplayValue(ref localizedIndex);

                if (_localizedMemoryItemChangedAutomationFormat == null)
                {
                    _localizedMemoryItemChangedAutomationFormat =
                        AppResources.Instance.GetResourceString("Format_MemorySlotChanged");
                }
                string announcement = LocalizationStringUtil.GetLocalizedString(
                    _localizedMemoryItemChangedAutomationFormat, localizedIndex, localizedValue);
                Announcement = CalculatorAnnouncement.GetMemoryItemChangedAnnouncement(announcement);
            }
        }

        public void OnInputChanged()
        {
            IsInputEmpty = _engine.IsInputEmpty;
        }

        #endregion

        private string LocalizeDisplayValue(string displayValue)
        {
            string result = displayValue;

            if (IsProgrammer && CurrentRadixType == NumberBase.BinBase)
            {
                result = AddPadding(result);
            }

            LinuxLocalization.Current.LocalizeDisplayValue(ref result);
            return result;
        }

        private string CalculateNarratorDisplayValue(string displayValue, string localizedDisplayValue)
        {
            string localizedValue = localizedDisplayValue;
            string automationFormat = _localizedCalculationResultAutomationFormat;

            if (CalcUtils.IsLastCharacterTarget(displayValue, _decimalSeparator))
            {
                localizedValue = LocalizeDisplayValue(displayValue.Substring(0, displayValue.Length - 1));
                automationFormat = _localizedCalculationResultDecimalAutomationFormat;
            }

            if (IsProgrammer && CurrentRadixType != NumberBase.DecBase)
            {
                localizedValue = GetNarratorStringReadRawNumbers(localizedValue);
            }

            return GetLocalizedStringFormat(automationFormat, localizedValue);
        }

        private string GetNarratorStringReadRawNumbers(string localizedDisplayValue)
        {
            var locSettings = LinuxLocalization.Current;
            string result = string.Empty;

            foreach (char c in localizedDisplayValue)
            {
                result += c;
                if (locSettings.IsLocalizedHexDigit(c))
                {
                    result += ' ';
                }
            }

            return result;
        }

        private void DisplayPasteError()
        {
            // Displays the localized "Invalid input" string and sets error state.
            const int IDS_ERRORS_FIRST = 99;
            const int IDS_DOMAIN = IDS_ERRORS_FIRST + 1;
            string errorString = EngineStrings.GetCEngineString(IDS_DOMAIN.ToString());
            SetPrimaryDisplay(errorString, true);
        }

        private void SetNoParenAddedNarratorAnnouncement()
        {
            if (_localizedNoRightParenthesisAddedFormat == null)
            {
                _localizedNoRightParenthesisAddedFormat =
                    AppResources.Instance.GetResourceString("NoRightParenthesisAdded_Announcement");
            }
            Announcement = CalculatorAnnouncement.GetNoRightParenthesisAddedAnnouncement(_localizedNoRightParenthesisAddedFormat);
        }

        private void DisableButtons(CalcCommandType selectedExpressionCommandType)
        {
            if (selectedExpressionCommandType == CalcCommandType.OperandCommand)
            {
                IsBinaryOperatorEnabled = false;
                IsUnaryOperatorEnabled = false;
                IsOperandEnabled = true;
                IsNegateEnabled = true;
                IsDecimalEnabled = true;
            }
            else if (selectedExpressionCommandType == CalcCommandType.BinaryCommand)
            {
                IsBinaryOperatorEnabled = true;
                IsUnaryOperatorEnabled = false;
                IsOperandEnabled = false;
                IsNegateEnabled = false;
                IsDecimalEnabled = false;
            }
            else if (selectedExpressionCommandType == CalcCommandType.UnaryCommand)
            {
                IsBinaryOperatorEnabled = false;
                IsUnaryOperatorEnabled = true;
                IsOperandEnabled = false;
                IsNegateEnabled = true;
                IsDecimalEnabled = false;
            }
        }

        private void SetHistoryExpressionDisplay(
            List<(string Token, int CommandIndex, int CommandType)> tokens,
            List<int> commands)
        {
            _tokens = new List<(string Token, int CommandIndex, int CommandType)>(tokens);
            _commands = new List<int>(commands);
            IsEditingEnabled = false;

            _engine.SetHistoryLoadMode(true);
            Recalculate(fromHistory: true);
            _engine.SetHistoryLoadMode(false);
            _isLastOperationHistoryLoad = true;
        }

        private void SetTokens(List<(string Token, int CommandIndex, int CommandType)> tokens)
        {
            AreTokensUpdated = false;

            int nTokens = tokens?.Count ?? 0;

            if (nTokens == 0)
            {
                _expressionTokens.Clear();
                return;
            }

            var localizer = LinuxLocalization.Current;
            const string separator = " ";

            for (int i = 0; i < nTokens; i++)
            {
                var currentToken = tokens[i];

                TokenType type;
                bool isEditable = currentToken.CommandIndex != -1;
                localizer.LocalizeDisplayValue(ref currentToken.Token);

                if (!isEditable)
                {
                    type = currentToken.Token == separator ? TokenType.Separator : TokenType.Operator;
                }
                else
                {
                    type = currentToken.CommandType == (int)CalcCommandType.OperandCommand ? TokenType.Operand : TokenType.Operator;
                }

                string currentTokenString = currentToken.Token;
                if (i < _expressionTokens.Count)
                {
                    var existingItem = _expressionTokens[i];
                    if (type == existingItem.Type && existingItem.Token == currentTokenString)
                    {
                        existingItem.TokenPosition = i;
                        existingItem.IsTokenEditable = isEditable;
                        existingItem.CommandIndex = 0;
                    }
                    else
                    {
                        var expressionToken = new DisplayExpressionToken(currentTokenString, i, isEditable, type);
                        _expressionTokens.Insert(i, expressionToken);
                    }
                }
                else
                {
                    var expressionToken = new DisplayExpressionToken(currentTokenString, i, isEditable, type);
                    _expressionTokens.Add(expressionToken);
                }
            }

            while (_expressionTokens.Count != nTokens)
            {
                _expressionTokens.RemoveAt(_expressionTokens.Count - 1);
            }
        }

        private string GetCalculatorExpressionAutomationName()
        {
            string expression = string.Empty;
            foreach (var token in _expressionTokens)
            {
                expression += NarratorTokens.GetReadableToken(token.Token);
            }

            return GetLocalizedStringFormat(_expressionAutomationNameFormat, expression);
        }

        public string GetLocalizedStringFormat(string format, string displayValue)
        {
            return LocalizationStringUtil.GetLocalizedString(format, displayValue);
        }

        private void OnButtonPressed(object parameter)
        {
            if (parameter == null)
            {
                return;
            }

            _feedbackForButtonPress = CalculatorButtonPressedEventArgs.GetAuditoryFeedbackFromCommandParameter(parameter);
            OnButtonPressed(CalculatorButtonPressedEventArgs.GetOperationFromCommandParameter(parameter));
        }

        private void OnButtonPressed(NumbersAndOperatorsEnum numOpEnum)
        {
            int cmdenum = (int)numOpEnum;

            if (IsInError)
            {
                _engine.Send((int)CalcCommand.CommandCLEAR);

                if (!IsRecoverableCommand(cmdenum))
                {
                    return;
                }
            }

            if (IsEditingEnabled
                && numOpEnum != NumbersAndOperatorsEnum.IsScientificMode
                && numOpEnum != NumbersAndOperatorsEnum.IsStandardMode
                && numOpEnum != NumbersAndOperatorsEnum.IsProgrammerMode
                && numOpEnum != NumbersAndOperatorsEnum.FToE
                && numOpEnum != NumbersAndOperatorsEnum.Degree
                && numOpEnum != NumbersAndOperatorsEnum.Radians
                && numOpEnum != NumbersAndOperatorsEnum.Grads)
            {
                if (!_keyPressed)
                {
                    SaveEditedCommand(_selectedExpressionToken?.TokenPosition ?? 0, cmdenum);
                }
            }
            else
            {
                if (numOpEnum == NumbersAndOperatorsEnum.IsStandardMode
                    || numOpEnum == NumbersAndOperatorsEnum.IsScientificMode
                    || numOpEnum == NumbersAndOperatorsEnum.IsProgrammerMode)
                {
                    IsEditingEnabled = false;
                }

                if (numOpEnum == NumbersAndOperatorsEnum.Memory)
                {
                    OnMemoryButtonPressed();
                }
                else
                {
                    if (numOpEnum == NumbersAndOperatorsEnum.Clear
                        || numOpEnum == NumbersAndOperatorsEnum.ClearEntry
                        || numOpEnum == NumbersAndOperatorsEnum.IsStandardMode
                        || numOpEnum == NumbersAndOperatorsEnum.IsProgrammerMode)
                    {
                        if (IsFToEChecked)
                        {
                            IsFToEChecked = false;
                        }
                    }

                    if (numOpEnum == NumbersAndOperatorsEnum.Degree
                        || numOpEnum == NumbersAndOperatorsEnum.Radians
                        || numOpEnum == NumbersAndOperatorsEnum.Grads)
                    {
                        _currentAngleType = numOpEnum;
                    }

                    if (IsDigitOrBackspace(cmdenum))
                    {
                        IsOperatorCommand = false;
                    }
                    else
                    {
                        IsOperatorCommand = true;
                    }

                    if (_isLastOperationHistoryLoad
                        && numOpEnum != NumbersAndOperatorsEnum.Degree
                        && numOpEnum != NumbersAndOperatorsEnum.Radians
                        && numOpEnum != NumbersAndOperatorsEnum.Grads)
                    {
                        IsFToEEnabled = true;
                        _isLastOperationHistoryLoad = false;
                    }

                    LinuxTraceLogger.Instance.UpdateButtonUsage(numOpEnum, GetCalculatorMode());
                    _engine.Send((int)cmdenum);
                }
            }
        }

        private bool IsDigitOrBackspace(int cmd)
        {
            return (cmd >= (int)CalcCommand.Command0 && cmd <= (int)CalcCommand.Command9)
                   || cmd == (int)CalcCommand.CommandPNT
                   || cmd == (int)CalcCommand.CommandBACK
                   || cmd == (int)CalcCommand.CommandEXP;
        }

        private void OnClearMemoryCommand(object parameter)
        {
            _engine.MemoryClearAll();
            LinuxTraceLogger.Instance.UpdateButtonUsage(NumbersAndOperatorsEnum.MemoryClear, GetCalculatorMode());

            if (_localizedMemoryCleared == null)
            {
                _localizedMemoryCleared = AppResources.Instance.GetResourceString("Memory_Cleared");
            }
            Announcement = CalculatorAnnouncement.GetMemoryClearedAnnouncement(_localizedMemoryCleared);
        }

        public ButtonInfo MapCharacterToButtonId(char ch)
        {
            var result = new ButtonInfo
            {
                ButtonId = NumbersAndOperatorsEnum.None,
                CanSendNegate = false
            };

            switch (ch)
            {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    result.ButtonId = NumbersAndOperatorsEnum.Zero + (ch - '0');
                    result.CanSendNegate = true;
                    break;
                case '*':
                    result.ButtonId = NumbersAndOperatorsEnum.Multiply;
                    break;
                case '+':
                    result.ButtonId = NumbersAndOperatorsEnum.Add;
                    break;
                case '-':
                    result.ButtonId = NumbersAndOperatorsEnum.Subtract;
                    break;
                case '/':
                    result.ButtonId = NumbersAndOperatorsEnum.Divide;
                    break;
                case '^':
                    if (IsScientific) result.ButtonId = NumbersAndOperatorsEnum.XPowerY;
                    break;
                case '%':
                    if (IsScientific || IsProgrammer) result.ButtonId = NumbersAndOperatorsEnum.Mod;
                    break;
                case '=':
                    result.ButtonId = NumbersAndOperatorsEnum.Equals;
                    break;
                case '(':
                    result.ButtonId = NumbersAndOperatorsEnum.OpenParenthesis;
                    break;
                case ')':
                    result.ButtonId = NumbersAndOperatorsEnum.CloseParenthesis;
                    break;
                case 'a': case 'A':
                    result.ButtonId = NumbersAndOperatorsEnum.A;
                    break;
                case 'b': case 'B':
                    result.ButtonId = NumbersAndOperatorsEnum.B;
                    break;
                case 'c': case 'C':
                    result.ButtonId = NumbersAndOperatorsEnum.C;
                    break;
                case 'd': case 'D':
                    result.ButtonId = NumbersAndOperatorsEnum.D;
                    break;
                case 'e': case 'E':
                    result.ButtonId = IsProgrammer ? NumbersAndOperatorsEnum.E : NumbersAndOperatorsEnum.Exp;
                    break;
                case 'f': case 'F':
                    result.ButtonId = NumbersAndOperatorsEnum.F;
                    break;
                default:
                    if (ch == _decimalSeparator)
                    {
                        result.ButtonId = NumbersAndOperatorsEnum.Decimal;
                    }
                    break;
            }

            if (result.ButtonId == NumbersAndOperatorsEnum.None)
            {
                if (LinuxLocalization.Current.IsLocalizedDigit(ch))
                {
                    result.ButtonId = NumbersAndOperatorsEnum.Zero +
                        (ch - LinuxLocalization.Current.GetDigitSymbolFromEnUsDigit('0'));
                    result.CanSendNegate = true;
                }
            }

            if (NumbersAndOperatorsEnum.Zero == result.ButtonId)
            {
                result.CanSendNegate = false;
            }

            return result;
        }

        public void OnPaste(string pastedString)
        {
            if (CopyPasteManager.IsErrorMessage(pastedString))
            {
                DisplayPasteError();
                return;
            }

            LinuxTraceLogger.Instance.LogInputPasted(GetCalculatorMode());
            bool isFirstLegalChar = true;
            _engine.Send((int)CalcCommand.CommandCENTR);
            bool sendNegate = false;

            bool sentEquals = false;
            bool isPreviousOperator = false;

            var negateStack = new List<bool>();

            int i = 0;
            while (i < pastedString.Length)
            {
                bool sendCommand = true;
                var buttonInfo = MapCharacterToButtonId(pastedString[i]);

                NumbersAndOperatorsEnum mappedNumOp = buttonInfo.ButtonId;
                bool canSendNegate = buttonInfo.CanSendNegate;

                if (mappedNumOp == NumbersAndOperatorsEnum.None)
                {
                    i++;
                    continue;
                }

                if (isFirstLegalChar || isPreviousOperator)
                {
                    isFirstLegalChar = false;
                    isPreviousOperator = false;

                    if (NumbersAndOperatorsEnum.Subtract == mappedNumOp)
                    {
                        sendNegate = true;
                        sendCommand = false;
                    }
                    if (NumbersAndOperatorsEnum.Add == mappedNumOp)
                    {
                        sendCommand = false;
                    }
                }

                switch (mappedNumOp)
                {
                    case NumbersAndOperatorsEnum.OpenParenthesis:
                        negateStack.Add(sendNegate);
                        sendNegate = false;
                        break;
                    case NumbersAndOperatorsEnum.CloseParenthesis:
                        if (negateStack.Count > 0)
                        {
                            sendNegate = negateStack[negateStack.Count - 1];
                            negateStack.RemoveAt(negateStack.Count - 1);
                            canSendNegate = true;
                        }
                        else
                        {
                            sendCommand = false;
                        }
                        break;
                    case NumbersAndOperatorsEnum.Zero:
                    case NumbersAndOperatorsEnum.One:
                    case NumbersAndOperatorsEnum.Two:
                    case NumbersAndOperatorsEnum.Three:
                    case NumbersAndOperatorsEnum.Four:
                    case NumbersAndOperatorsEnum.Five:
                    case NumbersAndOperatorsEnum.Six:
                    case NumbersAndOperatorsEnum.Seven:
                    case NumbersAndOperatorsEnum.Eight:
                    case NumbersAndOperatorsEnum.Nine:
                        break;
                    case NumbersAndOperatorsEnum.Add:
                    case NumbersAndOperatorsEnum.Subtract:
                    case NumbersAndOperatorsEnum.Multiply:
                    case NumbersAndOperatorsEnum.Divide:
                        isPreviousOperator = true;
                        break;
                }

                if (sendCommand)
                {
                    sentEquals = (mappedNumOp == NumbersAndOperatorsEnum.Equals);
                    _engine.Send((int)(int)mappedNumOp);

                    if (sendNegate)
                    {
                        if (canSendNegate)
                        {
                            _engine.Send((int)(int)NumbersAndOperatorsEnum.Negate);
                        }
                        if (NumbersAndOperatorsEnum.Zero != mappedNumOp && NumbersAndOperatorsEnum.Decimal != mappedNumOp)
                        {
                            sendNegate = false;
                        }
                    }
                }

                if (mappedNumOp == NumbersAndOperatorsEnum.Exp && i + 1 < pastedString.Length)
                {
                    var nextButton = MapCharacterToButtonId(pastedString[i + 1]);
                    if (nextButton.ButtonId == NumbersAndOperatorsEnum.Subtract)
                    {
                        _engine.Send((int)(int)NumbersAndOperatorsEnum.Negate);
                        i++;
                    }
                    else if (nextButton.ButtonId == NumbersAndOperatorsEnum.Add)
                    {
                        i++;
                    }
                }

                i++;
            }
        }

        private string GetRawDisplayValue()
        {
            if (IsInError)
            {
                return DisplayValue;
            }
            return LinuxLocalization.Current.RemoveGroupSeparators(DisplayValue);
        }

        private void ResetRadixAndUpdateMemory(bool resetRadix)
        {
            if (resetRadix)
            {
                AreHEXButtonsEnabled = false;
                CurrentRadixType = NumberBase.DecBase;
                _engine.SetRadix((int)RadixType.Decimal);
            }
            else
            {
                _engine.RefreshMemorizedNumbers();
            }
        }

        private void SetPrecision(int precision)
        {
            _engine.SetPrecision(precision);
        }

        internal void UpdateMaxIntDigits()
        {
            _engine.UpdateMaxIntDigits();
        }

        internal NumbersAndOperatorsEnum GetCurrentAngleType()
        {
            return _currentAngleType;
        }

        private void SetMemorizedNumbersString()
        {
            _engine.RefreshMemorizedNumbers();
        }

        private void Recalculate(bool fromHistory = false)
        {
            int currentDegreeMode = _engine.CurrentDegreeMode;
            var currentCommands = new List<int>(_commands);

            var savedTokens = new List<(string Token, int CommandIndex, int CommandType)>(_tokens);
            var savedCommands = new List<int>(_commands);

            _engine.Reset(false);
            if (IsScientific)
            {
                _engine.Send((int)CalcCommand.ModeScientific);
            }

            if (IsFToEChecked)
            {
                _engine.Send((int)CalcCommand.CommandFE);
            }

            _engine.Send(currentDegreeMode);

            foreach (var command in currentCommands)
            {
                _engine.Send(command);
            }

            if (fromHistory)
            {
                _engine.Send((int)CalcCommand.CommandFE);
                _engine.Send((int)CalcCommand.CommandFE);
            }

            if (IsInError)
            {
                SetExpressionDisplay(savedTokens, savedCommands);
            }
        }

        private void UpdateProgrammerPanelDisplay()
        {
            const int precision = 64;
            string hexDisplayString = string.Empty;
            string decimalDisplayString = string.Empty;
            string octalDisplayString = string.Empty;
            string binaryDisplayString = string.Empty;

            if (!IsInError)
            {
                hexDisplayString = _engine.ResultForRadix(16, precision, true);
                if (string.IsNullOrEmpty(hexDisplayString))
                {
                    hexDisplayString = DisplayValue;
                    decimalDisplayString = DisplayValue;
                    octalDisplayString = DisplayValue;
                    binaryDisplayString = DisplayValue;
                }
                else
                {
                    decimalDisplayString = _engine.ResultForRadix(10, precision, true);
                    octalDisplayString = _engine.ResultForRadix(8, precision, true);
                    binaryDisplayString = _engine.ResultForRadix(2, precision, true);
                }
            }

            var localizer = LinuxLocalization.Current;
            binaryDisplayString = AddPadding(binaryDisplayString);

            localizer.LocalizeDisplayValue(ref hexDisplayString);
            localizer.LocalizeDisplayValue(ref decimalDisplayString);
            localizer.LocalizeDisplayValue(ref octalDisplayString);
            localizer.LocalizeDisplayValue(ref binaryDisplayString);

            HexDisplayValue = hexDisplayString;
            DecimalDisplayValue = decimalDisplayString;
            OctalDisplayValue = octalDisplayString;
            BinaryDisplayValue = binaryDisplayString;

            HexDisplayValue_AutomationName = GetLocalizedStringFormat(_localizedHexaDecimalAutomationFormat, GetNarratorStringReadRawNumbers(HexDisplayValue));
            DecDisplayValue_AutomationName = GetLocalizedStringFormat(_localizedDecimalAutomationFormat, DecimalDisplayValue);
            OctDisplayValue_AutomationName = GetLocalizedStringFormat(_localizedOctalAutomationFormat, GetNarratorStringReadRawNumbers(OctalDisplayValue));
            BinDisplayValue_AutomationName = GetLocalizedStringFormat(_localizedBinaryAutomationFormat, GetNarratorStringReadRawNumbers(BinaryDisplayValue));

            var binaryValueArray = new List<bool>(new bool[64]);
            string binaryValue = _engine.ResultForRadix(2, precision, false);
            int idx = 0;
            if (!string.IsNullOrEmpty(binaryValue))
            {
                for (int j = binaryValue.Length - 1; j >= 0; j--)
                {
                    binaryValueArray[idx++] = binaryValue[j] == '1';
                }
            }
            BinaryDigits = binaryValueArray;
        }

        private NarratorAnnouncement GetDisplayUpdatedNarratorAnnouncement()
        {
            string announcement;
            if (string.IsNullOrEmpty(_feedbackForButtonPress))
            {
                announcement = _calculationResultAutomationName;
            }
            else
            {
                if (_localizedButtonPressFeedbackAutomationFormat == null)
                {
                    _localizedButtonPressFeedbackAutomationFormat =
                        AppResources.Instance.GetResourceString("Format_ButtonPressAuditoryFeedback");
                }
                announcement = LocalizationStringUtil.GetLocalizedString(
                    _localizedButtonPressFeedbackAutomationFormat, _calculationResultAutomationName, _feedbackForButtonPress);
            }

            _feedbackForButtonPress = null;
            return CalculatorAnnouncement.GetDisplayUpdatedAnnouncement(announcement);
        }

        private ViewMode GetCalculatorMode()
        {
            if (IsStandard) return ViewMode.Standard;
            if (IsScientific) return ViewMode.Scientific;
            return ViewMode.Programmer;
        }

        private static RadixType GetRadixTypeFromNumberBase(NumberBase numberBase)
        {
            switch (numberBase)
            {
                case NumberBase.BinBase: return RadixType.Binary;
                case NumberBase.HexBase: return RadixType.Hex;
                case NumberBase.OctBase: return RadixType.Octal;
                default: return RadixType.Decimal;
            }
        }

        protected override void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            base.OnPropertyChanged(e);
            HandlePropertySideEffects(e.PropertyName);
        }

        private void HandlePropertySideEffects(string propertyName)
        {
            if (propertyName == nameof(IsScientific))
            {
                if (IsScientific) OnButtonPressed(NumbersAndOperatorsEnum.IsScientificMode);
            }
            else if (propertyName == nameof(IsProgrammer))
            {
                if (IsProgrammer) OnButtonPressed(NumbersAndOperatorsEnum.IsProgrammerMode);
            }
            else if (propertyName == nameof(IsStandard))
            {
                if (IsStandard) OnButtonPressed(NumbersAndOperatorsEnum.IsStandardMode);
            }
            else if (propertyName == nameof(DisplayValue))
            {
                OnPropertyChanged(nameof(CalculationResultAutomationName));
                Announcement = GetDisplayUpdatedNarratorAnnouncement();
            }
            else if (propertyName == nameof(IsBitFlipChecked))
            {
                LinuxTraceLogger.Instance.UpdateButtonUsage(
                    IsBitFlipChecked ? NumbersAndOperatorsEnum.BitflipButton : NumbersAndOperatorsEnum.FullKeypadButton,
                    ViewMode.Programmer);
            }
            else if (propertyName == nameof(IsAlwaysOnTop))
            {
                string announcement = IsAlwaysOnTop
                    ? AppResources.Instance.GetResourceString("CalcAlwaysOnTop")
                    : AppResources.Instance.GetResourceString("CalcBackToFullView");
                Announcement = CalculatorAnnouncement.GetAlwaysOnTopChangedAnnouncement(announcement);
            }
        }

        private string AddPadding(string binaryString)
        {
            if (string.IsNullOrEmpty(binaryString))
            {
                return binaryString;
            }

            string englishValue = LinuxLocalization.Current.GetEnglishValueFromLocalizedDigits(binaryString);
            if (englishValue == "0")
            {
                return binaryString;
            }

            int lengthWithoutPadding = LengthWithoutPadding(binaryString);
            int pad = 4 - (lengthWithoutPadding % 4);
            if (pad == 4) pad = 0;
            return new string('0', pad) + binaryString;
        }

        private int LengthWithoutPadding(string str)
        {
            int count = 0;
            foreach (char c in str)
            {
                if (c != ' ') count++;
            }
            return count;
        }

        private void SaveEditedCommand(int tokenPosition, int command)
        {
            // SaveEditedCommand not available via interop - expression editing handled through command replay
        }

        private static bool IsOperator(int cmd)
        {
            if ((cmd >= (int)CalcCommand.Command0 && cmd <= (int)CalcCommand.Command9)
                || cmd == (int)CalcCommand.CommandPNT
                || cmd == (int)CalcCommand.CommandBACK
                || cmd == (int)CalcCommand.CommandEXP
                || cmd == (int)CalcCommand.CommandFE
                || cmd == (int)CalcCommand.ModeBasic
                || cmd == (int)CalcCommand.ModeProgrammer
                || cmd == (int)CalcCommand.ModeScientific
                || cmd == (int)CalcCommand.CommandINV
                || cmd == (int)CalcCommand.CommandCENTR
                || cmd == (int)NumbersAndOperatorsEnum.Degree
                || cmd == (int)NumbersAndOperatorsEnum.Radians
                || cmd == (int)NumbersAndOperatorsEnum.Grads
                || (cmd >= (int)CalcCommand.CommandBINEDITSTART && cmd <= (int)CalcCommand.CommandBINEDITEND))
            {
                return false;
            }
            return true;
        }

        private static bool IsRecoverableCommand(int command)
        {
            if (command >= (int)CalcCommand.Command0 && command <= (int)CalcCommand.Command9)
                return true;
            if (command == (int)CalcCommand.CommandPNT)
                return true;
            if (command >= (int)CalcCommand.CommandBINEDITSTART && command <= (int)CalcCommand.CommandBINEDITEND)
                return true;

            int[] recoverableCommands = {
                (int)CalcCommand.CommandA, (int)CalcCommand.CommandB,
                (int)CalcCommand.CommandC, (int)CalcCommand.CommandD,
                (int)CalcCommand.CommandE, (int)CalcCommand.CommandF
            };
            return Array.IndexOf(recoverableCommands, command) >= 0;
        }

        public void Dispose()
        {
            _engine.Dispose();
            GC.SuppressFinalize(this);
        }

    }

}
