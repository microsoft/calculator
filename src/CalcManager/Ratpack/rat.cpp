// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//-----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           rat.c
//  Copyright      (C) 1995-96 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//  Contains mul, div, add, and other support functions for rationals.
//
//
//
//-----------------------------------------------------------------------------

#include "ratpak.h"

using namespace std;

//-----------------------------------------------------------------------------
//
//    FUNCTION: gcdrat
//
//    ARGUMENTS: pointer to a rational.
//
//
//    RETURN: None, changes first pointer.
//
//    DESCRIPTION: Divides p and q in rational by the G.C.D.
//    of both.  It was hoped this would speed up some
//    calculations, and until the above trimming was done it
//    did, but after trimming gcdratting, only slows things
//    down.
//
//-----------------------------------------------------------------------------

void gcdrat(_Inout_ PRAT* pa, int32_t precision)

{
    PNUMBER pgcd = nullptr;
    PRAT a = nullptr;

    a = *pa;
    pgcd = gcd(a->pp, a->pq);

    if (!zernum(pgcd))
    {
        divnumx(&(a->pp), pgcd, precision);
        divnumx(&(a->pq), pgcd, precision);
    }

    destroynum(pgcd);
    *pa = a;

    RENORMALIZE(*pa);
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: fracrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes pointer.
//
//    DESCRIPTION: Does the rational equivalent of frac(*pa);
//
//-----------------------------------------------------------------------------

void fracrat(_Inout_ PRAT* pa, uint32_t radix, int32_t precision)
{
    // Only do the flatrat operation if number is nonzero.
    // and only if the bottom part is not one.
    if (!zernum((*pa)->pp) && !equnum((*pa)->pq, num_one))
    {
        flatrat(*pa, radix, precision);
    }

    remnum(&((*pa)->pp), (*pa)->pq, BASEX);

    // Get *pa back in the integer over integer form.
    RENORMALIZE(*pa);
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: mulrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes first pointer.
//
//    DESCRIPTION: Does the rational equivalent of *pa *= b.
//    Assumes radix is the radix of both numbers.
//
//-----------------------------------------------------------------------------

void mulrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision)

{
    // Only do the multiply if it isn't zero.
    if (!zernum((*pa)->pp))
    {
        mulnumx(&((*pa)->pp), b->pp);
        mulnumx(&((*pa)->pq), b->pq);
        trimit(pa, precision);
    }
    else
    {
        // If it is zero, blast a one in the denominator.
        DUPNUM(((*pa)->pq), num_one);
    }

#ifdef MULGCD
    gcdrat(pa);
#endif
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: divrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes first pointer.
//
//    DESCRIPTION: Does the rational equivalent of *pa /= b.
//    Assumes radix is the radix of both numbers.
//
//-----------------------------------------------------------------------------

void divrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision)

{
    if (!zernum((*pa)->pp))
    {
        // Only do the divide if the top isn't zero.
        mulnumx(&((*pa)->pp), b->pq);
        mulnumx(&((*pa)->pq), b->pp);

        if (zernum((*pa)->pq))
        {
            // raise an exception if the bottom is 0.
            throw(CALC_E_DIVIDEBYZERO);
        }
        trimit(pa, precision);
    }
    else
    {
        // Top is zero.
        if (zerrat(b))
        {
            // If bottom is zero
            // 0 / 0 is indefinite, raise an exception.
            throw(CALC_E_INDEFINITE);
        }
        else
        {
            // 0/x make a unique 0.
            DUPNUM(((*pa)->pq), num_one);
        }
    }

#ifdef DIVGCD
    gcdrat(pa);
#endif
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: subrat, _subrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes first pointer.
//
//    DESCRIPTION: Does the rational equivalent of *pa -= b.
//    Assumes base is internal throughout.
//
//    subrat does snapping to zero after subtraction. All ratpak internal
//    should use _subrat by default.
//
//-----------------------------------------------------------------------------

void subrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision)

{
    PRAT a = nullptr;
    DUPRAT(a, *pa);

    _subrat(pa, b, precision);

    _snaprat(pa, a, b, precision);
    destroyrat(a);
}

void _subrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision)

{
    b->pp->sign *= -1;
    _addrat(pa, b, precision);
    b->pp->sign *= -1;
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: addrat, _addrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes first pointer.
//
//    DESCRIPTION: Does the rational equivalent of *pa += b.
//    Assumes base is internal throughout.
//
//    addrat does snapping to zero after addition. All ratpak internal should
//    use _addrat by default.
//
//-----------------------------------------------------------------------------

void addrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision)
{
    PRAT a = nullptr;
    DUPRAT(a, *pa);

    _addrat(pa, b, precision);

    _snaprat(pa, a, b, precision);
    destroyrat(a);
}

void _addrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision)
{
    PNUMBER bot = nullptr;

    if (equnum((*pa)->pq, b->pq))
    {
        // Very special case, q's match.,
        // make sure signs are involved in the calculation
        // we have to do this since the optimization here is only
        // working with the top half of the rationals.
        (*pa)->pp->sign *= (*pa)->pq->sign;
        (*pa)->pq->sign = 1;
        b->pp->sign *= b->pq->sign;
        b->pq->sign = 1;
        addnum(&((*pa)->pp), b->pp, BASEX);
    }
    else
    {
        // Usual case q's aren't the same.
        DUPNUM(bot, (*pa)->pq);
        mulnumx(&bot, b->pq);
        mulnumx(&((*pa)->pp), b->pq);
        mulnumx(&((*pa)->pq), b->pp);
        addnum(&((*pa)->pp), (*pa)->pq, BASEX);
        destroynum((*pa)->pq);
        (*pa)->pq = bot;
        trimit(pa, precision);

        // Get rid of negative zeros here.
        (*pa)->pp->sign *= (*pa)->pq->sign;
        (*pa)->pq->sign = 1;
    }

#ifdef ADDGCD
    gcdrat(pa);
#endif
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: rootrat
//
//  PARAMETERS: y prat representation of number to take the root of
//              n prat representation of the root to take.
//
//  RETURN: bth root of a in rat form.
//
//  EXPLANATION: This is now a stub function to powrat().
//
//-----------------------------------------------------------------------------

void rootrat(_Inout_ PRAT* py, _In_ PRAT n, uint32_t radix, int32_t precision)
{
    // Initialize 1/n
    PRAT oneovern = nullptr;
    DUPRAT(oneovern, rat_one);
    divrat(&oneovern, n, precision);

    powrat(py, oneovern, radix, precision);

    destroyrat(oneovern);
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: zerrat
//
//    ARGUMENTS: Rational number.
//
//    RETURN: Boolean
//
//    DESCRIPTION: Returns true if input is zero.
//    False otherwise.
//
//-----------------------------------------------------------------------------

bool zerrat(_In_ PRAT a)

{
    return (zernum(a->pp));
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: _snaprat
//
//    ARGUMENTS: r prat to potentially snap to zero
//               a, b prats for comparison.
//               b is optional and can be null for unary operations.
//
//    DESCRIPTION: If |pr| is magnitude smaller than |a| or |b| beyond
//    precision, snap pr to 0. This is to address issues with exposing tiny
//    residuals to the user in calculations that should yield zero.
// 
//    Example: let rat a = sqrt(2.25), rat b = 1.5. r = a - b should be zero.
//    However, rat a is an approximation of sqrt(2.25) and very close to 1.5,
//    but not exactly 1.5. The result r is a tiny residual close to zero, but
//    not zero. _snaprat can be used to check if r is small enough compared to
//    a or b, and snap it to zero if so. Without this, users may see unexpected
//    tiny values in results that should be zero.
//
//    log(a) where a is very close to 1 is another example. The result should be
//    zero.
//
//    trimit also removes digits but it's for a different reason.
//
//    Notice that trigonometric functions sinrat/cosrat have specifically
//    adjusted for approximation errors.
//
//-----------------------------------------------------------------------------

void _snaprat(_Inout_ PRAT* pr, _In_ PRAT a, _In_opt_ PRAT b, int32_t precision)
{
    PRAT threshold = nullptr;
    if (!b)
    {
        DUPRAT(threshold, a);
        ABSRAT(threshold);
    }
    else
    {
        PRAT absA = nullptr;
        PRAT absB = nullptr;
        DUPRAT(absA, a);
        DUPRAT(absB, b);
        ABSRAT(absA);
        ABSRAT(absB);

        if (rat_lt(absA, absB, precision))
        {
            DUPRAT(threshold, absB);
        }
        else
        {
            DUPRAT(threshold, absA);
        }
        destroyrat(absA);
        destroyrat(absB);
    }
    mulrat(&threshold, rat_smallest, precision);

    PRAT absR = nullptr;
    DUPRAT(absR, *pr);
    ABSRAT(absR);

    // if absResult < threshold => snap to zero
    if (rat_lt(absR, threshold, precision))
    {
        DUPRAT(*pr, rat_zero);
    }

    destroyrat(absR);
    destroyrat(threshold);
}
