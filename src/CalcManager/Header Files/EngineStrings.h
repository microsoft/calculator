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

#pragma once

#include <array>
#include <string>
#include <string_view>
#include <unordered_map>

inline constexpr auto IDS_ERRORS_FIRST = 99;

// This is the list of error strings corresponding to SCERR_DIVIDEZERO..

inline constexpr auto IDS_DIVBYZERO = IDS_ERRORS_FIRST;
inline constexpr auto IDS_DOMAIN = IDS_ERRORS_FIRST + 1;
inline constexpr auto IDS_UNDEFINED = IDS_ERRORS_FIRST + 2;
inline constexpr auto IDS_POS_INFINITY = IDS_ERRORS_FIRST + 3;
inline constexpr auto IDS_NEG_INFINITY = IDS_ERRORS_FIRST + 4;
inline constexpr auto IDS_NOMEM = IDS_ERRORS_FIRST + 6;
inline constexpr auto IDS_TOOMANY = IDS_ERRORS_FIRST + 7;
inline constexpr auto IDS_OVERFLOW = IDS_ERRORS_FIRST + 8;
inline constexpr auto IDS_NORESULT = IDS_ERRORS_FIRST + 9;
inline constexpr auto IDS_INSUFFICIENT_DATA = IDS_ERRORS_FIRST + 10;

inline constexpr auto CSTRINGSENGMAX = IDS_INSUFFICIENT_DATA + 1;

// Arithmetic expression evaluator error strings
inline constexpr auto IDS_ERR_UNK_CH = CSTRINGSENGMAX + 1;
inline constexpr auto IDS_ERR_UNK_FN = CSTRINGSENGMAX + 2;
inline constexpr auto IDS_ERR_UNEX_NUM = CSTRINGSENGMAX + 3;
inline constexpr auto IDS_ERR_UNEX_CH = CSTRINGSENGMAX + 4;
inline constexpr auto IDS_ERR_UNEX_SZ = CSTRINGSENGMAX + 5;
inline constexpr auto IDS_ERR_MISMATCH_CLOSE = CSTRINGSENGMAX + 6;
inline constexpr auto IDS_ERR_UNEX_END = CSTRINGSENGMAX + 7;
inline constexpr auto IDS_ERR_SG_INV_ERROR = CSTRINGSENGMAX + 8;
inline constexpr auto IDS_ERR_INPUT_OVERFLOW = CSTRINGSENGMAX + 9;
inline constexpr auto IDS_ERR_OUTPUT_OVERFLOW = CSTRINGSENGMAX + 10;

// Resource keys for CEngineStrings.resw
inline constexpr auto SIDS_PLUS_MINUS = L"0";
inline constexpr auto SIDS_CLEAR = L"1";
inline constexpr auto SIDS_CE = L"2";
inline constexpr auto SIDS_BACKSPACE = L"3";
inline constexpr auto SIDS_DECIMAL_SEPARATOR = L"4";
inline constexpr auto SIDS_EMPTY_STRING = L"5";
inline constexpr auto SIDS_AND = L"6";
inline constexpr auto SIDS_OR = L"7";
inline constexpr auto SIDS_XOR = L"8";
inline constexpr auto SIDS_LSH = L"9";
inline constexpr auto SIDS_RSH = L"10";
inline constexpr auto SIDS_DIVIDE = L"11";
inline constexpr auto SIDS_MULTIPLY = L"12";
inline constexpr auto SIDS_PLUS = L"13";
inline constexpr auto SIDS_MINUS = L"14";
inline constexpr auto SIDS_MOD = L"15";
inline constexpr auto SIDS_YROOT = L"16";
inline constexpr auto SIDS_POW_HAT = L"17";
inline constexpr auto SIDS_INT = L"18";
inline constexpr auto SIDS_ROL = L"19";
inline constexpr auto SIDS_ROR = L"20";
inline constexpr auto SIDS_NOT = L"21";
inline constexpr auto SIDS_SIN = L"22";
inline constexpr auto SIDS_COS = L"23";
inline constexpr auto SIDS_TAN = L"24";
inline constexpr auto SIDS_SINH = L"25";
inline constexpr auto SIDS_COSH = L"26";
inline constexpr auto SIDS_TANH = L"27";
inline constexpr auto SIDS_LN = L"28";
inline constexpr auto SIDS_LOG = L"29";
inline constexpr auto SIDS_SQRT = L"30";
inline constexpr auto SIDS_XPOW2 = L"31";
inline constexpr auto SIDS_XPOW3 = L"32";
inline constexpr auto SIDS_NFACTORIAL = L"33";
inline constexpr auto SIDS_RECIPROCAL = L"34";
inline constexpr auto SIDS_DMS = L"35";
inline constexpr auto SIDS_POWTEN = L"37";
inline constexpr auto SIDS_PERCENT = L"38";
inline constexpr auto SIDS_SCIENTIFIC_NOTATION = L"39";
inline constexpr auto SIDS_PI = L"40";
inline constexpr auto SIDS_EQUAL = L"41";
inline constexpr auto SIDS_MC = L"42";
inline constexpr auto SIDS_MR = L"43";
inline constexpr auto SIDS_MS = L"44";
inline constexpr auto SIDS_MPLUS = L"45";
inline constexpr auto SIDS_MMINUS = L"46";
inline constexpr auto SIDS_EXP = L"47";
inline constexpr auto SIDS_OPEN_PAREN = L"48";
inline constexpr auto SIDS_CLOSE_PAREN = L"49";
inline constexpr auto SIDS_0 = L"50";
inline constexpr auto SIDS_1 = L"51";
inline constexpr auto SIDS_2 = L"52";
inline constexpr auto SIDS_3 = L"53";
inline constexpr auto SIDS_4 = L"54";
inline constexpr auto SIDS_5 = L"55";
inline constexpr auto SIDS_6 = L"56";
inline constexpr auto SIDS_7 = L"57";
inline constexpr auto SIDS_8 = L"58";
inline constexpr auto SIDS_9 = L"59";
inline constexpr auto SIDS_A = L"60";
inline constexpr auto SIDS_B = L"61";
inline constexpr auto SIDS_C = L"62";
inline constexpr auto SIDS_D = L"63";
inline constexpr auto SIDS_E = L"64";
inline constexpr auto SIDS_F = L"65";
inline constexpr auto SIDS_FRAC = L"66";
inline constexpr auto SIDS_SIND = L"67";
inline constexpr auto SIDS_COSD = L"68";
inline constexpr auto SIDS_TAND = L"69";
inline constexpr auto SIDS_ASIND = L"70";
inline constexpr auto SIDS_ACOSD = L"71";
inline constexpr auto SIDS_ATAND = L"72";
inline constexpr auto SIDS_SINR = L"73";
inline constexpr auto SIDS_COSR = L"74";
inline constexpr auto SIDS_TANR = L"75";
inline constexpr auto SIDS_ASINR = L"76";
inline constexpr auto SIDS_ACOSR = L"77";
inline constexpr auto SIDS_ATANR = L"78";
inline constexpr auto SIDS_SING = L"79";
inline constexpr auto SIDS_COSG = L"80";
inline constexpr auto SIDS_TANG = L"81";
inline constexpr auto SIDS_ASING = L"82";
inline constexpr auto SIDS_ACOSG = L"83";
inline constexpr auto SIDS_ATANG = L"84";
inline constexpr auto SIDS_ASINH = L"85";
inline constexpr auto SIDS_ACOSH = L"86";
inline constexpr auto SIDS_ATANH = L"87";
inline constexpr auto SIDS_POWE = L"88";
inline constexpr auto SIDS_POWTEN2 = L"89";
inline constexpr auto SIDS_SQRT2 = L"90";
inline constexpr auto SIDS_SQR = L"91";
inline constexpr auto SIDS_CUBE = L"92";
inline constexpr auto SIDS_CUBERT = L"93";
inline constexpr auto SIDS_FACT = L"94";
inline constexpr auto SIDS_RECIPROC = L"95";
inline constexpr auto SIDS_DEGREES = L"96";
inline constexpr auto SIDS_NEGATE = L"97";
inline constexpr auto SIDS_RSH2 = L"98";
inline constexpr auto SIDS_DIVIDEBYZERO = L"99";
inline constexpr auto SIDS_DOMAIN = L"100";
inline constexpr auto SIDS_UNDEFINED = L"101";
inline constexpr auto SIDS_POS_INFINITY = L"102";
inline constexpr auto SIDS_NEG_INFINITY = L"103";
inline constexpr auto SIDS_ABORTED = L"104";
inline constexpr auto SIDS_NOMEM = L"105";
inline constexpr auto SIDS_TOOMANY = L"106";
inline constexpr auto SIDS_OVERFLOW = L"107";
inline constexpr auto SIDS_NORESULT = L"108";
inline constexpr auto SIDS_INSUFFICIENT_DATA = L"109";
// 110 is skipped by CSTRINGSENGMAX
inline constexpr auto SIDS_ERR_UNK_CH = L"111";
inline constexpr auto SIDS_ERR_UNK_FN = L"112";
inline constexpr auto SIDS_ERR_UNEX_NUM = L"113";
inline constexpr auto SIDS_ERR_UNEX_CH = L"114";
inline constexpr auto SIDS_ERR_UNEX_SZ = L"115";
inline constexpr auto SIDS_ERR_MISMATCH_CLOSE = L"116";
inline constexpr auto SIDS_ERR_UNEX_END = L"117";
inline constexpr auto SIDS_ERR_SG_INV_ERROR = L"118";
inline constexpr auto SIDS_ERR_INPUT_OVERFLOW = L"119";
inline constexpr auto SIDS_ERR_OUTPUT_OVERFLOW = L"120";
inline constexpr auto SIDS_SECD = L"SecDeg";
inline constexpr auto SIDS_SECR = L"SecRad";
inline constexpr auto SIDS_SECG = L"SecGrad";
inline constexpr auto SIDS_ASECD = L"InverseSecDeg";
inline constexpr auto SIDS_ASECR = L"InverseSecRad";
inline constexpr auto SIDS_ASECG = L"InverseSecGrad";
inline constexpr auto SIDS_CSCD = L"CscDeg";
inline constexpr auto SIDS_CSCR = L"CscRad";
inline constexpr auto SIDS_CSCG = L"CscGrad";
inline constexpr auto SIDS_ACSCD = L"InverseCscDeg";
inline constexpr auto SIDS_ACSCR = L"InverseCscRad";
inline constexpr auto SIDS_ACSCG = L"InverseCscGrad";
inline constexpr auto SIDS_COTD = L"CotDeg";
inline constexpr auto SIDS_COTR = L"CotRad";
inline constexpr auto SIDS_COTG = L"CotGrad";
inline constexpr auto SIDS_ACOTD = L"InverseCotDeg";
inline constexpr auto SIDS_ACOTR = L"InverseCotRad";
inline constexpr auto SIDS_ACOTG = L"InverseCotGrad";
inline constexpr auto SIDS_SECH = L"Sech";
inline constexpr auto SIDS_ASECH = L"InverseSech";
inline constexpr auto SIDS_CSCH = L"Csch";
inline constexpr auto SIDS_ACSCH = L"InverseCsch";
inline constexpr auto SIDS_COTH = L"Coth";
inline constexpr auto SIDS_ACOTH = L"InverseCoth";
inline constexpr auto SIDS_TWOPOWX = L"TwoPowX";
inline constexpr auto SIDS_LOGBASEY = L"LogBaseY";
inline constexpr auto SIDS_ABS = L"Abs";
inline constexpr auto SIDS_FLOOR = L"Floor";
inline constexpr auto SIDS_CEIL = L"Ceil";
inline constexpr auto SIDS_NAND = L"Nand";
inline constexpr auto SIDS_NOR = L"Nor";
inline constexpr auto SIDS_CUBEROOT = L"CubeRoot";
inline constexpr auto SIDS_PROGRAMMER_MOD = L"ProgrammerMod";

// Include the resource key ID from above into this vector to load it into memory for the engine to use
inline constexpr std::array<std::wstring_view, 152> g_sids = {
    SIDS_PLUS_MINUS,
    SIDS_C,
    SIDS_CE,
    SIDS_BACKSPACE,
    SIDS_DECIMAL_SEPARATOR,
    SIDS_EMPTY_STRING,
    SIDS_AND,
    SIDS_OR,
    SIDS_XOR,
    SIDS_LSH,
    SIDS_RSH,
    SIDS_DIVIDE,
    SIDS_MULTIPLY,
    SIDS_PLUS,
    SIDS_MINUS,
    SIDS_MOD,
    SIDS_YROOT,
    SIDS_POW_HAT,
    SIDS_INT,
    SIDS_ROL,
    SIDS_ROR,
    SIDS_NOT,
    SIDS_SIN,
    SIDS_COS,
    SIDS_TAN,
    SIDS_SINH,
    SIDS_COSH,
    SIDS_TANH,
    SIDS_LN,
    SIDS_LOG,
    SIDS_SQRT,
    SIDS_XPOW2,
    SIDS_XPOW3,
    SIDS_NFACTORIAL,
    SIDS_RECIPROCAL,
    SIDS_DMS,
    SIDS_POWTEN,
    SIDS_PERCENT,
    SIDS_SCIENTIFIC_NOTATION,
    SIDS_PI,
    SIDS_EQUAL,
    SIDS_MC,
    SIDS_MR,
    SIDS_MS,
    SIDS_MPLUS,
    SIDS_MMINUS,
    SIDS_EXP,
    SIDS_OPEN_PAREN,
    SIDS_CLOSE_PAREN,
    SIDS_0,
    SIDS_1,
    SIDS_2,
    SIDS_3,
    SIDS_4,
    SIDS_5,
    SIDS_6,
    SIDS_7,
    SIDS_8,
    SIDS_9,
    SIDS_A,
    SIDS_B,
    SIDS_C,
    SIDS_D,
    SIDS_E,
    SIDS_F,
    SIDS_FRAC,
    SIDS_SIND,
    SIDS_COSD,
    SIDS_TAND,
    SIDS_ASIND,
    SIDS_ACOSD,
    SIDS_ATAND,
    SIDS_SINR,
    SIDS_COSR,
    SIDS_TANR,
    SIDS_ASINR,
    SIDS_ACOSR,
    SIDS_ATANR,
    SIDS_SING,
    SIDS_COSG,
    SIDS_TANG,
    SIDS_ASING,
    SIDS_ACOSG,
    SIDS_ATANG,
    SIDS_ASINH,
    SIDS_ACOSH,
    SIDS_ATANH,
    SIDS_POWE,
    SIDS_POWTEN2,
    SIDS_SQRT2,
    SIDS_SQR,
    SIDS_CUBE,
    SIDS_CUBERT,
    SIDS_FACT,
    SIDS_RECIPROC,
    SIDS_DEGREES,
    SIDS_NEGATE,
    SIDS_RSH,
    SIDS_DIVIDEBYZERO,
    SIDS_DOMAIN,
    SIDS_UNDEFINED,
    SIDS_POS_INFINITY,
    SIDS_NEG_INFINITY,
    SIDS_ABORTED,
    SIDS_NOMEM,
    SIDS_TOOMANY,
    SIDS_OVERFLOW,
    SIDS_NORESULT,
    SIDS_INSUFFICIENT_DATA,
    SIDS_ERR_UNK_CH,
    SIDS_ERR_UNK_FN,
    SIDS_ERR_UNEX_NUM,
    SIDS_ERR_UNEX_CH,
    SIDS_ERR_UNEX_SZ,
    SIDS_ERR_MISMATCH_CLOSE,
    SIDS_ERR_UNEX_END,
    SIDS_ERR_SG_INV_ERROR,
    SIDS_ERR_INPUT_OVERFLOW,
    SIDS_ERR_OUTPUT_OVERFLOW,
    SIDS_SECD,
    SIDS_SECG,
    SIDS_SECR,
    SIDS_ASECD,
    SIDS_ASECR,
    SIDS_ASECG,
    SIDS_CSCD,
    SIDS_CSCR,
    SIDS_CSCG,
    SIDS_ACSCD,
    SIDS_ACSCR,
    SIDS_ACSCG,
    SIDS_COTD,
    SIDS_COTR,
    SIDS_COTG,
    SIDS_ACOTD,
    SIDS_ACOTR,
    SIDS_ACOTG,
    SIDS_SECH,
    SIDS_ASECH,
    SIDS_CSCH,
    SIDS_ACSCH,
    SIDS_COTH,
    SIDS_ACOTH,
    SIDS_TWOPOWX,
    SIDS_LOGBASEY,
    SIDS_ABS,
    SIDS_FLOOR,
    SIDS_CEIL,
    SIDS_NAND,
    SIDS_NOR,
    SIDS_CUBEROOT,
    SIDS_PROGRAMMER_MOD,
};
