// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#pragma once
#include "CalcViewModel\Common\CalculatorButtonUser.h"

namespace CalculatorUnitTests
{
    #define StandardModePrecision       16
    #define ScientificModePrecision     32
    #define ProgrammerModePrecision     64

    typedef struct item
    {
        CalculatorApp::NumbersAndOperatorsEnum command;
        std::wstring expectedPrimaryDisplay;
        std::wstring expectedExpressions;
    } TESTITEM;

    namespace UtfUtils {
        constexpr wchar_t LRE = 0x202a; // Left-to-Right Embedding
        constexpr wchar_t PDF = 0x202c; // Pop Directional Formatting
        constexpr wchar_t LRO = 0x202d; // Left-to-Right Override
        constexpr wchar_t MUL = 0x00d7; // Multiplication Symbol
    }


}

