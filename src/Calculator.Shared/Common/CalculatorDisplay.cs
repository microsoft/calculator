// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculationManager;
using System;
using System.Collections.Generic;

namespace CalculatorApp
{
    // Callback interface to be implemented by the CalculatorManager
    public class CalculatorDisplay : ICalcDisplay
    {
        private WeakReference m_callbackReference;
        private WeakReference m_historyCallbackReference;


        public CalculatorDisplay()
        {
        }

        public void SetCallback(WeakReference callbackReference)
        {
            m_callbackReference = callbackReference;
        }

        public void SetHistoryCallback(WeakReference callbackReference)
        {
            m_historyCallbackReference = callbackReference;
        }

        public void SetPrimaryDisplay(string displayStringValue,  bool isError)
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.SetPrimaryDisplay(displayStringValue, isError);
                }
            }
        }

        public void SetParenthesisNumber(int parenthesisCount)
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.SetParenthesisCount(parenthesisCount);
                }
            }
        }

        public void OnNoRightParenAdded()
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.OnNoRightParenAdded();
                }
            }
        }

        public void SetIsInError(bool isError)
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.IsInError = isError;
                }
            }
        }

        public void SetExpressionDisplay(
             CalculatorList<(string, int)> tokens,
             CalculatorList<IExpressionCommand> commands)
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.SetExpressionDisplay(tokens, commands);
                }
            }
        }

        public void SetMemorizedNumbers(List<string> newMemorizedNumbers)
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.SetMemorizedNumbers(newMemorizedNumbers);
                }
            }
        }

        public void OnHistoryItemAdded(int addedItemIndex)
        {
            if (m_historyCallbackReference != null)
            {
                if (m_historyCallbackReference.Target is ViewModel.HistoryViewModel historyVM)
                {
                    historyVM.OnHistoryItemAdded(addedItemIndex);
                }
            }
        }

        public void MaxDigitsReached()
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.OnMaxDigitsReached();
                }
            }
        }

        public void BinaryOperatorReceived()
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.OnBinaryOperatorReceived();
                }
            }
        }

        public void MemoryItemChanged(int indexOfMemory)
        {
            if (m_callbackReference != null)
            {
                if (m_callbackReference.Target is ViewModel.StandardCalculatorViewModel calcVM)
                {
                    calcVM.OnMemoryItemChanged(indexOfMemory);
                }
            }
        }
    }
}
