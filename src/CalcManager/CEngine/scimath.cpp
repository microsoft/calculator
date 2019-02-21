// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Header Files/CalcEngine.h"
#include "Ratpack/ratpak.h"

using namespace std;
using namespace CalcEngine;

Rational RationalMath::Frac(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();
    try
    {
        fracrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Integer(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();
    try
    {
        intrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Pow(Rational const& base, Rational const& pow, uint32_t radix, int32_t precision)
{
    PRAT baseRat = base.ToPRAT();
    PRAT powRat = pow.ToPRAT();

    try
    {
        powrat(&baseRat, powRat, radix, precision);
        destroyrat(powRat);
    }
    catch (DWORD error)
    {
        destroyrat(baseRat);
        destroyrat(powRat);
        throw(error);
    }

    Rational result{ baseRat };
    destroyrat(baseRat);

    return result;
}

Rational RationalMath::Root(Rational const& base, Rational const& root, uint32_t radix, int32_t precision)
{
    return Pow(base, Invert(root, precision), radix, precision);
}

Rational RationalMath::Fact(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        factrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Exp(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        exprat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Log(Rational const& rat, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        lograt(&prat, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Log10(Rational const& rat, int32_t precision)
{
    return Log(rat, precision).Div(Rational{ ln_ten }, precision);
}

Rational RationalMath::Invert(Rational const& rat, int32_t precision)
{
    return Rational{ 1 }.Div(rat, precision);
}

Rational RationalMath::Abs(Rational const& rat)
{
    return Rational{ Number{ 1, rat.P().Exp(), rat.P().Mantissa() }, Number{ 1, rat.Q().Exp(), rat.Q().Mantissa() } };
}

Rational RationalMath::Sin(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        sinanglerat(&prat, angletype, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Cos(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        cosanglerat(&prat, angletype, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Tan(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        tananglerat(&prat, angletype, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::ASin(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        asinanglerat(&prat, angletype, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::ACos(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        acosanglerat(&prat, angletype, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::ATan(Rational const& rat, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        atananglerat(&prat, angletype, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Sinh(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        sinhrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Cosh(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        coshrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::Tanh(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        tanhrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::ASinh(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        asinhrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::ACosh(Rational const& rat, uint32_t radix, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        acoshrat(&prat, radix, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}

Rational RationalMath::ATanh(Rational const& rat, int32_t precision)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        atanhrat(&prat, precision);
    }
    catch (DWORD error)
    {
        destroyrat(prat);
        throw(error);
    }

    Rational result{ prat };
    destroyrat(prat);

    return result;
}
