// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Header Files/CalcEngine.h"

using namespace CalcEngine;
using namespace CalcEngine::RationalMath;

// Routines to perform standard operations &|^~<<>>+-/*% and pwr.
CalcEngine::Rational CCalcEngine::DoOperation(int operation, CalcEngine::Rational const& lhs, CalcEngine::Rational const& rhs)
{
    // Remove any variance in how 0 could be represented in rat e.g. -0, 0/n, etc.
    auto result = (!lhs.IsZero() ? lhs : Rational{});

    try
    {
        switch (operation)
        {
        case IDC_AND:
            result = And(result, rhs, m_radix, m_precision);
            break;

        case IDC_OR:
            result = Or(result, rhs, m_radix, m_precision);
            break;

        case IDC_XOR:
            result = Xor(result, rhs, m_radix, m_precision);
            break;

        case IDC_RSHF:
        {
            if (m_fIntegerMode && result.IsGreaterEq(Rational{ m_dwWordBitWidth }, m_precision)) // Lsh/Rsh >= than current word size is always 0
            {
                throw CALC_E_NORESULT;
            }

            uint64_t w64Bits = rhs.ToUInt64_t(m_radix, m_precision);
            bool fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

            Rational holdVal = result;
            result = Rsh(rhs, holdVal, m_radix, m_precision);

            if (fMsb)
            {
                result = Integer(result, m_radix, m_precision);

                auto tempRat = Rsh(m_chopNumbers[m_numwidth], holdVal, m_radix, m_precision);
                tempRat = Integer(tempRat, m_radix, m_precision);

                tempRat = Xor(tempRat, m_chopNumbers[m_numwidth], m_radix, m_precision);
                result = Or(result, tempRat, m_radix, m_precision);
            }
            break;
        }

        case IDC_LSHF:
            if (m_fIntegerMode && result.IsGreaterEq(Rational{ m_dwWordBitWidth }, m_precision)) // Lsh/Rsh >= than current word size is always 0
            {
                throw CALC_E_NORESULT;
            }

            result = Lsh(rhs, result, m_radix, m_precision);
            break;

        case IDC_ADD:
            result = Add(result, rhs, m_precision);
            break;

        case IDC_SUB:
            result = Sub(rhs, result, m_precision);
            break;

        case IDC_MUL:
            result = Mul(result, rhs, m_precision);
            break;

        case IDC_DIV:
        case IDC_MOD:
        {
            int iNumeratorSign = 1, iDenominatorSign = 1, iFinalSign = 1;
            auto temp = result;
            result = rhs;

            if (m_fIntegerMode)
            {
                uint64_t w64Bits = rhs.ToUInt64_t(m_radix, m_precision);
                bool fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

                if (fMsb)
                {
                    result = Not(rhs, true, m_chopNumbers[m_numwidth], m_radix, m_precision);
                    result = Add(result, 1, m_precision);

                    iNumeratorSign = -1;
                }

                w64Bits = temp.ToUInt64_t(m_radix, m_precision);
                fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

                if (fMsb)
                {
                    temp = Not(temp, true, m_chopNumbers[m_numwidth], m_radix, m_precision);
                    temp = Add(temp, 1, m_precision);

                    iDenominatorSign = -1;
                }
            }

            if (operation == IDC_DIV)
            {
                iFinalSign = iNumeratorSign * iDenominatorSign;
                result = Div(result, temp, m_precision);
            }
            else
            {
                iFinalSign = iNumeratorSign;
                result = Mod(result, temp);
            }

            if (m_fIntegerMode && iFinalSign == -1)
            {
                result = Negate(Integer(result, m_radix, m_precision));
            }

            break;
        }

        case IDC_PWR: // Calculates rhs to the result(th) power.
            result = Pow(rhs, result, m_radix, m_precision);
            break;

        case IDC_ROOT: // Calculates rhs to the result(th) root.
            result = Root(rhs, result, m_radix, m_precision);
            break;
        }
    }
    catch (DWORD dwErrCode)
    {
        DisplayError(dwErrCode);

        // On error, return the original value
        result = lhs;
    }

    return result;
}
