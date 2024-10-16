// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <cassert>
#include <stdexcept>
#include <vector>

#include "Snapshots.h"

namespace
{
    ref struct UnaryCommand sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        internal :;
        std::vector<int> Commands;
    };

    ref struct BinaryCommand sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        internal :;
        int Command;
    };

    ref struct OperandCommand sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        internal :;
        bool IsNegative;
        bool IsDecimalPresent;
        bool IsSciFmt;
        std::vector<int> Commands;
    };

    ref struct Parentheses sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        internal :;
        int Command;
    };

} // namespace

namespace CalculatorApp::ViewModel
{
    ICalcManagerIExprCommand ^ CreateExprCommand(const IExpressionCommand* exprCmd) {
        switch (exprCmd->GetCommandType())
        {
        case CalculationManager::CommandType::UnaryCommand:
        {
            auto cmd = static_cast<const IUnaryCommand*>(exprCmd);
            auto result = ref new UnaryCommand();
            for (auto& subcmd : *cmd->GetCommands())
            {
                result->Commands.push_back(subcmd);
            }
            return result;
        }
        case CalculationManager::CommandType::BinaryCommand:
        {
            auto cmd = static_cast<const IBinaryCommand*>(exprCmd);
            auto result = ref new BinaryCommand();
            result->Command = cmd->GetCommand();
            return result;
        }
        case CalculationManager::CommandType::OperandCommand:
        {
            auto cmd = static_cast<const IOpndCommand*>(exprCmd);
            auto result = ref new OperandCommand();
            result->IsNegative = cmd->IsNegative();
            result->IsDecimalPresent = cmd->IsDecimalPresent();
            result->IsSciFmt = cmd->IsSciFmt();
            for (auto& subcmd : *cmd->GetCommands())
            {
                result->Commands.push_back(subcmd);
            }
            return result;
        }
        case CalculationManager::CommandType::Parentheses:
        {
            auto cmd = static_cast<const IParenthesisCommand*>(exprCmd);
            auto result = ref new Parentheses();
            result->Command = cmd->GetCommand();
            return result;
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
} // namespace CalculatorApp::ViewModel
