
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

constexpr size_t MAX_HISTORY_SIZE = 20;

namespace CalculatorEngineTests
{
    TEST_CLASS(CalcEngineTests)
    {
        TEST_METHOD_INITIALIZE(CommonSetup) noexcept
        {
            m_resourceProvider = make_shared<EngineResourceProvider>();
            m_history = make_shared<CalculatorHistory>(MAX_HISTORY_SIZE);
            CCalcEngine::InitialOneTimeOnlySetup(*m_resourceProvider);
            m_calcEngine = make_unique<CCalcEngine>(
                false /* Respect Order of Operations */,
                false /* Set to Integer Mode */,
                m_resourceProvider.get(),
                nullptr,
                m_history);
        }

        TEST_METHOD_CLEANUP(Cleanup) noexcept
        {
            m_resourceProvider.reset();
            m_history.reset();
            m_calcEngine.reset();
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
            VERIFY_ARE_EQUAL(L"1,234,567.89e5", m_calcEngine->GroupDigitsPerRadix(L"1234567.89e5", 10), L"Verify grouping in base10 with decimal and exponent.");
            VERIFY_ARE_EQUAL(L"-123,456,789", m_calcEngine->GroupDigitsPerRadix(L"-123456789", 10), L"Verify grouping in base10 with negative.");
        }

        TEST_METHOD(TestIsNumberInvalid)
        {
            // Binary Number Checks
            const vector<wstring> validBinStrs{ L"0", L"1", L"0011", L"1100" };
            const vector<wstring> invalidBinStrs{ L"2", L"A", L"0.1" };

            for (const auto& str : validBinStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }
            for (const auto& str : invalidBinStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }

            // Octal Number Checks
            const vector<wstring> validOctStrs{ L"0", L"7", L"01234567", L"76543210" };
            const vector<wstring> invalidOctStrs{ L"8", L"A", L"0.7" };

            for (const auto& str : validOctStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }
            for (const auto& str : invalidOctStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }

            // Hexadecimal Number Checks
            const vector<wstring> validHexStrs{ L"0", L"F", L"0123456789ABCDEF", L"FEDCBA9876543210" };
            const vector<wstring> invalidHexStrs{ L"G", L"abcdef", L"x", L"0.1" };

            for (const auto& str : validHexStrs)
            {
                VERIFY_ARE_EQUAL(0, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* Hex */));
            }
            for (const auto& str : invalidHexStrs)
            {
                VERIFY_ARE_EQUAL(IDS_ERR_UNK_CH, m_calcEngine->IsNumberInvalid(str, 0, 0, 16 /* Hex */));
            }
        }

    private:
        unique_ptr<CCalcEngine> m_calcEngine;
        shared_ptr<IResourceProvider> m_resourceProvider;
        shared_ptr<CalculatorHistory> m_history;
    };
}
