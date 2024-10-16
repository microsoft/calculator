// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <cassert>
#include <stdexcept>

#include "Snapshots.h"

namespace
{
    ref struct UnaryCommand sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        property Windows::Foundation::Collections::IVector<int> ^ Commands;
    };

    ref struct BinaryCommand sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        property int Command;
    };

    ref struct OperandCommand sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        property bool IsNegative;
        property bool IsDecimalPresent;
        property bool IsSciFmt;
        property Windows::Foundation::Collections::IVector<int> ^ Commands;
    };

    ref struct Parentheses sealed : public CalculatorApp::ViewModel::ICalcManagerIExprCommand
    {
        property int Command;
    };

    CalculatorApp::ViewModel::ICalcManagerIExprCommand ^ CreateExprCommand(const IExpressionCommand* exprCmd) {
        switch (exprCmd->GetCommandType())
        {
        case CalculationManager::CommandType::UnaryCommand:
        {
            auto cmd = static_cast<const IUnaryCommand*>(exprCmd);
            auto result = ref new UnaryCommand();
            result->Commands = ref new Platform::Collections::Vector<int>();
            for (auto& subcmd : *cmd->GetCommands())
            {
                result->Commands->Append(subcmd);
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
            result->Commands = ref new Platform::Collections::Vector<int>();
            for (auto& subcmd : *cmd->GetCommands())
            {
                result->Commands->Append(subcmd);
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
} // namespace

namespace CalculatorApp::ViewModel
{
    CalcManagerHistoryToken::CalcManagerHistoryToken(Platform::String ^ opCodeName, int cmdIndex)
    {
        assert(opCodeName != nullptr && "opCodeName is mandatory.");
        OpCodeName = opCodeName;
        CommandIndex = cmdIndex;
    }

    CalcManagerHistoryItem::CalcManagerHistoryItem(const CalculationManager::HISTORYITEM& item)
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerHistoryToken ^>();
        Commands = ref new Platform::Collections::Vector<ICalcManagerIExprCommand ^>();
        if (item.historyItemVector.spTokens != nullptr)
        {
            for (auto& [opCode, cmdIdx] : *item.historyItemVector.spTokens)
            {
                Tokens->Append(ref new CalcManagerHistoryToken(ref new Platform::String(opCode.c_str()), cmdIdx));
                // Commands->Append();
            }
        }
        Expression = ref new Platform::String(item.historyItemVector.expression.c_str());
        Result = ref new Platform::String(item.historyItemVector.result.c_str());
    }

    CalcManagerSnapshot::CalcManagerSnapshot(const CalculationManager::CalculatorManager& calcMgr)
    {
        auto& items = calcMgr.GetHistoryItems();
        if (!items.empty())
        {
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

    ExpressionDisplaySnapshot::ExpressionDisplaySnapshot(const std::vector<CalcHistoryToken>& tokens)
    {
        Tokens = ref new Platform::Collections::Vector<CalcManagerHistoryToken ^>();
        for (auto& [opCode, cmdIdx] : tokens)
        {
            Tokens->Append(ref new CalcManagerHistoryToken(ref new Platform::String(opCode.c_str()), cmdIdx));
        }
    }
}
