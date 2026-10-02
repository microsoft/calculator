// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using CalcManager.CEngine;

namespace CalcManager.CalculationManager
{
    public enum CalculatorMode
    {
        Standard = 0,
        Scientific,
    }

    public enum CalculatorPrecision
    {
        StandardModePrecision = 16,
        ScientificModePrecision = 32,
        ProgrammerModePrecision = 64
    }

    public enum MemoryCommand
    {
        MemorizeNumber = 330,
        MemorizedNumberLoad = 331,
        MemorizedNumberAdd = 332,
        MemorizedNumberSubtract = 333,
        MemorizedNumberClearAll = 334,
        MemorizedNumberClear = 335
    }

    public class CalculatorManager : ICalcDisplay
    {
        private const int MAX_HISTORY_ITEMS = 20;
        private const uint MAXIMUM_MEMORY_SIZE = 100;

        private readonly ICalcDisplay m_displayCallback;
        private CCalcEngine m_currentCalculatorEngine;
        private CCalcEngine m_scientificCalculatorEngine;
        private CCalcEngine m_standardCalculatorEngine;
        private CCalcEngine m_programmerCalculatorEngine;
        private readonly IResourceProvider m_resourceProvider;
        private bool m_inHistoryItemLoadMode;

        private readonly List<Rational> m_memorizedNumbers = new List<Rational>();
        private Rational m_persistedPrimaryValue;
        private bool m_isExponentialFormat;
        private Command m_currentDegreeMode;

        private readonly CalculatorHistory m_pStdHistory;
        private readonly CalculatorHistory m_pSciHistory;
        private CalculatorHistory m_pHistory;

        public CalculatorManager(ICalcDisplay displayCallback, IResourceProvider resourceProvider)
        {
            m_displayCallback = displayCallback;
            m_currentCalculatorEngine = null;
            m_resourceProvider = resourceProvider;
            m_inHistoryItemLoadMode = false;
            m_persistedPrimaryValue = 0;
            m_isExponentialFormat = false;
            m_currentDegreeMode = Command.CommandNULL;
            m_pStdHistory = new CalculatorHistory(MAX_HISTORY_ITEMS);
            m_pSciHistory = new CalculatorHistory(MAX_HISTORY_ITEMS);
            m_pHistory = null;

            if (m_resourceProvider != null)
            {
                CCalcEngine.InitialOneTimeOnlySetup(m_resourceProvider);
            }
        }

        public void SetPrimaryDisplay(string displayString, bool isError)
        {
            if (!m_inHistoryItemLoadMode)
            {
                m_displayCallback?.SetPrimaryDisplay(displayString, isError);
            }
        }

        public void SetIsInError(bool isError)
        {
            m_displayCallback?.SetIsInError(isError);
        }

        public void DisplayPasteError()
        {
            m_currentCalculatorEngine?.DisplayError(Ratpack.CalcErr.CALC_E_DOMAIN);
        }

        public void MaxDigitsReached()
        {
            m_displayCallback?.MaxDigitsReached();
        }

        public void BinaryOperatorReceived()
        {
            m_displayCallback?.BinaryOperatorReceived();
        }

        public void MemoryItemChanged(uint indexOfMemory)
        {
            m_displayCallback?.MemoryItemChanged(indexOfMemory);
        }

        public void InputChanged()
        {
            m_displayCallback?.InputChanged();
        }

        public void SetExpressionDisplay(List<Tuple<string, int>> tokens, List<IExpressionCommand> commands)
        {
            if (!m_inHistoryItemLoadMode)
            {
                m_displayCallback?.SetExpressionDisplay(tokens, commands);
            }
        }

        public void SetMemorizedNumbers(List<string> memorizedNumbers)
        {
            m_displayCallback?.SetMemorizedNumbers(memorizedNumbers);
        }

        public void SetParenthesisNumber(uint parenthesisCount)
        {
            m_displayCallback?.SetParenthesisNumber(parenthesisCount);
        }

        public void OnNoRightParenAdded()
        {
            m_displayCallback?.OnNoRightParenAdded();
        }

        public void Reset(bool clearMemory = true)
        {
            SetStandardMode();

            if (m_scientificCalculatorEngine != null)
            {
                m_scientificCalculatorEngine.ProcessCommand(CCommand.IDC_CLEAR);
                m_scientificCalculatorEngine.ProcessCommand(CCommand.IDC_DEG);

                if (m_isExponentialFormat)
                {
                    m_isExponentialFormat = false;
                    m_scientificCalculatorEngine.ProcessCommand(CCommand.IDC_FE);
                }
            }
            m_currentDegreeMode = Command.CommandDEG;

            if (m_programmerCalculatorEngine != null)
            {
                m_programmerCalculatorEngine.ProcessCommand(CCommand.IDC_CLEAR);
                m_programmerCalculatorEngine.ProcessCommand(CCommand.IDC_QWORD);
            }

            if (clearMemory)
            {
                MemorizedNumberClearAll();
            }
        }

        public void SetStandardMode()
        {
            if (m_standardCalculatorEngine == null)
            {
                m_standardCalculatorEngine = new CCalcEngine(false, false, m_resourceProvider, this, m_pStdHistory);
            }

            m_currentCalculatorEngine = m_standardCalculatorEngine;
            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_DEC);
            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_CLEAR);
            m_currentCalculatorEngine.ChangePrecision((int)CalculatorPrecision.StandardModePrecision);
            UpdateMaxIntDigits();
            m_pHistory = m_pStdHistory;
        }

        public void SetScientificMode()
        {
            if (m_scientificCalculatorEngine == null)
            {
                m_scientificCalculatorEngine = new CCalcEngine(true, false, m_resourceProvider, this, m_pSciHistory);
            }

            m_currentCalculatorEngine = m_scientificCalculatorEngine;
            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_DEC);
            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_CLEAR);
            m_currentCalculatorEngine.ChangePrecision((int)CalculatorPrecision.ScientificModePrecision);
            m_pHistory = m_pSciHistory;
        }

        public void SetProgrammerMode()
        {
            if (m_programmerCalculatorEngine == null)
            {
                m_programmerCalculatorEngine = new CCalcEngine(true, true, m_resourceProvider, this, null);
            }

            m_currentCalculatorEngine = m_programmerCalculatorEngine;
            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_DEC);
            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_CLEAR);
            m_currentCalculatorEngine.ChangePrecision((int)CalculatorPrecision.ProgrammerModePrecision);
        }

        public void SendCommand(Command command)
        {
            if (command == Command.CommandCLEAR || command == Command.CommandEQU || command == Command.ModeBasic ||
                command == Command.ModeScientific || command == Command.ModeProgrammer)
            {
                switch (command)
                {
                    case Command.ModeBasic:
                        SetStandardMode();
                        break;
                    case Command.ModeScientific:
                        SetScientificMode();
                        break;
                    case Command.ModeProgrammer:
                        SetProgrammerMode();
                        break;
                    default:
                        m_currentCalculatorEngine?.ProcessCommand((uint)command);
                        break;
                }

                InputChanged();
                return;
            }

            if (command == Command.CommandDEG || command == Command.CommandRAD || command == Command.CommandGRAD)
            {
                m_currentDegreeMode = command;
            }

            switch (command)
            {
                case Command.CommandASIN:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandSIN);
                    break;
                case Command.CommandACOS:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandCOS);
                    break;
                case Command.CommandATAN:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandTAN);
                    break;
                case Command.CommandPOWE:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandLN);
                    break;
                case Command.CommandASINH:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandSINH);
                    break;
                case Command.CommandACOSH:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandCOSH);
                    break;
                case Command.CommandATANH:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandTANH);
                    break;
                case Command.CommandASEC:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandSEC);
                    break;
                case Command.CommandACSC:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandCSC);
                    break;
                case Command.CommandACOT:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandCOT);
                    break;
                case Command.CommandASECH:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandSECH);
                    break;
                case Command.CommandACSCH:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandCSCH);
                    break;
                case Command.CommandACOTH:
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandINV);
                    m_currentCalculatorEngine?.ProcessCommand((uint)Command.CommandCOTH);
                    break;
                case Command.CommandFE:
                    m_isExponentialFormat = !m_isExponentialFormat;
                    goto default;
                default:
                    m_currentCalculatorEngine?.ProcessCommand((uint)command);
                    break;
            }

            InputChanged();
        }

        private void LoadPersistedPrimaryValue()
        {
            m_currentCalculatorEngine?.PersistedMemObject(m_persistedPrimaryValue);
            m_currentCalculatorEngine?.ProcessCommand(CCommand.IDC_RECALL);
            InputChanged();
        }

        public void MemorizeNumber()
        {
            if (m_currentCalculatorEngine == null || m_currentCalculatorEngine.FInErrorState())
            {
                return;
            }

            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_STORE);
            var memoryObject = m_currentCalculatorEngine.PersistedMemObject();
            m_memorizedNumbers.Insert(0, memoryObject);

            if (m_memorizedNumbers.Count > MAXIMUM_MEMORY_SIZE)
            {
                m_memorizedNumbers.RemoveRange((int)MAXIMUM_MEMORY_SIZE, (int)(m_memorizedNumbers.Count - MAXIMUM_MEMORY_SIZE));
            }
            SetMemorizedNumbersString();
        }

        public void MemorizedNumberLoad(uint indexOfMemory)
        {
            if (m_currentCalculatorEngine == null || m_currentCalculatorEngine.FInErrorState())
            {
                return;
            }

            MemorizedNumberSelect(indexOfMemory);
            m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_RECALL);
            InputChanged();
        }

        public void MemorizedNumberAdd(uint indexOfMemory)
        {
            if (m_currentCalculatorEngine == null || m_currentCalculatorEngine.FInErrorState())
            {
                return;
            }

            if (m_memorizedNumbers.Count == 0)
            {
                MemorizeNumber();
            }
            else
            {
                MemorizedNumberSelect(indexOfMemory);
                m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_MPLUS);
                MemorizedNumberChanged(indexOfMemory);
                SetMemorizedNumbersString();
            }

            m_displayCallback?.MemoryItemChanged(indexOfMemory);
        }

        public void MemorizedNumberClear(uint indexOfMemory)
        {
            if (indexOfMemory < m_memorizedNumbers.Count)
            {
                m_memorizedNumbers.RemoveAt((int)indexOfMemory);
            }
        }

        public void MemorizedNumberSubtract(uint indexOfMemory)
        {
            if (m_currentCalculatorEngine == null || m_currentCalculatorEngine.FInErrorState())
            {
                return;
            }

            if (m_memorizedNumbers.Count == 0)
            {
                MemorizeNumber();
                MemorizedNumberSubtract(0);
                MemorizedNumberSubtract(0);
            }
            else
            {
                MemorizedNumberSelect(indexOfMemory);
                m_currentCalculatorEngine.ProcessCommand(CCommand.IDC_MMINUS);
                MemorizedNumberChanged(indexOfMemory);
                SetMemorizedNumbersString();
            }

            m_displayCallback?.MemoryItemChanged(indexOfMemory);
        }

        public void MemorizedNumberClearAll()
        {
            m_memorizedNumbers.Clear();
            m_currentCalculatorEngine?.ProcessCommand(CCommand.IDC_MCLEAR);
            SetMemorizedNumbersString();
        }

        private void MemorizedNumberSelect(uint indexOfMemory)
        {
            if (m_currentCalculatorEngine == null || m_currentCalculatorEngine.FInErrorState())
            {
                return;
            }

            if (indexOfMemory < m_memorizedNumbers.Count)
            {
                var memoryObject = m_memorizedNumbers[(int)indexOfMemory];
                m_currentCalculatorEngine.PersistedMemObject(memoryObject);
            }
        }

        private void MemorizedNumberChanged(uint indexOfMemory)
        {
            if (m_currentCalculatorEngine == null || m_currentCalculatorEngine.FInErrorState())
            {
                return;
            }

            if (indexOfMemory < m_memorizedNumbers.Count)
            {
                m_memorizedNumbers[(int)indexOfMemory] = m_currentCalculatorEngine.PersistedMemObject();
            }
        }

        public List<HistoryItem> GetHistoryItems() => m_pHistory?.GetHistory() ?? new List<HistoryItem>();

        public List<HistoryItem> GetHistoryItems(CalculatorMode mode)
        {
            return (mode == CalculatorMode.Standard ? m_pStdHistory?.GetHistory() : m_pSciHistory?.GetHistory()) ?? new List<HistoryItem>();
        }

        public void SetHistoryItems(List<HistoryItem> historyItems)
        {
            if (historyItems == null) return;
            foreach (var historyItem in historyItems)
            {
                var index = m_pHistory?.AddItem(historyItem) ?? 0;
                OnHistoryItemAdded(index);
            }
        }

        public HistoryItem GetHistoryItem(uint uIdx)
        {
            return m_pHistory?.GetHistoryItem(uIdx);
        }

        public void OnHistoryItemAdded(uint addedItemIndex)
        {
            m_displayCallback?.OnHistoryItemAdded(addedItemIndex);
        }

        public bool RemoveHistoryItem(uint uIdx)
        {
            return m_pHistory?.RemoveItem(uIdx) ?? false;
        }

        public void ClearHistory()
        {
            m_pHistory?.ClearHistory();
        }

        public int MaxHistorySize() => m_pHistory?.MaxHistorySize ?? 0;

        public void SetRadix(RadixType iRadixType)
        {
            switch (iRadixType)
            {
                case RadixType.Hex:
                    m_currentCalculatorEngine?.ProcessCommand(CCommand.IDC_HEX);
                    break;
                case RadixType.Decimal:
                    m_currentCalculatorEngine?.ProcessCommand(CCommand.IDC_DEC);
                    break;
                case RadixType.Octal:
                    m_currentCalculatorEngine?.ProcessCommand(CCommand.IDC_OCT);
                    break;
                case RadixType.Binary:
                    m_currentCalculatorEngine?.ProcessCommand(CCommand.IDC_BIN);
                    break;
            }
            SetMemorizedNumbersString();
        }

        public void SetMemorizedNumbersString()
        {
            var resultVector = new List<string>();
            if (m_currentCalculatorEngine != null)
            {
                var radix = m_currentCalculatorEngine.GetCurrentRadix();
                foreach (var memoryItem in m_memorizedNumbers)
                {
                    string stringValue = m_currentCalculatorEngine.GetStringForDisplay(memoryItem, radix);
                    if (!string.IsNullOrEmpty(stringValue))
                    {
                        resultVector.Add(m_currentCalculatorEngine.GroupDigitsPerRadix(stringValue, radix));
                    }
                }
            }
            m_displayCallback?.SetMemorizedNumbers(resultVector);
        }

        public Command GetCurrentDegreeMode()
        {
            if (m_currentDegreeMode == Command.CommandNULL)
            {
                m_currentDegreeMode = Command.CommandDEG;
            }
            return m_currentDegreeMode;
        }

        public string GetResultForRadix(uint radix, int precision, bool groupDigitsPerRadix)
        {
            return m_currentCalculatorEngine != null ? m_currentCalculatorEngine.GetCurrentResultForRadix(radix, precision, groupDigitsPerRadix) : string.Empty;
        }

        public void SetPrecision(int precision)
        {
            m_currentCalculatorEngine?.ChangePrecision(precision);
        }

        public void UpdateMaxIntDigits()
        {
            m_currentCalculatorEngine?.UpdateMaxIntDigits();
        }

        public char DecimalSeparator()
        {
            if (m_currentCalculatorEngine != null)
            {
                return m_currentCalculatorEngine.DecimalSeparator();
            }
            string dec = m_resourceProvider?.GetCEngineString("sDecimal");
            return string.IsNullOrEmpty(dec) ? '.' : dec[0];
        }

        public bool IsEngineRecording()
        {
            return m_currentCalculatorEngine != null && m_currentCalculatorEngine.FInRecordingState();
        }

        public bool IsInputEmpty()
        {
            return m_currentCalculatorEngine != null && m_currentCalculatorEngine.IsInputEmpty();
        }

        public void SetInHistoryItemLoadMode(bool isHistoryItemLoadMode)
        {
            m_inHistoryItemLoadMode = isHistoryItemLoadMode;
        }

        public List<IExpressionCommand> GetDisplayCommandsSnapshot()
        {
            return m_currentCalculatorEngine?.GetHistoryCollectorCommandsSnapshot() ?? new List<IExpressionCommand>();
        }
    }
}
