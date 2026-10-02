// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        private const int FUNC_AND = 0;
        private const int FUNC_OR = 1;
        private const int FUNC_XOR = 2;

        public static void lshrat(ref Rat pa, Rat b, uint radix, int precision)
        {
            intrat(ref pa, radix, precision);
            if (!zernum(pa.P))
            {
                if (rat_gt(b, rat_max_exp, precision))
                {
                    throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
                }
                int intb = rattoi32(b, radix, precision);
                Rat pwr = rat_two.Clone();
                ratpowi32(ref pwr, intb, precision);
                mulrat(ref pa, pwr, precision);
            }
        }

        public static void rshrat(ref Rat pa, Rat b, uint radix, int precision)
        {
            intrat(ref pa, radix, precision);
            if (!zernum(pa.P))
            {
                if (rat_lt(b, rat_min_exp, precision))
                {
                    throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
                }
                int intb = rattoi32(b, radix, precision);
                Rat pwr = rat_two.Clone();
                ratpowi32(ref pwr, intb, precision);
                divrat(ref pa, pwr, precision);
            }
        }

        public static void andrat(ref Rat pa, Rat b, uint radix, int precision)
        {
            boolrat(ref pa, b, FUNC_AND, radix, precision);
        }

        public static void orrat(ref Rat pa, Rat b, uint radix, int precision)
        {
            boolrat(ref pa, b, FUNC_OR, radix, precision);
        }

        public static void xorrat(ref Rat pa, Rat b, uint radix, int precision)
        {
            boolrat(ref pa, b, FUNC_XOR, radix, precision);
        }

        private static void boolrat(ref Rat pa, Rat b, int func, uint radix, int precision)
        {
            intrat(ref pa, radix, precision);
            Rat tmp = b.Clone();
            intrat(ref tmp, radix, precision);

            boolnum(ref pa.P, tmp.P, func);
        }

        private static void boolnum(ref Number pa, Number b, int func)
        {
            Number a = pa;
            int cdigits = Math.Max(a.CDigit + a.Exp, b.CDigit + b.Exp) - Math.Min(a.Exp, b.Exp);
            Number c = new Number(cdigits)
            {
                Exp = Math.Min(a.Exp, b.Exp)
            };
            int mexp = c.Exp;
            c.CDigit = cdigits;

            int pcha = 0;
            int pchb = 0;
            int pchc = 0;

            for (; cdigits > 0; cdigits--, mexp++)
            {
                uint da = ((mexp >= a.Exp) && (cdigits + a.Exp - c.Exp > (c.CDigit - a.CDigit))) ? (pcha < a.Mant.Length ? a.Mant[pcha++] : 0) : 0;
                uint db = ((mexp >= b.Exp) && (cdigits + b.Exp - c.Exp > (c.CDigit - b.CDigit))) ? (pchb < b.Mant.Length ? b.Mant[pchb++] : 0) : 0;

                switch (func)
                {
                    case FUNC_AND:
                        c.Mant[pchc++] = da & db;
                        break;
                    case FUNC_OR:
                        c.Mant[pchc++] = da | db;
                        break;
                    case FUNC_XOR:
                        c.Mant[pchc++] = da ^ db;
                        break;
                }
            }

            c.Sign = a.Sign;
            while (c.CDigit > 1 && c.Mant[c.CDigit - 1] == 0)
            {
                c.CDigit--;
            }

            pa = c;
        }

        public static void remrat(ref Rat pa, Rat b)
        {
            if (zerrat(b))
            {
                throw new Exception(CalcErr.CALC_E_INDEFINITE.ToString());
            }

            Rat tmp = b.Clone();

            mulnumx(ref pa.P, tmp.Q);
            mulnumx(ref tmp.P, pa.Q);
            remnum(ref pa.P, tmp.P, BASEX);
            mulnumx(ref pa.Q, tmp.Q);

            RENORMALIZE(pa);
        }

        public static void modrat(ref Rat pa, Rat b)
        {
            if (zerrat(b))
            {
                return;
            }

            Rat tmp = b.Clone();
            bool needAdjust = (SIGN(pa) == -1 ? (SIGN(b) == 1) : (SIGN(b) == -1));

            mulnumx(ref pa.P, tmp.Q);
            mulnumx(ref tmp.P, pa.Q);
            remnum(ref pa.P, tmp.P, BASEX);
            mulnumx(ref pa.Q, tmp.Q);

            if (needAdjust && !zerrat(pa))
            {
                _addrat(ref pa, b, unchecked((int)BASEX));
            }

            RENORMALIZE(pa);
        }
    }
}
