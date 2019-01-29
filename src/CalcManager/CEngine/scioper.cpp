// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Header Files/CalcEngine.h"

using namespace CalcEngine;

// Routines to perform standard operations &|^~<<>>+-/*% and pwr.
CalcEngine::Rational CCalcEngine::DoOperation(int operation, CalcEngine::Rational const& lhs, CalcEngine::Rational const& rhs)
{
    PRAT rhsRat = rhs.ToPRAT();
    PRAT hno = nullptr;

    // Remove any variance in how 0 could be represented in rat e.g. -0, 0/n, etc.
    PRAT resultRat = nullptr;
    if (lhs.IsZero())
    {
        NumObjAssign(&resultRat, rat_zero);
    }
    else
    {
        resultRat = lhs.ToPRAT();
    }

    try
    {
        switch (operation)
        {
        case IDC_AND:
            andrat(&resultRat, rhsRat, m_radix, m_precision);
            break;

        case IDC_OR:
            orrat(&resultRat, rhsRat, m_radix, m_precision);
            break;

        case IDC_XOR:
            xorrat(&resultRat, rhsRat, m_radix, m_precision);
            break;

        case IDC_RSHF:
        {
            if (m_fIntegerMode)
            {
                NumObjSetIntValue(&hno, m_dwWordBitWidth);
                if (NumObjIsGreaterEq(resultRat, hno, m_precision)) // Lsh/Rsh >= than current word size is always 0
                {
                    throw CALC_E_NORESULT;
                }
            }

            NumObjAssign(&hno, resultRat);
            NumObjAssign(&resultRat, rhsRat);

            uint64_t w64Bits = NumObjGetUlValue(resultRat, m_radix, m_precision);
            bool fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

            rshrat(&resultRat, hno, m_radix, m_precision);

            if (fMsb)
            {
                intrat(&resultRat, m_radix, m_precision);
                PRAT temp = m_chopNumbers[m_numwidth].ToPRAT();
                PRAT chopRat = m_chopNumbers[m_numwidth].ToPRAT();
                rshrat(&temp, hno, m_radix, m_precision);
                intrat(&temp, m_radix, m_precision);
                xorrat(&temp, chopRat, m_radix, m_precision);
                orrat(&resultRat, temp, m_radix, m_precision);
                destroyrat(temp);
                destroyrat(chopRat);
            }
            break;
        }

        case IDC_LSHF:
            if (m_fIntegerMode)
            {
                NumObjSetIntValue(&hno, m_dwWordBitWidth);
                if (NumObjIsGreaterEq(resultRat, hno, m_precision))
                {
                    throw CALC_E_NORESULT;
                }
            }

            NumObjAssign(&hno, resultRat);
            NumObjAssign(&resultRat, rhsRat);

            lshrat(&resultRat, hno, m_radix, m_precision);
            break;

        case IDC_ADD:
            addrat(&resultRat, rhsRat, m_precision);
            break;

        case IDC_SUB:
            // in order to do ( rhsRat - resultRat ) we actually do -(resultRat - rhsRat ) because it's quicker
            subrat(&resultRat, rhsRat, m_precision);
            NumObjNegate(&resultRat);
            break;

        case IDC_MUL:
            mulrat(&resultRat, rhsRat, m_precision);
            break;

        case IDC_DIV:
        case IDC_MOD:
            {
                int iNumeratorSign = 1, iDenominatorSign = 1, iFinalSign = 1;
                // REVIEW:  These lengthly number assignments can be replaced with some quick pointer swaps.
                // the swaps cannot change the value of pratX unless we also modify the code that calls
                // the DoOperation function.
                NumObjAssign(&hno, resultRat);
                NumObjAssign(&resultRat, rhsRat);

                if(m_fIntegerMode)
                {
                    uint64_t w64Bits = NumObjGetUlValue(resultRat, m_radix, m_precision);
                    bool fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

                    if (fMsb)
                    {
                        PRAT chopRat = m_chopNumbers[m_numwidth].ToPRAT();
                        NumObjNot(&resultRat, true, chopRat, m_radix, m_precision);
                        destroyrat(chopRat);
                        addrat(&resultRat, rat_one, m_precision);

                        iNumeratorSign = -1;
                    }

                    w64Bits = NumObjGetUlValue(hno, m_radix, m_precision);
                    fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

                    if (fMsb)
                    {
                        PRAT chopRat = m_chopNumbers[m_numwidth].ToPRAT();
                        NumObjNot( &hno, true, chopRat, m_radix, m_precision);
                        destroyrat(chopRat);
                        addrat( &hno, rat_one, m_precision);

                        iDenominatorSign = -1;
                    }
                }

                if (operation == IDC_DIV)
                {
                    iFinalSign = iNumeratorSign * iDenominatorSign;
                    divrat(&resultRat, hno, m_precision);
                }
                else
                {
                    iFinalSign = iNumeratorSign; 
                    modrat(&resultRat, hno );
                }

                if (m_fIntegerMode && iFinalSign == -1)
                {
                    intrat(&resultRat, m_radix, m_precision);
                    NumObjNegate(&resultRat);
                }

                break;
            }

        case IDC_PWR: // Calculates rhsRat to the resultRat(th) power.
            {
                NumObjAssign(&hno, resultRat);
                NumObjAssign(&resultRat, rhsRat);

                powrat(&resultRat, hno , m_radix, m_precision);

                break;
            }
        case IDC_ROOT: // Calculates rhsRat to the resultRat(th) root.
            {
                NumObjAssign(&hno, resultRat);
                NumObjAssign(&resultRat, rhsRat);

                rootrat(&resultRat, hno, m_radix, m_precision);

                break;
            }
        }
    }
    catch (DWORD dwErrCode)
    {
        DisplayError(dwErrCode);

        // On error, return the original value
        destroyrat(resultRat);
        resultRat = lhs.ToPRAT();
    }

    destroyrat(hno);
    destroyrat(rhsRat);

    Rational result{ resultRat };
    destroyrat(resultRat);

    return result;
}
