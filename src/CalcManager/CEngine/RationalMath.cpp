// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Header Files/RationalMath.h"

using namespace std;
using namespace CalcEngine;

Rational RationalMath::Frac(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();
    try
    {
        fracrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Integer(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();
    try
    {
        intrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Pow(Rational const& base, Rational const& pow)
{
    PRAT baseRat = base.ToPRAT();
    PRAT powRat = pow.ToPRAT();

    try
    {
        powrat(&baseRat, powRat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Root(Rational const& base, Rational const& root)
{
    return Pow(base, Invert(root));
}

Rational RationalMath::Fact(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        factrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Exp(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        exprat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Log(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        lograt(&prat, RATIONAL_PRECISION);
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

Rational RationalMath::Log10(Rational const& rat)
{
    return Log(rat) / Rational{ ln_ten };
}

Rational RationalMath::Invert(Rational const& rat)
{
    return 1 / rat;
}

Rational RationalMath::Abs(Rational const& rat)
{
    return Rational{ Number{ 1, rat.P().Exp(), rat.P().Mantissa() }, Number{ 1, rat.Q().Exp(), rat.Q().Mantissa() } };
}

Rational RationalMath::Sin(Rational const& rat, ANGLE_TYPE angletype)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        sinanglerat(&prat, angletype, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Cos(Rational const& rat, ANGLE_TYPE angletype)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        cosanglerat(&prat, angletype, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Tan(Rational const& rat, ANGLE_TYPE angletype)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        tananglerat(&prat, angletype, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::ASin(Rational const& rat, ANGLE_TYPE angletype)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        asinanglerat(&prat, angletype, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::ACos(Rational const& rat, ANGLE_TYPE angletype)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        acosanglerat(&prat, angletype, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::ATan(Rational const& rat, ANGLE_TYPE angletype)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        atananglerat(&prat, angletype, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Sinh(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        sinhrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Cosh(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        coshrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::Tanh(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        tanhrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::ASinh(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        asinhrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::ACosh(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        acoshrat(&prat, RATIONAL_BASE, RATIONAL_PRECISION);
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

Rational RationalMath::ATanh(Rational const& rat)
{
    PRAT prat = rat.ToPRAT();

    try
    {
        atanhrat(&prat, RATIONAL_PRECISION);
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
