// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalcManager.Ratpack
{
    public enum NumberFormat
    {
        Float,      // returns floating point, or exponential if number is too big
        Scientific, // always returns scientific notation
        Engineering // always returns engineering notation such that exponent is a multiple of 3
    }

    public enum AngleType
    {
        Degrees, // Calculate trig using 360 degrees per revolution
        Radians, // Calculate trig using 2 pi radians per revolution
        Gradians // Calculate trig using 400 gradians per revolution
    }

    // NUMBER type is a representation of a generic sized generic radix number
    public class Number
    {
        public int Sign;   // The sign of the mantissa, +1, or -1, or 0
        public int CDigit; // The number of digits, or what passes for digits in the radix being used
        public int Exp;    // The offset of digits from the radix point
        public uint[] Mant; // Digit array

        public Number(int cdigit = 0)
        {
            Sign = 1;
            CDigit = cdigit;
            Exp = 0;
            Mant = new uint[cdigit > 0 ? cdigit : 1];
        }

        public Number(int sign, int cdigit, int exp, uint[] mant)
        {
            Sign = sign;
            CDigit = cdigit;
            Exp = exp;
            Mant = mant ?? new uint[1];
        }

        public Number Clone()
        {
            var copy = new Number(CDigit)
            {
                Sign = Sign,
                Exp = Exp
            };
            if (Mant != null)
            {
                copy.Mant = new uint[Math.Max(CDigit, Mant.Length)];
                Array.Copy(Mant, copy.Mant, Math.Min(Mant.Length, copy.Mant.Length));
            }
            return copy;
        }
    }

    // RAT type is a representation radix on 2 NUMBER types. pp/pq
    public class Rat
    {
        public Number P;
        public Number Q;

        public Rat()
        {
            P = null;
            Q = null;
        }

        public Rat(Number p, Number q)
        {
            P = p;
            Q = q;
        }

        public Rat Clone()
        {
            return new Rat(P?.Clone(), Q?.Clone());
        }
    }

    public static partial class Ratpak
    {
        public const uint BASEXPWR = 31;            // Internal log2(BASEX)
        public const uint BASEX = 0x80000000;       // Internal radix used in calculations (2^31)
        public const uint MAX_LONG_SIZE = 33;       // Base 2 requires 32 'digits'

        public static bool g_ftrueinfinite = false;
        public static int g_ratio = 1;
        public static char DecimalSeparator = '.';

        public static void SetDecimalSeparator(char decimalSeparator)
        {
            DecimalSeparator = decimalSeparator;
        }

        public static Number DUPNUM(Number b)
        {
            return b?.Clone();
        }

        public static Rat DUPRAT(Rat b)
        {
            return b?.Clone();
        }

        public static void ABSRAT(Rat x)
        {
            if (x?.P != null) x.P.Sign = 1;
            if (x?.Q != null) x.Q.Sign = 1;
        }

        public static int LOGNUMRADIX(Number pnum)
        {
            return (pnum.CDigit + pnum.Exp) * g_ratio;
        }

        public static int LOGRATRADIX(Rat prat)
        {
            return LOGNUMRADIX(prat.P) - LOGNUMRADIX(prat.Q);
        }

        public static int LOGNUM2(Number pnum)
        {
            return pnum.CDigit + pnum.Exp;
        }

        public static int LOGRAT2(Rat prat)
        {
            return LOGNUM2(prat.P) - LOGNUM2(prat.Q);
        }

        public static int SIGN(Rat prat)
        {
            return prat.P.Sign * prat.Q.Sign;
        }

        public static void RENORMALIZE(Rat x)
        {
            if (x.P.Exp < 0)
            {
                x.Q.Exp -= x.P.Exp;
                x.P.Exp = 0;
            }
            if (x.Q.Exp < 0)
            {
                x.P.Exp -= x.Q.Exp;
                x.Q.Exp = 0;
            }
        }

        public static void TRIMNUM(Number x, int precision)
        {
            if (!g_ftrueinfinite)
            {
                int trim = x.CDigit - precision - g_ratio;
                if (trim > 1)
                {
                    int newCount = x.CDigit - trim;
                    Array.Copy(x.Mant, trim, x.Mant, 0, newCount);
                    x.CDigit -= trim;
                    x.Exp += trim;
                }
            }
        }

        public static void TRIMTOP(Rat x, int precision)
        {
            if (!g_ftrueinfinite)
            {
                int trim = x.P.CDigit - (precision / g_ratio) - 2;
                if (trim > 1)
                {
                    int newCount = x.P.CDigit - trim;
                    Array.Copy(x.P.Mant, trim, x.P.Mant, 0, newCount);
                    x.P.CDigit -= trim;
                    x.P.Exp += trim;
                }
                trim = Math.Min(x.P.Exp, x.Q.Exp);
                x.P.Exp -= trim;
                x.Q.Exp -= trim;
            }
        }

        public static bool SMALL_ENOUGH_RAT(Rat a, int precision)
        {
            return zernum(a.P) || ((((a.Q.CDigit + a.Q.Exp) - (a.P.CDigit + a.P.Exp) - 1) * g_ratio) > precision);
        }
    }
}
