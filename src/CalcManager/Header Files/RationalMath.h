// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Rational.h"

namespace CalcEngine::RationalMath
{
    Rational Frac(Rational const& rat);
    Rational Integer(Rational const& rat);

    Rational Pow(Rational const& base, Rational const& pow);
    Rational Root(Rational const& base, Rational const& root);
    Rational Fact(Rational const& rat);
    Rational Mod(Rational const& a, Rational const& b);

    Rational Exp(Rational const& rat);
    Rational Log(Rational const& rat);
    Rational Log10(Rational const& rat);

    Rational Invert(Rational const& rat);
    Rational Abs(Rational const& rat);

    Rational Sin(Rational const& rat, AngleType angletype);
    Rational Cos(Rational const& rat, AngleType angletype);
    Rational Tan(Rational const& rat, AngleType angletype);
    Rational ASin(Rational const& rat, AngleType angletype);
    Rational ACos(Rational const& rat, AngleType angletype);
    Rational ATan(Rational const& rat, AngleType angletype);

    Rational Sinh(Rational const& rat);
    Rational Cosh(Rational const& rat);
    Rational Tanh(Rational const& rat);
    Rational ASinh(Rational const& rat);
    Rational ACosh(Rational const& rat);
    Rational ATanh(Rational const& rat);
}
