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

using System;
using System.Collections.Generic;
using System.Text;

namespace CalculationManager
{
    public static class EngineStrings
    {
        public const int IDS_ERRORS_FIRST = 99;

        // This is the list of error strings corresponding to SCERR_DIVIDEZERO..

        public const int IDS_DIVBYZERO = IDS_ERRORS_FIRST;
        public const int IDS_DOMAIN = IDS_ERRORS_FIRST + 1;
        public const int IDS_UNDEFINED = IDS_ERRORS_FIRST + 2;
        public const int IDS_POS_INFINITY = IDS_ERRORS_FIRST + 3;
        public const int IDS_NEG_INFINITY = IDS_ERRORS_FIRST + 4;
        public const int IDS_NOMEM = IDS_ERRORS_FIRST + 6;
        public const int IDS_TOOMANY = IDS_ERRORS_FIRST + 7;
        public const int IDS_OVERFLOW = IDS_ERRORS_FIRST + 8;
        public const int IDS_NORESULT = IDS_ERRORS_FIRST + 9;
        public const int IDS_INSUFFICIENT_DATA = IDS_ERRORS_FIRST + 10;

        public const int CSTRINGSENGMAX = IDS_INSUFFICIENT_DATA + 1;

        // Arithmetic expression evaluator error strings
        public const int IDS_ERR_UNK_CH = CSTRINGSENGMAX + 1;
        public const int IDS_ERR_UNK_FN = CSTRINGSENGMAX + 2;
        public const int IDS_ERR_UNEX_NUM = CSTRINGSENGMAX + 3;
        public const int IDS_ERR_UNEX_CH = CSTRINGSENGMAX + 4;
        public const int IDS_ERR_UNEX_SZ = CSTRINGSENGMAX + 5;
        public const int IDS_ERR_MISMATCH_CLOSE = CSTRINGSENGMAX + 6;
        public const int IDS_ERR_UNEX_END = CSTRINGSENGMAX + 7;
        public const int IDS_ERR_SG_INV_ERROR = CSTRINGSENGMAX + 8;
        public const int IDS_ERR_INPUT_OVERFLOW = CSTRINGSENGMAX + 9;
        public const int IDS_ERR_OUTPUT_OVERFLOW = CSTRINGSENGMAX + 10;

        // Resource keys for CEngineStrings.resw
        public const string SIDS_PLUS_MINUS = "0";
        public const string SIDS_CLEAR = "1";
        public const string SIDS_CE = "2";
        public const string SIDS_BACKSPACE = "3";
        public const string SIDS_DECIMAL_SEPARATOR = "4";
        public const string SIDS_EMPTY_STRING = "5";
        public const string SIDS_AND = "6";
        public const string SIDS_OR = "7";
        public const string SIDS_XOR = "8";
        public const string SIDS_LSH = "9";
        public const string SIDS_RSH = "10";
        public const string SIDS_DIVIDE = "11";
        public const string SIDS_MULTIPLY = "12";
        public const string SIDS_PLUS = "13";
        public const string SIDS_MINUS = "14";
        public const string SIDS_MOD = "15";
        public const string SIDS_YROOT = "16";
        public const string SIDS_POW_HAT = "17";
        public const string SIDS_INT = "18";
        public const string SIDS_ROL = "19";
        public const string SIDS_ROR = "20";
        public const string SIDS_NOT = "21";
        public const string SIDS_SIN = "22";
        public const string SIDS_COS = "23";
        public const string SIDS_TAN = "24";
        public const string SIDS_SINH = "25";
        public const string SIDS_COSH = "26";
        public const string SIDS_TANH = "27";
        public const string SIDS_LN = "28";
        public const string SIDS_LOG = "29";
        public const string SIDS_SQRT = "30";
        public const string SIDS_XPOW2 = "31";
        public const string SIDS_XPOW3 = "32";
        public const string SIDS_NFACTORIAL = "33";
        public const string SIDS_RECIPROCAL = "34";
        public const string SIDS_DMS = "35";
        public const string SIDS_CUBEROOT = "36";
        public const string SIDS_POWTEN = "37";
        public const string SIDS_PERCENT = "38";
        public const string SIDS_SCIENTIFIC_NOTATION = "39";
        public const string SIDS_PI = "40";
        public const string SIDS_EQUAL = "41";
        public const string SIDS_MC = "42";
        public const string SIDS_MR = "43";
        public const string SIDS_MS = "44";
        public const string SIDS_MPLUS = "45";
        public const string SIDS_MMINUS = "46";
        public const string SIDS_EXP = "47";
        public const string SIDS_OPEN_PAREN = "48";
        public const string SIDS_CLOSE_PAREN = "49";
        public const string SIDS_0 = "50";
        public const string SIDS_1 = "51";
        public const string SIDS_2 = "52";
        public const string SIDS_3 = "53";
        public const string SIDS_4 = "54";
        public const string SIDS_5 = "55";
        public const string SIDS_6 = "56";
        public const string SIDS_7 = "57";
        public const string SIDS_8 = "58";
        public const string SIDS_9 = "59";
        public const string SIDS_A = "60";
        public const string SIDS_B = "61";
        public const string SIDS_C = "62";
        public const string SIDS_D = "63";
        public const string SIDS_E = "64";
        public const string SIDS_F = "65";
        public const string SIDS_FRAC = "66";
        public const string SIDS_SIND = "67";
        public const string SIDS_COSD = "68";
        public const string SIDS_TAND = "69";
        public const string SIDS_ASIND = "70";
        public const string SIDS_ACOSD = "71";
        public const string SIDS_ATAND = "72";
        public const string SIDS_SINR = "73";
        public const string SIDS_COSR = "74";
        public const string SIDS_TANR = "75";
        public const string SIDS_ASINR = "76";
        public const string SIDS_ACOSR = "77";
        public const string SIDS_ATANR = "78";
        public const string SIDS_SING = "79";
        public const string SIDS_COSG = "80";
        public const string SIDS_TANG = "81";
        public const string SIDS_ASING = "82";
        public const string SIDS_ACOSG = "83";
        public const string SIDS_ATANG = "84";
        public const string SIDS_ASINH = "85";
        public const string SIDS_ACOSH = "86";
        public const string SIDS_ATANH = "87";
        public const string SIDS_POWE = "88";
        public const string SIDS_POWTEN2 = "89";
        public const string SIDS_SQRT2 = "90";
        public const string SIDS_SQR = "91";
        public const string SIDS_CUBE = "92";
        public const string SIDS_CUBERT = "93";
        public const string SIDS_FACT = "94";
        public const string SIDS_RECIPROC = "95";
        public const string SIDS_DEGREES = "96";
        public const string SIDS_NEGATE = "97";
        public const string SIDS_RSH2 = "98";
        public const string SIDS_DIVIDEBYZERO = "99";
        public const string SIDS_DOMAIN = "100";
        public const string SIDS_UNDEFINED = "101";
        public const string SIDS_POS_INFINITY = "102";
        public const string SIDS_NEG_INFINITY = "103";
        public const string SIDS_ABORTED = "104";
        public const string SIDS_NOMEM = "105";
        public const string SIDS_TOOMANY = "106";
        public const string SIDS_OVERFLOW = "107";
        public const string SIDS_NORESULT = "108";
        public const string SIDS_INSUFFICIENT_DATA = "109";
        // 110 is skipped by CSTRINGSENGMAX
        public const string SIDS_ERR_UNK_CH = "111";
        public const string SIDS_ERR_UNK_FN = "112";
        public const string SIDS_ERR_UNEX_NUM = "113";
        public const string SIDS_ERR_UNEX_CH = "114";
        public const string SIDS_ERR_UNEX_SZ = "115";
        public const string SIDS_ERR_MISMATCH_CLOSE = "116";
        public const string SIDS_ERR_UNEX_END = "117";
        public const string SIDS_ERR_SG_INV_ERROR = "118";
        public const string SIDS_ERR_INPUT_OVERFLOW = "119";
        public const string SIDS_ERR_OUTPUT_OVERFLOW = "120";

        // Include the resource key ID from above into this vector to load it into memory for the engine to use
        public static readonly string[] g_sids = { SIDS_PLUS_MINUS,
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
                                            SIDS_CUBEROOT,
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
                                            SIDS_ERR_OUTPUT_OVERFLOW };

    }
}
