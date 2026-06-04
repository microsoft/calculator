// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace UnitConversionManager
{
    enum class Command
    {
        Zero,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Decimal,
        Negate,
        Backspace,
        Clear,
        Reset,
        None
    };
}

namespace CurrencyConversionManager
{
    enum class Command
    {
        Zero,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Decimal,
        Negate,
        Backspace,
        Clear,
        None
    };
}

namespace CalculationManager
{
    enum class CommandType
    {
        UnaryCommand,
        BinaryCommand,
        OperandCommand,
        Parentheses
    };

    enum class Command
    {
        // Commands for programmer calculators are omitted.
        CommandDEG = 321,
        CommandRAD = 322,
        CommandGRAD = 323,
        CommandDegrees = 324,
        CommandHYP = 325,
        CommandENG = 326,

        CommandNULL = 0,

        CommandSIGN = 80,
        CommandCLEAR = 81,
        CommandCENTR = 82,
        CommandBACK = 83,

        CommandPNT = 84,

        // Hole  85
        // Unused commands defined in Command.h is omitted.
        CommandXor = 88,
        CommandLSHF = 89,
        CommandRSHF = 90,
        CommandDIV = 91,
        CommandMUL = 92,
        CommandADD = 93,
        CommandSUB = 94,
        CommandMOD = 95,
        CommandROOT = 96,
        CommandPWR = 97,

        CommandCHOP = 98, // Unary operators must be between CommandCHOP and CommandEQU
        CommandROL = 99,
        CommandROR = 100,
        CommandCOM = 101,

        CommandSIN = 102,
        CommandCOS = 103,
        CommandTAN = 104,

        CommandSINH = 105,
        CommandCOSH = 106,
        CommandTANH = 107,

        CommandLN = 108,
        CommandLOG = 109,
        CommandSQRT = 110,
        CommandSQR = 111,
        CommandCUB = 112,
        CommandFAC = 113,
        CommandREC = 114,
        CommandDMS = 115,
        CommandCUBEROOT = 116, // x ^ 1/3
        CommandPOW10 = 117,    // 10 ^ x
        CommandPERCENT = 118,

        CommandFE = 119,
        CommandPI = 120,
        CommandEQU = 121,

        CommandMCLEAR = 122,
        CommandRECALL = 123,
        CommandSTORE = 124,
        CommandMPLUS = 125,
        CommandMMINUS = 126,

        CommandEXP = 127,

        CommandOPENP = 128,
        CommandCLOSEP = 129,

        Command0 = 130, // The controls for 0 through F must be consecutive and in order
        Command1 = 131,
        Command2 = 132,
        Command3 = 133,
        Command4 = 134,
        Command5 = 135,
        Command6 = 136,
        Command7 = 137,
        Command8 = 138,
        Command9 = 139,
        CommandA = 140,
        CommandB = 141,
        CommandC = 142,
        CommandD = 143,
        CommandE = 144,
        CommandF = 145, // this is last control ID which must match the string table
        CommandINV = 146,
        CommandSET_RESULT = 147,

        CommandSEC = 400,
        CommandASEC = 401,
        CommandCSC = 402,
        CommandACSC = 403,
        CommandCOT = 404,
        CommandACOT = 405,

        CommandSECH = 406,
        CommandASECH = 407,
        CommandCSCH = 408,
        CommandACSCH = 409,
        CommandCOTH = 410,
        CommandACOTH = 411,

        CommandPOW2 = 412, // 2 ^ x
        CommandAbs = 413,
        CommandFloor = 414,
        CommandCeil = 415,
        CommandROLC = 416,
        CommandRORC = 417,
        CommandLogBaseY = 500,
        CommandNand = 501,
        CommandNor = 502,

        CommandRSHFL = 505,
        CommandRand = 600,
        CommandEuler = 601,

        CommandAnd = 86,
        CommandOR = 87,
        CommandNot = 101,

        ModeBasic = 200,
        ModeScientific = 201,

        CommandASIN = 202,
        CommandACOS = 203,
        CommandATAN = 204,
        CommandPOWE = 205,
        CommandASINH = 206,
        CommandACOSH = 207,
        CommandATANH = 208,

        ModeProgrammer = 209,
        CommandHex = 313,
        CommandDec = 314,
        CommandOct = 315,
        CommandBin = 316,
        CommandQword = 317,
        CommandDword = 318,
        CommandWord = 319,
        CommandByte = 320,

        CommandBINEDITSTART = 700,
        CommandBINPOS0 = 700,
        CommandBINPOS1 = 701,
        CommandBINPOS2 = 702,
        CommandBINPOS3 = 703,
        CommandBINPOS4 = 704,
        CommandBINPOS5 = 705,
        CommandBINPOS6 = 706,
        CommandBINPOS7 = 707,
        CommandBINPOS8 = 708,
        CommandBINPOS9 = 709,
        CommandBINPOS10 = 710,
        CommandBINPOS11 = 711,
        CommandBINPOS12 = 712,
        CommandBINPOS13 = 713,
        CommandBINPOS14 = 714,
        CommandBINPOS15 = 715,
        CommandBINPOS16 = 716,
        CommandBINPOS17 = 717,
        CommandBINPOS18 = 718,
        CommandBINPOS19 = 719,
        CommandBINPOS20 = 720,
        CommandBINPOS21 = 721,
        CommandBINPOS22 = 722,
        CommandBINPOS23 = 723,
        CommandBINPOS24 = 724,
        CommandBINPOS25 = 725,
        CommandBINPOS26 = 726,
        CommandBINPOS27 = 727,
        CommandBINPOS28 = 728,
        CommandBINPOS29 = 729,
        CommandBINPOS30 = 730,
        CommandBINPOS31 = 731,
        CommandBINPOS32 = 732,
        CommandBINPOS33 = 733,
        CommandBINPOS34 = 734,
        CommandBINPOS35 = 735,
        CommandBINPOS36 = 736,
        CommandBINPOS37 = 737,
        CommandBINPOS38 = 738,
        CommandBINPOS39 = 739,
        CommandBINPOS40 = 740,
        CommandBINPOS41 = 741,
        CommandBINPOS42 = 742,
        CommandBINPOS43 = 743,
        CommandBINPOS44 = 744,
        CommandBINPOS45 = 745,
        CommandBINPOS46 = 746,
        CommandBINPOS47 = 747,
        CommandBINPOS48 = 748,
        CommandBINPOS49 = 749,
        CommandBINPOS50 = 750,
        CommandBINPOS51 = 751,
        CommandBINPOS52 = 752,
        CommandBINPOS53 = 753,
        CommandBINPOS54 = 754,
        CommandBINPOS55 = 755,
        CommandBINPOS56 = 756,
        CommandBINPOS57 = 757,
        CommandBINPOS58 = 758,
        CommandBINPOS59 = 759,
        CommandBINPOS60 = 760,
        CommandBINPOS61 = 761,
        CommandBINPOS62 = 762,
        CommandBINPOS63 = 763,
        CommandBINEDITEND = 763
    };
}
