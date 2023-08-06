// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/****************************Module*Header***********************************\
* Module Name: SCICOMM.C
*
* Module Description:
*
* Warnings:
*
* Created:
*
* Author:
\****************************************************************************/

#include <iomanip>
#include <string>
#include <sstream>
#include "Header Files/CalcEngine.h"
#include "Header Files/CalcUtils.h"

using namespace std;
using namespace CalcEngine;

namespace
{
    // NPrecedenceOfOp
    //
    // returns a virtual number for precedence for the operator. We expect binary operator only, otherwise the lowest number
    // 0 is returned. Higher the number, higher the precedence of the operator.
    int NPrecedenceOfOp(int nopCode)
    {
        switch (nopCode)
        {
        default:
        case IDC_OR:
        case IDC_XOR:
            return 0;
        case IDC_AND:
        case IDC_NAND:
        case IDC_NOR:
            return 1;
        case IDC_ADD:
        case IDC_SUB:
            return 2;
        case IDC_LSHF:
        case IDC_RSHF:
        case IDC_RSHFL:
        case IDC_MOD:
        case IDC_DIV:
        case IDC_MUL:
            return 3;
        case IDC_PWR:
        case IDC_ROOT:
        case IDC_LOGBASEY:
            return 4;
        }
    }
}

// HandleErrorCommand
//
// When it is discovered by the state machine that at this point the input is not valid (eg. "1+)"), we want to proceed as though this input never
// occurred and may be some feedback to user like Beep. The rest of input can then continue by just ignoring this command.
void CCalcEngine::HandleErrorCommand(OpCode idc)
{
    if (!IsGuiSettingOpCode(idc))
    {
        // We would have saved the prev command. Need to forget this state
        m_nTempCom = m_nLastCom;
    }
}

void CCalcEngine::HandleMaxDigitsReached()
{
    if (nullptr != m_pCalcDisplay)
    {
        m_pCalcDisplay->MaxDigitsReached();
    }
}

void CCalcEngine::ClearTemporaryValues()
{
    m_bInv = false;
    m_input.Clear();
    m_bRecord = true;
    CheckAndAddLastBinOpToHistory();
    DisplayNum();
    m_bError = false;
}

void CCalcEngine::ClearDisplay()
{
    if (nullptr != m_pCalcDisplay)
    {
        m_pCalcDisplay->SetExpressionDisplay(make_shared<vector<pair<wstring, int>>>(), make_shared<vector<shared_ptr<IExpressionCommand>>>());
    }
}

void CCalcEngine::ProcessCommand(OpCode wParam)
{
    if (wParam == IDC_SET_RESULT)
    {
        wParam = IDC_RECALL;
        m_bSetCalcState = true;
    }

    ProcessCommandWorker(wParam);
}

void CCalcEngine::ProcessCommandWorker(OpCode wParam)
{
    // Save the last command.  Some commands are not saved in this manor, these
    // commands are:
    // Inv, Deg, Rad, Grad, Stat, FE, MClear, and Back.  The excluded
    // commands are not
    // really mathematical operations, rather they are GUI mode settings.
    //
    // UDATE:  We now save the last command for the Exp command as well.
    if (!IsGuiSettingOpCode(wParam))
    {
        m_nLastCom = m_nTempCom;
        m_nTempCom = (int)wParam;
    }

    // Clear expression shown after = sign, when user do any action.
    if (!m_bNoPrevEqu)
    {
        ClearDisplay();
    }

    if (m_bError)
    {
        if (wParam == IDC_CLEAR)
        {
            // handle "C" normally
        }
        else if (wParam == IDC_CENTR)
        {
            // treat "CE" as "C"
            wParam = IDC_CLEAR;
        }
        else
        {
            HandleErrorCommand(wParam);
            return;
        }
    }

    if (wParam == IDC_SUB && m_nLastCom == IDC_EXP)
    {
        wParam = IDC_SIGN;
    }

    // Toggle Record/Display mode if appropriate.
    if (m_bRecord)
    {
        if (IsBinOpCode(wParam) || IsUnaryOpCode(wParam) || IsOpInRange(wParam, IDC_FE, IDC_MMINUS) || IsOpInRange(wParam, IDC_OPENP, IDC_CLOSEP)
            || IsOpInRange(wParam, IDM_HEX, IDM_BIN) || IsOpInRange(wParam, IDM_QWORD, IDM_BYTE) || IsOpInRange(wParam, IDM_DEG, IDM_GRAD)
            || IsOpInRange(wParam, IDC_BINEDITSTART, IDC_BINEDITEND) || (IDC_INV == wParam) || (IDC_SIGN == wParam && 10 != m_radix) || (IDC_RAND == wParam)
            || (IDC_EULER == wParam))
        {
            m_bRecord = false;
            m_currentVal = m_input.ToRational(m_radix, m_precision);
            DisplayNum(); // Causes 3.000 to shrink to 3. on first op.
        }
    }
    else if (IsDigitOpCode(wParam) || wParam == IDC_PNT)
    {
        m_bRecord = true;
        m_input.Clear();
        CheckAndAddLastBinOpToHistory();
    }

    // Interpret digit keys.
    if (IsDigitOpCode(wParam))
    {
        unsigned int iValue = static_cast<unsigned int>(wParam - IDC_0);

        // this is redundant, illegal keys are disabled
        if (iValue >= static_cast<unsigned int>(m_radix))
        {
            HandleErrorCommand(wParam);
            return;
        }

        if (!m_input.TryAddDigit(iValue, m_radix, m_fIntegerMode, GetMaxDecimalValueString(), m_dwWordBitWidth, m_cIntDigitsSav))
        {
            HandleErrorCommand(wParam);
            HandleMaxDigitsReached();
            return;
        }

        DisplayNum();

        /*
            Addressing issues:
                #1541 https://github.com/microsoft/calculator/issues/1541
                #1311 https://github.com/microsoft/calculator/issues/1311
            Solution:
                Consider the previous '-' Binary Op as a '+/-' sign value if
                the first opnd is 0.
        */
        if (m_bFlagSign)
        {
            wParam = IDC_SIGN;
            m_bFlagSign = false;
        }
        else
        {
            return;
        }
    }

    // BINARY OPERATORS:
    if (IsBinOpCode(wParam)) 
    {
        // Change the operation if last input was operation.
        if (IsBinOpCode(m_nLastCom))
        {
            bool fPrecInvToHigher = false; // Is Precedence Inversion from lower to higher precedence happening ??

            m_nOpCode = (int)wParam;

            // Check to see if by changing this binop, a Precedence inversion is happening.
            // Eg. 1 * 2  + and + is getting changed to ^. The previous precedence rules would have already computed
            // 1*2, so we will put additional brackets to cover for precedence inversion and it will become (1 * 2) ^
            // Here * is m_nPrevOpCode, m_currentVal is 2  (by 1*2), m_nLastCom is +, m_nOpCode is ^
            if (m_fPrecedence && 0 != m_nPrevOpCode)
            {
                int nPrev = NPrecedenceOfOp(m_nPrevOpCode);
                int nx = NPrecedenceOfOp(m_nLastCom);
                int ni = NPrecedenceOfOp(m_nOpCode);
                if (nx <= nPrev && ni > nPrev) // condition for Precedence Inversion
                {
                    fPrecInvToHigher = true;
                    m_nPrevOpCode = 0; // Once the precedence inversion has put additional brackets, its no longer required
                }
            }
            m_HistoryCollector.ChangeLastBinOp(m_nOpCode, fPrecInvToHigher, m_fIntegerMode);
            DisplayAnnounceBinaryOperator();
            return;
        }

        if (!m_HistoryCollector.FOpndAddedToHistory())
        {
            // if the prev command was ) or unop then it is already in history as a opnd form (...)
            m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
        }

        /* m_bChangeOp is true if there was an operation done and the   */
        /* current m_currentVal is the result of that operation.  This is so */
        /* entering 3+4+5= gives 7 after the first + and 12 after the */
        /* the =.  The rest of this stuff attempts to do precedence in*/
        /* Scientific mode.                                           */
        if (m_bChangeOp)
        {
        DoPrecedenceCheckAgain:

            int nx = NPrecedenceOfOp((int)wParam);
            int ni = NPrecedenceOfOp(m_nOpCode);

            if ((nx > ni) && m_fPrecedence)
            {
                if (m_precedenceOpCount < MAXPRECDEPTH)
                {
                    m_precedenceVals[m_precedenceOpCount] = m_lastVal;

                    m_nPrecOp[m_precedenceOpCount] = m_nOpCode;
                    m_HistoryCollector.PushLastOpndStart(); // Eg. 1 + 2  *, Need to remember the start of 2 to do Precedence inversion if need to
                }
                else
                {
                    m_precedenceOpCount = MAXPRECDEPTH - 1;
                    HandleErrorCommand(wParam);
                }
                m_precedenceOpCount++;
            }
            else
            {
                /* do the last operation and then if the precedence array is not
                 * empty or the top is not the '(' demarcator then pop the top
                 * of the array and recheck precedence against the new operator
                 */
                m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
                m_nPrevOpCode = m_nOpCode;

                if (!m_bError)
                {
                    DisplayNum();
                    if (!m_fPrecedence)
                    {
                        wstring groupedString = GroupDigitsPerRadix(m_numberString, m_radix);
                        m_HistoryCollector.CompleteEquation(groupedString);
                        m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
                    }
                }

                if ((m_precedenceOpCount != 0) && (m_nPrecOp[m_precedenceOpCount - 1]))
                {
                    m_precedenceOpCount--;
                    m_nOpCode = m_nPrecOp[m_precedenceOpCount];

                    m_lastVal = m_precedenceVals[m_precedenceOpCount];

                    nx = NPrecedenceOfOp(m_nOpCode);
                    // Precedence Inversion Higher to lower can happen which needs explicit enclosure of brackets
                    // Eg.  1 + 2 * Or 3 Or.  We would have pushed 1+ before, and now last + forces 2 Or 3 to be evaluated
                    // because last Or is less or equal to first + (after 1). But we see that 1+ is in stack and we evaluated to 2 Or 3
                    // This is precedence inversion happened because of operator changed in between. We put extra brackets like
                    // 1 + (2 Or 3)
                    if (ni <= nx)
                    {
                        m_HistoryCollector.EnclosePrecInversionBrackets();
                    }
                    m_HistoryCollector.PopLastOpndStart();
                    goto DoPrecedenceCheckAgain;
                }
            }
        }

        DisplayAnnounceBinaryOperator();

        // consider this sub as +/- for the upcoming number
        if ( wParam == IDC_SUB && m_currentVal.IsZero())
        {
            m_bFlagSign = true;
        }

        m_lastVal = m_currentVal;
        m_nOpCode = (int)wParam;
        if (!m_bFlagSign)
        {
            m_HistoryCollector.AddBinOpToHistory(m_nOpCode, m_fIntegerMode);
        }
        m_bNoPrevEqu = m_bChangeOp = true;
        return;
    }

    // UNARY OPERATORS:
    if (IsUnaryOpCode(wParam) || (wParam == IDC_DEGREES))
    {
        /* Functions are unary operations.                            */
        /* If the last thing done was an operator, m_currentVal was cleared. */
        /* In that case we better use the number before the operator  */
        /* was entered, otherwise, things like 5+ 1/x give Divide By  */
        /* zero.  This way 5+=gives 10 like most calculators do.      */
        if (IsBinOpCode(m_nLastCom))
        {
            m_currentVal = m_lastVal;
        }

        // we do not add percent sign to history or to two line display.
        // instead, we add the result of applying %.
        if (wParam != IDC_PERCENT)
        {
            if (!m_HistoryCollector.FOpndAddedToHistory())
            {
                m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
            }

            m_HistoryCollector.AddUnaryOpToHistory((int)wParam, m_bInv, m_angletype);
        }

        if ((wParam == IDC_SIN) || (wParam == IDC_COS) || (wParam == IDC_TAN) || (wParam == IDC_SINH) || (wParam == IDC_COSH) || (wParam == IDC_TANH)
            || (wParam == IDC_SEC) || (wParam == IDC_CSC) || (wParam == IDC_COT) || (wParam == IDC_SECH) || (wParam == IDC_CSCH) || (wParam == IDC_COTH))
        {
            if (IsCurrentTooBigForTrig())
            {
                m_currentVal = 0;
                DisplayError(CALC_E_DOMAIN);
                return;
            }
        }

        m_currentVal = SciCalcFunctions(m_currentVal, (uint32_t)wParam);

        if (m_bError)
            return;

        /* Display the result, reset flags, and reset indicators.     */
        DisplayNum();

        if (wParam == IDC_PERCENT)
        {
            CheckAndAddLastBinOpToHistory();
            m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal, true /* Add to primary and secondary display */);
        }

        /* reset the m_bInv flag and indicators if it is set
        and have been used */

        if (m_bInv
            && ((wParam == IDC_CHOP) || (wParam == IDC_SIN) || (wParam == IDC_COS) || (wParam == IDC_TAN) || (wParam == IDC_LN) || (wParam == IDC_DMS)
                || (wParam == IDC_DEGREES) || (wParam == IDC_SINH) || (wParam == IDC_COSH) || (wParam == IDC_TANH) || (wParam == IDC_SEC) || (wParam == IDC_CSC)
                || (wParam == IDC_COT) || (wParam == IDC_SECH) || (wParam == IDC_CSCH) || (wParam == IDC_COTH)))
        {
            m_bInv = false;
        }

        return;
    }

    // Tiny binary edit windows clicked. Toggle that bit and update display
    if (IsOpInRange(wParam, IDC_BINEDITSTART, IDC_BINEDITEND))
    {
        // Same reasoning as for unary operators. We need to seed it previous number
        if (IsBinOpCode(m_nLastCom))
        {
            m_currentVal = m_lastVal;
        }

        CheckAndAddLastBinOpToHistory();

        if (TryToggleBit(m_currentVal, (uint32_t)wParam - IDC_BINEDITSTART))
        {
            DisplayNum();
        }

        return;
    }

    /* Now branch off to do other commands and functions.                 */
    switch (wParam)
    {
    case IDC_CLEAR: /* Total clear.                                       */
    {
        if (!m_bChangeOp)
        {
            // Preserve history, if everything done before was a series of unary operations.
            CheckAndAddLastBinOpToHistory(false);
        }

        m_lastVal = 0;

        m_bChangeOp = false;
        m_openParenCount = 0;
        m_precedenceOpCount = m_nTempCom = m_nLastCom = m_nOpCode = 0;
        m_nPrevOpCode = 0;
        m_bNoPrevEqu = true;
        m_carryBit = 0;

        /* clear the parenthesis status box indicator, this will not be
        cleared for CENTR */
        if (nullptr != m_pCalcDisplay)
        {
            m_pCalcDisplay->SetParenthesisNumber(0);
            ClearDisplay();
        }

        m_HistoryCollector.ClearHistoryLine(wstring());
        ClearTemporaryValues();
    }
    break;

    case IDC_CENTR: /* Clear only temporary values.                       */
    {
        // Clear the INV & leave (=xx indicator active
        ClearTemporaryValues();
    }

    break;

    case IDC_BACK:
        // Divide number by the current radix and truncate.
        // Only allow backspace if we're recording.
        if (m_bRecord)
        {
            m_input.Backspace();
            DisplayNum();
        }
        else
        {
            HandleErrorCommand(wParam);
        }
        break;

        /* EQU enables the user to press it multiple times after and      */
        /* operation to enable repeats of the last operation.             */
    case IDC_EQU:
        while (m_openParenCount > 0)
        {
            // when m_bError is set and m_ParNum is non-zero it goes into infinite loop
            if (m_bError)
            {
                break;
            }
            // automatic closing of all the parenthesis to get a meaningful result as well as ensure data integrity
            m_nTempCom = m_nLastCom; // Put back this last saved command to the prev state so ) can be handled properly
            ProcessCommand(IDC_CLOSEP);
            m_nLastCom = m_nTempCom;  // Actually this is IDC_CLOSEP
            m_nTempCom = (int)wParam; // put back in the state where last op seen was IDC_CLOSEP, and current op is IDC_EQU
        }

        if (!m_bNoPrevEqu)
        {
            // It is possible now unary op changed the num in screen, but still m_lastVal hasn't changed.
            m_lastVal = m_currentVal;
        }

        /* Last thing keyed in was an operator.  Lets do the op on*/
        /* a duplicate of the last entry.                     */
        if (IsBinOpCode(m_nLastCom))
        {
            m_currentVal = m_lastVal;
        }

        if (!m_HistoryCollector.FOpndAddedToHistory())
        {
            m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
        }

        // Evaluate the precedence stack.
        ResolveHighestPrecedenceOperation();
        while (m_fPrecedence && m_precedenceOpCount > 0)
        {
            m_precedenceOpCount--;
            m_nOpCode = m_nPrecOp[m_precedenceOpCount];
            m_lastVal = m_precedenceVals[m_precedenceOpCount];

            // Precedence Inversion check
            int ni = NPrecedenceOfOp(m_nPrevOpCode);
            int nx = NPrecedenceOfOp(m_nOpCode);
            if (ni <= nx)
            {
                m_HistoryCollector.EnclosePrecInversionBrackets();
            }
            m_HistoryCollector.PopLastOpndStart();

            m_bNoPrevEqu = true;

            ResolveHighestPrecedenceOperation();
        }

        if (!m_bError)
        {
            wstring groupedString = GroupDigitsPerRadix(m_numberString, m_radix);
            m_HistoryCollector.CompleteEquation(groupedString);
        }

        m_bChangeOp = false;
        m_nPrevOpCode = 0;

        break;

    case IDC_OPENP:
    case IDC_CLOSEP:

        // -IF- the Paren holding array is full and we try to add a paren
        // -OR- the paren holding array is empty and we try to remove a
        //      paren
        // -OR- the precedence holding array is full
        if ((m_openParenCount >= MAXPRECDEPTH && (wParam == IDC_OPENP)) || (!m_openParenCount && (wParam != IDC_OPENP))
            || ((m_precedenceOpCount >= MAXPRECDEPTH && m_nPrecOp[m_precedenceOpCount - 1] != 0)))
        {
            if (!m_openParenCount && (wParam != IDC_OPENP))
            {
                m_pCalcDisplay->OnNoRightParenAdded();
            }

            HandleErrorCommand(wParam);
            break;
        }

        if (wParam == IDC_OPENP)
        {
            // if there's an omitted multiplication sign
            if (IsDigitOpCode(m_nLastCom) || IsUnaryOpCode(m_nLastCom) || m_nLastCom == IDC_PNT || m_nLastCom == IDC_CLOSEP)
            {
                ProcessCommand(IDC_MUL);
            }

            CheckAndAddLastBinOpToHistory();
            m_HistoryCollector.AddOpenBraceToHistory();

            // Open level of parentheses, save number and operation.
            m_parenVals[m_openParenCount] = m_lastVal;

            m_nOp[m_openParenCount++] = (m_bChangeOp ? m_nOpCode : 0);

            /* save a special marker on the precedence array */
            if (m_precedenceOpCount < m_nPrecOp.size())
            {
                m_nPrecOp[m_precedenceOpCount++] = 0;
            }

            m_lastVal = 0;
            if (IsBinOpCode(m_nLastCom))
            {
                // We want 1 + ( to start as 1 + (0. Any number you type replaces 0. But if it is 1 + 3 (, it is
                // treated as 1 + (3
                m_currentVal = 0;
            }
            m_nTempCom = 0;
            m_nOpCode = 0;
            m_bChangeOp = false; // a ( is like starting a fresh sub equation
        }
        else
        {
            // Last thing keyed in was an operator. Lets do the op on a duplicate of the last entry.
            if (IsBinOpCode(m_nLastCom))
            {
                m_currentVal = m_lastVal;
            }

            if (!m_HistoryCollector.FOpndAddedToHistory())
            {
                m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
            }

            // Get the operation and number and return result.
            m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
            m_nPrevOpCode = m_nOpCode;

            // Now process the precedence stack till we get to an opcode which is zero.
            for (m_nOpCode = m_nPrecOp[--m_precedenceOpCount]; m_nOpCode; m_nOpCode = m_nPrecOp[--m_precedenceOpCount])
            {
                // Precedence Inversion check
                int ni = NPrecedenceOfOp(m_nPrevOpCode);
                int nx = NPrecedenceOfOp(m_nOpCode);
                if (ni <= nx)
                {
                    m_HistoryCollector.EnclosePrecInversionBrackets();
                }
                m_HistoryCollector.PopLastOpndStart();

                m_lastVal = m_precedenceVals[m_precedenceOpCount];

                m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
                m_nPrevOpCode = m_nOpCode;
            }

            m_HistoryCollector.AddCloseBraceToHistory();

            // Now get back the operation and opcode at the beginning of this parenthesis pair

            m_openParenCount -= 1;
            m_lastVal = m_parenVals[m_openParenCount];
            m_nOpCode = m_nOp[m_openParenCount];

            // m_bChangeOp should be true if m_nOpCode is valid
            m_bChangeOp = (m_nOpCode != 0);
        }

        // Set the "(=xx" indicator.
        if (nullptr != m_pCalcDisplay)
        {
            m_pCalcDisplay->SetParenthesisNumber(static_cast<unsigned int>(m_openParenCount));
        }

        if (!m_bError)
        {
            DisplayNum();
        }

        break;

        // BASE CHANGES:
    case IDM_HEX:
    case IDM_DEC:
    case IDM_OCT:
    case IDM_BIN:
    {
        SetRadixTypeAndNumWidth((RadixType)(wParam - IDM_HEX), (NUM_WIDTH)-1);
        m_HistoryCollector.UpdateHistoryExpression(m_radix, m_precision);
        break;
    }

    case IDM_QWORD:
    case IDM_DWORD:
    case IDM_WORD:
    case IDM_BYTE:
        if (m_bRecord)
        {
            m_currentVal = m_input.ToRational(m_radix, m_precision);
            m_bRecord = false;
        }

        // Compat. mode BaseX: Qword, Dword, Word, Byte
        SetRadixTypeAndNumWidth((RadixType)-1, (NUM_WIDTH)(wParam - IDM_QWORD));
        break;

    case IDM_DEG:
    case IDM_RAD:
    case IDM_GRAD:
        m_angletype = static_cast<AngleType>(wParam - IDM_DEG);
        break;

    case IDC_SIGN:
    {
        if (m_bRecord)
        {
            if (m_input.TryToggleSign(m_fIntegerMode, GetMaxDecimalValueString()))
            {
                DisplayNum();
            }
            else
            {
                HandleErrorCommand(wParam);
            }
            break;
        }

        // Doing +/- while in Record mode is not a unary operation
        if (IsBinOpCode(m_nLastCom))
        {
            m_currentVal = m_lastVal;
        }

        if (!m_HistoryCollector.FOpndAddedToHistory())
        {
            m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal);
        }

        m_currentVal = -(m_currentVal);

        DisplayNum();
        m_HistoryCollector.AddUnaryOpToHistory(IDC_SIGN, m_bInv, m_angletype);
    }
    break;

    case IDC_RECALL:

        if (m_bSetCalcState)
        {
            // Not a Memory recall. set the result
            m_bSetCalcState = false;
        }
        else
        {
            // Recall immediate memory value.
            m_currentVal = *m_memoryValue;
        }
        CheckAndAddLastBinOpToHistory();
        DisplayNum();
        break;

    case IDC_MPLUS:
    {
        /* MPLUS adds m_currentVal to immediate memory and kills the "mem"   */
        /* indicator if the result is zero.                           */
        Rational result = *m_memoryValue + m_currentVal;
        m_memoryValue = make_unique<Rational>(TruncateNumForIntMath(result)); // Memory should follow the current int mode

        break;
    }
    case IDC_MMINUS:
    {
        /* MMINUS subtracts m_currentVal to immediate memory and kills the "mem"   */
        /* indicator if the result is zero.                           */
        Rational result = *m_memoryValue - m_currentVal;
        m_memoryValue = make_unique<Rational>(TruncateNumForIntMath(result));

        break;
    }
    case IDC_STORE:
    case IDC_MCLEAR:
        m_memoryValue = make_unique<Rational>(wParam == IDC_STORE ? TruncateNumForIntMath(m_currentVal) : 0);
        break;
    case IDC_PI:
        if (!m_fIntegerMode)
        {
            CheckAndAddLastBinOpToHistory(); // pi is like entering the number
            m_currentVal = Rational{ (m_bInv ? two_pi : pi) };

            DisplayNum();
            m_bInv = false;
            break;
        }
        HandleErrorCommand(wParam);
        break;
    case IDC_RAND:
        if (!m_fIntegerMode)
        {
            CheckAndAddLastBinOpToHistory(); // rand is like entering the number

            wstringstream str;
            str << fixed << setprecision(m_precision) << GenerateRandomNumber();

            auto rat = StringToRat(false, str.str(), false, L"", m_radix, m_precision);
            if (rat != nullptr)
            {
                m_currentVal = Rational{ rat };
            }
            else
            {
                m_currentVal = Rational{ 0 };
            }
            destroyrat(rat);

            DisplayNum();
            m_bInv = false;
            break;
        }
        HandleErrorCommand(wParam);
        break;
    case IDC_EULER:
        if (!m_fIntegerMode)
        {
            CheckAndAddLastBinOpToHistory(); // e is like entering the number
            m_currentVal = Rational{ rat_exp };

            DisplayNum();
            m_bInv = false;
            break;
        }
        HandleErrorCommand(wParam);
        break;
    case IDC_FE:
        // Toggle exponential notation display.
        m_nFE = m_nFE == NumberFormat::Float ? NumberFormat::Scientific : NumberFormat::Float;
        DisplayNum();
        break;

    case IDC_EXP:
        if (m_bRecord && !m_fIntegerMode && m_input.TryBeginExponent())
        {
            DisplayNum();
            break;
        }
        HandleErrorCommand(wParam);
        break;

    case IDC_PNT:
        if (m_bRecord && !m_fIntegerMode && m_input.TryAddDecimalPt())
        {
            DisplayNum();
            break;
        }
        HandleErrorCommand(wParam);
        break;

    case IDC_INV:
        m_bInv = !m_bInv;
        break;
    }
}

// Helper function to resolve one item on the precedence stack.
void CCalcEngine::ResolveHighestPrecedenceOperation()
{
    // Is there a valid operation around?
    if (m_nOpCode)
    {
        // If this is the first EQU in a string, set m_holdVal=m_currentVal
        // Otherwise let m_currentVal=m_holdVal.  This keeps m_currentVal constant
        // through all EQUs in a row.
        if (m_bNoPrevEqu)
        {
            m_holdVal = m_currentVal;
        }
        else
        {
            m_currentVal = m_holdVal;
            DisplayNum(); // to update the m_numberString
            m_HistoryCollector.AddBinOpToHistory(m_nOpCode, m_fIntegerMode, false);
            m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal); // Adding the repeated last op to history
        }

        // Do the current or last operation.
        m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
        m_nPrevOpCode = m_nOpCode;
        m_lastVal = m_currentVal;

        // Check for errors.  If this wasn't done, DisplayNum
        // would immediately overwrite any error message.
        if (!m_bError)
        {
            DisplayNum();
        }

        // No longer the first EQU.
        m_bNoPrevEqu = false;
    }
    else if (!m_bError)
    {
        DisplayNum();
    }
}

// CheckAndAddLastBinOpToHistory
//
//  This is a very confusing helper routine to add the last entered binary operator to the history. This is expected to
// leave the history with <exp> <binop> state. It can really add the last entered binary op, or it can actually remove
// the last operand from history. This happens because you can 'type' or 'compute' over last operand in some cases, thereby
// effectively removing only it from the equation but still keeping the previous portion of the equation. Eg. 1 + 4 sqrt 5. The last
// 5 will remove sqrt(4) as it is not used anymore to participate in 1 + 5
// If you are messing with this, test cases like this CE, statistical functions, ( & MR buttons
void CCalcEngine::CheckAndAddLastBinOpToHistory(bool addToHistory)
{
    if (m_bChangeOp)
    {
        if (m_HistoryCollector.FOpndAddedToHistory())
        {
            // if last time opnd was added but the last command was not a binary operator, then it must have come
            // from commands which add the operand, like unary operator. So history at this is showing 1 + sqrt(4)
            // but in reality the sqrt(4) is getting replaced by new number (may be unary op, or MR or SUM etc.)
            // So erase the last operand
            m_HistoryCollector.RemoveLastOpndFromHistory();
        }
    }
    else if (m_HistoryCollector.FOpndAddedToHistory() && !m_bError)
    {
        // Corner case, where opnd is already in history but still a new opnd starting (1 + 4 sqrt 5). This is yet another
        // special casing of previous case under if (m_bChangeOp), but this time we can do better than just removing it
        // Let us make a current value =. So in case of 4 SQRT (or a equation under braces) and then a new equation is started, we can just form
        // a useful equation of sqrt(4) = 2 and continue a new equation from now on. But no point in doing this for things like
        // MR, SUM etc. All you will get is 5 = 5 kind of no useful equation.
        if ((IsUnaryOpCode(m_nLastCom) || IDC_SIGN == m_nLastCom || IDC_CLOSEP == m_nLastCom) && 0 == m_openParenCount)
        {
            if (addToHistory)
            {
                m_HistoryCollector.CompleteHistoryLine(GroupDigitsPerRadix(m_numberString, m_radix));
            }
        }
        else
        {
            m_HistoryCollector.RemoveLastOpndFromHistory();
        }
    }
}

// change the display area from a static text to an editbox, which has the focus can make
// Magnifier (Accessibility tool) work
void CCalcEngine::SetPrimaryDisplay(const wstring& szText, bool isError)
{
    if (m_pCalcDisplay != nullptr)
    {
        m_pCalcDisplay->SetPrimaryDisplay(szText, isError);
        m_pCalcDisplay->SetIsInError(isError);
    }
}

void CCalcEngine::DisplayAnnounceBinaryOperator()
{
    // If m_pCalcDisplay is null, this is not a high priority function
    // and should not be the reason we crash.
    if (m_pCalcDisplay != nullptr)
    {
        m_pCalcDisplay->BinaryOperatorReceived();
    }
}

// Unary operator Function Name table Element
// since unary operators button names aren't exactly friendly for history purpose,
// we have this separate table to get its localized name and for its Inv function if it exists.
struct FunctionNameElement
{
    wstring degreeString;        // Used by default if there are no rad or grad specific strings.
    wstring inverseDegreeString; // Will fall back to degreeString if empty

    wstring radString;
    wstring inverseRadString; // Will fall back to radString if empty

    wstring gradString;
    wstring inverseGradString; // Will fall back to gradString if empty

    wstring programmerModeString;

    bool hasAngleStrings = ((!radString.empty()) || (!inverseRadString.empty()) || (!gradString.empty()) || (!inverseGradString.empty()));
};

// Table for each unary operator
static const std::unordered_map<int, FunctionNameElement> operatorStringTable = {
    { IDC_CHOP, { L"", SIDS_FRAC } },

    { IDC_SIN, { SIDS_SIND, SIDS_ASIND, SIDS_SINR, SIDS_ASINR, SIDS_SING, SIDS_ASING } },
    { IDC_COS, { SIDS_COSD, SIDS_ACOSD, SIDS_COSR, SIDS_ACOSR, SIDS_COSG, SIDS_ACOSG } },
    { IDC_TAN, { SIDS_TAND, SIDS_ATAND, SIDS_TANR, SIDS_ATANR, SIDS_TANG, SIDS_ATANG } },

    { IDC_SINH, { L"", SIDS_ASINH } },
    { IDC_COSH, { L"", SIDS_ACOSH } },
    { IDC_TANH, { L"", SIDS_ATANH } },

    { IDC_SEC, { SIDS_SECD, SIDS_ASECD, SIDS_SECR, SIDS_ASECR, SIDS_SECG, SIDS_ASECG } },
    { IDC_CSC, { SIDS_CSCD, SIDS_ACSCD, SIDS_CSCR, SIDS_ACSCR, SIDS_CSCG, SIDS_ACSCG } },
    { IDC_COT, { SIDS_COTD, SIDS_ACOTD, SIDS_COTR, SIDS_ACOTR, SIDS_COTG, SIDS_ACOTG } },

    { IDC_SECH, { SIDS_SECH, SIDS_ASECH } },
    { IDC_CSCH, { SIDS_CSCH, SIDS_ACSCH } },
    { IDC_COTH, { SIDS_COTH, SIDS_ACOTH } },

    { IDC_LN, { L"", SIDS_POWE } },
    { IDC_SQR, { SIDS_SQR } },
    { IDC_CUB, { SIDS_CUBE } },
    { IDC_FAC, { SIDS_FACT } },
    { IDC_REC, { SIDS_RECIPROC } },
    { IDC_DMS, { L"", SIDS_DEGREES } },
    { IDC_SIGN, { SIDS_NEGATE } },
    { IDC_DEGREES, { SIDS_DEGREES } },
    { IDC_POW2, { SIDS_TWOPOWX } },
    { IDC_LOGBASEY, { SIDS_LOGBASEY } },
    { IDC_ABS, { SIDS_ABS } },
    { IDC_CEIL, { SIDS_CEIL } },
    { IDC_FLOOR, { SIDS_FLOOR } },
    { IDC_NAND, { SIDS_NAND } },
    { IDC_NOR, { SIDS_NOR } },
    { IDC_RSHFL, { SIDS_RSH } },
    { IDC_RORC, { SIDS_ROR } },
    { IDC_ROLC, { SIDS_ROL } },
    { IDC_CUBEROOT, { SIDS_CUBEROOT } },
    { IDC_MOD, { SIDS_MOD, L"", L"", L"", L"", L"", SIDS_PROGRAMMER_MOD } },
};

wstring_view CCalcEngine::OpCodeToUnaryString(int nOpCode, bool fInv, AngleType angletype)
{
    // Try to lookup the ID in the UFNE table
    wstring ids = L"";

    if (auto pair = operatorStringTable.find(nOpCode); pair != operatorStringTable.end())
    {
        const FunctionNameElement& element = pair->second;
        if (!element.hasAngleStrings || AngleType::Degrees == angletype)
        {
            if (fInv)
            {
                ids = element.inverseDegreeString;
            }

            if (ids.empty())
            {
                ids = element.degreeString;
            }
        }
        else if (AngleType::Radians == angletype)
        {
            if (fInv)
            {
                ids = element.inverseRadString;
            }
            if (ids.empty())
            {
                ids = element.radString;
            }
        }
        else if (AngleType::Gradians == angletype)
        {
            if (fInv)
            {
                ids = element.inverseGradString;
            }
            if (ids.empty())
            {
                ids = element.gradString;
            }
        }
    }

    if (!ids.empty())
    {
        return GetString(ids);
    }

    // If we didn't find an ID in the table, use the op code.
    return OpCodeToString(nOpCode);
}

wstring_view CCalcEngine::OpCodeToBinaryString(int nOpCode, bool isIntegerMode)
{
    // Try to lookup the ID in the UFNE table
    wstring ids = L"";

    if (auto pair = operatorStringTable.find(nOpCode); pair != operatorStringTable.end())
    {
        if (isIntegerMode && !pair->second.programmerModeString.empty())
        {
            ids = pair->second.programmerModeString;
        }
        else
        {
            ids = pair->second.degreeString;
        }
    }

    if (!ids.empty())
    {
        return GetString(ids);
    }

    // If we didn't find an ID in the table, use the op code.
    return OpCodeToString(nOpCode);
}

bool CCalcEngine::IsCurrentTooBigForTrig()
{
    return m_currentVal >= m_maxTrigonometricNum;
}

uint32_t CCalcEngine::GetCurrentRadix()
{
    return m_radix;
}

wstring CCalcEngine::GetCurrentResultForRadix(uint32_t radix, int32_t precision, bool groupDigitsPerRadix)
{
    Rational rat = (m_bRecord ? m_input.ToRational(m_radix, m_precision) : m_currentVal);

    ChangeConstants(m_radix, precision);

    wstring numberString = GetStringForDisplay(rat, radix);
    if (!numberString.empty())
    {
        // Revert the precision to previously stored precision
        ChangeConstants(m_radix, m_precision);
    }

    if (groupDigitsPerRadix)
    {
        return GroupDigitsPerRadix(numberString, radix);
    }
    else
    {
        return numberString;
    }
}

wstring CCalcEngine::GetStringForDisplay(Rational const& rat, uint32_t radix)
{
    wstring result{};
    // Check for standard\scientific mode
    if (!m_fIntegerMode)
    {
        result = rat.ToString(radix, m_nFE, m_precision);
    }
    else
    {
        // Programmer mode
        // Find most significant bit to determine if number is negative
        auto tempRat = TruncateNumForIntMath(rat);

        try
        {
            uint64_t w64Bits = tempRat.ToUInt64_t();
            bool fMsb = ((w64Bits >> (m_dwWordBitWidth - 1)) & 1);
            if ((radix == 10) && fMsb)
            {
                // If high bit is set, then get the decimal number in negative 2's complement form.
                tempRat = -((tempRat ^ GetChopNumber()) + 1);
            }

            result = tempRat.ToString(radix, m_nFE, m_precision);
        }
        catch (uint32_t)
        {
        }
    }

    return result;
}

double CCalcEngine::GenerateRandomNumber()
{
    if (m_randomGeneratorEngine == nullptr)
    {
        random_device rd;
        m_randomGeneratorEngine = std::make_unique<std::mt19937>(rd());
        m_distr = std::make_unique<std::uniform_real_distribution<>>(0, 1);
    }
    return (*m_distr.get())(*m_randomGeneratorEngine.get());
}
