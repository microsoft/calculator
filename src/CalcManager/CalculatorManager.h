// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalculatorHistory.h"
#include "Header Files/CalcEngine.h"
#include "Header Files/Rational.h"
#include "Header Files/ICalcDisplay.h"

namespace CalculationManager
{
    enum class Command;
    struct HISTORYITEM;

    enum class CalculatorMode
    {
        Standard = 0,
        Scientific,
    };

    enum class CalculatorPrecision
    {
        StandardModePrecision = 16,
        ScientificModePrecision = 32,
        ProgrammerModePrecision = 64
    };

    // Numbering continues from the Enum Command from Command.h
    // with some gap to ensure there is no overlap of these ids
    // when static_cast<unsigned char> is performed on these ids
    // they shouldn't fall in any number range greater than 80. So never
    // make the memory command ids go below 330
    enum class MemoryCommand
    {
        MemorizeNumber = 330,
        MemorizedNumberLoad = 331,
        MemorizedNumberAdd = 332,
        MemorizedNumberSubtract = 333,
        MemorizedNumberClearAll = 334,
        MemorizedNumberClear = 335
    };

    class CalculatorManager final : public ICalcDisplay
    {
    private:
        static const unsigned int m_maximumMemorySize = 100;
        ICalcDisplay* const m_displayCallback;
        CCalcEngine* m_currentCalculatorEngine;
        std::unique_ptr<CCalcEngine> m_scientificCalculatorEngine;
        std::unique_ptr<CCalcEngine> m_standardCalculatorEngine;
        std::unique_ptr<CCalcEngine> m_programmerCalculatorEngine;
        IResourceProvider* const m_resourceProvider;
        bool m_inHistoryItemLoadMode;

        std::vector<CalcEngine::Rational> m_memorizedNumbers;
        CalcEngine::Rational m_persistedPrimaryValue;
        bool m_isExponentialFormat;
        Command m_currentDegreeMode;

        void MemorizedNumberSelect(_In_ unsigned int);
        void MemorizedNumberChanged(_In_ unsigned int);

        void LoadPersistedPrimaryValue();

        std::shared_ptr<CalculatorHistory> m_pStdHistory;
        std::shared_ptr<CalculatorHistory> m_pSciHistory;
        CalculatorHistory* m_pHistory;

    public:
        // ICalcDisplay
        void SetPrimaryDisplay(_In_ const std::wstring& displayString, _In_ bool isError) override;
        void SetIsInError(bool isError) override;
        void SetExpressionDisplay(
            _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
            _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands) override;
        void SetMemorizedNumbers(_In_ const std::vector<std::wstring>& memorizedNumbers) override;
        void OnHistoryItemAdded(_In_ unsigned int addedItemIndex) override;
        void SetParenthesisNumber(_In_ unsigned int parenthesisCount) override;
        void OnNoRightParenAdded() override;
        void DisplayPasteError();
        void MaxDigitsReached() override;
        void BinaryOperatorReceived() override;
        void MemoryItemChanged(unsigned int indexOfMemory) override;
        void InputChanged() override;
        CalculatorManager(_In_ ICalcDisplay* displayCallback, _In_ IResourceProvider* resourceProvider);

        void Reset(bool clearMemory = true);
        void SetStandardMode();
        void SetScientificMode();
        void SetProgrammerMode();
        void SendCommand(_In_ Command command);

        void MemorizeNumber();
        void MemorizedNumberLoad(_In_ unsigned int);
        void MemorizedNumberAdd(_In_ unsigned int);
        void MemorizedNumberSubtract(_In_ unsigned int);
        void MemorizedNumberClear(_In_ unsigned int);
        void MemorizedNumberClearAll();

        bool IsEngineRecording();
        bool IsInputEmpty();
        void SetRadix(RadixType iRadixType);
        void SetMemorizedNumbersString();
        std::wstring GetResultForRadix(uint32_t radix, int32_t precision, bool groupDigitsPerRadix);
        void SetPrecision(int32_t precision);
        void UpdateMaxIntDigits();
        wchar_t DecimalSeparator();

        std::vector<std::shared_ptr<HISTORYITEM>> const& GetHistoryItems() const;
        std::vector<std::shared_ptr<HISTORYITEM>> const& GetHistoryItems(_In_ CalculatorMode mode) const;
        void SetHistoryItems(_In_ std::vector<std::shared_ptr<HISTORYITEM>> const& historyItems);
        std::shared_ptr<HISTORYITEM> const& GetHistoryItem(_In_ unsigned int uIdx);
        bool RemoveHistoryItem(_In_ unsigned int uIdx);
        void ClearHistory();
        size_t MaxHistorySize() const
        {
            return m_pHistory->MaxHistorySize();
        }
        CalculationManager::Command GetCurrentDegreeMode();
        void SetInHistoryItemLoadMode(_In_ bool isHistoryItemLoadMode);
        std::vector<std::shared_ptr<IExpressionCommand>> GetDisplayCommandsSnapshot() const;
    };
}
