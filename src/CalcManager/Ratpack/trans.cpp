// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//----------------------------------------------------------------------------
//  File           trans.c
//  Copyright      (C) 1995-96 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Contains sin, cos and tan for rationals
//
//
//----------------------------------------------------------------------------

#include "ratpak.h"

void scalerat(_Inout_ PRAT* pa, AngleType angletype, uint32_t radix, int32_t precision)
{
    switch (angletype)
    {
    case AngleType::Radians:
        scale2pi(pa, radix, precision);
        break;
    case AngleType::Degrees:
        scale(pa, rat_360, radix, precision);
        break;
    case AngleType::Gradians:
        scale(pa, rat_400, radix, precision);
        break;
    }
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: sinrat, _sinrat
//
//  ARGUMENTS:  x PRAT representation of number to take the sine of
//
//  RETURN: sin of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___          2j+1
//   \  ]   j    X
//    \   -1  * ---------
//    /          (2j+1)!
//   /__]
//   j=0
//          or,
//    n
//   ___                                                 2
//   \  ]                                              -X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j)*(2j+1)
//   /__]
//   j=0
//
//   thisterm  = X ;  and stop when thisterm < precision used.
//           0                              n
//
//-----------------------------------------------------------------------------

void _sinrat(PRAT* px, int32_t precision)

{
    CREATETAYLOR();

    DUPRAT(pret, *px);
    DUPRAT(thisterm, *px);

    DUPNUM(n2, num_one);
    xx->pp->sign *= -1;

    do
    {
        NEXTTERM(xx, INC(n2) DIVNUM(n2) INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();

    // Since *px might be epsilon above 1 or below -1, due to TRIMIT we need
    // this trick here.
    inbetween(px, rat_one, precision);

    // Since *px might be epsilon near zero we must set it to zero.
    if (rat_le(*px, rat_smallest, precision) && rat_ge(*px, rat_negsmallest, precision))
    {
        DUPRAT(*px, rat_zero);
    }
}

void sinrat(PRAT* px, uint32_t radix, int32_t precision)
{
    scale2pi(px, radix, precision);
    _sinrat(px, precision);
}

void sinanglerat(_Inout_ PRAT* pa, AngleType angletype, uint32_t radix, int32_t precision)

{
    scalerat(pa, angletype, radix, precision);
    switch (angletype)
    {
    case AngleType::Degrees:
        if (rat_gt(*pa, rat_180, precision))
        {
            subrat(pa, rat_360, precision);
        }
        divrat(pa, rat_180, precision);
        mulrat(pa, pi, precision);
        break;
    case AngleType::Gradians:
        if (rat_gt(*pa, rat_200, precision))
        {
            subrat(pa, rat_400, precision);
        }
        divrat(pa, rat_200, precision);
        mulrat(pa, pi, precision);
        break;
    }
    _sinrat(pa, precision);
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: cosrat, _cosrat
//
//  ARGUMENTS:  x PRAT representation of number to take the cosine of
//
//  RETURN: cosine of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___    2j   j
//   \  ]  X   -1
//    \   ---------
//    /    (2j)!
//   /__]
//   j=0
//          or,
//    n
//   ___                                                 2
//   \  ]                                              -X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j)*(2j+1)
//   /__]
//   j=0
//
//   thisterm  = 1 ;  and stop when thisterm < precision used.
//           0                              n
//
//-----------------------------------------------------------------------------

void _cosrat(PRAT* px, uint32_t radix, int32_t precision)

{
    CREATETAYLOR();

    destroynum(pret->pp);
    destroynum(pret->pq);

    pret->pp = i32tonum(1L, radix);
    pret->pq = i32tonum(1L, radix);

    DUPRAT(thisterm, pret)

    n2 = i32tonum(0L, radix);
    xx->pp->sign *= -1;

    do
    {
        NEXTTERM(xx, INC(n2) DIVNUM(n2) INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();
    // Since *px might be epsilon above 1 or below -1, due to TRIMIT we need
    // this trick here.
    inbetween(px, rat_one, precision);
    // Since *px might be epsilon near zero we must set it to zero.
    if (rat_le(*px, rat_smallest, precision) && rat_ge(*px, rat_negsmallest, precision))
    {
        DUPRAT(*px, rat_zero);
    }
}

void cosrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)
{
    scale2pi(px, radix, precision);
    _cosrat(px, radix, precision);
}

void cosanglerat(_Inout_ PRAT* pa, AngleType angletype, uint32_t radix, int32_t precision)

{
    scalerat(pa, angletype, radix, precision);
    switch (angletype)
    {
    case AngleType::Degrees:
        if (rat_gt(*pa, rat_180, precision))
        {
            PRAT ptmp = nullptr;
            DUPRAT(ptmp, rat_360);
            subrat(&ptmp, *pa, precision);
            destroyrat(*pa);
            *pa = ptmp;
        }
        divrat(pa, rat_180, precision);
        mulrat(pa, pi, precision);
        break;
    case AngleType::Gradians:
        if (rat_gt(*pa, rat_200, precision))
        {
            PRAT ptmp = nullptr;
            DUPRAT(ptmp, rat_400);
            subrat(&ptmp, *pa, precision);
            destroyrat(*pa);
            *pa = ptmp;
        }
        divrat(pa, rat_200, precision);
        mulrat(pa, pi, precision);
        break;
    }
    _cosrat(pa, radix, precision);
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: tanrat, _tanrat
//
//  ARGUMENTS:  x PRAT representation of number to take the tangent of
//
//  RETURN: tan     of x in PRAT form.
//
//  EXPLANATION: This uses sinrat and cosrat
//
//-----------------------------------------------------------------------------

void _tanrat(PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT ptmp = nullptr;

    DUPRAT(ptmp, *px);
    _sinrat(px, precision);
    _cosrat(&ptmp, radix, precision);
    if (zerrat(ptmp))
    {
        destroyrat(ptmp);
        throw(CALC_E_DOMAIN);
    }
    divrat(px, ptmp, precision);

    destroyrat(ptmp);
}

void tanrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)
{
    scale2pi(px, radix, precision);
    _tanrat(px, radix, precision);
}

void tananglerat(_Inout_ PRAT* pa, AngleType angletype, uint32_t radix, int32_t precision)

{
    scalerat(pa, angletype, radix, precision);
    switch (angletype)
    {
    case AngleType::Degrees:
        if (rat_gt(*pa, rat_180, precision))
        {
            subrat(pa, rat_180, precision);
        }
        divrat(pa, rat_180, precision);
        mulrat(pa, pi, precision);
        break;
    case AngleType::Gradians:
        if (rat_gt(*pa, rat_200, precision))
        {
            subrat(pa, rat_200, precision);
        }
        divrat(pa, rat_200, precision);
        mulrat(pa, pi, precision);
        break;
    }
    _tanrat(pa, radix, precision);
}
