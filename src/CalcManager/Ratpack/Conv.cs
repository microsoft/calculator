// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Text;

namespace CalcManager.Ratpack
{
    public static partial class Ratpak
    {
        private const int MAX_ZEROS_AFTER_DECIMAL = 2;
        private const string DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_@";

        private const byte DP = 0;
        private const byte ZR = 1;
        private const byte NZ = 2;
        private const byte SG = 3;
        private const byte EX = 4;

        private const byte START = 0;
        private const byte MANTS = 1;
        private const byte LZ = 2;
        private const byte LZDP = 3;
        private const byte LD = 4;
        private const byte DZ = 5;
        private const byte DD = 6;
        private const byte DDP = 7;
        private const byte EXPB = 8;
        private const byte EXPS = 9;
        private const byte EXPD = 10;
        private const byte EXPBZ = 11;
        private const byte EXPSZ = 12;
        private const byte EXPDZ = 13;
        private const byte ERR = 14;

        private static readonly byte[,] machine = new byte[ERR + 1, EX + 1]
        {
            // DP, ZR, NZ, SG, EX
            { LZDP, LZ, LD, MANTS, ERR }, // START
            { LZDP, LZ, LD, ERR, ERR },   // MANTS
            { LZDP, LZ, LD, ERR, EXPBZ }, // LZ
            { ERR, DZ, DD, ERR, EXPB },   // LZDP
            { DDP, LD, LD, ERR, EXPB },   // LD
            { ERR, DZ, DD, ERR, EXPBZ },  // DZ
            { ERR, DD, DD, ERR, EXPB },   // DD
            { ERR, DD, DD, ERR, EXPB },   // DDP
            { ERR, EXPD, EXPD, EXPS, ERR }, // EXPB
            { ERR, EXPD, EXPD, ERR, ERR }, // EXPS
            { ERR, EXPD, EXPD, ERR, ERR }, // EXPD
            { ERR, EXPDZ, EXPDZ, EXPSZ, ERR }, // EXPBZ
            { ERR, EXPDZ, EXPDZ, ERR, ERR }, // EXPSZ
            { ERR, EXPDZ, EXPDZ, ERR, ERR }, // EXPDZ
            { ERR, ERR, ERR, ERR, ERR }   // ERR
        };

        public static Rat numtorat(Number pin, uint radix)
        {
            Number pnRadixn = pin.Clone();
            Number qnRadixn = i32tonum(1, radix);

            if (pnRadixn.Exp < 0)
            {
                qnRadixn.Exp -= pnRadixn.Exp;
                pnRadixn.Exp = 0;
            }

            Rat pout = new Rat(numtonRadixx(pnRadixn, radix), numtonRadixx(qnRadixn, radix));
            return pout;
        }

        public static Number nRadixxtonum(Number a, uint radix, int precision)
        {
            Number sum = i32tonum(0, radix);
            Number powofnRadix = Ui32tonum(BASEX, radix);

            int cdigits = precision + 1;
            if (cdigits > a.CDigit)
            {
                cdigits = a.CDigit;
            }

            numpowi32(ref powofnRadix, a.Exp + (a.CDigit - cdigits), radix, precision);

            int ptr = a.CDigit - 1;
            for (; cdigits > 0; ptr--, cdigits--)
            {
                uint val = a.Mant[ptr];
                for (uint bitmask = BASEX / 2; bitmask > 0; bitmask /= 2)
                {
                    addnum(ref sum, sum, radix);
                    if ((val & bitmask) != 0)
                    {
                        sum.Mant[0] |= 1;
                    }
                }
            }

            mulnum(ref sum, powofnRadix, radix);
            sum.Sign = a.Sign;
            return sum;
        }

        public static Number numtonRadixx(Number a, uint radix)
        {
            Number pnumret = i32tonum(0, BASEX);
            Number num_radix = i32tonum((int)radix, BASEX);
            int ptrdigit = a.CDigit - 1;

            for (int idigit = 0; idigit < a.CDigit; idigit++)
            {
                mulnumx(ref pnumret, num_radix);
                Number thisdigit = i32tonum((int)a.Mant[ptrdigit--], BASEX);
                addnum(ref pnumret, thisdigit, BASEX);
            }

            numpowi32x(ref num_radix, a.Exp);
            mulnumx(ref pnumret, num_radix);

            pnumret.Sign = a.Sign;
            return pnumret;
        }

        public static Rat StringToRat(bool mantissaIsNegative, string mantissa, bool exponentIsNegative, string exponent, uint radix, int precision)
        {
            Rat resultRat;

            if (string.IsNullOrEmpty(mantissa))
            {
                if (string.IsNullOrEmpty(exponent))
                {
                    resultRat = rat_zero.Clone();
                }
                else
                {
                    resultRat = rat_one.Clone();
                }
            }
            else
            {
                Number pnummant = StringToNumber(mantissa, radix, precision);
                if (pnummant == null)
                {
                    return null;
                }

                resultRat = numtorat(pnummant, radix);
            }

            int expt = 0;
            if (!string.IsNullOrEmpty(exponent))
            {
                Number numExp = StringToNumber(exponent, radix, precision);
                if (numExp == null)
                {
                    return null;
                }

                expt = numtoi32(numExp, radix);
            }

            Number pnumexp = i32tonum((int)radix, BASEX);
            numpowi32x(ref pnumexp, Math.Abs(expt));

            Rat pratexp = new Rat(pnumexp, i32tonum(1, BASEX));

            if (exponentIsNegative)
            {
                divrat(ref resultRat, pratexp, precision);
            }
            else if (expt > 0)
            {
                mulrat(ref resultRat, pratexp, precision);
            }

            if (mantissaIsNegative)
            {
                resultRat.P.Sign *= -1;
            }

            return resultRat;
        }

        private static char NormalizeCharDigit(char c, uint radix)
        {
            int posA = DIGITS.IndexOf('A');
            int posZ = DIGITS.IndexOf('Z');
            if (radix >= posA && radix <= posZ)
            {
                return char.ToUpperInvariant(c);
            }
            return c;
        }

        public static Number StringToNumber(string numberString, uint radix, int precision)
        {
            if (string.IsNullOrEmpty(numberString)) return null;

            int expSign = 1;
            int expValue = 0;

            Number pnumret = new Number(numberString.Length)
            {
                Sign = 1,
                CDigit = 0,
                Exp = 0
            };
            int pmant = numberString.Length - 1;

            byte state = START;
            foreach (char c in numberString)
            {
                char curChar = (c == DecimalSeparator ? '.' : c);

                switch (curChar)
                {
                    case '-':
                    case '+':
                        state = machine[state, SG];
                        break;
                    case '.':
                        state = machine[state, DP];
                        break;
                    case '0':
                        state = machine[state, ZR];
                        break;
                    case '^':
                    case 'e':
                        if (curChar == '^' || radix == 10)
                        {
                            state = machine[state, EX];
                            break;
                        }
                        goto default;
                    default:
                        state = machine[state, NZ];
                        break;
                }

                switch (state)
                {
                    case MANTS:
                        pnumret.Sign = (curChar == '-') ? -1 : 1;
                        break;
                    case EXPSZ:
                    case EXPS:
                        expSign = (curChar == '-') ? -1 : 1;
                        break;
                    case EXPDZ:
                    case EXPD:
                        curChar = NormalizeCharDigit(curChar, radix);
                        int posExp = DIGITS.IndexOf(curChar);
                        if (posExp >= 0)
                        {
                            expValue = (int)(expValue * radix + posExp);
                        }
                        else
                        {
                            state = ERR;
                        }
                        break;
                    case LD:
                        pnumret.Exp++;
                        goto case DD;
                    case DD:
                        curChar = NormalizeCharDigit(curChar, radix);
                        int pos = DIGITS.IndexOf(curChar);
                        if (pos >= 0 && (uint)pos < radix)
                        {
                            pnumret.Mant[pmant--] = (uint)pos;
                            pnumret.Exp--;
                            pnumret.CDigit++;
                        }
                        else
                        {
                            state = ERR;
                        }
                        break;
                    case DZ:
                        pnumret.Exp--;
                        break;
                    case LZ:
                    case LZDP:
                    case DDP:
                        break;
                }
            }

            if (state == DZ || state == EXPDZ)
            {
                pnumret.CDigit = 1;
                pnumret.Exp = 0;
                pnumret.Sign = 1;
            }
            else
            {
                while (pnumret.CDigit < numberString.Length)
                {
                    pnumret.CDigit++;
                    pnumret.Exp--;
                }

                pnumret.Exp += expSign * expValue;
            }

            if (pnumret.CDigit == 0)
            {
                return null;
            }

            stripzeroesnum(pnumret, precision);
            return pnumret;
        }

        public static Rat i32torat(int ini32)
        {
            return new Rat(i32tonum(ini32, BASEX), i32tonum(1, BASEX));
        }

        public static Rat Ui32torat(uint inui32)
        {
            return new Rat(Ui32tonum(inui32, BASEX), i32tonum(1, BASEX));
        }

        public static Number i32tonum(int ini32, uint radix)
        {
            Number pnumret = new Number((int)MAX_LONG_SIZE)
            {
                CDigit = 0,
                Exp = 0
            };

            long val = ini32;
            if (val < 0)
            {
                pnumret.Sign = -1;
                val = -val;
            }
            else
            {
                pnumret.Sign = 1;
            }

            int pmant = 0;
            do
            {
                pnumret.Mant[pmant++] = (uint)(val % radix);
                val /= radix;
                pnumret.CDigit++;
            } while (val != 0);

            return pnumret;
        }

        public static Number Ui32tonum(uint ini32, uint radix)
        {
            Number pnumret = new Number((int)MAX_LONG_SIZE)
            {
                CDigit = 0,
                Exp = 0,
                Sign = 1
            };

            ulong val = ini32;
            int pmant = 0;
            do
            {
                pnumret.Mant[pmant++] = (uint)(val % radix);
                val /= radix;
                pnumret.CDigit++;
            } while (val != 0);

            return pnumret;
        }

        public static int rattoi32(Rat prat, uint radix, int precision)
        {
            if (rat_gt(prat, rat_max_i32, precision) || rat_lt(prat, rat_min_i32, precision))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            Rat pint = prat.Clone();
            intrat(ref pint, radix, precision);
            divnumx(ref pint.P, pint.Q, precision);
            pint.Q = num_one.Clone();

            int lret = numtoi32(pint.P, BASEX);
            return lret;
        }

        public static uint rattoUi32(Rat prat, uint radix, int precision)
        {
            if (rat_gt(prat, rat_dword, precision) || rat_lt(prat, rat_zero, precision))
            {
                throw new Exception(CalcErr.CALC_E_DOMAIN.ToString());
            }

            Rat pint = prat.Clone();
            intrat(ref pint, radix, precision);
            divnumx(ref pint.P, pint.Q, precision);
            pint.Q = num_one.Clone();

            uint lret = (uint)numtoi32(pint.P, BASEX);
            return lret;
        }

        public static ulong rattoUi64(Rat prat, uint radix, int precision)
        {
            Rat pint = prat.Clone();
            andrat(ref pint, rat_dword, radix, precision);
            uint lo = rattoUi32(pint, radix, precision);

            pint = prat.Clone();
            Rat prat32 = i32torat(32);
            rshrat(ref pint, prat32, radix, precision);
            intrat(ref pint, radix, precision);
            andrat(ref pint, rat_dword, radix, precision);
            uint hi = rattoUi32(pint, radix, precision);

            return (((ulong)hi << 32) | lo);
        }

        public static int numtoi32(Number pnum, uint radix)
        {
            int lret = 0;
            int pmant = pnum.CDigit - 1;
            int expt = pnum.Exp;

            for (int length = pnum.CDigit; length > 0 && length + expt > 0; length--)
            {
                lret = (int)(lret * radix + pnum.Mant[pmant--]);
            }

            while (expt-- > 0)
            {
                lret = (int)(lret * radix);
            }
            lret *= pnum.Sign;

            return lret;
        }

        public static bool stripzeroesnum(Number pnum, int starting)
        {
            bool fstrip = false;
            int pmant = 0;
            int cdigits = pnum.CDigit;

            if (cdigits > starting)
            {
                pmant += cdigits - starting;
                cdigits = starting;
            }

            while (cdigits > 0 && pnum.Mant[pmant] == 0)
            {
                pmant++;
                cdigits--;
                fstrip = true;
            }

            if (fstrip)
            {
                if (cdigits > 0)
                {
                    Array.Copy(pnum.Mant, pmant, pnum.Mant, 0, cdigits);
                }
                pnum.Exp += (pnum.CDigit - cdigits);
                pnum.CDigit = cdigits;
            }

            return fstrip;
        }

        public static string NumberToString(ref Number pnum, NumberFormat format, uint radix, int precision)
        {
            stripzeroesnum(pnum, precision + 2);
            int length = pnum.CDigit;
            int exponent = pnum.Exp + length;

            NumberFormat oldFormat = format;
            if (exponent > precision && format == NumberFormat.Float)
            {
                format = NumberFormat.Scientific;
            }

            if (length > precision)
            {
                length = precision;
            }

            Number round = null;
            if (!zernum(pnum) && (pnum.CDigit >= precision || (length - exponent > precision && exponent >= -MAX_ZEROS_AFTER_DECIMAL)))
            {
                round = i32tonum((int)radix, radix);
                divnum(ref round, num_two, radix, precision);

                if (exponent > 0 || format == NumberFormat.Float)
                {
                    round.Exp = pnum.Exp + pnum.CDigit - round.CDigit - precision;
                }
                else
                {
                    round.Exp = pnum.Exp + pnum.CDigit - round.CDigit - precision - exponent;
                    length = precision + exponent;
                }

                round.Sign = pnum.Sign;
            }

            if (format == NumberFormat.Float)
            {
                if ((length - exponent > precision) || (exponent > precision + 3))
                {
                    if (exponent >= -MAX_ZEROS_AFTER_DECIMAL)
                    {
                        if (round != null) round.Exp -= exponent;
                        length = precision + exponent;
                    }
                    else
                    {
                        format = NumberFormat.Scientific;
                    }
                }
                else if (length + Math.Abs(exponent) < precision && round != null)
                {
                    round.Exp -= exponent;
                }
            }

            if (round != null)
            {
                addnum(ref pnum, round, radix);
                int offset = (pnum.CDigit + pnum.Exp) - (round.CDigit + round.Exp);
                if (stripzeroesnum(pnum, offset))
                {
                    return NumberToString(ref pnum, oldFormat, radix, precision);
                }
            }
            else
            {
                stripzeroesnum(pnum, precision);
            }

            bool useSciForm = false;
            int eout = exponent - 1;
            int pmant = pnum.CDigit - 1;

            if (format == NumberFormat.Scientific || format == NumberFormat.Engineering)
            {
                useSciForm = true;
                if (eout != 0)
                {
                    if (format == NumberFormat.Engineering)
                    {
                        exponent = (eout % 3);
                        eout -= exponent;
                        exponent++;

                        if (exponent < 0)
                        {
                            exponent += 3;
                            eout -= 3;
                        }
                    }
                    else
                    {
                        exponent = 1;
                    }
                }
            }
            else
            {
                eout = 0;
            }

            var sb = new StringBuilder();

            if (pnum.Sign == -1 && length > 0)
            {
                sb.Append('-');
            }

            if (exponent <= 0 && !useSciForm)
            {
                sb.Append('0');
                sb.Append(DecimalSeparator);
            }

            while (exponent < 0)
            {
                sb.Append('0');
                exponent++;
            }

            while (length > 0)
            {
                exponent--;
                sb.Append(DIGITS[(int)pnum.Mant[pmant--]]);
                length--;

                if (exponent == 0)
                {
                    sb.Append(DecimalSeparator);
                }
            }

            while (exponent > 0)
            {
                sb.Append('0');
                exponent--;
                if (exponent == 0)
                {
                    sb.Append(DecimalSeparator);
                }
            }

            if (useSciForm)
            {
                sb.Append(radix == 10 ? 'e' : '^');
                sb.Append(eout < 0 ? '-' : '+');
                int absEout = Math.Abs(eout);
                var expSb = new StringBuilder();
                do
                {
                    expSb.Append(DIGITS[(int)(absEout % radix)]);
                    absEout = (int)(absEout / radix);
                } while (absEout > 0);

                for (int i = expSb.Length - 1; i >= 0; i--)
                {
                    sb.Append(expSb[i]);
                }
            }

            string result = sb.ToString();
            if (result.EndsWith(DecimalSeparator.ToString()))
            {
                result = result.Substring(0, result.Length - 1);
            }

            return result;
        }

        public static string RatToString(ref Rat prat, NumberFormat format, uint radix, int precision)
        {
            Number p = RatToNumber(prat, radix, precision);
            return NumberToString(ref p, format, radix, precision);
        }

        public static Number RatToNumber(Rat prat, uint radix, int precision)
        {
            Rat temprat = prat.Clone();
            int scaleby = Math.Min(temprat.P.Exp, temprat.Q.Exp);
            scaleby = Math.Max(scaleby, 0);

            temprat.P.Exp -= scaleby;
            temprat.Q.Exp -= scaleby;

            Number p = nRadixxtonum(temprat.P, radix, precision);
            Number q = nRadixxtonum(temprat.Q, radix, precision);

            divnum(ref p, q, radix, precision);
            return p;
        }

        public static void flatrat(ref Rat prat, uint radix, int precision)
        {
            Number pnum = RatToNumber(prat, radix, precision);
            prat = numtorat(pnum, radix);
        }

        public static Number gcd(Number a, Number b)
        {
            if (zernum(a))
            {
                return b?.Clone();
            }
            if (zernum(b))
            {
                return a?.Clone();
            }

            Number larger;
            Number smaller;

            if (lessnum(a, b))
            {
                larger = b.Clone();
                smaller = a.Clone();
            }
            else
            {
                larger = a.Clone();
                smaller = b.Clone();
            }

            while (!zernum(smaller))
            {
                remnum(ref larger, smaller, BASEX);
                Number r = larger;
                larger = smaller;
                smaller = r;
            }

            return larger;
        }

        public static Number i32factnum(int ini32, uint radix)
        {
            Number lret = i32tonum(1, radix);
            while (ini32 > 0)
            {
                Number tmp = i32tonum(ini32--, radix);
                mulnum(ref lret, tmp, radix);
            }
            return lret;
        }

        public static Number i32prodnum(int start, int stop, uint radix)
        {
            Number lret = i32tonum(1, radix);
            while (start <= stop)
            {
                if (start != 0)
                {
                    Number tmp = i32tonum(start, radix);
                    mulnum(ref lret, tmp, radix);
                }
                start++;
            }
            return lret;
        }

        public static void numpowi32(ref Number proot, int power, uint radix, int precision)
        {
            Number lret = i32tonum(1, radix);

            while (power > 0)
            {
                if ((power & 1) != 0)
                {
                    mulnum(ref lret, proot, radix);
                }
                mulnum(ref proot, proot, radix);
                TRIMNUM(proot, precision);
                power >>= 1;
            }

            proot = lret;
        }

        public static void ratpowi32(ref Rat proot, int power, int precision)
        {
            if (power < 0)
            {
                ratpowi32(ref proot, -power, precision);
                Number pnumtemp = proot.P;
                proot.P = proot.Q;
                proot.Q = pnumtemp;
            }
            else
            {
                Rat lret = i32torat(1);

                while (power > 0)
                {
                    if ((power & 1) != 0)
                    {
                        mulnumx(ref lret.P, proot.P);
                        mulnumx(ref lret.Q, proot.Q);
                    }
                    mulrat(ref proot, proot, precision);
                    trimit(ref lret, precision);
                    trimit(ref proot, precision);
                    power >>= 1;
                }

                proot = lret;
            }
        }
    }
}
