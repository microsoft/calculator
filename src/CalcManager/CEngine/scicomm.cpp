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
#include "pch.h"
#include "Header Files/CalcEngine.h"
#include "Header Files/CalcUtils.h"

#define IDC_RADSIN  IDC_UNARYLAST+1
#define IDC_RADCOS  IDC_UNARYLAST+2
#define IDC_RADTAN  IDC_UNARYLAST+3
#define IDC_GRADSIN IDC_UNARYLAST+4
#define IDC_GRADCOS IDC_UNARYLAST+5
#define IDC_GRADTAN IDC_UNARYLAST+6

using namespace std;
using namespace CalcEngine;

// NPrecedenceOfOp
//
//  returns a virtual number for precedence for the operator. We expect binary operator only, otherwise the lowest number
// 0 is returned. Higher the number, higher the precedence of the operator.
INT NPrecedenceOfOp(int nopCode)
{
    static BYTE    rgbPrec[] = { 0,0,  IDC_OR,0, IDC_XOR,0,  IDC_AND,1,
        IDC_ADD,2, IDC_SUB,2,    IDC_RSHF,3, IDC_LSHF,3,
        IDC_MOD,3, IDC_DIV,3, IDC_MUL,3,  IDC_PWR,4,   IDC_ROOT, 4 };
    int iPrec;

    iPrec = 0;
    while ((iPrec < ARRAYSIZE(rgbPrec)) && (nopCode != rgbPrec[iPrec]))
    {
        iPrec += 2;
    }
    if (iPrec >= ARRAYSIZE(rgbPrec))
    {
        iPrec = 0;
    }
    return rgbPrec[iPrec + 1];

}

// HandleErrorCommand
//
//  When it is discovered by the state machine that at this point the input is not valid (eg. "1+)"), we want to proceed as though this input never
// occurred and may be some feedback to user like Beep. The rest of input can then continue by just ignoring this command.
void CCalcEngine::HandleErrorCommand(WPARAM idc)
{
    if (!IsGuiSettingOpCode(idc))
    {
        // we would have saved the prev command. Need to forget this state
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

void CCalcEngine::ProcessCommand(WPARAM wParam)
{
    if (wParam == IDC_SET_RESULT)
    {
        wParam = IDC_RECALL;
        m_bSetCalcState = true;
    }

    ProcessCommandWorker(wParam);
}

void CCalcEngine::ProcessCommandWorker(WPARAM wParam)
{
    INT            nx, ni;

    // Save the last command.  Some commands are not saved in this manor, these
    // commands are:
    // Inv, Deg, Rad, Grad, Stat, FE, MClear, Back, and Exp.  The excluded
    // commands are not
    // really mathematical operations, rather they are GUI mode settings.

    if (!IsGuiSettingOpCode(wParam))
    {
        m_nLastCom = m_nTempCom;
        m_nTempCom = (INT)wParam;
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

    // Toggle Record/Display mode if appropriate.    
    if (m_bRecord)
    {
        if (IsOpInRange(wParam, IDC_AND, IDC_MMINUS) ||
            IsOpInRange(wParam, IDC_OPENP, IDC_CLOSEP) ||
            IsOpInRange(wParam, IDM_HEX, IDM_BIN) ||
            IsOpInRange(wParam, IDM_QWORD, IDM_BYTE) ||
            IsOpInRange(wParam, IDM_DEG, IDM_GRAD) ||
            IsOpInRange(wParam, IDC_BINEDITSTART, IDC_BINEDITSTART + 63) ||
            (IDC_INV == wParam) ||
            (IDC_SIGN == wParam && 10 != m_radix))
        {
            m_bRecord = false;
            m_currentVal = m_input.ToRational(m_radix, m_precision);
            DisplayNum();   // Causes 3.000 to shrink to 3. on first op.
        }
    }
    else
    {
        if (IsDigitOpCode(wParam) || wParam == IDC_PNT)
        {
            m_bRecord = true;
            m_input.Clear();
            CheckAndAddLastBinOpToHistory();
        }
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

        if (!m_input.TryAddDigit(iValue, m_radix, m_fIntegerMode, m_maxDecimalValueStrings[m_numwidth], m_dwWordBitWidth, m_cIntDigitsSav))
        {
            HandleErrorCommand(wParam);
            HandleMaxDigitsReached();
            return;
        }

        DisplayNum();

        return;
    }

    // BINARY OPERATORS:
    if (IsBinOpCode(wParam))
    {
        /* Change the operation if last input was operation.          */
        if (IsBinOpCode(m_nLastCom))
        {
            INT nPrev;
            bool fPrecInvToHigher = false; // Is Precedence Inversion from lower to higher precedence happening ??

            m_nOpCode = (INT)wParam;

            // Check to see if by changing this binop, a Precedence inversion is happening.
            // Eg. 1 * 2  + and + is getting changed to ^. The previous precedence rules would have already computed
            // 1*2, so we will put additional brackets to cover for precedence inversion and it will become (1 * 2) ^
            // Here * is m_nPrevOpCode, m_currentVal is 2  (by 1*2), m_nLastCom is +, m_nOpCode is ^
            if (m_fPrecedence && 0 != m_nPrevOpCode)
            {
                nPrev = NPrecedenceOfOp(m_nPrevOpCode);
                nx = NPrecedenceOfOp(m_nLastCom);
                ni = NPrecedenceOfOp(m_nOpCode);
                if (nx <= nPrev && ni > nPrev) // condition for Precedence Inversion
                {
                    fPrecInvToHigher = true;
                    m_nPrevOpCode = 0; // Once the precedence inversion has put additional brackets, its no longer required
                }
            }
            m_HistoryCollector.ChangeLastBinOp(m_nOpCode, fPrecInvToHigher);
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

            nx = NPrecedenceOfOp((int)wParam);
            ni = NPrecedenceOfOp(m_nOpCode);

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

        m_lastVal = m_currentVal;
        m_nOpCode = (INT)wParam;
        m_HistoryCollector.AddBinOpToHistory(m_nOpCode);
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

            m_HistoryCollector.AddUnaryOpToHistory((INT)wParam, m_bInv, m_angletype);
        }

        if ((wParam == IDC_SIN) || (wParam == IDC_COS) || (wParam == IDC_TAN) || (wParam == IDC_SINH) || (wParam == IDC_COSH) || (wParam == IDC_TANH))
        {
            if (IsCurrentTooBigForTrig())
            {
                m_currentVal = 0;
                DisplayError(CALC_E_DOMAIN);
                return;
            }
        }

        m_currentVal = SciCalcFunctions(m_currentVal, (DWORD)wParam);

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

        if (m_bInv &&
            ((wParam == IDC_CHOP) ||
            (wParam == IDC_SIN) || (wParam == IDC_COS) || (wParam == IDC_TAN) ||
                (wParam == IDC_LN) || (wParam == IDC_DMS) || (wParam == IDC_DEGREES) ||
                (wParam == IDC_SINH) || (wParam == IDC_COSH) || (wParam == IDC_TANH)))
        {
            m_bInv = false;
        }

        return;
    }

    // Tiny binary edit windows clicked. Toggle that bit and update display
    if (IsOpInRange(wParam, IDC_BINEDITSTART, IDC_BINEDITSTART + 63))
    {
        // Same reasoning as for unary operators. We need to seed it previous number
        if (m_nLastCom >= IDC_AND && m_nLastCom <= IDC_PWR)
        {
            m_currentVal = m_lastVal;
        }

        CheckAndAddLastBinOpToHistory();

        if (TryToggleBit(m_currentVal, (DWORD)wParam - IDC_BINEDITSTART))
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
            // A special goody we are doing to preserve the history, if all was done was serious of unary operations last
            CheckAndAddLastBinOpToHistory(false);
        }

        m_lastVal = 0;

        m_bChangeOp = false;
        m_precedenceOpCount = m_nTempCom = m_nLastCom = m_nOpCode = m_openParenCount = 0;
        m_nPrevOpCode = 0;
        m_bNoPrevEqu = true;


        /* clear the parenthesis status box indicator, this will not be
        cleared for CENTR */
        if (nullptr != m_pCalcDisplay)
        {
            m_pCalcDisplay->SetParenDisplayText(L"");
            m_pCalcDisplay->SetExpressionDisplay(make_shared<CalculatorVector<pair<wstring, int>>>(), make_shared<CalculatorVector<shared_ptr<IExpressionCommand>>>());
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
            m_nLastCom = m_nTempCom; // Actually this is IDC_CLOSEP
            m_nTempCom = (INT)wParam; // put back in the state where last op seen was IDC_CLOSEP, and current op is IDC_EQU
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

        do {

            if (m_nOpCode) /* Is there a valid operation around?        */
            {
                /* If this is the first EQU in a string, set m_holdVal=m_currentVal */
                /* Otherwise let m_currentVal=m_holdVal.  This keeps m_currentVal constant */
                /* through all EQUs in a row.                     */
                if (m_bNoPrevEqu)
                {
                    m_holdVal = m_currentVal;
                }
                else
                {
                    m_currentVal = m_holdVal;
                    DisplayNum(); // to update the m_numberString
                    m_HistoryCollector.AddBinOpToHistory(m_nOpCode, false);
                    m_HistoryCollector.AddOpndToHistory(m_numberString, m_currentVal); // Adding the repeated last op to history
                }

                // Do the current or last operation.
                m_currentVal = DoOperation(m_nOpCode, m_currentVal, m_lastVal);
                m_nPrevOpCode = m_nOpCode;
                m_lastVal = m_currentVal;

                /* Check for errors.  If this wasn't done, DisplayNum */
                /* would immediately overwrite any error message.     */
                if (!m_bError)
                    DisplayNum();

                /* No longer the first EQU.                       */
                m_bNoPrevEqu = false;
            }
            else if (!m_bError)
                DisplayNum();

            if (m_precedenceOpCount == 0 || !m_fPrecedence)
                break;

            m_nOpCode = m_nPrecOp[--m_precedenceOpCount];
            m_lastVal = m_precedenceVals[m_precedenceOpCount];

            // Precedence Inversion check
            ni = NPrecedenceOfOp(m_nPrevOpCode);
            nx = NPrecedenceOfOp(m_nOpCode);
            if (ni <= nx)
            {
                m_HistoryCollector.EnclosePrecInversionBrackets();
            }
            m_HistoryCollector.PopLastOpndStart();

            m_bNoPrevEqu = true;
        } while (m_precedenceOpCount >= 0);

        if (!m_bError)
        {
            wstring groupedString = GroupDigitsPerRadix(m_numberString, m_radix);
            m_HistoryCollector.CompleteHistoryLine(groupedString);
            if (nullptr != m_pCalcDisplay)
            {
                m_pCalcDisplay->SetExpressionDisplay(make_shared<CalculatorVector<pair<wstring, int>>>(), make_shared<CalculatorVector<shared_ptr<IExpressionCommand>>>());
            }
        }

        m_bChangeOp = false;
        m_nPrevOpCode = 0;

        break;

    case IDC_OPENP:
    case IDC_CLOSEP:
        nx = 0;
        if (wParam == IDC_OPENP)
        {
            nx = 1;
        }

        // -IF- the Paren holding array is full and we try to add a paren
        // -OR- the paren holding array is empty and we try to remove a
        //      paren
        // -OR- the precedence holding array is full
        if ((m_openParenCount >= MAXPRECDEPTH && nx) || (!m_openParenCount && !nx)
            || ((m_precedenceOpCount >= MAXPRECDEPTH && m_nPrecOp[m_precedenceOpCount - 1] != 0)))
        {
            HandleErrorCommand(wParam);
            break;
        }

        if (nx)
        {
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
                ni = NPrecedenceOfOp(m_nPrevOpCode);
                nx = NPrecedenceOfOp(m_nOpCode);
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
            m_pCalcDisplay->SetParenDisplayText(m_openParenCount ? to_wstring(m_openParenCount) : L"");
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
        SetRadixTypeAndNumWidth((RADIX_TYPE)(wParam - IDM_HEX), (NUM_WIDTH)-1);
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
        SetRadixTypeAndNumWidth((RADIX_TYPE)-1, (NUM_WIDTH)(wParam - IDM_QWORD));
        break;

    case IDM_DEG:
    case IDM_RAD:
    case IDM_GRAD:
        m_angletype = static_cast<ANGLE_TYPE>(wParam - IDM_DEG);
        break;

    case IDC_SIGN:
    {
        if (m_bRecord)
        {
            if (m_input.TryToggleSign(m_fIntegerMode, m_maxDecimalValueStrings[m_numwidth]))
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

    case IDC_FE:
        // Toggle exponential notation display.
        m_nFE = NUMOBJ_FMT(!(int)m_nFE);
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
        if ((IsUnaryOpCode(m_nLastCom) || IDC_SIGN == m_nLastCom || IDC_CLOSEP == m_nLastCom) &&
            0 == m_openParenCount)
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
typedef struct
{
    int idsFunc;    // index of string for the unary op function. Can be NULL, in which case it same as button name
    int idsFuncInv; // index of string for Inv of unary op. Can be NULL, in case it is same as idsFunc
    bool fDontUseInExpEval;  // true if this cant be used in reverse direction as well, ie. during expression evaluation
} UFNE;

// Table for each unary operator
static const UFNE rgUfne[] =
{
    /* IDC_CHOP  */{ 0, IDS_FRAC, false },
    /* IDC_ROL   */{ 0, 0, true },
    /* IDC_ROR   */{ 0, 0, true },

    /* IDC_COM   */{ 0, 0, true },
    /* IDC_SIN    */{ IDS_SIND, IDS_ASIND, false },   // default in this table is degrees for sin,cos & tan
    /* IDC_COS   */{ IDS_COSD, IDS_ACOSD, false },
    /* IDC_TAN   */{ IDS_TAND, IDS_ATAND, false },

    /* IDC_SINH    */{ 0, IDS_ASINH, false },
    /* IDC_COSH   */{ 0, IDS_ACOSH, false },
    /* IDC_TANH   */{ 0, IDS_ATANH, false },

    /* IDC_LN     */{ 0, IDS_POWE, false },
    /* IDC_LOG   */{ 0, 0, false },
    /* IDC_SQRT */{ 0, 0, false },
    /* IDC_SQR   */{ IDS_SQR, 0, false },
    /* IDC_CUB   */{ IDS_CUBE, 0, false },
    /* IDC_FAC   */{ IDS_FACT, 0, false },
    /* IDC_REC   */{ IDS_REC, 0, false },
    /* IDC_DMS  */{ 0, IDS_DEGREES, false },
    /* IDC_CUBEROOT  */{ 0, 0, false },
    /* IDC_POW10  */{ 0, 0, false },
    /* IDC_PERCENT  */{ 0, 0, false },

    /* IDC_RADSIN  */{ IDS_SINR, IDS_ASINR, false },
    /* IDC_RADCOS  */{ IDS_COSR, IDS_ACOSR, false },
    /* IDC_RADTAN  */{ IDS_TANR, IDS_ATANR, false },
    /* IDC_GRADCOS  */{ IDS_SING, IDS_ASING, false },
    /* IDC_GRADCOS  */{ IDS_COSG, IDS_ACOSG, false },
    /* IDC_GRADTAN  */{ IDS_TANG, IDS_ATANG, false },
};

wstring_view CCalcEngine::OpCodeToUnaryString(int nOpCode, bool fInv, ANGLE_TYPE angletype)
{
    // Special cases for Sign and Degrees
    if (IDC_SIGN == nOpCode)
    {
        return GetString(IDS_NEGATE);
    }
    if (IDC_DEGREES == nOpCode)
    {
        return GetString(IDS_DEGREES);
    }

    // Correct the trigonometric functions with type of angle argument they take
    if (ANGLE_RAD == angletype)
    {
        switch (nOpCode)
        {
        case IDC_SIN:
            nOpCode = IDC_RADSIN;
            break;
        case IDC_COS:
            nOpCode = IDC_RADCOS;
            break;
        case IDC_TAN:
            nOpCode = IDC_RADTAN;
            break;
        }
    }
    else if (ANGLE_GRAD == angletype)
    {
        switch (nOpCode)
        {
        case IDC_SIN:
            nOpCode = IDC_GRADSIN;
            break;
        case IDC_COS:
            nOpCode = IDC_GRADCOS;
            break;
        case IDC_TAN:
            nOpCode = IDC_GRADTAN;
            break;
        }
    }

    // Try to lookup the ID in the UFNE table
    int ids = 0;
    int iufne = nOpCode - IDC_UNARYFIRST;
    if (iufne >= 0 && iufne < ARRAYSIZE(rgUfne))
    {
        if (fInv)
        {
            ids = rgUfne[iufne].idsFuncInv;
        }
        if (0 == ids)
        {
            ids = rgUfne[iufne].idsFunc;
        }
    }

    // If we didn't find an ID in the table, use the op code.
    if (0 == ids)
    {
        ids = IdStrFromCmdId(nOpCode);
    }

    return GetString(ids);
}

//
// Sets the Angle Mode for special unary op IDC's which are used to index to the table rgUfne
// and returns the equivalent plain IDC for trigonometric function. If it isn't a trigonometric function
// returns the passed in idc itself.
int CCalcEngine::IdcSetAngleTypeDecMode(int idc)
{
    int idcAngleCmd = IDM_DEG;

    switch (idc)
    {
    case IDC_RADSIN:
        idcAngleCmd = IDM_RAD;
        idc = IDC_SIN;
        break;
    case IDC_RADCOS:
        idcAngleCmd = IDM_RAD;
        idc = IDC_COS;
        break;
    case IDC_RADTAN:
        idcAngleCmd = IDM_RAD;
        idc = IDC_TAN;
        break;
    case IDC_GRADSIN:
        idcAngleCmd = IDM_GRAD;
        idc = IDC_SIN;
        break;
    case IDC_GRADCOS:
        idcAngleCmd = IDM_GRAD;
        idc = IDC_COS;
        break;
    case IDC_GRADTAN:
        idcAngleCmd = IDM_GRAD;
        idc = IDC_TAN;
        break;
    }
    ProcessCommand(idcAngleCmd);
    return idc;

}

bool CCalcEngine::IsCurrentTooBigForTrig()
{
    return m_currentVal >= m_maxTrigonometricNum;
}

int CCalcEngine::GetCurrentRadix()
{
    return m_radix;
}

wstring CCalcEngine::GetCurrentResultForRadix(uint32_t radix, int32_t precision)
{
    Rational rat = (m_bRecord ? m_input.ToRational(m_radix, m_precision) : m_currentVal);

    ChangeConstants(m_radix, precision);

    wstring numberString = GetStringForDisplay(rat, radix);
    if (!numberString.empty())
    {
        //revert the precision to previously stored precision
        ChangeConstants(m_radix, m_precision);
    }

    return GroupDigitsPerRadix(numberString, radix);
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
                tempRat = -((tempRat ^ m_chopNumbers[m_numwidth]) + 1);
            }

            result = tempRat.ToString(radix, m_nFE, m_precision);
        }
        catch (DWORD)
        {
        }
    }

    return result;
}
