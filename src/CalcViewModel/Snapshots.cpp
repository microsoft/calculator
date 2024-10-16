// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <cassert>

#include "Snapshots.h"

namespace CalculatorApp::ViewModel
{
    CalcEngineHistoryToken::CalcEngineHistoryToken(Platform::String ^ opCodeName, int cmdIndex)
    {
        assert(opCodeName != nullptr && "opCodeName is mandatory.");
        OpCodeName = opCodeName;
        CommandIndex = cmdIndex;
    }

    CalcEnginHistoryItem::CalcEnginHistoryItem(const CalculationManager::HISTORYITEM& item)
    {
        Tokens = ref new Platform::Collections::Vector<CalcEngineHistoryToken ^>();
        if (item.historyItemVector.spTokens != nullptr)
        {
            for (auto& [opCode, cmdIdx] : *item.historyItemVector.spTokens)
            {
                Tokens->Append(ref new CalcEngineHistoryToken(ref new Platform::String(opCode.c_str()), cmdIdx));
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
                HistoryItems->Append(ref new CalcEnginHistoryItem(*item));
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
        Tokens = ref new Platform::Collections::Vector<CalcEngineHistoryToken ^>();
        for (auto& [opCode, cmdIdx] : tokens)
        {
            Tokens->Append(ref new CalcEngineHistoryToken(ref new Platform::String(opCode.c_str()), cmdIdx));
        }
    }
}
