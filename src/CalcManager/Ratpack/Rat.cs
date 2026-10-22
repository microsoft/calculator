// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static void gcdrat(ref Rat pa, int precision)
        {
            Rat a = pa;
            Number pgcd = gcd(a.P, a.Q);

            if (!zernum(pgcd))
            {
                divnumx(ref a.P, pgcd, precision);
                divnumx(ref a.Q, pgcd, precision);
            }

            pa = a;
            RENORMALIZE(pa);
        }

        public static void fracrat(ref Rat pa, uint radix, int precision)
        {
            if (!zernum(pa.P) && !equnum(pa.Q, num_one))
            {
                flatrat(ref pa, radix, precision);
            }

            remnum(ref pa.P, pa.Q, BASEX);
            RENORMALIZE(pa);
        }

        public static void mulrat(ref Rat pa, Rat b, int precision)
        {
            if (!zernum(pa.P))
            {
                mulnumx(ref pa.P, b.P);
                mulnumx(ref pa.Q, b.Q);
                trimit(ref pa, precision);
            }
            else
            {
                pa.Q = num_one.Clone();
            }
        }

        public static void divrat(ref Rat pa, Rat b, int precision)
        {
            if (!zernum(pa.P))
            {
                mulnumx(ref pa.P, b.Q);
                mulnumx(ref pa.Q, b.P);

                if (zernum(pa.Q))
                {
                    throw new Exception(CalcErr.CALC_E_DIVIDEBYZERO.ToString());
                }
                trimit(ref pa, precision);
            }
            else
            {
                if (zerrat(b))
                {
                    throw new Exception(CalcErr.CALC_E_INDEFINITE.ToString());
                }
                else
                {
                    pa.Q = num_one.Clone();
                }
            }
        }

        public static void subrat(ref Rat pa, Rat b, int precision)
        {
            Rat a = pa.Clone();
            _subrat(ref pa, b, precision);
            _snaprat(ref pa, a, b, precision);
        }

        public static void _subrat(ref Rat pa, Rat b, int precision)
        {
            b.P.Sign *= -1;
            _addrat(ref pa, b, precision);
            b.P.Sign *= -1;
        }

        public static void addrat(ref Rat pa, Rat b, int precision)
        {
            Rat a = pa.Clone();
            _addrat(ref pa, b, precision);
            _snaprat(ref pa, a, b, precision);
        }

        public static void _addrat(ref Rat pa, Rat b, int precision)
        {
            if (equnum(pa.Q, b.Q))
            {
                pa.P.Sign *= pa.Q.Sign;
                pa.Q.Sign = 1;
                b.P.Sign *= b.Q.Sign;
                b.Q.Sign = 1;
                addnum(ref pa.P, b.P, BASEX);
            }
            else
            {
                Number bot = pa.Q.Clone();
                mulnumx(ref bot, b.Q);
                mulnumx(ref pa.P, b.Q);
                mulnumx(ref pa.Q, b.P);
                addnum(ref pa.P, pa.Q, BASEX);
                pa.Q = bot;
                trimit(ref pa, precision);

                pa.P.Sign *= pa.Q.Sign;
                pa.Q.Sign = 1;
            }
        }

        public static void rootrat(ref Rat py, Rat n, uint radix, int precision)
        {
            Rat oneovern = rat_one.Clone();
            divrat(ref oneovern, n, precision);
            powrat(ref py, oneovern, radix, precision);
        }

        public static bool zerrat(Rat a)
        {
            return zernum(a?.P);
        }

        public static void _snaprat(ref Rat pr, Rat a, Rat b, int precision)
        {
            Rat threshold = null;
            if (b == null)
            {
                threshold = a.Clone();
                ABSRAT(threshold);
            }
            else
            {
                Rat absA = a.Clone();
                Rat absB = b.Clone();
                ABSRAT(absA);
                ABSRAT(absB);

                if (rat_lt(absA, absB, precision))
                {
                    threshold = absB.Clone();
                }
                else
                {
                    threshold = absA.Clone();
                }
            }

            mulrat(ref threshold, rat_smallest, precision);

            Rat absR = pr.Clone();
            ABSRAT(absR);

            if (rat_lt(absR, threshold, precision))
            {
                pr = rat_zero.Clone();
            }
        }
    }
}
