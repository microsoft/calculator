// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//-----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           fact.c
//  Copyright      (C) 1995-96 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Contains fact(orial) and supporting _gamma functions.
//
//-----------------------------------------------------------------------------
#include "ratpak.h"

#define ABSRAT(x) (((x)->pp->sign = 1), ((x)->pq->sign = 1))
#define NEGATE(x) ((x)->pp->sign *= -1)

//-----------------------------------------------------------------------------
//
//  FUNCTION: factrat, _gamma, gamma
//
//  ARGUMENTS:  x PRAT representation of number to take the sine of
//
//  RETURN: factorial of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//      n
//     ___    2j
//   n \  ]  A       1          A
//  A   \   -----[ ---- - ---------------]
//      /   (2j)!  n+2j   (n+2j+1)(2j+1)
//     /__]
//     j=0
//
//                        / oo
//                        |    n-1 -x     __
//  This was derived from |   x   e  dx = |
//                        |               | (n) { = (n-1)! for +integers}
//                        / 0
//
//  It can be shown that the above series is within precision if A is chosen
//  big enough.
//                          A    n  precision
//  Based on the relation ne  = A 10            A was chosen as
//
//             precision
//  A = ln(Base         /n)+1
//  A += n*ln(A)  This is close enough for precision > base and n < 1.5
//
//
//-----------------------------------------------------------------------------

void _gamma(PRAT* pn, uint32_t radix, int32_t precision)

{
    PRAT factorial = nullptr;
    PNUMBER count = nullptr;
    PRAT tmp = nullptr;
    PRAT one_pt_five = nullptr;
    PRAT a = nullptr;
    PRAT a2 = nullptr;
    PRAT term = nullptr;
    PRAT sum = nullptr;
    PRAT err = nullptr;
    PRAT mpy = nullptr;

    // Set up constants and initial conditions
    PRAT ratprec = i32torat(precision);

    // Find the best 'A' for convergence to the required precision.
    a = i32torat(radix);
    lograt(&a, precision);
    mulrat(&a, ratprec, precision);

    // Really is -ln(n)+1, but -ln(n) will be < 1
    // if we scale n between 0.5 and 1.5
    addrat(&a, rat_two, precision);
    DUPRAT(tmp, a);
    lograt(&tmp, precision);
    mulrat(&tmp, *pn, precision);
    addrat(&a, tmp, precision);
    addrat(&a, rat_one, precision);

    // Calculate the necessary bump in precision and up the precision.
    // The following code is equivalent to
    // precision += ln(exp(a)*pow(a,n+1.5))-ln(radix));
    DUPRAT(tmp, *pn);
    one_pt_five = i32torat(3L);
    divrat(&one_pt_five, rat_two, precision);
    addrat(&tmp, one_pt_five, precision);
    DUPRAT(term, a);
    powratcomp(&term, tmp, radix, precision);
    DUPRAT(tmp, a);
    exprat(&tmp, radix, precision);
    mulrat(&term, tmp, precision);
    lograt(&term, precision);
    const auto ratRadix = i32torat(radix);
    DUPRAT(tmp, ratRadix);
    lograt(&tmp, precision);
    subrat(&term, tmp, precision);
    precision += rattoi32(term, radix, precision);

    // Set up initial terms for series, refer to series in above comment block.
    DUPRAT(factorial, rat_one); // Start factorial out with one
    count = i32tonum(0L, BASEX);

    DUPRAT(mpy, a);
    powratcomp(&mpy, *pn, radix, precision);
    // a2=a^2
    DUPRAT(a2, a);
    mulrat(&a2, a, precision);

    // sum=(1/n)-(a/(n+1))
    DUPRAT(sum, rat_one);
    divrat(&sum, *pn, precision);
    DUPRAT(tmp, *pn);
    addrat(&tmp, rat_one, precision);
    DUPRAT(term, a);
    divrat(&term, tmp, precision);
    subrat(&sum, term, precision);

    DUPRAT(err, ratRadix);
    NEGATE(ratprec);
    powratcomp(&err, ratprec, radix, precision);
    divrat(&err, ratRadix, precision);

    // Just get something not tiny in term
    DUPRAT(term, rat_two);

    // Loop until precision is reached, or asked to halt.
    while (!zerrat(term) && rat_gt(term, err, precision))
    {
        addrat(pn, rat_two, precision);

        // WARNING: mixing numbers and  rationals here.
        // for speed and efficiency.
        INC(count);
        mulnumx(&(factorial->pp), count);
        INC(count)
        mulnumx(&(factorial->pp), count);

        divrat(&factorial, a2, precision);

        DUPRAT(tmp, *pn);
        addrat(&tmp, rat_one, precision);
        destroyrat(term);
        createrat(term);
        DUPNUM(term->pp, count);
        DUPNUM(term->pq, num_one);
        addrat(&term, rat_one, precision);
        mulrat(&term, tmp, precision);
        DUPRAT(tmp, a);
        divrat(&tmp, term, precision);

        DUPRAT(term, rat_one);
        divrat(&term, *pn, precision);
        subrat(&term, tmp, precision);

        divrat(&term, factorial, precision);
        addrat(&sum, term, precision);
        ABSRAT(term);
    }

    // Multiply by factor.
    mulrat(&sum, mpy, precision);

    // And cleanup
    destroyrat(ratprec);
    destroyrat(err);
    destroyrat(term);
    destroyrat(a);
    destroyrat(a2);
    destroyrat(tmp);
    destroyrat(one_pt_five);

    destroynum(count);

    destroyrat(factorial);
    destroyrat(*pn);
    DUPRAT(*pn, sum);
    destroyrat(sum);
}

void factrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT fact = nullptr;
    PRAT frac = nullptr;
    PRAT neg_rat_one = nullptr;

    if (rat_gt(*px, rat_max_fact, precision) || rat_lt(*px, rat_min_fact, precision))
    {
        // Don't attempt factorial of anything too large or small.
        throw CALC_E_OVERFLOW;
    }

    DUPRAT(fact, rat_one);

    DUPRAT(neg_rat_one, rat_one);
    neg_rat_one->pp->sign *= -1;

    DUPRAT(frac, *px);
    fracrat(&frac, radix, precision);

    // Check for negative integers and throw an error.
    if ((zerrat(frac) || (LOGRATRADIX(frac) <= -precision)) && (SIGN(*px) == -1))
    {
        throw CALC_E_DOMAIN;
    }
    while (rat_gt(*px, rat_zero, precision) && (LOGRATRADIX(*px) > -precision))
    {
        mulrat(&fact, *px, precision);
        subrat(px, rat_one, precision);
    }

    // Added to make numbers 'close enough' to integers use integer factorial.
    if (LOGRATRADIX(*px) <= -precision)
    {
        DUPRAT((*px), rat_zero);
        intrat(&fact, radix, precision);
    }

    while (rat_lt(*px, neg_rat_one, precision))
    {
        addrat(px, rat_one, precision);
        divrat(&fact, *px, precision);
    }

    if (rat_neq(*px, rat_zero, precision))
    {
        addrat(px, rat_one, precision);
        _gamma(px, radix, precision);
        mulrat(px, fact, precision);
    }
    else
    {
        DUPRAT(*px, fact);
    }

    destroyrat(fact);
    destroyrat(frac);
    destroyrat(neg_rat_one);
}
