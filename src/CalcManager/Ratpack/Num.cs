// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        public static uint MSD(Number x)
        {
            return x.Mant[x.CDigit - 1];
        }

        public static void addnum(ref Number pa, Number b, uint radix)
        {
            if (b.CDigit > 1 || b.Mant[0] != 0)
            {
                // If b is zero we are done.
                if (pa.CDigit > 1 || pa.Mant[0] != 0)
                {
                    // pa and b are both nonzero.
                    _addnum(ref pa, b, radix);
                }
                else
                {
                    // if pa is zero and b isn't just copy b.
                    pa = b.Clone();
                }
            }
        }

        public static void _addnum(ref Number pa, Number b, uint radix)
        {
            Number a = pa;

            // Calculate the overlap of the numbers after alignment, this includes necessary padding 0's
            int cdigits = Math.Max(a.CDigit + a.Exp, b.CDigit + b.Exp) - Math.Min(a.Exp, b.Exp);

            Number c = new Number(cdigits + 1);
            c.Exp = Math.Min(a.Exp, b.Exp);
            int mexp = c.Exp;
            c.CDigit = cdigits;

            int pchaIdx = 0;
            int pchbIdx = 0;
            int pchcIdx = 0;

            uint cy = 0;
            bool fcompla = false;
            bool fcomplb = false;

            if (a.Sign != b.Sign)
            {
                cy = 1;
                fcompla = (a.Sign == -1);
                fcomplb = (b.Sign == -1);
            }

            for (; cdigits > 0; cdigits--, mexp++)
            {
                uint da = ((mexp >= a.Exp) && (cdigits + a.Exp - c.Exp > (c.CDigit - a.CDigit))) ? (pchaIdx < a.Mant.Length ? a.Mant[pchaIdx++] : 0) : 0;
                uint db = ((mexp >= b.Exp) && (cdigits + b.Exp - c.Exp > (c.CDigit - b.CDigit))) ? (pchbIdx < b.Mant.Length ? b.Mant[pchbIdx++] : 0) : 0;

                if (fcompla)
                {
                    da = (radix - 1) - da;
                }
                if (fcomplb)
                {
                    db = (radix - 1) - db;
                }

                cy = da + db + cy;
                c.Mant[pchcIdx++] = cy % radix;
                cy /= radix;
            }

            if (cy != 0 && !(fcompla || fcomplb))
            {
                if (pchcIdx >= c.Mant.Length)
                {
                    Array.Resize(ref c.Mant, pchcIdx + 1);
                }
                c.Mant[pchcIdx++] = cy;
                c.CDigit++;
            }

            if (!(fcompla || fcomplb))
            {
                c.Sign = a.Sign;
            }
            else
            {
                if (cy != 0)
                {
                    c.Sign = 1;
                }
                else
                {
                    c.Sign = -1;
                    cy = 1;
                    for (int i = 0; i < c.CDigit; i++)
                    {
                        cy = radix - 1 - c.Mant[i] + cy;
                        c.Mant[i] = cy % radix;
                        cy /= radix;
                    }
                }
            }

            // Remove leading zeros
            while (c.CDigit > 1 && c.Mant[c.CDigit - 1] == 0)
            {
                c.CDigit--;
            }

            pa = c;
        }

        public static void mulnum(ref Number pa, Number b, uint radix)
        {
            if (b.CDigit > 1 || b.Mant[0] != 1 || b.Exp != 0)
            {
                if (pa.CDigit > 1 || pa.Mant[0] != 1 || pa.Exp != 0)
                {
                    _mulnum(ref pa, b, radix);
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

        public static void _mulnum(ref Number pa, Number b, uint radix)
        {
            Number a = pa;
            int ibdigit = a.CDigit + b.CDigit - 1;
            Number c = new Number(ibdigit + 1);
            c.CDigit = ibdigit;
            c.Sign = a.Sign * b.Sign;
            c.Exp = a.Exp + b.Exp;

            int pchaIdx = 0;
            int pchcoffset = 0;

            for (int iadigit = a.CDigit; iadigit > 0; iadigit--)
            {
                uint da = a.Mant[pchaIdx++];
                int pchbIdx = 0;
                int pchc = pchcoffset++;

                for (int jbdigit = b.CDigit; jbdigit > 0; jbdigit--)
                {
                    ulong cy = 0;
                    ulong mcy = (ulong)da * b.Mant[pchbIdx++];
                    if (mcy != 0)
                    {
                        if (jbdigit == 1 && iadigit == 1)
                        {
                            c.CDigit++;
                        }
                    }

                    int icdigit = 0;
                    while (mcy != 0 || cy != 0)
                    {
                        int targetIdx = pchc + icdigit;
                        if (targetIdx >= c.Mant.Length)
                        {
                            Array.Resize(ref c.Mant, targetIdx + 8);
                        }
                        cy += (ulong)c.Mant[targetIdx] + (mcy % (ulong)radix);
                        c.Mant[targetIdx] = (uint)(cy % (ulong)radix);
                        icdigit++;

                        mcy /= (ulong)radix;
                        cy /= (ulong)radix;
                    }

                    pchc++;
                }
            }

            while (c.CDigit > 1 && c.Mant[c.CDigit - 1] == 0)
            {
                c.CDigit--;
            }

            pa = c;
        }

        public static void remnum(ref Number pa, Number b, uint radix)
        {
            Number tmp = null;
            Number lasttmp = null;

            while (!lessnum(pa, b))
            {
                tmp = b.Clone();
                if (lessnum(tmp, pa))
                {
                    tmp.Exp = pa.CDigit + pa.Exp - tmp.CDigit;
                    if (MSD(pa) <= MSD(tmp))
                    {
                        tmp.Exp--;
                    }
                }

                lasttmp = i32tonum(0, radix);

                while (lessnum(tmp, pa))
                {
                    lasttmp = tmp.Clone();
                    addnum(ref tmp, tmp, radix);
                }

                if (lessnum(pa, tmp))
                {
                    tmp = lasttmp;
                    lasttmp = null;
                }

                tmp.Sign = -1 * pa.Sign;
                addnum(ref pa, tmp, radix);
            }
        }

        public static void divnum(ref Number pa, Number b, uint radix, int precision)
        {
            if (b.CDigit > 1 || b.Mant[0] != 1 || b.Exp != 0)
            {
                _divnum(ref pa, b, radix, precision);
            }
            else
            {
                pa.Sign *= b.Sign;
            }
        }

        public static void _divnum(ref Number pa, Number b, uint radix, int precision)
        {
            Number a = pa;
            int thismax = precision + 2;
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
            Number rem = a.Clone();
            Number tmp = b.Clone();
            tmp.Sign = a.Sign;
            rem.Exp = b.CDigit + b.Exp - rem.CDigit;

            var numberList = new LinkedList<Number>();
            numberList.AddFirst(i32tonum(0, radix));
            for (uint i = 1; i < radix; i++)
            {
                Number newValue = numberList.First.Value.Clone();
                addnum(ref newValue, tmp, radix);
                numberList.AddFirst(newValue);
            }

            int cdigits = 0;
            while (cdigits++ < thismax && !zernum(rem))
            {
                uint digit = radix - 1;
                Number multiple = null;
                foreach (var num in numberList)
                {
                    if (!lessnum(rem, num) || (--digit == 0))
                    {
                        multiple = num;
                        break;
                    }
                }

                if (digit != 0 && multiple != null)
                {
                    multiple.Sign *= -1;
                    addnum(ref rem, multiple, radix);
                    multiple.Sign *= -1;
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
                c.CDigit = 1;
                c.Exp = 0;
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

        public static bool equnum(Number a, Number b)
        {
            int diff = (a.CDigit + a.Exp) - (b.CDigit + b.Exp);
            if (diff != 0)
            {
                return false;
            }

            int pa = a.CDigit - 1;
            int pb = b.CDigit - 1;
            int cdigits = Math.Max(a.CDigit, b.CDigit);
            int ccdigits = cdigits;

            for (; cdigits > 0; cdigits--)
            {
                uint da = (cdigits > (ccdigits - a.CDigit)) ? a.Mant[pa--] : 0;
                uint db = (cdigits > (ccdigits - b.CDigit)) ? b.Mant[pb--] : 0;
                if (da != db)
                {
                    return false;
                }
            }

            return true;
        }

        public static bool lessnum(Number a, Number b)
        {
            int diff = (a.CDigit + a.Exp) - (b.CDigit + b.Exp);
            if (diff < 0)
            {
                return true;
            }
            if (diff > 0)
            {
                return false;
            }

            int pa = a.CDigit - 1;
            int pb = b.CDigit - 1;
            int cdigits = Math.Max(a.CDigit, b.CDigit);
            int ccdigits = cdigits;

            for (; cdigits > 0; cdigits--)
            {
                uint da = (cdigits > (ccdigits - a.CDigit)) ? a.Mant[pa--] : 0;
                uint db = (cdigits > (ccdigits - b.CDigit)) ? b.Mant[pb--] : 0;
                if (da != db)
                {
                    return da < db;
                }
            }

            return false;
        }

        public static bool zernum(Number a)
        {
            if (a == null) return true;
            int length = a.CDigit;
            for (int i = 0; i < length; i++)
            {
                if (a.Mant[i] != 0)
                {
                    return false;
                }
            }
            return true;
        }
    }
}
