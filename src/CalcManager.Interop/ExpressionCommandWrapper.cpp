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
            }
            break;
        }
        }
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
}
