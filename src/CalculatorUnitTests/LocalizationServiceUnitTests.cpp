// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>

#include "CalcViewModel/Common/LocalizationService.h"

using namespace CalculatorApp::Common;
using namespace Platform;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    TEST_CLASS(LocalizationServiceUnitTests){ public:

                                                  TEST_METHOD(TestSortStrings){ auto localizationService = LocalizationService::GetInstance();
    vector<String ^> stringsToSort = {
        L"Zebra", L"Alpha", L"beta1", L"Gamma", L"Beta", L"alpha1", L"États-Unis", L"Epsilon", L"Etude",
    };

    vector<String ^> expectedResult = {
        L"Alpha", L"alpha1", L"Beta", L"beta1", L"Epsilon", L"États-Unis", L"Etude", L"Gamma", L"Zebra",
    };
    VERIFY_ARE_EQUAL(stringsToSort.size(), expectedResult.size());
    VERIFY_IS_FALSE(equal(stringsToSort.begin(), stringsToSort.end(), expectedResult.begin()));
    localizationService->Sort(stringsToSort);
    VERIFY_IS_TRUE(equal(stringsToSort.begin(), stringsToSort.end(), expectedResult.begin()));
}

TEST_METHOD(TestSortEmptyStrings)
{
    // Verify if LocalizationService::Sort doesn't crash when the vector is empty or null
    auto localizationService = LocalizationService::GetInstance();
    vector<String ^> stringsToSort = {};
    localizationService->Sort(stringsToSort);
    stringsToSort = { L"" };
    localizationService->Sort(stringsToSort);
    stringsToSort = { L"", L"", L"" };
    localizationService->Sort(stringsToSort);
    stringsToSort = { nullptr, L"", L"" };
    localizationService->Sort(stringsToSort);
}

TEST_METHOD(TestSortGeneric)
{
    vector<String ^> stringsToSort = {
        L"fermentum",    L"fringilla", L"Curabitur", L"rhoncus",  L"Aenean",   L"Fusce",
        L"sollicitudin", L"empor",     L"edapibus",  L"édapibas", L"édapîbos", L"édapîbÉs",
    };

    vector<String ^> expectedResult = {
        L"Aenean", L"Curabitur", L"édapibas",  L"édapîbÉs", L"édapîbos", L"edapibus",
        L"empor",  L"fermentum", L"fringilla", L"Fusce",    L"rhoncus",  L"sollicitudin",
    };

    auto sortFunction = [](String ^ s) { return ref new String(L"CAL:") + s + L"TEST"; };

    VERIFY_ARE_EQUAL(stringsToSort.size(), expectedResult.size());
    VERIFY_IS_FALSE(equal(stringsToSort.begin(), stringsToSort.end(), expectedResult.begin()));

    auto localizationService = LocalizationService::GetInstance();
    localizationService->Sort<String ^>(stringsToSort, sortFunction);
    VERIFY_IS_TRUE(equal(stringsToSort.begin(), stringsToSort.end(), expectedResult.begin()));

    vector<String ^> expected2Result = {
        L"édapibas", L"édapîbÉs", L"édapîbos",     L"edapibus",  L"Aenean",    L"fermentum",
        L"rhoncus",  L"empor",    L"sollicitudin", L"fringilla", L"Curabitur", L"Fusce",
    };

    auto sort2Function = [](String ^ s) { return ref new String(s->Begin() + 1); };

    VERIFY_ARE_EQUAL(stringsToSort.size(), expected2Result.size());
    VERIFY_IS_FALSE(equal(stringsToSort.begin(), stringsToSort.end(), expected2Result.begin()));

    localizationService->Sort<String ^>(stringsToSort, sort2Function);
    VERIFY_IS_TRUE(equal(stringsToSort.begin(), stringsToSort.end(), expected2Result.begin()));
}
}
;
}
