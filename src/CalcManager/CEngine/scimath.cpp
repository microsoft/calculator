// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Header Files/CalcEngine.h"

using namespace std;

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
*    #          #                           #####                          *
*    #         #              #             #    #                         *
*    #         #  #        #  #             #    #                         *
*    #        ###            ###            #    #                         *
*    # # ###   #  # # ###  #  #   ###       #####  # ###  ###   ###        *
*    # ##   #  #  # ##   # #  #  #   #      #      ##    #   # #           *
*    # #    #  #  # #    # #  #  #####      #      #     ##### #           *
*    # #    #  #  # #    # #  #  #          #      #     #     #    ##     *
*    # #    #  #  # #    # #   #  ###       #      #      ###   ### ##     *
*                                                                          *
*                                                                          *
*              Infinte Precision Production Version                        *
*                                                                          *
\**************************************************************************/
//
// RETAIL version of NUMOBJ math that uses Infinite Precision
//
// History
//
//  16-Nov-1996 Wrote it
//  whenever-97 Rewrote it using improved ratpak model
//

/*****************************************************************\
*
* Generic Math Package support routines and variables
*
* History:
*   01-Dec-1996 Wrote them
*   whenever-97 Rewrote them
*
\*****************************************************************/

/*****************************************************************\
*
* Unary functions
*
* History:
*   01-Dec-1996 Wrote them
*   whenever-97 Rewrote them
*
\*****************************************************************/

void NumObjInvert(PRAT * phno, int32_t precision)
{
    PRAT hno = nullptr;

    NumObjAssign( &hno, rat_one);
    divrat( &hno, *phno, precision);
    NumObjAssign( phno, hno );
    NumObjDestroy( &hno );
}

void NumObjNegate(PRAT *phno)
{
    (*phno)->pp->sign = -(*phno)->pp->sign;
}

void NumObjAbs(PRAT *phno)
{
    (*phno)->pp->sign = 1;
    (*phno)->pq->sign = 1;
}

void NumObjAntiLog10(PRAT *phno, uint32_t radix, int32_t precision)
{
    PRAT hno = nullptr;

    NumObjSetIntValue( &hno, 10 );
    powrat( &hno, *phno, radix, precision);
    NumObjAssign( phno, hno );
    NumObjDestroy( &hno );
}

void NumObjNot(PRAT *phno, bool fIntegerMode, PRAT chopNum, uint32_t radix, int32_t precision)
{
    if (radix == 10 && !fIntegerMode)
    {
        intrat( phno, radix, precision);
        addrat( phno, rat_one, precision);
        NumObjNegate( phno );
    }
    else
    {

        xorrat( phno, chopNum, radix, precision);
    }
}

void NumObjSin(PRAT *phno, ANGLE_TYPE angletype, uint32_t radix, int32_t precision )
{
    sinanglerat(phno, angletype, radix, precision);
}

void NumObjCos(PRAT *phno, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    cosanglerat(phno, angletype, radix, precision);
}

void NumObjTan(PRAT *phno, ANGLE_TYPE angletype, uint32_t radix, int32_t precision)
{
    tananglerat(phno, angletype, radix, precision);
}

/******************************************************************\
*
* Number format conversion routines
*
* History:
*   06-Dec-1996 wrote them
\******************************************************************/
void NumObjSetIntValue(PRAT *phnol, LONG i )
{
    PRAT pr = nullptr;

    pr = longtorat( i );
    NumObjAssign( phnol, pr );
    destroyrat(pr);
}

void NumObjSetIUlongValue(PRAT *phnol, ULONG ul )
{
    PRAT pr = nullptr;

    pr = Ulongtorat( ul );
    NumObjAssign( phnol, pr );
    destroyrat(pr);
}

// A Set with 64 bit number
void NumObjSetUlonglongValue(PRAT *phnol, ULONGLONG ul, uint32_t radix, int32_t precision)
{
    ULONG hi, lo;
    PRAT hno = nullptr;

    hi = HIDWORD(ul);
    lo = LODWORD(ul);
    NumObjSetIUlongValue(phnol, hi);
    NumObjSetIntValue(&hno, 32);
    lshrat(phnol, hno, radix, precision);
    NumObjSetIUlongValue(&hno, lo);
    orrat(phnol, hno, radix, precision);
    NumObjDestroy(&hno);
}

ULONGLONG NumObjGetUlValue( PRAT hnol, uint32_t radix, int32_t precision)
{
    return rattoUlonglong(hnol, radix, precision);
}

wstring NumObjToString(PRAT hnoNum, uint32_t radix, NUMOBJ_FMT fmt, int32_t precision)
{
    return RatToString(hnoNum, fmt, radix, precision);
}

bool NumObjIsZero(PRAT hno)
{
    return zerrat(hno);
}

bool NumObjIsLess(PRAT hno1, PRAT hno2, int32_t precision)
{
    return rat_lt(hno1, hno2, precision);
}

bool NumObjIsLessEq(PRAT hno1, PRAT hno2, int32_t precision)
{
    return rat_le(hno1, hno2, precision);
}

bool NumObjIsGreaterEq(PRAT hno1, PRAT hno2, int32_t precision)
{
    return rat_ge(hno1, hno2, precision);
}

bool NumObjIsEq(PRAT hno1, PRAT hno2, int32_t precision)
{
    return rat_equ(hno1, hno2, precision);
}

void NumObjAssign(PRAT *phnol, PRAT hnor)
{
    DUPRAT(*phnol, hnor);
}

int32_t NumObjGetExp(PRAT hno)
{
    return LOGRATRADIX(hno);
}

void NumObjDestroy(PRAT *phno)
{
    destroyrat(*phno);
}
