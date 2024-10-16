// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "CalcManager/CalculatorManager.h"

namespace CalculatorApp::ViewModel
{
public
    interface struct ICalcManagerIExprCommand
    {
    };

public
    ref struct CalcManagerHistoryToken sealed
    {
        property Platform::String ^ OpCodeName; // mandatory
        property int CommandIndex;

        internal :;
        explicit CalcManagerHistoryToken(Platform::String ^ opCodeName, int cmdIndex);
    };

public
    ref struct CalcManagerHistoryItem sealed
    {
        property Windows::Foundation::Collections::IVector<CalcManagerHistoryToken ^> ^ Tokens;    // mandatory
        property Windows::Foundation::Collections::IVector<ICalcManagerIExprCommand ^> ^ Commands; // mandatory
        property Platform::String ^ Expression;                                                    // mandatory
        property Platform::String ^ Result;                                                        // mandatory

        internal :;
        explicit CalcManagerHistoryItem(const CalculationManager::HISTORYITEM& item);
    };

public
    ref struct CalcManagerSnapshot sealed
    {
        property Windows::Foundation::Collections::IVector<CalcManagerHistoryItem ^> ^ HistoryItems; // optional

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
        property Windows::Foundation::Collections::IVector<CalcManagerHistoryToken ^> ^ Tokens;
        // TODO: commands

        internal :;
        using CalcHistoryToken = std::pair<std::wstring, int>;
        explicit ExpressionDisplaySnapshot(const std::vector<CalcHistoryToken>& tokens);
    };

public
    ref struct StandardCalculatorSnapshot sealed
    {
        property CalcManagerSnapshot ^ CalcManager;                                                // mandatory
        property PrimaryDisplaySnapshot ^ PrimaryDisplay;                                          // mandatory
        property ExpressionDisplaySnapshot ^ ExpressionDisplay;                                    // optional
        property Windows::Foundation::Collections::IVector<ICalcManagerIExprCommand ^> ^ Commands; // mandatory
    };
} // namespace CalculatorApp::ViewModel
