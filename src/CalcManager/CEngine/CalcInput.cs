// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Text;
using CalcManager.Ratpack;

namespace CalcManager.CEngine
{
    public class CalcNumSec
    {
        public string Value { get; set; } = string.Empty;
        public bool IsNegative { get; set; }

        public void Clear()
        {
            Value = string.Empty;
            IsNegative = false;
        }

        public bool IsEmpty => string.IsNullOrEmpty(Value);
    }

    public class CalcInput
    {
        public const int MAX_STRLEN = 84;
        private const int C_NUM_MAX_DIGITS = MAX_STRLEN;
        private const int C_EXP_MAX_DIGITS = 4;

        private bool m_hasExponent;
        private bool m_hasDecimal;
        private int m_decPtIndex;
        private char m_decSymbol;
        private readonly CalcNumSec m_base;
        private readonly CalcNumSec m_exponent;

        public CalcInput()
            : this('.')
        {
        }

        public CalcInput(char decSymbol)
        {
            m_hasExponent = false;
            m_hasDecimal = false;
            m_decPtIndex = 0;
            m_decSymbol = decSymbol;
            m_base = new CalcNumSec();
            m_exponent = new CalcNumSec();
        }

        public void Clear()
        {
            m_base.Clear();
            m_exponent.Clear();
            m_hasExponent = false;
            m_hasDecimal = false;
            m_decPtIndex = 0;
        }

        public bool TryToggleSign(bool isIntegerMode, string maxNumStr)
        {
            if (m_base.IsEmpty)
            {
                m_base.IsNegative = false;
                m_exponent.IsNegative = false;
            }
            else if (m_hasExponent)
            {
                m_exponent.IsNegative = !m_exponent.IsNegative;
            }
            else
            {
                if (isIntegerMode && m_base.IsNegative)
                {
                    if (!string.IsNullOrEmpty(maxNumStr) && m_base.Value.Length >= maxNumStr.Length &&
                        m_base.Value[m_base.Value.Length - 1] > maxNumStr[maxNumStr.Length - 1])
                    {
                        return false;
                    }
                }
                m_base.IsNegative = !m_base.IsNegative;
            }

            return true;
        }

        public bool TryAddDigit(uint value, uint radix, bool isIntegerMode, string maxNumStr, int wordBitWidth, int maxDigits)
        {
            char chDigit = (value < 10) ? (char)('0' + value) : (char)('A' + value - 10);

            CalcNumSec pNumSec;
            int maxCount;
            if (m_hasExponent)
            {
                pNumSec = m_exponent;
                maxCount = C_EXP_MAX_DIGITS;
            }
            else
            {
                pNumSec = m_base;
                maxCount = maxDigits;
                if (HasDecimalPt())
                {
                    maxCount++;
                }
                if (!pNumSec.IsEmpty && pNumSec.Value[0] == '0')
                {
                    maxCount++;
                }
            }

            if (pNumSec.IsEmpty && (value == 0))
            {
                return true;
            }

            if (pNumSec.Value.Length < maxCount)
            {
                pNumSec.Value += chDigit;
                return true;
            }

            if (isIntegerMode && pNumSec.Value.Length == maxCount && !m_hasExponent)
            {
                bool allowExtraDigit = false;

                if (radix == 8)
                {
                    switch (wordBitWidth % 3)
                    {
                        case 1:
                            allowExtraDigit = (pNumSec.Value[0] == '1');
                            break;
                        case 2:
                            allowExtraDigit = (pNumSec.Value[0] <= '3');
                            break;
                    }
                }
                else if (radix == 10 && !string.IsNullOrEmpty(maxNumStr))
                {
                    if (pNumSec.Value.Length < maxNumStr.Length)
                    {
                        int cmpResult = string.CompareOrdinal(pNumSec.Value, 0, maxNumStr, 0, pNumSec.Value.Length);
                        if (cmpResult < 0)
                        {
                            allowExtraDigit = true;
                        }
                        else if (cmpResult == 0)
                        {
                            char lastChar = maxNumStr[pNumSec.Value.Length];
                            if (chDigit <= lastChar)
                            {
                                allowExtraDigit = true;
                            }
                            else if (pNumSec.IsNegative && chDigit <= (char)(lastChar + 1))
                            {
                                allowExtraDigit = true;
                            }
                        }
                    }
                }

                if (allowExtraDigit)
                {
                    pNumSec.Value += chDigit;
                    return true;
                }
            }

            return false;
        }

        public bool TryAddDecimalPt()
        {
            if (m_hasDecimal || m_hasExponent)
            {
                return false;
            }

            if (m_base.IsEmpty)
            {
                m_base.Value += '0';
            }

            m_decPtIndex = m_base.Value.Length;
            m_base.Value += m_decSymbol;
            m_hasDecimal = true;

            return true;
        }

        public bool HasDecimalPt()
        {
            return m_hasDecimal;
        }

        public bool TryBeginExponent()
        {
            TryAddDecimalPt();

            if (m_hasExponent)
            {
                return false;
            }

            m_hasExponent = true;
            return true;
        }

        public void Backspace()
        {
            if (m_hasExponent)
            {
                if (!m_exponent.IsEmpty)
                {
                    m_exponent.Value = m_exponent.Value.Substring(0, m_exponent.Value.Length - 1);
                    if (m_exponent.IsEmpty)
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
                if (!m_base.IsEmpty)
                {
                    m_base.Value = m_base.Value.Substring(0, m_base.Value.Length - 1);
                    if (m_base.Value == "0")
                    {
                        m_base.Value = string.Empty;
                    }
                }

                if (m_base.Value.Length <= m_decPtIndex)
                {
                    m_hasDecimal = false;
                    m_decPtIndex = 0;
                }

                if (m_base.IsEmpty)
                {
                    m_base.Clear();
                }
            }
        }

        public void SetDecimalSymbol(char decSymbol)
        {
            if (m_decSymbol != decSymbol)
            {
                m_decSymbol = decSymbol;

                if (m_hasDecimal && m_decPtIndex < m_base.Value.Length)
                {
                    char[] chars = m_base.Value.ToCharArray();
                    chars[m_decPtIndex] = m_decSymbol;
                    m_base.Value = new string(chars);
                }
            }
        }

        public bool IsEmpty()
        {
            return m_base.IsEmpty && !m_hasExponent && m_exponent.IsEmpty && !m_hasDecimal;
        }

        public string ToString(uint radix)
        {
            if ((m_base.Value.Length > MAX_STRLEN) || (m_hasExponent && m_exponent.Value.Length > MAX_STRLEN))
            {
                return string.Empty;
            }

            var sb = new StringBuilder();

            if (m_base.IsNegative)
            {
                sb.Append('-');
            }

            if (m_base.IsEmpty)
            {
                sb.Append('0');
            }
            else
            {
                sb.Append(m_base.Value);
            }

            if (m_hasExponent)
            {
                if (!m_hasDecimal)
                {
                    sb.Append(m_decSymbol);
                }

                sb.Append((radix == 10) ? 'e' : '^');
                sb.Append(m_exponent.IsNegative ? '-' : '+');

                if (m_exponent.IsEmpty)
                {
                    sb.Append('0');
                }
                else
                {
                    sb.Append(m_exponent.Value);
                }
            }

            if (sb.Length > C_NUM_MAX_DIGITS * 2 + 4)
            {
                return string.Empty;
            }

            return sb.ToString();
        }

        public Rational ToRational(uint radix, int precision)
        {
            Rat rat = Ratpak.StringToRat(m_base.IsNegative, m_base.Value, m_exponent.IsNegative, m_exponent.Value, radix, precision);
            if (rat == null)
            {
                return 0;
            }

            return new Rational(rat);
        }
    }
}
