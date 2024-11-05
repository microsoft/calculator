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
    UnaryCommand::UnaryCommand()
    {
    }

    UnaryCommand::UnaryCommand(std::vector<int> cmds)
        : m_cmds(std::move(cmds))
    {
    }

    Windows::Foundation::Collections::IVectorView<int> ^ UnaryCommand::Commands::get()
    {
        return ref new Platform::Collections::VectorView<int>(m_cmds);
    }

    void UnaryCommand::Commands::set(Windows::Foundation::Collections::IVectorView<int> ^ commands)
    {
        m_cmds.clear();
        for (auto cmd : commands)
        {
            m_cmds.push_back(cmd);
        }
    }

    BinaryCommand::BinaryCommand()
    {
        Command = 0;
    }

    BinaryCommand::BinaryCommand(int cmd)
    {
        Command = cmd;
    }

    OperandCommand::OperandCommand()
    {
        IsNegative = false;
        IsDecimalPresent = false;
        IsSciFmt = false;
    }

    OperandCommand::OperandCommand(bool isNegative, bool isDecimal, bool isSciFmt, std::vector<int> cmds)
    {
        IsNegative = isNegative;
        IsDecimalPresent = isDecimal;
        IsSciFmt = isSciFmt;
        m_cmds = std::move(cmds);
    }

    Windows::Foundation::Collections::IVectorView<int> ^ OperandCommand::Commands::get()
    {
        return ref new Platform::Collections::VectorView<int>(m_cmds);
    }

    void OperandCommand::Commands::set(Windows::Foundation::Collections::IVectorView<int> ^ commands)
    {
        m_cmds.clear();
        for (auto cmd : commands)
        {
            m_cmds.push_back(cmd);
        }
    }

    Parentheses::Parentheses()
    {
        Command = 0;
    }

    Parentheses::Parentheses(int cmd)
    {
        Command = cmd;
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

    CalcManagerToken::CalcManagerToken()
    {
        OpCodeName = ref new Platform::String();
        CommandIndex = 0;
    }

    CalcManagerToken::CalcManagerToken(Platform::String ^ opCodeName, int cmdIndex)
    {
        assert(opCodeName != nullptr && "opCodeName is mandatory.");
        OpCodeName = opCodeName;
        CommandIndex = cmdIndex;
    }

    CalcManagerHistoryItem::CalcManagerHistoryItem()
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerToken ^>();
        Commands = ref new Platform::Collections::Vector<ICalcManagerIExprCommand ^>();
        Expression = ref new Platform::String();
        Result = ref new Platform::String();
    }

    CalcManagerHistoryItem::CalcManagerHistoryItem(const CalculationManager::HISTORYITEM& item)
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerToken ^>();
        assert(item.historyItemVector.spTokens != nullptr && "spTokens shall not be null.");
        for (auto& [opCode, cmdIdx] : *item.historyItemVector.spTokens)
        {
            Tokens->Append(ref new CalcManagerToken(ref new Platform::String(opCode.c_str()), cmdIdx));
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

    CalcManagerSnapshot::CalcManagerSnapshot()
    {
        HistoryItems = nullptr;
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

    PrimaryDisplaySnapshot::PrimaryDisplaySnapshot()
    {
        DisplayValue = ref new Platform::String();
        IsError = false;
    }

    PrimaryDisplaySnapshot::PrimaryDisplaySnapshot(Platform::String ^ display, bool isError)
    {
        assert(display != nullptr && "display is mandatory");
        DisplayValue = display;
        IsError = isError;
    }

    ExpressionDisplaySnapshot::ExpressionDisplaySnapshot()
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerToken ^>();
        Commands = ref new Platform::Collections::Vector<ICalcManagerIExprCommand ^>();
    }

    ExpressionDisplaySnapshot::ExpressionDisplaySnapshot(
        const std::vector<CalcHistoryToken>& tokens,
        const std::vector<std::shared_ptr<IExpressionCommand>>& commands)
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerToken ^>();
        for (auto& [opCode, cmdIdx] : tokens)
        {
            Tokens->Append(ref new CalcManagerToken(ref new Platform::String(opCode.c_str()), cmdIdx));
        }

        Commands = ref new Platform::Collections::Vector<ICalcManagerIExprCommand ^>();
        for (auto& cmd : commands)
        {
            Commands->Append(CreateExprCommand(cmd.get()));
        }
    }

    StandardCalculatorSnapshot::StandardCalculatorSnapshot()
    {
        CalcManager = ref new CalcManagerSnapshot();
        PrimaryDisplay = ref new PrimaryDisplaySnapshot();
        ExpressionDisplay = nullptr;
        DisplayCommands = ref new Platform::Collections::Vector<ICalcManagerIExprCommand ^>();
    }

    std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>> ToUnderlying(Windows::Foundation::Collections::IVector<CalcManagerHistoryItem ^> ^ items)
    {
        std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>> result;
        for (CalcManagerHistoryItem ^ item : items)
        {
            CalculationManager::HISTORYITEMVECTOR nativeItem;
            nativeItem.spTokens = std::make_shared<std::vector<std::pair<std::wstring, int>>>();
            for (CalcManagerToken ^ token : item->Tokens)
            {
                nativeItem.spTokens->push_back(std::make_pair(token->OpCodeName->Data(), token->CommandIndex));
            }
            nativeItem.spCommands = std::make_shared<std::vector<std::shared_ptr<IExpressionCommand>>>(ToUnderlying(item->Commands));
            nativeItem.expression = item->Expression->Data();
            nativeItem.result = item->Result->Data();
            auto spItem = std::make_shared<CalculationManager::HISTORYITEM>(CalculationManager::HISTORYITEM{ std::move(nativeItem) });
            result.push_back(std::move(std::move(spItem)));
        }
        return result;
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
