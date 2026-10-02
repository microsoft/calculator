// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalcManager.Ratpack;

namespace CalcManager.CEngine
{
    public static class RationalMath
    {
        public static Rational Frac(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.fracrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Integer(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.intrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Pow(Rational @base, Rational pow)
        {
            Rat baseRat = @base.ToPRAT();
            Rat powRat = pow.ToPRAT();
            Ratpak.powrat(ref baseRat, powRat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(baseRat);
        }

        public static Rational Root(Rational @base, Rational root)
        {
            return Pow(@base, Invert(root));
        }

        public static Rational Fact(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.factrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Exp(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.exprat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Log(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.lograt(ref prat, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Log10(Rational rat)
        {
            return Log(rat) / new Rational(Ratpak.ln_ten);
        }

        public static Rational Invert(Rational rat)
        {
            return new Rational(1) / rat;
        }

        public static Rational Abs(Rational rat)
        {
            return new Rational(new Number(1, rat.P.Exp, rat.P.Mantissa), new Number(1, rat.Q.Exp, rat.Q.Mantissa));
        }

        public static Rational Sin(Rational rat, AngleType angletype)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.sinanglerat(ref prat, angletype, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Cos(Rational rat, AngleType angletype)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.cosanglerat(ref prat, angletype, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Tan(Rational rat, AngleType angletype)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.tananglerat(ref prat, angletype, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational ASin(Rational rat, AngleType angletype)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.asinanglerat(ref prat, angletype, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational ACos(Rational rat, AngleType angletype)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.acosanglerat(ref prat, angletype, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational ATan(Rational rat, AngleType angletype)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.atananglerat(ref prat, angletype, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Sinh(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.sinhrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Cosh(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.coshrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Tanh(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.tanhrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational ASinh(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.asinhrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational ACosh(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.acoshrat(ref prat, Rational.RATIONAL_BASE, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational ATanh(Rational rat)
        {
            Rat prat = rat.ToPRAT();
            Ratpak.atanhrat(ref prat, Rational.RATIONAL_PRECISION);
            return new Rational(prat);
        }

        public static Rational Mod(Rational a, Rational b)
        {
            Rat prat = a.ToPRAT();
            Rat pn = b.ToPRAT();
            Ratpak.modrat(ref prat, pn);
            return new Rational(prat);
        }
    }
}
