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
inline constexpr auto SIDS_CUBEROOT = L"36";
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

// Include the resource key ID from above into this vector to load it into memory for the engine to use
__declspec(selectany) std::vector<std::wstring> g_sids =
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
