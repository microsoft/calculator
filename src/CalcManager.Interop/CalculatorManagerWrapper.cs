// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using CalcManager;
using CalcManager.CalculationManager;

namespace CalcManager.Interop
{
    public sealed class CalculatorManagerWrapper
    {
        private class CalcDisplayBridge : ICalcDisplay
        {
            private readonly SetPrimaryDisplayHandler m_onSetPrimaryDisplay;
            private readonly SetIsInErrorHandler m_onSetIsInError;
            private readonly SetExpressionDisplayHandler m_onSetExpressionDisplay;
            private readonly SetParenthesisNumberHandler m_onSetParenthesisNumber;
            private readonly SimpleHandler m_onNoRightParenAdded;
            private readonly SimpleHandler m_onMaxDigitsReached;
            private readonly SimpleHandler m_onBinaryOperatorReceived;
            private readonly OnHistoryItemAddedHandler m_onHistoryItemAdded;
            private readonly SetMemorizedNumbersHandler m_onSetMemorizedNumbers;
            private readonly MemoryItemChangedHandler m_onMemoryItemChanged;
            private readonly SimpleHandler m_onInputChanged;

            public CalcDisplayBridge(
                SetPrimaryDisplayHandler onSetPrimaryDisplay,
                SetIsInErrorHandler onSetIsInError,
                SetExpressionDisplayHandler onSetExpressionDisplay,
                SetParenthesisNumberHandler onSetParenthesisNumber,
                SimpleHandler onNoRightParenAdded,
                SimpleHandler onMaxDigitsReached,
                SimpleHandler onBinaryOperatorReceived,
                OnHistoryItemAddedHandler onHistoryItemAdded,
                SetMemorizedNumbersHandler onSetMemorizedNumbers,
                MemoryItemChangedHandler onMemoryItemChanged,
                SimpleHandler onInputChanged)
            {
                m_onSetPrimaryDisplay = onSetPrimaryDisplay;
                m_onSetIsInError = onSetIsInError;
                m_onSetExpressionDisplay = onSetExpressionDisplay;
                m_onSetParenthesisNumber = onSetParenthesisNumber;
                m_onNoRightParenAdded = onNoRightParenAdded;
                m_onMaxDigitsReached = onMaxDigitsReached;
                m_onBinaryOperatorReceived = onBinaryOperatorReceived;
                m_onHistoryItemAdded = onHistoryItemAdded;
                m_onSetMemorizedNumbers = onSetMemorizedNumbers;
                m_onMemoryItemChanged = onMemoryItemChanged;
                m_onInputChanged = onInputChanged;
            }

            public void SetPrimaryDisplay(string displayString, bool isError)
            {
                m_onSetPrimaryDisplay?.Invoke(displayString, isError);
            }

            public void SetIsInError(bool isError)
            {
                m_onSetIsInError?.Invoke(isError);
            }

            public void SetExpressionDisplay(List<Tuple<string, int>> tokens, List<IExpressionCommand> commands)
            {
                var winrtTokens = new List<HistoryToken>();
                if (tokens != null)
                {
                    foreach (var pair in tokens)
                    {
                        winrtTokens.Add(new HistoryToken(pair.Item1, pair.Item2));
                    }
                }

                var winrtCommands = new List<ExpressionCommandWrapper>();
                if (commands != null)
                {
                    foreach (var cmd in commands)
                    {
                        winrtCommands.Add(new ExpressionCommandWrapper(cmd));
                    }
                }

                m_onSetExpressionDisplay?.Invoke(winrtTokens.ToArray(), winrtCommands.ToArray());
            }

            public void SetParenthesisNumber(uint count)
            {
                m_onSetParenthesisNumber?.Invoke(count);
            }

            public void OnNoRightParenAdded()
            {
                m_onNoRightParenAdded?.Invoke();
            }

            public void MaxDigitsReached()
            {
                m_onMaxDigitsReached?.Invoke();
            }

            public void BinaryOperatorReceived()
            {
                m_onBinaryOperatorReceived?.Invoke();
            }

            public void OnHistoryItemAdded(uint addedItemIndex)
            {
                m_onHistoryItemAdded?.Invoke(addedItemIndex);
            }

            public void SetMemorizedNumbers(List<string> memorizedNumbers)
            {
                m_onSetMemorizedNumbers?.Invoke(memorizedNumbers?.ToArray() ?? Array.Empty<string>());
            }

            public void MemoryItemChanged(uint indexOfMemory)
            {
                m_onMemoryItemChanged?.Invoke(indexOfMemory);
            }

            public void InputChanged()
            {
                m_onInputChanged?.Invoke();
            }
        }

        private class ResourceProviderBridge : IResourceProvider
        {
            private readonly GetCEngineStringHandler m_onGetCEngineString;

            public ResourceProviderBridge(GetCEngineStringHandler onGetCEngineString)
            {
                m_onGetCEngineString = onGetCEngineString;
            }

            public string GetCEngineString(string id)
            {
                return m_onGetCEngineString?.Invoke(id) ?? string.Empty;
            }
        }

        private readonly CalcDisplayBridge m_displayBridge;
        private readonly ResourceProviderBridge m_resourceBridge;
        private readonly CalculatorManager m_manager;

        public CalculatorManagerWrapper(
            SetPrimaryDisplayHandler onSetPrimaryDisplay,
            SetIsInErrorHandler onSetIsInError,
            SetExpressionDisplayHandler onSetExpressionDisplay,
            SetParenthesisNumberHandler onSetParenthesisNumber,
            SimpleHandler onNoRightParenAdded,
            SimpleHandler onMaxDigitsReached,
            SimpleHandler onBinaryOperatorReceived,
            OnHistoryItemAddedHandler onHistoryItemAdded,
            SetMemorizedNumbersHandler onSetMemorizedNumbers,
            MemoryItemChangedHandler onMemoryItemChanged,
            SimpleHandler onInputChanged,
            GetCEngineStringHandler onGetCEngineString)
        {
            m_displayBridge = new CalcDisplayBridge(
                onSetPrimaryDisplay, onSetIsInError, onSetExpressionDisplay,
                onSetParenthesisNumber, onNoRightParenAdded, onMaxDigitsReached,
                onBinaryOperatorReceived, onHistoryItemAdded, onSetMemorizedNumbers,
                onMemoryItemChanged, onInputChanged);
            m_resourceBridge = new ResourceProviderBridge(onGetCEngineString);
            m_manager = new CalculatorManager(m_displayBridge, m_resourceBridge);
        }

        public void Reset(bool clearMemory) => m_manager.Reset(clearMemory);
        public void SetStandardMode() => m_manager.SetStandardMode();
        public void SetScientificMode() => m_manager.SetScientificMode();
        public void SetProgrammerMode() => m_manager.SetProgrammerMode();

        public void SendCommand(CalculatorCommand command)
        {
            m_manager.SendCommand((Command)(int)command);
        }

        public void MemorizeNumber() => m_manager.MemorizeNumber();
        public void MemorizedNumberLoad(uint index) => m_manager.MemorizedNumberLoad(index);
        public void MemorizedNumberAdd(uint index) => m_manager.MemorizedNumberAdd(index);
        public void MemorizedNumberSubtract(uint index) => m_manager.MemorizedNumberSubtract(index);
        public void MemorizedNumberClear(uint index) => m_manager.MemorizedNumberClear(index);
        public void MemorizedNumberClearAll() => m_manager.MemorizedNumberClearAll();

        public bool IsEngineRecording => m_manager.IsEngineRecording();
        public bool IsInputEmpty => m_manager.IsInputEmpty();

        public void SetRadix(int radixType) => m_manager.SetRadix((RadixType)radixType);
        public void SetMemorizedNumbersString() => m_manager.SetMemorizedNumbersString();
        public string GetResultForRadix(uint radix, int precision, bool groupDigitsPerRadix) =>
            m_manager.GetResultForRadix(radix, precision, groupDigitsPerRadix);

        public void SetPrecision(int precision) => m_manager.SetPrecision(precision);
        public void UpdateMaxIntDigits() => m_manager.UpdateMaxIntDigits();
        public char DecimalSeparator => m_manager.DecimalSeparator();

        private static HistoryItemWrapper[] WrapHistoryItems(IEnumerable<HistoryItem> items)
        {
            if (items == null) return Array.Empty<HistoryItemWrapper>();
            return items.Select(item => new HistoryItemWrapper(item)).ToArray();
        }

        public HistoryItemWrapper[] GetHistoryItems() => WrapHistoryItems(m_manager.GetHistoryItems());

        public HistoryItemWrapper[] GetHistoryItemsForMode(CalculatorMode mode) =>
            WrapHistoryItems(m_manager.GetHistoryItems((CalculationManager.CalculatorMode)(int)mode));

        public void SetHistoryItems(HistoryItemWrapper[] historyItems)
        {
            if (historyItems == null) return;
            var nativeItems = historyItems.Select(item => item.ToUnderlying()).ToList();
            m_manager.SetHistoryItems(nativeItems);
        }

        public HistoryItemWrapper GetHistoryItem(uint index)
        {
            var item = m_manager.GetHistoryItem(index);
            return item != null ? new HistoryItemWrapper(item) : null;
        }

        public bool RemoveHistoryItem(uint index) => m_manager.RemoveHistoryItem(index);
        public void ClearHistory() => m_manager.ClearHistory();
        public ulong MaxHistorySize => (ulong)m_manager.MaxHistorySize();

        public CalculatorCommand GetCurrentDegreeMode() => (CalculatorCommand)(int)m_manager.GetCurrentDegreeMode();
        public void SetInHistoryItemLoadMode(bool isHistoryItemLoadMode) => m_manager.SetInHistoryItemLoadMode(isHistoryItemLoadMode);

        public ExpressionCommandWrapper[] GetDisplayCommandsSnapshot()
        {
            var nativeCommands = m_manager.GetDisplayCommandsSnapshot();
            return nativeCommands?.Select(cmd => new ExpressionCommandWrapper(cmd)).ToArray() ?? Array.Empty<ExpressionCommandWrapper>();
        }
    }
}
