// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/****************************Module*Header***********************************
* Module Name: EngineStrings.h
*
* Module Description:
*       Resource String ID's for the private strings used by Engine. Internal to Engine related code
*         not required by the clients
*
* Warnings:
*
* Created: 13-Feb-2008
*
\****************************************************************************/
#define IDS_FIRSTENGSTR     IDS_ENGINESTR_FIRST

#define IDS_DECIMAL     4

// All unary op function names for easy history reading
// This is where the first string after all the commands in order have been placed, should be placed
// keeping in consecutive helps us to allocate 1 string table and index them
#define IDS_FNSZFIRST   (IDC_F -IDC_FIRSTCONTROL)+1

#define IDS_FRAC        IDS_FNSZFIRST

#define IDS_SIND        IDS_FNSZFIRST+1
#define IDS_COSD        IDS_FNSZFIRST+2
#define IDS_TAND        IDS_FNSZFIRST+3
#define IDS_ASIND       IDS_FNSZFIRST+4
#define IDS_ACOSD       IDS_FNSZFIRST+5
#define IDS_ATAND       IDS_FNSZFIRST+6

#define IDS_SINR        IDS_FNSZFIRST+7
#define IDS_COSR        IDS_FNSZFIRST+8
#define IDS_TANR        IDS_FNSZFIRST+9
#define IDS_ASINR       IDS_FNSZFIRST+10
#define IDS_ACOSR       IDS_FNSZFIRST+11
#define IDS_ATANR       IDS_FNSZFIRST+12

#define IDS_SING        IDS_FNSZFIRST+13
#define IDS_COSG        IDS_FNSZFIRST+14
#define IDS_TANG        IDS_FNSZFIRST+15
#define IDS_ASING       IDS_FNSZFIRST+16
#define IDS_ACOSG       IDS_FNSZFIRST+17
#define IDS_ATANG       IDS_FNSZFIRST+18

#define IDS_ASINH       IDS_FNSZFIRST+19
#define IDS_ACOSH       IDS_FNSZFIRST+20
#define IDS_ATANH       IDS_FNSZFIRST+21
#define IDS_POWE        IDS_FNSZFIRST+22
#define IDS_POW10       IDS_FNSZFIRST+23
#define IDS_SQRT        IDS_FNSZFIRST+24
#define IDS_SQR         IDS_FNSZFIRST+25
#define IDS_CUBE        IDS_FNSZFIRST+26
#define IDS_CUBERT      IDS_FNSZFIRST+27
#define IDS_FACT        IDS_FNSZFIRST+28
#define IDS_REC         IDS_FNSZFIRST+29
#define IDS_DEGREES     IDS_FNSZFIRST+30
#define IDS_NEGATE      IDS_FNSZFIRST+31
#define IDS_RSH         IDS_FNSZFIRST+32

#define IDS_FNSZLAST    IDS_RSH

#define IDS_ERRORS_FIRST     IDS_FNSZLAST+1

// This is the list of error strings corresponding to SCERR_DIVIDEZERO..

#define IDS_DIVBYZERO           IDS_ERRORS_FIRST
#define IDS_DOMAIN              IDS_ERRORS_FIRST+1
#define IDS_UNDEFINED           IDS_ERRORS_FIRST+2
#define IDS_POS_INFINITY        IDS_ERRORS_FIRST+3
#define IDS_NEG_INFINITY        IDS_ERRORS_FIRST+4
#define IDS_NOMEM               IDS_ERRORS_FIRST+6
#define IDS_TOOMANY             IDS_ERRORS_FIRST+7
#define IDS_OVERFLOW            IDS_ERRORS_FIRST+8
#define IDS_NORESULT            IDS_ERRORS_FIRST+9
#define IDS_INSUFFICIENT_DATA   IDS_ERRORS_FIRST+10

#define CSTRINGSENGMAX       IDS_INSUFFICIENT_DATA+1

// Arithmetic expression evaluator error strings
#define IDS_ERR_UNK_CH           CSTRINGSENGMAX+1
#define IDS_ERR_UNK_FN           CSTRINGSENGMAX+2
#define IDS_ERR_UNEX_NUM         CSTRINGSENGMAX+3
#define IDS_ERR_UNEX_CH          CSTRINGSENGMAX+4
#define IDS_ERR_UNEX_SZ          CSTRINGSENGMAX+5
#define IDS_ERR_MISMATCH_CLOSE   CSTRINGSENGMAX+6
#define IDS_ERR_UNEX_END         CSTRINGSENGMAX+7
#define IDS_ERR_SG_INV_ERROR     CSTRINGSENGMAX+8
#define IDS_ERR_INPUT_OVERFLOW   CSTRINGSENGMAX+9
#define IDS_ERR_OUTPUT_OVERFLOW  CSTRINGSENGMAX+10


#define SIDS_PLUS_MINUS L"0"
#define SIDS_CLEAR L"1"
#define SIDS_CE L"2"
#define SIDS_BACKSPACE L"3"
#define SIDS_DECIMAL_SEPARATOR L"4"
#define SIDS_EMPTY_STRING L"5"
#define SIDS_AND L"6"
#define SIDS_OR L"7"
#define SIDS_XOR L"8"
#define SIDS_LSH L"9"
#define SIDS_RSH L"10"
#define SIDS_DIVIDE L"11"
#define SIDS_MULTIPLY L"12"
#define SIDS_PLUS L"13"
#define SIDS_MINUS L"14"
#define SIDS_MOD L"15"
#define SIDS_YROOT L"16"
#define SIDS_POW_HAT L"17"
#define SIDS_INT L"18"
#define SIDS_ROL L"19"
#define SIDS_ROR L"20"
#define SIDS_NOT L"21"
#define SIDS_SIN L"22"
#define SIDS_COS L"23"
#define SIDS_TAN L"24"
#define SIDS_SINH L"25"
#define SIDS_COSH L"26"
#define SIDS_TANH L"27"
#define SIDS_LN L"28"
#define SIDS_LOG L"29"
#define SIDS_SQRT L"30"
#define SIDS_XPOW2 L"31"
#define SIDS_XPOW3 L"32"
#define SIDS_NFACTORIAL L"33"
#define SIDS_RECIPROCAL L"34"
#define SIDS_DMS L"35"
#define SIDS_CUBEROOT L"36"    
#define SIDS_POWTEN L"37"
#define SIDS_PERCENT L"38"
#define SIDS_SCIENTIFIC_NOTATION L"39"
#define SIDS_PI L"40"
#define SIDS_EQUAL L"41"
#define SIDS_MC L"42"
#define SIDS_MR L"43"
#define SIDS_MS L"44"
#define SIDS_MPLUS L"45"
#define SIDS_MMINUS L"46"
#define SIDS_EXP L"47"
#define SIDS_OPEN_PAREN L"48"
#define SIDS_CLOSE_PAREN L"49"
#define SIDS_0 L"50"
#define SIDS_1 L"51"
#define SIDS_2 L"52"
#define SIDS_3 L"53"
#define SIDS_4 L"54"
#define SIDS_5 L"55"
#define SIDS_6 L"56"
#define SIDS_7 L"57"
#define SIDS_8 L"58"
#define SIDS_9 L"59"
#define SIDS_A L"60"
#define SIDS_B L"61"
#define SIDS_C L"62"
#define SIDS_D L"63"
#define SIDS_E L"64"
#define SIDS_F L"65"
#define SIDS_FRAC L"66"
#define SIDS_SIND L"67"
#define SIDS_COSD L"68"
#define SIDS_TAND L"69"
#define SIDS_ASIND L"70"
#define SIDS_ACOSD L"71"
#define SIDS_ATAND L"72"
#define SIDS_SINR L"73"
#define SIDS_COSR L"74"
#define SIDS_TANR L"75"
#define SIDS_ASINR L"76"
#define SIDS_ACOSR L"77"
#define SIDS_ATANR L"78"
#define SIDS_SING L"79"
#define SIDS_COSG L"80"
#define SIDS_TANG L"81"
#define SIDS_ASING L"82"
#define SIDS_ACOSG L"83"
#define SIDS_ATANG L"84"
#define SIDS_ASINH L"85"
#define SIDS_ACOSH L"86"
#define SIDS_ATANH L"87"
#define SIDS_POWE L"88"
#define SIDS_POWTEN2 L"89"
#define SIDS_SQRT2 L"90"
#define SIDS_SQR L"91"
#define SIDS_CUBE L"92"
#define SIDS_CUBERT L"93"
#define SIDS_FACT L"94"
#define SIDS_RECIPROC L"95"
#define SIDS_DEGREES L"96"
#define SIDS_NEGATE L"97"
#define SIDS_RSH2 L"98"
#define SIDS_DIVIDEBYZERO L"99"
#define SIDS_DOMAIN L"100"
#define SIDS_UNDEFINED L"101"
#define SIDS_POS_INFINITY L"102"
#define SIDS_NEG_INFINITY L"103"
#define SIDS_ABORTED L"104"
#define SIDS_NOMEM L"105"
#define SIDS_TOOMANY L"106"
#define SIDS_OVERFLOW L"107"
#define SIDS_NORESULT L"108" 
#define SIDS_INSUFFICIENT_DATA L"109"
// 110 is skipped by CSTRINGSENGMAX
#define SIDS_ERR_UNK_CH L"111"
#define SIDS_ERR_UNK_FN L"112"
#define SIDS_ERR_UNEX_NUM L"113"
#define SIDS_ERR_UNEX_CH L"114"
#define SIDS_ERR_UNEX_SZ L"115"
#define SIDS_ERR_MISMATCH_CLOSE L"116"
#define SIDS_ERR_UNEX_END L"117"
#define SIDS_ERR_SG_INV_ERROR L"118"
#define SIDS_ERR_INPUT_OVERFLOW L"119"
#define SIDS_ERR_OUTPUT_OVERFLOW L"120"

__declspec(selectany) std::wstring g_sids[] = 
{
    std::wstring(SIDS_PLUS_MINUS),
    std::wstring(SIDS_C),
    std::wstring(SIDS_CE),
    std::wstring(SIDS_BACKSPACE),
    std::wstring(SIDS_DECIMAL_SEPARATOR),
    std::wstring(SIDS_EMPTY_STRING),
    std::wstring(SIDS_AND),
    std::wstring(SIDS_OR),
    std::wstring(SIDS_XOR),
    std::wstring(SIDS_LSH),
    std::wstring(SIDS_RSH),
    std::wstring(SIDS_DIVIDE),
    std::wstring(SIDS_MULTIPLY),
    std::wstring(SIDS_PLUS),
    std::wstring(SIDS_MINUS),
    std::wstring(SIDS_MOD),
    std::wstring(SIDS_YROOT),
    std::wstring(SIDS_POW_HAT),
    std::wstring(SIDS_INT),
    std::wstring(SIDS_ROL),
    std::wstring(SIDS_ROR),
    std::wstring(SIDS_NOT),
    std::wstring(SIDS_SIN),
    std::wstring(SIDS_COS),
    std::wstring(SIDS_TAN),
    std::wstring(SIDS_SINH),
    std::wstring(SIDS_COSH),
    std::wstring(SIDS_TANH),
    std::wstring(SIDS_LN),
    std::wstring(SIDS_LOG),
    std::wstring(SIDS_SQRT),
    std::wstring(SIDS_XPOW2),
    std::wstring(SIDS_XPOW3),
    std::wstring(SIDS_NFACTORIAL),
    std::wstring(SIDS_RECIPROCAL),
    std::wstring(SIDS_DMS),
    std::wstring(SIDS_CUBEROOT),    
    std::wstring(SIDS_POWTEN),
    std::wstring(SIDS_PERCENT),
    std::wstring(SIDS_SCIENTIFIC_NOTATION),
    std::wstring(SIDS_PI),
    std::wstring(SIDS_EQUAL),
    std::wstring(SIDS_MC),
    std::wstring(SIDS_MR),
    std::wstring(SIDS_MS),
    std::wstring(SIDS_MPLUS),
    std::wstring(SIDS_MMINUS),
    std::wstring(SIDS_EXP),
    std::wstring(SIDS_OPEN_PAREN),
    std::wstring(SIDS_CLOSE_PAREN),
    std::wstring(SIDS_0),
    std::wstring(SIDS_1),
    std::wstring(SIDS_2),
    std::wstring(SIDS_3),
    std::wstring(SIDS_4),
    std::wstring(SIDS_5),
    std::wstring(SIDS_6),
    std::wstring(SIDS_7),
    std::wstring(SIDS_8),
    std::wstring(SIDS_9),
    std::wstring(SIDS_A),
    std::wstring(SIDS_B),
    std::wstring(SIDS_C),
    std::wstring(SIDS_D),
    std::wstring(SIDS_E),
    std::wstring(SIDS_F),
    std::wstring(SIDS_FRAC),
    std::wstring(SIDS_SIND),
    std::wstring(SIDS_COSD),
    std::wstring(SIDS_TAND),
    std::wstring(SIDS_ASIND),
    std::wstring(SIDS_ACOSD),
    std::wstring(SIDS_ATAND),
    std::wstring(SIDS_SINR),
    std::wstring(SIDS_COSR),
    std::wstring(SIDS_TANR),
    std::wstring(SIDS_ASINR),
    std::wstring(SIDS_ACOSR),
    std::wstring(SIDS_ATANR),
    std::wstring(SIDS_SING),
    std::wstring(SIDS_COSG),
    std::wstring(SIDS_TANG),
    std::wstring(SIDS_ASING),
    std::wstring(SIDS_ACOSG),
    std::wstring(SIDS_ATANG),
    std::wstring(SIDS_ASINH),
    std::wstring(SIDS_ACOSH),
    std::wstring(SIDS_ATANH),
    std::wstring(SIDS_POWE),
    std::wstring(SIDS_POWTEN2),
    std::wstring(SIDS_SQRT2),
    std::wstring(SIDS_SQR),
    std::wstring(SIDS_CUBE),
    std::wstring(SIDS_CUBERT),
    std::wstring(SIDS_FACT),
    std::wstring(SIDS_RECIPROC),
    std::wstring(SIDS_DEGREES),
    std::wstring(SIDS_NEGATE),
    std::wstring(SIDS_RSH),
    std::wstring(SIDS_DIVIDEBYZERO),
    std::wstring(SIDS_DOMAIN),
    std::wstring(SIDS_UNDEFINED),
    std::wstring(SIDS_POS_INFINITY),
    std::wstring(SIDS_NEG_INFINITY),
    std::wstring(SIDS_ABORTED),
    std::wstring(SIDS_NOMEM),
    std::wstring(SIDS_TOOMANY),
    std::wstring(SIDS_OVERFLOW),
    std::wstring(SIDS_NORESULT), 
    std::wstring(SIDS_INSUFFICIENT_DATA),
    std::wstring(SIDS_ERR_UNK_CH),
    std::wstring(SIDS_ERR_UNK_FN),
    std::wstring(SIDS_ERR_UNEX_NUM),
    std::wstring(SIDS_ERR_UNEX_CH),
    std::wstring(SIDS_ERR_UNEX_SZ),
    std::wstring(SIDS_ERR_MISMATCH_CLOSE),
    std::wstring(SIDS_ERR_UNEX_END),
    std::wstring(SIDS_ERR_SG_INV_ERROR),
    std::wstring(SIDS_ERR_INPUT_OVERFLOW),
    std::wstring(SIDS_ERR_OUTPUT_OVERFLOW)
};
