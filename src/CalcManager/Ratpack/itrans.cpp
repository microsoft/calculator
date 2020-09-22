// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//-----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           itrans.c
//  Copyright      (C) 1995-96 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Contains inverse sin, cos, tan functions for rationals
//
//  Special Information
//
//-----------------------------------------------------------------------------
#include "ratpak.h"

void ascalerat(_Inout_ PRAT* pa, AngleType angletype, int32_t precision)
{
    switch (angletype)
    {
    case AngleType::Radians:
        break;
    case AngleType::Degrees:
        divrat(pa, two_pi, precision);
        mulrat(pa, rat_360, precision);
        break;
    case AngleType::Gradians:
        divrat(pa, two_pi, precision);
        mulrat(pa, rat_400, precision);
        break;
    }
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: asinrat, _asinrat
//
//  ARGUMENTS: x PRAT representation of number to take the inverse
//    sine of
//  RETURN: asin  of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___                                                   2 2
//   \  ]                                            (2j+1) X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j+2)*(2j+3)
//   /__]
//   j=0
//
//   thisterm  = X ;  and stop when thisterm < precision used.
//           0                              n
//
//   If abs(x) > 0.85 then an alternate form is used
//      pi/2-sgn(x)*asin(sqrt(1-x^2)
//
//
//-----------------------------------------------------------------------------

void _asinrat(PRAT* px, int32_t precision)

{
    CREATETAYLOR();
    DUPRAT(pret, *px);
    DUPRAT(thisterm, *px);
    DUPNUM(n2, num_one);

    do
    {
        NEXTTERM(xx, MULNUM(n2) MULNUM(n2) INC(n2) DIVNUM(n2) INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));
    DESTROYTAYLOR();
}

void asinanglerat(_Inout_ PRAT* pa, AngleType angletype, uint32_t radix, int32_t precision)

{
    asinrat(pa, radix, precision);
    ascalerat(pa, angletype, precision);
}

void asinrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT pret = nullptr;
    PRAT phack = nullptr;
    int32_t sgn = SIGN(*px);

    (*px)->pp->sign = 1;
    (*px)->pq->sign = 1;

    // Avoid the really bad part of the asin curve near +/-1.
    DUPRAT(phack, *px);
    subrat(&phack, rat_one, precision);
    // Since *px might be epsilon near zero we must set it to zero.
    if (rat_le(phack, rat_smallest, precision) && rat_ge(phack, rat_negsmallest, precision))
    {
        destroyrat(phack);
        DUPRAT(*px, pi_over_two);
    }
    else
    {
        destroyrat(phack);
        if (rat_gt(*px, pt_eight_five, precision))
        {
            if (rat_gt(*px, rat_one, precision))
            {
                subrat(px, rat_one, precision);
                if (rat_gt(*px, rat_smallest, precision))
                {
                    throw(CALC_E_DOMAIN);
                }
                else
                {
                    DUPRAT(*px, rat_one);
                }
            }
            DUPRAT(pret, *px);
            mulrat(px, pret, precision);
            (*px)->pp->sign *= -1;
            addrat(px, rat_one, precision);
            rootrat(px, rat_two, radix, precision);
            _asinrat(px, precision);
            (*px)->pp->sign *= -1;
            addrat(px, pi_over_two, precision);
            destroyrat(pret);
        }
        else
        {
            _asinrat(px, precision);
        }
    }
    (*px)->pp->sign = sgn;
    (*px)->pq->sign = 1;
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: acosrat, _acosrat
//
//  ARGUMENTS: x PRAT representation of number to take the inverse
//    cosine of
//  RETURN: acos  of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___                                                   2 2
//   \  ]                                            (2j+1) X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j+2)*(2j+3)
//   /__]
//   j=0
//
//   thisterm  = 1 ;  and stop when thisterm < precision used.
//           0                              n
//
//   In this case pi/2-asin(x) is used.  At least for now _acosrat isn't
//      called.
//
//-----------------------------------------------------------------------------

void acosanglerat(_Inout_ PRAT* pa, AngleType angletype, uint32_t radix, int32_t precision)

{
    acosrat(pa, radix, precision);
    ascalerat(pa, angletype, precision);
}

void _acosrat(PRAT* px, int32_t precision)

{
    CREATETAYLOR();

    createrat(thisterm);
    thisterm->pp = i32tonum(1L, BASEX);
    thisterm->pq = i32tonum(1L, BASEX);

    DUPNUM(n2, num_one);

    do
    {
        NEXTTERM(xx, MULNUM(n2) MULNUM(n2) INC(n2) DIVNUM(n2) INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();
}

void acosrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    int32_t sgn = SIGN(*px);

    (*px)->pp->sign = 1;
    (*px)->pq->sign = 1;

    if (rat_equ(*px, rat_one, precision))
    {
        if (sgn == -1)
        {
            DUPRAT(*px, pi);
        }
        else
        {
            DUPRAT(*px, rat_zero);
        }
    }
    else
    {
        (*px)->pp->sign = sgn;
        asinrat(px, radix, precision);
        (*px)->pp->sign *= -1;
        addrat(px, pi_over_two, precision);
    }
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: atanrat, _atanrat
//
//  ARGUMENTS: x PRAT representation of number to take the inverse
//              hyperbolic tangent of
//
//  RETURN: atanh of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___                                                   2
//   \  ]                                            (2j)*X (-1^j)
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j+2)
//   /__]
//   j=0
//
//   thisterm  = X ;  and stop when thisterm < precision used.
//           0                              n
//
//   If abs(x) > 0.85 then an alternate form is used
//      asin(x/sqrt(q+x^2))
//
//   And if abs(x) > 2.0 then this form is used.
//
//   pi/2 - atan(1/x)
//
//-----------------------------------------------------------------------------

void atananglerat(_Inout_ PRAT* pa, AngleType angletype, uint32_t radix, int32_t precision)

{
    atanrat(pa, radix, precision);
    ascalerat(pa, angletype, precision);
}

void _atanrat(PRAT* px, int32_t precision)

{
    CREATETAYLOR();

    DUPRAT(pret, *px);
    DUPRAT(thisterm, *px);

    DUPNUM(n2, num_one);

    xx->pp->sign *= -1;

    do
    {
        NEXTTERM(xx, MULNUM(n2) INC(n2) INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();
}

void atanrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT tmpx = nullptr;
    int32_t sgn = SIGN(*px);

    (*px)->pp->sign = 1;
    (*px)->pq->sign = 1;

    if (rat_gt((*px), pt_eight_five, precision))
    {
        if (rat_gt((*px), rat_two, precision))
        {
            (*px)->pp->sign = sgn;
            (*px)->pq->sign = 1;
            DUPRAT(tmpx, rat_one);
            divrat(&tmpx, (*px), precision);
            _atanrat(&tmpx, precision);
            tmpx->pp->sign = sgn;
            tmpx->pq->sign = 1;
            DUPRAT(*px, pi_over_two);
            subrat(px, tmpx, precision);
            destroyrat(tmpx);
        }
        else
        {
            (*px)->pp->sign = sgn;
            DUPRAT(tmpx, *px);
            mulrat(&tmpx, *px, precision);
            addrat(&tmpx, rat_one, precision);
            rootrat(&tmpx, rat_two, radix, precision);
            divrat(px, tmpx, precision);
            destroyrat(tmpx);
            asinrat(px, radix, precision);
            (*px)->pp->sign = sgn;
            (*px)->pq->sign = 1;
        }
    }
    else
    {
        (*px)->pp->sign = sgn;
        (*px)->pq->sign = 1;
        _atanrat(px, precision);
    }
    if (rat_gt(*px, pi_over_two, precision))
    {
        subrat(px, pi, precision);
    }
}
