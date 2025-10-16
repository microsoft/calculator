// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
/****************************Module*Header***********************************\
* Module Name: CalcEngine.h
*
* Module Description:
*       The class definition for the Calculator's engine class CCalcEngine
*
* Warnings:
*
* Created: 17-Jan-2008
*
\****************************************************************************/

#include <random>
#include "CCommand.h"
#include "EngineStrings.h"
#include "../Command.h"
#include "../ExpressionCommand.h"
#include "RadixType.h"
#include "History.h" // for History Collector
#include "CalcInput.h"
#include "CalcUtils.h"
#include "ICalcDisplay.h"
#include "Rational.h"
#include "RationalMath.h"

// The following are NOT real exports of CalcEngine, but for forward declarations
// The real exports follows later

// This is expected to be in same order as IDM_QWORD, IDM_DWORD etc.
enum class NUM_WIDTH
{
    QWORD_WIDTH, // Number width of 64 bits mode (default)
    DWORD_WIDTH, // Number width of 32 bits mode
    WORD_WIDTH,  // Number width of 16 bits mode
    BYTE_WIDTH   // Number width of 16 bits mode
};
static constexpr size_t NUM_WIDTH_LENGTH = 4;

namespace CalculationManager
{
    class IResourceProvider;
}

namespace CalculatorEngineTests
{
    class CalcEngineTests;
}

class CCalcEngine
{
public:
    CCalcEngine(
        bool fPrecedence,
        bool fIntegerMode,
        CalculationManager::IResourceProvider* const pResourceProvider,
        __in_opt ICalcDisplay* pCalcDisplay,
        __in_opt std::shared_ptr<IHistoryDisplay> pHistoryDisplay);
    void ProcessCommand(OpCode wID);
    void DisplayError(uint32_t nError);
    std::unique_ptr<CalcEngine::Rational> PersistedMemObject();
    void PersistedMemObject(CalcEngine::Rational const& memObject);
    bool FInErrorState()
    {
        return m_bError;
    }
    bool IsInputEmpty()
    {
        return m_input.IsEmpty() && (m_numberString.empty() || m_numberString == L"0");
    }
    bool FInRecordingState()
    {
        return m_bRecord;
    }
    void SettingsChanged();
    bool IsCurrentTooBigForTrig();
    uint32_t GetCurrentRadix();
    std::wstring GetCurrentResultForRadix(uint32_t radix, int32_t precision, bool groupDigitsPerRadix);
    void ChangePrecision(int32_t precision)
    {
        m_precision = precision;
        ChangeConstants(m_radix, precision);
    }
    std::wstring GroupDigitsPerRadix(std::wstring_view numberString, uint32_t radix);
    std::wstring GetStringForDisplay(CalcEngine::Rational const& rat, uint32_t radix);
    void UpdateMaxIntDigits();
    wchar_t DecimalSeparator() const;

    std::vector<std::shared_ptr<IExpressionCommand>> GetHistoryCollectorCommandsSnapshot() const;

    // Static methods for the instance
    static void
    InitialOneTimeOnlySetup(CalculationManager::IResourceProvider& resourceProvider); // Once per load time to call to initialize all shared global variables
    // returns the ptr to string representing the operator. Mostly same as the button, but few special cases for x^y etc.
    static std::wstring_view GetString(int ids)
    {
        return s_engineStrings[std::to_wstring(ids)];
    }
    static std::wstring_view GetString(std::wstring_view ids)
    {
        return s_engineStrings[ids];
    }
    static std::wstring_view OpCodeToString(int nOpCode)
    {
        return GetString(IdStrFromCmdId(nOpCode));
    }
    static std::wstring_view OpCodeToUnaryString(int nOpCode, bool fInv, AngleType angletype);
    static std::wstring_view OpCodeToBinaryString(int nOpCode, bool isIntegerMode);

private:
    bool m_fPrecedence;
    bool m_fIntegerMode; /* This is true if engine is explicitly called to be in integer mode. All bases are restricted to be in integers only */
    ICalcDisplay* m_pCalcDisplay;
    CalculationManager::IResourceProvider* const m_resourceProvider;
    int m_nOpCode;     /* ID value of operation.                       */
    int m_nPrevOpCode; // opcode which computed the number in m_currentVal. 0 if it is already bracketed or plain number or
    // if it hasn't yet been computed
    bool m_bChangeOp;              // Flag for changing operation
    bool m_bRecord;                // Global mode: recording or displaying
    bool m_bSetCalcState;          // Flag for setting the engine result state
    CalcEngine::CalcInput m_input; // Global calc input object for decimal strings
    NumberFormat m_nFE;            // Scientific notation conversion flag
    CalcEngine::Rational m_maxTrigonometricNum;
    std::unique_ptr<CalcEngine::Rational> m_memoryValue; // Current memory value.

    CalcEngine::Rational m_holdVal; // For holding the second operand in repetitive calculations ( pressing "=" continuously)

    CalcEngine::Rational m_currentVal;                               // Currently displayed number used everywhere.
    CalcEngine::Rational m_lastVal;                                  // Number before operation (left operand).
    std::array<CalcEngine::Rational, MAXPRECDEPTH> m_parenVals;      // Holding array for parenthesis values.
    std::array<CalcEngine::Rational, MAXPRECDEPTH> m_precedenceVals; // Holding array for precedence values.
    bool m_bError;                                                   // Error flag.
    bool m_bInv;                                                     // Inverse on/off flag.
    bool m_bNoPrevEqu;                                               /* Flag for previous equals.          */
    bool m_bFlagSign;                                                /* Flag for +/- on next op */

    uint32_t m_radix;
    int32_t m_precision;
    int m_cIntDigitsSav;
    std::vector<uint32_t> m_decGrouping; // Holds the decimal digit grouping number

    std::wstring m_numberString;

    int m_nTempCom;                          /* Holding place for the last command.          */
    size_t m_openParenCount;                 // Number of open parentheses.
    std::array<int, MAXPRECDEPTH> m_nOp;     /* Holding array for parenthesis operations.    */
    std::array<int, MAXPRECDEPTH> m_nPrecOp; /* Holding array for precedence  operations.    */
    size_t m_precedenceOpCount;              /* Current number of precedence ops in holding. */
    int m_nLastCom;                          // Last command entered.
    AngleType m_angletype;                   // Current Angle type when in dec mode. one of deg, rad or grad
    NUM_WIDTH m_numwidth;                    // one of qword, dword, word or byte mode.
    int32_t m_dwWordBitWidth;                // # of bits in currently selected word size

    std::unique_ptr<std::mt19937> m_randomGeneratorEngine;
    std::unique_ptr<std::uniform_real_distribution<>> m_distr;

    uint64_t m_carryBit;

    CHistoryCollector m_HistoryCollector; // Accumulator of each line of history as various commands are processed

    std::array<CalcEngine::Rational, NUM_WIDTH_LENGTH> m_chopNumbers;           // word size enforcement
    std::array<std::wstring, NUM_WIDTH_LENGTH> m_maxDecimalValueStrings;        // maximum values represented by a given word width based off m_chopNumbers
    static std::unordered_map<std::wstring_view, std::wstring> s_engineStrings; // the string table shared across all instances
    wchar_t m_decimalSeparator;
    wchar_t m_groupSeparator;

private:
    void ProcessCommandWorker(OpCode wParam);
    void ResolveHighestPrecedenceOperation();
    void HandleErrorCommand(OpCode idc);
    void HandleMaxDigitsReached();
    void DisplayNum(void);
    int IsNumberInvalid(const std::wstring& numberString, int iMaxExp, int iMaxMantissa, uint32_t radix) const;
    void DisplayAnnounceBinaryOperator();
    void SetPrimaryDisplay(const std::wstring& szText, bool isError = false);
    void ClearTemporaryValues();
    void ClearDisplay();
    CalcEngine::Rational TruncateNumForIntMath(CalcEngine::Rational const& rat);
    CalcEngine::Rational SciCalcFunctions(CalcEngine::Rational const& rat, uint32_t op);
    CalcEngine::Rational DoOperation(int operation, CalcEngine::Rational const& lhs, CalcEngine::Rational const& rhs);
    void SetRadixTypeAndNumWidth(RadixType radixtype, NUM_WIDTH numwidth);
    int32_t DwWordBitWidthFromNumWidth(NUM_WIDTH numwidth);
    uint32_t NRadixFromRadixType(RadixType radixtype);
    double GenerateRandomNumber();

    bool TryToggleBit(CalcEngine::Rational& rat, uint32_t wbitno);
    void CheckAndAddLastBinOpToHistory(bool addToHistory = true);

    void InitChopNumbers();
    CalcEngine::Rational GetChopNumber() const;
    std::wstring GetMaxDecimalValueString() const;

    static void LoadEngineStrings(CalculationManager::IResourceProvider& resourceProvider);
    static int IdStrFromCmdId(int id)
    {
        return id - IDC_FIRSTCONTROL + IDS_ENGINESTR_FIRST;
    }

    static std::vector<uint32_t> DigitGroupingStringToGroupingVector(std::wstring_view groupingString);
    std::wstring GroupDigits(std::wstring_view delimiter, std::vector<uint32_t> const& grouping, std::wstring_view displayString, bool isNumNegative = false);

    static int QuickLog2(int iNum);
    static void ChangeBaseConstants(uint32_t radix, int maxIntDigits, int32_t precision);
    void BaseOrPrecisionChanged();

    friend class CalculatorEngineTests::CalcEngineTests;
};
