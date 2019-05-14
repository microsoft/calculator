// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/****************************Module*Header***********************************
* Module Name: CCommand.h
*
* Module Description:
*       Resource ID's for the Engine Commands exposed.
*
* Warnings:
*
* Created: 13-Feb-2008
*
\****************************************************************************/

// The following are the valid id's which can be passed to CCalcEngine::ProcessCommand

namespace CalculationManager
{

    public static class CCommand
    {
        public const int IDM_HEX = 313;
        public const int IDM_DEC = 314;
        public const int IDM_OCT = 315;
        public const int IDM_BIN = 316;
        public const int IDM_QWORD = 317;
        public const int IDM_DWORD = 318;
        public const int IDM_WORD = 319;
        public const int IDM_BYTE = 320;
        public const int IDM_DEG = 321;
        public const int IDM_RAD = 322;
        public const int IDM_GRAD = 323;
        public const int IDM_DEGREES = 324;

        public const int IDC_HEX = IDM_HEX;
        public const int IDC_DEC = IDM_DEC;
        public const int IDC_OCT = IDM_OCT;
        public const int IDC_BIN = IDM_BIN;

        public const int IDC_DEG = IDM_DEG;
        public const int IDC_RAD = IDM_RAD;
        public const int IDC_GRAD = IDM_GRAD;
        public const int IDC_DEGREES = IDM_DEGREES;

        public const int IDC_QWORD = IDM_QWORD;
        public const int IDC_DWORD = IDM_DWORD;
        public const int IDC_WORD = IDM_WORD;
        public const int IDC_BYTE = IDM_BYTE;

        // Key IDs:
        // These id's must be consecutive from IDC_FIRSTCONTROL to IDC_LASTCONTROL.
        // The actual values don't matter but the order and sequence are very important.
        // Also, the order of the controls must match the order of the control names
        // in the string table.
        // For example you want to declare the color for the control IDC_ST_AVE
        // Find the string id for that control from the rc file
        // Now define the control's id as IDC_FRISTCONTROL+stringID(IDC_ST_AVE)
        public const int IDC_FIRSTCONTROL = IDC_SIGN;
        public const int IDC_SIGN = 80;
        public const int IDC_CLEAR = 81;
        public const int IDC_CENTR = 82;
        public const int IDC_BACK = 83;

        public const int IDC_PNT = 84;

        // Hole  85

        public const int IDC_AND = 86; // Binary operators must be between IDC_AND and IDC_PWR;
        public const int IDC_OR = 87;
        public const int IDC_XOR = 88;
        public const int IDC_LSHF = 89;
        public const int IDC_RSHF = 90;
        public const int IDC_DIV = 91;
        public const int IDC_MUL = 92;
        public const int IDC_ADD = 93;
        public const int IDC_SUB = 94;
        public const int IDC_MOD = 95;
        public const int IDC_ROOT = 96;
        public const int IDC_PWR = 97;

        public const int IDC_UNARYFIRST = IDC_CHOP;
        public const int IDC_CHOP = 98; // Unary operators must be between IDC_CHOP and IDC_EQU;
        public const int IDC_ROL = 99;
        public const int IDC_ROR = 100;
        public const int IDC_COM = 101;
        public const int IDC_SIN = 102;
        public const int IDC_COS = 103;
        public const int IDC_TAN = 104;

        public const int IDC_SINH = 105;
        public const int IDC_COSH = 106;
        public const int IDC_TANH = 107;

        public const int IDC_LN = 108;
        public const int IDC_LOG = 109;
        public const int IDC_SQRT = 110;
        public const int IDC_SQR = 111;
        public const int IDC_CUB = 112;
        public const int IDC_FAC = 113;
        public const int IDC_REC = 114;
        public const int IDC_DMS = 115;
        public const int IDC_CUBEROOT = 116; // x ^ 1/3;
        public const int IDC_POW10 = 117;    // 10 ^ x;
        public const int IDC_PERCENT = 118;
        public const int IDC_UNARYLAST = IDC_PERCENT;

        public const int IDC_FE = 119;
        public const int IDC_PI = 120;
        public const int IDC_EQU = 121;

        public const int IDC_MCLEAR = 122;
        public const int IDC_RECALL = 123;
        public const int IDC_STORE = 124;
        public const int IDC_MPLUS = 125;
        public const int IDC_MMINUS = 126;

        public const int IDC_EXP = 127;

        public const int IDC_OPENP = 128;
        public const int IDC_CLOSEP = 129;

        public const int IDC_0 = 130; // The controls for 0 through F must be consecutive and in order;
        public const int IDC_1 = 131;
        public const int IDC_2 = 132;
        public const int IDC_3 = 133;
        public const int IDC_4 = 134;
        public const int IDC_5 = 135;
        public const int IDC_6 = 136;
        public const int IDC_7 = 137;
        public const int IDC_8 = 138;
        public const int IDC_9 = 139;
        public const int IDC_A = 140;
        public const int IDC_B = 141;
        public const int IDC_C = 142;
        public const int IDC_D = 143;
        public const int IDC_E = 144;
        public const int IDC_F = 145; // this is last control ID which must match the string table;
        public const int IDC_INV = 146;
        public const int IDC_SET_RESULT = 147;

        public const int IDC_LASTCONTROL = IDC_SET_RESULT;

        public const int IDC_BINEDITSTART = 700;
        public const int IDC_BINPOS0 = 700;
        public const int IDC_BINPOS1 = 701;
        public const int IDC_BINPOS2 = 702;
        public const int IDC_BINPOS3 = 703;
        public const int IDC_BINPOS4 = 704;
        public const int IDC_BINPOS5 = 705;
        public const int IDC_BINPOS6 = 706;
        public const int IDC_BINPOS7 = 707;
        public const int IDC_BINPOS8 = 708;
        public const int IDC_BINPOS9 = 709;
        public const int IDC_BINPOS10 = 710;
        public const int IDC_BINPOS11 = 711;
        public const int IDC_BINPOS12 = 712;
        public const int IDC_BINPOS13 = 713;
        public const int IDC_BINPOS14 = 714;
        public const int IDC_BINPOS15 = 715;
        public const int IDC_BINPOS16 = 716;
        public const int IDC_BINPOS17 = 717;
        public const int IDC_BINPOS18 = 718;
        public const int IDC_BINPOS19 = 719;
        public const int IDC_BINPOS20 = 720;
        public const int IDC_BINPOS21 = 721;
        public const int IDC_BINPOS22 = 722;
        public const int IDC_BINPOS23 = 723;
        public const int IDC_BINPOS24 = 724;
        public const int IDC_BINPOS25 = 725;
        public const int IDC_BINPOS26 = 726;
        public const int IDC_BINPOS27 = 727;
        public const int IDC_BINPOS28 = 728;
        public const int IDC_BINPOS29 = 729;
        public const int IDC_BINPOS30 = 730;
        public const int IDC_BINPOS31 = 731;
        public const int IDC_BINPOS32 = 732;
        public const int IDC_BINPOS33 = 733;
        public const int IDC_BINPOS34 = 734;
        public const int IDC_BINPOS35 = 735;
        public const int IDC_BINPOS36 = 736;
        public const int IDC_BINPOS37 = 737;
        public const int IDC_BINPOS38 = 738;
        public const int IDC_BINPOS39 = 739;
        public const int IDC_BINPOS40 = 740;
        public const int IDC_BINPOS41 = 741;
        public const int IDC_BINPOS42 = 742;
        public const int IDC_BINPOS43 = 743;
        public const int IDC_BINPOS44 = 744;
        public const int IDC_BINPOS45 = 745;
        public const int IDC_BINPOS46 = 746;
        public const int IDC_BINPOS47 = 747;
        public const int IDC_BINPOS48 = 748;
        public const int IDC_BINPOS49 = 749;
        public const int IDC_BINPOS50 = 750;
        public const int IDC_BINPOS51 = 751;
        public const int IDC_BINPOS52 = 752;
        public const int IDC_BINPOS53 = 753;
        public const int IDC_BINPOS54 = 754;
        public const int IDC_BINPOS55 = 755;
        public const int IDC_BINPOS56 = 756;
        public const int IDC_BINPOS57 = 757;
        public const int IDC_BINPOS58 = 758;
        public const int IDC_BINPOS59 = 759;
        public const int IDC_BINPOS60 = 760;
        public const int IDC_BINPOS61 = 761;
        public const int IDC_BINPOS62 = 762;
        public const int IDC_BINPOS63 = 763;
        public const int IDC_BINEDITEND = 763;

        // The strings in the following range IDS_ENGINESTR_FIRST ... IDS_ENGINESTR_MAX are strings allocated in the
        // resource for the purpose internal to Engine and cant be used by the clients
        public const int IDS_ENGINESTR_FIRST = 0;
        public const int IDS_ENGINESTR_MAX = 200;
    }
}