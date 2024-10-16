// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "CalcManager/CalculatorManager.h"

namespace CalculatorApp::ViewModel
{
public
    ref struct CalcEngineHistoryToken sealed
    {
        property Platform::String ^ OpCodeName; // mandatory
        property int CommandIndex;

        internal :;
        explicit CalcEngineHistoryToken(Platform::String ^ opCodeName, int cmdIndex);
    };

public
    ref struct CalcEnginHistoryItem sealed
    {
        property Windows::Foundation::Collections::IVector<CalcEngineHistoryToken ^> ^ Tokens; // mandatory
        // TODO: commands
        property Platform::String ^ Expression; // mandatory
        property Platform::String ^ Result;     // mandatory

        internal :;
        explicit CalcEnginHistoryItem(const CalculationManager::HISTORYITEM& item);
    };

public
    ref struct CalcManagerSnapshot sealed
    {
        property Windows::Foundation::Collections::IVector<CalcEnginHistoryItem ^> ^ HistoryItems; // optional

        internal :;
        explicit CalcManagerSnapshot(const CalculationManager::CalculatorManager& calcMgr);
    };

public
    ref struct PrimaryDisplaySnapshot sealed
    {
        property Platform::String ^ DisplayValue; // mandatory
        property bool IsError;

        internal :;
        explicit PrimaryDisplaySnapshot(Platform::String ^ display, bool isError);
    };

public
    ref struct ExpressionDisplaySnapshot sealed
    {
        property Windows::Foundation::Collections::IVector<CalcEngineHistoryToken ^> ^ Tokens;
        // TODO: commands

        internal :;
        using CalcHistoryToken = std::pair<std::wstring, int>;
        explicit ExpressionDisplaySnapshot(const std::vector<CalcHistoryToken>& tokens);
    };

public
    ref struct StandardCalculatorSnapshot sealed
    {
        property CalcManagerSnapshot ^ CalcManager;             // mandatory
        property PrimaryDisplaySnapshot ^ PrimaryDisplay;       // mandatory
        property ExpressionDisplaySnapshot ^ ExpressionDisplay; // optional
        // TODO: commands
    };
}
