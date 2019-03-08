// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Header Files/CalcEngine.h"
#include "CalculatorManager.h"
#include "CalculatorResource.h"

using namespace std;
using namespace CalcEngine;

static constexpr size_t MAX_HISTORY_ITEMS = 20;
static constexpr size_t SERIALIZED_NUMBER_MINSIZE = 3;

// Converts Memory Command enum value to unsigned char,
// while ignoring Warning C4309: 'conversion' : truncation of constant value
#define MEMORY_COMMAND_TO_UNSIGNED_CHAR(c)\
    __pragma(warning(push))\
    __pragma(warning(disable: 4309))\
        static_cast<unsigned char>(c)\
    __pragma(warning(pop))

namespace CalculationManager
{
    CalculatorManager::CalculatorManager(_In_ ICalcDisplay* displayCallback, _In_ IResourceProvider* resourceProvider) :
        m_displayCallback(displayCallback),
        m_resourceProvider(resourceProvider),
        m_currentDegreeMode(Command::CommandNULL),
        m_savedDegreeMode(Command::CommandDEG),
        m_isExponentialFormat(false),
        m_persistedPrimaryValue(),
        m_currentCalculatorEngine(nullptr),
        m_pStdHistory(new CalculatorHistory(CM_STD, MAX_HISTORY_ITEMS)),
        m_pSciHistory(new CalculatorHistory(CM_SCI, MAX_HISTORY_ITEMS)),
        m_inHistoryItemLoadMode(false)
    {
        CCalcEngine::InitialOneTimeOnlySetup(*m_resourceProvider);
    }

    /// <summary>
    /// Destructor for CalculatorManager
    /// Ends two CCalcEngine
    /// </summary>
    CalculatorManager::~CalculatorManager()
    {
        this->MemorizedNumberClearAll();
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

    /// <summary>
    /// Call the callback function using passed in IDisplayHelper.
    /// Used to set the expression display value on ViewModel
    /// </summary>
    /// <param name="expressionString">wstring representing expression to be displayed</param>
    void CalculatorManager::SetExpressionDisplay(_Inout_ shared_ptr<CalculatorVector<pair<wstring, int>>> const &tokens, _Inout_ shared_ptr<CalculatorVector<shared_ptr<IExpressionCommand>>> const &commands)
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
    /// Used to set the current unmatched open parenthesis count
    /// </summary>
    /// <param name="parenthesisCount">string containing the parenthesis count</param>
    void CalculatorManager::SetParenDisplayText(const wstring& parenthesisCount)
    {
        m_displayCallback->SetParenDisplayText(parenthesisCount);
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
        m_savedCommands.clear();
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
            m_standardCalculatorEngine = make_unique<CCalcEngine>(false /* Respect Order of Operations */, false /* Set to Integer Mode */, m_resourceProvider, this, m_pStdHistory);
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
            m_scientificCalculatorEngine = make_unique<CCalcEngine>(true /* Respect Order of Operations */, false /* Set to Integer Mode */, m_resourceProvider, this, m_pSciHistory);
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
        if(!m_programmerCalculatorEngine)
        {
            m_programmerCalculatorEngine = make_unique<CCalcEngine>(true /* Respect Order of Operations */, true /* Set to Integer Mode */, m_resourceProvider, this, nullptr);
        }

        m_currentCalculatorEngine = m_programmerCalculatorEngine.get();
        m_currentCalculatorEngine->ProcessCommand(IDC_DEC);
        m_currentCalculatorEngine->ProcessCommand(IDC_CLEAR);
        m_currentCalculatorEngine->ChangePrecision(static_cast<int>(CalculatorPrecision::ProgrammerModePrecision));
    }


    /// <summary>
    /// Send command to the Calc Engine
    /// Cast Command Enum to WPARAM.
    /// Handle special commands such as mode change and combination of two commands.
    /// </summary>
    /// <param name="command">Enum Command</command>
    void CalculatorManager::SendCommand(_In_ Command command)
    {
        // When the expression line is cleared, we save the current state, which includes,
        // primary display, memory, and degree mode
        if (command == Command::CommandCLEAR || command == Command::CommandEQU
            || command == Command::ModeBasic || command == Command::ModeScientific || command == Command::ModeProgrammer)
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
                m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(command));
            }

            m_savedCommands.clear(); // Clear the previous command history

            if (command != Command::CommandEQU && command != Command::CommandCLEAR)
            {
                m_savedCommands.push_back(MapCommandForSerialize(command));
            }
            this->SerializePrimaryDisplay();
            this->SerializeMemory();
            m_savedDegreeMode = m_currentDegreeMode;
            return;
        }

        if (command == Command::CommandDEG || command == Command::CommandRAD || command == Command::CommandGRAD)
        {
            m_currentDegreeMode = command;
        }

        if (command != Command::CommandFE)
        {
            m_savedCommands.push_back(MapCommandForSerialize(command)); // Save the commands in the m_savedCommands
        }

        switch (command)
        {
        case Command::CommandASIN:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandSIN));
            break;
        case Command::CommandACOS:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandCOS));
            break;
        case Command::CommandATAN:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandTAN));
            break;
        case Command::CommandPOWE:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandLN));
            break;
        case Command::CommandASINH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandSINH));
            break;
        case Command::CommandACOSH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandCOSH));
            break;
        case Command::CommandATANH:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandINV));
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(Command::CommandTANH));
            break;
        case Command::CommandFE:
            m_isExponentialFormat = !m_isExponentialFormat;
            // fall through
        default:
            m_currentCalculatorEngine->ProcessCommand(static_cast<WPARAM>(command));
            break;
        }
    }

    /// <summary>
    /// Convert Command to unsigned char. 
    /// Since some Commands are higher than 255, they are saved after subtracting 255
    /// The smallest Command is CommandSIGN = 80, thus, subtracted value does not overlap with other values.
    /// </summary>
    /// <param name="command">Enum Command</command>
    unsigned char CalculatorManager::MapCommandForSerialize(Command command)
    {
        unsigned int commandToSave = static_cast<unsigned int>(command);
        commandToSave > UCHAR_MAX ? commandToSave -= UCHAR_MAX : commandToSave;
        return static_cast<unsigned char>(commandToSave);
    }

    /// <summary>
    /// Convert Command to unsigned int
    /// The command that is smaller than 80, CommandSIGN, can be converted back to original value by adding 255.
    /// </summary>
    /// <param name="command">unsigned char value represent the saved command</command>
    unsigned int CalculatorManager::MapCommandForDeSerialize(unsigned char command)
    {
        unsigned int commandToLoad = command;
        if (command < static_cast<unsigned int>(Command::CommandSIGN))
        {
            commandToLoad += UCHAR_MAX;
        }
        return commandToLoad;
    }

    /// <summary>
    /// Return saved degree mode which is saved when last time the expression was cleared.
    /// </summary>
    Command CalculatorManager::SerializeSavedDegreeMode()
    {
        return m_savedDegreeMode;
    }

    void CalculatorManager::SerializePrimaryDisplay()
    {
        m_savedPrimaryValue.clear();
        m_currentCalculatorEngine->ProcessCommand(IDC_STORE);
        auto memoryObject = m_currentCalculatorEngine->PersistedMemObject();
        if (memoryObject != nullptr)
        {
            m_savedPrimaryValue = SerializeRational(*memoryObject);
        }
    }

    /// <summary>
    /// Return serialized primary display that is saved when the expression line was cleared.
    /// </summary>
    vector<long> CalculatorManager::GetSerializedPrimaryDisplay()
    {
        return m_savedPrimaryValue;
    }

    /// <summary>
    /// DeSerialize the primary display from vector of long
    /// </summary>
    /// <param name = "serializedPrimaryDisplay">Serialized Rational of primary display</param>
    void CalculatorManager::DeSerializePrimaryDisplay(const vector<long> &serializedPrimaryDisplay)
    {
        if (serializedPrimaryDisplay.size() == 0)
        {
            return;
        }
        m_persistedPrimaryValue = DeSerializeRational(serializedPrimaryDisplay.begin());
        this->LoadPersistedPrimaryValue();
    }

    /// <summary>
    /// Load the persisted value that is saved in memory of CalcEngine
    /// </summary>
    void CalculatorManager::LoadPersistedPrimaryValue()
    {
        m_currentCalculatorEngine->PersistedMemObject(m_persistedPrimaryValue);
        m_currentCalculatorEngine->ProcessCommand(IDC_RECALL);
    }

    /// <summary>
    /// Serialize the Memory to vector of long
    /// </summary>
    /// <return type = "std::vector<long>">Serialized Rational of memory</return>
    void CalculatorManager::SerializeMemory()
    {
        m_serializedMemory.clear();

        for (auto const& memoryItem : m_memorizedNumbers)
        {
            auto serialMem = SerializeRational(memoryItem);
            m_serializedMemory.insert(m_serializedMemory.end(), serialMem.begin(), serialMem.end());
        }
    }

    vector<long> CalculatorManager::GetSerializedMemory()
    {
        return m_serializedMemory;
    }

    /// <summary>
    /// DeSerialize the Memory from vector of long
    /// </summary>
    /// <param name = "serializedMemory">Serialized Rational of memory</param>
    void CalculatorManager::DeSerializeMemory(const vector<long> &serializedMemory)
    {
        vector<long>::const_iterator itr = serializedMemory.begin();
        while (itr != serializedMemory.end())
        {
            Rational memoryItem = DeSerializeRational(itr);
            auto lengthMemoryItem = (2 * SERIALIZED_NUMBER_MINSIZE) + memoryItem.P().Mantissa().size() + memoryItem.Q().Mantissa().size();
            m_memorizedNumbers.push_back(memoryItem);
            itr += lengthMemoryItem;
        }
        this->SetMemorizedNumbersString();
    }

    /// <summary>
    /// Return the commands saved since the expression has been cleared.
    /// </summary>
    vector<unsigned char> CalculatorManager::SerializeCommands()
    {
        return m_savedCommands;
    }

    /// <summary>
    /// Replay the serialized commands
    /// </summary>
    /// <param name = "serializedData">Serialized commands</param>
    void CalculatorManager::DeSerializeCommands(_In_ const vector<unsigned char>& serializedData)
    {
        m_savedCommands.clear();

        for (auto commandItr = serializedData.begin(); commandItr != serializedData.end(); ++commandItr)
        {
            if (*commandItr >= MEMORY_COMMAND_TO_UNSIGNED_CHAR(MemoryCommand::MemorizeNumber) &&
                *commandItr <= MEMORY_COMMAND_TO_UNSIGNED_CHAR(MemoryCommand::MemorizedNumberClearAll))
            {
                //MemoryCommands(which have values above 255) are pushed on m_savedCommands upon casting to unsigned char.
                //SerializeCommands uses m_savedCommands, which is then used in DeSerializeCommands.
                //Hence, a simple cast to MemoryCommand is not sufficient.
                MemoryCommand memoryCommand = static_cast<MemoryCommand>(*commandItr + UCHAR_MAX + 1);
                unsigned int indexOfMemory = 0;
                switch (memoryCommand)
                {
                case MemoryCommand::MemorizeNumber:
                    this->MemorizeNumber();
                    break;
                case MemoryCommand::MemorizedNumberLoad:
                    if (commandItr + 1 == serializedData.end())
                    {
                        throw out_of_range("Expecting index of memory, data ended prematurely");
                    }
                    indexOfMemory = *(++commandItr);
                    this->MemorizedNumberLoad(indexOfMemory);
                    break;
                case MemoryCommand::MemorizedNumberAdd:
                    if (commandItr + 1 == serializedData.end())
                    {
                        throw out_of_range("Expecting index of memory, data ended prematurely");
                    }
                    indexOfMemory = *(++commandItr);
                    this->MemorizedNumberAdd(indexOfMemory);
                    break;
                case MemoryCommand::MemorizedNumberSubtract:
                    if (commandItr + 1 == serializedData.end())
                    {
                        throw out_of_range("Expecting index of memory, data ended prematurely");
                    }
                    indexOfMemory = *(++commandItr);
                    this->MemorizedNumberSubtract(indexOfMemory);
                    break;
                case MemoryCommand::MemorizedNumberClearAll:
                    this->MemorizedNumberClearAll();
                    break;
                default:
                    break;
                }
            }
            else
            {
                this->SendCommand(static_cast<Command>(MapCommandForDeSerialize(*commandItr)));
            }
        }
    }

    /// <summary>
    /// Memorize the current displayed value
    /// Notify the client with new the new memorize value vector
    /// </summary>
    void CalculatorManager::MemorizeNumber()
    {
        m_savedCommands.push_back(MEMORY_COMMAND_TO_UNSIGNED_CHAR(MemoryCommand::MemorizeNumber));
        if (!(m_currentCalculatorEngine->FInErrorState()))
        {
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
    }

    /// <summary>
    /// Recall the memorized number.
    /// The memorized number gets loaded to the primary display
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberLoad(_In_ unsigned int indexOfMemory)
    {
        SaveMemoryCommand(MemoryCommand::MemorizedNumberLoad, indexOfMemory);
        if (!(m_currentCalculatorEngine->FInErrorState()))
        {
            this->MemorizedNumberSelect(indexOfMemory);
            m_currentCalculatorEngine->ProcessCommand(IDC_RECALL);
        }
    }

    /// <summary>
    /// Do the addition to the selected memory
    /// It adds primary display value to the selected memory
    /// Notify the client with new the new memorize value vector
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberAdd(_In_ unsigned int indexOfMemory)
    {
        SaveMemoryCommand(MemoryCommand::MemorizedNumberAdd, indexOfMemory);
        if (!(m_currentCalculatorEngine->FInErrorState()))
        {
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
    }

    void CalculatorManager::MemorizedNumberClear(_In_ unsigned int indexOfMemory)
    {
        if (indexOfMemory < m_memorizedNumbers.size())
        {
            SaveMemoryCommand(MemoryCommand::MemorizedNumberClear, indexOfMemory);
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
        SaveMemoryCommand(MemoryCommand::MemorizedNumberSubtract, indexOfMemory);
        if (!(m_currentCalculatorEngine->FInErrorState()))
        {
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
    }

    /// <summary>
    /// Clear all the memorized values
    /// Notify the client with new the new memorize value vector
    /// </summary>
    void CalculatorManager::MemorizedNumberClearAll()
    {
        m_savedCommands.push_back(MEMORY_COMMAND_TO_UNSIGNED_CHAR(MemoryCommand::MemorizedNumberClearAll));
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
        if (!(m_currentCalculatorEngine->FInErrorState()))
        {
            auto memoryObject = m_memorizedNumbers.at(indexOfMemory);
            m_currentCalculatorEngine->PersistedMemObject(memoryObject);
        }
    }

    /// <summary>
    /// Helper function that needs to be executed when memory is modified
    /// When memory is modified, destroy the old RAT and put the new RAT in vector
    /// </summary>
    /// <param name="indexOfMemory">Index of the target memory</param>
    void CalculatorManager::MemorizedNumberChanged(_In_ unsigned int indexOfMemory)
    {
        if (!(m_currentCalculatorEngine->FInErrorState()))
        {
            auto memoryObject = m_currentCalculatorEngine->PersistedMemObject();
            if (memoryObject != nullptr)
            {
                m_memorizedNumbers.at(indexOfMemory) = *memoryObject;
            }
        }
    }

    void CalculatorManager::SaveMemoryCommand(_In_ MemoryCommand command, _In_ unsigned int indexOfMemory)
    {
        m_savedCommands.push_back(MEMORY_COMMAND_TO_UNSIGNED_CHAR(command));
        if (indexOfMemory > UCHAR_MAX)
        {
            throw invalid_argument("Unexpected value. IndexOfMemory is bigger than the biggest unsigned char");
        }
        m_savedCommands.push_back(static_cast<unsigned char>(indexOfMemory));
    }

    vector<shared_ptr<HISTORYITEM>> const& CalculatorManager::GetHistoryItems()
    {
        return m_pHistory->GetHistory();
    }

    vector<shared_ptr<HISTORYITEM>> const& CalculatorManager::GetHistoryItems(_In_ CALCULATOR_MODE mode)
    {
        return (mode == CM_STD) ?
            m_pStdHistory->GetHistory() :
            m_pSciHistory->GetHistory();
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

    void CalculatorManager::SetRadix(RADIX_TYPE iRadixType)
    {
        switch (iRadixType)
        {
        case RADIX_TYPE::HEX_RADIX:
            m_currentCalculatorEngine->ProcessCommand(IDC_HEX);
            break;
        case RADIX_TYPE::DEC_RADIX:
            m_currentCalculatorEngine->ProcessCommand(IDC_DEC);
            break;
        case RADIX_TYPE::OCT_RADIX:
            m_currentCalculatorEngine->ProcessCommand(IDC_OCT);
            break;
        case RADIX_TYPE::BIN_RADIX:
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
            int radix = m_currentCalculatorEngine->GetCurrentRadix();
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

    void CalculatorManager::SetHistory(_In_ CALCULATOR_MODE eMode, _In_ vector<shared_ptr<HISTORYITEM>> const& history)
    {
        CalculatorHistory* pHistory = nullptr;

        switch (eMode)
        {
        case CM_STD:
            pHistory = m_pStdHistory.get();
            break;
        case CM_SCI:
            pHistory = m_pSciHistory.get();
            break;
        }

        if (pHistory)
        {
            pHistory->ClearHistory();
            for (unsigned int i = 0; i < history.size(); ++i)
            {
                pHistory->AddItem(history[i]);
            }
        }
    }

    wstring CalculatorManager::GetResultForRadix(uint32_t radix, int32_t precision)
    {
        return m_currentCalculatorEngine ? m_currentCalculatorEngine->GetCurrentResultForRadix(radix, precision) : L"";
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
        return m_currentCalculatorEngine->FInRecordingState() ? true : false;
    }

    void CalculatorManager::SetInHistoryItemLoadMode(_In_ bool isHistoryItemLoadMode)
    {
        m_inHistoryItemLoadMode = isHistoryItemLoadMode;
    }

    /// <summary>
    /// Serialize Rational to vector of long
    /// How Rational is serialized :
    ///     Serialized Rational.P(Number) + Serialized Rational.Q(Number)
    /// How Number is saved :
    ///     [0] = Rational.P.Sign 
    ///     [1] = Rational.P.Mantissa.size
    ///     [2] = Rational.P.Exp
    ///     [3] = Rational.P.Mantissa[0]
    ///     [4] = Rational.P.Mantissa[1]
    ///       ...
    ///     [2 + Rational.P.Mantissa.size] = Rational.P.Mantissa[size - 1]
    /// </summary>
    /// <param name = "rat">Rational number to be serialized</param>
    vector<long> CalculatorManager::SerializeRational(Rational const& rat)
    {
        vector<long> serializedRational{};

        auto serialP = SerializeNumber(rat.P());
        serializedRational.insert(serializedRational.end(), serialP.begin(), serialP.end());

        auto serialQ = SerializeNumber(rat.Q());
        serializedRational.insert(serializedRational.end(), serialQ.begin(), serialQ.end());

        return serializedRational;
    }

    /// <summary>
    /// DeserializeRational vector and construct a Rational
    /// How Rational is serialized :
    ///     Serialized Rational.P(Number) + Serialized Rational.Q(Number)
    /// </summary>
    Rational CalculatorManager::DeSerializeRational(vector<long>::const_iterator itr)
    {
        auto p = DeSerializeNumber(itr);
        auto q = DeSerializeNumber(itr + SERIALIZED_NUMBER_MINSIZE + p.Mantissa().size());

        return Rational(p, q);
    }

    /// <summary>
    /// Serialize Number to vector of long
    /// How Number is saved :
    ///     [0] = Number.Sign 
    ///     [1] = Number.Mantissa.size
    ///     [2] = Number.Exp
    ///     [3] = Number.Mantissa[0]
    ///     [4] = Number.Mantissa[1]
    ///       ...
    ///     [2 + Number.Mantissa.size] = Number.Mantissa[size - 1]
    /// </summary>
    /// <param name = "num">Number to be serialized</param>
    vector<long> CalculatorManager::SerializeNumber(Number const& num)
    {
        vector<long> serializedNumber{};

        serializedNumber.push_back(num.Sign());
        serializedNumber.push_back(static_cast<long>(num.Mantissa().size()));
        serializedNumber.push_back(num.Exp());
        for (auto const& digit : num.Mantissa())
        {
            serializedNumber.push_back(digit);
        }

        return serializedNumber;
    }

    /// <summary>
    /// DeserializeNumber vector and construct a Number
    /// How Number is saved :
    ///     [0] = Number.Sign 
    ///     [1] = Number.Mantissa.size
    ///     [2] = Number.Exp
    ///     [3] = Number.Mantissa[0]
    ///     [4] = Number.Mantissa[1]
    ///       ...
    ///     [2 + Number.Mantissa.size] = Number.Mantissa[size - 1]
    /// </summary>
    Number CalculatorManager::DeSerializeNumber(vector<long>::const_iterator itr)
    {
        int32_t sign = *itr;
        uint32_t size = *(itr + 1);
        int32_t exp = *(itr + 2);
        vector<uint32_t> mant{};
        for (size_t i = 0; i < size; ++i)
        {
            mant.emplace_back(*(itr + 3 + i));
        }

        return Number{ sign, exp, mant };
    }
}
