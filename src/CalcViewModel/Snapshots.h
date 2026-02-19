// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <vector>
#include <memory>

#include "CalcManager/CalculatorManager.h"

namespace CalculatorApp::ViewModel::Snapshot
{
public
    interface struct ICalcManagerIExprCommand
    {
    };

public
    ref struct UnaryCommand sealed : public ICalcManagerIExprCommand
    {
        property Windows::Foundation::Collections::IVectorView<int> ^ Commands {
            Windows::Foundation::Collections::IVectorView<int> ^ get();
            void set(Windows::Foundation::Collections::IVectorView<int> ^ commands);
        };

        UnaryCommand();

        internal :;
        explicit UnaryCommand(std::vector<int> cmds);
        std::vector<int> m_cmds;
    };

public
    ref struct BinaryCommand sealed : public ICalcManagerIExprCommand
    {
        property int Command;

        BinaryCommand();

        internal :;
        explicit BinaryCommand(int cmd);
    };

public
    ref struct OperandCommand sealed : public ICalcManagerIExprCommand
    {
        property bool IsNegative;
        property bool IsDecimalPresent;
        property bool IsSciFmt;
        property Windows::Foundation::Collections::IVectorView<int> ^ Commands {
            Windows::Foundation::Collections::IVectorView<int> ^ get();
            void set(Windows::Foundation::Collections::IVectorView<int> ^ commands);
        };

        OperandCommand();

        internal :;
        explicit OperandCommand(bool isNegative, bool isDecimal, bool isSciFmt, std::vector<int> cmds);
        std::vector<int> m_cmds;
    };

public
    ref struct Parentheses sealed : public ICalcManagerIExprCommand
    {
        property int Command;

        Parentheses();

        internal :;
        explicit Parentheses(int cmd);
    };

public
    ref struct CalcManagerToken sealed
    {
        property Platform::String ^ OpCodeName; // mandatory
        property int CommandIndex;

        CalcManagerToken();

        internal :;
        explicit CalcManagerToken(Platform::String ^ opCodeName, int cmdIndex);
    };

public
    ref struct CalcManagerHistoryItem sealed
    {
        property Windows::Foundation::Collections::IVector<CalcManagerToken ^> ^ Tokens;           // mandatory
        property Windows::Foundation::Collections::IVector<ICalcManagerIExprCommand ^> ^ Commands; // mandatory
        property Platform::String ^ Expression;                                                    // mandatory
        property Platform::String ^ Result;                                                        // mandatory

        CalcManagerHistoryItem();

        internal :;
        explicit CalcManagerHistoryItem(const CalculationManager::HISTORYITEM& item);
    };

public
    ref struct CalcManagerSnapshot sealed
    {
        property Windows::Foundation::Collections::IVector<CalcManagerHistoryItem ^> ^ HistoryItems; // optional

        CalcManagerSnapshot();

        internal :;
        explicit CalcManagerSnapshot(const CalculationManager::CalculatorManager& calcMgr);
    };

public
    ref struct PrimaryDisplaySnapshot sealed
    {
        property Platform::String ^ DisplayValue; // mandatory
        property bool IsError;

        PrimaryDisplaySnapshot();

        internal :;
        explicit PrimaryDisplaySnapshot(Platform::String ^ display, bool isError);
    };

public
    ref struct ExpressionDisplaySnapshot sealed
    {
        property Windows::Foundation::Collections::IVector<CalcManagerToken ^> ^ Tokens;
        property Windows::Foundation::Collections::IVector<ICalcManagerIExprCommand ^> ^ Commands;

        ExpressionDisplaySnapshot();

        internal :;
        using CalcHistoryToken = std::pair<std::wstring, int>;
        explicit ExpressionDisplaySnapshot(const std::vector<CalcHistoryToken>& tokens, const std::vector<std::shared_ptr<IExpressionCommand>>& commands);
    };

public
    ref struct StandardCalculatorSnapshot sealed
    {
        property CalcManagerSnapshot ^ CalcManager;                                                       // mandatory
        property PrimaryDisplaySnapshot ^ PrimaryDisplay;                                                 // mandatory
        property ExpressionDisplaySnapshot ^ ExpressionDisplay;                                           // optional
        property Windows::Foundation::Collections::IVector<ICalcManagerIExprCommand ^> ^ DisplayCommands; // mandatory

        StandardCalculatorSnapshot();
    };

public
    ref struct ApplicationSnapshot sealed
    {
        property int Mode;
        property StandardCalculatorSnapshot ^ StandardCalculator; // optional
    };

    ICalcManagerIExprCommand ^ CreateExprCommand(const IExpressionCommand* exprCmd);
    std::vector<std::shared_ptr<IExpressionCommand>> ToUnderlying(Windows::Foundation::Collections::IVector<ICalcManagerIExprCommand ^> ^ commands);
    std::vector<std::shared_ptr<CalculationManager::HISTORYITEM>> ToUnderlying(Windows::Foundation::Collections::IVector<CalcManagerHistoryItem ^> ^ items);

} // namespace CalculatorApp::ViewModel
