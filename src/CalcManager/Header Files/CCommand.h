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

#pragma once

// The following are the valid id's which can be passed to CCalcEngine::ProcessCommand

#define IDM_HEX 313
#define IDM_DEC 314
#define IDM_OCT 315
#define IDM_BIN 316
#define IDM_QWORD 317
#define IDM_DWORD 318
#define IDM_WORD 319
#define IDM_BYTE 320
#define IDM_DEG 321
#define IDM_RAD 322
#define IDM_GRAD 323
#define IDM_DEGREES 324
#define IDC_ENG 326

#define IDC_HEX IDM_HEX
#define IDC_DEC IDM_DEC
#define IDC_OCT IDM_OCT
#define IDC_BIN IDM_BIN

#define IDC_DEG IDM_DEG
#define IDC_RAD IDM_RAD
#define IDC_GRAD IDM_GRAD
#define IDC_DEGREES IDM_DEGREES

#define IDC_QWORD IDM_QWORD
#define IDC_DWORD IDM_DWORD
#define IDC_WORD IDM_WORD
#define IDC_BYTE IDM_BYTE

// Key IDs:
// These id's must be consecutive from IDC_FIRSTCONTROL to IDC_LASTCONTROL.
// The actual values don't matter but the order and sequence are very important.
// Also, the order of the controls must match the order of the control names
// in the string table.
// For example you want to declare the color for the control IDC_ST_AVE
// Find the string id for that control from the rc file
// Now define the control's id as IDC_FRISTCONTROL+stringID(IDC_ST_AVE)
#define IDC_FIRSTCONTROL IDC_SIGN
#define IDC_SIGN 80
#define IDC_CLEAR 81
#define IDC_CENTR 82
#define IDC_BACK 83

#define IDC_PNT 84

// Hole  85

#define IDC_AND 86 // Binary operators must be between IDC_AND and IDC_PWR
#define IDC_OR 87
#define IDC_XOR 88
#define IDC_LSHF 89
#define IDC_RSHF 90
#define IDC_DIV 91
#define IDC_MUL 92
#define IDC_ADD 93
#define IDC_SUB 94
#define IDC_MOD 95
#define IDC_ROOT 96
#define IDC_PWR 97

#define IDC_UNARYFIRST IDC_CHOP
#define IDC_CHOP 98 // Unary operators must be between IDC_CHOP and IDC_EQU
#define IDC_ROL 99
#define IDC_ROR 100
#define IDC_COM 101

#define IDC_SIN 102
#define IDC_COS 103
#define IDC_TAN 104

#define IDC_SINH 105
#define IDC_COSH 106
#define IDC_TANH 107

#define IDC_LN 108
#define IDC_LOG 109
#define IDC_SQRT 110
#define IDC_SQR 111
#define IDC_CUB 112
#define IDC_FAC 113
#define IDC_REC 114
#define IDC_DMS 115
#define IDC_CUBEROOT 116 // x ^ 1/3
#define IDC_POW10 117    // 10 ^ x
#define IDC_PERCENT 118
#define IDC_UNARYLAST IDC_PERCENT

#define IDC_FE 119
#define IDC_PI 120
#define IDC_EQU 121

#define IDC_MCLEAR 122
#define IDC_RECALL 123
#define IDC_STORE 124
#define IDC_MPLUS 125
#define IDC_MMINUS 126

#define IDC_EXP 127

#define IDC_OPENP 128
#define IDC_CLOSEP 129

#define IDC_0 130 // The controls for 0 through F must be consecutive and in order
#define IDC_1 131
#define IDC_2 132
#define IDC_3 133
#define IDC_4 134
#define IDC_5 135
#define IDC_6 136
#define IDC_7 137
#define IDC_8 138
#define IDC_9 139
#define IDC_A 140
#define IDC_B 141
#define IDC_C 142
#define IDC_D 143
#define IDC_E 144
#define IDC_F 145 // this is last control ID which must match the string table
#define IDC_INV 146
#define IDC_SET_RESULT 147

#define IDC_STRING_MAPPED_VALUES 400
#define IDC_UNARYEXTENDEDFIRST IDC_STRING_MAPPED_VALUES
#define IDC_SEC 400 // Secant
// 401 reserved for inverse
#define IDC_CSC 402 // Cosecant
// 403 reserved for inverse
#define IDC_COT 404 // Cotangent
// 405 reserved for inverse

#define IDC_SECH 406 // Hyperbolic Secant
// 407 reserved for inverse
#define IDC_CSCH 408 // Hyperbolic Cosecant
// 409 reserved for inverse
#define IDC_COTH 410 // Hyperbolic Cotangent
// 411 reserved for inverse

#define IDC_POW2 412  // 2 ^ x
#define IDC_ABS 413   // Absolute Value
#define IDC_FLOOR 414 // Floor
#define IDC_CEIL 415  // Ceiling

#define IDC_ROLC 416 // Rotate Left Circular
#define IDC_RORC 417 // Rotate Right Circular

#define IDC_UNARYEXTENDEDLAST IDC_RORC

#define IDC_LASTCONTROL IDC_CEIL

#define IDC_BINARYEXTENDEDFIRST 500
#define IDC_LOGBASEY 500 // logy(x)
#define IDC_NAND 501     // Nand
#define IDC_NOR 502      // Nor

#define IDC_RSHFL 505 // Right Shift Logical
#define IDC_BINARYEXTENDEDLAST IDC_RSHFL

#define IDC_RAND 600  // Random
#define IDC_EULER 601 // e Constant

#define IDC_BINEDITSTART 700
#define IDC_BINPOS0 700
#define IDC_BINPOS1 701
#define IDC_BINPOS2 702
#define IDC_BINPOS3 703
#define IDC_BINPOS4 704
#define IDC_BINPOS5 705
#define IDC_BINPOS6 706
#define IDC_BINPOS7 707
#define IDC_BINPOS8 708
#define IDC_BINPOS9 709
#define IDC_BINPOS10 710
#define IDC_BINPOS11 711
#define IDC_BINPOS12 712
#define IDC_BINPOS13 713
#define IDC_BINPOS14 714
#define IDC_BINPOS15 715
#define IDC_BINPOS16 716
#define IDC_BINPOS17 717
#define IDC_BINPOS18 718
#define IDC_BINPOS19 719
#define IDC_BINPOS20 720
#define IDC_BINPOS21 721
#define IDC_BINPOS22 722
#define IDC_BINPOS23 723
#define IDC_BINPOS24 724
#define IDC_BINPOS25 725
#define IDC_BINPOS26 726
#define IDC_BINPOS27 727
#define IDC_BINPOS28 728
#define IDC_BINPOS29 729
#define IDC_BINPOS30 730
#define IDC_BINPOS31 731
#define IDC_BINPOS32 732
#define IDC_BINPOS33 733
#define IDC_BINPOS34 734
#define IDC_BINPOS35 735
#define IDC_BINPOS36 736
#define IDC_BINPOS37 737
#define IDC_BINPOS38 738
#define IDC_BINPOS39 739
#define IDC_BINPOS40 740
#define IDC_BINPOS41 741
#define IDC_BINPOS42 742
#define IDC_BINPOS43 743
#define IDC_BINPOS44 744
#define IDC_BINPOS45 745
#define IDC_BINPOS46 746
#define IDC_BINPOS47 747
#define IDC_BINPOS48 748
#define IDC_BINPOS49 749
#define IDC_BINPOS50 750
#define IDC_BINPOS51 751
#define IDC_BINPOS52 752
#define IDC_BINPOS53 753
#define IDC_BINPOS54 754
#define IDC_BINPOS55 755
#define IDC_BINPOS56 756
#define IDC_BINPOS57 757
#define IDC_BINPOS58 758
#define IDC_BINPOS59 759
#define IDC_BINPOS60 760
#define IDC_BINPOS61 761
#define IDC_BINPOS62 762
#define IDC_BINPOS63 763
#define IDC_BINEDITEND 763

// The strings in the following range IDS_ENGINESTR_FIRST ... IDS_ENGINESTR_MAX are strings allocated in the
// resource for the purpose internal to Engine and cant be used by the clients
#define IDS_ENGINESTR_FIRST 0
#define IDS_ENGINESTR_MAX 200
