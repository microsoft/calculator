// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Header Files/CalcEngine.h"

using namespace CalcEngine;
using namespace CalcEngine::RationalMath;

// Routines to perform standard operations &|^~<<>>+-/*% and pwr.
CalcEngine::Rational CCalcEngine::DoOperation(int operation, CalcEngine::Rational const& lhs, CalcEngine::Rational const& rhs)
{
    // Remove any variance in how 0 could be represented in rat e.g. -0, 0/n, etc.
    auto result = (lhs != 0 ? lhs : 0);

    try
    {
        switch (operation)
        {
        case IDC_AND:
            result &= rhs;
            break;

        case IDC_OR:
            result |= rhs;
            break;

        case IDC_XOR:
            result ^= rhs;
            break;

        case IDC_NAND:
            result = (result & rhs) ^ GetChopNumber();
            break;

        case IDC_NOR:
            result = (result | rhs) ^ GetChopNumber();
            break;

        case IDC_RSHF:
        {
            if (m_fIntegerMode && result >= m_dwWordBitWidth) // Lsh/Rsh >= than current word size is always 0
            {
                throw CALC_E_NORESULT;
            }

            uint64_t w64Bits = rhs.ToUInt64_t();
            bool fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

            Rational holdVal = result;
            result = rhs >> holdVal;

            if (fMsb)
            {
                result = Integer(result);

                auto tempRat = GetChopNumber() >> holdVal;
                tempRat = Integer(tempRat);

                result |= tempRat ^ GetChopNumber();
            }
            break;
        }
        case IDC_RSHFL:
        {
            if (m_fIntegerMode && result >= m_dwWordBitWidth) // Lsh/Rsh >= than current word size is always 0
            {
                throw CALC_E_NORESULT;
            }

            result = rhs >> result;
            break;
        }
        case IDC_LSHF:
            if (m_fIntegerMode && result >= m_dwWordBitWidth) // Lsh/Rsh >= than current word size is always 0
            {
                throw CALC_E_NORESULT;
            }

            result = rhs << result;
            break;

        case IDC_ADD:
            result += rhs;
            break;

        case IDC_SUB:
            result = rhs - result;
            break;

        case IDC_MUL:
            result *= rhs;
            break;

        case IDC_DIV:
        case IDC_MOD:
        {
            int iNumeratorSign = 1, iDenominatorSign = 1;
            auto temp = result;
            result = rhs;

            if (m_fIntegerMode)
            {
                uint64_t w64Bits = rhs.ToUInt64_t();
                bool fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

                if (fMsb)
                {
                    result = (rhs ^ GetChopNumber()) + 1;

                    iNumeratorSign = -1;
                }

                w64Bits = temp.ToUInt64_t();
                fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1;

                if (fMsb)
                {
                    temp = (temp ^ GetChopNumber()) + 1;

                    iDenominatorSign = -1;
                }
            }

            if (operation == IDC_DIV)
            {
                result /= temp;
                if (m_fIntegerMode && (iNumeratorSign * iDenominatorSign) == -1)
                {
                    result = -(Integer(result));
                }
            }
            else
            {
                if (m_fIntegerMode)
                {
                    // Programmer mode, use remrat (remainder after division)
                    result %= temp;

                    if (iNumeratorSign == -1)
                    {
                        result = -(Integer(result));
                    }
                }
                else
                {
                    // other modes, use modrat (modulus after division)
                    result = Mod(result, temp);
                }
            }
            break;
        }

        case IDC_PWR: // Calculates rhs to the result(th) power.
            result = Pow(rhs, result);
            break;

        case IDC_ROOT: // Calculates rhs to the result(th) root.
            result = Root(rhs, result);
            break;

        case IDC_LOGBASEY:
            result = (Log(rhs) / Log(result));
            break;
        }
    }
    catch (uint32_t dwErrCode)
    {
        DisplayError(dwErrCode);

        // On error, return the original value
        result = lhs;
    }

    return result;
}
