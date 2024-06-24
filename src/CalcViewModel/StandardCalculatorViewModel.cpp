// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "StandardCalculatorViewModel.h"
#include "Common/CalculatorButtonPressedEventArgs.h"
#include "Common/LocalizationStringUtil.h"
#include "Common/LocalizationSettings.h"
#include "Common/CopyPasteManager.h"
#include "Common/TraceLogger.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel::Common::Automation;
using namespace CalculatorApp::ViewModel;
using namespace CalculationManager;
using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation::Collections;
using namespace concurrency;

constexpr int StandardModePrecision = 16;
constexpr int ScientificModePrecision = 32;
constexpr int ProgrammerModePrecision = 64;

namespace
{
    StringReference IsStandardPropertyName(L"IsStandard");
    StringReference IsScientificPropertyName(L"IsScientific");
    StringReference IsProgrammerPropertyName(L"IsProgrammer");
    StringReference IsAlwaysOnTopPropertyName(L"IsAlwaysOnTop");
    StringReference DisplayValuePropertyName(L"DisplayValue");
    StringReference CalculationResultAutomationNamePropertyName(L"CalculationResultAutomationName");
    StringReference IsBitFlipCheckedPropertyName(L"IsBitFlipChecked");
    StringReference CalcAlwaysOnTop(L"CalcAlwaysOnTop");
    StringReference CalcBackToFullView(L"CalcBackToFullView");

    std::vector<int> GetCommandsFromExpressionCommands(const std::vector<std::shared_ptr<IExpressionCommand>>& expressionCommands)
    {
        vector<int> commands;
        for (const auto& command : expressionCommands)
        {
            CommandType commandType = command->GetCommandType();

            if (commandType == CommandType::UnaryCommand)
            {
                shared_ptr<IUnaryCommand> spCommand = dynamic_pointer_cast<IUnaryCommand>(command);
                const shared_ptr<vector<int>>& unaryCommands = spCommand->GetCommands();

                for (int nUCode : *unaryCommands)
                {
                    commands.push_back(nUCode);
                }
            }

            if (commandType == CommandType::BinaryCommand)
            {
                shared_ptr<IBinaryCommand> spCommand = dynamic_pointer_cast<IBinaryCommand>(command);
                commands.push_back(spCommand->GetCommand());
            }

            if (commandType == CommandType::Parentheses)
            {
                shared_ptr<IParenthesisCommand> spCommand = dynamic_pointer_cast<IParenthesisCommand>(command);
                commands.push_back(spCommand->GetCommand());
            }

            if (commandType == CommandType::OperandCommand)
            {
                shared_ptr<IOpndCommand> spCommand = dynamic_pointer_cast<IOpndCommand>(command);
                const shared_ptr<vector<int>>& opndCommands = spCommand->GetCommands();
                bool fNeedIDCSign = spCommand->IsNegative();

                for (int nOCode : *opndCommands)
                {
                    commands.push_back(nOCode);

                    if (fNeedIDCSign && nOCode != IDC_0)
                    {
                        commands.push_back(static_cast<int>(CalculationManager::Command::CommandSIGN));
                        fNeedIDCSign = false;
                    }
                }
            }
        }
        return commands;
    }
}

namespace CalculatorResourceKeys
{
    StringReference CalculatorExpression(L"Format_CalculatorExpression");
    StringReference CalculatorResults(L"Format_CalculatorResults");
    StringReference CalculatorResults_DecimalSeparator_Announced(L"Format_CalculatorResults_Decimal");
    StringReference HexButton(L"Format_HexButtonValue");
    StringReference DecButton(L"Format_DecButtonValue");
    StringReference OctButton(L"Format_OctButtonValue");
    StringReference BinButton(L"Format_BinButtonValue");
    StringReference OpenParenthesisCountAutomationFormat(L"Format_OpenParenthesisCountAutomationNamePrefix");
    StringReference NoParenthesisAdded(L"NoRightParenthesisAdded_Announcement");
    StringReference MaxDigitsReachedFormat(L"Format_MaxDigitsReached");
    StringReference ButtonPressFeedbackFormat(L"Format_ButtonPressAuditoryFeedback");
    StringReference MemorySave(L"Format_MemorySave");
    StringReference MemoryItemChanged(L"Format_MemorySlotChanged");
    StringReference MemoryItemCleared(L"Format_MemorySlotCleared");
    StringReference MemoryCleared(L"Memory_Cleared");
    StringReference DisplayCopied(L"Display_Copied");
}

StandardCalculatorViewModel::StandardCalculatorViewModel()
    : m_DisplayValue(L"0")
    , m_DecimalDisplayValue(L"0")
    , m_HexDisplayValue(L"0")
    , m_BinaryDisplayValue(L"0")
    , m_OctalDisplayValue(L"0")
    , m_BinaryDigits(ref new Vector<bool>(64, false))
    , m_standardCalculatorManager(&m_calculatorDisplay, &m_resourceProvider)
    , m_ExpressionTokens(ref new Vector<DisplayExpressionToken ^>())
    , m_MemorizedNumbers(ref new Vector<MemoryItemViewModel ^>())
    , m_IsMemoryEmpty(true)
    , m_IsFToEChecked(false)
    , m_IsShiftProgrammerChecked(false)
    , m_valueBitLength(BitLength::BitLengthQWord)
    , m_isBitFlipChecked(false)
    , m_IsBinaryBitFlippingEnabled(false)
    , m_CurrentRadixType(NumberBase::DecBase)
    , m_CurrentAngleType(NumbersAndOperatorsEnum::Degree)
    , m_Announcement(nullptr)
    , m_OpenParenthesisCount(0)
    , m_feedbackForButtonPress(nullptr)
    , m_isRtlLanguage(false)
    , m_localizedMaxDigitsReachedAutomationFormat(nullptr)
    , m_localizedButtonPressFeedbackAutomationFormat(nullptr)
    , m_localizedMemorySavedAutomationFormat(nullptr)
    , m_localizedMemoryItemChangedAutomationFormat(nullptr)
    , m_localizedMemoryItemClearedAutomationFormat(nullptr)
    , m_localizedMemoryCleared(nullptr)
    , m_localizedOpenParenthesisCountChangedAutomationFormat(nullptr)
    , m_localizedNoRightParenthesisAddedFormat(nullptr)
    , m_TokenPosition(-1)
    , m_isLastOperationHistoryLoad(false)
{
    WeakReference calculatorViewModel(this);
    auto appResourceProvider = AppResourceProvider::GetInstance();
    m_calculatorDisplay.SetCallback(calculatorViewModel);
    m_expressionAutomationNameFormat = appResourceProvider->GetResourceString(CalculatorResourceKeys::CalculatorExpression);
    m_localizedCalculationResultAutomationFormat = appResourceProvider->GetResourceString(CalculatorResourceKeys::CalculatorResults);
    m_localizedCalculationResultDecimalAutomationFormat =
        appResourceProvider->GetResourceString(CalculatorResourceKeys::CalculatorResults_DecimalSeparator_Announced);
    m_localizedHexaDecimalAutomationFormat = appResourceProvider->GetResourceString(CalculatorResourceKeys::HexButton);
    m_localizedDecimalAutomationFormat = appResourceProvider->GetResourceString(CalculatorResourceKeys::DecButton);
    m_localizedOctalAutomationFormat = appResourceProvider->GetResourceString(CalculatorResourceKeys::OctButton);
    m_localizedBinaryAutomationFormat = appResourceProvider->GetResourceString(CalculatorResourceKeys::BinButton);

    // Initialize the Automation Name
    CalculationResultAutomationName = GetLocalizedStringFormat(m_localizedCalculationResultAutomationFormat, m_DisplayValue);
    CalculationExpressionAutomationName = GetLocalizedStringFormat(m_expressionAutomationNameFormat, L"");

    // Initialize history view model
    m_HistoryVM = ref new HistoryViewModel(&m_standardCalculatorManager);
    m_HistoryVM->SetCalculatorDisplay(m_calculatorDisplay);

    m_decimalSeparator = LocalizationSettings::GetInstance()->GetDecimalSeparator();

    if (CoreWindow::GetForCurrentThread() != nullptr)
    {
        // Must have a CoreWindow to access the resource context.
        m_isRtlLanguage = LocalizationService::GetInstance()->IsRtlLayout();
    }

    IsEditingEnabled = false;
    IsUnaryOperatorEnabled = true;
    IsBinaryOperatorEnabled = true;
    IsOperandEnabled = true;
    IsNegateEnabled = true;
    IsDecimalEnabled = true;
    AreProgrammerRadixOperatorsVisible = false;
}

String ^ StandardCalculatorViewModel::LocalizeDisplayValue(_In_ wstring const& displayValue)
{
    wstring result(displayValue);

    // Adds leading padding 0's to Programmer Mode's Binary Display
    if (IsProgrammer && CurrentRadixType == NumberBase::BinBase)
    {
        result = AddPadding(result);
    }

    LocalizationSettings::GetInstance()->LocalizeDisplayValue(&result);
    return ref new Platform::String(result.c_str());
}

String ^ StandardCalculatorViewModel::CalculateNarratorDisplayValue(_In_ wstring const& displayValue, _In_ String ^ localizedDisplayValue)
{
    String ^ localizedValue = localizedDisplayValue;
    String ^ automationFormat = m_localizedCalculationResultAutomationFormat;

    // The narrator doesn't read the decimalSeparator if it's the last character
    if (Utils::IsLastCharacterTarget(displayValue, m_decimalSeparator))
    {
        // remove the decimal separator, to avoid a long pause between words
        localizedValue = LocalizeDisplayValue(displayValue.substr(0, displayValue.length() - 1));

        // Use a format which has a word in the decimal separator's place
        // "The Display is 10 point"
        automationFormat = m_localizedCalculationResultDecimalAutomationFormat;
    }

    // In Programmer modes using non-base10, we want the strings to be read as literal digits.
    if (IsProgrammer && CurrentRadixType != NumberBase::DecBase)
    {
        localizedValue = GetNarratorStringReadRawNumbers(localizedValue);
    }

    return GetLocalizedStringFormat(automationFormat, localizedValue);
}

String ^ StandardCalculatorViewModel::GetNarratorStringReadRawNumbers(_In_ String ^ localizedDisplayValue)
{
    wstring ws;
    LocalizationSettings^ locSettings = LocalizationSettings::GetInstance();

    // Insert a space after each digit in the string, to force Narrator to read them as separate numbers.
    for (const wchar_t& c : localizedDisplayValue)
    {
        ws += c;
        if (locSettings->IsLocalizedHexDigit(c))
        {
            ws += L' ';
        }
    }

    return ref new String(ws.c_str());
}

void StandardCalculatorViewModel::SetPrimaryDisplay(_In_ String ^ displayStringValue, _In_ bool isError)
{
    String ^ localizedDisplayStringValue = LocalizeDisplayValue(displayStringValue->Data());

    // Set this variable before the DisplayValue is modified, Otherwise the DisplayValue will
    // not match what the narrator is saying
    m_CalculationResultAutomationName = CalculateNarratorDisplayValue(displayStringValue->Data(), localizedDisplayStringValue);

    AreAlwaysOnTopResultsUpdated = false;
    if (DisplayValue != localizedDisplayStringValue)
    {
        DisplayValue = localizedDisplayStringValue;
        AreAlwaysOnTopResultsUpdated = true;
    }

    IsInError = isError;

    if (IsProgrammer)
    {
        UpdateProgrammerPanelDisplay();
    }
}

void StandardCalculatorViewModel::DisplayPasteError()
{
    m_standardCalculatorManager.DisplayPasteError();
}

void StandardCalculatorViewModel::SetParenthesisCount(_In_ unsigned int parenthesisCount)
{
    if (m_OpenParenthesisCount == parenthesisCount)
    {
        return;
    }

    OpenParenthesisCount = parenthesisCount;
    if (IsProgrammer || IsScientific)
    {
        SetOpenParenthesisCountNarratorAnnouncement();
    }
}

void StandardCalculatorViewModel::SetOpenParenthesisCountNarratorAnnouncement()
{
    wstring localizedParenthesisCount = to_wstring(m_OpenParenthesisCount).c_str();
    LocalizationSettings::GetInstance()->LocalizeDisplayValue(&localizedParenthesisCount);

    if (m_localizedOpenParenthesisCountChangedAutomationFormat == nullptr)
    {
        m_localizedOpenParenthesisCountChangedAutomationFormat =
            AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::OpenParenthesisCountAutomationFormat);
    }
    String ^ announcement =
        LocalizationStringUtil::GetLocalizedString(m_localizedOpenParenthesisCountChangedAutomationFormat, StringReference(localizedParenthesisCount.c_str()));

    Announcement = CalculatorAnnouncement::GetOpenParenthesisCountChangedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnNoRightParenAdded()
{
    SetNoParenAddedNarratorAnnouncement();
}

void StandardCalculatorViewModel::SetNoParenAddedNarratorAnnouncement()
{
    if (m_localizedNoRightParenthesisAddedFormat == nullptr)
    {
        m_localizedNoRightParenthesisAddedFormat = AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::NoParenthesisAdded);
    }

    Announcement = CalculatorAnnouncement::GetNoRightParenthesisAddedAnnouncement(m_localizedNoRightParenthesisAddedFormat);
}

void StandardCalculatorViewModel::DisableButtons(CommandType selectedExpressionCommandType)
{
    if (selectedExpressionCommandType == CommandType::OperandCommand)
    {
        IsBinaryOperatorEnabled = false;
        IsUnaryOperatorEnabled = false;
        IsOperandEnabled = true;
        IsNegateEnabled = true;
        IsDecimalEnabled = true;
    }
    if (selectedExpressionCommandType == CommandType::BinaryCommand)
    {
        IsBinaryOperatorEnabled = true;
        IsUnaryOperatorEnabled = false;
        IsOperandEnabled = false;
        IsNegateEnabled = false;
        IsDecimalEnabled = false;
    }
    if (selectedExpressionCommandType == CommandType::UnaryCommand)
    {
        IsBinaryOperatorEnabled = false;
        IsUnaryOperatorEnabled = true;
        IsOperandEnabled = false;
        IsNegateEnabled = true;
        IsDecimalEnabled = false;
    }
}

void StandardCalculatorViewModel::SetExpressionDisplay(
    _Inout_ shared_ptr<std::vector<pair<wstring, int>>> const& tokens,
    _Inout_ shared_ptr<std::vector<shared_ptr<IExpressionCommand>>> const& commands)
{
    m_tokens = tokens;
    m_commands = commands;
    if (!IsEditingEnabled)
    {
        SetTokens(tokens);
    }

    CalculationExpressionAutomationName = GetCalculatorExpressionAutomationName();

    AreTokensUpdated = true;
}

void StandardCalculatorViewModel::SetHistoryExpressionDisplay(
    _Inout_ shared_ptr<vector<pair<wstring, int>>> const& tokens,
    _Inout_ shared_ptr<vector<shared_ptr<IExpressionCommand>>> const& commands)
{
    m_tokens = make_shared<vector<pair<wstring, int>>>(*tokens);
    m_commands = make_shared<vector<shared_ptr<IExpressionCommand>>>(*commands);
    IsEditingEnabled = false;

    // Setting the History Item Load Mode so that UI does not get updated with recalculation of every token
    m_standardCalculatorManager.SetInHistoryItemLoadMode(true);
    Recalculate(true);
    m_standardCalculatorManager.SetInHistoryItemLoadMode(false);
    m_isLastOperationHistoryLoad = true;
}

void StandardCalculatorViewModel::SetTokens(_Inout_ shared_ptr<vector<pair<wstring, int>>> const& tokens)
{
    AreTokensUpdated = false;

    const size_t nTokens = tokens->size();

    if (nTokens == 0)
    {
        m_ExpressionTokens->Clear();
        return;
    }

    LocalizationSettings^ localizer = LocalizationSettings::GetInstance();

    const wstring separator = L" ";
    for (unsigned int i = 0; i < nTokens; ++i)
    {
        auto currentToken = (*tokens)[i];

        Common::TokenType type;
        bool isEditable = currentToken.second != -1;
        localizer->LocalizeDisplayValue(&(currentToken.first));

        if (!isEditable)
        {
            type = currentToken.first == separator ? TokenType::Separator : TokenType::Operator;
        }
        else
        {
            const shared_ptr<IExpressionCommand>& command = m_commands->at(currentToken.second);
            type = command->GetCommandType() == CommandType::OperandCommand ? TokenType::Operand : TokenType::Operator;
        }

        auto currentTokenString = StringReference(currentToken.first.c_str());
        if (i < m_ExpressionTokens->Size)
        {
            auto existingItem = m_ExpressionTokens->GetAt(i);
            if (type == existingItem->Type && existingItem->Token->Equals(currentTokenString))
            {
                existingItem->TokenPosition = i;
                existingItem->IsTokenEditable = isEditable;
                existingItem->CommandIndex = 0;
            }
            else
            {
                auto expressionToken = ref new DisplayExpressionToken(currentTokenString, i, isEditable, type);
                m_ExpressionTokens->InsertAt(i, expressionToken);
            }
        }
        else
        {
            auto expressionToken = ref new DisplayExpressionToken(currentTokenString, i, isEditable, type);
            m_ExpressionTokens->Append(expressionToken);
        }
    }

    while (m_ExpressionTokens->Size != nTokens)
    {
        m_ExpressionTokens->RemoveAtEnd();
    }
}

String ^ StandardCalculatorViewModel::GetCalculatorExpressionAutomationName()
{
    String ^ expression = L"";
    for (auto&& token : m_ExpressionTokens)
    {
        expression += LocalizationService::GetNarratorReadableToken(token->Token);
    }

    return GetLocalizedStringFormat(m_expressionAutomationNameFormat, expression);
}

void StandardCalculatorViewModel::SetMemorizedNumbers(const vector<wstring>& newMemorizedNumbers)
{
    LocalizationSettings^ localizer = LocalizationSettings::GetInstance();
    if (newMemorizedNumbers.size() == 0) // Memory has been cleared
    {
        MemorizedNumbers->Clear();
        IsMemoryEmpty = true;
    }
    // A new value is added to the memory
    else if (newMemorizedNumbers.size() > MemorizedNumbers->Size)
    {
        while (newMemorizedNumbers.size() > MemorizedNumbers->Size)
        {
            size_t newValuePosition = newMemorizedNumbers.size() - MemorizedNumbers->Size - 1;
            auto stringValue = newMemorizedNumbers.at(newValuePosition);

            MemoryItemViewModel ^ memorySlot = ref new MemoryItemViewModel(this);
            memorySlot->Position = 0;
            localizer->LocalizeDisplayValue(&stringValue);
            memorySlot->Value = ref new String(stringValue.c_str());

            MemorizedNumbers->InsertAt(0, memorySlot);
            IsMemoryEmpty = IsAlwaysOnTop;

            // Update the slot position for the rest of the slots
            for (unsigned int i = 1; i < MemorizedNumbers->Size; i++)
            {
                MemorizedNumbers->GetAt(i)->Position++;
            }
        }
    }
    else if (newMemorizedNumbers.size() == MemorizedNumbers->Size) // Either M+ or M-
    {
        for (unsigned int i = 0; i < MemorizedNumbers->Size; i++)
        {
            auto newStringValue = newMemorizedNumbers.at(i);
            localizer->LocalizeDisplayValue(&newStringValue);

            // If the value is different, update the value
            if (MemorizedNumbers->GetAt(i)->Value != StringReference(newStringValue.c_str()))
            {
                MemorizedNumbers->GetAt(i)->Value = ref new String(newStringValue.c_str());
            }
        }
    }
}

void StandardCalculatorViewModel::FtoEButtonToggled()
{
    OnButtonPressed(NumbersAndOperatorsEnum::FToE);
}

void StandardCalculatorViewModel::EngButton()
{
    OnButtonPressed(NumbersAndOperatorsEnum::Engineering);
}


void StandardCalculatorViewModel::HandleUpdatedOperandData(Command cmdenum)
{
    DisplayExpressionToken ^ displayExpressionToken = ExpressionTokens->GetAt(m_TokenPosition);
    if (displayExpressionToken == nullptr)
    {
        return;
    }
    if ((displayExpressionToken->Token == nullptr) || (displayExpressionToken->Token->Length() == 0))
    {
        displayExpressionToken->CommandIndex = 0;
    }

    wchar_t ch = 0;
    if ((cmdenum >= Command::Command0) && (cmdenum <= Command::Command9))
    {
        switch (cmdenum)
        {
        case Command::Command0:
            ch = L'0';
            break;
        case Command::Command1:
            ch = L'1';
            break;
        case Command::Command2:
            ch = L'2';
            break;
        case Command::Command3:
            ch = L'3';
            break;
        case Command::Command4:
            ch = L'4';
            break;
        case Command::Command5:
            ch = L'5';
            break;
        case Command::Command6:
            ch = L'6';
            break;
        case Command::Command7:
            ch = L'7';
            break;
        case Command::Command8:
            ch = L'8';
            break;
        case Command::Command9:
            ch = L'9';
            break;
        }
    }
    else if (cmdenum == Command::CommandPNT)
    {
        ch = L'.';
    }
    else if (cmdenum == Command::CommandBACK)
    {
        ch = L'x';
    }
    else
    {
        return;
    }

    int length = 0;
    wchar_t* temp = new wchar_t[100];
    const wchar_t* data = m_selectedExpressionLastData->Data();
    int i = 0, j = 0;
    int commandIndex = displayExpressionToken->CommandIndex;

    if (IsOperandTextCompletelySelected)
    {
        // Clear older text;
        m_selectedExpressionLastData = L"";
        if (ch == L'x')
        {
            temp[0] = L'\0';
            commandIndex = 0;
        }
        else
        {
            temp[0] = ch;
            temp[1] = L'\0';
            commandIndex = 1;
        }
        IsOperandTextCompletelySelected = false;
    }
    else
    {
        if (ch == L'x')
        {
            if (commandIndex == 0)
            {
                delete[] temp;
                return;
            }

            length = m_selectedExpressionLastData->Length();
            for (; j < length; ++j)
            {
                if (j == commandIndex - 1)
                {
                    continue;
                }
                temp[i++] = data[j];
            }
            temp[i] = L'\0';
            commandIndex -= 1;
        }
        else
        {
            length = m_selectedExpressionLastData->Length() + 1;
            if (length > 50)
            {
                delete[] temp;
                return;
            }
            for (; i < length; ++i)
            {
                if (i == commandIndex)
                {
                    temp[i] = ch;
                    continue;
                }
                temp[i] = data[j++];
            }
            temp[i] = L'\0';
            commandIndex += 1;
        }
    }

    String ^ updatedData = ref new String(temp);
    UpdateOperand(m_TokenPosition, updatedData);
    displayExpressionToken->Token = updatedData;
    IsOperandUpdatedUsingViewModel = true;
    displayExpressionToken->CommandIndex = commandIndex;
}

bool StandardCalculatorViewModel::IsOperator(Command cmdenum)
{
    if ((cmdenum >= Command::Command0 && cmdenum <= Command::Command9) || (cmdenum == Command::CommandPNT) || (cmdenum == Command::CommandBACK)
        || (cmdenum == Command::CommandEXP) || (cmdenum == Command::CommandFE) || (cmdenum == Command::ModeBasic) || (cmdenum == Command::ModeProgrammer)
        || (cmdenum == Command::ModeScientific) || (cmdenum == Command::CommandINV) || (cmdenum == Command::CommandCENTR) || (cmdenum == Command::CommandDEG)
        || (cmdenum == Command::CommandRAD) || (cmdenum == Command::CommandGRAD)
        || ((cmdenum >= Command::CommandBINEDITSTART) && (cmdenum <= Command::CommandBINEDITEND)))
    {
        return false;
    }
    return true;
}

void StandardCalculatorViewModel::OnButtonPressed(Object ^ parameter)
{
    m_feedbackForButtonPress = CalculatorButtonPressedEventArgs::GetAuditoryFeedbackFromCommandParameter(parameter);
    NumbersAndOperatorsEnum numOpEnum = CalculatorButtonPressedEventArgs::GetOperationFromCommandParameter(parameter);
    Command cmdenum = ConvertToOperatorsEnum(numOpEnum);

    if (IsInError)
    {
        m_standardCalculatorManager.SendCommand(Command::CommandCLEAR);

        if (!IsRecoverableCommand(static_cast<Command>(numOpEnum)))
        {
            return;
        }
    }

    if (IsEditingEnabled && numOpEnum != NumbersAndOperatorsEnum::IsScientificMode && numOpEnum != NumbersAndOperatorsEnum::IsStandardMode
        && numOpEnum != NumbersAndOperatorsEnum::IsProgrammerMode && numOpEnum != NumbersAndOperatorsEnum::FToE
        && (numOpEnum != NumbersAndOperatorsEnum::Degree) && (numOpEnum != NumbersAndOperatorsEnum::Radians) && (numOpEnum != NumbersAndOperatorsEnum::Grads))
    {
        if (!m_KeyPressed)
        {
            SaveEditedCommand(m_selectedExpressionToken->TokenPosition, cmdenum);
        }
    }
    else
    {
        if (numOpEnum == NumbersAndOperatorsEnum::IsStandardMode || numOpEnum == NumbersAndOperatorsEnum::IsScientificMode
            || numOpEnum == NumbersAndOperatorsEnum::IsProgrammerMode)
        {
            IsEditingEnabled = false;
        }
        if (numOpEnum == NumbersAndOperatorsEnum::Memory)
        {
            OnMemoryButtonPressed();
        }
        else
        {
            if (numOpEnum == NumbersAndOperatorsEnum::Clear || numOpEnum == NumbersAndOperatorsEnum::ClearEntry
                || numOpEnum == NumbersAndOperatorsEnum::IsStandardMode || numOpEnum == NumbersAndOperatorsEnum::IsProgrammerMode)
            {
                // On Clear('C') the F-E button needs to be UnChecked if it in Checked state.
                // Also, the Primary Display Value should not show in exponential format.
                // Hence the check below to ensure parity with Desktop Calculator.
                // Clear the FE mode if the switching to StandardMode, since 'C'/'CE' in StandardMode
                // doesn't honor the FE button.
                if (IsFToEChecked)
                {
                    IsFToEChecked = false;
                }
            }
            if (numOpEnum == NumbersAndOperatorsEnum::Degree || numOpEnum == NumbersAndOperatorsEnum::Radians || numOpEnum == NumbersAndOperatorsEnum::Grads)
            {
                m_CurrentAngleType = numOpEnum;
            }
            if ((cmdenum >= Command::Command0 && cmdenum <= Command::Command9) || (cmdenum == Command::CommandPNT) || (cmdenum == Command::CommandBACK)
                || (cmdenum == Command::CommandEXP))
            {
                IsOperatorCommand = false;
            }
            else
            {
                IsOperatorCommand = true;
            }

            if (m_isLastOperationHistoryLoad
                && ((numOpEnum != NumbersAndOperatorsEnum::Degree) && (numOpEnum != NumbersAndOperatorsEnum::Radians)
                    && (numOpEnum != NumbersAndOperatorsEnum::Grads)))
            {
                IsFToEEnabled = true;
                m_isLastOperationHistoryLoad = false;
            }

            TraceLogger::GetInstance()->UpdateButtonUsage(numOpEnum, GetCalculatorMode());
            m_standardCalculatorManager.SendCommand(cmdenum);
        }
    }
}

RadixType StandardCalculatorViewModel::GetRadixTypeFromNumberBase(NumberBase base)
{
    switch (base)
    {
    case NumberBase::BinBase:
        return RadixType::Binary;
    case NumberBase::HexBase:
        return RadixType::Hex;
    case NumberBase::OctBase:
        return RadixType::Octal;
    default:
        return RadixType::Decimal;
    }
}

void StandardCalculatorViewModel::OnCopyCommand(Object ^ parameter)
{
    CopyPasteManager::CopyToClipboard(GetRawDisplayValue());

    String ^ announcement = AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::DisplayCopied);
    Announcement = CalculatorAnnouncement::GetDisplayCopiedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnPasteCommand(Object ^ parameter)
{
    auto that(this);
    ViewMode mode;
    BitLength bitLengthType = BitLength::BitLengthUnknown;
    NumberBase numberBase = NumberBase::Unknown;
    if (IsScientific)
    {
        mode = ViewMode::Scientific;
    }
    else if (IsProgrammer)
    {
        mode = ViewMode::Programmer;
        bitLengthType = m_valueBitLength;
        numberBase = CurrentRadixType;
    }
    else
    {
        mode = ViewMode::Standard;
    }
    // if there's nothing to copy early out
    if (IsEditingEnabled || !CopyPasteManager::HasStringToPaste())
    {
        return;
    }

    // Ensure that the paste happens on the UI thread
    create_task(CopyPasteManager::GetStringToPaste(mode, NavCategoryStates::GetGroupType(mode), numberBase, bitLengthType))
        .then([that, mode](String ^ pastedString) { that->OnPaste(pastedString); }, concurrency::task_continuation_context::use_current());
}

CalculationManager::Command StandardCalculatorViewModel::ConvertToOperatorsEnum(NumbersAndOperatorsEnum operation)
{
    return safe_cast<Command>(operation);
}

void StandardCalculatorViewModel::OnPaste(String ^ pastedString)
{
    // If pastedString is invalid("NoOp") then display pasteError else process the string
    if (CopyPasteManager::IsErrorMessage(pastedString))
    {
        this->DisplayPasteError();
        return;
    }

    TraceLogger::GetInstance()->LogInputPasted(GetCalculatorMode());
    bool isFirstLegalChar = true;
    m_standardCalculatorManager.SendCommand(Command::CommandCENTR);
    bool sendNegate = false;
    bool processedDigit = false;
    bool sentEquals = false;
    bool isPreviousOperator = false;

    vector<bool> negateStack;

    // Iterate through each character pasted, and if it's valid, send it to the model.
    auto it = pastedString->Begin();

    while (it != pastedString->End())
    {
        bool sendCommand = true;
        auto buttonInfo = MapCharacterToButtonId(*it);

        NumbersAndOperatorsEnum mappedNumOp = buttonInfo.buttonId;
        bool canSendNegate = buttonInfo.canSendNegate;

        if (mappedNumOp == NumbersAndOperatorsEnum::None)
        {
            ++it;
            continue;
        }

        if (isFirstLegalChar || isPreviousOperator)
        {
            isFirstLegalChar = false;
            isPreviousOperator = false;

            // If the character is a - sign, send negate
            // after sending the next legal character.  Send nothing now, or
            // it will be ignored.
            if (NumbersAndOperatorsEnum::Subtract == mappedNumOp)
            {
                sendNegate = true;
                sendCommand = false;
            }

            // Support (+) sign prefix
            if (NumbersAndOperatorsEnum::Add == mappedNumOp)
            {
                sendCommand = false;
            }
        }

        switch (mappedNumOp)
        {
            // Opening parenthesis starts a new expression and pushes negation state onto the stack
        case NumbersAndOperatorsEnum::OpenParenthesis:
            negateStack.push_back(sendNegate);
            sendNegate = false;
            break;

            // Closing parenthesis pops the negation state off the stack and sends it down to the calc engine
        case NumbersAndOperatorsEnum::CloseParenthesis:
            if (!negateStack.empty())
            {
                sendNegate = negateStack.back();
                negateStack.pop_back();
                canSendNegate = true;
            }
            else
            {
                // Don't send a closing parenthesis if a matching opening parenthesis hasn't been sent already
                sendCommand = false;
            }
            break;

        case NumbersAndOperatorsEnum::Zero:
        case NumbersAndOperatorsEnum::One:
        case NumbersAndOperatorsEnum::Two:
        case NumbersAndOperatorsEnum::Three:
        case NumbersAndOperatorsEnum::Four:
        case NumbersAndOperatorsEnum::Five:
        case NumbersAndOperatorsEnum::Six:
        case NumbersAndOperatorsEnum::Seven:
        case NumbersAndOperatorsEnum::Eight:
        case NumbersAndOperatorsEnum::Nine:
            processedDigit = true;
            break;

        case NumbersAndOperatorsEnum::Add:
        case NumbersAndOperatorsEnum::Subtract:
        case NumbersAndOperatorsEnum::Multiply:
        case NumbersAndOperatorsEnum::Divide:
            isPreviousOperator = true;
            break;
        }

        if (sendCommand)
        {
            sentEquals = (mappedNumOp == NumbersAndOperatorsEnum::Equals);
            Command cmdenum = ConvertToOperatorsEnum(mappedNumOp);
            m_standardCalculatorManager.SendCommand(cmdenum);

            // The CalcEngine state machine won't allow the negate command to be sent before any
            // other digits, so instead a flag is set and the command is sent after the first appropriate
            // command.
            if (sendNegate)
            {
                if (canSendNegate)
                {
                    Command cmdNegate = ConvertToOperatorsEnum(NumbersAndOperatorsEnum::Negate);
                    m_standardCalculatorManager.SendCommand(cmdNegate);
                }

                // Can't send negate on a leading zero, so wait until the appropriate time to send it.
                if (NumbersAndOperatorsEnum::Zero != mappedNumOp && NumbersAndOperatorsEnum::Decimal != mappedNumOp)
                {
                    sendNegate = false;
                }
            }
        }

        // Handle exponent and exponent sign (...e+... or ...e-... or ...e...)
        if (mappedNumOp == NumbersAndOperatorsEnum::Exp)
        {
            // Check the following item
            switch (MapCharacterToButtonId(*(it + 1)).buttonId)
            {
            case NumbersAndOperatorsEnum::Subtract:
            {
                Command cmdNegate = ConvertToOperatorsEnum(NumbersAndOperatorsEnum::Negate);
                m_standardCalculatorManager.SendCommand(cmdNegate);
                ++it;
            }
            break;
            case NumbersAndOperatorsEnum::Add:
            {
                // Nothing to do, skip to the next item
                ++it;
            }
            break;
            }
        }

        ++it;
    }
}

void StandardCalculatorViewModel::OnClearMemoryCommand(Object ^ parameter)
{
    m_standardCalculatorManager.MemorizedNumberClearAll();

    TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::MemoryClear, GetCalculatorMode());

    if (m_localizedMemoryCleared == nullptr)
    {
        m_localizedMemoryCleared = AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::MemoryCleared);
    }
    Announcement = CalculatorAnnouncement::GetMemoryClearedAnnouncement(m_localizedMemoryCleared);
}

ButtonInfo StandardCalculatorViewModel::MapCharacterToButtonId(char16 ch)
{
    ButtonInfo result;
    result.buttonId = NumbersAndOperatorsEnum::None;
    result.canSendNegate = false;

    switch (ch)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        result.buttonId = NumbersAndOperatorsEnum::Zero + static_cast<NumbersAndOperatorsEnum>(ch - L'0');
        result.canSendNegate = true;
        break;

    case '*':
        result.buttonId = NumbersAndOperatorsEnum::Multiply;
        break;

    case '+':
        result.buttonId = NumbersAndOperatorsEnum::Add;
        break;

    case '-':
        result.buttonId = NumbersAndOperatorsEnum::Subtract;
        break;

    case '/':
        result.buttonId = NumbersAndOperatorsEnum::Divide;
        break;

    case '^':
        if (IsScientific)
        {
            result.buttonId = NumbersAndOperatorsEnum::XPowerY;
        }
        break;

    case '%':
        if (IsScientific || IsProgrammer)
        {
            result.buttonId = NumbersAndOperatorsEnum::Mod;
        }
        break;

    case '=':
        result.buttonId = NumbersAndOperatorsEnum::Equals;
        break;

    case '(':
        result.buttonId = NumbersAndOperatorsEnum::OpenParenthesis;
        break;

    case ')':
        result.buttonId = NumbersAndOperatorsEnum::CloseParenthesis;
        break;

    case 'a':
    case 'A':
        result.buttonId = NumbersAndOperatorsEnum::A;
        break;
    case 'b':
    case 'B':
        result.buttonId = NumbersAndOperatorsEnum::B;
        break;
    case 'c':
    case 'C':
        result.buttonId = NumbersAndOperatorsEnum::C;
        break;
    case 'd':
    case 'D':
        result.buttonId = NumbersAndOperatorsEnum::D;
        break;
    case 'e':
    case 'E':
        // Only allow scientific notation in scientific mode
        if (IsProgrammer)
        {
            result.buttonId = NumbersAndOperatorsEnum::E;
        }
        else
        {
            result.buttonId = NumbersAndOperatorsEnum::Exp;
        }
        break;
    case 'f':
    case 'F':
        result.buttonId = NumbersAndOperatorsEnum::F;
        break;
    default:
        // For the decimalSeparator, we need to respect the user setting.
        if (ch == m_decimalSeparator)
        {
            result.buttonId = NumbersAndOperatorsEnum::Decimal;
        }
        break;
    }

    if (result.buttonId == NumbersAndOperatorsEnum::None)
    {
        if (LocalizationSettings::GetInstance()->IsLocalizedDigit(ch))
        {
            result.buttonId =
                NumbersAndOperatorsEnum::Zero + static_cast<NumbersAndOperatorsEnum>(ch - LocalizationSettings::GetInstance()->GetDigitSymbolFromEnUsDigit('0'));
            result.canSendNegate = true;
        }
    }

    // Negate cannot be sent for leading zeroes
    if (NumbersAndOperatorsEnum::Zero == result.buttonId)
    {
        result.canSendNegate = false;
    }

    return result;
}
void StandardCalculatorViewModel::OnInputChanged()
{
    IsInputEmpty = m_standardCalculatorManager.IsInputEmpty();
}

void StandardCalculatorViewModel::OnMemoryButtonPressed()
{
    m_standardCalculatorManager.MemorizeNumber();

    TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::Memory, GetCalculatorMode());

    if (m_localizedMemorySavedAutomationFormat == nullptr)
    {
        m_localizedMemorySavedAutomationFormat = AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::MemorySave);
    }
    String ^ announcement = LocalizationStringUtil::GetLocalizedString(m_localizedMemorySavedAutomationFormat, m_DisplayValue);
    Announcement = CalculatorAnnouncement::GetMemoryItemAddedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnMemoryItemChanged(unsigned int indexOfMemory)
{
    if (indexOfMemory < MemorizedNumbers->Size)
    {
        MemoryItemViewModel ^ memSlot = MemorizedNumbers->GetAt(indexOfMemory);
        String ^ localizedValue = memSlot->Value;

        wstring localizedIndex = to_wstring(indexOfMemory + 1);
        LocalizationSettings::GetInstance()->LocalizeDisplayValue(&localizedIndex);

        if (m_localizedMemoryItemChangedAutomationFormat == nullptr)
        {
            m_localizedMemoryItemChangedAutomationFormat = AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::MemoryItemChanged);
        }
        String ^ announcement =
            LocalizationStringUtil::GetLocalizedString(m_localizedMemoryItemChangedAutomationFormat, StringReference(localizedIndex.c_str()), localizedValue);
        Announcement = CalculatorAnnouncement::GetMemoryItemChangedAnnouncement(announcement);
    }
}

void StandardCalculatorViewModel::OnMemoryItemPressed(Object ^ memoryItemPosition)
{
    if (MemorizedNumbers && MemorizedNumbers->Size > 0)
    {
        auto boxedPosition = safe_cast<Box<int> ^>(memoryItemPosition);
        m_standardCalculatorManager.MemorizedNumberLoad(boxedPosition->Value);
        HideMemoryClicked();

        auto mode = IsStandard ? ViewMode::Standard : IsScientific ? ViewMode::Scientific : ViewMode::Programmer;
        TraceLogger::GetInstance()->LogMemoryItemLoad(mode, MemorizedNumbers->Size, boxedPosition->Value);
    }
}

void StandardCalculatorViewModel::OnMemoryAdd(Object ^ memoryItemPosition)
{
    // M+ will add display to memorylist if memory list is empty.

    if (MemorizedNumbers)
    {
        auto boxedPosition = safe_cast<Box<int> ^>(memoryItemPosition);
        TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::MemoryAdd, GetCalculatorMode());
        m_standardCalculatorManager.MemorizedNumberAdd(boxedPosition->Value);
    }
}

void StandardCalculatorViewModel::OnMemorySubtract(Object ^ memoryItemPosition)
{
    // M- will add negative of displayed number to memorylist if memory list is empty.
    if (MemorizedNumbers)
    {
        auto boxedPosition = safe_cast<Box<int> ^>(memoryItemPosition);
        TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::MemorySubtract, GetCalculatorMode());
        m_standardCalculatorManager.MemorizedNumberSubtract(boxedPosition->Value);
    }
}

void StandardCalculatorViewModel::OnMemoryClear(_In_ Object ^ memoryItemPosition)
{
    if (MemorizedNumbers && MemorizedNumbers->Size > 0)
    {
        auto boxedPosition = safe_cast<Box<int> ^>(memoryItemPosition);

        if (boxedPosition->Value >= 0)
        {
            unsigned int unsignedPosition = safe_cast<unsigned int>(boxedPosition->Value);
            m_standardCalculatorManager.MemorizedNumberClear(unsignedPosition);

            MemorizedNumbers->RemoveAt(unsignedPosition);
            for (unsigned int i = 0; i < MemorizedNumbers->Size; i++)
            {
                MemorizedNumbers->GetAt(i)->Position = i;
            }

            if (MemorizedNumbers->Size == 0)
            {
                IsMemoryEmpty = true;
            }
            TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::MemoryClear, GetCalculatorMode());

            wstring localizedIndex = to_wstring(boxedPosition->Value + 1);
            LocalizationSettings::GetInstance()->LocalizeDisplayValue(&localizedIndex);

            if (m_localizedMemoryItemClearedAutomationFormat == nullptr)
            {
                m_localizedMemoryItemClearedAutomationFormat = AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::MemoryItemCleared);
            }
            String ^ announcement =
                LocalizationStringUtil::GetLocalizedString(m_localizedMemoryItemClearedAutomationFormat, StringReference(localizedIndex.c_str()));

            Announcement = CalculatorAnnouncement::GetMemoryClearedAnnouncement(announcement);
        }
    }
}

void StandardCalculatorViewModel::OnPropertyChanged(String ^ propertyname)
{
    if (propertyname == IsScientificPropertyName)
    {
        if (IsScientific)
        {
            OnButtonPressed(NumbersAndOperatorsEnum::IsScientificMode);
        }
    }
    else if (propertyname == IsProgrammerPropertyName)
    {
        if (IsProgrammer)
        {
            OnButtonPressed(NumbersAndOperatorsEnum::IsProgrammerMode);
        }
    }
    else if (propertyname == IsStandardPropertyName)
    {
        if (IsStandard)
        {
            OnButtonPressed(NumbersAndOperatorsEnum::IsStandardMode);
        }
    }
    else if (propertyname == DisplayValuePropertyName)
    {
        RaisePropertyChanged(CalculationResultAutomationNamePropertyName);
        Announcement = GetDisplayUpdatedNarratorAnnouncement();
    }
    else if (propertyname == IsBitFlipCheckedPropertyName)
    {
        TraceLogger::GetInstance()->UpdateButtonUsage(
            IsBitFlipChecked ? NumbersAndOperatorsEnum::BitflipButton : NumbersAndOperatorsEnum::FullKeypadButton, ViewMode::Programmer);
    }
    else if (propertyname == IsAlwaysOnTopPropertyName)
    {
        String ^ announcement;
        if (IsAlwaysOnTop)
        {
            announcement = AppResourceProvider::GetInstance()->GetResourceString(CalcAlwaysOnTop);
        }
        else
        {
            announcement = AppResourceProvider::GetInstance()->GetResourceString(CalcBackToFullView);
        }
        Announcement = CalculatorAnnouncement::GetAlwaysOnTopChangedAnnouncement(announcement);
    }
}

void StandardCalculatorViewModel::SetCalculatorType(ViewMode targetState)
{
    // Reset error state so that commands caused by the mode change are still
    // sent if calc is currently in error state.
    IsInError = false;

    // Setting one of these properties to true will set the others to false.
    switch (targetState)
    {
    case ViewMode::Standard:
        IsStandard = true;
        ResetRadixAndUpdateMemory(true);
        SetPrecision(StandardModePrecision);
        UpdateMaxIntDigits();
        break;

    case ViewMode::Scientific:
        IsScientific = true;
        ResetRadixAndUpdateMemory(true);
        SetPrecision(ScientificModePrecision);
        break;

    case ViewMode::Programmer:
        IsProgrammer = true;
        ResetRadixAndUpdateMemory(false);
        SetPrecision(ProgrammerModePrecision);
        break;
    }
}

String ^ StandardCalculatorViewModel::GetRawDisplayValue()
{
    if (IsInError)
    {
        return DisplayValue;
    }
    else
    {
        return LocalizationSettings::GetInstance()->RemoveGroupSeparators(DisplayValue);
    }
}

// Given a format string, returns a string with the input display value inserted.
//     'format' is a localized string containing a %1 formatting mark where the display value should be inserted.
//     'displayValue' is a localized string containing a numerical value to be displayed to the user.
String ^ StandardCalculatorViewModel::GetLocalizedStringFormat(String ^ format, String ^ displayValue)
{
    return LocalizationStringUtil::GetLocalizedString(format, displayValue);
}

void StandardCalculatorViewModel::ResetRadixAndUpdateMemory(bool resetRadix)
{
    if (resetRadix)
    {
        AreHEXButtonsEnabled = false;
        CurrentRadixType = NumberBase::DecBase;
        m_standardCalculatorManager.SetRadix(RadixType::Decimal);
    }
    else
    {
        m_standardCalculatorManager.SetMemorizedNumbersString();
    }
}

void StandardCalculatorViewModel::SetPrecision(int32_t precision)
{
    m_standardCalculatorManager.SetPrecision(precision);
}

void StandardCalculatorViewModel::SwitchProgrammerModeBase(NumberBase numberBase)
{
    if (IsInError)
    {
        m_standardCalculatorManager.SendCommand(Command::CommandCLEAR);
    }

    AreHEXButtonsEnabled = numberBase == NumberBase::HexBase;
    CurrentRadixType = numberBase;
    m_standardCalculatorManager.SetRadix(GetRadixTypeFromNumberBase(numberBase));
}

void StandardCalculatorViewModel::SetMemorizedNumbersString()
{
    m_standardCalculatorManager.SetMemorizedNumbersString();
}

AngleType GetAngleTypeFromCommand(Command command)
{
    switch (command)
    {
    case Command::CommandDEG:
        return AngleType::Degrees;
    case Command::CommandRAD:
        return AngleType::Radians;
    case Command::CommandGRAD:
        return AngleType::Gradians;
    default:
        throw ref new Exception(E_FAIL, L"Invalid command type");
    }
}

void StandardCalculatorViewModel::SaveEditedCommand(_In_ unsigned int tokenPosition, _In_ Command command)
{
    bool handleOperand = false;
    wstring updatedToken;

    const pair<wstring, int>& token = m_tokens->at(tokenPosition);
    const shared_ptr<IExpressionCommand>& tokenCommand = m_commands->at(token.second);

    if (IsUnaryOp(command) && command != Command::CommandSIGN)
    {
        int angleCmd = static_cast<int>(m_standardCalculatorManager.GetCurrentDegreeMode());
        AngleType angleType = GetAngleTypeFromCommand(static_cast<Command>(angleCmd));

        if (IsTrigOp(command))
        {
            shared_ptr<IUnaryCommand> spUnaryCommand = dynamic_pointer_cast<IUnaryCommand>(tokenCommand);
            spUnaryCommand->SetCommands(angleCmd, static_cast<int>(command));
        }
        else
        {
            shared_ptr<IUnaryCommand> spUnaryCommand = dynamic_pointer_cast<IUnaryCommand>(tokenCommand);
            spUnaryCommand->SetCommand(static_cast<int>(command));
        }

        switch (command)
        {
        case Command::CommandASIN:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandSIN), true, angleType);
            break;
        case Command::CommandACOS:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandCOS), true, angleType);
            break;
        case Command::CommandATAN:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandTAN), true, angleType);
            break;
        case Command::CommandASINH:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandSINH), true, angleType);
            break;
        case Command::CommandACOSH:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandCOSH), true, angleType);
            break;
        case Command::CommandATANH:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandTANH), true, angleType);
            break;
        case Command::CommandPOWE:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandLN), true, angleType);
            break;
        default:
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(command), false, angleType);
        }
        if ((token.first.length() > 0) && (token.first[token.first.length() - 1] == L'('))
        {
            updatedToken += L'(';
        }
    }
    else if (IsBinOp(command))
    {
        shared_ptr<IBinaryCommand> spBinaryCommand = dynamic_pointer_cast<IBinaryCommand>(tokenCommand);
        spBinaryCommand->SetCommand(static_cast<int>(command));
        updatedToken = CCalcEngine::OpCodeToString(static_cast<int>(command));
    }
    else if (IsOpnd(command) || command == Command::CommandBACK)
    {
        HandleUpdatedOperandData(command);
        handleOperand = true;
    }
    else if (command == Command::CommandSIGN)
    {
        if (tokenCommand->GetCommandType() == CommandType::UnaryCommand)
        {
            shared_ptr<IExpressionCommand> spSignCommand = make_shared<CUnaryCommand>(static_cast<int>(command));
            m_commands->insert(m_commands->begin() + token.second + 1, spSignCommand);
        }
        else
        {
            shared_ptr<IOpndCommand> spOpndCommand = dynamic_pointer_cast<IOpndCommand>(tokenCommand);
            spOpndCommand->ToggleSign();
            updatedToken = spOpndCommand->GetToken(m_standardCalculatorManager.DecimalSeparator());
        }
        IsOperandUpdatedUsingViewModel = true;
    }

    if (!handleOperand)
    {
        (*m_commands)[token.second] = tokenCommand;
        (*m_tokens)[tokenPosition].first = updatedToken;

        DisplayExpressionToken ^ displayExpressionToken = ExpressionTokens->GetAt(tokenPosition);
        displayExpressionToken->Token = ref new Platform::String(updatedToken.c_str());

        // Special casing
        if (command == Command::CommandSIGN && tokenCommand->GetCommandType() == CommandType::UnaryCommand)
        {
            IsEditingEnabled = false;
            Recalculate();
        }
    }
}

void StandardCalculatorViewModel::Recalculate(bool fromHistory)
{
    // Recalculate
    Command currentDegreeMode = m_standardCalculatorManager.GetCurrentDegreeMode();
    shared_ptr<vector<shared_ptr<IExpressionCommand>>> savedCommands = std::make_shared<std::vector<shared_ptr<IExpressionCommand>>>(*m_commands);
    vector<int> currentCommands = GetCommandsFromExpressionCommands(*m_commands);

    shared_ptr<vector<pair<wstring, int>>> savedTokens = make_shared<vector<pair<wstring, int>>>();

    for (const auto& currentToken : *m_tokens)
    {
        savedTokens->push_back(currentToken);
    }

    m_standardCalculatorManager.Reset(false);
    if (IsScientific)
    {
        m_standardCalculatorManager.SendCommand(Command::ModeScientific);
    }

    if (IsFToEChecked)
    {
        m_standardCalculatorManager.SendCommand(Command::CommandFE);
    }

    m_standardCalculatorManager.SendCommand(currentDegreeMode);

    for (int command : currentCommands)
    {
        m_standardCalculatorManager.SendCommand(static_cast<CalculationManager::Command>(command));
    }

    if (fromHistory) // This is for the cases where the expression is loaded from history
    {
        // To maintain F-E state of the engine, as the last operand hasn't reached engine by now
        m_standardCalculatorManager.SendCommand(Command::CommandFE);
        m_standardCalculatorManager.SendCommand(Command::CommandFE);
    }

    // After recalculation. If there is an error then
    // IsInError should be set synchronously.
    if (IsInError)
    {
        SetExpressionDisplay(savedTokens, savedCommands);
    }
}

bool StandardCalculatorViewModel::IsOpnd(Command command)
{
    static constexpr Command opnd[] = { Command::Command0, Command::Command1, Command::Command2, Command::Command3, Command::Command4,  Command::Command5,
                                        Command::Command6, Command::Command7, Command::Command8, Command::Command9, Command::CommandPNT };

    return find(begin(opnd), end(opnd), command) != end(opnd);
}

bool StandardCalculatorViewModel::IsUnaryOp(Command command)
{
    static constexpr Command unaryOp[] = { Command::CommandSQRT,  Command::CommandFAC,  Command::CommandSQR,   Command::CommandLOG,
                                           Command::CommandPOW10, Command::CommandPOWE, Command::CommandLN,    Command::CommandREC,
                                           Command::CommandSIGN,  Command::CommandSINH, Command::CommandASINH, Command::CommandCOSH,
                                           Command::CommandACOSH, Command::CommandTANH, Command::CommandATANH, Command::CommandCUB };

    if (find(begin(unaryOp), end(unaryOp), command) != end(unaryOp))
    {
        return true;
    }

    if (IsTrigOp(command))
    {
        return true;
    }

    return false;
}

bool StandardCalculatorViewModel::IsTrigOp(Command command)
{
    static constexpr Command trigOp[] = { Command::CommandSIN,  Command::CommandCOS,  Command::CommandTAN,
                                          Command::CommandASIN, Command::CommandACOS, Command::CommandATAN };

    return find(begin(trigOp), end(trigOp), command) != end(trigOp);
}

bool StandardCalculatorViewModel::IsBinOp(Command command)
{
    static constexpr Command binOp[] = { Command::CommandADD, Command::CommandSUB,  Command::CommandMUL, Command::CommandDIV,
                                         Command::CommandEXP, Command::CommandROOT, Command::CommandMOD, Command::CommandPWR };

    return find(begin(binOp), end(binOp), command) != end(binOp);
}

bool StandardCalculatorViewModel::IsRecoverableCommand(Command command)
{
    if (IsOpnd(command))
    {
        return true;
    }

    // Programmer mode, bit flipping
    if (Command::CommandBINEDITSTART <= command && command <= Command::CommandBINEDITEND)
    {
        return true;
    }

    static constexpr Command recoverableCommands[] = { Command::CommandA, Command::CommandB, Command::CommandC,
                                                       Command::CommandD, Command::CommandE, Command::CommandF };

    return find(begin(recoverableCommands), end(recoverableCommands), command) != end(recoverableCommands);
}

size_t StandardCalculatorViewModel::LengthWithoutPadding(wstring str)
{
    return str.length() - count(str.begin(), str.end(), L' ');
}

wstring StandardCalculatorViewModel::AddPadding(wstring binaryString)
{
    if (LocalizationSettings::GetInstance()->GetEnglishValueFromLocalizedDigits(StringReference(binaryString.c_str())) == L"0")
    {
        return binaryString;
    }
    size_t pad = 4 - LengthWithoutPadding(binaryString) % 4;
    if (pad == 4)
    {
        pad = 0;
    }
    return wstring(pad, L'0') + binaryString;
}

void StandardCalculatorViewModel::UpdateProgrammerPanelDisplay()
{
    constexpr int32_t precision = 64;
    wstring hexDisplayString;
    wstring decimalDisplayString;
    wstring octalDisplayString;
    wstring binaryDisplayString;
    if (!IsInError)
    {
        // we want the precision to be set to maximum value so that the autoconversions result as desired
        if ((hexDisplayString = m_standardCalculatorManager.GetResultForRadix(16, precision, true)) == L"")
        {
            hexDisplayString = DisplayValue->Data();
            decimalDisplayString = DisplayValue->Data();
            octalDisplayString = DisplayValue->Data();
            binaryDisplayString = DisplayValue->Data();
        }
        else
        {
            decimalDisplayString = m_standardCalculatorManager.GetResultForRadix(10, precision, true);
            octalDisplayString = m_standardCalculatorManager.GetResultForRadix(8, precision, true);
            binaryDisplayString = m_standardCalculatorManager.GetResultForRadix(2, precision, true);
        }
    }
    LocalizationSettings^ localizer = LocalizationSettings::GetInstance();
    binaryDisplayString = AddPadding(binaryDisplayString);

    localizer->LocalizeDisplayValue(&hexDisplayString);
    localizer->LocalizeDisplayValue(&decimalDisplayString);
    localizer->LocalizeDisplayValue(&octalDisplayString);
    localizer->LocalizeDisplayValue(&binaryDisplayString);

    HexDisplayValue = ref new Platform::String(hexDisplayString.c_str());
    DecimalDisplayValue = ref new Platform::String(decimalDisplayString.c_str());
    OctalDisplayValue = ref new Platform::String(octalDisplayString.c_str());
    BinaryDisplayValue = ref new Platform::String(binaryDisplayString.c_str());
    HexDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedHexaDecimalAutomationFormat, GetNarratorStringReadRawNumbers(HexDisplayValue));
    DecDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedDecimalAutomationFormat, DecimalDisplayValue);
    OctDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedOctalAutomationFormat, GetNarratorStringReadRawNumbers(OctalDisplayValue));
    BinDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedBinaryAutomationFormat, GetNarratorStringReadRawNumbers(BinaryDisplayValue));

    auto binaryValueArray = ref new Vector<bool>(64, false);
    auto binaryValue = m_standardCalculatorManager.GetResultForRadix(2, precision, false);
    int i = 0;

    // To get bit 0, grab from opposite end of string.
    for (std::wstring::reverse_iterator it = binaryValue.rbegin(); it != binaryValue.rend(); ++it)
    {
        binaryValueArray->SetAt(i++, *it == L'1');
    }
    BinaryDigits = binaryValueArray;
}

void StandardCalculatorViewModel::SwitchAngleType(NumbersAndOperatorsEnum num)
{
    OnButtonPressed(num);
}

void StandardCalculatorViewModel::UpdateOperand(int pos, String ^ text)
{
    pair<wstring, int> p = m_tokens->at(pos);

    String ^ englishString = LocalizationSettings::GetInstance()->GetEnglishValueFromLocalizedDigits(text);
    p.first = englishString->Data();

    int commandPos = p.second;
    const shared_ptr<IExpressionCommand>& exprCmd = m_commands->at(commandPos);
    auto operandCommand = std::dynamic_pointer_cast<IOpndCommand>(exprCmd);

    if (operandCommand != nullptr)
    {
        shared_ptr<vector<int>> commands = make_shared<vector<int>>();
        size_t length = p.first.length();
        if (length > 0)
        {
            int num = 0;
            for (unsigned int i = 0; i < length; ++i)
            {
                if (p.first[i] == L'.')
                {
                    num = static_cast<int>(Command::CommandPNT);
                }
                else if (p.first[i] == L'e')
                {
                    num = static_cast<int>(Command::CommandEXP);
                }
                else if (p.first[i] == L'-')
                {
                    num = static_cast<int>(Command::CommandSIGN);

                    if (i == 0)
                    {
                        shared_ptr<IOpndCommand> spOpndCommand = dynamic_pointer_cast<IOpndCommand>(exprCmd);
                        if (!spOpndCommand->IsNegative())
                        {
                            spOpndCommand->ToggleSign();
                        }
                        continue;
                    }
                }
                else
                {
                    num = static_cast<int>(p.first[i]) - ASCII_0;
                    num += IDC_0;
                    if (num == static_cast<int>(Command::CommandMPLUS))
                    {
                        continue;
                    }
                }
                commands->push_back(num);
            }
        }
        else
        {
            commands->push_back(0);
        }
        operandCommand->SetCommands(commands);
    }
}

void StandardCalculatorViewModel::OnMaxDigitsReached()
{
    if (m_localizedMaxDigitsReachedAutomationFormat == nullptr)
    {
        m_localizedMaxDigitsReachedAutomationFormat = AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::MaxDigitsReachedFormat);
    }
    String ^ announcement = LocalizationStringUtil::GetLocalizedString(m_localizedMaxDigitsReachedAutomationFormat, m_CalculationResultAutomationName);
    Announcement = CalculatorAnnouncement::GetMaxDigitsReachedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnBinaryOperatorReceived()
{
    Announcement = GetDisplayUpdatedNarratorAnnouncement();
}

NarratorAnnouncement ^ StandardCalculatorViewModel::GetDisplayUpdatedNarratorAnnouncement()
{
    String ^ announcement;
    if (m_feedbackForButtonPress == nullptr || m_feedbackForButtonPress->IsEmpty())
    {
        announcement = m_CalculationResultAutomationName;
    }
    else
    {
        if (m_localizedButtonPressFeedbackAutomationFormat == nullptr)
        {
            m_localizedButtonPressFeedbackAutomationFormat =
                AppResourceProvider::GetInstance()->GetResourceString(CalculatorResourceKeys::ButtonPressFeedbackFormat);
        }
        announcement = LocalizationStringUtil::GetLocalizedString(
            m_localizedButtonPressFeedbackAutomationFormat, m_CalculationResultAutomationName, m_feedbackForButtonPress);
    }

    // Make sure we don't accidentally repeat an announcement.
    m_feedbackForButtonPress = nullptr;

    return CalculatorAnnouncement::GetDisplayUpdatedAnnouncement(announcement);
}

ViewMode StandardCalculatorViewModel::GetCalculatorMode()
{
    if (IsStandard)
    {
        return ViewMode::Standard;
    }
    else if (IsScientific)
    {
        return ViewMode::Scientific;
    }
    return ViewMode::Programmer;
}

void StandardCalculatorViewModel::ValueBitLength::set(CalculatorApp::ViewModel::Common::BitLength value)
{
    if (m_valueBitLength != value)
    {
        m_valueBitLength = value;
        RaisePropertyChanged(L"ValueBitLength");

        switch (value)
        {
        case BitLength::BitLengthQWord:
            ButtonPressed->Execute(NumbersAndOperatorsEnum::Qword);
            break;
        case BitLength::BitLengthDWord:
            ButtonPressed->Execute(NumbersAndOperatorsEnum::Dword);
            break;
        case BitLength::BitLengthWord:
            ButtonPressed->Execute(NumbersAndOperatorsEnum::Word);
            break;
        case BitLength::BitLengthByte:
            ButtonPressed->Execute(NumbersAndOperatorsEnum::Byte);
            break;
        }

        // update memory list according to bit length
        SetMemorizedNumbersString();
    }
}

void StandardCalculatorViewModel::SelectHistoryItem(HistoryItemViewModel ^ item)
{
    SetHistoryExpressionDisplay(item->GetTokens(), item->GetCommands());
    SetExpressionDisplay(item->GetTokens(), item->GetCommands());
    SetPrimaryDisplay(item->Result, false);
    IsFToEEnabled = false;
}

void StandardCalculatorViewModel::ResetCalcManager(bool clearMemory)
{
    m_standardCalculatorManager.Reset(clearMemory);
}

void StandardCalculatorViewModel::SendCommandToCalcManager(int commandId)
{
    m_standardCalculatorManager.SendCommand(static_cast<Command>(commandId));
}

void StandardCalculatorViewModel::SetBitshiftRadioButtonCheckedAnnouncement(Platform::String ^ announcement)
{
    Announcement = CalculatorAnnouncement::GetBitShiftRadioButtonCheckedAnnouncement(announcement);
}

StandardCalculatorSnapshot StandardCalculatorViewModel::GetStandardCalculatorSnapshot() const
{
    StandardCalculatorSnapshot snapshot;
    auto& historyItems = m_standardCalculatorManager.GetHistoryItems();
    if (!historyItems.empty())
    {
        snapshot.CalcManager.HistoryItems = std::move(historyItems);
    }
    snapshot.PrimaryDisplay = PrimaryDisplaySnapshot{ m_DisplayValue, m_IsInError };
    if (!m_tokens->empty() && !m_commands->empty())
    {
        snapshot.ExpressionDisplay = { *m_tokens, *m_commands };
    }
    snapshot.DisplayCommands = m_standardCalculatorManager.GetDisplayCommandsSnapshot();
    return snapshot;
}

void StandardCalculatorViewModel::SetStandardCalculatorSnapshot(const StandardCalculatorSnapshot& snapshot)
{
    if (snapshot.CalcManager.HistoryItems.has_value())
    {
        m_standardCalculatorManager.SetHistoryItems(snapshot.CalcManager.HistoryItems.value());
    }

    std::vector<int> commands;
    if (snapshot.ExpressionDisplay.has_value() && snapshot.ExpressionDisplay->Tokens.back().first == L"=")
    {
        commands = GetCommandsFromExpressionCommands(snapshot.ExpressionDisplay->Commands);
    }
    if (commands.empty() && !snapshot.DisplayCommands.empty())
    {
        commands = GetCommandsFromExpressionCommands(snapshot.DisplayCommands);
    }
    for (const auto& command : commands)
    {
        m_standardCalculatorManager.SendCommand(static_cast<Command>(command));
    }

    if (snapshot.ExpressionDisplay.has_value())
    {
        SetExpressionDisplay(
            std::make_shared<std::vector<std::pair<std::wstring, int>>>(snapshot.ExpressionDisplay->Tokens),
            std::make_shared<std::vector<std::shared_ptr<IExpressionCommand>>>(snapshot.ExpressionDisplay->Commands));
    }
    SetPrimaryDisplay(snapshot.PrimaryDisplay.DisplayValue, snapshot.PrimaryDisplay.IsError);
}
