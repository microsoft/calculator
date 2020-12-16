// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <climits> // for UCHAR_MAX
#include "Header Files/CalcEngine.h"
#include "CalculatorManager.h"
#include "CalculatorResource.h"

using namespace std;
using namespace CalcEngine;

static constexpr size_t MAX_HISTORY_ITEMS = 20;

#ifndef _MSC_VER
#define __pragma(x)
#endif

namespace CalculationManager
{
    CalculatorManager::CalculatorManager(_In_ ICalcDisplay* displayCallback, _In_ IResourceProvider* resourceProvider)
        : m_displayCallback(displayCallback)
        , m_currentCalculatorEngine(nullptr)
        , m_resourceProvider(resourceProvider)
        , m_inHistoryItemLoadMode(false)
        , m_persistedPrimaryValue()
        , m_isExponentialFormat(false)
        , m_currentDegreeMode(Command::CommandNULL)
        , m_pStdHistory(new CalculatorHistory(MAX_HISTORY_ITEMS))
        , m_pSciHistory(new CalculatorHistory(MAX_HISTORY_ITEMS))
        , m_pHistory(nullptr)
    {
        CCalcEngine::InitialOneTimeOnlySetup(*m_resourceProvider);
    }

    /// <summary>
    /// Call the callback function using passed in IDisplayHelper.
    /// Used to set the primary display value on ViewModel
    /// </summary>
    /// <param name="text">wstring representing text to be displayed</param>
    void CalculatorManager::SetPrimaryDisplay(_In_ const wstring& displayString, _In_ bool isError)
    {
        if (!m_inHistoryItemLoadMode)
        {
            m_displayCallback->SetPrimaryDisplay(displayString, isError);
        }
    }

    void CalculatorManager::SetIsInError(bool isError)
    {
        m_displayCallback->SetIsInError(isError);
    }

    void CalculatorManager::DisplayPasteError()
    {
        m_currentCalculatorEngine->DisplayError(CALC_E_DOMAIN /*code for "Invalid input" error*/);
    }

    void CalculatorManager::MaxDigitsReached()
    {
        m_displayCallback->MaxDigitsReached();
    }

    void CalculatorManager::BinaryOperatorReceived()
    {
        m_displayCallback->BinaryOperatorReceived();
    }

    void CalculatorManager::MemoryItemChanged(unsigned int indexOfMemory)
    {
        m_displayCallback->MemoryItemChanged(indexOfMemory);
    }

    void CalculatorManager::InputChanged()
    {
        m_displayCallback->InputChanged();
    }

    /// <summary>
    /// Call the callback function using passed in IDisplayHelper.
    /// Used to set the expression display value on ViewModel
    /// </summary>
    /// <param name="expressionString">wstring representing expression to be displayed</param>
    void CalculatorManager::SetExpressionDisplay(
        _Inout_ shared_ptr<vector<pair<wstring, int>>> const& tokens,
        _Inout_ shared_ptr<vector<shared_ptr<IExpressionCommand>>> const& commands)
    {
        if (!m_inHistoryItemLoadMode)
        {
            m_displayCallback->SetExpressionDisplay(tokens, commands);
        }
    }

    /// <summary>
    /// Callback from the CalculatorControl
    /// Passed in string representations of memorized numbers get passed to the client
    /// </summary>
    /// <param name="memorizedNumber">vector containing wstring values of memorized numbers</param>
    void CalculatorManager::SetMemorizedNumbers(_In_ const vector<wstring>& memorizedNumbers)
    {
        m_displayCallback->SetMemorizedNumbers(memorizedNumbers);
    }

    /// <summary>
    /// Callback from the engine
    /// </summary>
    /// <param name="parenthesisCount">string containing the parenthesis count</param>
    void CalculatorManager::SetParenthesisNumber(_In_ unsigned int parenthesisCount)
    {
        m_displayCallback->SetParenthesisNumber(parenthesisCount);
    }

    /// <summary>
    /// Callback from the engine
    /// </summary>
    void CalculatorManager::OnNoRightParenAdded()
    {
        m_displayCallback->OnNoRightParenAdded();
    }

    /// <summary>
    /// Reset CalculatorManager.
    /// Set the mode to the standard calculator
    /// Set the degree mode as regular degree (as oppose to Rad or Grad)
    /// Clear all the entries and memories
    /// Clear Memory if clearMemory parameter is true.(Default value is true)
    /// </summary>
    void CalculatorManager::Reset(bool clearMemory /* = true*/)
    {
        SetStandardMode();

        if (m_scientificCalculatorEngine)
        {
            m_scientificCalculatorEngine->ProcessCommand(IDC_DEG);
            m_scientificCalculatorEngine->ProcessCommand(IDC_CLEAR);

            if (m_isExponentialFormat)
            {
                m_isExponentialFormat = false;
                m_scientificCalculatorEngine->ProcessCommand(IDC_FE);
            }
        }
        if (m_programmerCalculatorEngine)
        {
            m_programmerCalculatorEngine->ProcessCommand(IDC_CLEAR);
        }

        if (clearMemory)
        {
            this->MemorizedNumberClearAll();
        }
    }

    /// <summary>
    /// Change the current calculator engine to standard calculator engine.
    /// </summary>
    void CalculatorManager::SetStandardMode()
    {
        if (!m_standardCalculatorEngine)
        {
            m_standardCalculatorEngine =
                make_unique<CCalcEngine>(false /* Respect Order of Operations */, false /* Set to Integer Mode */, m_resourceProvider, this, m_pStdHistory);
        }

        m_currentCalculatorEngine = m_standardCalculatorEngine.get();
        m_currentCalculatorEngine->ProcessCommand(IDC_DEC);
        m_currentCalculatorEngine->ProcessCommand(IDC_CLEAR);
        m_currentCalculatorEngine->ChangePrecision(static_cast<int>(CalculatorPrecision::StandardModePrecision));
        UpdateMaxIntDigits();
        m_pHistory = m_pStdHistory.get();
    }

    /// <summary>
    /// Change the current calculator engine to scientific calculator engine.
    /// </summary>
    void CalculatorManager::SetScientificMode()
    {
        if (!m_scientificCalculatorEngine)
        {
            m_scientificCalculatorEngine =
                make_unique<CCalcEngine>(true /* Respect Order of Operations */, false /* Set to Integer Mode */, m_resourceProvider, this, m_pSciHistory);
        }

        m_currentCalculatorEngine = m_scientificCalculatorEngine.get();
        m_currentCalculatorEngine->ProcessCommand(IDC_DEC);
        m_currentCalculatorEngine->ProcessCommand(IDC_CLEAR);
        m_currentCalculatorEngine->ChangePrecision(static_cast<int>(CalculatorPrecision::ScientificModePrecision));
        m_pHistory = m_pSciHistory.get();
    }

    /// <summary>
    /// Change the current calculator engine to scientific calculator engine.
    /// </summary>
    void CalculatorManager::SetProgrammerMode()
    {
        if (!m_programmerCalculatorEngine)
        {
            m_programmerCalculatorEngine =
                make_unique<CCalcEngine>(true /* Respect Order of Operations */, true /* Set to Integer Mode */, m_resourceProvider, this, nullptr);
        }

        m_currentCalculatorEngine = m_programmerCalculatorEngine.get();
        m_currentCalculatorEngine->ProcessCommand(IDC_DEC);
        m_currentCalculatorEngine->ProcessCommand(IDC_CLEAR);
        m_currentCalculatorEngine->ChangePrecision(static_cast<int>(CalculatorPrecision::ProgrammerModePrecision));
    }

    /// <summary>
    /// Send command to the Calc Engine
    /// Cast Command Enum to OpCode.
    /// Handle special commands such as mode change and combination of two commands.
    /// </summary>
    /// <param name="command">Enum Command</command>
    void CalculatorManager::SendCommand(_In_ Command command)
    {
        // When the expression line is cleared, we save the current state, which includes,
        // primary display, memory, and degree mode
        if (command == Command::CommandCLEAR || command == Command::CommandEQU || command == Command::ModeBasic || command == Command::ModeScientific
            || command == Command::ModeProgrammer)
        {
            switch (command)
            {
            case Command::ModeBasic:
                this->SetStandardMode();
                break;
            case Command::ModeScientific:
                this->SetScientificMode();
                break;
            case Command::ModeProgrammer:
                this->SetProgrammerMode();
                break;
            default:
                m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(command));
            }

            InputChanged();
            return;
        }

        if (command == Command::CommandDEG || command == Command::CommandRAD || command == Command::CommandGRAD)
        {
            m_currentDegreeMode = command;
        }

        switch (command)
        {
        case Command::CommandASIN:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandSIN));
            break;
        case Command::CommandACOS:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandCOS));
            break;
        case Command::CommandATAN:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandTAN));
            break;
        case Command::CommandPOWE:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandLN));
            break;
        case Command::CommandASINH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandSINH));
            break;
        case Command::CommandACOSH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandCOSH));
            break;
        case Command::CommandATANH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandTANH));
            break;
        case Command::CommandASEC:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandSEC));
            break;
        case Command::CommandACSC:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandCSC));
            break;
        case Command::CommandACOT:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandCOT));
            break;
        case Command::CommandASECH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandSECH));
            break;
        case Command::CommandACSCH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandCSCH));
            break;
        case Command::CommandACOTH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(Command::CommandCOTH));
            break;
        case Command::CommandFE:
            m_isExponentialFormat = !m_isExponentialFormat;
            [[fallthrough]];
        default:
            m_currentCalculatorEngine->ProcessCommand(static_cast<OpCode>(command));
            break;
        }

        InputChanged();
    }

    /// <summary>
    /// Load the persisted value that is saved in memory of CalcEngine
    /// </summary>
    void CalculatorManager::LoadPersistedPrimaryValue()
    {
        m_currentCalculatorEngine->PersistedMemObject(m_persistedPrimaryValue);
        m_currentCalculatorEngine->ProcessCommand(IDC_RECALL);
        InputChanged();
    }

    /// <summary>
    /// Memorize the current displayed value
    /// Notify the client with new the new memorize value vector
    /// </summary>
    void CalculatorManager::MemorizeNumber()
    {
        if (m_currentCalculatorEngine->FInErrorState())
        {
            return;
        }

        m_currentCalculatorEngine->ProcessCommand(IDC_STORE);

        auto memoryObjectPtr = m_currentCalculatorEngine->PersistedMemObject();
        if (memoryObjectPtr != nullptr)
        {
            m_memorizedNumbers.insert(m_memorizedNumbers.begin(), *memoryObjectPtr);
        }

        if (m_memorizedNumbers.size() > m_maximumMemorySize)
        {
            m_memorizedNumbers.resize(m_maximumMemorySize);
        }
        this->SetMemorizedNumbersString();
    }

    /// <summary>
    /// Recall the memorized number.
    /// The memorized number gets loaded to the primary display
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberLoad(_In_ unsigned int indexOfMemory)
    {
        if (m_currentCalculatorEngine->FInErrorState())
        {
            return;
        }

        this->MemorizedNumberSelect(indexOfMemory);
        m_currentCalculatorEngine->ProcessCommand(IDC_RECALL);
        InputChanged();
    }

    /// <summary>
    /// Do the addition to the selected memory
    /// It adds primary display value to the selected memory
    /// Notify the client with new the new memorize value vector
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberAdd(_In_ unsigned int indexOfMemory)
    {
        if (m_currentCalculatorEngine->FInErrorState())
        {
            return;
        }

        if (m_memorizedNumbers.empty())
        {
            this->MemorizeNumber();
        }
        else
        {
            this->MemorizedNumberSelect(indexOfMemory);
            m_currentCalculatorEngine->ProcessCommand(IDC_MPLUS);

            this->MemorizedNumberChanged(indexOfMemory);

            this->SetMemorizedNumbersString();
        }

        m_displayCallback->MemoryItemChanged(indexOfMemory);
    }

    void CalculatorManager::MemorizedNumberClear(_In_ unsigned int indexOfMemory)
    {
        if (indexOfMemory < m_memorizedNumbers.size())
        {
            m_memorizedNumbers.erase(m_memorizedNumbers.begin() + indexOfMemory);
        }
    }

    /// <summary>
    /// Do the subtraction to the selected memory
    /// It adds primary display value to the selected memory
    /// Notify the client with new the new memorize value vector
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberSubtract(_In_ unsigned int indexOfMemory)
    {
        if (m_currentCalculatorEngine->FInErrorState())
        {
            return;
        }

        // To add negative of the number on display to the memory -x = x - 2x
        if (m_memorizedNumbers.empty())
        {
            this->MemorizeNumber();
            this->MemorizedNumberSubtract(0);
            this->MemorizedNumberSubtract(0);
        }
        else
        {
            this->MemorizedNumberSelect(indexOfMemory);
            m_currentCalculatorEngine->ProcessCommand(IDC_MMINUS);

            this->MemorizedNumberChanged(indexOfMemory);

            this->SetMemorizedNumbersString();
        }

        m_displayCallback->MemoryItemChanged(indexOfMemory);
    }

    /// <summary>
    /// Clear all the memorized values
    /// Notify the client with new the new memorize value vector
    /// </summary>
    void CalculatorManager::MemorizedNumberClearAll()
    {
        m_memorizedNumbers.clear();

        m_currentCalculatorEngine->ProcessCommand(IDC_MCLEAR);
        this->SetMemorizedNumbersString();
    }

    /// <summary>
    /// Helper function that selects a memory from the vector and set it to CCalcEngine
    /// Saved RAT number needs to be copied and passed in, as CCalcEngine destroyed the passed in RAT
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberSelect(_In_ unsigned int indexOfMemory)
    {
        if (m_currentCalculatorEngine->FInErrorState())
        {
            return;
        }

        auto memoryObject = m_memorizedNumbers.at(indexOfMemory);
        m_currentCalculatorEngine->PersistedMemObject(memoryObject);
    }

    /// <summary>
    /// Helper function that needs to be executed when memory is modified
    /// When memory is modified, destroy the old RAT and put the new RAT in vector
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberChanged(_In_ unsigned int indexOfMemory)
    {
        if (m_currentCalculatorEngine->FInErrorState())
        {
            return;
        }

        auto memoryObject = m_currentCalculatorEngine->PersistedMemObject();
        if (memoryObject != nullptr)
        {
            m_memorizedNumbers.at(indexOfMemory) = *memoryObject;
        }
    }

    vector<shared_ptr<HISTORYITEM>> const& CalculatorManager::GetHistoryItems()
    {
        return m_pHistory->GetHistory();
    }

    vector<shared_ptr<HISTORYITEM>> const& CalculatorManager::GetHistoryItems(_In_ CalculatorMode mode)
    {
        return (mode == CalculatorMode::Standard) ? m_pStdHistory->GetHistory() : m_pSciHistory->GetHistory();
    }

    shared_ptr<HISTORYITEM> const& CalculatorManager::GetHistoryItem(_In_ unsigned int uIdx)
    {
        return m_pHistory->GetHistoryItem(uIdx);
    }

    void CalculatorManager::OnHistoryItemAdded(_In_ unsigned int addedItemIndex)
    {
        m_displayCallback->OnHistoryItemAdded(addedItemIndex);
    }

    bool CalculatorManager::RemoveHistoryItem(_In_ unsigned int uIdx)
    {
        return m_pHistory->RemoveItem(uIdx);
    }

    void CalculatorManager::ClearHistory()
    {
        m_pHistory->ClearHistory();
    }

    void CalculatorManager::SetRadix(RadixType iRadixType)
    {
        switch (iRadixType)
        {
        case RadixType::Hex:
            m_currentCalculatorEngine->ProcessCommand(IDC_HEX);
            break;
        case RadixType::Decimal:
            m_currentCalculatorEngine->ProcessCommand(IDC_DEC);
            break;
        case RadixType::Octal:
            m_currentCalculatorEngine->ProcessCommand(IDC_OCT);
            break;
        case RadixType::Binary:
            m_currentCalculatorEngine->ProcessCommand(IDC_BIN);
            break;
        default:
            break;
        }
        SetMemorizedNumbersString();
    }

    void CalculatorManager::SetMemorizedNumbersString()
    {
        vector<wstring> resultVector;
        for (auto const& memoryItem : m_memorizedNumbers)
        {
            auto radix = m_currentCalculatorEngine->GetCurrentRadix();
            wstring stringValue = m_currentCalculatorEngine->GetStringForDisplay(memoryItem, radix);

            if (!stringValue.empty())
            {
                resultVector.push_back(m_currentCalculatorEngine->GroupDigitsPerRadix(stringValue, radix));
            }
        }
        m_displayCallback->SetMemorizedNumbers(resultVector);
    }

    CalculationManager::Command CalculatorManager::GetCurrentDegreeMode()
    {
        if (m_currentDegreeMode == Command::CommandNULL)
        {
            m_currentDegreeMode = Command::CommandDEG;
        }
        return m_currentDegreeMode;
    }

    wstring CalculatorManager::GetResultForRadix(uint32_t radix, int32_t precision, bool groupDigitsPerRadix)
    {
        return m_currentCalculatorEngine ? m_currentCalculatorEngine->GetCurrentResultForRadix(radix, precision, groupDigitsPerRadix) : L"";
    }

    void CalculatorManager::SetPrecision(int32_t precision)
    {
        m_currentCalculatorEngine->ChangePrecision(precision);
    }

    void CalculatorManager::UpdateMaxIntDigits()
    {
        m_currentCalculatorEngine->UpdateMaxIntDigits();
    }

    wchar_t CalculatorManager::DecimalSeparator()
    {
        return m_currentCalculatorEngine ? m_currentCalculatorEngine->DecimalSeparator() : m_resourceProvider->GetCEngineString(L"sDecimal")[0];
    }

    bool CalculatorManager::IsEngineRecording()
    {
        return m_currentCalculatorEngine->FInRecordingState();
    }

    bool CalculatorManager::IsInputEmpty()
    {
        return m_currentCalculatorEngine->IsInputEmpty();
    }

    void CalculatorManager::SetInHistoryItemLoadMode(_In_ bool isHistoryItemLoadMode)
    {
        m_inHistoryItemLoadMode = isHistoryItemLoadMode;
    }
}
