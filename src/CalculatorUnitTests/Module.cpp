// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    BEGIN_TEST_MODULE_ATTRIBUTE()
    TEST_MODULE_ATTRIBUTE(L"APPX:CertificateFileName", L"CalculatorUnitTests.cer:TrustedPeople")
    END_TEST_MODULE_ATTRIBUTE()

    TEST_MODULE_INITIALIZE(ModuleSetup)
    {
    }

    TEST_MODULE_CLEANUP(ModuleCleanup)
    {
    }
}
