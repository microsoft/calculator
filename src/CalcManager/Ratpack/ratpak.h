// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

//-----------------------------------------------------------------------------
//  Package Title  ratpak
//  File           ratpak.h
//  Copyright      (C) 1995-99 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Infinite precision math package header file, if you use ratpak.lib you
//  need to include this header.
//
//-----------------------------------------------------------------------------

#include <algorithm>
#include <string>
#include "CalcErr.h"
#include <cstring>              // for memmove
#include "sal_cross_platform.h" // for SAL

static constexpr uint32_t BASEXPWR = 31L;     // Internal log2(BASEX)
static constexpr uint32_t BASEX = 0x80000000; // Internal radix used in calculations, hope to raise
                                              // this to 2^32 after solving scaling problems with
                                              // overflow detection esp. in mul

typedef uint32_t MANTTYPE;
typedef uint64_t TWO_MANTTYPE;

enum class NumberFormat
{
    Float,      // returns floating point, or exponential if number is too big
    Scientific, // always returns scientific notation
    Engineering // always returns engineering notation such that exponent is a multiple of 3
};

enum class AngleType
{
    Degrees, // Calculate trig using 360 degrees per revolution
    Radians, // Calculate trig using 2 pi radians per revolution
    Gradians // Calculate trig using 400 gradians per revolution
};

//-----------------------------------------------------------------------------
//
//  NUMBER type is a representation of a generic sized generic radix number
//
//-----------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable : 4200) // nonstandard extension used : zero-sized array in struct/union
typedef struct _number
{
    int32_t sign;   // The sign of the mantissa, +1, or -1
    int32_t cdigit; // The number of digits, or what passes for digits in the
                    // radix being used.
    int32_t exp;    // The offset of digits from the radix point
                    // (decimal point in radix 10)
    MANTTYPE mant[];
    // This is actually allocated as a continuation of the
    // NUMBER structure.
} NUMBER, *PNUMBER, **PPNUMBER;
#pragma warning(pop)

//-----------------------------------------------------------------------------
//
//  RAT type is a representation radix  on 2 NUMBER types.
//  pp/pq, where pp and pq are pointers to integral NUMBER types.
//
//-----------------------------------------------------------------------------

typedef struct _rat
{
    PNUMBER pp;
    PNUMBER pq;
} RAT, *PRAT;

static constexpr uint32_t MAX_LONG_SIZE = 33; // Base 2 requires 32 'digits'

//-----------------------------------------------------------------------------
//
// List of useful constants for evaluation, note this list needs to be
// initialized.
//
//-----------------------------------------------------------------------------

extern PNUMBER num_one;
extern PNUMBER num_two;
extern PNUMBER num_five;
extern PNUMBER num_six;
extern PNUMBER num_ten;

extern PRAT ln_ten;
extern PRAT ln_two;
extern PRAT rat_zero;
extern PRAT rat_neg_one;
extern PRAT rat_one;
extern PRAT rat_two;
extern PRAT rat_six;
extern PRAT rat_half;
extern PRAT rat_ten;
extern PRAT pt_eight_five;
extern PRAT pi;
extern PRAT pi_over_two;
extern PRAT two_pi;
extern PRAT one_pt_five_pi;
extern PRAT e_to_one_half;
extern PRAT rat_exp;
extern PRAT rad_to_deg;
extern PRAT rad_to_grad;
extern PRAT rat_qword;
extern PRAT rat_dword;
extern PRAT rat_word;
extern PRAT rat_byte;
extern PRAT rat_360;
extern PRAT rat_400;
extern PRAT rat_180;
extern PRAT rat_200;
extern PRAT rat_nRadix;
extern PRAT rat_smallest;
extern PRAT rat_negsmallest;
extern PRAT rat_max_exp;
extern PRAT rat_min_exp;
extern PRAT rat_max_fact;
extern PRAT rat_min_fact;
extern PRAT rat_max_i32;
extern PRAT rat_min_i32;

// DUPNUM Duplicates a number taking care of allocation and internals
#define DUPNUM(a, b)                                                                                                                                           \
    destroynum(a);                                                                                                                                             \
    createnum(a, (b)->cdigit);                                                                                                                                 \
    _dupnum(a, b);

// DUPRAT Duplicates a rational taking care of allocation and internals
#define DUPRAT(a, b)                                                                                                                                           \
    destroyrat(a);                                                                                                                                             \
    createrat(a);                                                                                                                                              \
    DUPNUM((a)->pp, (b)->pp);                                                                                                                                  \
    DUPNUM((a)->pq, (b)->pq);

// LOG*RADIX calculates the integral portion of the log of a number in
// the base currently being used, only accurate to within g_ratio

#define LOGNUMRADIX(pnum) (((pnum)->cdigit + (pnum)->exp) * g_ratio)
#define LOGRATRADIX(prat) (LOGNUMRADIX((prat)->pp) - LOGNUMRADIX((prat)->pq))

// LOG*2 calculates the integral portion of the log of a number in
// the internal base being used, only accurate to within g_ratio

#define LOGNUM2(pnum) ((pnum)->cdigit + (pnum)->exp)
#define LOGRAT2(prat) (LOGNUM2((prat)->pp) - LOGNUM2((prat)->pq))

// SIGN returns the sign of the rational
#define SIGN(prat) ((prat)->pp->sign * (prat)->pq->sign)

#if defined(DEBUG_RATPAK)
//-----------------------------------------------------------------------------
//
//   Debug versions of rational number creation and destruction routines.
//   used for debugging allocation errors.
//
//-----------------------------------------------------------------------------

#define createrat(y)                                                                                                                                           \
    (y) = _createrat();                                                                                                                                        \
    {                                                                                                                                                          \
        std::wstringstream outputString;                                                                                                                       \
        outputString << "createrat " << y << " " << #y << " file= " << __FILE__ << ", line= " << __LINE__ << "\n";                                             \
        OutputDebugString(outputString.str().c_str());                                                                                                         \
    }
#define destroyrat(x)                                                                                                                                          \
    {                                                                                                                                                          \
        std::wstringstream outputString;                                                                                                                       \
        outputString << "destroyrat " << x << " file= " << __FILE__ << ", line= " << __LINE__ << "\n";                                                         \
        OutputDebugString(outputString.str().c_str());                                                                                                         \
    }                                                                                                                                                          \
    _destroyrat(x), (x) = nullptr
#define createnum(y, x)                                                                                                                                        \
    (y) = _createnum(x);                                                                                                                                       \
    {                                                                                                                                                          \
        std::wstringstream outputString;                                                                                                                       \
        outputString << "createnum " << y << " " << #y << " file= " << __FILE__ << ", line= " << __LINE__ << "\n";                                             \
        OutputDebugString(outputString.str().c_str());                                                                                                         \
    }
#define destroynum(x)                                                                                                                                          \
    {                                                                                                                                                          \
        std::wstringstream outputString;                                                                                                                       \
        outputString << "destroynum " << x << " file= " << __FILE__ << ", line= " << __LINE__ << "\n";                                                         \
        OutputDebugString(outputString.str().c_str());                                                                                                         \
    }                                                                                                                                                          \
    _destroynum(x), (x) = nullptr
#else
#define createrat(y) (y) = _createrat()
#define destroyrat(x) _destroyrat(x), (x) = nullptr
#define createnum(y, x) (y) = _createnum(x)
#define destroynum(x) _destroynum(x), (x) = nullptr
#endif

//-----------------------------------------------------------------------------
//
//   Defines for checking when to stop taylor series expansions due to
//   precision satisfaction.
//
//-----------------------------------------------------------------------------

// RENORMALIZE, gets the exponents non-negative.
#define RENORMALIZE(x)                                                                                                                                         \
    if ((x)->pp->exp < 0)                                                                                                                                      \
    {                                                                                                                                                          \
        (x)->pq->exp -= (x)->pp->exp;                                                                                                                          \
        (x)->pp->exp = 0;                                                                                                                                      \
    }                                                                                                                                                          \
    if ((x)->pq->exp < 0)                                                                                                                                      \
    {                                                                                                                                                          \
        (x)->pp->exp -= (x)->pq->exp;                                                                                                                          \
        (x)->pq->exp = 0;                                                                                                                                      \
    }

// TRIMNUM ASSUMES the number is in radix form NOT INTERNAL BASEX!!!
#define TRIMNUM(x, precision)                                                                                                                                  \
    if (!g_ftrueinfinite)                                                                                                                                      \
    {                                                                                                                                                          \
        int32_t trim = (x)->cdigit - precision - g_ratio;                                                                                                      \
        if (trim > 1)                                                                                                                                          \
        {                                                                                                                                                      \
            memmove((x)->mant, &((x)->mant[trim]), sizeof(MANTTYPE) * ((x)->cdigit - trim));                                                                   \
            (x)->cdigit -= trim;                                                                                                                               \
            (x)->exp += trim;                                                                                                                                  \
        }                                                                                                                                                      \
    }
// TRIMTOP ASSUMES the number is in INTERNAL BASEX!!!
#define TRIMTOP(x, precision)                                                                                                                                  \
    if (!g_ftrueinfinite)                                                                                                                                      \
    {                                                                                                                                                          \
        int32_t trim = (x)->pp->cdigit - (precision / g_ratio) - 2;                                                                                            \
        if (trim > 1)                                                                                                                                          \
        {                                                                                                                                                      \
            memmove((x)->pp->mant, &((x)->pp->mant[trim]), sizeof(MANTTYPE) * ((x)->pp->cdigit - trim));                                                       \
            (x)->pp->cdigit -= trim;                                                                                                                           \
            (x)->pp->exp += trim;                                                                                                                              \
        }                                                                                                                                                      \
        trim = std::min((x)->pp->exp, (x)->pq->exp);                                                                                                           \
        (x)->pp->exp -= trim;                                                                                                                                  \
        (x)->pq->exp -= trim;                                                                                                                                  \
    }

#define SMALL_ENOUGH_RAT(a, precision) (zernum((a)->pp) || ((((a)->pq->cdigit + (a)->pq->exp) - ((a)->pp->cdigit + (a)->pp->exp) - 1) * g_ratio > precision))

//-----------------------------------------------------------------------------
//
//   Defines for setting up taylor series expansions for infinite precision
//   functions.
//
//-----------------------------------------------------------------------------

#define CREATETAYLOR()                                                                                                                                         \
    PRAT xx = nullptr;                                                                                                                                         \
    PNUMBER n2 = nullptr;                                                                                                                                      \
    PRAT pret = nullptr;                                                                                                                                       \
    PRAT thisterm = nullptr;                                                                                                                                   \
    DUPRAT(xx, *px);                                                                                                                                           \
    mulrat(&xx, *px, precision);                                                                                                                               \
    createrat(pret);                                                                                                                                           \
    pret->pp = i32tonum(0L, BASEX);                                                                                                                            \
    pret->pq = i32tonum(0L, BASEX);

#define DESTROYTAYLOR()                                                                                                                                        \
    destroynum(n2);                                                                                                                                            \
    destroyrat(xx);                                                                                                                                            \
    destroyrat(thisterm);                                                                                                                                      \
    destroyrat(*px);                                                                                                                                           \
    trimit(&pret, precision);                                                                                                                                  \
    *px = pret;

// INC(a) is the rational equivalent of a++
// Check to see if we can avoid doing this the hard way.
#define INC(a)                                                                                                                                                 \
    if ((a)->mant[0] < BASEX - 1)                                                                                                                              \
    {                                                                                                                                                          \
        (a)->mant[0]++;                                                                                                                                        \
    }                                                                                                                                                          \
    else                                                                                                                                                       \
    {                                                                                                                                                          \
        addnum(&(a), num_one, BASEX);                                                                                                                          \
    }

#define MSD(x) ((x)->mant[(x)->cdigit - 1])
// MULNUM(b) is the rational equivalent of thisterm *= b where thisterm is
// a rational and b is a number, NOTE this is a mixed type operation for
// efficiency reasons.
#define MULNUM(b) mulnumx(&(thisterm->pp), b);

// DIVNUM(b) is the rational equivalent of thisterm /= b where thisterm is
// a rational and b is a number, NOTE this is a mixed type operation for
// efficiency reasons.
#define DIVNUM(b) mulnumx(&(thisterm->pq), b);

// NEXTTERM(p,d) is the rational equivalent of
// thisterm *= p
// d    <d is usually an expansion of operations to get thisterm updated.>
// pret += thisterm
#define NEXTTERM(p, d, precision)                                                                                                                              \
    mulrat(&thisterm, p, precision);                                                                                                                           \
    d addrat(&pret, thisterm, precision)

//-----------------------------------------------------------------------------
//
//   External variables used in the math package.
//
//-----------------------------------------------------------------------------

extern bool g_ftrueinfinite; // set to true to allow infinite precision
                             // don't use unless you know what you are doing
                             // used to help decide when to stop calculating.

extern int32_t g_ratio; // Internally calculated ratio of internal radix

//-----------------------------------------------------------------------------
//
//   External functions defined in the math package.
//
//-----------------------------------------------------------------------------

// Call whenever decimal separator character changes.
extern void SetDecimalSeparator(wchar_t decimalSeparator);

// Call whenever either radix or precision changes, is smarter about recalculating constants.
extern void ChangeConstants(uint32_t radix, int32_t precision);

extern bool equnum(_In_ PNUMBER a, _In_ PNUMBER b);  // returns true of a == b
extern bool lessnum(_In_ PNUMBER a, _In_ PNUMBER b); // returns true of a < b
extern bool zernum(_In_ PNUMBER a);                  // returns true of a == 0
extern bool zerrat(_In_ PRAT a);                     // returns true if a == 0/q
extern std::wstring NumberToString(_Inout_ PNUMBER& pnum, NumberFormat format, uint32_t radix, int32_t precision);

// returns a text representation of a PRAT
extern std::wstring RatToString(_Inout_ PRAT& prat, NumberFormat format, uint32_t radix, int32_t precision);
// converts a PRAT into a PNUMBER
extern PNUMBER RatToNumber(_In_ PRAT prat, uint32_t radix, int32_t precision);
// flattens a PRAT by converting it to a PNUMBER and back to a PRAT
extern void flatrat(_Inout_ PRAT& prat, uint32_t radix, int32_t precision);

extern int32_t numtoi32(_In_ PNUMBER pnum, uint32_t radix);
extern int32_t rattoi32(_In_ PRAT prat, uint32_t radix, int32_t precision);
uint64_t rattoUi64(_In_ PRAT prat, uint32_t radix, int32_t precision);
extern PNUMBER _createnum(_In_ uint32_t size); // returns an empty number structure with size digits
extern PNUMBER nRadixxtonum(_In_ PNUMBER a, uint32_t radix, int32_t precision);
extern PNUMBER gcd(_In_ PNUMBER a, _In_ PNUMBER b);
extern PNUMBER StringToNumber(
    std::wstring_view numberString,
    uint32_t radix,
    int32_t precision); // takes a text representation of a number and returns a number.

// takes a text representation of a number as a mantissa with sign and an exponent with sign.
extern PRAT
StringToRat(bool mantissaIsNegative, std::wstring_view mantissa, bool exponentIsNegative, std::wstring_view exponent, uint32_t radix, int32_t precision);

extern PNUMBER i32factnum(int32_t ini32, uint32_t radix);
extern PNUMBER i32prodnum(int32_t start, int32_t stop, uint32_t radix);
extern PNUMBER i32tonum(int32_t ini32, uint32_t radix);
extern PNUMBER Ui32tonum(uint32_t ini32, uint32_t radix);
extern PNUMBER numtonRadixx(_In_ PNUMBER a, uint32_t radix);

// creates a empty/undefined rational representation (p/q)
extern PRAT _createrat(void);

// returns a new rat structure with the acos of x->p/x->q taking into account
// angle type
extern void acosanglerat(_Inout_ PRAT* px, AngleType angletype, uint32_t radix, int32_t precision);

// returns a new rat structure with the acosh of x->p/x->q
extern void acoshrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the acos of x->p/x->q
extern void acosrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the asin of x->p/x->q taking into account
// angle type
extern void asinanglerat(_Inout_ PRAT* px, AngleType angletype, uint32_t radix, int32_t precision);

extern void asinhrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);
// returns a new rat structure with the asinh of x->p/x->q

// returns a new rat structure with the asin of x->p/x->q
extern void asinrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the atan of x->p/x->q taking into account
// angle type
extern void atananglerat(_Inout_ PRAT* px, AngleType angletype, uint32_t radix, int32_t precision);

// returns a new rat structure with the atanh of x->p/x->q
extern void atanhrat(_Inout_ PRAT* px, int32_t precision);

// returns a new rat structure with the atan of x->p/x->q
extern void atanrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the cosh of x->p/x->q
extern void coshrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the cos of x->p/x->q
extern void cosrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the cos of x->p/x->q taking into account
// angle type
extern void cosanglerat(_Inout_ PRAT* px, AngleType angletype, uint32_t radix, int32_t precision);

// returns a new rat structure with the exp of x->p/x->q this should not be called explicitly.
extern void _exprat(_Inout_ PRAT* px, int32_t precision);

// returns a new rat structure with the exp of x->p/x->q
extern void exprat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the log base 10 of x->p/x->q
extern void log10rat(_Inout_ PRAT* px, int32_t precision);

// returns a new rat structure with the natural log of x->p/x->q
extern void lograt(_Inout_ PRAT* px, int32_t precision);

extern PRAT i32torat(int32_t ini32);
extern PRAT Ui32torat(uint32_t inui32);
extern PRAT numtorat(_In_ PNUMBER pin, uint32_t radix);

extern void sinhrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);
extern void sinrat(_Inout_ PRAT* px);

// returns a new rat structure with the sin of x->p/x->q taking into account
// angle type
extern void sinanglerat(_Inout_ PRAT* px, AngleType angletype, uint32_t radix, int32_t precision);

extern void tanhrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);
extern void tanrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);

// returns a new rat structure with the tan of x->p/x->q taking into account
// angle type
extern void tananglerat(_Inout_ PRAT* px, AngleType angletype, uint32_t radix, int32_t precision);

extern void _dupnum(_In_ PNUMBER dest, _In_ const NUMBER* const src);

extern void _destroynum(_Frees_ptr_opt_ PNUMBER pnum);
extern void _destroyrat(_Frees_ptr_opt_ PRAT prat);
extern void addnum(_Inout_ PNUMBER* pa, _In_ PNUMBER b, uint32_t radix);
extern void addrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision);
extern void andrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void divnum(_Inout_ PNUMBER* pa, _In_ PNUMBER b, uint32_t radix, int32_t precision);
extern void divnumx(_Inout_ PNUMBER* pa, _In_ PNUMBER b, int32_t precision);
extern void divrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision);
extern void fracrat(_Inout_ PRAT* pa, uint32_t radix, int32_t precision);
extern void factrat(_Inout_ PRAT* pa, uint32_t radix, int32_t precision);
extern void remrat(_Inout_ PRAT* pa, _In_ PRAT b);
extern void modrat(_Inout_ PRAT* pa, _In_ PRAT b);
extern void gcdrat(_Inout_ PRAT* pa, int32_t precision);
extern void intrat(_Inout_ PRAT* px, uint32_t radix, int32_t precision);
extern void mulnum(_Inout_ PNUMBER* pa, _In_ PNUMBER b, uint32_t radix);
extern void mulnumx(_Inout_ PNUMBER* pa, _In_ PNUMBER b);
extern void mulrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision);
extern void numpowi32(_Inout_ PNUMBER* proot, int32_t power, uint32_t radix, int32_t precision);
extern void numpowi32x(_Inout_ PNUMBER* proot, int32_t power);
extern void orrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void powrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void powratNumeratorDenominator(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void powratcomp(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void ratpowi32(_Inout_ PRAT* proot, int32_t power, int32_t precision);
extern void remnum(_Inout_ PNUMBER* pa, _In_ PNUMBER b, uint32_t radix);
extern void rootrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void scale2pi(_Inout_ PRAT* px, uint32_t radix, int32_t precision);
extern void scale(_Inout_ PRAT* px, _In_ PRAT scalefact, uint32_t radix, int32_t precision);
extern void subrat(_Inout_ PRAT* pa, _In_ PRAT b, int32_t precision);
extern void xorrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void lshrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern void rshrat(_Inout_ PRAT* pa, _In_ PRAT b, uint32_t radix, int32_t precision);
extern bool rat_equ(_In_ PRAT a, _In_ PRAT b, int32_t precision);
extern bool rat_neq(_In_ PRAT a, _In_ PRAT b, int32_t precision);
extern bool rat_gt(_In_ PRAT a, _In_ PRAT b, int32_t precision);
extern bool rat_ge(_In_ PRAT a, _In_ PRAT b, int32_t precision);
extern bool rat_lt(_In_ PRAT a, _In_ PRAT b, int32_t precision);
extern bool rat_le(_In_ PRAT a, _In_ PRAT b, int32_t precision);
extern void inbetween(_In_ PRAT* px, _In_ PRAT range, int32_t precision);
extern void trimit(_Inout_ PRAT* px, int32_t precision);
extern void _dumprawrat(_In_ const wchar_t* varname, _In_ PRAT rat, std::wostream& out);
extern void _dumprawnum(_In_ const wchar_t* varname, _In_ PNUMBER num, std::wostream& out);
