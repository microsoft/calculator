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
            displayCallback = new CalculatorDisplay();
            resourceProvider = new EngineResourceProvider();

            _displayCallbackHandle = GCHandle.Alloc(displayCallback);
            _resourceProviderHandle = GCHandle.Alloc(resourceProvider);

#if __WASM__
			var rawPtrs = Uno.Foundation.WebAssemblyRuntime.InvokeJS("CalcManager.registerCallbacks()");
			var ptrs = rawPtrs.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries);

			var p = new CalculatorManager_CreateParams
			{
				CalculatorState = GCHandle.ToIntPtr(_displayCallbackHandle),
				ResourceState = GCHandle.ToIntPtr(_resourceProviderHandle),

				GetCEngineString = (IntPtr)int.Parse(ptrs[0]),
				BinaryOperatorReceived = (IntPtr)int.Parse(ptrs[1]),
				SetPrimaryDisplay = (IntPtr)int.Parse(ptrs[2]),
				SetIsInError = (IntPtr)int.Parse(ptrs[3]),
				SetParenthesisNumber = (IntPtr)int.Parse(ptrs[4]),
				MaxDigitsReached = (IntPtr)int.Parse(ptrs[5]),
				MemoryItemChanged = (IntPtr)int.Parse(ptrs[6]),
				OnHistoryItemAdded = (IntPtr)int.Parse(ptrs[7]),
				OnNoRightParenAdded = (IntPtr)int.Parse(ptrs[8]),
				SetExpressionDisplay = (IntPtr)int.Parse(ptrs[9]),
				SetMemorizedNumbers = (IntPtr)int.Parse(ptrs[10]),
			};

#else
            var p = new CalculatorManager_CreateParams
            {
                CalculatorState = GCHandle.ToIntPtr(_displayCallbackHandle),
                GetCEngineString = Marshal.GetFunctionPointerForDelegate(NativeDispatch._getCEngineStringCallback),

                ResourceState = GCHandle.ToIntPtr(_resourceProviderHandle),
                BinaryOperatorReceived = Marshal.GetFunctionPointerForDelegate(NativeDispatch._binaryOperatorReceivedCallback),
                SetPrimaryDisplay = Marshal.GetFunctionPointerForDelegate(NativeDispatch._setPrimaryDisplayCallback),
                SetIsInError = Marshal.GetFunctionPointerForDelegate(NativeDispatch._setIsInErrorCallback),
                SetParenthesisNumber = Marshal.GetFunctionPointerForDelegate(NativeDispatch._setParenthesisNumberCallback),
                MaxDigitsReached = Marshal.GetFunctionPointerForDelegate(NativeDispatch._maxDigitsReachedCallback),
                MemoryItemChanged = Marshal.GetFunctionPointerForDelegate(NativeDispatch._memoryItemChangedCallback),
                OnHistoryItemAdded = Marshal.GetFunctionPointerForDelegate(NativeDispatch._onHistoryItemAddedCallback),
                OnNoRightParenAdded = Marshal.GetFunctionPointerForDelegate(NativeDispatch._onNoRightParenAddedCallback),
                SetExpressionDisplay = Marshal.GetFunctionPointerForDelegate(NativeDispatch._setExpressionDisplayCallback),
                SetMemorizedNumbers = Marshal.GetFunctionPointerForDelegate(NativeDispatch._setMemorizedNumbersCallback),
            };

#endif

            _nativeManager = NativeDispatch.CalculatorManager_Create(ref p);
		}

        public void Reset(bool clearMemory = true)
			=> NativeDispatch.CalculatorManager_Reset(_nativeManager, clearMemory);

		public void SetStandardMode()
			=> NativeDispatch.CalculatorManager_SetStandardMode(_nativeManager);

		public void SetScientificMode()
			=> NativeDispatch.CalculatorManager_SetScientificMode(_nativeManager);

		public void SetProgrammerMode()
			=> NativeDispatch.CalculatorManager_SetProgrammerMode(_nativeManager);

		public void SendCommand(Command command)
			=> NativeDispatch.CalculatorManager_SendCommand(_nativeManager, command);

		public List<char> SerializeCommands() => throw new NotImplementedException();
        public void DeSerializeCommands(List<char> serializedData) => throw new NotImplementedException();
        public void SerializeMemory() => throw new NotImplementedException();
        public List<long> GetSerializedMemory() => throw new NotImplementedException();
        public void DeSerializeMemory(List<long> serializedMemory) => throw new NotImplementedException();
        public void SerializePrimaryDisplay() => throw new NotImplementedException();
        public List<long> GetSerializedPrimaryDisplay() => throw new NotImplementedException();
        public void DeSerializePrimaryDisplay(List<long> serializedPrimaryDisplay) => throw new NotImplementedException();
        public Command SerializeSavedDegreeMode() => throw new NotImplementedException();

        public void MemorizeNumber()
			=> NativeDispatch.CalculatorManager_MemorizeNumber(_nativeManager);

		public void MemorizedNumberLoad(int value)
			=> NativeDispatch.CalculatorManager_MemorizedNumberLoad(_nativeManager, value);

		public void MemorizedNumberAdd(int value)
			=> NativeDispatch.CalculatorManager_MemorizedNumberAdd(_nativeManager, value);

		public void MemorizedNumberSubtract(int value)
			=> NativeDispatch.CalculatorManager_MemorizedNumberSubtract(_nativeManager, value);

		public void MemorizedNumberClear(int value)
			=> NativeDispatch.CalculatorManager_MemorizedNumberClear(_nativeManager, value);

		public void MemorizedNumberClearAll()
			=> NativeDispatch.CalculatorManager_MemorizedNumberClearAll(_nativeManager);

		public bool IsEngineRecording()
			=> NativeDispatch.CalculatorManager_IsEngineRecording(_nativeManager);

		public List<char> GetSavedCommands()
			=> throw new NotImplementedException();

        public void SetRadix(RADIX_TYPE iRadixType)
			=> NativeDispatch.CalculatorManager_SetRadix(_nativeManager, iRadixType);

		public void SetMemorizedNumbersString()
			=> NativeDispatch.CalculatorManager_SetMemorizedNumbersString(_nativeManager);

		public string GetResultForRadix(int radix, int precision)
			=> NativeDispatch.CalculatorManager_GetResultForRadix(_nativeManager, radix, precision);

		public void SetPrecision(int precision)
			=> NativeDispatch.CalculatorManager_SetPrecision(_nativeManager, precision);

		public void UpdateMaxIntDigits()
			=> NativeDispatch.CalculatorManager_UpdateMaxIntDigits(_nativeManager);

		public char DecimalSeparator()
			=> NativeDispatch.CalculatorManager_DecimalSeparator(_nativeManager);

		public List<HISTORYITEM> GetHistoryItems()
		{
			var pResult = NativeDispatch.CalculatorManager_GetHistoryItems(_nativeManager);
			return UnmarshalHistoryItemsResult(pResult);
		}

		public List<HISTORYITEM> GetHistoryItems(CalculationManager.CALCULATOR_MODE mode)
		{
			var pResult = NativeDispatch.CalculatorManager_GetHistoryItemsWithMode(_nativeManager, mode);
			return UnmarshalHistoryItemsResult(pResult);
		}

		private static List<HISTORYITEM> UnmarshalHistoryItemsResult(IntPtr pResult)
		{
			var result = Marshal.PtrToStructure<GetHistoryItemsResult>(pResult);
			var output = new List<HISTORYITEM>();

			for (var i = 0; i < result.ItemsCount; i++)
			{
				var historyResultItem = Marshal.PtrToStructure<GetHistoryItemResult>(Marshal.ReadIntPtr(result.HistoryItems, i * Marshal.SizeOf<IntPtr>()));

				var historyItem = UnmarshalHistoryItemResult(historyResultItem);

				output.Add(historyItem);
			}

			return output;
		}

		private static HISTORYITEM UnmarshalHistoryItemResult(GetHistoryItemResult historyResultItem)
		{
			var historyItem = new HISTORYITEM();
			historyItem.historyItemVector.expression = historyResultItem.expression;
			historyItem.historyItemVector.result = historyResultItem.result;
			historyItem.historyItemVector.spTokens = new CalculatorList<(string, int)>();

			for (var i = 0; i < historyResultItem.TokenCount; i++)
			{
				var tokenString = Marshal.PtrToStringUni(Marshal.ReadIntPtr(historyResultItem.TokenStrings, i * Marshal.SizeOf<IntPtr>()));

				var tokenValue = Marshal.ReadInt32(historyResultItem.TokenValues, i * Marshal.SizeOf<int>());

				historyItem.historyItemVector.spTokens.Append((tokenString, tokenValue));
			}

			return historyItem;
		}

		public HISTORYITEM GetHistoryItem(int uIdx)
		{
			var pResult = NativeDispatch.CalculatorManager_GetHistoryItem(_nativeManager, uIdx);
			var result = Marshal.PtrToStructure<GetHistoryItemResult>(pResult);

			return UnmarshalHistoryItemResult(result);
		}

		public bool RemoveHistoryItem(int uIdx)
			=> NativeDispatch.CalculatorManager_RemoveHistoryItem(_nativeManager, uIdx);

		public void ClearHistory()
			=> NativeDispatch.CalculatorManager_ClearHistory(_nativeManager);

		public int MaxHistorySize()
			=> NativeDispatch.CalculatorManager_MaxHistorySize(_nativeManager);

		public CalculationManager.Command GetCurrentDegreeMode()
			=> NativeDispatch.CalculatorManager_GetCurrentDegreeMode(_nativeManager);

		public void SetHistory(CALCULATOR_MODE eMode, List<HISTORYITEM> history) => throw new NotImplementedException();

        public void SetInHistoryItemLoadMode(bool isHistoryItemLoadMode)
			=> NativeDispatch.CalculatorManager_SetInHistoryItemLoadMode(_nativeManager, isHistoryItemLoadMode);
	}
}
