// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ExpressionCommandWrapper.h"
namespace winrt::CalcManager::Interop::implementation
{
    ExpressionCommandWrapper::ExpressionCommandWrapper(const std::shared_ptr<IExpressionCommand>& command)
    {
        if (!command)
        {
            return;
        }

        auto nativeType = command->GetCommandType();
        m_type = static_cast<CalcManager::Interop::CommandType>(static_cast<int>(nativeType));

        switch (nativeType)
        {
        case CalculationManager::CommandType::BinaryCommand:
        {
            auto binaryCmd = dynamic_cast<IBinaryCommand*>(command.get());
            if (binaryCmd)
            {
                m_command = binaryCmd->GetCommand();
            }
            break;
        }
        case CalculationManager::CommandType::Parentheses:
        {
            auto parenCmd = dynamic_cast<IParenthesisCommand*>(command.get());
            if (parenCmd)
            {
                m_command = parenCmd->GetCommand();
            }
            break;
        }
        case CalculationManager::CommandType::UnaryCommand:
        {
            auto unaryCmd = dynamic_cast<IUnaryCommand*>(command.get());
            if (unaryCmd)
            {
                auto const& cmds = unaryCmd->GetCommands();
                if (cmds)
                {
                    m_commands.assign(cmds->begin(), cmds->end());
                }
            }
            break;
        }
        case CalculationManager::CommandType::OperandCommand:
        {
            auto opndCmd = dynamic_cast<IOpndCommand*>(command.get());
            if (opndCmd)
            {
                auto const& cmds = opndCmd->GetCommands();
                if (cmds)
                {
                    m_commands.assign(cmds->begin(), cmds->end());
                }
                m_isNegative = opndCmd->IsNegative();
                m_isDecimalPresent = opndCmd->IsDecimalPresent();
                m_isSciFmt = opndCmd->IsSciFmt();
            }
            break;
        }
        }
    }

    ExpressionCommandWrapper::ExpressionCommandWrapper(
        CalcManager::Interop::CommandType type,
        int32_t command,
        array_view<int32_t const> commands,
        bool isNegative,
        bool isDecimalPresent,
        bool isSciFmt)
        : m_type(type)
        , m_command(command)
        , m_commands(commands.begin(), commands.end())
        , m_isNegative(isNegative)
        , m_isDecimalPresent(isDecimalPresent)
        , m_isSciFmt(isSciFmt)
    {
    }

    std::shared_ptr<IExpressionCommand> ExpressionCommandWrapper::ToUnderlying() const
    {
        switch (m_type)
        {
        case CalcManager::Interop::CommandType::UnaryCommand:
            // Unary commands contain one or two command codes.
            if (m_commands.size() == 1)
            {
                return std::make_shared<CUnaryCommand>(m_commands[0]);
            }
            if (m_commands.size() == 2)
            {
                return std::make_shared<CUnaryCommand>(m_commands[0], m_commands[1]);
            }
            throw hresult_invalid_argument(L"ill-formed unary command.");

        case CalcManager::Interop::CommandType::BinaryCommand:
            return std::make_shared<CBinaryCommand>(m_command);

        case CalcManager::Interop::CommandType::Parentheses:
            return std::make_shared<CParentheses>(m_command);

        case CalcManager::Interop::CommandType::OperandCommand:
        {
            auto subCommands = std::make_shared<std::vector<int>>(m_commands.begin(), m_commands.end());
            return std::make_shared<COpndCommand>(std::move(subCommands), m_isNegative, m_isDecimalPresent, m_isSciFmt);
        }
        }

        throw hresult_invalid_argument(L"unhandled command type.");
    }

    CalcManager::Interop::CommandType ExpressionCommandWrapper::Type()
    {
        return m_type;
    }

    int32_t ExpressionCommandWrapper::Command()
    {
        return m_command;
    }

    com_array<int32_t> ExpressionCommandWrapper::Commands()
    {
        return com_array<int32_t>(m_commands);
    }

    bool ExpressionCommandWrapper::IsNegative()
    {
        return m_isNegative;
    }

    bool ExpressionCommandWrapper::IsDecimalPresent()
    {
        return m_isDecimalPresent;
    }

    bool ExpressionCommandWrapper::IsSciFmt()
    {
        return m_isSciFmt;
    }
}
