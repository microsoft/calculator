// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

#include "CalcViewModel/Common/EngineResourceProvider.h"

using namespace std;
using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculationManager;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static constexpr size_t MAX_HISTORY_SIZE = 20;

namespace CalculatorEngineTests
{
    TEST_CLASS(CalcEngineTests)
    {
        TEST_METHOD_INITIALIZE(CommonSetup)
        {
            m_resourceProvider = make_shared<EngineResourceProvider>();
            m_history = make_shared<CalculatorHistory>(MAX_HISTORY_SIZE);
            CCalcEngine::InitialOneTimeOnlySetup(*(m_resourceProvider.get()));
            m_calcEngine = make_unique<CCalcEngine>(
                false /* Respect Order of Operations */, false /* Set to Integer Mode */, m_resourceProvider.get(), nullptr, m_history);
        }
        TEST_METHOD_CLEANUP(Cleanup)
        {
            m_resourceProvider = nullptr;
            m_history = nullptr;
            m_calcEngine = nullptr;
        }

        TEST_METHOD(TestGroupDigitsPerRadix)
        {
            // Empty/Error cases
            VERIFY_IS_TRUE(m_calcEngine->GroupDigitsPerRadix(L"", 10).empty(), L"Verify grouping empty string returns empty string.");
            VERIFY_ARE_EQUAL(L"12345678", m_calcEngine->GroupDigitsPerRadix(L"12345678", 9), L"Verify grouping on invalid base returns original string");

            // Octal
            VERIFY_ARE_EQUAL(L"1 234 567", m_calcEngine->GroupDigitsPerRadix(L"1234567", 8), L"Verify grouping in octal.");
            VERIFY_ARE_EQUAL(L"123", m_calcEngine->GroupDigitsPerRadix(L"123", 8), L"Verify minimum grouping in octal.");

            // Binary/Hexadecimal
            VERIFY_ARE_EQUAL(L"12 3456 7890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 2), L"Verify grouping in binary.");
            VERIFY_ARE_EQUAL(L"1234", m_calcEngine->GroupDigitsPerRadix(L"1234", 2), L"Verify minimum grouping in binary.");
            VERIFY_ARE_EQUAL(L"12 3456 7890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 16), L"Verify grouping in hexadecimal.");
            VERIFY_ARE_EQUAL(L"1234", m_calcEngine->GroupDigitsPerRadix(L"1234", 16), L"Verify minimum grouping in hexadecimal.");

            // Decimal
            VERIFY_ARE_EQUAL(L"1,234,567,890", m_calcEngine->GroupDigitsPerRadix(L"1234567890", 10), L"Verify grouping in base10.");
            VERIFY_ARE_EQUAL(L"1,234,567.89", m_calcEngine->GroupDigitsPerRadix(L"1234567.89", 10), L"Verify grouping in base10 with decimal.");
            VERIFY_ARE_EQUAL(L"1,234,567e89", m_calcEngine->GroupDigitsPerRadix(L"1234567e89", 10), L"Verify grouping in base10 with exponent.");
            VERIFY_ARE_EQUAL(
                L"1,234,567.89e5", m_calcEngine->GroupDigitsPerRadix(L"1234567.89e5", 10), L"Verify grouping in base10 with decimal and exponent.");
            VERIFY_ARE_EQUAL(L"-123,456,789", m_calcEngine->GroupDigitsPerRadix(L"-123456789", 10), L"Verify grouping in base10 with negative.");
        }

        TEST_METHOD(TestIsNumberInvalid)
        {
            // Binary Number Checks
            vector<wstring> validBinStrs{ L"0", L"1", L"0011", L"1100" };
            vector<wstring> invalidBinStrs{ L"2", L"A", L"0.1" };
            for (wstring const& str : validBinStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }
            for (wstring const& str : invalidBinStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }

            // Octal Number Checks
            vector<wstring> validOctStrs{ L"0", L"7", L"01234567", L"76543210" };
            vector<wstring> invalidOctStrs{ L"8", L"A", L"0.7" };
            for (wstring const& str : validOctStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }
            for (wstring const& str : invalidOctStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }

            // Hexadecimal Number Checks
            vector<wstring> validHexStrs{ L"0", L"F", L"0123456789ABCDEF", L"FEDCBA9876543210" };
            vector<wstring> invalidHexStrs{ L"G", L"abcdef", L"x", L"0.1" };
            for (wstring const& str : validHexStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* HEx */));
            }
            for (wstring const& str : invalidHexStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* Hex */));
            }

            // Decimal Number Checks

            // Special case errors: long exponent, long mantissa
            wstring longExp(L"1e12345");
            VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(longExp, 5 /* Max exp length */, 100, 10 /* Decimal */));
            VERIFY_ARE_EQUAL(IDS_ERR_INPUT_OVERFLOW, m_calcEngine->IsNumberInvalid(longExp, 4 /* Max exp length */, 100, 10 /* Decimal */));
            // Mantissa length is sum of:
            //  - digits before decimal separator, minus leading zeroes
            //  - digits after decimal separator, including trailing zeroes
            // Each of these mantissa values should calculate as a length of 5
            vector<wstring> longMantStrs{ L"10000", L"10.000", L"0000012345", L"123.45", L"0.00123", L"0.12345", L"-123.45e678" };
            for (wstring const& str : longMantStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 100, 5 /* Max mantissa length */, 10 /* Decimal */));
            }
            for (wstring const& str : longMantStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_INPUT_OVERFLOW, m_calcEngine->IsNumberInvalid(str, 100, 4 /* Max mantissa length */, 10 /* Decimal */));
            }

            // Regex matching (descriptions taken from CalcUtils.cpp)
            // Use 100 for exp/mantissa length as they are tested above
            vector<wstring> validDecStrs{ // Start with an optional + or -
                                          L"+1",
                                          L"-1",
                                          L"1",
                                          // Followed by zero or more digits
                                          L"-",
                                          L"",
                                          L"1234567890",
                                          // Followed by an optional decimal point
                                          L"1.0",
                                          L"-.",
                                          L"1.",
                                          // Followed by zero or more digits
                                          L"0.0",
                                          L"0.123456",
                                          // Followed by an optional exponent ('e')
                                          L"1e",
                                          L"1.e",
                                          L"-e",
                                          // If there's an exponent, its optionally followed by + or -
                                          // and followed by zero or more digits
                                          L"1e+12345",
                                          L"1e-12345",
                                          L"1e123",
                                          // All together
                                          L"-123.456e+789"
            };
            vector<wstring> invalidDecStrs{ L"x123", L"123-", L"1e1.2", L"1-e2" };
            for (wstring const& str : validDecStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
            for (wstring const& str : invalidDecStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
        }

        TEST_METHOD(TestDigitGroupingStringToGroupingVector)
        {
            vector<uint32_t> groupingVector{};
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L""), L"Verify empty grouping");

            groupingVector = { 1 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"1"), L"Verify simple grouping");

            groupingVector = { 3, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"3;0"), L"Verify standard grouping");

            groupingVector = { 3, 0, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"3;0;0"), L"Verify expanded non-repeating grouping");

            groupingVector = { 5, 3, 2, 4, 6 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"5;3;2;4;6"), L"Verify long grouping");

            groupingVector = { 15, 15, 15, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"15;15;15;0"), L"Verify large grouping");

            groupingVector = { 4, 7, 0 };
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(L"4;16;7;25;0"), L"Verify we ignore oversize grouping");

            groupingVector = { 3, 0 };
            constexpr wstring_view nonRepeatingGrouping = L"3;0;0";
            constexpr wstring_view repeatingGrouping = nonRepeatingGrouping.substr(0, 3);
            VERIFY_ARE_EQUAL(groupingVector, CCalcEngine::DigitGroupingStringToGroupingVector(repeatingGrouping), L"Verify we don't go past the end of wstring_view range");
        }

        TEST_METHOD(TestGroupDigits)
        {
            wstring result{ L"1234567" };
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L"", { 3, 0 }, L"1234567", false), L"Verify handling of empty delimiter.");
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", {}, L"1234567", false), L"Verify handling of empty grouping.");

            result = L"1,234,567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"1234567", false), L"Verify standard digit grouping.");

            result = L"1 234 567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L" ", { 3, 0 }, L"1234567", false), L"Verify delimiter change.");

            result = L"1|||234|||567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L"|||", { 3, 0 }, L"1234567", false), L"Verify long delimiter.");

            result = L"12,345e67";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"12345e67", false), L"Verify respect of exponent.");

            result = L"12,345.67";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"12345.67", false), L"Verify respect of decimal.");

            result = L"1,234.56e7";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"1234.56e7", false), L"Verify respect of exponent and decimal.");

            result = L"-1,234,567";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0 }, L"-1234567", true), L"Verify negative number grouping.");

            // Test various groupings
            result = L"1234567890123456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 0, 0 }, L"1234567890123456", false), L"Verify no grouping.");

            result = L"1234567890123,456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3 }, L"1234567890123456", false), L"Verify non-repeating grouping.");
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 3, 0, 0 }, L"1234567890123456", false), L"Verify expanded form non-repeating grouping.");

            result = L"12,34,56,78,901,23456";
            VERIFY_ARE_EQUAL(
                result, m_calcEngine->GroupDigits(L",", { 5, 3, 2, 0 }, L"1234567890123456", false), L"Verify multigroup with repeating grouping.");

            result = L"1234,5678,9012,3456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 4, 0 }, L"1234567890123456", false), L"Verify repeating non-standard grouping.");

            result = L"123456,78,901,23456";
            VERIFY_ARE_EQUAL(result, m_calcEngine->GroupDigits(L",", { 5, 3, 2 }, L"1234567890123456", false), L"Verify multigroup non-repeating grouping.");
            VERIFY_ARE_EQUAL(
                result,
                m_calcEngine->GroupDigits(L",", { 5, 3, 2, 0, 0 }, L"1234567890123456", false),
                L"Verify expanded form multigroup non-repeating grouping.");
        }

    private:
        unique_ptr<CCalcEngine> m_calcEngine;
        shared_ptr<IResourceProvider> m_resourceProvider;
        shared_ptr<CalculatorHistory> m_history;
    };
}
