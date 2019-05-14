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
    public partial class CalculatorManager : ICalcDisplay
    {
        [DllImport("CalcManager")]
        public static extern IntPtr CalculatorManager_Create(ref CalculatorManager_CreateParams parms);

        [DllImport("CalcManager")]
        public static extern void CalculatorManager_SendCommand(IntPtr instance, Command command);

        private delegate IntPtr GetCEngineStringFunc(IntPtr state, string id);
        private delegate void BinaryOperatorReceivedFunc(IntPtr state);
        private delegate void SetPrimaryDisplayCallbackFunc(IntPtr state, string displayStringValue, bool isError);
        private delegate void SetIsInErrorCallbackFunc(IntPtr state, bool isError);
        private delegate void SetParenthesisNumberCallbackFunc(IntPtr state, int parenthesisCount);

        private delegate void MaxDigitsReachedCallbackFunc(IntPtr state);
        private delegate void MemoryItemChangedCallbackFunc(IntPtr state, int indexOfMemory);
        private delegate void OnHistoryItemAddedCallbackFunc(IntPtr state, int addedItemIndex);
        private delegate void OnNoRightParenAddedCallbackFunc(IntPtr state);
        private delegate void SetExpressionDisplayCallbackFunc(IntPtr state);
        private delegate void SetMemorizedNumbersCallbackFunc(IntPtr state, string[] newMemorizedNumbers);

        private static GetCEngineStringFunc _getCEngineStringCallback = GetCEngineStringCallback;
        private static BinaryOperatorReceivedFunc _binaryOperatorReceivedCallback = BinaryOperatorReceivedCallback;
        private static SetPrimaryDisplayCallbackFunc _setPrimaryDisplayCallback = SetPrimaryDisplayCallback;
        private static SetIsInErrorCallbackFunc _setIsInErrorCallback = SetIsInErrorCallback;
        private static SetParenthesisNumberCallbackFunc _setParenthesisNumberCallback = SetParenthesisNumberCallback;

        private static MaxDigitsReachedCallbackFunc _maxDigitsReachedCallback           = MaxDigitsReachedCallback;
        private static MemoryItemChangedCallbackFunc _memoryItemChangedCallback         = MemoryItemChangedCallback;
        private static OnHistoryItemAddedCallbackFunc _onHistoryItemAddedCallback       = OnHistoryItemAddedCallback;
        private static OnNoRightParenAddedCallbackFunc _onNoRightParenAddedCallback     = OnNoRightParenAddedCallback;
        private static SetExpressionDisplayCallbackFunc _setExpressionDisplayCallback   = SetExpressionDisplayCallback;
        private static SetMemorizedNumbersCallbackFunc _setMemorizedNumbersCallback     = SetMemorizedNumbersCallback;

        private GCHandle _displayCallbackHandle;
        private GCHandle _resourceProviderHandle;
        private readonly IntPtr _nativeManager;

        private static void MaxDigitsReachedCallback(IntPtr state)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.MaxDigitsReached();

            Debug.WriteLine($"CalculatorManager.MaxDigitsReachedCallback");
        }

        private static void MemoryItemChangedCallback(IntPtr state, int indexOfMemory)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.MemoryItemChanged(indexOfMemory);

            Debug.WriteLine($"CalculatorManager.MemoryItemChangedCallback({indexOfMemory})");
        }

        private static void OnHistoryItemAddedCallback(IntPtr state, int addedItemIndex)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.OnHistoryItemAdded(addedItemIndex);

            Debug.WriteLine($"CalculatorManager.OnHistoryItemAddedCallback({addedItemIndex})");
        }

        private static void OnNoRightParenAddedCallback(IntPtr state)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.OnNoRightParenAdded();

            Debug.WriteLine($"CalculatorManager.OnNoRightParenAddedCallback");
        }

        private static void SetExpressionDisplayCallback(IntPtr state)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            // manager.SetExpressionDisplay();

            Debug.WriteLine($"CalculatorManager.SetExpressionDisplayCallback");
        }

        private static void SetMemorizedNumbersCallback(IntPtr state, string[] newMemorizedNumbers)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.SetMemorizedNumbers(newMemorizedNumbers.ToList());

            Debug.WriteLine($"CalculatorManager.SetMemorizedNumbersCallback({string.Join(";", newMemorizedNumbers)})");
        }

        private static void SetParenthesisNumberCallback(IntPtr state, int parenthesisCount)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.SetParenthesisNumber(parenthesisCount);

            Debug.WriteLine($"CalculatorManager.SetParenthesisNumberCallback({parenthesisCount})");
        }

        private static void BinaryOperatorReceivedCallback(IntPtr state)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.BinaryOperatorReceived();

            Debug.WriteLine($"CalculatorManager.BinaryOperatorReceivedCallback");
        }

        private static void SetPrimaryDisplayCallback(IntPtr state, string displayStringValue, bool isError)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.SetPrimaryDisplay(displayStringValue, isError);

            Debug.WriteLine($"CalculatorManager.SetPrimaryDisplayCallback({displayStringValue}, {isError})");
        }

        private static void SetIsInErrorCallback(IntPtr state, bool isError)
        {
            var manager = GCHandle.FromIntPtr(state).Target as CalculatorDisplay;
            manager.SetIsInError(isError);

            Debug.WriteLine($"CalculatorManager.SetIsInErrorCallback({isError})");
        }

        private static IntPtr GetCEngineStringCallback(IntPtr state, string resourceId)
        {
            var provider = GCHandle.FromIntPtr(state).Target as EngineResourceProvider;
            var ret = provider.GetCEngineString(resourceId) ?? "";

            var retBytes = Encoding.UTF8.GetBytes(ret);
            var retPtr = Marshal.AllocHGlobal(retBytes.Length+1);
            Marshal.WriteByte(retPtr + retBytes.Length, 0);
            Marshal.Copy(retBytes, 0, retPtr, retBytes.Length);

            Debug.WriteLine($"CalculatorManager.GetCEngineStringCallback({resourceId},{ret})");

            return retPtr;
        }
    }
}
