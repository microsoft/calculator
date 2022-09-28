// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Rational.h"

// Space to hold enough digits for a quadword binary number (64) plus digit separator strings for that number (20)
constexpr int MAX_STRLEN = 84;

namespace CalcEngine
{
    class CalcNumSec
    {
    public:
        CalcNumSec()
            : value()
            , m_isNegative(false)
        {
        }

        void Clear();
        bool IsEmpty()
        {
            return value.empty();
        }

        bool IsNegative()
        {
            return m_isNegative;
        }
        void IsNegative(bool isNegative)
        {
            m_isNegative = isNegative;
        }

        std::wstring value;

    private:
        bool m_isNegative;
    };

    class CalcInput
    {
    public:
        CalcInput()
            : CalcInput(L'.')
        {
        }

        CalcInput(wchar_t decSymbol)
            : m_hasExponent(false)
            , m_hasDecimal(false)
            , m_decPtIndex(0)
            , m_decSymbol(decSymbol)
            , m_base()
            , m_exponent()
        {
        }

        void Clear();
        bool TryToggleSign(bool isIntegerMode, std::wstring_view maxNumStr);
        bool TryAddDigit(unsigned int value, uint32_t radix, bool isIntegerMode, std::wstring_view maxNumStr, uint32_t wordBitWidth, int maxDigits);
        bool TryAddDecimalPt();
        bool HasDecimalPt();
        bool TryBeginExponent();
        void Backspace();
        void SetDecimalSymbol(wchar_t decSymbol);
        bool IsEmpty();
        std::wstring ToString(uint32_t radix);
        Rational ToRational(uint32_t radix, int32_t precision);

    private:
        bool m_hasExponent;
        bool m_hasDecimal;
        size_t m_decPtIndex;
        wchar_t m_decSymbol;
        CalcNumSec m_base;
        CalcNumSec m_exponent;
    };
}
