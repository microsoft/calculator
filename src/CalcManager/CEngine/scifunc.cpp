// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/**************************************************************************/
/*** SCICALC Scientific Calculator for Windows 3.00.12                  ***/
/*** (c)1989 Microsoft Corporation.  All Rights Reserved.               ***/
/***                                                                    ***/
/*** scifunc.c                                                          ***/
/***                                                                    ***/
/*** Functions contained:                                               ***/
/***    SciCalcFunctions--do sin, cos, tan, com, log, ln, rec, fac, etc.***/
/***    DisplayError--Error display driver.                             ***/
/***                                                                    ***/
/*** Functions called:                                                  ***/
/***    SciCalcFunctions call DisplayError.                             ***/
/***                                                                    ***/
/***                                                                    ***/
/**************************************************************************/
#include "Header Files/CalcEngine.h"
#include "winerror_cross_platform.h"

using namespace std;
using namespace CalcEngine;
using namespace RationalMath;

/* Routines for more complex mathematical functions/error checking. */
Rational CCalcEngine::SciCalcFunctions(Rational const& rat, uint32_t op)
{
    Rational result{};
    try
    {
        switch (op)
        {
        case IDC_CHOP:
            result = m_bInv ? Frac(rat) : Integer(rat);
            break;

            /* Return complement. */
        case IDC_COM:
            if (m_radix == 10 && !m_fIntegerMode)
            {
                result = -(Integer(rat) + 1);
            }
            else
            {
                result = rat ^ GetChopNumber();
            }
            break;

        case IDC_ROL:
        case IDC_ROLC:
            if (m_fIntegerMode)
            {
                result = Integer(rat);

                uint64_t w64Bits = result.ToUInt64_t();
                uint64_t msb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;
                w64Bits <<= 1; // LShift by 1

                if (op == IDC_ROL)
                {
                    w64Bits |= msb; // Set the prev Msb as the current Lsb
                }
                else
                {
                    w64Bits |= m_carryBit; // Set the carry bit as the LSB
                    m_carryBit = msb;      // Store the msb as the next carry bit
                }

                result = w64Bits;
            }
            break;

        case IDC_ROR:
        case IDC_RORC:
            if (m_fIntegerMode)
            {
                result = Integer(rat);

                uint64_t w64Bits = result.ToUInt64_t();
                uint64_t lsb = w64Bits & 0x01;
                w64Bits >>= 1; // RShift by 1

                if (op == IDC_ROR)
                {
                    w64Bits |= (lsb << (m_dwWordBitWidth - 1));
                }
                else
                {
                    w64Bits |= (m_carryBit << (m_dwWordBitWidth - 1));
                    m_carryBit = lsb;
                }

                result = w64Bits;
            }
            break;

        case IDC_PERCENT:
        {
            // If the operator is multiply/divide, we evaluate this as "X [op] (Y%)"
            // Otherwise, we evaluate it as "X [op] (X * Y%)"
            if (m_nOpCode == IDC_MUL || m_nOpCode == IDC_DIV)
            {
                result = rat / 100;
            }
            else
            {
                result = rat * (m_lastVal / 100);
            }
            break;
        }

        case IDC_SIN: /* Sine; normal and arc */
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ASin(rat, m_angletype) : Sin(rat, m_angletype);
            }
            break;

        case IDC_SINH: /* Sine- hyperbolic and archyperbolic */
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ASinh(rat) : Sinh(rat);
            }
            break;

        case IDC_COS: /* Cosine, follows convention of sine function. */
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ACos(rat, m_angletype) : Cos(rat, m_angletype);
            }
            break;

        case IDC_COSH: /* Cosine hyperbolic, follows convention of sine h function. */
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ACosh(rat) : Cosh(rat);
            }
            break;

        case IDC_TAN: /* Same as sine and cosine. */
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ATan(rat, m_angletype) : Tan(rat, m_angletype);
            }
            break;

        case IDC_TANH: /* Same as sine h and cosine h. */
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ATanh(rat) : Tanh(rat);
            }
            break;

        case IDC_SEC:
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ACos(Invert(rat), m_angletype) : Invert(Cos(rat, m_angletype));
            }
            break;

        case IDC_CSC:
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ASin(Invert(rat), m_angletype) : Invert(Sin(rat, m_angletype));
            }
            break;

        case IDC_COT:
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ATan(Invert(rat), m_angletype) : Invert(Tan(rat, m_angletype));
            }
            break;

        case IDC_SECH:
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ACosh(Invert(rat)) : Invert(Cosh(rat));
            }
            break;

        case IDC_CSCH:
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ASinh(Invert(rat)) : Invert(Sinh(rat));
            }
            break;

        case IDC_COTH:
            if (!m_fIntegerMode)
            {
                result = m_bInv ? ATanh(Invert(rat)) : Invert(Tanh(rat));
            }
            break;

        case IDC_REC: /* Reciprocal. */
            result = Invert(rat);
            break;

        case IDC_SQR: /* Square */
            result = Pow(rat, 2);
            break;

        case IDC_SQRT: /* Square Root */
            result = Root(rat, 2);
            break;

        case IDC_CUBEROOT:
        case IDC_CUB: /* Cubing and cube root functions. */
            result = IDC_CUBEROOT == op ? Root(rat, 3) : Pow(rat, 3);
            break;

        case IDC_LOG: /* Functions for common log. */
            result = Log10(rat);
            break;

        case IDC_POW10:
            result = Pow(10, rat);
            break;

        case IDC_POW2:
            result = Pow(2, rat);
            break;

        case IDC_LN: /* Functions for natural log. */
            result = m_bInv ? Exp(rat) : Log(rat);
            break;

        case IDC_FAC: /* Calculate factorial.  Inverse is ineffective. */
            result = Fact(rat);
            break;

        case IDC_DEGREES:
            ProcessCommand(IDC_INV);
            // This case falls through to IDC_DMS case because in the old Win32 Calc,
            // the degrees functionality was achieved as 'Inv' of 'dms' operation,
            // so setting the IDC_INV command first and then performing 'dms' operation as global variables m_bInv, m_bRecord
            // are set properly through ProcessCommand(IDC_INV)
            [[fallthrough]];
        case IDC_DMS:
        {
            if (!m_fIntegerMode)
            {
                auto shftRat{ m_bInv ? 100 : 60 };

                Rational degreeRat = Integer(rat);

                Rational minuteRat = (rat - degreeRat) * shftRat;

                Rational secondRat = minuteRat;

                minuteRat = Integer(minuteRat);

                secondRat = (secondRat - minuteRat) * shftRat;

                //
                // degreeRat == degrees, minuteRat == minutes, secondRat == seconds
                //

                shftRat = m_bInv ? 60 : 100;
                secondRat /= shftRat;

                minuteRat = (minuteRat + secondRat) / shftRat;

                result = degreeRat + minuteRat;
            }
            break;
        }
        case IDC_CEIL:
            result = (Frac(rat) > 0) ? Integer(rat + 1) : Integer(rat);
            break;

        case IDC_FLOOR:
            result = (Frac(rat) < 0) ? Integer(rat - 1) : Integer(rat);
            break;

        case IDC_ABS:
            result = Abs(rat);
            break;

        } // end switch( op )
    }
    catch (uint32_t nErrCode)
    {
        DisplayError(nErrCode);
        result = rat;
    }

    return result;
}

/* Routine to display error messages and set m_bError flag.  Errors are */
/* called with DisplayError (n), where n is a uint32_t   between 0 and 5. */

void CCalcEngine::DisplayError(uint32_t nError)
{
    wstring errorString{ GetString(IDS_ERRORS_FIRST + SCODE_CODE(nError)) };

    SetPrimaryDisplay(errorString, true /*isError*/);

    m_bError = true; /* Set error flag.  Only cleared with CLEAR or CENTR. */

    m_HistoryCollector.ClearHistoryLine(errorString);
}
