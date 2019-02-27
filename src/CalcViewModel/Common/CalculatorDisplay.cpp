// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// This class provides the concrete implementation for the ICalcDisplay interface
// that is declared in the Calculation Manager Library. 
#include "pch.h"
#include "CalculatorDisplay.h"
#include "StandardCalculatorViewModel.h"

using namespace CalculatorApp;
using namespace CalculationManager;
using namespace std;

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
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->SetPrimaryDisplay(displayStringValue, isError);
        }
    }
}

void CalculatorDisplay::SetParenDisplayText(_In_ const std::wstring& parenthesisCount)
{
    if (m_callbackReference != nullptr)
    {
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->SetParenthesisCount(parenthesisCount);
        }
    }
}

void CalculatorDisplay::SetIsInError(bool isError)
{
    if (m_callbackReference != nullptr)
    {
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->IsInError = isError;
        }
    }
}

void CalculatorDisplay::SetExpressionDisplay(_Inout_ std::shared_ptr<CalculatorVector<std::pair<std::wstring, int>>> const &tokens, _Inout_ std::shared_ptr<CalculatorVector <std::shared_ptr<IExpressionCommand>>> const &commands)
{
    if (m_callbackReference != nullptr)
    {
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->SetExpressionDisplay(tokens, commands);
        }
    }
}

void CalculatorDisplay::SetMemorizedNumbers(_In_ const vector<std::wstring>& newMemorizedNumbers)
{
    if (m_callbackReference != nullptr)
    {
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->SetMemorizedNumbers(newMemorizedNumbers);
        }
    }
}

void CalculatorDisplay::OnHistoryItemAdded(_In_ unsigned int addedItemIndex)
{
    if (m_historyCallbackReference != nullptr)
    {
        auto historyVM = m_historyCallbackReference.Resolve<ViewModel::HistoryViewModel>();
        if (historyVM)
        {
            historyVM->OnHistoryItemAdded(addedItemIndex);
        }
    }
}

void CalculatorDisplay::MaxDigitsReached()
{
    if (m_callbackReference != nullptr)
    {
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->OnMaxDigitsReached();
        }
    }
}

void CalculatorDisplay::BinaryOperatorReceived()
{
    if (m_callbackReference != nullptr)
    {
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->OnBinaryOperatorReceived();
        }
    }
}

void CalculatorDisplay::MemoryItemChanged(unsigned int indexOfMemory)
{
    if (m_callbackReference != nullptr)
    {
        auto calcVM = m_callbackReference.Resolve<ViewModel::StandardCalculatorViewModel>();
        if (calcVM)
        {
            calcVM->OnMemoryItemChanged(indexOfMemory);
        }
    }
}
