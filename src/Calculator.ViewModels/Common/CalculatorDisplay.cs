// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using CalcManager.Interop;

namespace CalculatorApp.ViewModel.Common
{
    /// <summary>
    /// Forwards display callbacks to the ViewModel via weak references.
    /// This is the C# equivalent of the original C++/CX CalculatorDisplay class.
    /// No longer inherits from a WinRT base class; delegates are passed directly
    /// to CalculatorManagerWrapper instead.
    /// </summary>
    internal class CalculatorDisplay
    {
        private WeakReference<ICalcDisplayTarget> _callbackReference;
        private WeakReference<IHistoryDisplayTarget> _historyCallbackReference;

        public void SetCallback(ICalcDisplayTarget viewModel)
        {
            _callbackReference = new WeakReference<ICalcDisplayTarget>(viewModel);
        }

        public void SetHistoryCallback(IHistoryDisplayTarget historyViewModel)
        {
            _historyCallbackReference = new WeakReference<IHistoryDisplayTarget>(historyViewModel);
        }

        public void SetPrimaryDisplay(string display, bool isError)
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.SetPrimaryDisplay(display, isError);
            }
        }

        public void SetIsInError(bool isError)
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.SetIsInError(isError);
            }
        }

        public void SetExpressionDisplay(HistoryToken[] tokens, ExpressionCommandWrapper[] commands)
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.SetExpressionDisplay(tokens, commands);
            }
        }

        public void SetParenthesisNumber(uint count)
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.SetParenthesisCount(count);
            }
        }

        public void OnNoRightParenAdded()
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.OnNoRightParenAdded();
            }
        }

        public void MaxDigitsReached()
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.OnMaxDigitsReached();
            }
        }

        public void BinaryOperatorReceived()
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.OnBinaryOperatorReceived();
            }
        }

        public void OnHistoryItemAdded(uint addedItemIndex)
        {
            if (_historyCallbackReference != null && _historyCallbackReference.TryGetTarget(out var histVM))
            {
                histVM.OnHistoryItemAdded(addedItemIndex);
            }
        }

        public void SetMemorizedNumbers(string[] memorizedNumbers)
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.SetMemorizedNumbers(memorizedNumbers);
            }
        }

        public void MemoryItemChanged(uint indexOfMemory)
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.OnMemoryItemChanged(indexOfMemory);
            }
        }

        public void InputChanged()
        {
            if (_callbackReference != null && _callbackReference.TryGetTarget(out var vm))
            {
                vm.OnInputChanged();
            }
        }
    }

    /// <summary>
    /// Interface for ViewModels that receive calculator display callbacks.
    /// </summary>
    internal interface ICalcDisplayTarget
    {
        void SetPrimaryDisplay(string display, bool isError);
        void SetIsInError(bool isError);
        void SetExpressionDisplay(HistoryToken[] tokens, ExpressionCommandWrapper[] commands);
        void SetParenthesisCount(uint count);
        void OnNoRightParenAdded();
        void OnMaxDigitsReached();
        void OnBinaryOperatorReceived();
        void SetMemorizedNumbers(string[] memorizedNumbers);
        void OnMemoryItemChanged(uint indexOfMemory);
        void OnInputChanged();
    }

    /// <summary>
    /// Interface for ViewModels that receive history display callbacks.
    /// </summary>
    internal interface IHistoryDisplayTarget
    {
        void OnHistoryItemAdded(uint addedItemIndex);
    }
}
