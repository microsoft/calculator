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

Rational RationalMath::Add(Rational const& lhs, Rational const& rhs, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        addrat(&lhsRat, rhsRat, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
Rational RationalMath::Sub(Rational const& lhs, Rational const& rhs, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        subrat(&lhsRat, rhsRat, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
Rational RationalMath::Mul(Rational const& lhs, Rational const& rhs, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        mulrat(&lhsRat, rhsRat, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
Rational RationalMath::Div(Rational const& lhs, Rational const& rhs, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        divrat(&lhsRat, rhsRat, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
Rational RationalMath::Mod(Rational const& lhs, Rational const& rhs)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        modrat(&lhsRat, rhsRat);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

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
    return Pow(base, Div(1, root, precision), radix, precision);
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
    return Div(Log(rat, precision), 10, precision);
}

Rational RationalMath::Lsh(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        lshrat(&lhsRat, rhsRat, radix, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
Rational RationalMath::Rsh(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        rshrat(&lhsRat, rhsRat, radix, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}

Rational RationalMath::Invert(Rational const& rat, int32_t precision)
{
    return Div(1, rat, precision);
}
Rational RationalMath::Negate(Rational const& rat)
{
    return Rational{ Number{ -1 * rat.P().Sign(), rat.P().Exp(), rat.P().Mantissa() }, rat.Q() };
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

Rational RationalMath::Not(Rational const& rat, bool fIntegerMode, Rational const& chopNum, uint32_t radix, int32_t precision)
{
    Rational result{};

    if (radix == 10 && !fIntegerMode)
    {
        result = Integer(rat, radix, precision);
        result = Add(result, 1, precision);
        result = Negate(result);
    }
    else
    {
        result = Xor(rat, chopNum, radix, precision);
    }

    return result;
}
Rational RationalMath::And(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();

    try
    {
        andrat(&lhsRat, rhsRat, radix, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
Rational RationalMath::Or(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();
    try
    {
        orrat(&lhsRat, rhsRat, radix, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
Rational RationalMath::Xor(Rational const& lhs, Rational const& rhs, uint32_t radix, int32_t precision)
{
    PRAT lhsRat = lhs.ToPRAT();
    PRAT rhsRat = rhs.ToPRAT();
    try
    {
        xorrat(&lhsRat, rhsRat, radix, precision);
        destroyrat(rhsRat);
    }
    catch (DWORD error)
    {
        destroyrat(lhsRat);
        destroyrat(rhsRat);
        throw(error);
    }

    Rational result{ lhsRat };
    destroyrat(lhsRat);

    return result;
}
