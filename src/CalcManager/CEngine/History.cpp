// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#pragma once
#include "Header Files/CalcEngine.h"
#include "Command.h"
#include "CalculatorVector.h"
#include "ExpressionCommand.h"
#include "CalcException.h"

constexpr int ASCII_0 = 48;

using namespace std;
using namespace CalcEngine;

void CHistoryCollector::ReinitHistory()
{
    m_lastOpStartIndex = -1;
    m_lastBinOpStartIndex = -1;
    m_curOperandIndex = 0;
    m_bLastOpndBrace = false;
    if (m_spTokens != nullptr)
    {
        m_spTokens->Clear();
    }
    if (m_spCommands != nullptr)
    {
        m_spCommands->Clear();
    }
}

// Constructor
// Can throw Out of memory error
CHistoryCollector::CHistoryCollector(ICalcDisplay *pCalcDisplay, std::shared_ptr<IHistoryDisplay> pHistoryDisplay, wchar_t decimalSymbol) :
    m_pHistoryDisplay(pHistoryDisplay),
    m_pCalcDisplay(pCalcDisplay),
    m_iCurLineHistStart(-1),
    m_decimalSymbol(decimalSymbol)
{
    ReinitHistory();
}

CHistoryCollector::~CHistoryCollector()
{
    m_pHistoryDisplay = nullptr;
    m_pCalcDisplay = nullptr;

    if (m_spTokens != nullptr)
    {
        m_spTokens->Clear();
    }
}

void CHistoryCollector::AddOpndToHistory(wstring_view numStr, Rational const& rat, bool fRepetition)
{
    std::shared_ptr<CalculatorVector<int>> commands = std::make_shared<CalculatorVector<int>>();
    // Check for negate
    bool fNegative = (numStr[0] == L'-');
    bool fSciFmt = false;
    bool fDecimal = false;

    for (size_t i = (fNegative ? 1 : 0); i < numStr.length(); i++)
    {
        if (numStr[i] == m_decimalSymbol)
        {
            IFT(commands->Append(IDC_PNT));
            if (!fSciFmt)
            {
                fDecimal = true;
            }
        }
        else if (numStr[i] == L'e')
        {
            IFT(commands->Append(IDC_EXP));
            fSciFmt = true;
        }
        else if (numStr[i] == L'-')
        {
            IFT(commands->Append(IDC_SIGN));
        }
        else if (numStr[i] == L'+')
        {
            // Ignore.
        }
        // Number
        else
        {
            int num = static_cast<int>(numStr[i]) - ASCII_0;
            num += IDC_0;
            IFT(commands->Append(num));
        }
    }

    auto operandCommand = std::make_shared<COpndCommand>(commands, fNegative, fDecimal, fSciFmt);
    operandCommand->Initialize(rat);
    int iCommandEnd = AddCommand(operandCommand);
    m_lastOpStartIndex = IchAddSzToEquationSz(numStr, iCommandEnd);

    if (fRepetition)
    {
        SetExpressionDisplay();
    }
    m_bLastOpndBrace = false;
    m_lastBinOpStartIndex = -1;
}

void CHistoryCollector::RemoveLastOpndFromHistory()
{
    TruncateEquationSzFromIch(m_lastOpStartIndex);
    SetExpressionDisplay();
    m_lastOpStartIndex = -1;
    // This will not restore the m_lastBinOpStartIndex, as it isn't possible to remove that also later
}

void CHistoryCollector::AddBinOpToHistory(int nOpCode, bool fNoRepetition)
{
    int iCommandEnd = AddCommand(std::make_shared<CBinaryCommand>(nOpCode));
    m_lastBinOpStartIndex = IchAddSzToEquationSz(L" ", -1);

    IchAddSzToEquationSz(CCalcEngine::OpCodeToString(nOpCode), iCommandEnd);
    IchAddSzToEquationSz(L" ", -1);

    if (fNoRepetition)
    {
        SetExpressionDisplay();
    }
    m_lastOpStartIndex = -1;
}

// This is expected to be called when a binary op in the last say 1+2+ is changing to another one say 1+2* (+ changed to *)
// It needs to know by this change a Precedence inversion happened. i.e. previous op was lower or equal to its previous op, but the new 
// one isn't. (Eg. 1*2* to 1*2^). It can add explicit brackets to ensure the precedence is inverted. (Eg. (1*2) ^)
void CHistoryCollector::ChangeLastBinOp(int nOpCode, bool fPrecInvToHigher)
{
    TruncateEquationSzFromIch(m_lastBinOpStartIndex);
    if (fPrecInvToHigher)
    {
        EnclosePrecInversionBrackets();
    }
    AddBinOpToHistory(nOpCode);
}

void CHistoryCollector::PushLastOpndStart(int ichOpndStart)
{
    int ich = (ichOpndStart == -1) ? m_lastOpStartIndex : ichOpndStart;

    if (m_curOperandIndex < static_cast<int>(m_operandIndices.size()))
    {
        m_operandIndices[m_curOperandIndex++] = ich;
    }
}

void CHistoryCollector::PopLastOpndStart()
{
    if (m_curOperandIndex > 0)
    {
        m_lastOpStartIndex = m_operandIndices[--m_curOperandIndex];
    }
}

void CHistoryCollector::AddOpenBraceToHistory()
{
    AddCommand(std::make_shared<CParentheses>(IDC_OPENP));
    int ichOpndStart = IchAddSzToEquationSz(CCalcEngine::OpCodeToString(IDC_OPENP), -1);
    PushLastOpndStart(ichOpndStart);

    SetExpressionDisplay();
    m_lastBinOpStartIndex = -1;
}

void CHistoryCollector::AddCloseBraceToHistory()
{
    AddCommand(std::make_shared<CParentheses>(IDC_CLOSEP));
    IchAddSzToEquationSz(CCalcEngine::OpCodeToString(IDC_CLOSEP), -1);
    SetExpressionDisplay();
    PopLastOpndStart();

    m_lastBinOpStartIndex = -1;
    m_bLastOpndBrace = true;
}

void CHistoryCollector::EnclosePrecInversionBrackets()
{
    // Top of the Opnd starts index or 0 is nothing is in top
    int ichStart = (m_curOperandIndex > 0) ? m_operandIndices[m_curOperandIndex - 1] : 0;

    InsertSzInEquationSz(CCalcEngine::OpCodeToString(IDC_OPENP), -1, ichStart);
    IchAddSzToEquationSz(CCalcEngine::OpCodeToString(IDC_CLOSEP), -1);
}

bool CHistoryCollector::FOpndAddedToHistory()
{
    return (-1 != m_lastOpStartIndex);
}

// AddUnaryOpToHistory
//
// This is does the postfix to prefix translation of the input and adds the text to the history. Eg. doing 2 + 4 (sqrt), 
// this routine will ensure the last sqrt call unary operator, actually goes back in history and wraps 4 in sqrt(4)
//
void CHistoryCollector::AddUnaryOpToHistory(int nOpCode, bool fInv, ANGLE_TYPE angletype)
{
    int iCommandEnd;
    // When successfully applying a unary op, there should be an opnd already
    // A very special case of % which is a funny post op unary op.
    if (IDC_PERCENT == nOpCode)
    {
        iCommandEnd = AddCommand(std::make_shared<CUnaryCommand>(nOpCode));
        IchAddSzToEquationSz(CCalcEngine::OpCodeToString(nOpCode), iCommandEnd);
    }
    else // all the other unary ops
    {
        std::shared_ptr<IOperatorCommand> spExpressionCommand;
        if (IDC_SIGN == nOpCode)
        {
            spExpressionCommand = std::make_shared<CUnaryCommand>(nOpCode);
        }
        else
        {
            CalculationManager::Command angleOpCode;
            if (angletype == ANGLE_DEG)
            {
                angleOpCode = CalculationManager::Command::CommandDEG;
            }
            else if (angletype == ANGLE_RAD)
            {
                angleOpCode = CalculationManager::Command::CommandRAD;
            }
            else // (angletype == ANGLE_GRAD)
            {
                angleOpCode = CalculationManager::Command::CommandGRAD;
            }

            int command = nOpCode;
            switch (nOpCode)
            {
            case IDC_SIN:
                command = fInv ? static_cast<int>(CalculationManager::Command::CommandASIN) : IDC_SIN;
                spExpressionCommand = std::make_shared<CUnaryCommand>(static_cast<int>(angleOpCode), command);
                break;
            case IDC_COS:
                command = fInv ? static_cast<int>(CalculationManager::Command::CommandACOS) : IDC_COS;
                spExpressionCommand = std::make_shared<CUnaryCommand>(static_cast<int>(angleOpCode), command);
                break;
            case IDC_TAN:
                command = fInv ? static_cast<int>(CalculationManager::Command::CommandATAN) : IDC_TAN;
                spExpressionCommand = std::make_shared<CUnaryCommand>(static_cast<int>(angleOpCode), command);
                break;
            case IDC_SINH:
                command = fInv ? static_cast<int>(CalculationManager::Command::CommandASINH) : IDC_SINH;
                spExpressionCommand = std::make_shared<CUnaryCommand>(command);
                break;
            case IDC_COSH:
                command = fInv ? static_cast<int>(CalculationManager::Command::CommandACOSH) : IDC_COSH;
                spExpressionCommand = std::make_shared<CUnaryCommand>(command);
                break;
            case IDC_TANH:
                command = fInv ? static_cast<int>(CalculationManager::Command::CommandATANH) : IDC_TANH;
                spExpressionCommand = std::make_shared<CUnaryCommand>(command);
                break;
            case IDC_LN:
                command = fInv ? static_cast<int>(CalculationManager::Command::CommandPOWE) : IDC_LN;
                spExpressionCommand = std::make_shared<CUnaryCommand>(command);
                break;
            default:
                spExpressionCommand = std::make_shared<CUnaryCommand>(nOpCode);
            }
        }

        iCommandEnd = AddCommand(spExpressionCommand);

        wstring operandStr{ CCalcEngine::OpCodeToUnaryString(nOpCode, fInv, angletype) };
        if (!m_bLastOpndBrace) // The opnd is already covered in braces. No need for additional braces around it
        {
            operandStr.append(CCalcEngine::OpCodeToString(IDC_OPENP));
        }
        InsertSzInEquationSz(operandStr, iCommandEnd, m_lastOpStartIndex);

        if (!m_bLastOpndBrace)
        {
            IchAddSzToEquationSz(CCalcEngine::OpCodeToString(IDC_CLOSEP), -1);
        }
    }

    SetExpressionDisplay();
    m_bLastOpndBrace = false;
    // m_lastOpStartIndex remains the same as last opnd is just replaced by unaryop(lastopnd)
    m_lastBinOpStartIndex = -1;
}

// Called after = with the result of the equation
// Responsible for clearing the top line of current running history display, as well as adding yet another element to 
// history of equations
void CHistoryCollector::CompleteHistoryLine(wstring_view numStr)
{
    if (nullptr != m_pCalcDisplay)
    {
        m_pCalcDisplay->SetExpressionDisplay(std::make_shared<CalculatorVector<std::pair<std::wstring, int>>>(), std::make_shared<CalculatorVector<std::shared_ptr<IExpressionCommand>>>());
    }

    if (nullptr != m_pHistoryDisplay)
    {
        unsigned int addedItemIndex = m_pHistoryDisplay->AddToHistory(m_spTokens, m_spCommands, numStr);
        m_pCalcDisplay->OnHistoryItemAdded(addedItemIndex);
    }

    m_spTokens = nullptr;
    m_spCommands = nullptr;
    m_iCurLineHistStart = -1; // It will get recomputed at the first Opnd
    ReinitHistory();
}

void CHistoryCollector::ClearHistoryLine(wstring_view errStr)
{
    if (errStr.empty()) // in case of error let the display stay as it is
    {
        if (nullptr != m_pCalcDisplay)
        {
            m_pCalcDisplay->SetExpressionDisplay(std::make_shared<CalculatorVector<std::pair<std::wstring, int>>>(), std::make_shared<CalculatorVector<std::shared_ptr<IExpressionCommand>>>());
        }
        m_iCurLineHistStart = -1; // It will get recomputed at the first Opnd
        ReinitHistory();
    }
}


// Adds the given string psz to the globally maintained current equation string at the end.
//  Also returns the 0 based index in the string just added. Can throw out of memory error
int CHistoryCollector::IchAddSzToEquationSz(wstring_view str, int icommandIndex)
{
    if (m_spTokens == nullptr)
    {
        m_spTokens = std::make_shared<CalculatorVector<std::pair<std::wstring, int>>>();
    }

    if (FAILED(m_spTokens->Append(std::make_pair(wstring(str), icommandIndex))))
    {
        throw(CALC_E_OUTOFMEMORY);
    }

    unsigned int nTokens;
    m_spTokens->GetSize(&nTokens);
    return nTokens - 1;
}

// Inserts a given string into the global m_pszEquation at the given index ich taking care of reallocations etc.
void CHistoryCollector::InsertSzInEquationSz(wstring_view str, int icommandIndex, int ich)
{
    if (FAILED(m_spTokens->InsertAt(ich, std::make_pair(wstring(str), icommandIndex))))
    {
        throw(CALC_E_OUTOFMEMORY);
    }
}

// Chops off the current equation string from the given index
void CHistoryCollector::TruncateEquationSzFromIch(int ich)
{
    // Truncate commands
    int minIdx = -1;
    unsigned int nTokens = 0;
    std::pair<std::wstring, int> currentPair;
    m_spTokens->GetSize(&nTokens);

    for (unsigned int i = ich; i < nTokens; i++)
    {
        IFT(m_spTokens->GetAt(i, &currentPair));
        int curTokenId = currentPair.second;
        if (curTokenId != -1)
        {
            if ((minIdx != -1) || (curTokenId < minIdx))
            {
                minIdx = curTokenId;
                IFT(m_spCommands->Truncate(minIdx));
            }
        }
    }

    IFT(m_spTokens->Truncate(ich));
}

// Adds the m_pszEquation into the running history text
void CHistoryCollector::SetExpressionDisplay()
{
    if (nullptr != m_pCalcDisplay)
    {
        m_pCalcDisplay->SetExpressionDisplay(m_spTokens, m_spCommands);
    }

}

int CHistoryCollector::AddCommand(_In_ const std::shared_ptr<IExpressionCommand> & spCommand)
{
    if (m_spCommands == nullptr)
    {
        m_spCommands = std::make_shared <CalculatorVector<std::shared_ptr<IExpressionCommand>>>();
    }

    if (FAILED(m_spCommands->Append(spCommand)))
    {
        throw(CALC_E_OUTOFMEMORY);
    }

    unsigned int nCommands = 0;
    m_spCommands->GetSize(&nCommands);
    return nCommands - 1;
}

//To Update the operands in the Expression according to the current Radix 
void CHistoryCollector::UpdateHistoryExpression(uint32_t radix, int32_t precision)
{
    if (m_spTokens != nullptr)
    {
        unsigned int size;
        IFT(m_spTokens->GetSize(&size));

        for (unsigned int i = 0; i < size; ++i)
        {
            std::pair<std::wstring, int> token;
            IFT(m_spTokens->GetAt(i, &token));
            int commandPosition = token.second;
            if (commandPosition != -1)
            {
                std::shared_ptr<IExpressionCommand> expCommand;
                IFT(m_spCommands->GetAt(commandPosition, &expCommand));
                if (expCommand != nullptr && CalculationManager::CommandType::OperandCommand == expCommand->GetCommandType())
                {
                    std::shared_ptr<COpndCommand> opndCommand = std::static_pointer_cast<COpndCommand>(expCommand);
                    if (opndCommand != nullptr)
                    {
                        token.first = opndCommand->GetString(radix, precision);
                        IFT(m_spTokens->SetAt(i, token));
                        opndCommand->SetCommands(GetOperandCommandsFromString(token.first));
                    }
                }
            }
        }
        SetExpressionDisplay();
    }
}

void CHistoryCollector::SetDecimalSymbol(wchar_t decimalSymbol)
{
    m_decimalSymbol = decimalSymbol;
}

//Update the commands corresponding to the passed string Number
std::shared_ptr<CalculatorVector<int>> CHistoryCollector::GetOperandCommandsFromString(wstring_view numStr)
{
    std::shared_ptr<CalculatorVector<int>> commands = std::make_shared<CalculatorVector<int>>();
    // Check for negate
    bool fNegative = (numStr[0] == L'-');

    for (size_t i = (fNegative ? 1 : 0); i < numStr.length(); i++)
    {
        if (numStr[i] == m_decimalSymbol)
        {
            IFT(commands->Append(IDC_PNT));
        }
        else if (numStr[i] == L'e')
        {
            IFT(commands->Append(IDC_EXP));
        }
        else if (numStr[i] == L'-')
        {
            IFT(commands->Append(IDC_SIGN));
        }
        else if (numStr[i] == L'+')
        {
            // Ignore.
        }
        // Number
        else
        {
            int num = static_cast<int>(numStr[i]) - ASCII_0;
            num += IDC_0;
            IFT(commands->Append(num));
        }
    }

    // If the number is negative, append a sign command at the end.
    if (fNegative)
    {
        IFT(commands->Append(IDC_SIGN));
    }
    return commands;
}
