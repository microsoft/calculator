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
		public static extern IntPtr CalculatorManager_DecimalSeparator(IntPtr nativeManager);

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
		public static extern int CBinaryCommand_GetCommand(IntPtr m_pExpressionCommand);

		[DllImport(DllPath)]
		public static extern void Free(IntPtr ptr);

		[DllImport(DllPath)]
		public static extern CommandType IExpressionCommand_GetCommandType(IntPtr pExpressionCommand);

		[DllImport(DllPath)]
		public static extern bool COpndCommand_IsNegative(IntPtr pExpressionCommand);

		[DllImport(DllPath)]
		public static extern IntPtr COpndCommand_GetCommands(IntPtr pExpressionCommand);

		[DllImport(DllPath)]
		public static extern IntPtr CUnaryCommand_GetCommands(IntPtr pExpressionCommand);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate IntPtr GetCEngineStringFunc(IntPtr state, IntPtr id);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void BinaryOperatorReceivedFunc(IntPtr state);
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void SetPrimaryDisplayCallbackFunc(IntPtr state, IntPtr pDisplayStringValue, bool isError);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void SetIsInErrorCallbackFunc(IntPtr state, bool isError);
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void SetParenthesisNumberCallbackFunc(IntPtr state, int parenthesisCount);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void MaxDigitsReachedCallbackFunc(IntPtr state);
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void MemoryItemChangedCallbackFunc(IntPtr state, int indexOfMemory);
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void OnHistoryItemAddedCallbackFunc(IntPtr state, int addedItemIndex);
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void OnNoRightParenAddedCallbackFunc(IntPtr state);
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void SetExpressionDisplayCallbackFunc(IntPtr state, IntPtr data);
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
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

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(MaxDigitsReachedCallbackFunc))]
#endif
		public static void MaxDigitsReachedCallback(IntPtr state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.MaxDigitsReached();

			DebugTrace($"CalculatorManager.MaxDigitsReachedCallback");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(MemoryItemChangedCallbackFunc))]
#endif
		public static void MemoryItemChangedCallback(IntPtr state, int indexOfMemory)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.MemoryItemChanged(indexOfMemory);

			DebugTrace($"CalculatorManager.MemoryItemChangedCallback({indexOfMemory})");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(OnHistoryItemAddedCallbackFunc))]
#endif
		public static void OnHistoryItemAddedCallback(IntPtr state, int addedItemIndex)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.OnHistoryItemAdded(addedItemIndex);

			DebugTrace($"CalculatorManager.OnHistoryItemAddedCallback({addedItemIndex})");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(OnNoRightParenAddedCallbackFunc))]
#endif
		public static void OnNoRightParenAddedCallback(IntPtr state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.OnNoRightParenAdded();

			DebugTrace($"CalculatorManager.OnNoRightParenAddedCallback");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(SetExpressionDisplayCallbackFunc))]
#endif
		public static void SetExpressionDisplayCallback(IntPtr state, IntPtr historyItem)
		{
			DebugTrace($"CalculatorManager.SetExpressionDisplayCallback({state}, {historyItem})");

			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;

			var nativeResult = Marshal.PtrToStructure<GetHistoryItemResult>(historyItem);
			var itemResult = CalculatorManager.UnmarshalHistoryItemResult(nativeResult);

			var tokens = string.Join("; ", itemResult.historyItemVector.spTokens.Select(v => $"{v.Item1} : {v.Item2}"));

			DebugTrace($"CalculatorManager.SetExpressionDisplayCallback: Tokens {tokens}");

			manager.SetExpressionDisplay(itemResult.historyItemVector.spTokens, itemResult.historyItemVector.spCommands);

		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(SetMemorizedNumbersCallbackFunc))]
#endif
		public static void SetMemorizedNumbersCallback(IntPtr state, int count, IntPtr newMemorizedNumbers)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;

			var numbers = new List<String>();
			for (int i = 0; i < count; i++)
			{
				var value = PtrToString(Marshal.ReadIntPtr(newMemorizedNumbers, i));
				numbers.Add(value);
			}

			manager.SetMemorizedNumbers(numbers);

			DebugTrace($"CalculatorManager.SetMemorizedNumbersCallback({string.Join(";", numbers)})");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(SetParenthesisNumberCallbackFunc))]
#endif
		public static void SetParenthesisNumberCallback(IntPtr state, int parenthesisCount)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetParenthesisNumber(parenthesisCount);

			DebugTrace($"CalculatorManager.SetParenthesisNumberCallback({parenthesisCount})");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(BinaryOperatorReceivedFunc))]
#endif

		public static void BinaryOperatorReceivedCallback(IntPtr state)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.BinaryOperatorReceived();

			DebugTrace($"CalculatorManager.BinaryOperatorReceivedCallback");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(SetPrimaryDisplayCallbackFunc))]
#endif
		public static void SetPrimaryDisplayCallback(IntPtr state, IntPtr pDisplayStringValue, bool isError)
		{
			var displayStringValue = PtrToString(pDisplayStringValue);

			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetPrimaryDisplay(displayStringValue, isError);

			DebugTrace($"CalculatorManager.SetPrimaryDisplayCallback({displayStringValue}, {isError})");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(SetIsInErrorCallbackFunc))]
#endif
		public static void SetIsInErrorCallback(IntPtr state, bool isError)
		{
			var manager = GCHandle.FromIntPtr((IntPtr)state).Target as CalculatorDisplay;
			manager.SetIsInError(isError);

			DebugTrace($"CalculatorManager.SetIsInErrorCallback({isError})");
		}

#if __IOS__
		[ObjCRuntime.MonoPInvokeCallback(typeof(GetCEngineStringFunc))]
#endif
		public static IntPtr GetCEngineStringCallback(IntPtr state, IntPtr pResourceId)
		{
			var provider = GCHandle.FromIntPtr(state).Target as EngineResourceProvider;

			var resourceId = PtrToString(pResourceId);

			var resourceValue = provider.GetCEngineString(resourceId) ?? "";

			var pEngineString = StringToHGlobal(resourceValue);

			DebugTrace($"GetCEngineStringCallback({resourceId}, {resourceValue})");

			return pEngineString;
		}

		internal static IntPtr StringToHGlobal(string resourceValue)
		{
#if __WASM__ || __IOS__ || __ANDROID__
			// wchar_t is 32bits
			return StringToHGlobalUTF32(resourceValue);
#else
			return Marshal.StringToHGlobalUni(resourceValue);
#endif
		}

		internal static string PtrToString(IntPtr pResourceId)
		{
#if __WASM__ || __IOS__ || __ANDROID__
			return PtrToStringUTF32(pResourceId);
#else
			return Marshal.PtrToStringUni(pResourceId);
#endif
		}

		private static string PtrToStringUTF32(IntPtr ptr)
		{
			var endPtr = ptr;
			while (Marshal.ReadInt32(endPtr) is int c && c != 0)
			{
				endPtr += 4;
			}

			if (endPtr != ptr)
			{
				var b = new byte[(int)endPtr - (int)ptr];
				Marshal.Copy(ptr, b, 0, b.Length);

				return Encoding.UTF32.GetString(b);
			}
			else
			{
				return "";
			}

		}

		private static IntPtr StringToHGlobalUTF32(string resourceValue)
		{
			var ret = Encoding.UTF32.GetBytes(resourceValue);
			var pRet2 = Marshal.AllocHGlobal(resourceValue.Length * 4 + 4);
			Marshal.Copy(ret, 0, pRet2, resourceValue.Length * 4);
			Marshal.WriteInt32(pRet2 + resourceValue.Length * 4, 0);
			return pRet2;
		}

		private static void DebugTrace(string message)
		{
			Debug.WriteLine(message);
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
		public IntPtr expression;
		public IntPtr result;

		public int TokenCount;
		public IntPtr TokenStrings;
		public IntPtr TokenValues;

		public int CommandCount;
		public IntPtr Commands;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct COpndCommand_GetCommandsResult
	{
		public int CommandCount;
		public IntPtr Commands;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct CUnaryCommand_GetCommandsResult
	{
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
