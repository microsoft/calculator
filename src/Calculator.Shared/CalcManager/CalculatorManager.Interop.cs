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
		public static extern int CalculatorManager_Create(ref CalculatorManager_CreateParams parms);

		[DllImport("CalcManager")]
		public static extern void CalculatorManager_SendCommand(int instance, Command command);

		private delegate int GetCEngineStringFunc(int state, string id);
		private delegate void BinaryOperatorReceivedFunc(int state);
		private delegate void SetPrimaryDisplayCallbackFunc(int state, string displayStringValue, bool isError);
		private delegate void SetIsInErrorCallbackFunc(int state, bool isError);
		private delegate void SetParenthesisNumberCallbackFunc(int state, int parenthesisCount);

		private delegate void MaxDigitsReachedCallbackFunc(int state);
		private delegate void MemoryItemChangedCallbackFunc(int state, int indexOfMemory);
		private delegate void OnHistoryItemAddedCallbackFunc(int state, int addedItemIndex);
		private delegate void OnNoRightParenAddedCallbackFunc(int state);
		private delegate void SetExpressionDisplayCallbackFunc(int state);
		private delegate void SetMemorizedNumbersCallbackFunc(int state, string[] newMemorizedNumbers);

		private static GetCEngineStringFunc _getCEngineStringCallback = GetCEngineStringCallback;
		private static BinaryOperatorReceivedFunc _binaryOperatorReceivedCallback = BinaryOperatorReceivedCallback;
		private static SetPrimaryDisplayCallbackFunc _setPrimaryDisplayCallback = SetPrimaryDisplayCallback;
		private static SetIsInErrorCallbackFunc _setIsInErrorCallback = SetIsInErrorCallback;
		private static SetParenthesisNumberCallbackFunc _setParenthesisNumberCallback = SetParenthesisNumberCallback;

		private static MaxDigitsReachedCallbackFunc _maxDigitsReachedCallback = MaxDigitsReachedCallback;
		private static MemoryItemChangedCallbackFunc _memoryItemChangedCallback = MemoryItemChangedCallback;
		private static OnHistoryItemAddedCallbackFunc _onHistoryItemAddedCallback = OnHistoryItemAddedCallback;
		private static OnNoRightParenAddedCallbackFunc _onNoRightParenAddedCallback = OnNoRightParenAddedCallback;
		private static SetExpressionDisplayCallbackFunc _setExpressionDisplayCallback = SetExpressionDisplayCallback;
		private static SetMemorizedNumbersCallbackFunc _setMemorizedNumbersCallback = SetMemorizedNumbersCallback;

		public static void MaxDigitsReachedCallback(int state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.MaxDigitsReached();

			Debug.WriteLine($"CalculatorManager.MaxDigitsReachedCallback");
		}

		public static void MemoryItemChangedCallback(int state, int indexOfMemory)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.MemoryItemChanged(indexOfMemory);

			Debug.WriteLine($"CalculatorManager.MemoryItemChangedCallback({indexOfMemory})");
		}

		public static void OnHistoryItemAddedCallback(int state, int addedItemIndex)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.OnHistoryItemAdded(addedItemIndex);

			Debug.WriteLine($"CalculatorManager.OnHistoryItemAddedCallback({addedItemIndex})");
		}

		public static void OnNoRightParenAddedCallback(int state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.OnNoRightParenAdded();

			Debug.WriteLine($"CalculatorManager.OnNoRightParenAddedCallback");
		}

		public static void SetExpressionDisplayCallback(int state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			// manager.SetExpressionDisplay();

			Debug.WriteLine($"CalculatorManager.SetExpressionDisplayCallback");
		}

		public static void SetMemorizedNumbersCallback(int state, string[] newMemorizedNumbers)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetMemorizedNumbers(newMemorizedNumbers.ToList());

			Debug.WriteLine($"CalculatorManager.SetMemorizedNumbersCallback({string.Join(";", newMemorizedNumbers)})");
		}

		public static void SetParenthesisNumberCallback(int state, int parenthesisCount)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetParenthesisNumber(parenthesisCount);

			Debug.WriteLine($"CalculatorManager.SetParenthesisNumberCallback({parenthesisCount})");
		}

		public static void BinaryOperatorReceivedCallback(int state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.BinaryOperatorReceived();

			Debug.WriteLine($"CalculatorManager.BinaryOperatorReceivedCallback");
		}

		public static void SetPrimaryDisplayCallback(int state, string displayStringValue, bool isError)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetPrimaryDisplay(displayStringValue, isError);

			Debug.WriteLine($"CalculatorManager.SetPrimaryDisplayCallback({displayStringValue}, {isError})");
		}

		public static void SetIsInErrorCallback(int state, bool isError)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetIsInError(isError);

			Debug.WriteLine($"CalculatorManager.SetIsInErrorCallback({isError})");
		}

		public static int GetCEngineStringCallback(int state, string resourceId)
		{
			var provider = GCHandle.FromIntPtr((IntPtr)state).Target as EngineResourceProvider;
			var ret = provider.GetCEngineString(resourceId) ?? "";

			var retBytes = Encoding.UTF8.GetBytes(ret);
			var retPtr = Marshal.AllocHGlobal(retBytes.Length + 1);
			Marshal.WriteByte(retPtr + retBytes.Length, 0);
			Marshal.Copy(retBytes, 0, retPtr, retBytes.Length);

			Debug.WriteLine($"CalculatorManager.GetCEngineStringCallback({resourceId},{ret})");

			return (int)retPtr;
		}
	}

    public partial class CalculatorManager : ICalcDisplay
    {

		private GCHandle _displayCallbackHandle;
		private GCHandle _resourceProviderHandle;
		private readonly int _nativeManager;

	}
}
