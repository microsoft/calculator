// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

using namespace std;
using namespace CalculationManager;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorEngineTests
{
    TEST_CLASS(CalcInputTest)
    {
        TEST_METHOD_INITIALIZE(CommonSetup)
        {
            m_calcInput = CalcEngine::CalcInput(L'.');
        }
        TEST_METHOD_CLEANUP(Cleanup)
        {
            m_calcInput.Clear();
            m_calcInput.SetDecimalSymbol(L'.');
        }

        TEST_METHOD(Clear)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryToggleSign(false, L"999");
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);

            VERIFY_ARE_EQUAL(L"-1.2e+3", m_calcInput.ToString(10), L"Verify input is correct.");

            m_calcInput.Clear();

            ::Logger::WriteMessage(m_calcInput.ToString(10).c_str());
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input is 0 after clear.");
        }

        TEST_METHOD(TryToggleSignZero)
        {
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling 0 succeeds.");
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify toggling 0 does not create -0.");
        }
        TEST_METHOD(TryToggleSignExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling exponent sign succeeds.");
            VERIFY_ARE_EQUAL(L"1.e-2", m_calcInput.ToString(10), L"Verify toggling exponent sign does not toggle base sign.");
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling exponent sign succeeds.");
            VERIFY_ARE_EQUAL(L"1.e+2", m_calcInput.ToString(10), L"Verify toggling negative exponent sign does not toggle base sign.");
        }
        TEST_METHOD(TryToggleSignBase)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling base sign succeeds.");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify toggling base sign creates negative base.");
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(false, L"999"), L"Verify toggling base sign succeeds.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify toggling negative base sign creates positive base.");
        }
        TEST_METHOD(TryToggleSignBaseIntegerMode)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(true, L"999"), L"Verify toggling base sign in integer mode succeeds.");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify toggling base sign creates negative base.");
        }
        TEST_METHOD(TryToggleSignRollover)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryToggleSign(true, L"127"), L"Verify toggling base sign in integer mode succeeds.");
            m_calcInput.TryAddDigit(8, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryToggleSign(true, L"127"), L"Verify toggling base sign in integer mode fails on rollover.");
            VERIFY_ARE_EQUAL(L"-128", m_calcInput.ToString(10), L"Verify toggling base sign on rollover does not change value.");
        }

        TEST_METHOD(TryAddDigitLeadingZeroes)
        {
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32), L"Verify TryAddDigit succeeds.");
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify leading zeros are ignored.");
        }
        TEST_METHOD(TryAddDigitMaxCount)
        {
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for base with length < maxDigits succeeds.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify adding digit for base with length < maxDigits succeeded.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 1), L"Verify TryAddDigit for base with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify digit for base was not added.");
            m_calcInput.TryBeginExponent();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(4, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(5, 10, false, L"999", 64, 32), L"Verify TryAddDigit for exponent with length > maxDigits fails.");
            VERIFY_ARE_EQUAL(L"1.e+1234", m_calcInput.ToString(10), L"Verify adding digits for exponent with length < maxDigits succeeded.");

            m_calcInput.Clear();
            m_calcInput.TryAddDecimalPt();
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 1), L"Verify decimal point and leading zero does not count toward maxDigits.");
            VERIFY_ARE_EQUAL(L"0.1", m_calcInput.ToString(10), L"Verify input value checking dec pt and leading zero impact on maxDigits.");
        }
        TEST_METHOD(TryAddDigitValues)
        {
            // Use an arbitrary value > 16 to test that input accepts digits > hexadecimal 0xF.
            // TryAddDigit does not validate whether the digit fits within the current radix.
            for (unsigned int i = 0; i < 25; i++)
            {
                VERIFY_IS_TRUE(m_calcInput.TryAddDigit(i, 10, false, L"999", 64, 32), (L"Verify TryAddDigit succeeds for " + to_wstring(i)).c_str());
                m_calcInput.Clear();
            }
        }
        TEST_METHOD(TryAddDigitRolloverBaseCheck)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 16, true, L"999", 64, 1), L"Verify TryAddDigit rollover fails for bases other than 8,10.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(1, 2, true, L"999", 64, 1), L"Verify TryAddDigit rollover fails for bases other than 8,10.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalByte)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 8, 1), L"Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 8, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalWord)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 16, 1), L"Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 16, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalDword)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 32, 1), L"Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 32, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }
        TEST_METHOD(TryAddDigitRolloverOctalQword)
        {
            m_calcInput.TryAddDigit(1, 8, true, L"777", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(2, 8, true, L"377", 64, 1), L"Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, L"777", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 8, true, L"377", 64, 1), L"Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }
        TEST_METHOD(TryAddDigitRolloverDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(0, 10, true, L"1", 8, 1), L"Verify we cannot add a digit if input size matches maxStr size.");
            m_calcInput.TryAddDigit(2, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(2, 10, true, L"110", 8, 2), L"Verify we cannot add a digit if n char comparison > 0.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(7, 10, true, L"130", 8, 2), L"Verify we can add a digit if n char comparison < 0.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(1, 10, true, L"127", 64, 32);
            m_calcInput.TryAddDigit(2, 10, true, L"127", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(8, 10, true, L"127", 8, 2), L"Verify we cannot add a digit if digit exceeds max value.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDigit(7, 10, true, L"127", 8, 2), L"Verify we can add a digit if digit does not exceed max value.");

            m_calcInput.Backspace();
            m_calcInput.TryToggleSign(true, L"127");
            VERIFY_IS_FALSE(m_calcInput.TryAddDigit(9, 10, true, L"127", 8, 2), L"Negative value: verify we cannot add a digit if digit exceeds max value.");
            VERIFY_IS_TRUE(
                m_calcInput.TryAddDigit(8, 10, true, L"127", 8, 2), L"Negative value: verify we can add a digit if digit does not exceed max value.");
        }

        TEST_METHOD(TryAddDecimalPtEmpty)
        {
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify input has no decimal point.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal to empty input.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify decimal on empty input.");
        }
        TEST_METHOD(TryAddDecimalPointTwice)
        {
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify input has no decimal point.");
            VERIFY_IS_TRUE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal to empty input.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            VERIFY_IS_FALSE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal point fails if input has decimal point.");
        }
        TEST_METHOD(TryAddDecimalPointExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_IS_FALSE(m_calcInput.TryAddDecimalPt(), L"Verify adding decimal point fails if input has exponent.");
        }

        TEST_METHOD(TryBeginExponentNoExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryBeginExponent(), L"Verify adding exponent succeeds on input without exponent.");
            VERIFY_ARE_EQUAL(L"1.e+0", m_calcInput.ToString(10), L"Verify exponent present.");
        }
        TEST_METHOD(TryBeginExponentWithExponent)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_IS_TRUE(m_calcInput.TryBeginExponent(), L"Verify adding exponent succeeds on input without exponent.");
            VERIFY_IS_FALSE(m_calcInput.TryBeginExponent(), L"Verify cannot add exponent if input already has exponent.");
        }

        TEST_METHOD(BackspaceZero)
        {
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify backspace on 0 is still 0.");
        }
        TEST_METHOD(BackspaceSingleChar)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
        TEST_METHOD(BackspaceMultiChar)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"12", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
        TEST_METHOD(BackspaceDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"1.", m_calcInput.ToString(10), L"Verify input before backspace.");
            VERIFY_IS_TRUE(m_calcInput.HasDecimalPt(), L"Verify input has decimal point.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify input after backspace.");
            VERIFY_IS_FALSE(m_calcInput.HasDecimalPt(), L"Verify decimal point was removed.");
        }
        TEST_METHOD(BackspaceMultiCharDecimal)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1.23", m_calcInput.ToString(10), L"Verify input before backspace.");
            m_calcInput.Backspace();
            VERIFY_ARE_EQUAL(L"1.2", m_calcInput.ToString(10), L"Verify input after backspace.");
        }
        // Issue #817: Prefixed multiple zeros
        TEST_METHOD(BackspaceZeroDecimalWithoutPrefixZeros)
        {
            m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify input before backspace.")
            m_calcInput.Backspace();
            m_calcInput.TryAddDigit(0, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify input after backspace.")
        }

        TEST_METHOD(SetDecimalSymbol)
        {
            m_calcInput.TryAddDecimalPt();
            VERIFY_ARE_EQUAL(L"0.", m_calcInput.ToString(10), L"Verify default decimal point.");
            m_calcInput.SetDecimalSymbol(L',');
            VERIFY_ARE_EQUAL(L"0,", m_calcInput.ToString(10), L"Verify new decimal point.");
        }

        TEST_METHOD(ToStringEmpty)
        {
            VERIFY_ARE_EQUAL(L"0", m_calcInput.ToString(10), L"Verify ToString of empty value.");
        }
        TEST_METHOD(ToStringNegative)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryToggleSign(false, L"999");
            VERIFY_ARE_EQUAL(L"-1", m_calcInput.ToString(10), L"Verify ToString of negative value.");
        }
        TEST_METHOD(ToStringExponentBase10)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            VERIFY_ARE_EQUAL(L"1.e+0", m_calcInput.ToString(10), L"Verify ToString of empty base10 exponent.");
        }
        TEST_METHOD(ToStringExponentBase8)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            VERIFY_ARE_EQUAL(L"1.^+0", m_calcInput.ToString(8), L"Verify ToString of empty base8 exponent.");
        }
        TEST_METHOD(ToStringExponentNegative)
        {
            m_calcInput.TryAddDigit(1, 8, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryToggleSign(false, L"999");
            VERIFY_ARE_EQUAL(L"1.e-0", m_calcInput.ToString(10), L"Verify ToString of empty negative exponent.");
        }
        TEST_METHOD(ToStringExponentPositive)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(4, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1.e+234", m_calcInput.ToString(10), L"Verify ToString of exponent with value.");
        }
        TEST_METHOD(ToStringInteger)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"1", m_calcInput.ToString(10), L"Verify ToString of integer value hides decimal.");
        }
        TEST_METHOD(ToStringBaseTooLong)
        {
            wstring maxStr{};
            for (size_t i = 0; i < MAX_STRLEN + 1; i++)
            {
                maxStr += L'1';
                m_calcInput.TryAddDigit(1, 10, false, maxStr, 64, 100);
            }
            auto result = m_calcInput.ToString(10);
            VERIFY_IS_TRUE(result.empty(), L"Verify ToString of base value that is too large yields empty string.");
        }
        TEST_METHOD(ToStringExponentTooLong)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryBeginExponent();
            wstring maxStr{ L"11" };
            bool exponentCapped = false;
            for (size_t i = 0; i < MAX_STRLEN + 1; i++)
            {
                maxStr += L'1';
                if (!m_calcInput.TryAddDigit(1, 10, false, maxStr, 64, MAX_STRLEN + 25))
                {
                    exponentCapped = true;
                }
            }
            auto result = m_calcInput.ToString(10);

            // TryAddDigit caps the exponent length to C_EXP_MAX_DIGITS = 4, so ToString() succeeds.
            // If that cap is removed, ToString() should return an empty string.
            if (exponentCapped)
            {
                VERIFY_ARE_EQUAL(L"1.e+1111", result, L"Verify ToString succeeds; exponent length is capped at C_EXP_MAX_DIGITS.");
            }
            else
            {
                VERIFY_IS_TRUE(result.empty(), L"Verify ToString of exponent value that is too large yields empty string.");
            }
        }

        TEST_METHOD(ToRational)
        {
            m_calcInput.TryAddDigit(1, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, L"999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, L"999", 64, 32);
            VERIFY_ARE_EQUAL(L"123", m_calcInput.ToString(10), L"Verify input before conversion to rational.");

            auto rat = m_calcInput.ToRational(10, false);
            VERIFY_ARE_EQUAL(1, rat.P().Mantissa().size(), L"Verify digit count of rational.");
            VERIFY_ARE_EQUAL(123, rat.P().Mantissa().front(), L"Verify first digit of mantissa.");
        }

    private:
        CalcEngine::CalcInput m_calcInput;
    };
}
