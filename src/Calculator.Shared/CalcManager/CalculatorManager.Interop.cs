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
#if __IOS__
		private const string DllPath = "__Internal"; // https://docs.microsoft.com/en-us/xamarin/ios/platform/native-interop
#else
		private const string DllPath = "CalcManager";
#endif

		[DllImport(DllPath)]
		public static extern IntPtr CalculatorManager_Create(ref CalculatorManager_CreateParams parms);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SendCommand(IntPtr nativeManager, Command command);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SetRadix(IntPtr nativeManager, RADIX_TYPE iRadixType);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_Reset(IntPtr nativeManager, bool clearMemory);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SetStandardMode(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SetScientificMode(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SetProgrammerMode(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_MemorizeNumber(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_MemorizedNumberLoad(IntPtr nativeManager, int value);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_MemorizedNumberAdd(IntPtr nativeManager, int value);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_MemorizedNumberSubtract(IntPtr nativeManager, int value);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_MemorizedNumberClear(IntPtr nativeManager, int value);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_MemorizedNumberClearAll(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern bool CalculatorManager_IsEngineRecording(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SetMemorizedNumbersString(IntPtr nativeManager);

		[DllImport(DllPath, CharSet = CharSet.Unicode)]
		public static extern string CalculatorManager_GetResultForRadix(IntPtr nativeManager, int radix, int precision);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SetPrecision(IntPtr nativeManager, int precision);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_UpdateMaxIntDigits(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern char CalculatorManager_DecimalSeparator(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern bool CalculatorManager_RemoveHistoryItem(IntPtr nativeManager, int uIdx);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_ClearHistory(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern int CalculatorManager_MaxHistorySize(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern Command CalculatorManager_GetCurrentDegreeMode(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern void CalculatorManager_SetInHistoryItemLoadMode(IntPtr nativeManager, bool isHistoryItemLoadMode);

		[DllImport(DllPath)]
		public static extern IntPtr CalculatorManager_GetHistoryItems(IntPtr nativeManager);

		[DllImport(DllPath)]
		public static extern IntPtr CalculatorManager_GetHistoryItemsWithMode(IntPtr nativeManager, CALCULATOR_MODE mode);

		[DllImport(DllPath)]
		public static extern IntPtr CalculatorManager_GetHistoryItem(IntPtr nativeManager, int uIdx);

		[DllImport(DllPath)]
		public static extern CommandType IExpressionCommand_GetCommandType(IntPtr pExpressionCommand);

		public delegate IntPtr GetCEngineStringFunc(IntPtr state, IntPtr id);
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
				var value = Marshal.PtrToStringUni(Marshal.ReadIntPtr(newMemorizedNumbers, i));
				numbers.Add(value);
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

		public static void SetPrimaryDisplayCallback(IntPtr state, [MarshalAs(UnmanagedType.LPWStr)] string displayStringValue, bool isError)
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

		public static IntPtr GetCEngineStringCallback(IntPtr state, IntPtr pResourceId)
		{
			var provider = GCHandle.FromIntPtr(state).Target as EngineResourceProvider;
			var resourceId = Marshal.PtrToStringUni(pResourceId);
			var resourceValue = provider.GetCEngineString(resourceId) ?? "";

#if __WASM__
			// wchar_t is 32bits with emscripten
			var pEngineString = StringToHGlobalUTF32(resourceValue);
#else
			var pEngineString = Marshal.StringToHGlobalUni(resourceValue);
#endif

			Debug.WriteLine($"GetCEngineStringCallback({resourceId}, {resourceValue}");

			return pEngineString;
		}

		private static IntPtr StringToHGlobalUTF32(string resourceValue)
		{
			var ret = Encoding.UTF32.GetBytes(resourceValue);
			var pRet2 = Marshal.AllocHGlobal(resourceValue.Length * 4 + 4);
			Marshal.Copy(ret, 0, pRet2, resourceValue.Length * 4);
			Marshal.WriteInt32(pRet2 + resourceValue.Length * 4, 0);
			return pRet2;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct GetHistoryItemsResult
	{
		public int ItemsCount;
		public IntPtr HistoryItems;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct GetHistoryItemResult
	{
		public string expression;
		public string result;

		public int TokenCount;
		public IntPtr TokenStrings;
		public IntPtr TokenValues;

		public int CommandCount;
		public IntPtr Commands;
	}


	public partial class CalculatorManager : ICalcDisplay
    {

		private GCHandle _displayCallbackHandle;
		private GCHandle _resourceProviderHandle;
		private readonly IntPtr _nativeManager;

	}
}
