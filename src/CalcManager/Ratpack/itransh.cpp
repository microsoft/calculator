// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//-----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           itransh.c
//  Copyright      (C) 1995-97 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//    Contains inverse hyperbolic sin, cos, and tan functions.
//
//  Special Information
//
//
//-----------------------------------------------------------------------------
#include "ratpak.h"

//-----------------------------------------------------------------------------
//
//  FUNCTION: asinhrat
//
//  ARGUMENTS:  x PRAT representation of number to take the inverse
//    hyperbolic sine of
//  RETURN: asinh of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___                                                   2 2
//   \  ]                                           -(2j+1) X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j+2)*(2j+3)
//   /__]
//   j=0
//
//   thisterm  = X ;  and stop when thisterm < precision used.
//           0                              n
//
//   For abs(x) < .85, and
//
//   asinh(x) = log(x+sqrt(x^2+1))
//
//   For abs(x) >= .85
//
//-----------------------------------------------------------------------------

void asinhrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT neg_pt_eight_five = nullptr;

    DUPRAT(neg_pt_eight_five, pt_eight_five);
    neg_pt_eight_five->pp->sign *= -1;
    if (rat_gt(*px, pt_eight_five, precision) || rat_lt(*px, neg_pt_eight_five, precision))
    {
        PRAT ptmp = nullptr;
        DUPRAT(ptmp, (*px));
        mulrat(&ptmp, *px, precision);
        addrat(&ptmp, rat_one, precision);
        rootrat(&ptmp, rat_two, radix, precision);
        addrat(px, ptmp, precision);
        lograt(px, precision);
        destroyrat(ptmp);
    }
    else
    {
        CREATETAYLOR();
        xx->pp->sign *= -1;

        DUPRAT(pret, (*px));
        DUPRAT(thisterm, (*px));

        DUPNUM(n2, num_one);

        do
        {
            NEXTTERM(xx, MULNUM(n2) MULNUM(n2) INC(n2) DIVNUM(n2) INC(n2) DIVNUM(n2), precision);
        } while (!SMALL_ENOUGH_RAT(thisterm, precision));

        DESTROYTAYLOR();
    }
    destroyrat(neg_pt_eight_five);
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: acoshrat
//
//  ARGUMENTS:  x PRAT representation of number to take the inverse
//    hyperbolic cose of
//  RETURN: acosh of x in PRAT form.
//
//  EXPLANATION: This uses
//
//   acosh(x)=ln(x+sqrt(x^2-1))
//
//   For x >= 1
//
//-----------------------------------------------------------------------------

void acoshrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    if (rat_lt(*px, rat_one, precision))
    {
        throw CALC_E_DOMAIN;
    }
    else
    {
        PRAT ptmp = nullptr;
        DUPRAT(ptmp, (*px));
        mulrat(&ptmp, *px, precision);
        subrat(&ptmp, rat_one, precision);
        rootrat(&ptmp, rat_two, radix, precision);
        addrat(px, ptmp, precision);
        lograt(px, precision);
        destroyrat(ptmp);
    }
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: atanhrat
//
//  ARGUMENTS:  x PRAT representation of number to take the inverse
//              hyperbolic tangent of
//
//  RETURN: atanh of x in PRAT form.
//
//  EXPLANATION: This uses
//
//             1     x+1
//  atanh(x) = -*ln(----)
//             2     x-1
//
//-----------------------------------------------------------------------------

void atanhrat(_Inout_ PRAT* px, int32_t precision)

{
    PRAT ptmp = nullptr;
    DUPRAT(ptmp, (*px));
    subrat(&ptmp, rat_one, precision);
    addrat(px, rat_one, precision);
    divrat(px, ptmp, precision);
    (*px)->pp->sign *= -1;
    lograt(px, precision);
    divrat(px, rat_two, precision);
    destroyrat(ptmp);
}
