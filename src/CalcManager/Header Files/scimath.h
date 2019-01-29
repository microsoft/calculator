// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
#include "Ratpack/ratpak.h"

//
// Unary functions
//
void NumObjInvert(PRAT *phno, int32_t precision);
void NumObjNegate(PRAT *phno);
void NumObjAbs(PRAT *phno);

void NumObjSin(PRAT *phno, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
void NumObjCos(PRAT *phno, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
void NumObjTan(PRAT *phno, ANGLE_TYPE angletype, uint32_t radix, int32_t precision);
void NumObjAntiLog10(PRAT *phno, uint32_t radix, int32_t precision);

void NumObjNot(PRAT *phno, bool fIntegerMode, PRAT chopNum, uint32_t radix, int32_t precision);

//
// Comparison functions
//
bool NumObjIsZero(PRAT hno);
bool NumObjIsLess(PRAT hno1, PRAT hno2, int32_t precision);
bool NumObjIsLessEq(PRAT hno1, PRAT hno2, int32_t precision);
bool NumObjIsGreaterEq(PRAT hno1, PRAT hno2, int32_t precision);
bool NumObjIsEq(PRAT hno1, PRAT hno2, int32_t precision);

//
// Assignment operator.  ('=' in C language)
//
void NumObjAssign(PRAT *phnol, PRAT hnor);

//
// Data type conversion functions
//
void NumObjSetIntValue(PRAT *phnol, LONG i );
void NumObjSetUlonglongValue(PRAT *phnol, ULONGLONG ul, uint32_t radix, int32_t precision);

ULONGLONG NumObjGetUlValue( PRAT hnol, uint32_t radix, int32_t precision);

// Returns a string representation of hnoNum
std::wstring NumObjToString(PRAT hnoNum, uint32_t radix, NUMOBJ_FMT fmt, int32_t precision);

//
//  NumObjGetExp
//
//  returns an int that equals the exponent of the NumObj
//
int32_t NumObjGetExp(PRAT hno);

//
//  NumObjDestroy( hno )
//
//      call this when you nolonger need the NumObj.  Failure to do so
//  will result in memory leaks.
//
void NumObjDestroy(PRAT *phno);
