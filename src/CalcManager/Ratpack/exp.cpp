// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//-----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           exp.c
//  Copyright      (C) 1995-96 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Contains exp, and log functions for rationals
//
//
//-----------------------------------------------------------------------------
#include "ratpak.h"

//-----------------------------------------------------------------------------
//
//  FUNCTION: exprat
//
//  ARGUMENTS: x PRAT representation of number to exponentiate
//
//  RETURN: exp  of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___
//   \  ]                                               X
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j      j+1
//   /__]
//   j=0
//
//   thisterm  = X ;  and stop when thisterm < precision used.
//           0                              n
//
//-----------------------------------------------------------------------------

void _exprat(_Inout_ PRAT* px, int32_t precision)

{
    CREATETAYLOR();

    addnum(&(pret->pp), num_one, BASEX);
    addnum(&(pret->pq), num_one, BASEX);
    DUPRAT(thisterm, pret);

    n2 = i32tonum(0L, BASEX);

    do
    {
        NEXTTERM(*px, INC(n2) DIVNUM(n2), precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();
}

void exprat(_Inout_ PRAT* px, uint32_t radix, int32_t precision)

{
    PRAT pwr = nullptr;
    PRAT pint = nullptr;

    if (rat_gt(*px, rat_max_exp, precision) || rat_lt(*px, rat_min_exp, precision))
    {
        // Don't attempt exp of anything large.
        throw(CALC_E_DOMAIN);
    }

    DUPRAT(pwr, rat_exp);
    DUPRAT(pint, *px);

    intrat(&pint, radix, precision);

    const int32_t intpwr = rattoi32(pint, radix, precision);
    ratpowi32(&pwr, intpwr, precision);

    subrat(px, pint, precision);

    // It just so happens to be an integral power of e.
    if (rat_gt(*px, rat_negsmallest, precision) && rat_lt(*px, rat_smallest, precision))
    {
        DUPRAT(*px, pwr);
    }
    else
    {
        _exprat(px, precision);
        mulrat(px, pwr, precision);
    }

    destroyrat(pwr);
    destroyrat(pint);
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: lograt, _lograt
//
//  ARGUMENTS: x PRAT representation of number to logarithim
//
//  RETURN: log  of x in PRAT form.
//
//  EXPLANATION: This uses Taylor series
//
//    n
//   ___
//   \  ]                                             j*(1-X)
//    \   thisterm  ; where thisterm   = thisterm  * ---------
//    /           j                 j+1          j      j+1
//   /__]
//   j=0
//
//   thisterm  = X ;  and stop when thisterm < precision used.
//           0                              n
//
//   Number is scaled between one and e_to_one_half prior to taking the
//   log. This is to keep execution time from exploding.
//
//
//-----------------------------------------------------------------------------

void _lograt(PRAT* px, int32_t precision)

{
    CREATETAYLOR();

    createrat(thisterm);

    // sub one from x
    (*px)->pq->sign *= -1;
    addnum(&((*px)->pp), (*px)->pq, BASEX);
    (*px)->pq->sign *= -1;

    DUPRAT(pret, *px);
    DUPRAT(thisterm, *px);

    n2 = i32tonum(1L, BASEX);
    (*px)->pp->sign *= -1;

    do
    {
        NEXTTERM(*px, MULNUM(n2) INC(n2) DIVNUM(n2), precision);
        TRIMTOP(*px, precision);
    } while (!SMALL_ENOUGH_RAT(thisterm, precision));

    DESTROYTAYLOR();
}

void lograt(_Inout_ PRAT* px, int32_t precision)

{
    PRAT pwr = nullptr;    // pwr is the large scaling factor.
    PRAT offset = nullptr; // offset is the incremental scaling factor.

    // Check for someone taking the log of zero or a negative number.
    if (rat_le(*px, rat_zero, precision))
    {
        throw(CALC_E_DOMAIN);
    }

    // Get number > 1, for scaling
    bool fneglog = rat_lt(*px, rat_one, precision);
    if (fneglog)
    {
        PNUMBER pnumtemp = (*px)->pp;
        (*px)->pp = (*px)->pq;
        (*px)->pq = pnumtemp;
    }

    // Scale the number within BASEX factor of 1, for the large scale.
    // log(x*2^(BASEXPWR*k)) = BASEXPWR*k*log(2)+log(x)
    if (LOGRAT2(*px) > 1)
    {
        const int32_t intpwr = LOGRAT2(*px) - 1;
        (*px)->pq->exp += intpwr;
        pwr = i32torat(intpwr * BASEXPWR);
        mulrat(&pwr, ln_two, precision);
        // ln(x+e)-ln(x) looks close to e when x is close to one using some
        // expansions.  This means we can trim past precision digits+1.
        TRIMTOP(*px, precision);
    }
    else
    {
        DUPRAT(pwr, rat_zero);
    }

    DUPRAT(offset, rat_zero);
    // Scale the number between 1 and e_to_one_half, for the small scale.
    while (rat_gt(*px, e_to_one_half, precision))
    {
        divrat(px, e_to_one_half, precision);
        addrat(&offset, rat_one, precision);
    }

    _lograt(px, precision);

    // Add the large and small scaling factors, take into account
    // small scaling was done in e_to_one_half chunks.
    divrat(&offset, rat_two, precision);
    addrat(&pwr, offset, precision);

    // And add the resulting scaling factor to the answer.
    addrat(px, pwr, precision);

    trimit(px, precision);

    // If number started out < 1 rescale answer to negative.
    if (fneglog)
    {
        (*px)->pp->sign *= -1;
    }

    destroyrat(offset);
    destroyrat(pwr);
}

void log10rat(_Inout_ PRAT* px, int32_t precision)

{
    lograt(px, precision);
    divrat(px, ln_ten, precision);
}

//
// return if the given x is even number. The assumption here is its denominator is 1 and we are testing the numerator is
// even or not
bool IsEven(PRAT x, uint32_t radix, int32_t precision)
{
    PRAT tmp = nullptr;
    bool bRet = false;

    DUPRAT(tmp, x);
    divrat(&tmp, rat_two, precision);
    fracrat(&tmp, radix, precision);
    addrat(&tmp, tmp, precision);
    subrat(&tmp, rat_one, precision);
    if (rat_lt(tmp, rat_zero, precision))
    {
        bRet = true;
    }

    destroyrat(tmp);
    return bRet;
}

//---------------------------------------------------------------------------
//
//  FUNCTION: powrat
//
//  ARGUMENTS: PRAT *px, PRAT y, uint32_t radix, int32_t precision
//
//  RETURN: none, sets *px to *px to the y.
//
//  EXPLANATION: Calculates the power of both px and
//  handles special cases where px is a perfect root.
//  Assumes, all checking has been done on validity of numbers.
//
//
//---------------------------------------------------------------------------
void powrat(_Inout_ PRAT* px, _In_ PRAT y, uint32_t radix, int32_t precision)
{
    // Handle cases where px or y is 0 by calling powratcomp directly
    if (zerrat(*px) || zerrat(y))
    {
        powratcomp(px, y, radix, precision);
        return;
    }
    // When y is 1, return px
    if (rat_equ(y, rat_one, precision))
    {
        return;
    }

    try
    {
        powratNumeratorDenominator(px, y, radix, precision);
    }
    catch (...)
    {
        // If calculating the power using numerator/denominator
        // failed, fall back to the less accurate method of
        // passing in the original y
        powratcomp(px, y, radix, precision);
    }
}

void powratNumeratorDenominator(_Inout_ PRAT* px, _In_ PRAT y, uint32_t radix, int32_t precision)
{
    // Prepare rationals
    PRAT yNumerator = nullptr;
    PRAT yDenominator = nullptr;
    DUPRAT(yNumerator, rat_zero);   // yNumerator->pq is 1 one
    DUPRAT(yDenominator, rat_zero); // yDenominator->pq is 1 one
    DUPNUM(yNumerator->pp, y->pp);
    DUPNUM(yDenominator->pp, y->pq);

    // Calculate the following use the Powers of Powers rule:
    // px ^ (yNum/yDenom) == px ^ yNum ^ (1/yDenom)
    // 1. For px ^ yNum, we call powratcomp directly which will call ratpowi32
    //    and store the result in pxPowNum
    // 2. For pxPowNum ^ (1/yDenom), we call powratcomp
    // 3. Validate the result of 2 by adding/subtracting 0.5, flooring and call powratcomp with yDenom
    //    on the floored result.

    // 1. Initialize result.
    PRAT pxPow = nullptr;
    DUPRAT(pxPow, *px);

    // 2. Calculate pxPow = px ^ yNumerator
    // if yNumerator is not 1
    if (!rat_equ(yNumerator, rat_one, precision))
    {
        powratcomp(&pxPow, yNumerator, radix, precision);
    }

    // 2. Calculate pxPowNumDenom = pxPowNum ^ (1/yDenominator),
    // if yDenominator is not 1
    if (!rat_equ(yDenominator, rat_one, precision))
    {
        // Calculate 1 over y
        PRAT oneoveryDenom = nullptr;
        DUPRAT(oneoveryDenom, rat_one);
        divrat(&oneoveryDenom, yDenominator, precision);

        // ##################################
        // Take the oneoveryDenom power
        // ##################################
        PRAT originalResult = nullptr;
        DUPRAT(originalResult, pxPow);
        powratcomp(&originalResult, oneoveryDenom, radix, precision);

        // ##################################
        // Round the originalResult to roundedResult
        // ##################################
        PRAT roundedResult = nullptr;
        DUPRAT(roundedResult, originalResult);
        if (roundedResult->pp->sign == -1)
        {
            subrat(&roundedResult, rat_half, precision);
        }
        else
        {
            addrat(&roundedResult, rat_half, precision);
        }
        intrat(&roundedResult, radix, precision);

        // ##################################
        // Take the yDenom power of the roundedResult.
        // ##################################
        PRAT roundedPower = nullptr;
        DUPRAT(roundedPower, roundedResult);
        powratcomp(&roundedPower, yDenominator, radix, precision);

        // ##################################
        // if roundedPower == px,
        // we found an exact power in roundedResult
        // ##################################
        if (rat_equ(roundedPower, pxPow, precision))
        {
            DUPRAT(*px, roundedResult);
        }
        else
        {
            DUPRAT(*px, originalResult);
        }

        destroyrat(oneoveryDenom);
        destroyrat(originalResult);
        destroyrat(roundedResult);
        destroyrat(roundedPower);
    }
    else
    {
        DUPRAT(*px, pxPow);
    }

    destroyrat(yNumerator);
    destroyrat(yDenominator);
    destroyrat(pxPow);
}

//---------------------------------------------------------------------------
//
//  FUNCTION: powratcomp
//
//  ARGUMENTS: PRAT *px, and PRAT y
//
//  RETURN: none, sets *px to *px to the y.
//
//  EXPLANATION: This uses x^y=e(y*ln(x)), or a more exact calculation where
//  y is an integer.
//  Assumes, all checking has been done on validity of numbers.
//
//
//---------------------------------------------------------------------------
void powratcomp(_Inout_ PRAT* px, _In_ PRAT y, uint32_t radix, int32_t precision)
{
    int32_t sign = SIGN(*px);

    // Take the absolute value
    (*px)->pp->sign = 1;
    (*px)->pq->sign = 1;

    if (zerrat(*px))
    {
        // *px is zero.
        if (rat_lt(y, rat_zero, precision))
        {
            throw(CALC_E_DOMAIN);
        }
        else if (zerrat(y))
        {
            // *px and y are both zero, special case a 1 return.
            DUPRAT(*px, rat_one);
            // Ensure sign is positive.
            sign = 1;
        }
    }
    else
    {
        PRAT pxint = nullptr;
        DUPRAT(pxint, *px);
        subrat(&pxint, rat_one, precision);
        if (rat_gt(pxint, rat_negsmallest, precision) && rat_lt(pxint, rat_smallest, precision) && (sign == 1))
        {
            // *px is one, special case a 1 return.
            DUPRAT(*px, rat_one);
            // Ensure sign is positive.
            sign = 1;
        }
        else
        {
            // Only do the exp if the number isn't zero or one
            PRAT podd = nullptr;
            DUPRAT(podd, y);
            fracrat(&podd, radix, precision);
            if (rat_gt(podd, rat_negsmallest, precision) && rat_lt(podd, rat_smallest, precision))
            {
                // If power is an integer let ratpowi32 deal with it.
                PRAT iy = nullptr;
                DUPRAT(iy, y);
                subrat(&iy, podd, precision);
                int32_t inty = rattoi32(iy, radix, precision);

                PRAT plnx = nullptr;
                DUPRAT(plnx, *px);
                lograt(&plnx, precision);
                mulrat(&plnx, iy, precision);
                if (rat_gt(plnx, rat_max_exp, precision) || rat_lt(plnx, rat_min_exp, precision))
                {
                    // Don't attempt exp of anything large or small.A
                    destroyrat(plnx);
                    destroyrat(iy);
                    destroyrat(pxint);
                    destroyrat(podd);
                    throw(CALC_E_DOMAIN);
                }
                destroyrat(plnx);
                ratpowi32(px, inty, precision);
                if ((inty & 1) == 0)
                {
                    sign = 1;
                }
                destroyrat(iy);
            }
            else
            {
                // power is a fraction
                if (sign == -1)
                {
                    // Need to throw an error if the exponent has an even denominator.
                    // As a first step, the numerator and denominator must be divided by 2 as many times as
                    //     possible, so that 2/6 is allowed.
                    // If the final numerator is still even, the end result should be positive.
                    PRAT pNumerator = nullptr;
                    PRAT pDenominator = nullptr;
                    bool fBadExponent = false;

                    // Get the numbers in arbitrary precision rational number format
                    DUPRAT(pNumerator, rat_zero);   // pNumerator->pq is 1 one
                    DUPRAT(pDenominator, rat_zero); // pDenominator->pq is 1 one

                    DUPNUM(pNumerator->pp, y->pp);
                    pNumerator->pp->sign = 1;
                    DUPNUM(pDenominator->pp, y->pq);
                    pDenominator->pp->sign = 1;

                    while (IsEven(pNumerator, radix, precision) && IsEven(pDenominator, radix, precision)) // both Numerator & denominator is even
                    {
                        divrat(&pNumerator, rat_two, precision);
                        divrat(&pDenominator, rat_two, precision);
                    }
                    if (IsEven(pDenominator, radix, precision)) // denominator is still even
                    {
                        fBadExponent = true;
                    }
                    if (IsEven(pNumerator, radix, precision)) // numerator is still even
                    {
                        sign = 1;
                    }
                    destroyrat(pNumerator);
                    destroyrat(pDenominator);

                    if (fBadExponent)
                    {
                        throw(CALC_E_DOMAIN);
                    }
                }
                else
                {
                    // If the exponent is not odd disregard the sign.
                    sign = 1;
                }

                lograt(px, precision);
                mulrat(px, y, precision);
                exprat(px, radix, precision);
            }
            destroyrat(podd);
        }
        destroyrat(pxint);
    }
    (*px)->pp->sign *= sign;
}
