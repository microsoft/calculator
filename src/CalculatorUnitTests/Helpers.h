// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#pragma once
#include "CalcViewModel/Common/CalculatorButtonUser.h"

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

#define VERIFY_THROWS_EXPECTEDEXCEPTION(__operation, __exception)                                                                                    \
    {                                                                                                                                                    \
        bool __exceptionHit = false;                                                                                                                     \
        try                                                                                                                                              \
        {                                                                                                                                                \
            __operation;                                                                                                                                 \
        }                                                                                                                                                \
        catch(__exception)                                                                                                                               \
        {                                                                                                                                                \
            __exceptionHit = true;                                                                                                                       \
        }                                                                                                                                                \
                                                                                                                                                         \
        if (!__exceptionHit)                                                                                                                             \
        {                                                                                                                                                \
            Assert::Fail(L"Expected exception was not caught");                                                                                          \
        }                                                                                                                                                \
    }

#define VERIFY_IS_NULL(__operation, ...)                                                                                                             \
    {                                                                                                                                                    \
        Assert::IsTrue(nullptr == __operation, __VA_ARGS__);                                                                                             \
    }                                                                                                                                                    \

#define VERIFY_IS_NOT_NULL(__operation, ...)                                                                                                         \
    {                                                                                                                                                    \
        Assert::IsTrue(nullptr != __operation, __VA_ARGS__);                                                                                             \
    }                                                                                                                                                    \

#define VERIFY_IS_LESS_THAN(__expectedLess, __expectedGreater, ...)                                                                                  \
    {                                                                                                                                                    \
        Assert::IsTrue(__expectedLess < __expectedGreater, __VA_ARGS__);                                                                                 \
    }                                                                                                                                                    \

#define VERIFY_IS_GREATER_THAN(__expectedGreater, __expectedLess, ...)                                                                               \
    {                                                                                                                                                    \
        Assert::IsTrue(__expectedGreater > __expectedLess, __VA_ARGS__);                                                                                 \
    }                                                                                                                                                    \

#define VERIFY_ARE_EQUAL(__f1, __f2, ...)                                                                                                            \
    {                                                                                                                                                    \
        Assert::IsTrue(__f1 == __f2, __VA_ARGS__);                                                                                                       \
    }                                                                                                                                                    \

#define VERIFY_ARE_NOT_EQUAL(__f1, __f2, ...)                                                                                                        \
    {                                                                                                                                                    \
        Assert::IsTrue(__f1 != __f2, __VA_ARGS__);                                                                                                       \
    }                                                                                                                                                    \

#define VERIFY_IS_TRUE(__operation, ...)                                                                                                             \
    {                                                                                                                                                    \
        Assert::IsTrue(__operation, __VA_ARGS__);                                                                                                        \
    }                                                                                                                                                    \

#define VERIFY_IS_FALSE(__operation, ...)                                                                                                            \
    {                                                                                                                                                    \
        Assert::IsFalse(__operation, __VA_ARGS__);                                                                                                       \
    }                                                                                                                                                    \

#define VERIFY_IS_LESS_THAN_OR_EQUAL(__expectedLess, __expectedGreater, ...)                                                                         \
    {                                                                                                                                                    \
        Assert::IsTrue(__expectedLess <= __expectedGreater, __VA_ARGS__);                                                                                \
    }                                                                                                                                                    \

#define VERIFY_IS_GREATER_THAN_OR_EQUAL(__expectedGreater, __expectedLess, ...)                                                                      \
    {                                                                                                                                                    \
        Assert::IsTrue(__expectedGreater >= __expectedLess, __VA_ARGS__);                                                                                \
    }                                                                                                                                                    \

