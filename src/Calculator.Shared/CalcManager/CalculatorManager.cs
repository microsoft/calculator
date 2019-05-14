// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using CalculatorApp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace CalculationManager
{

    public enum CalculatorPrecision
    {
        StandardModePrecision = 16,
        ScientificModePrecision = 32,
        ProgrammerModePrecision = 64
    };

    // Numbering continues from the Enum Command from Command.h
    // with some gap to ensure there is no overlap of these ids
    // when (unsigned char) is performed on these ids
    // they shouldn't fall in any number range greater than 80. So never
    // make the memory command ids go below 330
    public enum MemoryCommand
    {
        MemorizeNumber = 330,
        MemorizedNumberLoad = 331,
        MemorizedNumberAdd = 332,
        MemorizedNumberSubtract = 333,
        MemorizedNumberClearAll = 334,
        MemorizedNumberClear = 335
    };

    public struct HISTORYITEMVECTOR
    {
        public CalculatorList<(string, int)> spTokens;
        public CalculatorList<IExpressionCommand> spCommands;
        public string expression;
        public string result;
    };

    public struct HISTORYITEM
    {
        public HISTORYITEMVECTOR historyItemVector;
    };

    public interface ICalcDisplay
    {
        void SetPrimaryDisplay(string pszText, bool isError);
        void SetIsInError(bool isInError);
        void SetExpressionDisplay(
             CalculatorList<(string, int)> tokens,
             CalculatorList<IExpressionCommand> commands);
        void SetParenthesisNumber( int  count);
        void OnNoRightParenAdded();
        void MaxDigitsReached(); // not an error but still need to inform UI layer.
        void BinaryOperatorReceived();
        void OnHistoryItemAdded( int  addedItemIndex);
        void SetMemorizedNumbers(List<string> memorizedNumbers);
        void MemoryItemChanged(int  indexOfMemory);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CalculatorManager_CreateParams
    {
        public IntPtr CalculatorState;

        public IntPtr SetPrimaryDisplay;
        public IntPtr SetIsInError;
        public IntPtr SetExpressionDisplay;
        public IntPtr SetParenthesisNumber;
        public IntPtr OnNoRightParenAdded;
        public IntPtr MaxDigitsReached;
        public IntPtr BinaryOperatorReceived;
        public IntPtr OnHistoryItemAdded;
        public IntPtr SetMemorizedNumbers;
        public IntPtr MemoryItemChanged;

        public IntPtr ResourceState;

        public IntPtr GetCEngineString;
    };

    public partial class CalculatorManager : ICalcDisplay
    {
        public void SetPrimaryDisplay(string displayString, bool isError) => throw new NotImplementedException();
        public void SetIsInError(bool isError) => throw new NotImplementedException();
        public void SetExpressionDisplay(
             CalculatorList<(string, int)> tokens,
             CalculatorList<IExpressionCommand> commands) => throw new NotImplementedException();
        public void SetMemorizedNumbers(List<string> memorizedNumbers) => throw new NotImplementedException();
        public void OnHistoryItemAdded(int addedItemIndex) => throw new NotImplementedException();
        public void SetParenthesisNumber(int parenthesisCount) => throw new NotImplementedException();
        public void OnNoRightParenAdded() => throw new NotImplementedException();
        public void DisplayPasteError() => throw new NotImplementedException();
        public void MaxDigitsReached() => throw new NotImplementedException();
        public void BinaryOperatorReceived() => throw new NotImplementedException();
        public void MemoryItemChanged(int indexOfMemory) => throw new NotImplementedException();

        public CalculatorManager(ref CalculatorDisplay displayCallback, ref EngineResourceProvider resourceProvider)
        {
            Debug.WriteLine($"new CalculatorManager");
            displayCallback = new CalculatorDisplay();
            resourceProvider = new EngineResourceProvider();

            _displayCallbackHandle = GCHandle.Alloc(displayCallback);
            _resourceProviderHandle = GCHandle.Alloc(resourceProvider);

            var p = new CalculatorManager_CreateParams
            {
                CalculatorState = GCHandle.ToIntPtr(_displayCallbackHandle),
                GetCEngineString = Marshal.GetFunctionPointerForDelegate(_getCEngineStringCallback),

                ResourceState = GCHandle.ToIntPtr(_resourceProviderHandle),
                BinaryOperatorReceived = Marshal.GetFunctionPointerForDelegate(_binaryOperatorReceivedCallback),
                SetPrimaryDisplay = Marshal.GetFunctionPointerForDelegate(_setPrimaryDisplayCallback),
                SetIsInError = Marshal.GetFunctionPointerForDelegate(_setIsInErrorCallback),
                SetParenthesisNumber = Marshal.GetFunctionPointerForDelegate(_setParenthesisNumberCallback),
                MaxDigitsReached = Marshal.GetFunctionPointerForDelegate(_maxDigitsReachedCallback),
                MemoryItemChanged = Marshal.GetFunctionPointerForDelegate(_memoryItemChangedCallback),
                OnHistoryItemAdded = Marshal.GetFunctionPointerForDelegate(_onHistoryItemAddedCallback),
                OnNoRightParenAdded = Marshal.GetFunctionPointerForDelegate(_onNoRightParenAddedCallback),
                SetExpressionDisplay = Marshal.GetFunctionPointerForDelegate(_setExpressionDisplayCallback),
                SetMemorizedNumbers = Marshal.GetFunctionPointerForDelegate(_setMemorizedNumbersCallback),
            };

            _nativeManager = CalculatorManager_Create(ref p);
        }

        public void Reset(bool clearMemory = true) => throw new NotImplementedException();
        public void SetStandardMode() => throw new NotImplementedException();
        public void SetScientificMode() => throw new NotImplementedException();
        public void SetProgrammerMode() => throw new NotImplementedException();
        public void SendCommand(Command command)
        {
            Debug.WriteLine($"CalculatorManager.SendCommand({command})");

            CalculatorManager_SendCommand(_nativeManager, command);
        }

        public List<char> SerializeCommands() => throw new NotImplementedException();
        public void DeSerializeCommands(List<char> serializedData) => throw new NotImplementedException();
        public void SerializeMemory() => throw new NotImplementedException();
        public List<long> GetSerializedMemory() => throw new NotImplementedException();
        public void DeSerializeMemory(List<long> serializedMemory) => throw new NotImplementedException();
        public void SerializePrimaryDisplay() => throw new NotImplementedException();
        public List<long> GetSerializedPrimaryDisplay() => throw new NotImplementedException();
        public void DeSerializePrimaryDisplay(List<long> serializedPrimaryDisplay) => throw new NotImplementedException();
        public Command SerializeSavedDegreeMode() => throw new NotImplementedException();

        public void MemorizeNumber() => throw new NotImplementedException();
        public void MemorizedNumberLoad(int value)
        {
            Debug.WriteLine($"CalculatorManager.MemorizedNumberLoad({value})");
        }
        public void MemorizedNumberAdd(int value)
        {
            Debug.WriteLine($"CalculatorManager.MemorizedNumberAdd({value})");
        }
        public void MemorizedNumberSubtract(int value)
        {
            Debug.WriteLine($"CalculatorManager.MemorizedNumberSubtract({value})");
        }
        public void MemorizedNumberClear(int value)
        {
            Debug.WriteLine($"CalculatorManager.MemorizedNumberClear({value})");
        }
        public void MemorizedNumberClearAll()
        {
            Debug.WriteLine($"CalculatorManager.MemorizedNumberClearAll()");
        }

        public bool IsEngineRecording() => throw new NotImplementedException();
        public List<char> GetSavedCommands() => throw new NotImplementedException();
        public void SetRadix(RADIX_TYPE iRadixType)
        {
            Debug.WriteLine($"CalculatorManager.SetRadix({iRadixType})");
        }
        public void SetMemorizedNumbersString() => throw new NotImplementedException();
        public string GetResultForRadix(int radix, int precision) => throw new NotImplementedException();
        public void SetPrecision(int precision)
        {
            Debug.WriteLine($"CalculatorManager.SetPrecision({precision})");
        }
        public void UpdateMaxIntDigits()
        {
            Debug.WriteLine($"CalculatorManager.UpdateMaxIntDigits()");
        }
        public char DecimalSeparator() => throw new NotImplementedException();

        public List<HISTORYITEM> GetHistoryItems() => throw new NotImplementedException();

        public List<HISTORYITEM> GetHistoryItems(CalculationManager.CALCULATOR_MODE mode)
        {
            Debug.WriteLine($"CalculatorManager.GetHistoryItems({mode})");

            return new List<HISTORYITEM>();
        }

        public HISTORYITEM GetHistoryItem(int uIdx) => throw new NotImplementedException();
        public bool RemoveHistoryItem(int uIdx) => throw new NotImplementedException();
        public void ClearHistory() => throw new NotImplementedException();
        public int MaxHistorySize() => throw new NotImplementedException();
        public CalculationManager.Command GetCurrentDegreeMode() => throw new NotImplementedException();
        public void SetHistory(CALCULATOR_MODE eMode, List<HISTORYITEM> history) => throw new NotImplementedException();
        public void SetInHistoryItemLoadMode(bool isHistoryItemLoadMode) => throw new NotImplementedException();
    }
}
