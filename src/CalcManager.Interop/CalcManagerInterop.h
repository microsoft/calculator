// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalculatorManagerWrapper.g.h"
#include "ExpressionCommandWrapper.h"
#include "HistoryItemWrapper.h"

namespace winrt::CalcManager::Interop::implementation
{
    // Bridge from native ICalcDisplay to WinRT delegates
    class CalcDisplayBridge : public ICalcDisplay
    {
    public:
        CalcDisplayBridge(
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
            CalcManager::Interop::SimpleHandler const& onInputChanged);

        void SetPrimaryDisplay(const std::wstring& pszText, bool isError) override;
        void SetIsInError(bool isInError) override;
        void SetExpressionDisplay(
            std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
            std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands) override;
        void SetParenthesisNumber(unsigned int count) override;
        void OnNoRightParenAdded() override;
        void MaxDigitsReached() override;
        void BinaryOperatorReceived() override;
        void OnHistoryItemAdded(unsigned int addedItemIndex) override;
        void SetMemorizedNumbers(const std::vector<std::wstring>& memorizedNumbers) override;
        void MemoryItemChanged(unsigned int indexOfMemory) override;
        void InputChanged() override;

    private:
        CalcManager::Interop::SetPrimaryDisplayHandler m_onSetPrimaryDisplay;
        CalcManager::Interop::SetIsInErrorHandler m_onSetIsInError;
        CalcManager::Interop::SetExpressionDisplayHandler m_onSetExpressionDisplay;
        CalcManager::Interop::SetParenthesisNumberHandler m_onSetParenthesisNumber;
        CalcManager::Interop::SimpleHandler m_onNoRightParenAdded;
        CalcManager::Interop::SimpleHandler m_onMaxDigitsReached;
        CalcManager::Interop::SimpleHandler m_onBinaryOperatorReceived;
        CalcManager::Interop::OnHistoryItemAddedHandler m_onHistoryItemAdded;
        CalcManager::Interop::SetMemorizedNumbersHandler m_onSetMemorizedNumbers;
        CalcManager::Interop::MemoryItemChangedHandler m_onMemoryItemChanged;
        CalcManager::Interop::SimpleHandler m_onInputChanged;
    };

    // Bridge from native IResourceProvider to WinRT delegate
    class ResourceProviderBridge : public CalculationManager::IResourceProvider
    {
    public:
        ResourceProviderBridge(CalcManager::Interop::GetCEngineStringHandler const& onGetCEngineString);
        std::wstring GetCEngineString(std::wstring_view id) override;

    private:
        CalcManager::Interop::GetCEngineStringHandler m_onGetCEngineString;
    };

    struct CalculatorManagerWrapper : CalculatorManagerWrapperT<CalculatorManagerWrapper>
    {
        CalculatorManagerWrapper(
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
            CalcManager::Interop::GetCEngineStringHandler const& onGetCEngineString);

        void Reset(bool clearMemory);
        void SetStandardMode();
        void SetScientificMode();
        void SetProgrammerMode();
        void SendCommand(CalcManager::Interop::CalculatorCommand command);

        void MemorizeNumber();
        void MemorizedNumberLoad(uint32_t index);
        void MemorizedNumberAdd(uint32_t index);
        void MemorizedNumberSubtract(uint32_t index);
        void MemorizedNumberClear(uint32_t index);
        void MemorizedNumberClearAll();

        bool IsEngineRecording();
        bool IsInputEmpty();

        void SetRadix(int32_t radixType);
        void SetMemorizedNumbersString();
        hstring GetResultForRadix(uint32_t radix, int32_t precision, bool groupDigitsPerRadix);
        void SetPrecision(int32_t precision);
        void UpdateMaxIntDigits();
        char16_t DecimalSeparator();

        com_array<CalcManager::Interop::HistoryItemWrapper> GetHistoryItems();
        CalcManager::Interop::HistoryItemWrapper GetHistoryItem(uint32_t index);
        bool RemoveHistoryItem(uint32_t index);
        void ClearHistory();
        uint64_t MaxHistorySize();

        CalcManager::Interop::CalculatorCommand GetCurrentDegreeMode();
        void SetInHistoryItemLoadMode(bool isHistoryItemLoadMode);
        com_array<CalcManager::Interop::ExpressionCommandWrapper> GetDisplayCommandsSnapshot();

    private:
        std::unique_ptr<CalcDisplayBridge> m_displayBridge;
        std::unique_ptr<ResourceProviderBridge> m_resourceBridge;
        std::unique_ptr<CalculationManager::CalculatorManager> m_manager;
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct CalculatorManagerWrapper
        : CalculatorManagerWrapperT<CalculatorManagerWrapper, implementation::CalculatorManagerWrapper>
    {
    };
}
