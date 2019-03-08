// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//---------------------------------------------------------------------------
//  Package Title  ratpak
//  File           conv.c
//  Copyright      (C) 1995-97 Microsoft
//  Date           01-16-95
//
//
//  Description
//
//     Contains conversion, input and output routines for numbers rationals
//  and longs.
//
//
//
//---------------------------------------------------------------------------

#include "pch.h"
#include "ratpak.h"

using namespace std;

static constexpr int MAX_ZEROS_AFTER_DECIMAL = 2;

// digits 0..64 used by bases 2 .. 64
static constexpr wstring_view DIGITS = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_@";

// ratio of internal 'digits' to output 'digits'
// Calculated elsewhere as part of initialization and when base is changed
long g_ratio;    // int(log(2L^BASEXPWR)/log(radix))
// Default decimal separator
wchar_t g_decimalSeparator = L'.';

// Used to strip trailing zeros, and prevent combinatorial explosions
bool stripzeroesnum(_Inout_ PNUMBER pnum, long starting);

void SetDecimalSeparator(wchar_t decimalSeparator)
{
    g_decimalSeparator = decimalSeparator;
}

//
// Windows heap allocation
//
void* zmalloc(size_t a)
{
    return calloc(a, sizeof(unsigned char));
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: _dupnum
//
//    ARGUMENTS: pointer to a number, pointer to a number
//
//    RETURN: None
//
//    DESCRIPTION: Copies the source to the destination
//
//-----------------------------------------------------------------------------

void _dupnum(_In_ PNUMBER dest, _In_ PNUMBER src)
{
    memcpy(dest, src, (int)(sizeof(NUMBER) + ((src)->cdigit)*(sizeof(MANTTYPE))));
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: _destroynum
//
//    ARGUMENTS: pointer to a number
//
//    RETURN: None
//
//    DESCRIPTION: Deletes the number and associated allocation
//
//-----------------------------------------------------------------------------

void _destroynum( _In_ PNUMBER pnum )

{
    if ( pnum != nullptr)
        {
        free( pnum );
        }
}


//-----------------------------------------------------------------------------
//
//    FUNCTION: _destroyrat
//
//    ARGUMENTS: pointer to a rational
//
//    RETURN: None
//
//    DESCRIPTION: Deletes the rational and associated
//    allocations.
//
//-----------------------------------------------------------------------------

void _destroyrat( _In_ PRAT prat )

{
    if ( prat != nullptr)
        {
        destroynum( prat->pp );
        destroynum( prat->pq );
        free( prat );
        }
}


//-----------------------------------------------------------------------------
//
//    FUNCTION: _createnum
//
//    ARGUMENTS: size of number in 'digits'
//
//    RETURN: pointer to a number
//
//    DESCRIPTION: allocates and zeros out number type.
//
//-----------------------------------------------------------------------------

PNUMBER _createnum( _In_ ULONG size )

{
    PNUMBER pnumret= nullptr;
    ULONG cbAlloc;

    // sizeof( MANTTYPE ) is the size of a 'digit'
    if (SUCCEEDED(ULongAdd(size, 1, &cbAlloc)) &&
        SUCCEEDED(ULongMult(cbAlloc, sizeof(MANTTYPE), &cbAlloc)) &&
        SUCCEEDED(ULongAdd(cbAlloc, sizeof(NUMBER), &cbAlloc)))
    {
        pnumret = (PNUMBER)zmalloc( cbAlloc );
        if ( pnumret == nullptr)
        {
            throw( CALC_E_OUTOFMEMORY );
        }
    }
    else
    {
        throw( CALC_E_INVALIDRANGE );
    }
    return( pnumret );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: _createrat
//
//    ARGUMENTS: none
//
//    RETURN: pointer to a rational
//
//    DESCRIPTION: allocates a rational structure but does not
//    allocate the numbers that make up the rational p over q
//    form.  These number pointers are left pointing to null.
//
//-----------------------------------------------------------------------------


PRAT _createrat( void )

{
    PRAT prat= nullptr;

    prat = (PRAT)zmalloc( sizeof( RAT ) );

    if ( prat == nullptr)
    {
        throw( CALC_E_OUTOFMEMORY );
    }
    prat->pp = nullptr;
    prat->pq = nullptr;
    return( prat );
}



//-----------------------------------------------------------------------------
//
//    FUNCTION: numtorat
//
//    ARGUMENTS: pointer to a number, radix number is in.
//
//    RETURN: Rational representation of number.
//
//    DESCRIPTION: The rational representation of the number
//    is guaranteed to be in the form p (number with internal
//    base   representation) over q (number with internal base
//    representation)  Where p and q are integers.
//
//-----------------------------------------------------------------------------

PRAT numtorat( _In_ PNUMBER pin, uint32_t radix)

{
    PNUMBER pnRadixn= nullptr;
    DUPNUM( pnRadixn, pin );

    PNUMBER qnRadixn=longtonum( 1, radix);

    // Ensure p and q start out as integers.
    if ( pnRadixn->exp < 0 )
        {
        qnRadixn->exp -= pnRadixn->exp;
        pnRadixn->exp = 0;
        }

    PRAT pout= nullptr;
    createrat(pout);

    // There is probably a better way to do this.
    pout->pp = numtonRadixx(pnRadixn, radix);
    pout->pq = numtonRadixx(qnRadixn, radix);


    destroynum( pnRadixn );
    destroynum( qnRadixn );

    return( pout );
}



//----------------------------------------------------------------------------
//
//    FUNCTION: nRadixxtonum
//
//    ARGUMENTS: pointer to a number, base requested.
//
//    RETURN: number representation in radix requested.
//
//    DESCRIPTION: Does a base conversion on a number from
//    internal to requested base. Assumes number being passed
//    in is really in internal base form.
//
//----------------------------------------------------------------------------

PNUMBER nRadixxtonum( _In_ PNUMBER a, uint32_t radix, int32_t precision)

{
    unsigned long bitmask;
    unsigned long cdigits;
    MANTTYPE *ptr;

    PNUMBER sum = longtonum( 0, radix );
    PNUMBER powofnRadix = longtonum( BASEX, radix );

    // A large penalty is paid for conversion of digits no one will see anyway.
    // limit the digits to the minimum of the existing precision or the
    // requested precision.
    cdigits = precision + 1;
    if ( cdigits > (unsigned long)a->cdigit )
        {
        cdigits = (unsigned long)a->cdigit;
        }

    // scale by the internal base to the internal exponent offset of the LSD
    numpowlong( &powofnRadix, a->exp + (a->cdigit - cdigits), radix, precision);

    // Loop over all the relative digits from MSD to LSD
    for ( ptr = &(a->mant[a->cdigit-1]); cdigits > 0;
        ptr--, cdigits-- )
        {
        // Loop over all the bits from MSB to LSB
        for ( bitmask = BASEX/2; bitmask > 0; bitmask /= 2 )
            {
            addnum( &sum, sum, radix );
            if ( *ptr & bitmask )
                {
                sum->mant[0] |= 1;
                }
            }
        }

    // Scale answer by power of internal exponent.
    mulnum( &sum, powofnRadix, radix );

    destroynum( powofnRadix );
    sum->sign = a->sign;
    return( sum );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: numtonRadixx
//
//    ARGUMENTS: pointer to a number, radix of that number.
//
//    RETURN: number representation in internal radix.
//
//    DESCRIPTION: Does a radix conversion on a number from
//    specified radix to requested radix.  Assumes the radix
//    specified is the radix of the number passed in.
//
//-----------------------------------------------------------------------------

PNUMBER numtonRadixx(_In_ PNUMBER a, uint32_t radix)
{
    PNUMBER pnumret = longtonum(0, BASEX); // pnumret is the number in internal form.
    PNUMBER num_radix = longtonum(radix, BASEX);
    MANTTYPE *ptrdigit = a->mant; // pointer to digit being worked on.

    // Digits are in reverse order, back over them LSD first.
    ptrdigit += a->cdigit-1;

    PNUMBER thisdigit = nullptr;      // thisdigit holds the current digit of a
                                   // being summed into result.
    long idigit;                   // idigit is the iterate of digits in a.
    for ( idigit = 0; idigit < a->cdigit; idigit++ )
        {
        mulnumx( &pnumret, num_radix);
        // WARNING:
        // This should just smack in each digit into a 'special' thisdigit.
        // and not do the overhead of recreating the number type each time.
        thisdigit = longtonum( *ptrdigit--, BASEX );
        addnum( &pnumret, thisdigit, BASEX );
        destroynum( thisdigit );
        }

    // Calculate the exponent of the external base for scaling.
    numpowlongx( &num_radix, a->exp );

    // ... and scale the result.
    mulnumx( &pnumret, num_radix);

    destroynum(num_radix);

    // And propagate the sign.
    pnumret->sign = a->sign;

    return( pnumret );
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: StringToRat
//
//  ARGUMENTS:
//              mantissaIsNegative true if mantissa is less than zero
//              mantissa a string representation of a number
//              exponentIsNegative  true if exponent is less than zero
//              exponent a string representation of a number
//              radix is the number base used in the source string
//
//  RETURN: PRAT representation of string input.
//          Or nullptr if no number scanned.
//
//  EXPLANATION: This is for calc.
//
//
//-----------------------------------------------------------------------------

PRAT StringToRat(bool mantissaIsNegative, wstring_view mantissa, bool exponentIsNegative, wstring_view exponent, uint32_t radix, int32_t precision)
{
    PRAT resultRat = nullptr; // holds exponent in rational form.

    // Deal with mantissa
    if (mantissa.empty())
    {
        // Preset value if no mantissa
        if (exponent.empty())
        {
            // Exponent not specified, preset value to zero
            DUPRAT(resultRat, rat_zero);
        }
        else
        {
            // Exponent specified, preset value to one
            DUPRAT(resultRat, rat_one);
        }
    }
    else
    {
        // Mantissa specified, convert to number form.
        PNUMBER pnummant = StringToNumber(mantissa, radix, precision);
        if (pnummant == nullptr)
        {
            return nullptr;
        }

        resultRat = numtorat(pnummant, radix);
        // convert to rational form, and cleanup.
        destroynum(pnummant);
    }

    // Deal with exponent
    long expt = 0;
    if (!exponent.empty())
    {
        // Exponent specified, convert to number form.
        // Don't use native stuff, as it is restricted in the bases it can
        // handle.
        PNUMBER numExp = StringToNumber(exponent, radix, precision);
        if (numExp == nullptr)
        {
            return nullptr;
        }

        // Convert exponent number form to native integral form,  and cleanup.
        expt = numtolong(numExp, radix);
        destroynum(numExp);
    }

    // Convert native integral exponent form to rational multiplier form.
    PNUMBER pnumexp = longtonum(radix, BASEX);
    numpowlongx(&pnumexp, abs(expt));

    PRAT pratexp = nullptr;
    createrat(pratexp);
    DUPNUM(pratexp->pp, pnumexp);
    pratexp->pq = longtonum(1, BASEX);
    destroynum(pnumexp);

    if (exponentIsNegative)
    {
        // multiplier is less than 1, this means divide.
        divrat(&resultRat, pratexp, precision);
    }
    else if (expt > 0)
    {
        // multiplier is greater than 1, this means multiply.
        mulrat(&resultRat, pratexp, precision);
    }
    // multiplier can be 1, in which case it'd be a waste of time to multiply.

    destroyrat(pratexp);

    if (mantissaIsNegative)
    {
        // A negative number was used, adjust the sign.
        resultRat->pp->sign *= -1;
    }

    return resultRat;
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: StringToNumber
//
//  ARGUMENTS:
//              wstring_view numberString
//              int radix
//              int32_t precision
//
//  RETURN: pnumber representation of string input.
//          Or nullptr if no number scanned.
//
//  EXPLANATION: This is a state machine,
//
//    State      Description            Example, ^shows just read position.
//                                                which caused the transition
//
//    START      Start state            ^1.0
//    MANTS      Mantissa sign          -^1.0
//    LZ         Leading Zero           0^1.0
//    LZDP       Post LZ dec. pt.       000.^1
//    LD         Leading digit          1^.0
//    DZ         Post LZDP Zero         000.0^1
//    DD         Post Decimal digit     .01^2
//    DDP        Leading Digit dec. pt. 1.^2
//    EXPB       Exponent Begins        1.0e^2
//    EXPS       Exponent sign          1.0e+^5
//    EXPD       Exponent digit         1.0e1^2 or  even 1.0e0^1
//    EXPBZ      Exponent begin post 0  0.000e^+1
//    EXPSZ      Exponent sign post 0   0.000e+^1
//    EXPDZ      Exponent digit post 0  0.000e+1^2
//    ERR        Error case             0.0.^
//
//    Terminal   Description
//
//    DP         '.'
//    ZR         '0'
//    NZ         '1'..'9' 'A'..'Z' 'a'..'z' '@' '_'
//    SG         '+' '-'
//    EX         'e' '^' e is used for radix 10, ^ for all other radixes.
//
//-----------------------------------------------------------------------------
static constexpr uint8_t DP = 0;
static constexpr uint8_t ZR = 1;
static constexpr uint8_t NZ = 2;
static constexpr uint8_t SG = 3;
static constexpr uint8_t EX = 4;

static constexpr uint8_t START = 0;
static constexpr uint8_t MANTS = 1;
static constexpr uint8_t LZ    = 2;
static constexpr uint8_t LZDP  = 3;
static constexpr uint8_t LD    = 4;
static constexpr uint8_t DZ    = 5;
static constexpr uint8_t DD    = 6;
static constexpr uint8_t DDP   = 7;
static constexpr uint8_t EXPB  = 8;
static constexpr uint8_t EXPS  = 9;
static constexpr uint8_t EXPD  = 10;
static constexpr uint8_t EXPBZ = 11;
static constexpr uint8_t EXPSZ = 12;
static constexpr uint8_t EXPDZ = 13;
static constexpr uint8_t ERR   = 14;

#if defined( DEBUG )
char *statestr[] = {
    "START",
    "MANTS",
    "LZ",
    "LZDP",
    "LD",
    "DZ",
    "DD",
    "DDP",
    "EXPB",
    "EXPS",
    "EXPD",
    "EXPBZ",
    "EXPSZ",
    "EXPDZ",
    "ERR",
};
#endif

// New state is machine[state][terminal]
static constexpr uint8_t machine[ERR+1][EX+1]= {
    //        DP,     ZR,      NZ,      SG,     EX
    // START
        {     LZDP,   LZ,      LD,      MANTS,  ERR },
    // MANTS
        {     LZDP,   LZ,      LD,      ERR,    ERR },
    // LZ
        {     LZDP,   LZ,      LD,      ERR,    EXPBZ },
    // LZDP
        {     ERR,    DZ,      DD,      ERR,    EXPB },
    // LD
        {     DDP,    LD,      LD,      ERR,    EXPB },
    // DZ
        {     ERR,    DZ,      DD,      ERR,    EXPBZ },
    // DD
        {     ERR,    DD,      DD,      ERR,    EXPB },
    // DDP
        {     ERR,    DD,      DD,      ERR,    EXPB },
    // EXPB
        {     ERR,    EXPD,    EXPD,    EXPS,   ERR },
    // EXPS
        {     ERR,    EXPD,    EXPD,    ERR,    ERR },
    // EXPD
        {     ERR,    EXPD,    EXPD,    ERR,    ERR },
    // EXPBZ
        {     ERR,    EXPDZ,   EXPDZ,   EXPSZ,  ERR },
    // EXPSZ
        {     ERR,    EXPDZ,   EXPDZ,   ERR,    ERR },
    // EXPDZ
        {     ERR,    EXPDZ,   EXPDZ,   ERR,    ERR },
    // ERR
        {     ERR,    ERR,     ERR,     ERR,    ERR }
};

wchar_t NormalizeCharDigit(wchar_t c, uint32_t radix)
{
    // Allow upper and lower case letters as equivalent, base
    // is in the range where this is not ambiguous.
    if (size_t{ radix } >= DIGITS.find(L'A') && size_t { radix } <= DIGITS.find(L'Z'))
    {
        return towupper(c);
    }

    return c;
}

PNUMBER StringToNumber(wstring_view numberString, uint32_t radix, int32_t precision)
{
    long expSign = 1L;           // expSign is exponent sign ( +/- 1 )
    long expValue = 0L;          // expValue is exponent mantissa, should be unsigned

    PNUMBER pnumret = nullptr;
    createnum(pnumret, static_cast<ULONG>(numberString.length()));
    pnumret->sign = 1L;
    pnumret->cdigit = 0;
    pnumret->exp = 0;
    MANTTYPE *pmant = pnumret->mant + numberString.length() - 1;

    uint8_t state = START; // state is the state of the input state machine.
    wchar_t curChar;
    for (const auto& c : numberString)
    {
        // If the character is the decimal separator, use L'.' for the purposes of the state machine.
        curChar = (c == g_decimalSeparator ? L'.' : c);

        // Switch states based on the character we encountered
        switch (curChar)
        {
        case L'-':
        case L'+':
            state = machine[state][SG];
            break;
        case L'.':
            state = machine[state][DP];
            break;
        case L'0':
            state = machine[state][ZR];
            break;
        case L'^':
        case L'e':
            if (curChar == L'^' || radix == 10)
            {
                state = machine[state][EX];
                break;
            }
            // Drop through in the 'e'-as-a-digit case
        default:
            state = machine[state][NZ];
            break;
        }

        // Now update our result value based on the state we are in
        switch (state)
        {
        case MANTS:
            pnumret->sign = (curChar == L'-') ? -1 : 1;
            break;
        case EXPSZ:
        case EXPS:
            expSign = (curChar == L'-') ? -1 : 1;
            break;
        case EXPDZ:
        case EXPD:
            {
                curChar = NormalizeCharDigit(curChar, radix);

                size_t pos = DIGITS.find(curChar);
                if (pos != wstring_view::npos)
                {
                    expValue *= radix;
                    expValue += static_cast<long>(pos);
                }
                else
                {
                    state = ERR;
                }
            }
            break;
        case LD:
            pnumret->exp++;
            // Fall through
        case DD:
            {
                curChar = NormalizeCharDigit(curChar, radix);

                size_t pos = DIGITS.find(curChar);
                if (pos != wstring_view::npos && pos < static_cast<size_t>(radix))
                {
                    *pmant-- = static_cast<MANTTYPE>(pos);
                    pnumret->exp--;
                    pnumret->cdigit++;
                }
                else
                {
                    state = ERR;
                }
            }
            break;
        case DZ:
            pnumret->exp--;
            break;
        case LZ:
        case LZDP:
        case DDP:
            break;
        }
    }

    if (state == DZ || state == EXPDZ)
    {
        pnumret->cdigit = 1;
        pnumret->exp = 0;
        pnumret->sign = 1;
    }
    else
    {
        while (pnumret->cdigit < static_cast<long>(numberString.length()))
        {
            pnumret->cdigit++;
            pnumret->exp--;
        }

        pnumret->exp += expSign * expValue;
    }

    // If we don't have a number, clear our result.
    if (pnumret->cdigit == 0)
    {
        destroynum(pnumret);
        pnumret = nullptr;
    }

    stripzeroesnum(pnumret, precision);

    return pnumret;
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: longtorat
//
//    ARGUMENTS: long
//
//    RETURN: Rational representation of long input.
//
//    DESCRIPTION: Converts long input to rational (p over q)
//    form, where q is 1 and p is the long.
//
//-----------------------------------------------------------------------------

PRAT longtorat( _In_ long inlong )

{
    PRAT pratret= nullptr;
    createrat( pratret );
    pratret->pp = longtonum(inlong, BASEX );
    pratret->pq = longtonum(1L, BASEX );
    return( pratret );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: Ulongtorat
//
//    ARGUMENTS: ulong
//
//    RETURN: Rational representation of unsigned long input.
//
//    DESCRIPTION: Converts unsigned long input to rational (p over q)
//    form, where q is 1 and p is the unsigned long. Being unsigned cant take negative
//    numbers, but the full range of unsigned numbers
//
//-----------------------------------------------------------------------------

PRAT Ulongtorat( _In_ unsigned long inulong )

{
    PRAT pratret= nullptr;
    createrat( pratret );
    pratret->pp = Ulongtonum(inulong, BASEX );
    pratret->pq = longtonum(1L, BASEX );
    return( pratret );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: longtonum
//
//    ARGUMENTS: long input and radix requested.
//
//    RETURN: number
//
//    DESCRIPTION: Returns a number representation in the
//    base   requested of the long value passed in.
//
//-----------------------------------------------------------------------------

PNUMBER longtonum( long inlong, uint32_t radix)

{
    MANTTYPE *pmant;
    PNUMBER pnumret= nullptr;

    createnum( pnumret, MAX_LONG_SIZE );
    pmant = pnumret->mant;
    pnumret->cdigit = 0;
    pnumret->exp = 0;
    if ( inlong < 0 )
        {
        pnumret->sign = -1;
        inlong *= -1;
        }
    else
        {
        pnumret->sign = 1;
        }

    do    {
        *pmant++ = (MANTTYPE)(inlong % radix);
        inlong /= radix;
        pnumret->cdigit++;
        } while ( inlong );

    return( pnumret );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: Ulongtonum
//
//    ARGUMENTS: ULONG input and radix requested.
//
//    RETURN: number
//
//    DESCRIPTION: Returns a number representation in the
//    base   requested of the unsigned long value passed in. Being unsigned number it has no 
//    negative number and takes the full range of unsigned number
//
//-----------------------------------------------------------------------------


PNUMBER Ulongtonum(unsigned long inlong, uint32_t radix)
{
    MANTTYPE *pmant;
    PNUMBER pnumret= nullptr;

    createnum( pnumret, MAX_LONG_SIZE );
    pmant = pnumret->mant;
    pnumret->cdigit = 0;
    pnumret->exp = 0;
    pnumret->sign = 1;
    
    do    {
        *pmant++ = (MANTTYPE)(inlong % radix);
        inlong /= radix;
        pnumret->cdigit++;
        } while ( inlong );

    return( pnumret );
}


//-----------------------------------------------------------------------------
//
//    FUNCTION: rattolong
//
//    ARGUMENTS: rational number in internal base, integer radix and int32_t precision.
//
//    RETURN: long
//
//    DESCRIPTION: returns the long representation of the
//    number input.  Assumes that the number is in the internal
//    base.
//
//-----------------------------------------------------------------------------

long rattolong( _In_ PRAT prat , uint32_t radix, int32_t precision)
{
    if ( rat_gt( prat, rat_max_long, precision) || rat_lt( prat, rat_min_long, precision) )
    {
        // Don't attempt rattolong of anything too big or small
        throw( CALC_E_DOMAIN );
    }

    PRAT pint = nullptr;
    DUPRAT(pint,prat);

    intrat( &pint, radix, precision);
    divnumx( &(pint->pp), pint->pq, precision);
    DUPNUM( pint->pq, num_one );

    long lret = numtolong( pint->pp, BASEX );

    destroyrat(pint);

    return( lret );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: rattoUlong
//
//    ARGUMENTS: rational number in internal base, integer radix and int32_t precision.
//
//    RETURN: Ulong
//
//    DESCRIPTION: returns the Ulong representation of the
//    number input.  Assumes that the number is in the internal
//    base.
//
//-----------------------------------------------------------------------------
unsigned long rattoUlong( _In_ PRAT prat, uint32_t radix, int32_t precision)
{
    if ( rat_gt( prat, rat_dword, precision) || rat_lt( prat, rat_zero, precision) )
    {
        // Don't attempt rattoulong of anything too big or small
        throw( CALC_E_DOMAIN );
    }

    PRAT pint = nullptr;
    DUPRAT(pint,prat);

    intrat( &pint, radix, precision);
    divnumx( &(pint->pp), pint->pq, precision);
    DUPNUM( pint->pq, num_one );

    unsigned long lret = numtolong( pint->pp, BASEX ); // This happens to work even if it is only signed

    destroyrat(pint);

    return( lret );
}


//-----------------------------------------------------------------------------
//
//    FUNCTION: rattoUlonglong
//
//    ARGUMENTS: rational number in internal base, integer radix and int32_t precision
//
//    RETURN: Ulonglong
//
//    DESCRIPTION: returns the 64 bit (irrespective of which processor this is running in) representation of the
//    number input.  Assumes that the number is in the internal
//    base. Can throw exception if the number exceeds 2^64
//    Implementation by getting the HI & LO 32 bit words and concatenating them, as the
//    internal base chosen happens to be 2^32, this is easier.
//-----------------------------------------------------------------------------

ULONGLONG rattoUlonglong( _In_ PRAT prat, uint32_t radix, int32_t precision)
{
    PRAT pint = nullptr;

    // first get the LO 32 bit word
    DUPRAT(pint, prat);
    andrat(&pint, rat_dword, radix, precision);  // & 0xFFFFFFFF   (2 ^ 32 -1)
    unsigned long lo = rattoUlong(pint, radix, precision);  // wont throw exception because already hi-dword chopped off

    DUPRAT(pint, prat); // previous pint will get freed by this as well
    PRAT prat32 = longtorat(32);
    rshrat(&pint, prat32, radix, precision);
    intrat( &pint, radix, precision);
    andrat(&pint, rat_dword, radix, precision);  // & 0xFFFFFFFF   (2 ^ 32 -1)
    unsigned long hi = rattoUlong(pint, radix, precision);

    destroyrat(prat32);
    destroyrat(pint);

    return (((ULONGLONG)hi << 32) | lo);
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: numtolong
//
//    ARGUMENTS: number input and base of that number.
//
//    RETURN: long
//
//    DESCRIPTION: returns the long representation of the
//    number input.  Assumes that the number is really in the
//    base   claimed.
//
//-----------------------------------------------------------------------------
long numtolong( _In_ PNUMBER pnum, uint32_t radix )
{
    long lret = 0;

    MANTTYPE *pmant = pnum->mant;
    pmant += pnum->cdigit - 1;

    long expt = pnum->exp;
    for (long length = pnum->cdigit; length > 0 && length + expt > 0; length--)
    {
        lret *= radix;
        lret += *(pmant--);
    }

    while (expt-- > 0)
    {
        lret *= radix;
    }
    lret *= pnum->sign;

    return lret;
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: bool stripzeroesnum
//
//    ARGUMENTS:            a number representation
//
//    RETURN: true if stripping done, modifies number in place.
//
//    DESCRIPTION: Strips off trailing zeros.
//
//-----------------------------------------------------------------------------

bool stripzeroesnum(_Inout_ PNUMBER pnum, long starting)
{
    MANTTYPE *pmant;
    long cdigits;
    bool fstrip = false;

    // point pmant to the LeastCalculatedDigit
    pmant=pnum->mant;
    cdigits=pnum->cdigit;
    // point pmant to the LSD
    if ( cdigits > starting )
        {
        pmant += cdigits - starting;
        cdigits = starting;
        }

    // Check we haven't gone too far, and we are still looking at zeros.
    while ( ( cdigits > 0 ) && !(*pmant) )
        {
        // move to next significant digit and keep track of digits we can
    // ignore later.
        pmant++;
        cdigits--;
        fstrip = true;
    }

    // If there are zeros to remove.
    if ( fstrip )
        {
        // Remove them.
        memmove( pnum->mant, pmant, (int)(cdigits*sizeof(MANTTYPE)) );
        // And adjust exponent and digit count accordingly.
        pnum->exp += ( pnum->cdigit - cdigits );
        pnum->cdigit = cdigits;
        }
    return( fstrip );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: NumberToString
//
//    ARGUMENTS: number representation
//          fmt, one of FMT_FLOAT FMT_SCIENTIFIC or
//          FMT_ENGINEERING
//          integer radix and int32_t precision value
//
//    RETURN: String representation of number.
//
//    DESCRIPTION: Converts a number to it's string
//    representation.
//
//-----------------------------------------------------------------------------
wstring NumberToString(_Inout_ PNUMBER& pnum, int format, uint32_t radix, int32_t precision)
{
    stripzeroesnum(pnum, precision + 2);
    long length = pnum->cdigit;
    long exponent = pnum->exp + length; // Actual number of digits to the left of decimal

    long oldFormat = format;
    if (exponent > precision && format == FMT_FLOAT)
    {
        // Force scientific mode to prevent user from assuming 33rd digit is exact.
        format = FMT_SCIENTIFIC;
    }

    // Make length small enough to fit in pret.
    if (length > precision)
    {
        length = precision;
    }

    // If there is a chance a round has to occur, round.
    // - if number is zero no rounding
    // - if number of digits is less than the maximum output no rounding
    PNUMBER round = nullptr;
    if (!zernum(pnum) && (pnum->cdigit >= precision || (length - exponent > precision && exponent >= -MAX_ZEROS_AFTER_DECIMAL)))
    {
        // Otherwise round.
        round = longtonum(radix, radix);
        divnum(&round, num_two, radix, precision);

        // Make round number exponent one below the LSD for the number.
        if (exponent > 0 || format == FMT_FLOAT)
        {
            round->exp = pnum->exp + pnum->cdigit - round->cdigit - precision;
        }
        else
        {
            round->exp = pnum->exp + pnum->cdigit - round->cdigit - precision - exponent;
            length = precision + exponent;
        }

        round->sign = pnum->sign;
    }

    if (format == FMT_FLOAT)
    {
        // Figure out if the exponent will fill more space than the non-exponent field.
        if ((length - exponent > precision) || (exponent > precision + 3))
        {
            if (exponent >= -MAX_ZEROS_AFTER_DECIMAL)
            {
                round->exp -= exponent;
                length = precision + exponent;
            }
            else
            {
                // Case where too many zeros are to the right or left of the
                // decimal pt. And we are forced to switch to scientific form.
                format = FMT_SCIENTIFIC;
            }
        }
        else if (length + abs(exponent) < precision && round)
        {
            // Minimum loss of precision occurs with listing leading zeros
            // if we need to make room for zeros sacrifice some digits.
            round->exp -= exponent;
        }
    }

    if (round != nullptr)
    {
        addnum(&pnum, round, radix);
        long offset = (pnum->cdigit + pnum->exp) - (round->cdigit + round->exp);
        destroynum(round);
        if (stripzeroesnum(pnum, offset))
        {
            // WARNING: nesting/recursion, too much has been changed, need to
            // re-figure format.
            return NumberToString(pnum, oldFormat, radix, precision);
        }
    }
    else
    {
        stripzeroesnum(pnum, precision);
    }

    // Set up all the post rounding stuff.
    bool useSciForm = false;
    long eout = exponent - 1; // Displayed exponent.
    MANTTYPE *pmant = pnum->mant + pnum->cdigit - 1;
    // Case where too many digits are to the left of the decimal or
    // FMT_SCIENTIFIC or FMT_ENGINEERING was specified.
    if ((format == FMT_SCIENTIFIC) || (format == FMT_ENGINEERING))
    {
        useSciForm = true;
        if (eout != 0)
        {
            if (format == FMT_ENGINEERING)
            {
                exponent = (eout % 3);
                eout -= exponent;
                exponent++;

                // Fix the case where 0.02e-3 should really be 2.e-6 etc.
                if (exponent < 0)
                {
                    exponent += 3;
                    eout -= 3;
                }
            }
            else
            {
                exponent = 1;
            }
        }
    }
    else
    {
        eout = 0;
    }

    // Begin building the result string
    wstringstream resultStream{};

    // Make sure negative zeros aren't allowed.
    if ((pnum->sign == -1) && (length > 0))
    {
        resultStream << L'-';
    }

    if (exponent <= 0 && !useSciForm)
    {
        resultStream << L'0';
        resultStream << g_decimalSeparator;
        // Used up a digit unaccounted for.
    }

    while (exponent < 0)
    {
        resultStream << L'0';
        exponent++;
    }

    while (length > 0)
    {
        exponent--;
        resultStream << DIGITS[*pmant--];
        length--;

        // Be more regular in using a decimal point.
        if (exponent == 0)
        {
            resultStream << g_decimalSeparator;
        }
    }

    while (exponent > 0)
    {
        resultStream << L'0';
        exponent--;
        // Be more regular in using a decimal point.
        if (exponent == 0)
        {
            resultStream << g_decimalSeparator;
        }
    }

    if (useSciForm)
    {
        resultStream << (radix == 10 ? L'e' : L'^');
        resultStream << (eout < 0 ? L'-' : L'+');
        eout = abs(eout);
        wstringstream exponentStream{};
        do
        {
            exponentStream << DIGITS[eout % radix];
            eout /= radix;
        } while (eout > 0);

        auto expString = exponentStream.str();
        for (auto ritr = expString.rbegin(); ritr != expString.rend(); ritr++)
        {
            resultStream << *ritr;
        }
    }

    // Remove trailing decimal
    auto resultString = resultStream.str();
    if (!resultString.empty() && resultString.back() == g_decimalSeparator)
    {
        resultString.pop_back();
    }

    return resultString;
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: RatToString
//
//  ARGUMENTS:
//              PRAT *representation of a number.
//              long representation of base  to  dump to screen.
//              fmt, one of FMT_FLOAT FMT_SCIENTIFIC or FMT_ENGINEERING
//              precision uint32_t
//
//  RETURN: string
//
//  DESCRIPTION: returns a string representation of rational number passed
//  in, at least to the precision digits.
//
//  NOTE: It may be that doing a GCD() could shorten the rational form
//       And it may eventually be worthwhile to keep the result.  That is
//       why a pointer to the rational is passed in.
//
//-----------------------------------------------------------------------------
wstring RatToString(_Inout_ PRAT& prat, int format, uint32_t radix, int32_t precision)
{
    PNUMBER p = RatToNumber(prat, radix, precision);

    wstring result = NumberToString(p, format, radix, precision);
    destroynum(p);

    return result;
}

PNUMBER RatToNumber(_In_ PRAT prat, uint32_t radix, int32_t precision)
{
    PRAT temprat = nullptr;
    DUPRAT(temprat, prat);
    // Convert p and q of rational form from internal base to requested base.
    // Scale by largest power of BASEX possible.
    long scaleby = min(temprat->pp->exp, temprat->pq->exp);
    scaleby = max(scaleby, 0);

    temprat->pp->exp -= scaleby;
    temprat->pq->exp -= scaleby;

    PNUMBER p = nRadixxtonum(temprat->pp, radix, precision);
    PNUMBER q = nRadixxtonum(temprat->pq, radix, precision);

    destroyrat(temprat);

    // finally take the time hit to actually divide.
    divnum(&p, q, radix, precision);
    destroynum(q);

    return p;
}

// Converts a PRAT to a PNUMBER and back to a PRAT, flattening/simplifying the rational in the process
void flatrat(_Inout_ PRAT& prat, uint32_t radix, int32_t precision)
{
    PNUMBER pnum = RatToNumber(prat, radix, precision);

    destroyrat(prat);
    prat = numtorat(pnum, radix);
    destroynum(pnum);
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: gcd
//
//  ARGUMENTS:
//              PNUMBER representation of a number.
//              PNUMBER representation of a number.
//              int for Radix
//
//  RETURN: Greatest common divisor in internal BASEX PNUMBER form.
//
//  DESCRIPTION: gcd uses remainders to find the greatest common divisor.
//
//  ASSUMPTIONS: gcd assumes inputs are integers.
//
//  NOTE: Before it was found that the TRIM macro actually kept the
//        size down cheaper than GCD, this routine was used extensively.
//        now it is not used but might be later.
//
//-----------------------------------------------------------------------------

PNUMBER gcd( _In_ PNUMBER a, _In_ PNUMBER b)
{
    PNUMBER r= nullptr;
    PNUMBER larger= nullptr;
    PNUMBER smaller= nullptr;

    if (zernum(a))
    {
        return b;
    }
    else if(zernum(b))
    {
        return a;
    }

    if (lessnum(a, b))
    {
        DUPNUM(larger, b);
        DUPNUM(smaller, a);
    }
    else
    {
        DUPNUM(larger, a);
        DUPNUM(smaller, b);
    }

    while (!zernum(smaller))
    {
        remnum(&larger, smaller, BASEX);
        // swap larger and smaller
        r = larger;
        larger = smaller;
        smaller = r;
    }
    destroynum(smaller);
    return larger;
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: longfactnum
//
//  ARGUMENTS:
//              long integer to factorialize.
//              long integer representing base   of answer.
//              unsigned long integer for radix
//
//  RETURN: Factorial of input in radix PNUMBER form.
//
//  NOTE:  Not currently used.
//
//-----------------------------------------------------------------------------

PNUMBER longfactnum(long inlong, uint32_t radix)

{
    PNUMBER lret= nullptr;
    PNUMBER tmp= nullptr;

    lret = longtonum( 1, radix);

    while ( inlong > 0 )
        {
        tmp = longtonum( inlong--, radix);
        mulnum( &lret, tmp, radix);
        destroynum( tmp );
        }
    return( lret );
}

//-----------------------------------------------------------------------------
//
//  FUNCTION: longprodnum
//
//  ARGUMENTS:
//              long integer to factorialize.
//              long integer representing base of answer.
//              unsigned long integer for radix
//
//  RETURN: Factorial of input in base PNUMBER form.
//
//-----------------------------------------------------------------------------

PNUMBER longprodnum(long start, long stop, uint32_t radix)

{
    PNUMBER lret= nullptr;
    PNUMBER tmp= nullptr;

    lret = longtonum( 1, radix);

    while ( start <= stop )
        {
        if ( start )
            {
            tmp = longtonum( start, radix);
            mulnum( &lret, tmp, radix);
            destroynum( tmp );
            }
        start++;
        }
    return( lret );
}

//-----------------------------------------------------------------------------
//
//    FUNCTION: numpowlong
//
//    ARGUMENTS: root as number power as long and radix of
//               number along with the precision value in long.
//
//    RETURN: None root is changed.
//
//    DESCRIPTION: changes numeric representation of root to
//    root ** power. Assumes radix is the radix of root.
//
//-----------------------------------------------------------------------------

void numpowlong( _Inout_ PNUMBER *proot, long power, uint32_t radix, int32_t precision)
{
    PNUMBER lret = longtonum( 1, radix );

    while ( power > 0 )
        {
        if ( power & 1 )
            {
            mulnum( &lret, *proot, radix );
            }
        mulnum( proot, *proot, radix );
        TRIMNUM(*proot, precision);
        power >>= 1;
        }
    destroynum( *proot );
    *proot=lret;

}

//-----------------------------------------------------------------------------
//
//    FUNCTION: ratpowlong
//
//    ARGUMENTS: root as rational, power as long and precision as uint32_t.
//
//    RETURN: None root is changed.
//
//    DESCRIPTION: changes rational representation of root to
//    root ** power.
//
//-----------------------------------------------------------------------------

void ratpowlong( _Inout_ PRAT *proot, long power, int32_t precision)

{
    if ( power < 0 )
        {
        // Take the positive power and invert answer.
        PNUMBER pnumtemp = nullptr;
        ratpowlong( proot, -power, precision);
        pnumtemp = (*proot)->pp;
        (*proot)->pp  = (*proot)->pq;
        (*proot)->pq = pnumtemp;
        }
    else
        {
        PRAT lret= nullptr;

        lret = longtorat( 1 );

        while ( power > 0 )
            {
            if ( power & 1 )
                {
                mulnumx( &(lret->pp), (*proot)->pp );
                mulnumx( &(lret->pq), (*proot)->pq );
                }
            mulrat( proot, *proot, precision);
            trimit(&lret, precision);
            trimit(proot, precision);
            power >>= 1;
            }
        destroyrat( *proot );
        *proot=lret;
        }
}
