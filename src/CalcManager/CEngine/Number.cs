// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using CalcManager.Ratpack;

namespace CalcManager.CEngine
{
    public class Number
    {
        private int m_sign;
        private int m_exp;
        private List<uint> m_mantissa;

        public Number()
            : this(1, 0, new List<uint> { 0 })
        {
        }

        public Number(int sign, int exp, List<uint> mantissa)
        {
            m_sign = sign;
            m_exp = exp;
            m_mantissa = mantissa != null ? new List<uint>(mantissa) : new List<uint> { 0 };
        }

        public Number(int sign, int exp, params uint[] mantissa)
        {
            m_sign = sign;
            m_exp = exp;
            m_mantissa = mantissa != null ? new List<uint>(mantissa) : new List<uint> { 0 };
        }

        public Number(Ratpack.Number p)
        {
            if (p != null)
            {
                m_sign = p.Sign;
                m_exp = p.Exp;
                m_mantissa = new List<uint>(p.CDigit);
                for (int i = 0; i < p.CDigit; i++)
                {
                    m_mantissa.Add(p.Mant[i]);
                }
            }
            else
            {
                m_sign = 1;
                m_exp = 0;
                m_mantissa = new List<uint> { 0 };
            }
        }

        public Ratpack.Number ToPNUMBER()
        {
            Ratpack.Number ret = new Ratpack.Number(m_mantissa.Count + 1)
            {
                Sign = m_sign,
                Exp = m_exp,
                CDigit = m_mantissa.Count
            };

            for (int i = 0; i < m_mantissa.Count; i++)
            {
                ret.Mant[i] = m_mantissa[i];
            }

            return ret;
        }

        public int Sign => m_sign;
        public int Exp => m_exp;
        public List<uint> Mantissa => m_mantissa;

        public bool IsZero()
        {
            return m_mantissa.All(i => i == 0);
        }
    }
}
