// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <WexTestClass.h>

namespace CalculatorUnitTests
{
    BEGIN_MODULE()
        MODULE_PROPERTY(L"APPX:CertificateFileName", L"CalculatorUnitTests.cer:TrustedPeople")
    END_MODULE()

    MODULE_SETUP(ModuleSetup)
    {
        return true;
    }

    MODULE_CLEANUP(ModuleCleanup)
    {
        return true;
    }
}
