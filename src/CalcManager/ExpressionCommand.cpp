// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Header Files/CCommand.h"
#include "CalculatorVector.h"
#include "ExpressionCommand.h"

using namespace std;
using namespace CalcEngine;

constexpr wchar_t chNegate = L'-';
constexpr wchar_t chExp = L'e';
constexpr wchar_t chPlus = L'+';

CParentheses::CParentheses(_In_ int command) :m_command(command)
{}

int CParentheses::GetCommand() const
{
    return m_command;
}

CalculationManager::CommandType CParentheses::GetCommandType() const
{
    return CalculationManager::CommandType::Parentheses;
}

void CParentheses::Accept(_In_ ISerializeCommandVisitor &commandVisitor)
{
    commandVisitor.Visit(*this);
}

CUnaryCommand::CUnaryCommand(int command)
{
    m_command = make_shared<CalculatorVector<int>>();
    m_command->Append(command);
}

CUnaryCommand::CUnaryCommand(int command1, int command2)
{
    m_command = make_shared<CalculatorVector<int>>();
    m_command->Append(command1);
    m_command->Append(command2);
}

const shared_ptr<CalculatorVector<int>> & CUnaryCommand::GetCommands() const
{
    return m_command;
}

CalculationManager::CommandType CUnaryCommand::GetCommandType() const
{
    return CalculationManager::CommandType::UnaryCommand;
}

void CUnaryCommand::SetCommand(int command)
{
    m_command->Clear();
    m_command->Append(command);
}

void CUnaryCommand::SetCommands(int command1, int command2)
{
    m_command->Clear();
    m_command->Append(command1);
    m_command->Append(command2);
}

void CUnaryCommand::Accept(_In_ ISerializeCommandVisitor &commandVisitor)
{
    commandVisitor.Visit(*this);
}

CBinaryCommand::CBinaryCommand(int command) :m_command(command)
{}

void CBinaryCommand::SetCommand(int command) 
{ 
    m_command = command; 
}

int CBinaryCommand::GetCommand() const
{ 
    return m_command; 
}

CalculationManager::CommandType CBinaryCommand::GetCommandType() const
{ 
    return CalculationManager::CommandType::BinaryCommand; 
}

void CBinaryCommand::Accept(_In_ ISerializeCommandVisitor &commandVisitor)
{
    commandVisitor.Visit(*this);
}

COpndCommand::COpndCommand(shared_ptr<CalculatorVector<int>> const &commands, bool fNegative, bool fDecimal, bool fSciFmt) :
    m_commands(commands),
    m_fNegative(fNegative),
    m_fDecimal(fDecimal),
    m_fSciFmt(fSciFmt),
    m_fInitialized(false),
    m_value{}
{}

void COpndCommand::Initialize(Rational const& rat)
{
    m_value = rat;
    m_fInitialized = true;
}

const shared_ptr<CalculatorVector<int>> & COpndCommand::GetCommands() const
{ 
    return m_commands; 
}

void COpndCommand::SetCommands(shared_ptr<CalculatorVector<int>> const& commands)
{
    m_commands = commands;
}

void COpndCommand::AppendCommand(int command)
{
    unsigned int nCommands;
    m_commands->GetSize(&nCommands);
    if (m_fSciFmt)
    {
        ClearAllAndAppendCommand(static_cast<CalculationManager::Command>(command));
    }
    else
    {
        m_commands->Append(command);
    }
    if (command == IDC_PNT)
    {
        m_fDecimal = true;
    }
}

void COpndCommand::ToggleSign()
{
    unsigned int commandCount;
    m_commands->GetSize(&commandCount);

    for (unsigned int i = 0; i < commandCount; i++)
    {
        int nOpCode;
        m_commands->GetAt(i, &nOpCode);

        if (nOpCode != IDC_0)
        {
            m_fNegative = !m_fNegative;
            break;
        }
    }
}

void COpndCommand::RemoveFromEnd()
{
    if (m_fSciFmt)
    {
        ClearAllAndAppendCommand(CalculationManager::Command::Command0);
    }
    else
    {
        unsigned int nCommands;
        m_commands->GetSize(&nCommands);
        
        if (nCommands == 1)
        {
            ClearAllAndAppendCommand(CalculationManager::Command::Command0);
        }
        else
        {
            int nOpCode;
            m_commands->GetAt(nCommands - 1, &nOpCode);
            if (nOpCode == IDC_PNT)
            {
                m_fDecimal = false;
            }
            m_commands->RemoveAt(nCommands - 1);
        }
    }
}

bool COpndCommand::IsNegative() const
{ 
    return m_fNegative; 
}

bool COpndCommand::IsSciFmt() const
{
    return m_fSciFmt;
}

bool COpndCommand::IsDecimalPresent() const
{ 
    return m_fDecimal; 
}

CalculationManager::CommandType COpndCommand::GetCommandType() const
{ 
    return CalculationManager::CommandType::OperandCommand; 
}

void COpndCommand::ClearAllAndAppendCommand(CalculationManager::Command command)
{
    m_commands->Clear();
    m_commands->Append(static_cast<int>(command));
    m_fSciFmt = false;
    m_fNegative = false;
    m_fDecimal = false;
}

const wstring & COpndCommand::GetToken(wchar_t decimalSymbol)
{
    static const wchar_t chZero = L'0';

    unsigned int nCommands;
    m_commands->GetSize(&nCommands);
    m_token.clear();
    int nOpCode;

    for (unsigned int i = 0; i < nCommands; i++)
    {
        m_commands->GetAt(i, &nOpCode);
        if (nOpCode == IDC_PNT)
        {
            m_token.append(wstring{ decimalSymbol });
        }
        else if (nOpCode == IDC_EXP)
        {
            m_token.append(&chExp);
            int nextOpCode;
            m_commands->GetAt(i + 1, &nextOpCode);
            if (nextOpCode != IDC_SIGN)
            {
                m_token.append(&chPlus);
            }
        }
        else if (nOpCode == IDC_SIGN)
        {
            m_token.append(&chNegate);
        }
        else
        {
            wstring num = to_wstring(nOpCode - IDC_0);
            m_token.append(num);
        }
    }

    // Remove zeros
    bool fDigitsFound = false;
    int trimIdx = 0;
    for (unsigned int i = 0; i < m_token.size(); i++)
    {
        if (m_token.at(i) != chZero)
        {
            if (m_token.at(i) == decimalSymbol)
            {
                trimIdx = i - 1;
            }
            else
            {
                trimIdx = i;
            }
            fDigitsFound = true;
            break;
        }
    }

    if (fDigitsFound)
    {
        m_token.erase(0, trimIdx);
        if (m_fNegative)
        {
            m_token.insert(0, &chNegate);
        }
    }
    else
    {
        m_token.clear();
        m_token.append(&chZero);
    }
    
    return m_token;
}

wstring COpndCommand::GetString(uint32_t radix, int32_t precision)
{
    wstring result{};

    if (m_fInitialized)
    {
        result = m_value.ToString(radix, eNUMOBJ_FMT::FMT_FLOAT, precision);
    }

    return result;
}

void COpndCommand::Accept(_In_ ISerializeCommandVisitor &commandVisitor)
{
    commandVisitor.Visit(*this);
}
