// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// This class provides the concrete implementation for the ICalcDisplay interface
// that is declared in the Calculation Manager Library.
#include "pch.h"
#include "CalculatorDisplay.h"
#include "StandardCalculatorViewModel.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculationManager;
using namespace Platform;
using namespace std;

namespace CalculatorApp::ViewModel::Common
{
    CalculatorDisplay::CalculatorDisplay()
    {
    }

    void CalculatorDisplay::SetCallback(Platform::WeakReference callbackReference)
    {
        m_callbackReference = callbackReference;
    }

    void CalculatorDisplay::SetHistoryCallback(Platform::WeakReference callbackReference)
    {
        m_historyCallbackReference = callbackReference;
    }

    void CalculatorDisplay::SetPrimaryDisplay(_In_ const wstring& displayStringValue, _In_ bool isError)
    {
        if (m_callbackReference)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->SetPrimaryDisplay(StringReference(displayStringValue.c_str()), isError);
            }
        }
    }

    void CalculatorDisplay::SetParenthesisNumber(_In_ unsigned int parenthesisCount)
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->SetParenthesisCount(parenthesisCount);
            }
        }
    }

    void CalculatorDisplay::OnNoRightParenAdded()
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->OnNoRightParenAdded();
            }
        }
    }

    void CalculatorDisplay::SetIsInError(bool isError)
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->IsInError = isError;
            }
        }
    }

    void CalculatorDisplay::SetExpressionDisplay(
        _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
        _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands)
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->SetExpressionDisplay(tokens, commands);
            }
        }
    }

    void CalculatorDisplay::SetMemorizedNumbers(_In_ const vector<std::wstring>& newMemorizedNumbers)
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->SetMemorizedNumbers(newMemorizedNumbers);
            }
        }
    }

    void CalculatorDisplay::OnHistoryItemAdded(_In_ unsigned int addedItemIndex)
    {
        if (m_historyCallbackReference != nullptr)
        {
            if (auto historyVM = m_historyCallbackReference.Resolve<ViewModel::HistoryViewModel>())
            {
                historyVM->OnHistoryItemAdded(addedItemIndex);
            }
        }
    }

    void CalculatorDisplay::MaxDigitsReached()
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->OnMaxDigitsReached();
            }
        }
    }

    void CalculatorDisplay::BinaryOperatorReceived()
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->OnBinaryOperatorReceived();
            }
        }
    }

    void CalculatorDisplay::MemoryItemChanged(unsigned int indexOfMemory)
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->OnMemoryItemChanged(indexOfMemory);
            }
        }
    }

    void CalculatorDisplay::InputChanged()
    {
        if (m_callbackReference != nullptr)
        {
            if (auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>())
            {
                calcVM->OnInputChanged();
            }
        }
    }
}

