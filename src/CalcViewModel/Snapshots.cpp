// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <cassert>
#include <stdexcept>
#include <vector>

#include "CalcManager/ExpressionCommand.h"
#include "Snapshots.h"

namespace CalculatorApp::ViewModel::Snapshot
{
    UnaryCommand::UnaryCommand(Windows::Foundation::Collections::IVectorView<int> ^ cmds)
    {
        for (auto cmd : cmds)
        {
            m_cmds.push_back(cmd);
        }
    }

    Windows::Foundation::Collections::IVectorView<int> ^ UnaryCommand::Commands::get()
    {
        return ref new Platform::Collections::VectorView<int>(m_cmds);
    }

    OperandCommand::OperandCommand(bool isNegative, bool isDecimal, bool isSciFmt, Windows::Foundation::Collections::IVectorView<int> ^ cmds)
    {
        IsNegative = isNegative;
        IsDecimalPresent = isDecimal;
        IsSciFmt = isSciFmt;
        for (auto cmd : cmds)
        {
            m_cmds.push_back(cmd);
        }
    }

    ICalcManagerIExprCommand ^ CreateExprCommand(const IExpressionCommand* exprCmd) {
        switch (exprCmd->GetCommandType())
        {
        case CalculationManager::CommandType::UnaryCommand:
        {
            auto cmd = static_cast<const IUnaryCommand*>(exprCmd);
            std::vector<int> cmdlist;
            for (auto& subcmd : *cmd->GetCommands())
            {
                cmdlist.push_back(subcmd);
            }
            return ref new UnaryCommand(std::move(cmdlist));
        }
        case CalculationManager::CommandType::BinaryCommand:
        {
            auto cmd = static_cast<const IBinaryCommand*>(exprCmd);
            return ref new BinaryCommand(cmd->GetCommand());
        }
        case CalculationManager::CommandType::OperandCommand:
        {
            auto cmd = static_cast<const IOpndCommand*>(exprCmd);
            std::vector<int> cmdlist;
            for (auto& subcmd : *cmd->GetCommands())
            {
                cmdlist.push_back(subcmd);
            }
            return ref new OperandCommand(cmd->IsNegative(), cmd->IsDecimalPresent(), cmd->IsSciFmt(), std::move(cmdlist));
        }
        case CalculationManager::CommandType::Parentheses:
        {
            auto cmd = static_cast<const IParenthesisCommand*>(exprCmd);
            return ref new Parentheses(cmd->GetCommand());
        }
        default:
            throw std::logic_error{ "unhandled command type." };
        }
    }

    CalcManagerHistoryToken::CalcManagerHistoryToken(Platform::String ^ opCodeName, int cmdIndex)
    {
        assert(opCodeName != nullptr && "opCodeName is mandatory.");
        OpCodeName = opCodeName;
        CommandIndex = cmdIndex;
    }

    CalcManagerHistoryItem::CalcManagerHistoryItem(const CalculationManager::HISTORYITEM& item)
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerHistoryToken ^>();
        assert(item.historyItemVector.spTokens != nullptr && "spTokens shall not be null.");
        for (auto& [opCode, cmdIdx] : *item.historyItemVector.spTokens)
        {
            Tokens->Append(ref new CalcManagerHistoryToken(ref new Platform::String(opCode.c_str()), cmdIdx));
        }
        Commands = ref new Platform::Collections::Vector<ICalcManagerIExprCommand ^>();
        assert(item.historyItemVector.spCommands != nullptr && "spCommands shall not be null.");
        for (auto& cmd : *item.historyItemVector.spCommands)
        {
            Commands->Append(CreateExprCommand(cmd.get()));
        }
        Expression = ref new Platform::String(item.historyItemVector.expression.c_str());
        Result = ref new Platform::String(item.historyItemVector.result.c_str());
    }

    CalcManagerSnapshot::CalcManagerSnapshot(const CalculationManager::CalculatorManager& calcMgr)
    {
        auto& items = calcMgr.GetHistoryItems();
        if (!items.empty())
        {
            HistoryItems = ref new Platform::Collections::Vector<CalcManagerHistoryItem ^>();
            for (auto& item : items)
            {
                HistoryItems->Append(ref new CalcManagerHistoryItem(*item));
            }
        }
    }

    PrimaryDisplaySnapshot::PrimaryDisplaySnapshot(Platform::String ^ display, bool isError)
    {
        assert(display != nullptr && "display is mandatory");
        DisplayValue = display;
        IsError = isError;
    }

    ExpressionDisplaySnapshot::ExpressionDisplaySnapshot(
        const std::vector<CalcHistoryToken>& tokens,
        const std::vector<std::shared_ptr<IExpressionCommand>>& commands)
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerHistoryToken ^>();
        for (auto& [opCode, cmdIdx] : tokens)
        {
            Tokens->Append(ref new CalcManagerHistoryToken(ref new Platform::String(opCode.c_str()), cmdIdx));
        }

        Commands = ref new Platform::Collections::Vector<ICalcManagerIExprCommand ^>();
        for (auto& cmd : commands)
        {
            Commands->Append(CreateExprCommand(cmd.get()));
        }
    }

    std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>> ToUnderlying(Windows::Foundation::Collections::IVector<CalcManagerHistoryItem ^> ^ items)
    {
        return {}; // TODO
    }

    std::vector<std::shared_ptr<IExpressionCommand>> ToUnderlying(Windows::Foundation::Collections::IVector<ICalcManagerIExprCommand ^> ^ commands)
    {
        std::vector<std::shared_ptr<IExpressionCommand>> result;
        for (ICalcManagerIExprCommand ^ cmdEntry : commands)
        {
            if (auto unary = dynamic_cast<UnaryCommand ^>(cmdEntry); unary != nullptr)
            {
                if (unary->m_cmds.size() == 1)
                {
                    result.push_back(std::make_shared<CUnaryCommand>(unary->m_cmds[0]));
                }
                else if (unary->m_cmds.size() == 2)
                {
                    result.push_back(std::make_shared<CUnaryCommand>(unary->m_cmds[0], unary->m_cmds[1]));
                }
                else
                {
                    throw std::logic_error{ "ill-formed command." };
                }
            }
            else if (auto binary = dynamic_cast<BinaryCommand ^>(cmdEntry); binary != nullptr)
            {
                result.push_back(std::make_shared<CBinaryCommand>(binary->Command));
            }
            else if (auto paren = dynamic_cast<Parentheses ^>(cmdEntry); paren != nullptr)
            {
                result.push_back(std::make_shared<CParentheses>(paren->Command));
            }
            else if (auto operand = dynamic_cast<OperandCommand ^>(cmdEntry); operand != nullptr)
            {
                auto subcmds = std::make_shared<std::vector<int>>(operand->m_cmds);
                result.push_back(std::make_shared<COpndCommand>(std::move(subcmds), operand->IsNegative, operand->IsDecimalPresent, operand->IsSciFmt));
            }
        }
        return result;
    }
} // namespace CalculatorApp::ViewModel
