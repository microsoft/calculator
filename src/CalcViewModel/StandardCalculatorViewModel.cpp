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
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Common::Automation;
using namespace CalculatorApp::ViewModel;
using namespace CalculationManager;
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
using namespace Utils;

constexpr int StandardModePrecision = 16;
constexpr int ScientificModePrecision = 32;
constexpr int ProgrammerModePrecision = 64;

namespace CalculatorApp::ViewModel
{
    namespace CalculatorViewModelProperties
    {
        StringReference IsMemoryEmpty(L"IsMemoryEmpty");
        StringReference IsScientific(L"IsScientific");
        StringReference IsStandard(L"IsStandard");
        StringReference IsProgrammer(L"IsProgrammer");
        StringReference DisplayValue(L"DisplayValue");
        StringReference IsInError(L"IsInError");
        StringReference BinaryDisplayValue(L"BinaryDisplayValue");
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
        StringReference LeftParenthesisAutomationFormat(L"Format_OpenParenthesisAutomationNamePrefix");
        StringReference MaxDigitsReachedFormat(L"Format_MaxDigitsReached");
        StringReference ButtonPressFeedbackFormat(L"Format_ButtonPressAuditoryFeedback");
        StringReference MemorySave(L"Format_MemorySave");
        StringReference MemoryItemChanged(L"Format_MemorySlotChanged");
        StringReference MemoryItemCleared(L"Format_MemorySlotCleared");
        StringReference MemoryCleared(L"Memory_Cleared");
        StringReference DisplayCopied(L"Display_Copied");
    }
}

StandardCalculatorViewModel::StandardCalculatorViewModel() :
    m_DisplayValue(L"0"),
    m_DecimalDisplayValue(L"0"),
    m_HexDisplayValue(L"0"),
    m_BinaryDisplayValue(L"0"),
    m_OctalDisplayValue(L"0"),
    m_standardCalculatorManager(&m_calculatorDisplay, &m_resourceProvider),
    m_MemorizedNumbers(ref new Vector<MemoryItemViewModel^>()),
    m_IsMemoryEmpty(true),
    m_IsFToEChecked(false),
    m_isShiftChecked(false),
    m_IsShiftProgrammerChecked(false),
    m_IsQwordEnabled(true),
    m_IsDwordEnabled(true),
    m_IsWordEnabled(true),
    m_IsByteEnabled(true),
    m_isBitFlipChecked(false),
    m_isBinaryBitFlippingEnabled(false),
    m_CurrentRadixType(RADIX_TYPE::DEC_RADIX),
    m_CurrentAngleType(NumbersAndOperatorsEnum::Degree),
    m_OpenParenthesisCount(L""),
    m_Announcement(nullptr),
    m_feedbackForButtonPress(nullptr),
    m_isRtlLanguage(false),
    m_localizedMaxDigitsReachedAutomationFormat(nullptr),
    m_localizedButtonPressFeedbackAutomationFormat(nullptr),
    m_localizedMemorySavedAutomationFormat(nullptr),
    m_localizedMemoryItemChangedAutomationFormat(nullptr),
    m_localizedMemoryItemClearedAutomationFormat(nullptr),
    m_localizedMemoryCleared(nullptr)
{
    WeakReference calculatorViewModel(this);
    m_calculatorDisplay.SetCallback(calculatorViewModel);
    m_expressionAutomationNameFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::CalculatorExpression);
    m_localizedCalculationResultAutomationFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::CalculatorResults);
    m_localizedCalculationResultDecimalAutomationFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::CalculatorResults_DecimalSeparator_Announced);
    m_localizedHexaDecimalAutomationFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::HexButton);
    m_localizedDecimalAutomationFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::DecButton);
    m_localizedOctalAutomationFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::OctButton);
    m_localizedBinaryAutomationFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::BinButton);
    m_leftParenthesisAutomationFormat = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::LeftParenthesisAutomationFormat);

    // Initialize the Automation Name
    CalculationResultAutomationName = GetLocalizedStringFormat(m_localizedCalculationResultAutomationFormat, m_DisplayValue);
    CalculationExpressionAutomationName = GetLocalizedStringFormat(m_expressionAutomationNameFormat, L"");

    // Initialize history view model
    m_HistoryVM = ref new HistoryViewModel(&m_standardCalculatorManager);
    m_HistoryVM->SetCalculatorDisplay(m_calculatorDisplay);

    m_decimalSeparator = LocalizationSettings::GetInstance().GetDecimalSeparator();

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
    AreHistoryShortcutsEnabled = true;
    AreProgrammerRadixOperatorsEnabled = false;

    m_tokenPosition = -1;
    m_isLastOperationHistoryLoad = false;
}

String^ StandardCalculatorViewModel::LocalizeDisplayValue(_In_ wstring const &displayValue, _In_ bool isError)
{
    wstring result(displayValue);

    LocalizationSettings::GetInstance().LocalizeDisplayValue(&result);

    // WINBLUE: 440747 - In BiDi languages, error messages need to be wrapped in LRE/PDF
    if (isError && m_isRtlLanguage)
    {
        result.insert(result.begin(), Utils::LRE);
        result.push_back(Utils::PDF);
    }

    return ref new Platform::String(result.c_str());
}

String^ StandardCalculatorViewModel::CalculateNarratorDisplayValue(_In_ wstring const &displayValue, _In_ String^ localizedDisplayValue, _In_ bool isError)
{
    String^ localizedValue = localizedDisplayValue;
    String^ automationFormat = m_localizedCalculationResultAutomationFormat;

    // The narrator doesn't read the decimalSeparator if it's the last character
    if (Utils::IsLastCharacterTarget(displayValue, m_decimalSeparator))
    {
        // remove the decimal separator, to avoid a long pause between words
        localizedValue = LocalizeDisplayValue(displayValue.substr(0, displayValue.length() - 1), isError);

        // Use a format which has a word in the decimal separator's place
        // "The Display is 10 point"
        automationFormat = m_localizedCalculationResultDecimalAutomationFormat;
    }

    // In Programmer modes using non-base10, we want the strings to be read as literal digits.
    if (IsProgrammer && CurrentRadixType != RADIX_TYPE::DEC_RADIX)
    {
        localizedValue = GetNarratorStringReadRawNumbers(localizedValue);
    }

    return GetLocalizedStringFormat(automationFormat, localizedValue);
}

String^ StandardCalculatorViewModel::GetNarratorStringReadRawNumbers(_In_ String^ localizedDisplayValue)
{
    wstringstream wss;
    auto& locSettings = LocalizationSettings::GetInstance();

    // Insert a space after each digit in the string, to force Narrator to read them as separate numbers.
    wstring wstrValue(localizedDisplayValue->Data());
    for (wchar_t& c : wstrValue)
    {
        wss << c;
        if (locSettings.IsLocalizedHexDigit(c))
        {
            wss << L' ';
        }
    }

    return ref new String(wss.str().c_str());
}

void StandardCalculatorViewModel::SetPrimaryDisplay(_In_ wstring const &displayStringValue, _In_ bool isError)
{
    String^ localizedDisplayStringValue = LocalizeDisplayValue(displayStringValue, isError);

    // Set this variable before the DisplayValue is modified, Otherwise the DisplayValue will
    // not match what the narrator is saying
    m_CalculationResultAutomationName = CalculateNarratorDisplayValue(displayStringValue, localizedDisplayStringValue, isError);

    DisplayValue = localizedDisplayStringValue;

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

void StandardCalculatorViewModel::SetParenthesisCount(_In_ const wstring& parenthesisCount)
{
    if (IsProgrammer || IsScientific)
    {
        OpenParenthesisCount = ref new String(parenthesisCount.c_str());
        RaisePropertyChanged("LeftParenthesisAutomationName");
    }
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

String ^ StandardCalculatorViewModel::GetLeftParenthesisAutomationName()
{
    String^ parenthesisCount = ((m_OpenParenthesisCount == nullptr) ? "0" : m_OpenParenthesisCount);
    wstring localizedParenthesisCount = std::wstring(parenthesisCount->Data());
    LocalizationSettings::GetInstance().LocalizeDisplayValue(&localizedParenthesisCount);

    return GetLocalizedStringFormat(m_leftParenthesisAutomationFormat, ref new String(localizedParenthesisCount.c_str()));
}

void StandardCalculatorViewModel::SetExpressionDisplay(_Inout_ shared_ptr<CalculatorVector<pair<wstring, int>>> const &tokens, _Inout_ shared_ptr<CalculatorVector <shared_ptr<IExpressionCommand>>> const &commands)
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

void StandardCalculatorViewModel::SetHistoryExpressionDisplay(_Inout_ shared_ptr<CalculatorVector<pair<wstring, int>>> const &tokens, _Inout_ shared_ptr<CalculatorVector <shared_ptr<IExpressionCommand>>> const &commands)
{
    m_tokens = make_shared<CalculatorVector<pair<wstring, int>>>(*tokens);
    m_commands = make_shared<CalculatorVector <shared_ptr<IExpressionCommand>>>(*commands);
    IsEditingEnabled = false;

    // Setting the History Item Load Mode so that UI does not get updated with recalculation of every token
    m_standardCalculatorManager.SetInHistoryItemLoadMode(true);
    Recalculate(true);
    m_standardCalculatorManager.SetInHistoryItemLoadMode(false);
    m_isLastOperationHistoryLoad = true;
}

void StandardCalculatorViewModel::SetTokens(_Inout_ shared_ptr<CalculatorVector<pair<wstring, int>>> const &tokens)
{
    AreTokensUpdated = false;

    if (m_ExpressionTokens == nullptr)
    {
        m_ExpressionTokens = ref new Vector<DisplayExpressionToken^>();
    }
    else
    {
        m_ExpressionTokens->Clear();
    }

    unsigned int nTokens = 0;
    tokens->GetSize(&nTokens);
    pair <wstring, int> currentToken;
    const auto& localizer = LocalizationSettings::GetInstance();

    for (unsigned int i = 0; i < nTokens; ++i)
    {
        if (SUCCEEDED(tokens->GetAt(i, &currentToken)))
        {
            Common::TokenType type;
            const wstring separator = L" ";
            bool isEditable = (currentToken.second == -1) ? false : true;
            localizer.LocalizeDisplayValue(&(currentToken.first));

            if (!isEditable)
            {
                if (currentToken.first == separator)
                {
                    type = TokenType::Separator;
                }
                else
                {
                    type = TokenType::Operator;
                }
            }

            else
            {
                shared_ptr<IExpressionCommand> command;
                IFTPlatformException(m_commands->GetAt(static_cast<unsigned int>(currentToken.second), &command));

                if (command->GetCommandType() == CommandType::OperandCommand)
                {
                    type = TokenType::Operand;
                }
                else
                {
                    type = TokenType::Operator;
                }
            }
            DisplayExpressionToken^ expressionToken = ref new DisplayExpressionToken(ref new String(currentToken.first.c_str()), i, isEditable, type);
            m_ExpressionTokens->Append(expressionToken);
        }
    }
}

String^ StandardCalculatorViewModel::GetCalculatorExpressionAutomationName()
{
    String^ expression = L"";
    for (auto&& token : m_ExpressionTokens)
    {
        expression += LocalizationService::GetNarratorReadableToken(token->Token);
    }

    return GetLocalizedStringFormat(m_expressionAutomationNameFormat, expression);
}

void StandardCalculatorViewModel::SetMemorizedNumbers(const vector<wstring>& newMemorizedNumbers)
{
    const auto& localizer = LocalizationSettings::GetInstance();
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

            MemoryItemViewModel^ memorySlot = ref new MemoryItemViewModel(this);
            memorySlot->Position = 0;
            localizer.LocalizeDisplayValue(&stringValue);
            memorySlot->Value = Utils::LRO + ref new String(stringValue.c_str()) + Utils::PDF;

            MemorizedNumbers->InsertAt(0, memorySlot);
            IsMemoryEmpty = false;

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
            localizer.LocalizeDisplayValue(&newStringValue);

            // If the value is different, update the value
            if (MemorizedNumbers->GetAt(i)->Value != StringReference(newStringValue.c_str()))
            {
                MemorizedNumbers->GetAt(i)->Value = Utils::LRO + ref new String(newStringValue.c_str()) + Utils::PDF;
            }

        }
    }
}

void StandardCalculatorViewModel::FtoEButtonToggled()
{
    OnButtonPressed(NumbersAndOperatorsEnum::FToE);
}

void StandardCalculatorViewModel::HandleUpdatedOperandData(Command cmdenum)
{
    DisplayExpressionToken^ displayExpressionToken = ExpressionTokens->GetAt(m_tokenPosition);
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
        //Clear older text;
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
                delete [] temp;
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

    String^ updatedData = ref new String(temp);
    UpdateOperand(m_tokenPosition, updatedData);
    displayExpressionToken->Token = updatedData;
    IsOperandUpdatedUsingViewModel = true;
    displayExpressionToken->CommandIndex = commandIndex;
}

bool StandardCalculatorViewModel::IsOperator(Command cmdenum)
{
    if ((cmdenum == Command::Command0) || (cmdenum == Command::Command1) || (cmdenum == Command::Command2) || (cmdenum == Command::Command3) || (cmdenum == Command::Command4) || (cmdenum == Command::Command5)
        || (cmdenum == Command::Command6) || (cmdenum == Command::Command7) || (cmdenum == Command::Command8) || (cmdenum == Command::Command9) || (cmdenum == Command::CommandPNT) || (cmdenum == Command::CommandBACK)
        || (cmdenum == Command::CommandEXP) || (cmdenum == Command::CommandFE) || (cmdenum == Command::ModeBasic) || (cmdenum == Command::ModeProgrammer) || (cmdenum == Command::ModeScientific)
        || (cmdenum == Command::CommandINV) || (cmdenum == Command::CommandCENTR) || (cmdenum == Command::CommandDEG) || (cmdenum == Command::CommandRAD) || (cmdenum == Command::CommandGRAD)
        || ((cmdenum >= Command::CommandBINEDITSTART) && (cmdenum <= Command::CommandBINEDITEND)))
    {
        return false;
    }
    return true;
}

void StandardCalculatorViewModel::OnButtonPressed(Object^ parameter)
{
    m_feedbackForButtonPress = CalculatorButtonPressedEventArgs::GetAuditoryFeedbackFromCommandParameter(parameter);
    NumbersAndOperatorsEnum numOpEnum = CalculatorButtonPressedEventArgs::GetOperationFromCommandParameter(parameter);
    Command cmdenum = ConvertToOperatorsEnum(numOpEnum);

    TraceLogger::GetInstance().UpdateFunctionUsage((int)numOpEnum);

    if (IsInError)
    {
        m_standardCalculatorManager.SendCommand(Command::CommandCLEAR);
        
        if (!IsRecoverableCommand((int)numOpEnum))
        {
            return;
        }
    }

    if (IsEditingEnabled &&
        numOpEnum != NumbersAndOperatorsEnum::IsScientificMode &&
        numOpEnum != NumbersAndOperatorsEnum::IsStandardMode &&
        numOpEnum != NumbersAndOperatorsEnum::IsProgrammerMode &&
        numOpEnum != NumbersAndOperatorsEnum::FToE &&
        (numOpEnum != NumbersAndOperatorsEnum::Degree) && (numOpEnum != NumbersAndOperatorsEnum::Radians) && (numOpEnum != NumbersAndOperatorsEnum::Grads))
    {
        if (!m_keyPressed)
        {
            SaveEditedCommand(m_selectedExpressionToken->TokenPosition, cmdenum);
        }
    }
    else
    {
        if (numOpEnum == NumbersAndOperatorsEnum::IsStandardMode ||
            numOpEnum == NumbersAndOperatorsEnum::IsScientificMode ||
            numOpEnum == NumbersAndOperatorsEnum::IsProgrammerMode)
        {
            IsEditingEnabled = false;
        }
        if (numOpEnum == NumbersAndOperatorsEnum::Memory)
        {
            OnMemoryButtonPressed();
        }
        else
        {
            if (numOpEnum == NumbersAndOperatorsEnum::Clear ||
                numOpEnum == NumbersAndOperatorsEnum::ClearEntry ||
                numOpEnum == NumbersAndOperatorsEnum::IsStandardMode ||
                numOpEnum == NumbersAndOperatorsEnum::IsProgrammerMode)
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
            if ((cmdenum == Command::Command0) || (cmdenum == Command::Command1) || (cmdenum == Command::Command2) || (cmdenum == Command::Command3) || (cmdenum == Command::Command4) || (cmdenum == Command::Command5)
                || (cmdenum == Command::Command6) || (cmdenum == Command::Command7) || (cmdenum == Command::Command8) || (cmdenum == Command::Command9) || (cmdenum == Command::CommandPNT) || (cmdenum == Command::CommandBACK) || (cmdenum == Command::CommandEXP))
            {
                IsOperatorCommand = false;
            }
            else
            {
                IsOperatorCommand = true;
            }

            if (m_isLastOperationHistoryLoad &&
                ((numOpEnum != NumbersAndOperatorsEnum::Degree) && (numOpEnum != NumbersAndOperatorsEnum::Radians) && (numOpEnum != NumbersAndOperatorsEnum::Grads)))
            {
                IsFToEEnabled = true;
                m_isLastOperationHistoryLoad = false;
            }

            m_standardCalculatorManager.SendCommand(cmdenum);
        }
    }
}

int StandardCalculatorViewModel::GetBitLengthType()
{
    if (IsQwordEnabled)
    {
        return QwordType;
    }
    else if (IsDwordEnabled)
    {
        return DwordType;
    }
    else if (IsWordEnabled)
    {
        return WordType;
    }
    else
    {
        return ByteType;
    }
}

int StandardCalculatorViewModel::GetNumberBase()
{
    if (CurrentRadixType == HEX_RADIX)
    {
        return HexBase;
    }
    else if (CurrentRadixType == DEC_RADIX)
    {
        return DecBase;
    }
    else if (CurrentRadixType == OCT_RADIX)
    {
        return OctBase;
    }
    else
    {
        return BinBase;
    }

}

void StandardCalculatorViewModel::OnCopyCommand(Object^ parameter)
{
    CopyPasteManager::CopyToClipboard(GetRawDisplayValue());

    String^ announcement = AppResourceProvider::GetInstance().GetResourceString(CalculatorResourceKeys::DisplayCopied);
    Announcement = CalculatorAnnouncement::GetDisplayCopiedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnPasteCommand(Object^ parameter)
{
    ViewMode mode;
    int NumberBase = -1;
    int bitLengthType = -1;
    if (IsScientific)
    {
        mode = ViewMode::Scientific;
    }
    else if (IsProgrammer)
    {
        mode = ViewMode::Programmer;
        NumberBase = GetNumberBase();
        bitLengthType = GetBitLengthType();
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
    CopyPasteManager::GetStringToPaste(mode, NavCategory::GetGroupType(mode), NumberBase, bitLengthType).then(
        [this, mode](String^ pastedString)
    {
        OnPaste(pastedString, mode);
    }, concurrency::task_continuation_context::use_current());
}

CalculationManager::Command StandardCalculatorViewModel::ConvertToOperatorsEnum(NumbersAndOperatorsEnum operation)
{
    return safe_cast<Command>(operation);
}

void StandardCalculatorViewModel::OnPaste(String^ pastedString, ViewMode mode)
{
    // If pastedString is invalid("NoOp") then display pasteError else process the string
    if (pastedString == StringReference(CopyPasteManager::PasteErrorString))
    {
        this->DisplayPasteError();
        return;
    }

    TraceLogger::GetInstance().LogValidInputPasted(mode);
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
        bool canSendNegate = false;

        NumbersAndOperatorsEnum mappedNumOp = MapCharacterToButtonId(*it, canSendNegate);

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

        if (mappedNumOp != NumbersAndOperatorsEnum::None)
        {
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
        }

        // Handle exponent and exponent sign (...e+... or ...e-...)
        if (mappedNumOp == NumbersAndOperatorsEnum::Exp)
        {
            ++it;
            if (!(MapCharacterToButtonId(*it, canSendNegate) == NumbersAndOperatorsEnum::Add))
            {
                Command cmdNegate = ConvertToOperatorsEnum(NumbersAndOperatorsEnum::Negate);
                m_standardCalculatorManager.SendCommand(cmdNegate);
            }
        }

        ++it;
    }
}

void StandardCalculatorViewModel::OnClearMemoryCommand(
    Object^ parameter)
{
    m_standardCalculatorManager.MemorizedNumberClearAll();

    int windowId = Utils::GetWindowId();
    TraceLogger::GetInstance().LogMemoryClearAll(windowId);

    String^ announcement = LocalizationStringUtil::GetLocalizedNarratorAnnouncement(CalculatorResourceKeys::MemoryCleared, m_localizedMemoryCleared);
    Announcement = CalculatorAnnouncement::GetMemoryClearedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnPinUnpinCommand(Object^ parameter)
{
    SetViewPinnedState(!IsViewPinned());
}

bool StandardCalculatorViewModel::IsViewPinned()
{
    return m_IsCurrentViewPinned;
}

void StandardCalculatorViewModel::SetViewPinnedState(
    bool pinned)
{
    IsCurrentViewPinned = pinned;
}

NumbersAndOperatorsEnum StandardCalculatorViewModel::MapCharacterToButtonId(
    const wchar_t ch,
    bool& canSendNegate)
{
    NumbersAndOperatorsEnum mappedValue = NumbersAndOperatorsEnum::None;
    canSendNegate = false;

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
        mappedValue = NumbersAndOperatorsEnum::Zero + static_cast<NumbersAndOperatorsEnum>(ch - L'0');
        canSendNegate = true;
        break;

    case '*':
        mappedValue = NumbersAndOperatorsEnum::Multiply;
        break;

    case '+':
        mappedValue = NumbersAndOperatorsEnum::Add;
        break;

    case '-':
        mappedValue = NumbersAndOperatorsEnum::Subtract;
        break;

    case '/':
        mappedValue = NumbersAndOperatorsEnum::Divide;
        break;

    case '=':
        mappedValue = NumbersAndOperatorsEnum::Equals;
        break;

    case '(':
        mappedValue = NumbersAndOperatorsEnum::OpenParenthesis;
        break;

    case ')':
        mappedValue = NumbersAndOperatorsEnum::CloseParenthesis;
        break;

    case 'a':
    case 'A':
        mappedValue = NumbersAndOperatorsEnum::A;
        break;
    case 'b':
    case 'B':
        mappedValue = NumbersAndOperatorsEnum::B;
        break;
    case 'c':
    case 'C':
        mappedValue = NumbersAndOperatorsEnum::C;
        break;
    case 'd':
    case 'D':
        mappedValue = NumbersAndOperatorsEnum::D;
        break;
    case 'e':
    case 'E':
        // Only allow scientific notation in scientific mode
        if (IsProgrammer)
        {
            mappedValue = NumbersAndOperatorsEnum::E;
        }
        else
        {
            mappedValue = NumbersAndOperatorsEnum::Exp;
        }
        break;
    case 'f':
    case 'F':
        mappedValue = NumbersAndOperatorsEnum::F;
        break;
    default:
        // For the decimalSeparator, we need to respect the user setting.
        if (ch == m_decimalSeparator)
        {
            mappedValue = NumbersAndOperatorsEnum::Decimal;
        }
        break;
    }

    if (mappedValue == NumbersAndOperatorsEnum::None)
    {
        if (LocalizationSettings::GetInstance().IsLocalizedDigit(ch))
        {
            mappedValue = NumbersAndOperatorsEnum::Zero + static_cast<NumbersAndOperatorsEnum>(ch - LocalizationSettings::GetInstance().GetDigitSymbolFromEnUsDigit('0'));
            canSendNegate = true;
        }
    }

    // Negate cannot be sent for leading zeroes
    if (NumbersAndOperatorsEnum::Zero == mappedValue)
    {
        canSendNegate = false;
    }

    return mappedValue;
}

void StandardCalculatorViewModel::OnMemoryButtonPressed()
{
    m_standardCalculatorManager.MemorizeNumber();

    int windowId = Utils::GetWindowId();
    TraceLogger::GetInstance().InsertIntoMemoryMap(windowId, IsStandard, IsScientific, IsProgrammer);

    String^ announcement = LocalizationStringUtil::GetLocalizedNarratorAnnouncement(
        CalculatorResourceKeys::MemorySave,
        m_localizedMemorySavedAutomationFormat,
        m_DisplayValue->Data());

    Announcement = CalculatorAnnouncement::GetMemoryItemAddedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnMemoryItemChanged(unsigned int indexOfMemory)
{
    if (indexOfMemory < MemorizedNumbers->Size)
    {
        MemoryItemViewModel^ memSlot = MemorizedNumbers->GetAt(indexOfMemory);
        String^ localizedValue = memSlot->Value;

        wstring localizedIndex = to_wstring(indexOfMemory + 1);
        LocalizationSettings::GetInstance().LocalizeDisplayValue(&localizedIndex);

        String^ announcement = LocalizationStringUtil::GetLocalizedNarratorAnnouncement(
            CalculatorResourceKeys::MemoryItemChanged,
            m_localizedMemoryItemChangedAutomationFormat,
            localizedIndex.c_str(),
            localizedValue->Data());

        Announcement = CalculatorAnnouncement::GetMemoryItemChangedAnnouncement(announcement);
    }
}

void StandardCalculatorViewModel::OnMemoryItemPressed(Object^ memoryItemPosition)
{
    if (MemorizedNumbers && MemorizedNumbers->Size > 0)
    {
        auto boxedPosition = safe_cast<Box<int>^>(memoryItemPosition);
        m_standardCalculatorManager.MemorizedNumberLoad(boxedPosition->Value);
        HideMemoryClicked();
        int windowId = Utils::GetWindowId();
        TraceLogger::GetInstance().LogMemoryUsed(windowId, boxedPosition->Value, IsStandard, IsScientific, IsProgrammer, MemorizedNumbers->Size);
    }
}

void StandardCalculatorViewModel::OnMemoryAdd(Object^ memoryItemPosition)
{
    // M+ will add display to memorylist if memory list is empty.
    int windowId = Utils::GetWindowId();

    if (MemorizedNumbers)
    {
        auto boxedPosition = safe_cast<Box<int>^>(memoryItemPosition);
        if (MemorizedNumbers->Size > 0)
        {
            TraceLogger::GetInstance().LogMemoryUsed(windowId, boxedPosition->Value, IsStandard, IsScientific, IsProgrammer, MemorizedNumbers->Size);
            TraceLogger::GetInstance().UpdateMemoryMap(windowId, boxedPosition->Value, IsStandard, IsScientific, IsProgrammer);
        }
        else
        {
            TraceLogger::GetInstance().InsertIntoMemoryMap(windowId, IsStandard, IsScientific, IsProgrammer);
        }
        m_standardCalculatorManager.MemorizedNumberAdd(boxedPosition->Value);
    }
}

void StandardCalculatorViewModel::OnMemorySubtract(Object^ memoryItemPosition)
{
    int windowId = Utils::GetWindowId();

    // M- will add negative of displayed number to memorylist if memory list is empty.
    if (MemorizedNumbers)
    {
        auto boxedPosition = safe_cast<Box<int>^>(memoryItemPosition);
        if (MemorizedNumbers->Size > 0)
        {
            TraceLogger::GetInstance().LogMemoryUsed(windowId, boxedPosition->Value, IsStandard, IsScientific, IsProgrammer, MemorizedNumbers->Size);
            TraceLogger::GetInstance().UpdateMemoryMap(windowId, boxedPosition->Value, IsStandard, IsScientific, IsProgrammer);
        }
        else
        {
            TraceLogger::GetInstance().InsertIntoMemoryMap(windowId, IsStandard, IsScientific, IsProgrammer);
        }
        m_standardCalculatorManager.MemorizedNumberSubtract(boxedPosition->Value);
    }
}

void StandardCalculatorViewModel::OnMemoryClear(_In_ Object^ memoryItemPosition)
{
    if (MemorizedNumbers && MemorizedNumbers->Size > 0)
    {
        int windowId = Utils::GetWindowId();
        auto boxedPosition = safe_cast<Box<int>^>(memoryItemPosition);

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

            TraceLogger::GetInstance().LogMemoryUsed(windowId, boxedPosition->Value, IsStandard, IsScientific, IsProgrammer, MemorizedNumbers->Size);
            TraceLogger::GetInstance().DeleteFromMemoryMap(windowId, boxedPosition->Value);

            wstring localizedIndex = to_wstring(boxedPosition->Value + 1);
            LocalizationSettings::GetInstance().LocalizeDisplayValue(&localizedIndex);

            String^ announcement = LocalizationStringUtil::GetLocalizedNarratorAnnouncement(
                CalculatorResourceKeys::MemoryItemCleared,
                m_localizedMemoryItemClearedAutomationFormat,
                localizedIndex.c_str());

            Announcement = CalculatorAnnouncement::GetMemoryClearedAnnouncement(announcement);
        }
    }
}

Array<unsigned char>^ StandardCalculatorViewModel::Serialize()
{
    DataWriter^ writer = ref new DataWriter();
    writer->WriteUInt32(static_cast<UINT32>(m_CurrentAngleType));
    writer->WriteBoolean(IsFToEChecked);
    writer->WriteBoolean(IsCurrentViewPinned);
    writer->WriteUInt32(static_cast<UINT32>(m_standardCalculatorManager.SerializeSavedDegreeMode()));

    // Serialize Memory
    vector<long> serializedMemory;
    serializedMemory = m_standardCalculatorManager.GetSerializedMemory();
    size_t lengthOfSerializedMemory = serializedMemory.size();
    writer->WriteUInt32(static_cast<UINT32>(lengthOfSerializedMemory));
    for (auto data : serializedMemory)
    {
        writer->WriteInt32(data);
    }

    // Serialize Primary Display
    vector<long> serializedPrimaryDisplay = m_standardCalculatorManager.GetSerializedPrimaryDisplay();
    writer->WriteUInt32(static_cast<UINT32>(serializedPrimaryDisplay.size()));
    for (auto data : serializedPrimaryDisplay)
    {
        writer->WriteInt32(data);
    }

    //For ProgrammerMode
    writer->WriteUInt32(static_cast<UINT32>(CurrentRadixType));

    //Serialize commands of calculator manager
    vector<unsigned char> serializedCommand = m_standardCalculatorManager.SerializeCommands();
    writer->WriteUInt32(static_cast<UINT32>(serializedCommand.size()));
    writer->WriteBytes(ref new Array<unsigned char>(serializedCommand.data(), static_cast<unsigned int>(serializedCommand.size())));

    if (IsInError)
    {
        Utils::SerializeCommandsAndTokens(m_tokens, m_commands, writer);
    }

    //Convert viewmodel data in writer to bytes
    IBuffer^ buffer = writer->DetachBuffer();
    DataReader^ reader = DataReader::FromBuffer(buffer);
    Platform::Array<unsigned char>^ viewModelDataAsBytes = ref new Array<unsigned char>(buffer->Length);
    reader->ReadBytes(viewModelDataAsBytes);

    // Return byte array
    return viewModelDataAsBytes;
}

void StandardCalculatorViewModel::Deserialize(Array<unsigned char>^ state)
{
    // Read byte array into a buffer
    DataWriter^ writer = ref new DataWriter();
    writer->WriteBytes(state);
    IBuffer^ buffer = writer->DetachBuffer();

    // Read view model data
    if (buffer->Length != 0)
    {
        DataReader^ reader = DataReader::FromBuffer(buffer);
        m_CurrentAngleType = ConvertIntegerToNumbersAndOperatorsEnum(reader->ReadUInt32());

        IsFToEChecked = reader->ReadBoolean();
        IsCurrentViewPinned = reader->ReadBoolean();
        Command serializedDegreeMode = static_cast<Command>(reader->ReadUInt32());

        m_standardCalculatorManager.SendCommand(serializedDegreeMode);

        // Deserialize Memory
        UINT32 memoryDataLength = reader->ReadUInt32();
        vector<long> serializedMemory;
        for (unsigned int i = 0; i < memoryDataLength; i++)
        {
            serializedMemory.push_back(reader->ReadInt32());
        }
        m_standardCalculatorManager.DeSerializeMemory(serializedMemory);

        // Serialize Primary Display
        UINT32 serializedPrimaryDisplayLength = reader->ReadUInt32();
        vector<long> serializedPrimaryDisplay;
        for (unsigned int i = 0; i < serializedPrimaryDisplayLength; i++)
        {
            serializedPrimaryDisplay.push_back(reader->ReadInt32());
        }
        m_standardCalculatorManager.DeSerializePrimaryDisplay(serializedPrimaryDisplay);

        CurrentRadixType = reader->ReadUInt32();
        //Read command data and Deserialize
        UINT32 modeldatalength = reader->ReadUInt32();
        Array<unsigned char>^ modelDataAsBytes = ref new Array<unsigned char>(modeldatalength);
        reader->ReadBytes(modelDataAsBytes);
        m_standardCalculatorManager.DeSerializeCommands(vector<unsigned char>(modelDataAsBytes->begin(), modelDataAsBytes->end()));

        // After recalculation. If there is an error then
        // IsInError should be set synchronously.
        if (IsInError)
        {
            shared_ptr<CalculatorVector<shared_ptr<IExpressionCommand>>> commandVector = Utils::DeserializeCommands(reader);
            shared_ptr<CalculatorVector <pair<wstring, int>>> tokenVector = Utils::DeserializeTokens(reader);
            SetExpressionDisplay(tokenVector, commandVector);
        }
    }
}

void StandardCalculatorViewModel::OnPropertyChanged(String^ propertyname)
{
    if (propertyname == CalculatorViewModelProperties::IsScientific)
    {
        if (IsScientific)
        {
            OnButtonPressed(NumbersAndOperatorsEnum::IsScientificMode);
        }
    }
    else if (propertyname == CalculatorViewModelProperties::IsProgrammer)
    {
        if (IsProgrammer)
        {
            OnButtonPressed(NumbersAndOperatorsEnum::IsProgrammerMode);
        }
    }
    else if (propertyname == CalculatorViewModelProperties::IsStandard)
    {
        if (IsStandard)
        {
            OnButtonPressed(NumbersAndOperatorsEnum::IsStandardMode);
        }
    }
    else if (propertyname == CalculatorViewModelProperties::DisplayValue)
    {
        RaisePropertyChanged(CalculationResultAutomationName_PropertyName);
        Announcement = GetDisplayUpdatedNarratorAnnouncement();
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
        ResetDisplay();
        SetPrecision(StandardModePrecision);
        UpdateMaxIntDigits();
        break;

    case ViewMode::Scientific:
        IsScientific = true;
        ResetDisplay();
        SetPrecision(ScientificModePrecision);
        break;

    case ViewMode::Programmer:
        IsProgrammer = true;
        ResetDisplay();
        SetPrecision(ProgrammerModePrecision);
        break;
    }
}

Platform::String^ StandardCalculatorViewModel::GetRawDisplayValue()
{
    wstring rawValue;

    LocalizationSettings::GetInstance().RemoveGroupSeparators(DisplayValue->Data(), DisplayValue->Length(), &rawValue);

    return ref new Platform::String(rawValue.c_str());
}

// Given a format string, returns a string with the input display value inserted.
//     'format' is a localized string containing a %1 formatting mark where the display value should be inserted.
//     'displayValue' is a localized string containing a numerical value to be displayed to the user.
String^ StandardCalculatorViewModel::GetLocalizedStringFormat(String^ format, String^ displayValue)
{
    String^ localizedString = ref new String(LocalizationStringUtil::GetLocalizedString(format->Data(), displayValue->Data()).c_str());
    return localizedString;
}


void StandardCalculatorViewModel::ResetDisplay()
{
    AreHEXButtonsEnabled = false;
    CurrentRadixType = (int)RADIX_TYPE::DEC_RADIX;
    m_standardCalculatorManager.SetRadix(DEC_RADIX);
    ProgModeRadixChange();
}

void StandardCalculatorViewModel::SetPrecision(int32_t precision)
{
    m_standardCalculatorManager.SetPrecision(precision);
}

void StandardCalculatorViewModel::SwitchProgrammerModeBase(RADIX_TYPE radixType)
{
    if (IsInError)
    {
        m_standardCalculatorManager.SendCommand(Command::CommandCLEAR);
    }

    AreHEXButtonsEnabled = (radixType == RADIX_TYPE::HEX_RADIX);
    CurrentRadixType = (int)radixType;
    m_standardCalculatorManager.SetRadix(radixType);
    ProgModeRadixChange();
}

void StandardCalculatorViewModel::SetMemorizedNumbersString()
{
    m_standardCalculatorManager.SetMemorizedNumbersString();
}

ANGLE_TYPE GetAngleTypeFromCommand(Command command)
{
    switch (command)
    {
    case Command::CommandDEG:
        return ANGLE_DEG;
    case Command::CommandRAD:
        return ANGLE_RAD;
    case Command::CommandGRAD:
        return ANGLE_GRAD;
    default:
        throw ref new Exception(E_FAIL, L"Invalid command type");
    }
}

void StandardCalculatorViewModel::SaveEditedCommand(_In_ unsigned int tokenPosition, _In_ Command command)
{
    pair<wstring, int> token;
    bool handleOperand = false;
    int nOpCode = static_cast<int>(command);
    wstring updatedToken = L"";

    shared_ptr<IExpressionCommand> tokenCommand;
    IFTPlatformException(m_tokens->GetAt(tokenPosition, &token));

    unsigned int tokenCommandIndex = token.second;
    IFTPlatformException(m_commands->GetAt(tokenCommandIndex, &tokenCommand));

    if (IsUnaryOp(nOpCode) && command != Command::CommandSIGN)
    {
        int angleCmd = static_cast<int>(m_standardCalculatorManager.GetCurrentDegreeMode());
        ANGLE_TYPE angleType = GetAngleTypeFromCommand(static_cast<Command>(angleCmd));

        if (IsTrigOp(nOpCode))
        {
            shared_ptr<IUnaryCommand> spUnaryCommand = dynamic_pointer_cast<IUnaryCommand>(tokenCommand);
            spUnaryCommand->SetCommands(angleCmd, nOpCode);
        }
        else
        {
            shared_ptr<IUnaryCommand> spUnaryCommand = dynamic_pointer_cast<IUnaryCommand>(tokenCommand);
            spUnaryCommand->SetCommand(nOpCode);
        }

        switch (nOpCode)
        {
        case static_cast<int>(Command::CommandASIN) :
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandSIN), true, angleType);
            break;
        case static_cast<int>(Command::CommandACOS) :
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandCOS), true, angleType);
            break;
        case static_cast<int>(Command::CommandATAN) :
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandTAN), true, angleType);
            break;
        case static_cast<int>(Command::CommandASINH) :
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandSINH), true, angleType);
            break;
        case static_cast<int>(Command::CommandACOSH) :
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandCOSH), true, angleType);
            break;
        case static_cast<int>(Command::CommandATANH) :
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandTANH), true, angleType);
            break;
        case static_cast<int>(Command::CommandPOWE) :
            updatedToken = CCalcEngine::OpCodeToUnaryString(static_cast<int>(Command::CommandLN), true, angleType);
            break;
        default:
            updatedToken = CCalcEngine::OpCodeToUnaryString(nOpCode, false, angleType);
        }
        if ((token.first.length() > 0) && (token.first[token.first.length() - 1] == L'('))
        {
            wstring chOpenBrace = L"(";
            updatedToken.append(chOpenBrace);
        }
    }
    else if (IsBinOp(nOpCode))
    {
        shared_ptr<IBinaryCommand> spBinaryCommand = dynamic_pointer_cast<IBinaryCommand>(tokenCommand);
        spBinaryCommand->SetCommand(nOpCode);
        updatedToken = CCalcEngine::OpCodeToString(nOpCode);
    }
    else if (IsOpnd(nOpCode) || command == Command::CommandBACK)
    {
        HandleUpdatedOperandData(command);
        handleOperand = true;
    }
    else if (command == Command::CommandSIGN)
    {
        if (tokenCommand->GetCommandType() == CommandType::UnaryCommand)
        {
            shared_ptr<IExpressionCommand> spSignCommand = make_shared<CUnaryCommand>(nOpCode);
            IFTPlatformException(m_commands->InsertAt(tokenCommandIndex + 1, spSignCommand));
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
        IFTPlatformException(m_commands->SetAt(tokenCommandIndex, tokenCommand));

        pair < wstring, int> selectedToken;
        IFTPlatformException(m_tokens->GetAt(tokenPosition, &selectedToken));
        selectedToken.first = updatedToken;
        IFTPlatformException(m_tokens->SetAt(tokenPosition, selectedToken));

        DisplayExpressionToken^ displayExpressionToken = ExpressionTokens->GetAt(tokenPosition);
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
    shared_ptr <CalculatorVector<shared_ptr<IExpressionCommand>>> savedCommands = make_shared <CalculatorVector<shared_ptr<IExpressionCommand>>>();

    vector<int> currentCommands;
    unsigned int commandListCount;
    m_commands->GetSize(&commandListCount);
    for (unsigned int i = 0; i < commandListCount; i++)
    {
        shared_ptr<IExpressionCommand> command;
        IFTPlatformException(m_commands->GetAt(i, &command));

        savedCommands->Append(command);
        CommandType commandType = command->GetCommandType();

        if (commandType == CommandType::UnaryCommand)
        {
            shared_ptr<IUnaryCommand> spCommand = dynamic_pointer_cast<IUnaryCommand>(command);
            shared_ptr<CalculatorVector<int>> unaryCommands = spCommand->GetCommands();
            unsigned int unaryCommandCount;
            unaryCommands->GetSize(&unaryCommandCount);

            int nUCode;
            for (unsigned int j = 0; j < unaryCommandCount; ++j)
            {
                IFTPlatformException(unaryCommands->GetAt(j, &nUCode));
                currentCommands.push_back(nUCode);
            }
        }

        if (commandType == CommandType::BinaryCommand)
        {
            shared_ptr<IBinaryCommand> spCommand = dynamic_pointer_cast<IBinaryCommand>(command);
            currentCommands.push_back(spCommand->GetCommand());
        }

        if (commandType == CommandType::Parentheses)
        {
            shared_ptr<IParenthesisCommand> spCommand = dynamic_pointer_cast<IParenthesisCommand>(command);
            currentCommands.push_back(spCommand->GetCommand());
        }

        if (commandType == CommandType::OperandCommand)
        {
            shared_ptr<IOpndCommand> spCommand = dynamic_pointer_cast<IOpndCommand>(command);
            shared_ptr<CalculatorVector<int>> opndCommands = spCommand->GetCommands();
            unsigned int opndCommandCount;
            opndCommands->GetSize(&opndCommandCount);
            bool fNeedIDCSign = spCommand->IsNegative();

            int nOCode;
            for (unsigned int j = 0; j < opndCommandCount; ++j)
            {
                IFTPlatformException(opndCommands->GetAt(j, &nOCode));
                currentCommands.push_back(nOCode);

                if (fNeedIDCSign && nOCode != IDC_0)
                {
                    currentCommands.push_back(static_cast<int>(CalculationManager::Command::CommandSIGN));
                    fNeedIDCSign = false;
                }
            }
        }
    }
    shared_ptr<CalculatorVector<pair<wstring, int>>> savedTokens = make_shared<CalculatorVector<pair<wstring, int>>>();

    unsigned int tokenCount;
    IFTPlatformException(m_tokens->GetSize(&tokenCount));

    for (unsigned int i = 0; i < tokenCount; ++i)
    {
        pair<wstring, int> currentToken;
        IFTPlatformException(m_tokens->GetAt(i, &currentToken));
        savedTokens->Append(currentToken);
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
    size_t currentCommandsSize = currentCommands.size();
    for (size_t i = 0; i < currentCommandsSize; i++)
    {
        m_standardCalculatorManager.SendCommand(static_cast<CalculationManager::Command>(currentCommands[i]));
    }

    if (fromHistory)   // This is for the cases where the expression is loaded from history    
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

CommandType StandardCalculatorViewModel::GetSelectedTokenType(_In_ unsigned int tokenPosition)
{
    pair<wstring, int>token;
    shared_ptr<IExpressionCommand> tokenCommand;
    IFTPlatformException(m_tokens->GetAt(tokenPosition, &token));

    unsigned int tokenCommandIndex = token.second;
    IFTPlatformException(m_commands->GetAt(tokenCommandIndex, &tokenCommand));

    return tokenCommand->GetCommandType();
}

bool StandardCalculatorViewModel::IsOpnd(int nOpCode)
{

    static Command opnd[] = {
        Command::Command0,
        Command::Command1,
        Command::Command2,
        Command::Command3,
        Command::Command4,
        Command::Command5,
        Command::Command6,
        Command::Command7,
        Command::Command8,
        Command::Command9,
        Command::CommandPNT
    };

    for (int i = 0; i < ARRAYSIZE(opnd); i++)
    {
        if (nOpCode == static_cast<int>(opnd[i]))
        {
            return true;
        }
    }
    return false;
}

bool StandardCalculatorViewModel::IsUnaryOp(int nOpCode)
{
    static Command unaryOp[] = {
        Command::CommandSQRT,
        Command::CommandFAC,
        Command::CommandSQR,
        Command::CommandLOG,
        Command::CommandPOW10,
        Command::CommandPOWE,
        Command::CommandLN,
        Command::CommandREC,
        Command::CommandSIGN,
        Command::CommandSINH,
        Command::CommandASINH,
        Command::CommandCOSH,
        Command::CommandACOSH,
        Command::CommandTANH,
        Command::CommandATANH,
        Command::CommandCUB
    };

    for (int i = 0; i < ARRAYSIZE(unaryOp); i++)
    {
        if (nOpCode == static_cast<int>(unaryOp[i]))
        {
            return true;
        }
    }

    if (IsTrigOp(nOpCode))
    {
        return true;
    }

    return false;
}

bool StandardCalculatorViewModel::IsTrigOp(int nOpCode)
{
    static Command trigOp[] = {
        Command::CommandSIN,
        Command::CommandCOS,
        Command::CommandTAN,
        Command::CommandASIN,
        Command::CommandACOS,
        Command::CommandATAN
    };

    for (int i = 0; i < ARRAYSIZE(trigOp); i++)
    {
        if (nOpCode == static_cast<int>(trigOp[i]))
        {
            return true;
        }
    }
    return false;
}

bool StandardCalculatorViewModel::IsBinOp(int nOpCode)
{
    static Command binOp[] = {
        Command::CommandADD,
        Command::CommandSUB,
        Command::CommandMUL,
        Command::CommandDIV,
        Command::CommandEXP,
        Command::CommandROOT,
        Command::CommandMOD,
        Command::CommandPWR
    };

    for (int i = 0; i < ARRAYSIZE(binOp); i++)
    {
        if (nOpCode == static_cast<int>(binOp[i]))
        {
            return true;
        }
    }
    return false;
}

bool StandardCalculatorViewModel::IsRecoverableCommand(int nOpCode)
{
    if (IsOpnd(nOpCode))
    {
        return true;
    }

    // Programmer mode, bit flipping
    int minBinPos = static_cast<int>(Command::CommandBINEDITSTART);
    int maxBinPos = static_cast<int>(Command::CommandBINEDITEND);
    if (minBinPos <= nOpCode && nOpCode <= maxBinPos)
    {
        return true;
    }

    static Command recoverableCommands[] = {
        Command::CommandA,
        Command::CommandB,
        Command::CommandC,
        Command::CommandD,
        Command::CommandE,
        Command::CommandF
    };

    for (int i = 0; i < ARRAYSIZE(recoverableCommands); i++)
    {
        if (nOpCode == static_cast<int>(recoverableCommands[i]))
        {
            return true;
        }
    }
    return false;
}

size_t StandardCalculatorViewModel::LengthWithoutPadding(wstring str)
{
    size_t count = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] != L' ')
        {
            count++;
        }
    }
    return count;
}

wstring StandardCalculatorViewModel::AddPadding(wstring binaryString)
{
    if (LocalizationSettings::GetInstance().GetEnglishValueFromLocalizedDigits(binaryString) == L"0")
    {
        return binaryString;
    }
    size_t pad = 4 - LengthWithoutPadding(binaryString) % 4;
    if (pad == 4)
    {
        pad = 0;
    }
    wstring padString = L"";
    for (size_t i = 0; i < pad; i++)
    {
        padString += L"0";
    }
    return padString + binaryString;
}

void StandardCalculatorViewModel::UpdateProgrammerPanelDisplay()
{
    wstring hexDisplayString;
    wstring decimalDisplayString;
    wstring octalDisplayString;
    wstring binaryDisplayString;
    if (!IsInError)
    {
        // we want the precision to be set to maximum value so that the autoconversions result as desired
        int32_t precision = 64;
        if (m_standardCalculatorManager.GetResultForRadix(16, precision) == L"")
        {
            hexDisplayString = DisplayValue->Data();
            decimalDisplayString = DisplayValue->Data();
            octalDisplayString = DisplayValue->Data();
            binaryDisplayString = DisplayValue->Data();
        }
        else
        {
            hexDisplayString = m_standardCalculatorManager.GetResultForRadix(16, precision);
            decimalDisplayString = m_standardCalculatorManager.GetResultForRadix(10, precision);
            octalDisplayString = m_standardCalculatorManager.GetResultForRadix(8, precision);
            binaryDisplayString = m_standardCalculatorManager.GetResultForRadix(2, precision);
        }
    }
    const auto& localizer = LocalizationSettings::GetInstance();
    binaryDisplayString = AddPadding(binaryDisplayString);

    localizer.LocalizeDisplayValue(&hexDisplayString);
    localizer.LocalizeDisplayValue(&decimalDisplayString);
    localizer.LocalizeDisplayValue(&octalDisplayString);
    localizer.LocalizeDisplayValue(&binaryDisplayString);

    HexDisplayValue = Utils::LRO + ref new Platform::String(hexDisplayString.c_str()) + Utils::PDF;
    DecimalDisplayValue = Utils::LRO + ref new Platform::String(decimalDisplayString.c_str()) + Utils::PDF;
    OctalDisplayValue = Utils::LRO + ref new Platform::String(octalDisplayString.c_str()) + Utils::PDF;
    BinaryDisplayValue = Utils::LRO + ref new Platform::String(binaryDisplayString.c_str()) + Utils::PDF;
    HexDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedHexaDecimalAutomationFormat, GetNarratorStringReadRawNumbers(HexDisplayValue));
    DecDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedDecimalAutomationFormat, DecimalDisplayValue);
    OctDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedOctalAutomationFormat, GetNarratorStringReadRawNumbers(OctalDisplayValue));
    BinDisplayValue_AutomationName = GetLocalizedStringFormat(m_localizedBinaryAutomationFormat, GetNarratorStringReadRawNumbers(BinaryDisplayValue));
}

void StandardCalculatorViewModel::SwitchAngleType(NumbersAndOperatorsEnum num)
{
    OnButtonPressed(num);
}

NumbersAndOperatorsEnum StandardCalculatorViewModel::ConvertIntegerToNumbersAndOperatorsEnum(unsigned int parameter)
{
    NumbersAndOperatorsEnum angletype;
    switch (parameter)
    {
    case 321:
        angletype = NumbersAndOperatorsEnum::Degree;
        break;
    case 322:
        angletype = NumbersAndOperatorsEnum::Radians;
        break;
    case 323:
        angletype = NumbersAndOperatorsEnum::Grads;
        break;
    default:
        angletype = NumbersAndOperatorsEnum::Degree;
    };
    return angletype;
}

void StandardCalculatorViewModel::UpdateOperand(int pos, String^ text)
{
    pair<wstring, int> p;
    m_tokens->GetAt(pos, &p);

    String^ englishString = LocalizationSettings::GetInstance().GetEnglishValueFromLocalizedDigits(text->Data());
    p.first = englishString->Data();

    int commandPos = p.second;
    shared_ptr<IExpressionCommand> exprCmd;
    m_commands->GetAt(commandPos, &exprCmd);
    auto operandCommand = std::dynamic_pointer_cast<IOpndCommand>(exprCmd);

    if (operandCommand != nullptr)
    {
        shared_ptr<CalculatorVector<int>> commands = make_shared<CalculatorVector<int>>();
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
                commands->Append(num);
            }
        }
        else
        {
            commands->Append(0);
        }
        operandCommand->SetCommands(commands);
    }
}

void  StandardCalculatorViewModel::UpdatecommandsInRecordingMode()
{
    vector<unsigned char> savedCommands = m_standardCalculatorManager.GetSavedCommands();
    shared_ptr<CalculatorVector<int>> commands = make_shared<CalculatorVector<int>>();
    bool isDecimal = false;
    bool isNegative = false;
    bool isExpMode = false;
    bool ePlusMode = false;
    bool eMinusMode = false;

    int num = 0;
    Command val;
    for (unsigned int i = 0; i < savedCommands.size(); ++i)
    {
        val = safe_cast<Command>(savedCommands[i]);
        num = static_cast<int>(val);
        if (val == Command::CommandSIGN)
        {
            isNegative = true;
            continue;
        }
        else if ((val >= Command::Command0 && val <= Command::Command9))
        {
        }
        else if (val == Command::CommandPNT)
        {
            isDecimal = true;
        }
        else if (val == Command::CommandEXP)
        {
            isExpMode = true;
        }
        else if (isExpMode && !ePlusMode && (val == Command::CommandMPLUS))
        {
            ePlusMode = true;
            continue;
        }
        else if (isExpMode && !eMinusMode && (val == Command::CommandMMINUS))
        {
            eMinusMode = true;
            continue;
        }
        else
        {
            //reset all vars
            isDecimal = false;
            isNegative = false;
            isExpMode = false;
            ePlusMode = false;
            eMinusMode = false;
            commands->Clear();
            continue;
        }
        commands->Append(num);
    }

    unsigned int size = 0;
    commands->GetSize(&size);
    if (size > 0)
    {
        shared_ptr<IOpndCommand> sp = make_shared<COpndCommand>(commands, isNegative, isDecimal, isExpMode);
        m_commands->Append(sp);
    }
    Recalculate();
}

void StandardCalculatorViewModel::OnMaxDigitsReached()
{
    String^ announcement = LocalizationStringUtil::GetLocalizedNarratorAnnouncement(
        CalculatorResourceKeys::MaxDigitsReachedFormat,
        m_localizedMaxDigitsReachedAutomationFormat,
        m_CalculationResultAutomationName->Data());

    Announcement = CalculatorAnnouncement::GetMaxDigitsReachedAnnouncement(announcement);
}

void StandardCalculatorViewModel::OnBinaryOperatorReceived()
{
    Announcement = GetDisplayUpdatedNarratorAnnouncement();
}

NarratorAnnouncement^ StandardCalculatorViewModel::GetDisplayUpdatedNarratorAnnouncement()
{
    String^ announcement;
    if (m_feedbackForButtonPress == nullptr || m_feedbackForButtonPress->IsEmpty())
    {
        announcement = m_CalculationResultAutomationName;
    }
    else
    {
        announcement = LocalizationStringUtil::GetLocalizedNarratorAnnouncement(
            CalculatorResourceKeys::ButtonPressFeedbackFormat,
            m_localizedButtonPressFeedbackAutomationFormat,
            m_CalculationResultAutomationName->Data(),
            m_feedbackForButtonPress->Data());
    }

    // Make sure we don't accidentally repeat an announcement.
    m_feedbackForButtonPress = nullptr;

    return CalculatorAnnouncement::GetDisplayUpdatedAnnouncement(announcement);
}
