// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//---------------------------------------------------------------------------
//  Package Title  ratpak
//  File           num.c
//  Copyright      (C) 1995-99 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Contains routines for and, or, xor, not and other support
//
//---------------------------------------------------------------------------
#include "ratpak.h"

using namespace std;

void lshrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision)

{
    PRAT pwr = nullptr;

    intrat(pa, radix, precision);
    if (!zernum((*pa)->pp))
    {
        // If input is zero we're done.
        if (rat_gt(b, rat_max_exp, precision))
        {
            // Don't attempt lsh of anything big
            throw(CALC_E_DOMAIN);
        }
        const int32_t intb = rattoi32(b, radix, precision);
        DUPRAT(pwr, rat_two);
        ratpowi32(&pwr, intb, precision);
        mulrat(pa, pwr, precision);
        destroyrat(pwr);
    }
}

void rshrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision)

{
    PRAT pwr = nullptr;

    intrat(pa, radix, precision);
    if (!zernum((*pa)->pp))
    {
        // If input is zero we're done.
        if (rat_lt(b, rat_min_exp, precision))
        {
            // Don't attempt rsh of anything big and negative.
            throw(CALC_E_DOMAIN);
        }
        const int32_t intb = rattoi32(b, radix, precision);
        DUPRAT(pwr, rat_two);
        ratpowi32(&pwr, intb, precision);
        divrat(pa, pwr, precision);
        destroyrat(pwr);
    }
}

void boolrat(PRAT* pa, PRAT b, int func, uint32_t radix, int32_t precision);
void boolnum(PNUMBER* pa, PNUMBER b, int func);

enum
{
    FUNC_AND,
    FUNC_OR,
    FUNC_XOR
} BOOL_FUNCS;

void andrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision)

{
    boolrat(pa, b, FUNC_AND, radix, precision);
}

void orrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision)

{
    boolrat(pa, b, FUNC_OR, radix, precision);
}

void xorrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision)

{
    boolrat(pa, b, FUNC_XOR, radix, precision);
}

//---------------------------------------------------------------------------
//
//    FUNCTION: boolrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes pointer.
//
//    DESCRIPTION: Does the rational equivalent of *pa op= b;
//
//---------------------------------------------------------------------------

void boolrat(PRAT* pa, PRAT b, int func, uint32_t radix, int32_t precision)

{
    PRAT tmp = nullptr;
    intrat(pa, radix, precision);
    DUPRAT(tmp, b);
    intrat(&tmp, radix, precision);

    boolnum(&((*pa)->pp), tmp->pp, func);
    destroyrat(tmp);
}

//---------------------------------------------------------------------------
//
//    FUNCTION: boolnum
//
//    ARGUMENTS: pointer to a number a second number
//
//    RETURN: None, changes first pointer.
//
//    DESCRIPTION: Does the number equivalent of *pa &= b.
//    radix doesn't matter for logicals.
//    WARNING: Assumes numbers are unsigned.
//
//---------------------------------------------------------------------------

void boolnum(PNUMBER* pa, PNUMBER b, int func)

{
    PNUMBER c = nullptr;
    PNUMBER a = nullptr;
    MANTTYPE* pcha;
    MANTTYPE* pchb;
    MANTTYPE* pchc;
    int32_t cdigits;
    int32_t mexp;
    MANTTYPE da;
    MANTTYPE db;

    a = *pa;
    cdigits = max(a->cdigit + a->exp, b->cdigit + b->exp) - min(a->exp, b->exp);
    createnum(c, cdigits);
    c->exp = min(a->exp, b->exp);
    mexp = c->exp;
    c->cdigit = cdigits;
    pcha = a->mant;
    pchb = b->mant;
    pchc = c->mant;
    for (; cdigits > 0; cdigits--, mexp++)
    {
        da = (((mexp >= a->exp) && (cdigits + a->exp - c->exp > (c->cdigit - a->cdigit))) ? *pcha++ : 0);
        db = (((mexp >= b->exp) && (cdigits + b->exp - c->exp > (c->cdigit - b->cdigit))) ? *pchb++ : 0);
        switch (func)
        {
        case FUNC_AND:
            *pchc++ = da & db;
            break;
        case FUNC_OR:
            *pchc++ = da | db;
            break;
        case FUNC_XOR:
            *pchc++ = da ^ db;
            break;
        }
    }
    c->sign = a->sign;
    while (c->cdigit > 1 && *(--pchc) == 0)
    {
        c->cdigit--;
    }
    destroynum(*pa);
    *pa = c;
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: remrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes pointer.
//
//    DESCRIPTION: Calculate the remainder of *pa / b,
//                 equivalent of 'pa % b' in C/C++ and produces a result
//                 that is either zero or has the same sign as the dividend.
//
//-----------------------------------------------------------------------------

void remrat(_Inout_ PRAT* pa, _In_ PRAT b)

{
    if (zerrat(b))
    {
        throw CALC_E_INDEFINITE;
    }

    PRAT tmp = nullptr;
    DUPRAT(tmp, b);

    mulnumx(&((*pa)->pp), tmp->pq);
    mulnumx(&(tmp->pp), (*pa)->pq);
    remnum(&((*pa)->pp), tmp->pp, BASEX);
    mulnumx(&((*pa)->pq), tmp->pq);

    // Get *pa back in the integer over integer form.
    RENORMALIZE(*pa);

    destroyrat(tmp);
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: modrat
//
//    ARGUMENTS: pointer to a rational a second rational.
//
//    RETURN: None, changes pointer.
//
//    DESCRIPTION: Calculate the remainder of *pa / b, with the sign of the result
//                 either zero or has the same sign as the divisor.
//    NOTE: When *pa or b are negative, the result won't be the same as
//          the C/C++ operator %, use remrat if it's the behavior you expect.
//
//-----------------------------------------------------------------------------

void modrat(_Inout_ PRAT* pa, _In_ PRAT b)
{
    // contrary to remrat(X, 0) returning 0, modrat(X, 0) must return X
    if (zerrat(b))
    {
        return;
    }

    PRAT tmp = nullptr;
    DUPRAT(tmp, b);

    auto needAdjust = (SIGN(*pa) == -1 ? (SIGN(b) == 1) : (SIGN(b) == -1));

    mulnumx(&((*pa)->pp), tmp->pq);
    mulnumx(&(tmp->pp), (*pa)->pq);
    remnum(&((*pa)->pp), tmp->pp, BASEX);
    mulnumx(&((*pa)->pq), tmp->pq);

    if (needAdjust && !zerrat(*pa))
    {
        addrat(pa, b, BASEX);
    }

    // Get *pa back in the integer over integer form.
    RENORMALIZE(*pa);

    destroyrat(tmp);
}
