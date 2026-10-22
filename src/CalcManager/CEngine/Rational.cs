// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using CalcManager.Ratpack;

namespace CalcManager.CEngine
{
    public struct Rational : IEquatable<Rational>
    {
        // Default Base/Radix to use for Rational calculations
        public const uint RATIONAL_BASE = 10;

        // Default Precision to use for Rational calculations
        public const int RATIONAL_PRECISION = 128;

        private Number m_p;
        private Number m_q;

        public Rational()
        {
            m_p = new Number();
            m_q = new Number(1, 0, new List<uint> { 1 });
        }

        public Rational(Number n)
        {
            int qExp = 0;
            if (n.Exp < 0)
            {
                qExp -= n.Exp;
            }

            m_p = new Number(n.Sign, 0, n.Mantissa);
            m_q = new Number(1, qExp, new List<uint> { 1 });
        }

        public Rational(Number p, Number q)
        {
            m_p = p ?? new Number();
            m_q = q ?? new Number(1, 0, new List<uint> { 1 });
        }

        public Rational(int i)
        {
            Rat pr = Ratpak.i32torat(i);
            m_p = new Number(pr.P);
            m_q = new Number(pr.Q);
        }

        public Rational(uint ui)
        {
            Rat pr = Ratpak.Ui32torat(ui);
            m_p = new Number(pr.P);
            m_q = new Number(pr.Q);
        }

        public Rational(ulong ui)
        {
            uint hi = (uint)((ui >> 32) & 0xffffffff);
            uint lo = (uint)ui;

            Rational temp = (new Rational(hi) << 32) | new Rational(lo);
            m_p = temp.P;
            m_q = temp.Q;
        }

        public Rational(Rat prat)
        {
            m_p = new Number(prat.P);
            m_q = new Number(prat.Q);
        }

        public Rat ToPRAT()
        {
            return new Rat(m_p.ToPNUMBER(), m_q.ToPNUMBER());
        }

        public Number P => m_p;
        public Number Q => m_q;

        public static Rational operator -(Rational r)
        {
            return new Rational(new Number(-1 * r.m_p.Sign, r.m_p.Exp, r.m_p.Mantissa), r.m_q);
        }

        public static Rational operator +(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.addrat(ref lhsRat, rhsRat, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator -(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.subrat(ref lhsRat, rhsRat, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator *(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.mulrat(ref lhsRat, rhsRat, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator /(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.divrat(ref lhsRat, rhsRat, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator %(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.remrat(ref lhsRat, rhsRat);
            return new Rational(lhsRat);
        }

        public static Rational operator <<(Rational lhs, int shift)
        {
            return lhs << new Rational(shift);
        }

        public static Rational operator <<(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.lshrat(ref lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator >>(Rational lhs, int shift)
        {
            return lhs >> new Rational(shift);
        }

        public static Rational operator >>(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.rshrat(ref lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator &(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.andrat(ref lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator |(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.orrat(ref lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static Rational operator ^(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            Ratpak.xorrat(ref lhsRat, rhsRat, RATIONAL_BASE, RATIONAL_PRECISION);
            return new Rational(lhsRat);
        }

        public static bool operator ==(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            return Ratpak.rat_equ(lhsRat, rhsRat, RATIONAL_PRECISION);
        }

        public static bool operator !=(Rational lhs, Rational rhs)
        {
            return !(lhs == rhs);
        }

        public static bool operator <(Rational lhs, Rational rhs)
        {
            Rat lhsRat = lhs.ToPRAT();
            Rat rhsRat = rhs.ToPRAT();
            return Ratpak.rat_lt(lhsRat, rhsRat, RATIONAL_PRECISION);
        }

        public static bool operator >(Rational lhs, Rational rhs)
        {
            return rhs < lhs;
        }

        public static bool operator <=(Rational lhs, Rational rhs)
        {
            return !(lhs > rhs);
        }

        public static bool operator >=(Rational lhs, Rational rhs)
        {
            return !(lhs < rhs);
        }

        public override bool Equals(object obj)
        {
            return obj is Rational r && this == r;
        }

        public bool Equals(Rational other)
        {
            return this == other;
        }

        public override int GetHashCode()
        {
            return (m_p?.GetHashCode() ?? 0) ^ (m_q?.GetHashCode() ?? 0);
        }

        public string ToString(uint radix, NumberFormat fmt, int precision)
        {
            Rat rat = ToPRAT();
            return Ratpak.RatToString(ref rat, fmt, radix, precision);
        }

        public ulong ToUInt64_t()
        {
            Rat rat = ToPRAT();
            return Ratpak.rattoUi64(rat, RATIONAL_BASE, RATIONAL_PRECISION);
        }

        public static implicit operator Rational(int value) => new Rational(value);
        public static implicit operator Rational(uint value) => new Rational(value);
        public static implicit operator Rational(ulong value) => new Rational(value);
    }
}
