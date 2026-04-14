// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalcManagerInterop.h"
#include "HistoryToken.h"


namespace winrt::CalcManager::Interop::implementation
{
    // ---- CalcDisplayBridge ----

    CalcDisplayBridge::CalcDisplayBridge(
        CalcManager::Interop::SetPrimaryDisplayHandler const& onSetPrimaryDisplay,
        CalcManager::Interop::SetIsInErrorHandler const& onSetIsInError,
        CalcManager::Interop::SetExpressionDisplayHandler const& onSetExpressionDisplay,
        CalcManager::Interop::SetParenthesisNumberHandler const& onSetParenthesisNumber,
        CalcManager::Interop::SimpleHandler const& onNoRightParenAdded,
        CalcManager::Interop::SimpleHandler const& onMaxDigitsReached,
        CalcManager::Interop::SimpleHandler const& onBinaryOperatorReceived,
        CalcManager::Interop::OnHistoryItemAddedHandler const& onHistoryItemAdded,
        CalcManager::Interop::SetMemorizedNumbersHandler const& onSetMemorizedNumbers,
        CalcManager::Interop::MemoryItemChangedHandler const& onMemoryItemChanged,
        CalcManager::Interop::SimpleHandler const& onInputChanged)
        : m_onSetPrimaryDisplay(onSetPrimaryDisplay)
        , m_onSetIsInError(onSetIsInError)
        , m_onSetExpressionDisplay(onSetExpressionDisplay)
        , m_onSetParenthesisNumber(onSetParenthesisNumber)
        , m_onNoRightParenAdded(onNoRightParenAdded)
        , m_onMaxDigitsReached(onMaxDigitsReached)
        , m_onBinaryOperatorReceived(onBinaryOperatorReceived)
        , m_onHistoryItemAdded(onHistoryItemAdded)
        , m_onSetMemorizedNumbers(onSetMemorizedNumbers)
        , m_onMemoryItemChanged(onMemoryItemChanged)
        , m_onInputChanged(onInputChanged)
    {
    }

    void CalcDisplayBridge::SetPrimaryDisplay(const std::wstring& pszText, bool isError)
    {
        if (m_onSetPrimaryDisplay) { m_onSetPrimaryDisplay(hstring(pszText), isError); }
    }

    void CalcDisplayBridge::SetIsInError(bool isInError)
    {
        if (m_onSetIsInError) { m_onSetIsInError(isInError); }
    }

    void CalcDisplayBridge::SetExpressionDisplay(
        std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
        std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands)
    {
        std::vector<CalcManager::Interop::HistoryToken> winrtTokens;
        if (tokens)
        {
            for (auto const& [value, cmdIndex] : *tokens)
            {
                auto token = winrt::make<implementation::HistoryToken>();
                token.Value(hstring(value));
                token.CommandIndex(cmdIndex);
                winrtTokens.push_back(token);
            }
        }

        std::vector<CalcManager::Interop::ExpressionCommandWrapper> winrtCommands;
        if (commands)
        {
            for (auto const& cmd : *commands)
            {
                winrtCommands.push_back(winrt::make<ExpressionCommandWrapper>(cmd));
            }
        }

        if (m_onSetExpressionDisplay)
        {
            m_onSetExpressionDisplay(
                com_array<CalcManager::Interop::HistoryToken>(std::move(winrtTokens)),
                com_array<CalcManager::Interop::ExpressionCommandWrapper>(std::move(winrtCommands)));
        }
    }

    void CalcDisplayBridge::SetParenthesisNumber(unsigned int count)
    {
        if (m_onSetParenthesisNumber) { m_onSetParenthesisNumber(count); }
    }

    void CalcDisplayBridge::OnNoRightParenAdded()
    {
        if (m_onNoRightParenAdded) { m_onNoRightParenAdded(); }
    }

    void CalcDisplayBridge::MaxDigitsReached()
    {
        if (m_onMaxDigitsReached) { m_onMaxDigitsReached(); }
    }

    void CalcDisplayBridge::BinaryOperatorReceived()
    {
        if (m_onBinaryOperatorReceived) { m_onBinaryOperatorReceived(); }
    }

    void CalcDisplayBridge::OnHistoryItemAdded(unsigned int addedItemIndex)
    {
        if (m_onHistoryItemAdded) { m_onHistoryItemAdded(addedItemIndex); }
    }

    void CalcDisplayBridge::SetMemorizedNumbers(const std::vector<std::wstring>& memorizedNumbers)
    {
        std::vector<hstring> winrtNumbers;
        winrtNumbers.reserve(memorizedNumbers.size());
        for (auto const& num : memorizedNumbers)
        {
            winrtNumbers.push_back(hstring(num));
        }
        if (m_onSetMemorizedNumbers) { m_onSetMemorizedNumbers(com_array<hstring>(std::move(winrtNumbers))); }
    }

    void CalcDisplayBridge::MemoryItemChanged(unsigned int indexOfMemory)
    {
        if (m_onMemoryItemChanged) { m_onMemoryItemChanged(indexOfMemory); }
    }

    void CalcDisplayBridge::InputChanged()
    {
        if (m_onInputChanged) { m_onInputChanged(); }
    }

    // ---- ResourceProviderBridge ----

    ResourceProviderBridge::ResourceProviderBridge(CalcManager::Interop::GetCEngineStringHandler const& onGetCEngineString)
        : m_onGetCEngineString(onGetCEngineString)
    {
    }

    std::wstring ResourceProviderBridge::GetCEngineString(std::wstring_view id)
    {
        if (m_onGetCEngineString)
        {
            hstring result = m_onGetCEngineString(hstring(id));
            return std::wstring(result);
        }
        return std::wstring();
    }

    // ---- CalculatorManagerWrapper ----

    CalculatorManagerWrapper::CalculatorManagerWrapper(
        CalcManager::Interop::SetPrimaryDisplayHandler const& onSetPrimaryDisplay,
        CalcManager::Interop::SetIsInErrorHandler const& onSetIsInError,
        CalcManager::Interop::SetExpressionDisplayHandler const& onSetExpressionDisplay,
        CalcManager::Interop::SetParenthesisNumberHandler const& onSetParenthesisNumber,
        CalcManager::Interop::SimpleHandler const& onNoRightParenAdded,
        CalcManager::Interop::SimpleHandler const& onMaxDigitsReached,
        CalcManager::Interop::SimpleHandler const& onBinaryOperatorReceived,
        CalcManager::Interop::OnHistoryItemAddedHandler const& onHistoryItemAdded,
        CalcManager::Interop::SetMemorizedNumbersHandler const& onSetMemorizedNumbers,
        CalcManager::Interop::MemoryItemChangedHandler const& onMemoryItemChanged,
        CalcManager::Interop::SimpleHandler const& onInputChanged,
        CalcManager::Interop::GetCEngineStringHandler const& onGetCEngineString)
    {
        m_displayBridge = std::make_unique<CalcDisplayBridge>(
            onSetPrimaryDisplay, onSetIsInError, onSetExpressionDisplay,
            onSetParenthesisNumber, onNoRightParenAdded, onMaxDigitsReached,
            onBinaryOperatorReceived, onHistoryItemAdded, onSetMemorizedNumbers,
            onMemoryItemChanged, onInputChanged);
        m_resourceBridge = std::make_unique<ResourceProviderBridge>(onGetCEngineString);
        m_manager = std::make_unique<CalculationManager::CalculatorManager>(m_displayBridge.get(), m_resourceBridge.get());
    }

    void CalculatorManagerWrapper::Reset(bool clearMemory)
    {
        m_manager->Reset(clearMemory);
    }

    void CalculatorManagerWrapper::SetStandardMode()
    {
        m_manager->SetStandardMode();
    }

    void CalculatorManagerWrapper::SetScientificMode()
    {
        m_manager->SetScientificMode();
    }

    void CalculatorManagerWrapper::SetProgrammerMode()
    {
        m_manager->SetProgrammerMode();
    }

    void CalculatorManagerWrapper::SendCommand(CalcManager::Interop::CalculatorCommand command)
    {
        m_manager->SendCommand(static_cast<CalculationManager::Command>(static_cast<int>(command)));
    }

    void CalculatorManagerWrapper::MemorizeNumber()
    {
        m_manager->MemorizeNumber();
    }

    void CalculatorManagerWrapper::MemorizedNumberLoad(uint32_t index)
    {
        m_manager->MemorizedNumberLoad(index);
    }

    void CalculatorManagerWrapper::MemorizedNumberAdd(uint32_t index)
    {
        m_manager->MemorizedNumberAdd(index);
    }

    void CalculatorManagerWrapper::MemorizedNumberSubtract(uint32_t index)
    {
        m_manager->MemorizedNumberSubtract(index);
    }

    void CalculatorManagerWrapper::MemorizedNumberClear(uint32_t index)
    {
        m_manager->MemorizedNumberClear(index);
    }

    void CalculatorManagerWrapper::MemorizedNumberClearAll()
    {
        m_manager->MemorizedNumberClearAll();
    }

    bool CalculatorManagerWrapper::IsEngineRecording()
    {
        return m_manager->IsEngineRecording();
    }

    bool CalculatorManagerWrapper::IsInputEmpty()
    {
        return m_manager->IsInputEmpty();
    }

    void CalculatorManagerWrapper::SetRadix(int32_t radixType)
    {
        m_manager->SetRadix(static_cast<RadixType>(radixType));
    }

    void CalculatorManagerWrapper::SetMemorizedNumbersString()
    {
        m_manager->SetMemorizedNumbersString();
    }

    hstring CalculatorManagerWrapper::GetResultForRadix(uint32_t radix, int32_t precision, bool groupDigitsPerRadix)
    {
        std::wstring result = m_manager->GetResultForRadix(radix, precision, groupDigitsPerRadix);
        return hstring(result);
    }

    void CalculatorManagerWrapper::SetPrecision(int32_t precision)
    {
        m_manager->SetPrecision(precision);
    }

    void CalculatorManagerWrapper::UpdateMaxIntDigits()
    {
        m_manager->UpdateMaxIntDigits();
    }

    char16_t CalculatorManagerWrapper::DecimalSeparator()
    {
        return static_cast<char16_t>(m_manager->DecimalSeparator());
    }

    com_array<CalcManager::Interop::HistoryItemWrapper> CalculatorManagerWrapper::GetHistoryItems()
    {
        auto const& items = m_manager->GetHistoryItems();
        std::vector<CalcManager::Interop::HistoryItemWrapper> result;
        result.reserve(items.size());
        for (auto const& item : items)
        {
            result.push_back(winrt::make<HistoryItemWrapper>(item));
        }
        return com_array<CalcManager::Interop::HistoryItemWrapper>(std::move(result));
    }

    CalcManager::Interop::HistoryItemWrapper CalculatorManagerWrapper::GetHistoryItem(uint32_t index)
    {
        auto const& item = m_manager->GetHistoryItem(index);
        return winrt::make<HistoryItemWrapper>(item);
    }

    bool CalculatorManagerWrapper::RemoveHistoryItem(uint32_t index)
    {
        return m_manager->RemoveHistoryItem(index);
    }

    void CalculatorManagerWrapper::ClearHistory()
    {
        m_manager->ClearHistory();
    }

    uint64_t CalculatorManagerWrapper::MaxHistorySize()
    {
        return static_cast<uint64_t>(m_manager->MaxHistorySize());
    }

    CalcManager::Interop::CalculatorCommand CalculatorManagerWrapper::GetCurrentDegreeMode()
    {
        auto mode = m_manager->GetCurrentDegreeMode();
        return static_cast<CalcManager::Interop::CalculatorCommand>(static_cast<int>(mode));
    }

    void CalculatorManagerWrapper::SetInHistoryItemLoadMode(bool isHistoryItemLoadMode)
    {
        m_manager->SetInHistoryItemLoadMode(isHistoryItemLoadMode);
    }

    com_array<CalcManager::Interop::ExpressionCommandWrapper> CalculatorManagerWrapper::GetDisplayCommandsSnapshot()
    {
        auto nativeCommands = m_manager->GetDisplayCommandsSnapshot();
        std::vector<CalcManager::Interop::ExpressionCommandWrapper> result;
        result.reserve(nativeCommands.size());
        for (auto const& cmd : nativeCommands)
        {
            result.push_back(winrt::make<ExpressionCommandWrapper>(cmd));
        }
        return com_array<CalcManager::Interop::ExpressionCommandWrapper>(std::move(result));
    }
}
