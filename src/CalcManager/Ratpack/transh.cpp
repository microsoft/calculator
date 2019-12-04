// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//-----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           transh.c
//  Copyright      (C) 1995-96 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Contains hyperbolic sin, cos, and tan for rationals.
//
//
//-----------------------------------------------------------------------------
#include "ratpak.h"

bool IsValidForHypFunc(PRAT px, int32_t precision)
{
    PRAT ptmp = nullptr;
    bool bRet = true;

    DUPRAT(ptmp, rat_min_exp);
    divrat(&ptmp, rat_ten, precision);
    if (rat_lt(px, ptmp, precision))
    {
        bRet = false;
    }
    destroyrat(ptmp);
    return bRet;
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: sinhrat, _sinhrat
//
//  ARGUMENTS:  x PRAT representation of number to take the sine hyperbolic
//    of
//  RETURN: sinh of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___    2j+1
//   \  ]  X
//    \   ---------
//    /    (2j+1)!
//   /__]
//   j=0
//          or,
//    n
//   ___                                                 2
//   \  ]                                               X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j)*(2j+1)
//   /__]
//   j=0
//
//   thisterm  = X ;  and stop when thisterm < precision used.
//           0                              n
//
//   if x is bigger than 1.0 (e^x-e^-x)/2 is used.
//
//-----------------------------------------------------------------------------

void _sinhrat(PRAT* px, int32_t precision)

{
    if (!IsValidForHypFunc(*px, precision))
    {
        // Don't attempt exp of anything large or small
        throw(CALC_E_DOMAIN);
    }

    CREATETAYLOR();

    DUPRAT(pret, *px);
    DUPRAT(thisterm, pret);

    DUPNUM(n2, num_one);

    do
    {
        NEXTTERM(xx, INC(n2) DIVNUM(n2) INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();
}

void sinhrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT tmpx = nullptr;

    if (rat_ge(*px, rat_one, precision))
    {
        DUPRAT(tmpx, *px);
        exprat(px, radix, precision);
        tmpx->pp->sign *= -1;
        exprat(&tmpx, radix, precision);
        subrat(px, tmpx, precision);
        divrat(px, rat_two, precision);
        destroyrat(tmpx);
    }
    else
    {
        _sinhrat(px, precision);
    }
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: coshrat
//
//  ARGUMENTS:  x PRAT representation of number to take the cosine
//              hyperbolic of
//
//  RETURN: cosh  of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___    2j
//   \  ]  X
//    \   ---------
//    /    (2j)!
//   /__]
//   j=0
//          or,
//    n
//   ___                                                 2
//   \  ]                                               X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j   (2j)*(2j+1)
//   /__]
//   j=0
//
//   thisterm  = 1 ;  and stop when thisterm < precision used.
//           0                              n
//
//   if x is bigger than 1.0 (e^x+e^-x)/2 is used.
//
//-----------------------------------------------------------------------------

void _coshrat(PRAT* px, uint32_t radix, int32_t precision)

{
    if (!IsValidForHypFunc(*px, precision))
    {
        // Don't attempt exp of anything large or small
        throw(CALC_E_DOMAIN);
    }

    CREATETAYLOR();

    pret->pp = i32tonum(1L, radix);
    pret->pq = i32tonum(1L, radix);

    DUPRAT(thisterm, pret)

    n2 = i32tonum(0L, radix);

    do
    {
        NEXTTERM(xx, INC(n2) DIVNUM(n2) INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();
}

void coshrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT tmpx = nullptr;

    (*px)->pp->sign = 1;
    (*px)->pq->sign = 1;
    if (rat_ge(*px, rat_one, precision))
    {
        DUPRAT(tmpx, *px);
        exprat(px, radix, precision);
        tmpx->pp->sign *= -1;
        exprat(&tmpx, radix, precision);
        addrat(px, tmpx, precision);
        divrat(px, rat_two, precision);
        destroyrat(tmpx);
    }
    else
    {
        _coshrat(px, radix, precision);
    }
    // Since *px might be epsilon below 1 due to TRIMIT
    // we need this trick here.
    if (rat_lt(*px, rat_one, precision))
    {
        DUPRAT(*px, rat_one);
    }
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: tanhrat
//
//  ARGUMENTS:  x PRAT representation of number to take the tangent
//              hyperbolic of
//
//  RETURN: tanh    of x in PRAT form.
//
//  EXPLANATION: This uses sinhrat and coshrat
//
//-----------------------------------------------------------------------------

void tanhrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT ptmp = nullptr;

    DUPRAT(ptmp, *px);
    sinhrat(px, radix, precision);
    coshrat(&ptmp, radix, precision);
    mulnumx(&((*px)->pp), ptmp->pq);
    mulnumx(&((*px)->pq), ptmp->pp);

    destroyrat(ptmp);
}
