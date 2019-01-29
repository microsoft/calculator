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
#include "pch.h"
#include "Header Files/CalcEngine.h"

using namespace std;
using namespace CalcEngine;

/* Routines for more complex mathematical functions/error checking. */
CalcEngine::Rational CCalcEngine::SciCalcFunctions(CalcEngine::Rational const& rat, DWORD op)
{
    PRAT tempRat = rat.ToPRAT();
    try
    {
        switch (op)
        {
            case IDC_CHOP:
                m_bInv ? fracrat(&tempRat , m_radix, m_precision) : intrat(&tempRat, m_radix, m_precision);
                break;

            /* Return complement. */
            case IDC_COM:
            {
                PRAT chopRat = m_chopNumbers[m_numwidth].ToPRAT();
                NumObjNot(&tempRat, m_fIntegerMode, chopRat, m_radix, m_precision);
                destroyrat(chopRat);
                break;
            }

            // Rotate Left with hi bit wrapped over to lo bit
            case IDC_ROL:
                if (m_fIntegerMode)
                {
                    intrat(&tempRat, m_radix, m_precision);

                    PRAT curRat = m_currentVal.ToPRAT();
                    ULONGLONG w64Bits = NumObjGetUlValue(curRat, m_radix, m_precision);
                    ULONGLONG msb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

                    w64Bits <<= 1; // LShift by 1
                    w64Bits |= msb; // Set the prev Msb as the current Lsb
                    NumObjSetUlonglongValue(&curRat, w64Bits, m_radix, m_precision);
                    m_currentVal = Rational{ curRat };
                    destroyrat(curRat);
                }
                break;

            // Rotate right with lo bit wrapped over to hi bit
            case IDC_ROR:
                if (m_fIntegerMode)
                {
                    intrat(&tempRat, m_radix, m_precision);
                    PRAT curRat = m_currentVal.ToPRAT();
                    ULONGLONG w64Bits = NumObjGetUlValue(curRat, m_radix, m_precision);

                    ULONGLONG lsb = ((w64Bits & 0x01) == 1) ? 1 : 0;
                    w64Bits >>= 1; //RShift by 1
                    w64Bits |= (lsb << (m_dwWordBitWidth - 1));
                    NumObjSetUlonglongValue(&curRat, w64Bits, m_radix, m_precision);
                    m_currentVal = Rational{ curRat };
                    destroyrat(curRat);
                }
                break;

            case IDC_PERCENT:
            {
                PRAT hno = nullptr;
                PRAT hno100 = nullptr;

                try
                {
                    // If the operator is multiply/divide, we evaluate this as "X [op] (Y%)"
                    // Otherwise, we evaluate it as "X [op] (X * Y%)"
                    if (m_nOpCode == IDC_MUL || m_nOpCode == IDC_DIV)
                    {
                        NumObjSetIntValue(&hno100, 100);

                        divrat(&tempRat, hno100, m_precision);
                        destroyrat(hno100);
                    }
                    else
                    {
                        hno = m_lastVal.ToPRAT();
                        NumObjSetIntValue(&hno100, 100);

                        divrat(&hno, hno100, m_precision);
                        destroyrat(hno100);

                        mulrat(&tempRat, hno, m_precision);
                        destroyrat(hno);
                    }
                }
                catch ( DWORD nErrCode )
                {
                    destroyrat(hno);
                    destroyrat(hno100);
                    destroyrat(tempRat);
                    throw nErrCode;
                }
                break;
            }

            case IDC_SIN: /* Sine; normal and arc */
                if (!m_fIntegerMode)
                {
                    m_bInv ? asinanglerat(&tempRat, m_angletype, m_radix, m_precision) : NumObjSin(&tempRat, m_angletype, m_radix, m_precision);
                }
                break;

            case IDC_SINH: /* Sine- hyperbolic and archyperbolic */
                if (!m_fIntegerMode)
                {
                    m_bInv ? asinhrat(&tempRat, m_radix, m_precision) : sinhrat(&tempRat, m_radix, m_precision);
                }
                break;

            case IDC_COS: /* Cosine, follows convention of sine function. */
                if (!m_fIntegerMode)
                {
                    m_bInv ? acosanglerat(&tempRat, m_angletype, m_radix, m_precision) : NumObjCos(&tempRat, m_angletype, m_radix, m_precision);
                }
                break;

            case IDC_COSH: /* Cosine hyperbolic, follows convention of sine h function. */
                if (!m_fIntegerMode)
                {
                    m_bInv ? acoshrat(&tempRat, m_radix, m_precision) : coshrat(&tempRat, m_radix, m_precision);
                }
                break;

            case IDC_TAN: /* Same as sine and cosine. */
                if (!m_fIntegerMode)
                {
                    m_bInv ? atananglerat(&tempRat, m_angletype, m_radix, m_precision) : NumObjTan(&tempRat, m_angletype, m_radix, m_precision);
                }
                break;

            case IDC_TANH: /* Same as sine h and cosine h. */
                if (!m_fIntegerMode)
                {
                    m_bInv ? atanhrat(&tempRat, m_precision) : tanhrat(&tempRat, m_radix, m_precision);
                }
                break;

            case IDC_REC: /* Reciprocal. */
                NumObjInvert(&tempRat, m_precision);
                break;

            case IDC_SQR: /* Square */
                powrat(&tempRat, rat_two, m_radix, m_precision);
                break;

            case IDC_SQRT: /* Sqrt only in Std mode */
                rootrat(&tempRat, rat_two, m_radix, m_precision);
                break;

            case IDC_CUBEROOT:
            case IDC_CUB: /* Cubing and cube root functions. */
            {
                PRAT hno = nullptr;

                try
                {
                    NumObjAssign(&hno, rat_one);
                    addrat(&hno, rat_two, m_precision);

                    if (IDC_CUBEROOT == op)
                    {
                        rootrat(&tempRat, hno, m_radix, m_precision);
                    }
                    else
                    {
                        powrat(&tempRat, hno, m_radix, m_precision);
                    }

                    destroyrat(hno);
                }
                catch (DWORD nErrCode)
                {
                    destroyrat(hno);
                    destroyrat(tempRat);
                    throw nErrCode;
                }
                break;
            }

            case IDC_LOG: /* Functions for common log. */
                log10rat(&tempRat, m_precision);
                break;
            case IDC_POW10:
                NumObjAntiLog10(&tempRat, m_radix, m_precision);
                break;
            case IDC_LN: /* Functions for natural log. */
                if (m_bInv)
                {
                    exprat(&tempRat, m_radix, m_precision);  // e^x.
                }
                else
                {
                    lograt(&tempRat, m_precision);
                }
                break;

            case IDC_FAC: /* Calculate factorial.  Inverse is ineffective. */
                factrat(&tempRat, m_radix, m_precision);
                break;

            case IDC_DEGREES:
                ProcessCommand(IDC_INV);
                // This case falls through to IDC_DMS case because in the old Win32 Calc, 
                // the degrees functionality was achieved as 'Inv' of 'dms' operation,
                // so setting the IDC_INV command first and then performing 'dms' operation as global variables m_bInv, m_bRecord 
                // are set properly through ProcessCommand(IDC_INV)
            case IDC_DMS:
            {
                if (!m_fIntegerMode)
                {
                    PRAT hnoMin = nullptr;
                    PRAT hnoSec = nullptr;
                    PRAT hnoShft = nullptr;

                    try
                    {
                        NumObjSetIntValue(&hnoShft, m_bInv ? 100 : 60);

                        NumObjAssign(&hnoMin, tempRat);
                        intrat(&tempRat, m_radix, m_precision);

                        subrat(&hnoMin, tempRat, m_precision);
                        mulrat(&hnoMin, hnoShft, m_precision);
                        NumObjAssign(&hnoSec, hnoMin );
                        intrat(&hnoMin, m_radix, m_precision);

                        subrat(&hnoSec, hnoMin, m_precision);
                        mulrat(&hnoSec, hnoShft, m_precision);

                        //
                        // tempRat == degrees, hnoMin == minutes, hnoSec == seconds
                        //

                        NumObjSetIntValue(&hnoShft, m_bInv ? 60 : 100);
                        divrat(&hnoSec, hnoShft, m_precision);
                        addrat(&hnoMin, hnoSec, m_precision);

                        divrat(&hnoMin, hnoShft, m_precision);
                        addrat(&tempRat, hnoMin, m_precision);

                        destroyrat(hnoShft);
                        destroyrat(hnoMin);
                        destroyrat(hnoSec);
                    }
                    catch (DWORD nErrCode)
                    {
                        destroyrat(hnoShft);
                        destroyrat(hnoMin);
                        destroyrat(hnoSec);
                        destroyrat(tempRat);
                        throw nErrCode;
                    }
                }
                break;
            }
        }   // end switch( op )

        Rational result{ tempRat };
        destroyrat(tempRat);
        return result;
    }
    catch(DWORD nErrCode)
    {
        DisplayError(nErrCode);
        destroyrat(tempRat);
        return rat;
    }
}

/* Routine to display error messages and set m_bError flag.  Errors are */
/* called with DisplayError (n), where n is a DWORD   between 0 and 5. */

void CCalcEngine::DisplayError(DWORD nError)
{
    wstring errorString{ GetString(IDS_ERRORS_FIRST + SCODE_CODE(nError)) };

    SetPrimaryDisplay(errorString, true /*isError*/);

    m_bError = true; /* Set error flag.  Only cleared with CLEAR or CENTR. */

    m_HistoryCollector.ClearHistoryLine(errorString);
}

