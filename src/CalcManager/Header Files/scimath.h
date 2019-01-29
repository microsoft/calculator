// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Rational.h"

namespace CalcEngine::RationalMath
{
    Rational Frac(Rational const& rat, uint32_t radix, int32_t precision);
    Rational Integer(Rational const& rat, uint32_t radix, int32_t precision);

    Rational Add(Rational const& lhs, Rational const& rhs, int32_t precision);
    Rational Sub(Rational const& lhs, Rational const& rhs, int32_t precision);
    Rational Mul(Rational const& lhs, Rational const& rhs, int32_t precision);
    Rational Div(Rational const& lhs, Rational const& rhs, int32_t precision);
    Rational Mod(Rational const& lhs, Rational const& rhs);

    Rational Pow(Rational const& base, Rational const& pow, uint32_t radix, int32_t precision);
    Rational Root(Rational const& base, Rational const& root, uint32_t radix, int32_t precision);
    Rational Fact(Rational const& rat, uint32_t radix, int32_t precision);

    Rational Exp(Rational const& rat, uint32_t radix, int32_t precision);
    Rational Log(Rational const& rat, int32_t precision);
    Rational Log10(Rational const& rat, int32_t precision);

    Rational Lsh(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision);
    Rational Rsh(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision);

    Rational Invert(Rational const& rat, int32_t precision);
    Rational Negate(Rational const& rat);
    Rational Abs(Rational const& rat);

    Rational Sin(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
    Rational Cos(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
    Rational Tan(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
    Rational ASin(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
    Rational ACos(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
    Rational ATan(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);

    Rational Sinh(Rational const& rat, uint32_t radix, int32_t precision);
    Rational Cosh(Rational const& rat, uint32_t radix, int32_t precision);
    Rational Tanh(Rational const& rat, uint32_t radix, int32_t precision);
    Rational ASinh(Rational const& rat, uint32_t radix, int32_t precision);
    Rational ACosh(Rational const& rat, uint32_t radix, int32_t precision);
    Rational ATanh(Rational const& rat, int32_t precision);

    Rational Not(Rational const& rat, bool fIntegerMode, Rational const& chopNum, uint32_t radix, int32_t precision);
    Rational And(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision);
    Rational Or(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision);
    Rational Xor(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision);
}
