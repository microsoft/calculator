// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <string>
#include "Header Files/CCommand.h"
#include "ExpressionCommand.h"

using namespace std;
using namespace CalcEngine;

constexpr wchar_t chNegate = L'-';
constexpr wchar_t chExp = L'e';
constexpr wchar_t chPlus = L'+';

CParentheses::CParentheses(_In_ int command)
    : m_command(command)
{
}

int CParentheses::GetCommand() const
{
    return m_command;
}

CalculationManager::CommandType CParentheses::GetCommandType() const
{
    return CalculationManager::CommandType::Parentheses;
}

void CParentheses::Accept(_In_ ISerializeCommandVisitor& commandVisitor)
{
    commandVisitor.Visit(*this);
}

CUnaryCommand::CUnaryCommand(int command)
{
    m_command = make_shared<vector<int>>();
    m_command->push_back(command);
}

CUnaryCommand::CUnaryCommand(int command1, int command2)
{
    m_command = make_shared<vector<int>>();
    m_command->push_back(command1);
    m_command->push_back(command2);
}

const shared_ptr<vector<int>>& CUnaryCommand::GetCommands() const
{
    return m_command;
}

CalculationManager::CommandType CUnaryCommand::GetCommandType() const
{
    return CalculationManager::CommandType::UnaryCommand;
}

void CUnaryCommand::SetCommand(int command)
{
    m_command->clear();
    m_command->push_back(command);
}

void CUnaryCommand::SetCommands(int command1, int command2)
{
    m_command->clear();
    m_command->push_back(command1);
    m_command->push_back(command2);
}

void CUnaryCommand::Accept(_In_ ISerializeCommandVisitor& commandVisitor)
{
    commandVisitor.Visit(*this);
}

CBinaryCommand::CBinaryCommand(int command)
    : m_command(command)
{
}

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

void CBinaryCommand::Accept(_In_ ISerializeCommandVisitor& commandVisitor)
{
    commandVisitor.Visit(*this);
}

COpndCommand::COpndCommand(shared_ptr<vector<int>> const& commands, bool fNegative, bool fDecimal, bool fSciFmt)
    : m_commands(commands)
    , m_fNegative(fNegative)
    , m_fSciFmt(fSciFmt)
    , m_fDecimal(fDecimal)
    , m_fInitialized(false)
    , m_value{}
{
}

void COpndCommand::Initialize(Rational const& rat)
{
    m_value = rat;
    m_fInitialized = true;
}

const shared_ptr<vector<int>>& COpndCommand::GetCommands() const
{
    return m_commands;
}

void COpndCommand::SetCommands(shared_ptr<vector<int>> const& commands)
{
    m_commands = commands;
}

void COpndCommand::AppendCommand(int command)
{
    if (m_fSciFmt)
    {
        ClearAllAndAppendCommand(static_cast<CalculationManager::Command>(command));
    }
    else
    {
        m_commands->push_back(command);
    }

    if (command == IDC_PNT)
    {
        m_fDecimal = true;
    }
}

void COpndCommand::ToggleSign()
{
    for (int nOpCode : *m_commands)
    {
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
        const size_t nCommands = m_commands->size();

        if (nCommands == 1)
        {
            ClearAllAndAppendCommand(CalculationManager::Command::Command0);
        }
        else
        {
            int nOpCode = m_commands->at(nCommands - 1);

            if (nOpCode == IDC_PNT)
            {
                m_fDecimal = false;
            }

            m_commands->pop_back();
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
    m_commands->clear();
    m_commands->push_back(static_cast<int>(command));
    m_fSciFmt = false;
    m_fNegative = false;
    m_fDecimal = false;
}

const wstring& COpndCommand::GetToken(wchar_t decimalSymbol)
{
    static const wchar_t chZero = L'0';

    const size_t nCommands = m_commands->size();
    m_token.clear();

    for (size_t i = 0; i < nCommands; i++)
    {
        int nOpCode = (*m_commands)[i];

        if (nOpCode == IDC_PNT)
        {
            m_token += decimalSymbol;
        }
        else if (nOpCode == IDC_EXP)
        {
            m_token += chExp;
            int nextOpCode = m_commands->at(i + 1);
            if (nextOpCode != IDC_SIGN)
            {
                m_token += chPlus;
            }
        }
        else if (nOpCode == IDC_SIGN)
        {
            m_token += chNegate;
        }
        else
        {
            wstring num = to_wstring(nOpCode - IDC_0);
            m_token.append(num);
        }
    }

    // Remove zeros
    for (size_t i = 0; i < m_token.size(); i++)
    {
        if (m_token.at(i) != chZero)
        {
            if (m_token.at(i) == decimalSymbol)
            {
                m_token.erase(0, i - 1);
            }
            else
            {
                m_token.erase(0, i);
            }

            if (m_fNegative)
            {
                m_token.insert(0, 1, chNegate);
            }

            return m_token;
        }
    }

    m_token = chZero;

    return m_token;
}

wstring COpndCommand::GetString(uint32_t radix, int32_t precision)
{
    if (m_fInitialized)
    {
        return m_value.ToString(radix, NumberFormat::Float, precision);
    }

    return wstring{};
}

void COpndCommand::Accept(_In_ ISerializeCommandVisitor& commandVisitor)
{
    commandVisitor.Visit(*this);
}
