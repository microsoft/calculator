// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Header Files/CalcEngine.h"

using namespace CalcEngine;
using namespace CalcEngine::RationalMath;
using namespace std;

// To be called when either the radix or num width changes. You can use -1 in either of these values to mean
// dont change that.
void CCalcEngine::SetRadixTypeAndNumWidth(RadixType radixtype, NUM_WIDTH numwidth)
{
    // When in integer mode, the number is represented in 2's complement form. When a bit width is changing, we can
    // change the number representation back to sign, abs num form in ratpak. Soon when display sees this, it will
    // convert to 2's complement form, but this time all high bits will be propagated. Eg. -127, in byte mode is
    // represented as 1000,0001. This puts it back as sign=-1, 01111111 . But DisplayNum will see this and convert it
    // back to 1111,1111,1000,0001 when in Word mode.
    if (m_fIntegerMode)
    {
        uint64_t w64Bits = m_currentVal.ToUInt64_t();
        bool fMsb = (w64Bits >> (m_dwWordBitWidth - 1)) & 1; // make sure you use the old width

        if (fMsb)
        {
            // If high bit is set, then get the decimal number in -ve 2'scompl form.
            auto tempResult = m_currentVal ^ GetChopNumber();

            m_currentVal = -(tempResult + 1);
        }
    }

    if (radixtype >= RadixType::Hex && radixtype <= RadixType::Binary)
    {
        m_radix = NRadixFromRadixType(radixtype);
        // radixtype is not even saved
    }

    if (numwidth >= NUM_WIDTH::QWORD_WIDTH && numwidth <= NUM_WIDTH::BYTE_WIDTH)
    {
        m_numwidth = numwidth;
        m_dwWordBitWidth = DwWordBitWidthFromNumWidth(numwidth);
    }

    // inform ratpak that a change in base or precision has occurred
    BaseOrPrecisionChanged();

    // display the correct number for the new state (ie convert displayed
    //  number to correct base)
    DisplayNum();
}

uint32_t CCalcEngine::DwWordBitWidthFromNumWidth(NUM_WIDTH numwidth)
{
    switch (numwidth)
    {
    case NUM_WIDTH::DWORD_WIDTH:
        return 32;
    case NUM_WIDTH::WORD_WIDTH:
        return 16;
    case NUM_WIDTH::BYTE_WIDTH:
        return 8;
    case NUM_WIDTH::QWORD_WIDTH:
    default:
        return 64;
    }
}

uint32_t CCalcEngine::NRadixFromRadixType(RadixType radixtype)
{
    switch (radixtype)
    {
    case RadixType::Hex:
        return 16;
    case RadixType::Octal:
        return 8;
    case RadixType::Binary:
        return 2;
    case RadixType::Decimal:
    default:
        return 10;
    }
}

//  Toggles a given bit into the number representation. returns true if it changed it actually.
bool CCalcEngine::TryToggleBit(CalcEngine::Rational& rat, uint32_t wbitno)
{
    uint32_t wmax = DwWordBitWidthFromNumWidth(m_numwidth);
    if (wbitno >= wmax)
    {
        return false; // ignore error cant happen
    }

    Rational result = Integer(rat);

    // Remove any variance in how 0 could be represented in rat e.g. -0, 0/n, etc.
    result = (result != 0 ? result : 0);

    // XOR the result with 2^wbitno power
    rat = result ^ Pow(2, static_cast<int32_t>(wbitno));

    return true;
}

// Returns the nearest power of two
unsigned int CCalcEngine::QuickLog2(unsigned int iNum)
{
    // Nearest power of 2 to 0 is 1
    if (!iNum)
        return 1;

    // Count the number of leading 0's of iNum - 1,
    // and then shift 1 by that amount
    return 1U << (32 - __builtin_clz(iNum - 1));
}

////////////////////////////////////////////////////////////////////////
//
//  UpdateMaxIntDigits
//
// determine the maximum number of digits needed for the current precision,
// word size, and base.  This number is conservative towards the small side
// such that there may be some extra bits left over. For example, base 8 requires 3 bits per digit.
// A word size of 32 bits allows for 10 digits with a remainder of two bits.  Bases
// that require variable number of bits (non-power-of-two bases) are approximated
// by the next highest power-of-two base (again, to be conservative and guarantee
// there will be no over flow verse the current word size for numbers entered).
// Base 10 is a special case and always uses the base 10 precision (m_nPrecisionSav).
void CCalcEngine::UpdateMaxIntDigits()
{
    if (m_radix == 10)
    {
        // if in integer mode you still have to honor the max digits you can enter based on bit width
        if (m_fIntegerMode)
        {
            m_cIntDigitsSav = static_cast<int>(GetMaxDecimalValueString().length()) - 1;
            // This is the max digits you can enter a decimal in fixed width mode aka integer mode -1. The last digit
            // has to be checked separately
        }
        else
        {
            m_cIntDigitsSav = m_precision;
        }
    }
    else
    {
        m_cIntDigitsSav = m_dwWordBitWidth / CCalcEngine::QuickLog2(m_radix);
    }
}

void CCalcEngine::ChangeBaseConstants(uint32_t radix, int maxIntDigits, int32_t precision)
{
    if (10 == radix)
    {
        ChangeConstants(radix, precision); // Base 10 precision for internal computing still needs to be 32, to
        // take care of decimals precisely. For eg. to get the HI word of a qword, we do a rsh, which depends on getting
        // 18446744073709551615 / 4294967296 = 4294967295.9999917... This is important it works this and doesn't reduce
        // the precision to number of digits allowed to enter. In other words, precision and # of allowed digits to be
        // entered are different.
    }
    else
    {
        ChangeConstants(radix, maxIntDigits + 1);
    }
}

void CCalcEngine::BaseOrPrecisionChanged()
{
    UpdateMaxIntDigits();
    CCalcEngine::ChangeBaseConstants(m_radix, m_cIntDigitsSav, m_precision);
}
