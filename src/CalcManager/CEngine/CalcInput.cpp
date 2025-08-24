// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <sstream>
#include "Header Files/CalcEngine.h"

using namespace std;
using namespace CalcEngine;

constexpr int C_NUM_MAX_DIGITS = MAX_STRLEN;
constexpr int C_EXP_MAX_DIGITS = 4;

void CalcNumSec::Clear()
{
    value.clear();
    m_isNegative = false;
}

void CalcInput::Clear()
{
    m_base.Clear();
    m_exponent.Clear();
    m_hasExponent = false;
    m_hasDecimal = false;
    m_decPtIndex = 0;
}

bool CalcInput::TryToggleSign(bool isIntegerMode, wstring_view maxNumStr)
{
    // Zero is always positive
    if (m_base.IsEmpty())
    {
        m_base.IsNegative(false);
        m_exponent.IsNegative(false);
    }
    else if (m_hasExponent)
    {
        m_exponent.IsNegative(!m_exponent.IsNegative());
    }
    else
    {
        // When in integer only mode, it isn't always allowed to toggle, as toggling can cause the num to be out of
        // bounds. For eg. in byte -128 is valid, but when it toggled it becomes 128, which is more than 127.
        if (isIntegerMode && m_base.IsNegative())
        {
            // Decide if this additional digit will fit for the given bit width
            if (m_base.value.size() >= maxNumStr.size() && m_base.value.back() > maxNumStr.back())
            {
                // Last digit is more than the allowed positive number. Fail
                return false;
            }
        }
        m_base.IsNegative(!m_base.IsNegative());
    }

    return true;
}

bool CalcInput::TryAddDigit(unsigned int value, uint32_t radix, bool isIntegerMode, wstring_view maxNumStr, uint32_t wordBitWidth, int maxDigits)
{
    // Convert from an integer into a character
    // This includes both normal digits and alpha 'digits' for radixes > 10
    auto chDigit = static_cast<wchar_t>((value < 10) ? (L'0' + value) : (L'A' + value - 10));

    CalcNumSec* pNumSec;
    size_t maxCount;
    if (m_hasExponent)
    {
        pNumSec = &m_exponent;
        maxCount = C_EXP_MAX_DIGITS;
    }
    else
    {
        pNumSec = &m_base;
        maxCount = maxDigits;
        // Don't include the decimal point in the count. In that way you can enter the maximum allowed precision.
        // Precision doesn't include decimal point.
        if (HasDecimalPt())
        {
            maxCount++;
        }
        // First leading 0 is not counted in input restriction as the output can be of that form
        // See NumberToString algorithm. REVIEW: We don't have such input restriction mimicking based on output of NumberToString for exponent
        // NumberToString can give 10 digit exponent, but we still restrict the exponent here to be only 4 digits.
        if (!pNumSec->IsEmpty() && pNumSec->value.front() == L'0')
        {
            maxCount++;
        }
    }

    // Ignore leading zeros
    if (pNumSec->IsEmpty() && (value == 0))
    {
        return true;
    }

    if (pNumSec->value.size() < maxCount)
    {
        pNumSec->value += chDigit;
        return true;
    }

    // if we are in integer mode, within the base, and we're on the last digit then
    // there are special cases where we can actually add one more digit.
    if (isIntegerMode && pNumSec->value.size() == maxCount && !m_hasExponent)
    {
        bool allowExtraDigit = false;

        if (radix == 8)
        {
            switch (wordBitWidth % 3)
            {
            case 1:
                // in 16 or 64bit word size, if the first digit is a 1 we can enter 6 (16bit) or 22 (64bit) digits
                allowExtraDigit = (pNumSec->value.front() == L'1');
                break;

            case 2:
                // in 8 or 32bit word size, if the first digit is a 3 or less we can enter 3 (8bit) or 11 (32bit) digits
                allowExtraDigit = (pNumSec->value.front() <= L'3');
                break;
            }
        }
        else if (radix == 10)
        {
            // If value length is at least the max, we know we can't add another digit.
            if (pNumSec->value.size() < maxNumStr.size())
            {
                // Compare value to substring of maxNumStr of value.size() length.
                // If cmpResult > 0:
                // eg. max is "127", and the current number is "20". first digit itself says we are out.
                // Additional digit is not possible

                // If cmpResult < 0:
                // Success case. eg. max is "127", and current number is say "11". The second digit '1' being <
                // corresponding digit '2', means all digits are possible to append, like 119 will still be < 127

                // If cmpResult == 0:
                // Undecided still. The case when max is "127", and current number is "12". Look for the new number being 7 or less to allow
                auto cmpResult = pNumSec->value.compare(0, wstring::npos, maxNumStr, 0, pNumSec->value.size());
                if (cmpResult < 0)
                {
                    allowExtraDigit = true;
                }
                else if (cmpResult == 0)
                {
                    auto lastChar = maxNumStr[pNumSec->value.size()];
                    if (chDigit <= lastChar)
                    {
                        allowExtraDigit = true;
                    }
                    else if (pNumSec->IsNegative() && chDigit <= lastChar + 1)
                    {
                        // Negative value case, eg. max is "127", and current number is "-12". Then 8 is also valid, as the range
                        // is always from -(max+1)...max in signed mode
                        allowExtraDigit = true;
                    }
                }
            }
        }

        if (allowExtraDigit)
        {
            pNumSec->value += chDigit;
            return true;
        }
    }

    return false;
}

bool CalcInput::TryAddDecimalPt()
{
    // Already have a decimal pt or we're in the exponent
    if (m_hasDecimal || m_hasExponent)
    {
        return false;
    }

    if (m_base.IsEmpty())
    {
        m_base.value += L'0'; // Add a leading zero
    }

    m_decPtIndex = m_base.value.size();
    m_base.value += m_decSymbol;
    m_hasDecimal = true;

    return true;
}

bool CalcInput::HasDecimalPt()
{
    return m_hasDecimal;
}

bool CalcInput::TryBeginExponent()
{
    // For compatibility, add a trailing dec point to base num if it doesn't have one
    TryAddDecimalPt();

    if (m_hasExponent) // Already entering exponent
    {
        return false;
    }

    m_hasExponent = true; // Entering exponent
    return true;
}

void CalcInput::Backspace()
{
    if (m_hasExponent)
    {
        if (!m_exponent.IsEmpty())
        {
            m_exponent.value.pop_back();

            if (m_exponent.IsEmpty())
            {
                m_exponent.Clear();
            }
        }
        else
        {
            m_hasExponent = false;
        }
    }
    else
    {
        if (!m_base.IsEmpty())
        {
            m_base.value.pop_back();
            if (m_base.value == L"0")
            {
                m_base.value.pop_back();
            }
        }

        if (m_base.value.size() <= m_decPtIndex)
        {
            // Backed up over decimal point
            m_hasDecimal = false;
            m_decPtIndex = 0;
        }

        if (m_base.IsEmpty())
        {
            m_base.Clear();
        }
    }
}

void CalcInput::SetDecimalSymbol(wchar_t decSymbol)
{
    if (m_decSymbol != decSymbol)
    {
        m_decSymbol = decSymbol;

        if (m_hasDecimal)
        {
            // Change to new decimal pt
            m_base.value[m_decPtIndex] = m_decSymbol;
        }
    }
}

bool CalcInput::IsEmpty()
{
    return m_base.IsEmpty() && !m_hasExponent && m_exponent.IsEmpty() && !m_hasDecimal;
}

wstring CalcInput::ToString(uint32_t radix)
{
    // In theory both the base and exponent could be C_NUM_MAX_DIGITS long.
    if ((m_base.value.size() > MAX_STRLEN) || (m_hasExponent && m_exponent.value.size() > MAX_STRLEN))
    {
        return wstring();
    }

    wstring result;

    if (m_base.IsNegative())
    {
        result = L'-';
    }

    if (m_base.IsEmpty())
    {
        result += L'0';
    }
    else
    {
        result += m_base.value;
    }

    if (m_hasExponent)
    {
        // Add a decimal point if it is not already there
        if (!m_hasDecimal)
        {
            result += m_decSymbol;
        }

        result += ((radix == 10) ? L'e' : L'^');
        result += (m_exponent.IsNegative() ? L'-' : L'+');

        if (m_exponent.IsEmpty())
        {
            result += L'0';
        }
        else
        {
            result += m_exponent.value;
        }
    }

    // Base and Exp can each be up to C_NUM_MAX_DIGITS in length, plus 4 characters for sign, dec, exp, and expSign.
    if (result.size() > C_NUM_MAX_DIGITS * 2 + 4)
    {
        return wstring();
    }

    return result;
}

Rational CalcInput::ToRational(uint32_t radix, int32_t precision)
{
    PRAT rat = StringToRat(m_base.IsNegative(), m_base.value, m_exponent.IsNegative(), m_exponent.value, radix, precision);
    if (rat == nullptr)
    {
        return 0;
    }

    Rational result{ rat };
    destroyrat(rat);

    return result;
}
