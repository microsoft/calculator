// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <array>
#include "ExpressionCommand.h"
#include "ICalcDisplay.h"
#include "IHistoryDisplay.h"
#include "Rational.h"

class COpndCommand;

// maximum depth you can get by precedence. It is just an array's size limit.
static constexpr size_t MAXPRECDEPTH = 25;

// Helper class really a internal class to CCalcEngine, to accumulate each history line of text by collecting the
// operands, operator, unary operator etc. Since it is a separate entity, it can be unit tested on its own but does
// rely on CCalcEngine calling it in appropriate order.
class CHistoryCollector
{
public:
    CHistoryCollector(ICalcDisplay* pCalcDisplay, std::shared_ptr<IHistoryDisplay> pHistoryDisplay, wchar_t decimalSymbol); // Can throw errors
    ~CHistoryCollector();
    void AddOpndToHistory(std::wstring_view numStr, CalcEngine::Rational const& rat, bool fRepetition = false);
    void RemoveLastOpndFromHistory();
    void AddBinOpToHistory(int nOpCode, bool isIntegerMode, bool fNoRepetition = true);
    void ChangeLastBinOp(int nOpCode, bool fPrecInvToHigher, bool isIntegerMode);
    void AddUnaryOpToHistory(int nOpCode, bool fInv, AngleType angletype);
    void AddOpenBraceToHistory();
    void AddCloseBraceToHistory();
    void PushLastOpndStart(int ichOpndStart = -1);
    void PopLastOpndStart();
    void EnclosePrecInversionBrackets();
    bool FOpndAddedToHistory() const;
    void CompleteHistoryLine(std::wstring_view numStr);
    void CompleteEquation(std::wstring_view numStr);
    void ClearHistoryLine(std::wstring_view errStr);
    int AddCommand(_In_ const std::shared_ptr<IExpressionCommand>& spCommand);
    void UpdateHistoryExpression(uint32_t radix, int32_t precision);
    void SetDecimalSymbol(wchar_t decimalSymbol);
    std::shared_ptr<COpndCommand> GetOperandCommandsFromString(std::wstring_view numStr, CalcEngine::Rational const& rat) const;
    std::vector<std::shared_ptr<IExpressionCommand>> GetCommands() const;

private:
    std::shared_ptr<IHistoryDisplay> m_pHistoryDisplay;
    ICalcDisplay* m_pCalcDisplay;

    int m_iCurLineHistStart; // index of the beginning of the current equation
    // a sort of state, set to the index before 2 after 2 in the expression 2 + 3 say. Useful for auto correct portion of history and for
    // attaching the unary op around the last operand
    int m_lastOpStartIndex;    // index of the beginning of the last operand added to the history
    int m_lastBinOpStartIndex; // index of the beginning of the last binary operator added to the history
    std::array<int, MAXPRECDEPTH>
        m_operandIndices;  // Stack of index of opnd's beginning for each '('. A parallel array to m_hnoParNum, but abstracted independently of that
    int m_curOperandIndex; // Stack index for the above stack
    bool m_bLastOpndBrace; // iff the last opnd in history is already braced so we can avoid putting another one for unary operator
    wchar_t m_decimalSymbol;
    std::shared_ptr<std::vector<std::pair<std::wstring, int>>> m_spTokens;
    std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> m_spCommands;

private:
    void ReinitHistory();
    int IchAddSzToEquationSz(std::wstring_view str, int icommandIndex);
    void TruncateEquationSzFromIch(int ich);
    void SetExpressionDisplay();
    void InsertSzInEquationSz(std::wstring_view str, int icommandIndex, int ich);
    std::shared_ptr<std::vector<int>> GetOperandCommandsFromString(std::wstring_view numStr) const;
};
