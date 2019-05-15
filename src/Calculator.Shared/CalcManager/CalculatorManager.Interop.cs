// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using CalculatorApp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace CalculationManager
{
	public static class NativeDispatch
	{
		[DllImport("CalcManager")]
		public static extern IntPtr CalculatorManager_Create(ref CalculatorManager_CreateParams parms);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SendCommand(IntPtr nativeManager, Command command);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SetRadix(IntPtr nativeManager, RADIX_TYPE iRadixType);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_Reset(IntPtr nativeManager, bool clearMemory);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SetStandardMode(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SetScientificMode(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SetProgrammerMode(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_MemorizeNumber(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_MemorizedNumberLoad(IntPtr nativeManager, int value);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_MemorizedNumberAdd(IntPtr nativeManager, int value);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_MemorizedNumberSubtract(IntPtr nativeManager, int value);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_MemorizedNumberClear(IntPtr nativeManager, int value);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_MemorizedNumberClearAll(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern bool CalculatorManager_IsEngineRecording(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SetMemorizedNumbersString(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern string CalculatorManager_GetResultForRadix(IntPtr nativeManager, int radix, int precision);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SetPrecision(IntPtr nativeManager, int precision);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_UpdateMaxIntDigits(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern char CalculatorManager_DecimalSeparator(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern bool CalculatorManager_RemoveHistoryItem(IntPtr nativeManager, int uIdx);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_ClearHistory(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern int CalculatorManager_MaxHistorySize(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern Command CalculatorManager_GetCurrentDegreeMode(IntPtr nativeManager);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SetInHistoryItemLoadMode(IntPtr nativeManager, bool isHistoryItemLoadMode);

		public delegate IntPtr GetCEngineStringFunc(IntPtr state, string id);
		public delegate void BinaryOperatorReceivedFunc(IntPtr state);
		public delegate void SetPrimaryDisplayCallbackFunc(IntPtr state, string displayStringValue, bool isError);
		public delegate void SetIsInErrorCallbackFunc(IntPtr state, bool isError);
		public delegate void SetParenthesisNumberCallbackFunc(IntPtr state, int parenthesisCount);

		public delegate void MaxDigitsReachedCallbackFunc(IntPtr state);
		public delegate void MemoryItemChangedCallbackFunc(IntPtr state, int indexOfMemory);
		public delegate void OnHistoryItemAddedCallbackFunc(IntPtr state, int addedItemIndex);
		public delegate void OnNoRightParenAddedCallbackFunc(IntPtr state);
		public delegate void SetExpressionDisplayCallbackFunc(IntPtr state);
		public delegate void SetMemorizedNumbersCallbackFunc(IntPtr state, int count, IntPtr newMemorizedNumbers);

		public static GetCEngineStringFunc _getCEngineStringCallback = GetCEngineStringCallback;
		public static BinaryOperatorReceivedFunc _binaryOperatorReceivedCallback = BinaryOperatorReceivedCallback;
		public static SetPrimaryDisplayCallbackFunc _setPrimaryDisplayCallback = SetPrimaryDisplayCallback;
		public static SetIsInErrorCallbackFunc _setIsInErrorCallback = SetIsInErrorCallback;
		public static SetParenthesisNumberCallbackFunc _setParenthesisNumberCallback = SetParenthesisNumberCallback;

		public static MaxDigitsReachedCallbackFunc _maxDigitsReachedCallback = MaxDigitsReachedCallback;
		public static MemoryItemChangedCallbackFunc _memoryItemChangedCallback = MemoryItemChangedCallback;
		public static OnHistoryItemAddedCallbackFunc _onHistoryItemAddedCallback = OnHistoryItemAddedCallback;
		public static OnNoRightParenAddedCallbackFunc _onNoRightParenAddedCallback = OnNoRightParenAddedCallback;
		public static SetExpressionDisplayCallbackFunc _setExpressionDisplayCallback = SetExpressionDisplayCallback;
		public static SetMemorizedNumbersCallbackFunc _setMemorizedNumbersCallback = SetMemorizedNumbersCallback;

		public static void MaxDigitsReachedCallback(IntPtr state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.MaxDigitsReached();

			Debug.WriteLine($"CalculatorManager.MaxDigitsReachedCallback");
		}

		public static void MemoryItemChangedCallback(IntPtr state, int indexOfMemory)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.MemoryItemChanged(indexOfMemory);

			Debug.WriteLine($"CalculatorManager.MemoryItemChangedCallback({indexOfMemory})");
		}

		public static void OnHistoryItemAddedCallback(IntPtr state, int addedItemIndex)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.OnHistoryItemAdded(addedItemIndex);

			Debug.WriteLine($"CalculatorManager.OnHistoryItemAddedCallback({addedItemIndex})");
		}

		public static void OnNoRightParenAddedCallback(IntPtr state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.OnNoRightParenAdded();

			Debug.WriteLine($"CalculatorManager.OnNoRightParenAddedCallback");
		}

		public static void SetExpressionDisplayCallback(IntPtr state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			// manager.SetExpressionDisplay();

			Debug.WriteLine($"CalculatorManager.SetExpressionDisplayCallback");
		}

		public static void SetMemorizedNumbersCallback(IntPtr state, int count, IntPtr newMemorizedNumbers)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;

			var numbers = new List<String>();
			for (int i = 0; i < count; i++)
			{
				numbers.Add(Marshal.PtrToStringUTF8(Marshal.ReadIntPtr(newMemorizedNumbers, i)));
			}

			manager.SetMemorizedNumbers(numbers);

			Debug.WriteLine($"CalculatorManager.SetMemorizedNumbersCallback({string.Join(";", numbers)})");
		}

		public static void SetParenthesisNumberCallback(IntPtr state, int parenthesisCount)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetParenthesisNumber(parenthesisCount);

			Debug.WriteLine($"CalculatorManager.SetParenthesisNumberCallback({parenthesisCount})");
		}

		public static void BinaryOperatorReceivedCallback(IntPtr state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.BinaryOperatorReceived();

			Debug.WriteLine($"CalculatorManager.BinaryOperatorReceivedCallback");
		}

		public static void SetPrimaryDisplayCallback(IntPtr state, string displayStringValue, bool isError)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetPrimaryDisplay(displayStringValue, isError);

			Debug.WriteLine($"CalculatorManager.SetPrimaryDisplayCallback({displayStringValue}, {isError})");
		}

		public static void SetIsInErrorCallback(IntPtr state, bool isError)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetIsInError(isError);

			Debug.WriteLine($"CalculatorManager.SetIsInErrorCallback({isError})");
		}

		public static IntPtr GetCEngineStringCallback(IntPtr state, string resourceId)
		{
			var provider = GCHandle.FromIntPtr((IntPtr)state).Target as EngineResourceProvider;
			var ret = provider.GetCEngineString(resourceId) ?? "";

			var retBytes = Encoding.UTF8.GetBytes(ret);
			var retPtr = Marshal.AllocHGlobal(retBytes.Length + 1);
			Marshal.WriteByte(retPtr + retBytes.Length, 0);
			Marshal.Copy(retBytes, 0, retPtr, retBytes.Length);

			Debug.WriteLine($"CalculatorManager.GetCEngineStringCallback({resourceId},{ret})");

			return retPtr;
		}
	}

    public partial class CalculatorManager : ICalcDisplay
    {

		private GCHandle _displayCallbackHandle;
		private GCHandle _resourceProviderHandle;
		private readonly IntPtr _nativeManager;

	}
}
