// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <WexTestClass.h>

namespace CalculatorUnitTests
{
    class UtilsTests
    {
    public:
        TEST_CLASS(UtilsTests);

        TEST_METHOD(IsLastCharacterSuccess)
        {
            VERIFY_IS_TRUE(Utils::IsLastCharacterTarget(L"Test.", L'.'));
        }

        TEST_METHOD(IsLastCharacterSuccessMultipleSuffices)
        {
            VERIFY_IS_TRUE(Utils::IsLastCharacterTarget(L"Test..", L'.'));
        }

        TEST_METHOD(IsLastCharacterFailure)
        {
            VERIFY_IS_FALSE(Utils::IsLastCharacterTarget(L"Test", L'.'));
        }

        TEST_METHOD(IsLastCharacterFailureAllButLastMatch)
        {
            VERIFY_IS_FALSE(Utils::IsLastCharacterTarget(L".....T", L'.'));
        }

        TEST_METHOD(IsLastCharacterFailureEmptyInput)
        {
            VERIFY_IS_FALSE(Utils::IsLastCharacterTarget({}, L'.'));
        }

        TEST_METHOD(IsLastCharacterFailureNullTarget)
        {
            VERIFY_IS_FALSE(Utils::IsLastCharacterTarget({}, NULL));
        }
    };
}
