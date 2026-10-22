// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static void mulnumx(ref Number pa, Number b)
        {
            if (b.CDigit > 1 || b.Mant[0] != 1 || b.Exp != 0)
            {
                if (pa.CDigit > 1 || pa.Mant[0] != 1 || pa.Exp != 0)
                {
                    _mulnumx(ref pa, b);
                }
                else
                {
                    int sign = pa.Sign;
                    pa = b.Clone();
                    pa.Sign *= sign;
                }
            }
            else
            {
                pa.Sign *= b.Sign;
            }
        }

        public static void _mulnumx(ref Number pa, Number b)
        {
            Number a = pa;
            int ibdigit = a.CDigit + b.CDigit - 1;
            Number c = new Number(ibdigit + 1);
            c.CDigit = ibdigit;
            c.Sign = a.Sign * b.Sign;
            c.Exp = a.Exp + b.Exp;

            int ptraIdx = 0;
            int ptrcoffset = 0;

            for (int iadigit = a.CDigit; iadigit > 0; iadigit--)
            {
                uint da = a.Mant[ptraIdx++];
                int ptrbIdx = 0;
                int ptrc = ptrcoffset++;

                for (int jbdigit = b.CDigit; jbdigit > 0; jbdigit--)
                {
                    ulong cy = 0;
                    ulong mcy = (ulong)da * b.Mant[ptrbIdx];
                    int icdigit = 0;
                    if (mcy != 0)
                    {
                        if (jbdigit == 1 && iadigit == 1)
                        {
                            c.CDigit++;
                        }
                    }

                    while (mcy != 0 || cy != 0)
                    {
                        int targetIdx = ptrc + icdigit;
                        if (targetIdx >= c.Mant.Length)
                        {
                            Array.Resize(ref c.Mant, targetIdx + 8);
                        }
                        cy += (ulong)c.Mant[targetIdx] + (mcy & (~BASEX));
                        c.Mant[targetIdx] = (uint)(cy & (~BASEX));
                        icdigit++;

                        mcy >>= (int)BASEXPWR;
                        cy >>= (int)BASEXPWR;
                    }

                    ptrbIdx++;
                    ptrc++;
                }
            }

            while (c.CDigit > 1 && c.Mant[c.CDigit - 1] == 0)
            {
                c.CDigit--;
            }

            pa = c;
        }

        public static void numpowi32x(ref Number proot, int power)
        {
            Number lret = i32tonum(1, BASEX);

            while (power > 0)
            {
                if ((power & 1) != 0)
                {
                    mulnumx(ref lret, proot);
                }

                mulnumx(ref proot, proot);
                power >>= 1;
            }

            proot = lret;
        }

        public static void divnumx(ref Number pa, Number b, int precision)
        {
            if (b.CDigit > 1 || b.Mant[0] != 1 || b.Exp != 0)
            {
                if (pa.CDigit > 1 || pa.Mant[0] != 1 || pa.Exp != 0)
                {
                    _divnumx(ref pa, b, precision);
                }
                else
                {
                    int sign = pa.Sign;
                    pa = b.Clone();
                    pa.Sign *= sign;
                }
            }
            else
            {
                pa.Sign *= b.Sign;
            }
        }

        public static void _divnumx(ref Number pa, Number b, int precision)
        {
            Number a = pa;
            int thismax = precision + g_ratio;

            if (thismax < a.CDigit)
            {
                thismax = a.CDigit;
            }
            if (thismax < b.CDigit)
            {
                thismax = b.CDigit;
            }

            Number c = new Number(thismax + 1);
            c.Exp = (a.CDigit + a.Exp) - (b.CDigit + b.Exp) + 1;
            c.Sign = a.Sign * b.Sign;

            int ptrc = thismax;
            int cdigits = 0;

            Number rem = a.Clone();
            rem.Sign = b.Sign;
            rem.Exp = b.CDigit + b.Exp - rem.CDigit;

            while (cdigits++ < thismax && !zernum(rem))
            {
                uint digit = 0;
                c.Mant[ptrc] = 0;
                while (!lessnum(rem, b))
                {
                    uint bitDigit = 1;
                    Number tmp = b.Clone();
                    Number lasttmp = i32tonum(0, BASEX);

                    while (lessnum(tmp, rem))
                    {
                        lasttmp = tmp.Clone();
                        addnum(ref tmp, tmp, BASEX);
                        bitDigit *= 2;
                    }

                    if (lessnum(rem, tmp))
                    {
                        tmp = lasttmp;
                        bitDigit /= 2;
                        lasttmp = null;
                    }

                    tmp.Sign *= -1;
                    addnum(ref rem, tmp, BASEX);
                    digit |= bitDigit;
                }

                rem.Exp++;
                c.Mant[ptrc--] = digit;
            }
            cdigits--;

            int startSrc = ptrc + 1;
            if (startSrc != 0 && cdigits > 0)
            {
                Array.Copy(c.Mant, startSrc, c.Mant, 0, cdigits);
            }

            if (cdigits == 0)
            {
                c.Exp = 0;
                c.CDigit = 1;
            }
            else
            {
                c.CDigit = cdigits;
                c.Exp -= cdigits;
                while (c.CDigit > 1 && c.Mant[c.CDigit - 1] == 0)
                {
                    c.CDigit--;
                }
            }

            pa = c;
        }
    }
}
