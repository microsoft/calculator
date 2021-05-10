// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/Common/CalculatorButtonPressedEventArgs.h"

#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/Common/CalculatorButtonPressedEventArgs.h"
#include "Helpers.h"

using namespace CalculationManager;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Windows::Foundation::Collections;

namespace CalculatorUnitTests
{
    template <typename T>
    void CompareVector(IVector<T> ^ vecA, IVector<T> ^ vecB)
    {
        if (vecA->Size != vecB->Size)
        {
            Assert::Fail();
        }

        for (unsigned int i = 0; i < vecA->Size; ++i)
        {
            VERIFY_ARE_EQUAL(vecA->GetAt(i), vecB->GetAt(i));
        }
    }

    void ChangeMode(StandardCalculatorViewModel ^ viewModel, int mode)
    {
        if (mode == 0)
        {
            viewModel->IsStandard = true;
            viewModel->IsScientific = false;
            viewModel->IsProgrammer = false;
            viewModel->ResetDisplay();
            viewModel->SetPrecision(StandardModePrecision);
        }
        else if (mode == 1)
        {
            viewModel->IsScientific = true;
            viewModel->IsProgrammer = false;
            viewModel->IsStandard = false;
            viewModel->ResetDisplay();
            viewModel->SetPrecision(ScientificModePrecision);
        }
        else if (mode == 2)
        {
            viewModel->IsProgrammer = true;
            viewModel->IsScientific = false;
            viewModel->IsStandard = false;
            viewModel->ResetDisplay();
            viewModel->SetPrecision(ProgrammerModePrecision);
        }
    }

    Object^ CommandParameterFromTestItem(TESTITEM* item)
    {
        // Need to wrap Binary Operators with their feedback
        static const std::vector<std::pair<NumbersAndOperatorsEnum, String^>> binOps = {
            { NumbersAndOperatorsEnum::Add, L"plus" },
            { NumbersAndOperatorsEnum::Subtract, L"minus" },
            { NumbersAndOperatorsEnum::Multiply, L"times" },
            { NumbersAndOperatorsEnum::Divide, L"divided by" }
        };

        for (auto& binOp : binOps)
        {
            if (item->command == binOp.first)
            {
                return ref new CalculatorButtonPressedEventArgs(binOp.second, binOp.first);
            }
        }

        return item->command;
    }

    void ValidateViewModelByCommands(StandardCalculatorViewModel^ viewModel, TESTITEM item[], bool doReset = false)
    {
        if (doReset)
        {
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Clear);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::ClearEntry);
            viewModel->ClearMemoryCommand->Execute(nullptr);
        }

        TESTITEM* currentItem = item;

        while (currentItem->command != NumbersAndOperatorsEnum::None)
        {
            Object ^ commandParam = CommandParameterFromTestItem(currentItem);
            viewModel->ButtonPressed->Execute(commandParam);

            if (currentItem->expectedPrimaryDisplay != L"N/A")
            {
                VERIFY_ARE_EQUAL(Platform::StringReference(currentItem->expectedPrimaryDisplay.c_str()), viewModel->DisplayValue);
            }
            currentItem++;
        }
    }

    TEST_CLASS(StandardViewModelUnitTests)
    {
    public:
        TEST_METHOD_INITIALIZE(InitializeViewModel)
        {
            m_viewModel = ref new StandardCalculatorViewModel();
            m_viewModel->IsStandard = true;
            m_engineResourceProvider = std::make_shared<EngineResourceProvider>();
            m_decimalSeparator = ref new Platform::String(m_engineResourceProvider->GetCEngineString(L"sDecimal").c_str());
        }

        void ValidateViewModelValue(String ^ value)
        {
            String ^ displayValue = m_viewModel->DisplayValue;
            if (value != nullptr)
            {
                VERIFY_ARE_EQUAL(value, displayValue);
            }
        }

        void ValidateViewModelValueAndSecondaryExpression(String ^ value, String ^ expression = nullptr)
        {
            String ^ displayValue = m_viewModel->DisplayValue;

            unsigned int nTokens = m_viewModel->ExpressionTokens->Size;
            String ^ displaySecondaryExpression = ref new String();

            for (unsigned int i = 0; i < nTokens; ++i)
            {
                DisplayExpressionToken ^ currentToken;
                currentToken = m_viewModel->ExpressionTokens->GetAt(i);
                displaySecondaryExpression = String::Concat(displaySecondaryExpression, currentToken->Token);
            }

            if (value != nullptr)
            {
                VERIFY_ARE_EQUAL(value, displayValue);
            }

            if (expression != nullptr)
            {
                VERIFY_ARE_EQUAL(expression, displaySecondaryExpression);
            }
        }

        TEST_METHOD(ViewModelConstructorDisplayValueAndExpressionInitializedTest)
        {
            StandardCalculatorViewModel ^ vmconstructortest = ref new StandardCalculatorViewModel();
            vmconstructortest->IsStandard = true;
            String ^ displayValue = vmconstructortest->DisplayValue;
            String ^ calculationResultAutomationName = vmconstructortest->CalculationResultAutomationName;

            VERIFY_ARE_EQUAL(StringReference(L"0"), displayValue);
            VERIFY_ARE_EQUAL(StringReference(L"Display is 0"), calculationResultAutomationName);
        }

        TEST_METHOD(ViewModelConstructorButtonPressedInitializedTest)
        {
            StandardCalculatorViewModel ^ vmconstructortest = ref new StandardCalculatorViewModel();
            vmconstructortest->IsStandard = true;
            VERIFY_IS_NOT_NULL(vmconstructortest->ButtonPressed);
        }

        /// Expression : 135
        TEST_METHOD(ButtonPressedLeftHandOperandEnteredTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Three, L"13", L"" },
                                 { NumbersAndOperatorsEnum::Five, L"135", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 13.
        TEST_METHOD(ButtonPressedLeftHandOperandAndDecimalEnteredTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Three, L"13", L"" },
                                 { NumbersAndOperatorsEnum::Decimal, L"13" + std::wstring(m_decimalSeparator->Data()), L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 13==
        TEST_METHOD(ButtonPressedLeftHandOperandAndEqualsEnteredTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Three, L"13", L"" },
                                 { NumbersAndOperatorsEnum::Equals, L"13", L"" },
                                 { NumbersAndOperatorsEnum::Equals, L"13", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 13+
        TEST_METHOD(ButtonPressedLeftHandOperandAndOperationEnteredTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Three, L"13", L"" },
                                 { NumbersAndOperatorsEnum::Add, L"13", L"13 + " },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 13+801
        TEST_METHOD(ButtonPressedRightHandOperandEnteredTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },        { NumbersAndOperatorsEnum::Three, L"13", L"" },
                                 { NumbersAndOperatorsEnum::Add, L"13", L"13 + " },  { NumbersAndOperatorsEnum::Eight, L"8", L"13 + " },
                                 { NumbersAndOperatorsEnum::Zero, L"80", L"13 + " }, { NumbersAndOperatorsEnum::One, L"801", L"13 + " },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 1+2=
        TEST_METHOD(ButtonPressedAdditionWithEqualsTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                 { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                 { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 1-2=
        TEST_METHOD(ButtonPressedSubtractionWithEqualsTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Subtract, L"1", L"1 - " },
                                 { NumbersAndOperatorsEnum::Two, L"2", L"1 - " },
                                 { NumbersAndOperatorsEnum::Equals, L"-1", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 3*5=
        TEST_METHOD(ButtonPressedMultiplyWithEqualsTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Three, L"3", L"" },
                                 { NumbersAndOperatorsEnum::Multiply, L"3", L"3 * " },
                                 { NumbersAndOperatorsEnum::Five, L"5", L"3 * " },
                                 { NumbersAndOperatorsEnum::Equals, L"15", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 9/3=
        TEST_METHOD(ButtonPressedDivideTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Nine, L"9", L"" },
                                 { NumbersAndOperatorsEnum::Divide, L"9", L"9 / " },
                                 { NumbersAndOperatorsEnum::Three, L"3", L"9 / " },
                                 { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 7.555*3=
        TEST_METHOD(ButtonPressedDecimalOperationTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Seven, L"7", L"" },
                                 { NumbersAndOperatorsEnum::Decimal, L"7" + std::wstring(m_decimalSeparator->Data()), L"" },
                                 { NumbersAndOperatorsEnum::Five, L"7" + std::wstring(m_decimalSeparator->Data()) + L"5", L"" },
                                 { NumbersAndOperatorsEnum::Five, L"7" + std::wstring(m_decimalSeparator->Data()) + L"55", L"" },
                                 { NumbersAndOperatorsEnum::Five, L"7" + std::wstring(m_decimalSeparator->Data()) + L"555", L"" },
                                 { NumbersAndOperatorsEnum::Multiply,
                                   L"7" + std::wstring(m_decimalSeparator->Data()) + L"555",
                                   L"7" + std::wstring(m_decimalSeparator->Data()) + L"555 * " },
                                 { NumbersAndOperatorsEnum::Three, L"3", L"7" + std::wstring(m_decimalSeparator->Data()) + L"555 * " },
                                 { NumbersAndOperatorsEnum::Equals, L"22" + std::wstring(m_decimalSeparator->Data()) + L"665", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 7/0
        TEST_METHOD(ButtonPressedDivideByZeroNegativeTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Seven, L"7", L"" },
                                 { NumbersAndOperatorsEnum::Divide, L"7", L"7 / " },
                                 { NumbersAndOperatorsEnum::Zero, L"0", L"7 / " },
                                 { NumbersAndOperatorsEnum::Equals, L"Cannot divide by zero", L"7 / " },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 8/2*
        TEST_METHOD(ButtonPressedExpressionWithMultipleOperatorsTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Eight, L"8", L"" },
                                 { NumbersAndOperatorsEnum::Divide, L"8", L"8 / " },
                                 { NumbersAndOperatorsEnum::Two, L"2", L"8 / " },
                                 { NumbersAndOperatorsEnum::Multiply, L"4", L"8 / 2 * " },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 8/+*2*
        TEST_METHOD(ButtonPressedExpressionWithMultipleOperatorsInSuccessionTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Eight, L"8", L"" },   { NumbersAndOperatorsEnum::Divide, L"8", L"8 / " },
                                 { NumbersAndOperatorsEnum::Add, L"8", L"8 + " }, { NumbersAndOperatorsEnum::Multiply, L"8", L"8 * " },
                                 { NumbersAndOperatorsEnum::Two, L"2", L"8 * " }, { NumbersAndOperatorsEnum::Multiply, L"16", L"8 * 2 * " },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 8*2==
        TEST_METHOD(ButtonPressedExpressionWithMultipleEqualsAfterEvaluateTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Eight, L"8", L"" },   { NumbersAndOperatorsEnum::Multiply, L"8", L"8 * " },
                                 { NumbersAndOperatorsEnum::Two, L"2", L"8 * " }, { NumbersAndOperatorsEnum::Equals, L"16", L"" },
                                 { NumbersAndOperatorsEnum::Equals, L"32", L"" }, { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 7-6 and Backspace
        TEST_METHOD(ButtonPressedExpressionWithBackSpaceTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Seven, L"7", L"" },         { NumbersAndOperatorsEnum::Subtract, L"7", L"7 - " },
                                 { NumbersAndOperatorsEnum::Six, L"6", L"7 - " },       { NumbersAndOperatorsEnum::Backspace, L"0", L"7 - " },
                                 { NumbersAndOperatorsEnum::Backspace, L"0", L"7 - " }, { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        /// Expression : 91-68 and Clear
        TEST_METHOD(ButtonPressedExpressionWithClearTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::Nine, L"9", L"" },
                                 { NumbersAndOperatorsEnum::One, L"91", L"" },
                                 { NumbersAndOperatorsEnum::Subtract, L"91", L"91 - " },
                                 { NumbersAndOperatorsEnum::Six, L"6", L"91 - " },
                                 { NumbersAndOperatorsEnum::Eight, L"68", L"91 - " },
                                 { NumbersAndOperatorsEnum::Clear, L"0", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
        }

        ///
        /// Paste tests
        ///
        template <typename T, typename Base>
        void ValidateIntegralsAreEqual(NumbersAndOperatorsEnum a, NumbersAndOperatorsEnum b)
        {
            VERIFY_ARE_EQUAL(static_cast<Base>(a), static_cast<Base>(b));
        }

        void ValidateNumbersAndOperatorsAreEqual(NumbersAndOperatorsEnum a, NumbersAndOperatorsEnum b)
        {
            ValidateIntegralsAreEqual<NumbersAndOperatorsEnum, int>(a, b);
        }

        /// Low-level test of character mapping
        TEST_METHOD(VerifyCorrectCharacterMapping)
        {
            // Valid numbers
            NumbersAndOperatorsEnum n = m_viewModel->MapCharacterToButtonId(L'0').buttonId;
            ValidateNumbersAndOperatorsAreEqual(n, NumbersAndOperatorsEnum::Zero);

            n = m_viewModel->MapCharacterToButtonId(L'1').buttonId;
            ValidateNumbersAndOperatorsAreEqual(n, NumbersAndOperatorsEnum::One);

            // Valid operators
            n = m_viewModel->MapCharacterToButtonId(L'+').buttonId;
            ValidateNumbersAndOperatorsAreEqual(n, NumbersAndOperatorsEnum::Add);

            n = m_viewModel->MapCharacterToButtonId(L'=').buttonId;
            ValidateNumbersAndOperatorsAreEqual(n, NumbersAndOperatorsEnum::Equals);

            n = m_viewModel->MapCharacterToButtonId(L'a').buttonId;
            ValidateNumbersAndOperatorsAreEqual(n, NumbersAndOperatorsEnum::A);

            // Invalid character
            n = m_viewModel->MapCharacterToButtonId(L'$').buttonId;
            ValidateNumbersAndOperatorsAreEqual(n, NumbersAndOperatorsEnum::None);
        }

        /// Various strings get pasted
        TEST_METHOD(PasteExpressions)
        {
            m_viewModel->IsScientific = false;

            m_viewModel->OnPaste("-0.99");
            ValidateViewModelValue("-0" + m_decimalSeparator + "99");

            m_viewModel->OnPaste("1+1=");
            ValidateViewModelValue("2");

            // This result is not obvious: it's the result of the previous operation
            m_viewModel->OnPaste("0=");
            ValidateViewModelValue("1");

            // Negative value
            m_viewModel->OnPaste("-1");
            ValidateViewModelValue("-1");

            // Negated expression
            m_viewModel->OnPaste("-(1+1)");
            ValidateViewModelValueAndSecondaryExpression("-2", "negate(1 + 1)");

            // More complicated Negated expression
            m_viewModel->OnPaste("-(-(-1))");
            ValidateViewModelValueAndSecondaryExpression("-1", "negate(0 - (0 - 1))");

            // Switch to scientific mode
            m_viewModel->IsScientific = true;

            VERIFY_IS_FALSE(m_viewModel->IsFToEChecked);

            //// Positive exponent
            m_viewModel->OnPaste("1.23e+10");
            ValidateViewModelValue("1" + m_decimalSeparator + "23e+10");

            m_viewModel->OnPaste("1.23e10");
            ValidateViewModelValue("1" + m_decimalSeparator + "23e+10");

            m_viewModel->OnPaste("135e10");
            ValidateViewModelValue("135" + m_decimalSeparator + "e+10");

            //// Negative exponent
            m_viewModel->OnPaste("1.23e-10");
            ValidateViewModelValue("1" + m_decimalSeparator + "23e-10");

            //// Uppercase E (for exponent)
            m_viewModel->OnPaste("1.23E-10");
            ValidateViewModelValue("1" + m_decimalSeparator + "23e-10");

            m_viewModel->OnPaste("135E10");
            ValidateViewModelValue("135" + m_decimalSeparator + "e+10");
        }

        // Verify Calculator CalculationResultAutomationName is set correctly
        TEST_METHOD(CalculationResultAutomationNameVerification)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::IsStandardMode, L"0", L"" },
                                 { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Two, L"12", L"" },
                                 { NumbersAndOperatorsEnum::Three, L"123", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
            VERIFY_ARE_EQUAL(StringReference(L"Display is 123"), m_viewModel->CalculationResultAutomationName);

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" }, { NumbersAndOperatorsEnum::One, L"1", L"" },
                                  { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },          { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                  { NumbersAndOperatorsEnum::Multiply, L"2", L"1 + 2 * " }, { NumbersAndOperatorsEnum::Three, L"3", L"1 + 2 * " },
                                  { NumbersAndOperatorsEnum::Equals, L"7", L"" },           { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, true);
            VERIFY_ARE_EQUAL(StringReference(L"Display is 7"), m_viewModel->CalculationResultAutomationName);

            TESTITEM items3[] = { { NumbersAndOperatorsEnum::Clear, L"0", L"" },
                                  { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" },
                                  { NumbersAndOperatorsEnum::Five, L"5", L"" },
                                  { NumbersAndOperatorsEnum::InvSin, L"Invalid input", L"asind(5)" },
                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items3, false);
            VERIFY_ARE_EQUAL(StringReference(L"Display is Invalid input"), m_viewModel->CalculationResultAutomationName);
        }

        // Switch Calculator Mode and verify if mode switch is happening as expected.
        // Test precedence to check if mode switch is happening
        // Standard mode 1+2*3  = 9; Scientific mode 1+2*3 = 7
        // Intermediate value is also different. after 1 + 2 * , standard shows 3, scientific shows 2
        TEST_METHOD(ButtonPressedCalculatorModeSwitch)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::IsStandardMode, L"0", L"" },   { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },          { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                 { NumbersAndOperatorsEnum::Multiply, L"3", L"1 + 2 * " }, { NumbersAndOperatorsEnum::Three, L"3", L"1 + 2 * " },
                                 { NumbersAndOperatorsEnum::Equals, L"9", L"" },           { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" }, { NumbersAndOperatorsEnum::One, L"1", L"" },
                                  { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },          { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                  { NumbersAndOperatorsEnum::Multiply, L"2", L"1 + 2 * " }, { NumbersAndOperatorsEnum::Three, L"3", L"1 + 2 * " },
                                  { NumbersAndOperatorsEnum::Equals, L"7", L"" },           { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, true);
        }

        // Test AutoConvertedValue
        TEST_METHOD(ProgrammerModeAutoConvertedValue)
        {
            TESTITEM none[] = { { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, none, true);
            m_viewModel->IsProgrammer = true;
            TESTITEM items[] = { { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                                 { NumbersAndOperatorsEnum::F, L"F", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, false);
            VERIFY_ARE_EQUAL(m_viewModel->HexDisplayValue, StringReference(L"F"));
            VERIFY_ARE_EQUAL(m_viewModel->DecimalDisplayValue, StringReference(L"15"));
            VERIFY_ARE_EQUAL(m_viewModel->OctalDisplayValue, StringReference(L"17"));
            VERIFY_ARE_EQUAL(m_viewModel->BinaryDisplayValue, StringReference(L"1111"));
            auto val = ref new Platform::Collections::Vector<bool>(64, false);
            val->SetAt(0, true);
            val->SetAt(1, true);
            val->SetAt(2, true);
            val->SetAt(3, true);
            CompareVector<bool>(m_viewModel->BinaryDigits, val);
        }

        // Test Button disabling in different Radixes
        TEST_METHOD(ProgrammerModeButtonsDisable)
        {
            /* m_viewModel->IsProgrammer = true;
            m_viewModel->SwitchProgrammerModeBase(RadixType::Octal);
            VERIFY_IS_TRUE(m_viewModel->AreOCTButtonsEnabled);
            VERIFY_IS_FALSE(m_viewModel->AreHEXButtonsEnabled);
            VERIFY_IS_FALSE(m_viewModel->AreDECButtonsEnabled);
            m_viewModel->SwitchProgrammerModeBase(RadixType::Decimal);
            VERIFY_IS_FALSE(m_viewModel->AreHEXButtonsEnabled);
            VERIFY_IS_TRUE(m_viewModel->AreDECButtonsEnabled);
            VERIFY_IS_TRUE(m_viewModel->AreOCTButtonsEnabled);
            m_viewModel->SwitchProgrammerModeBase(RadixType::Hex);
            VERIFY_IS_TRUE(m_viewModel->AreHEXButtonsEnabled);
            VERIFY_IS_TRUE(m_viewModel->AreDECButtonsEnabled);
            VERIFY_IS_TRUE(m_viewModel->AreOCTButtonsEnabled);
            m_viewModel->SwitchProgrammerModeBase(RadixType::Binary);
            VERIFY_IS_FALSE(m_viewModel->AreHEXButtonsEnabled);
            VERIFY_IS_FALSE(m_viewModel->AreDECButtonsEnabled);
            VERIFY_IS_FALSE(m_viewModel->AreOCTButtonsEnabled);*/
        }

        // Test digit grouping for different radix in programmer mode
        TEST_METHOD(ProgrammerModeRadixGrouping)
        {
            TESTITEM none[] = { { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, none, true);
            m_viewModel->IsProgrammer = true;
            TESTITEM items[] = {
                { NumbersAndOperatorsEnum::One, L"1", L"" },
                { NumbersAndOperatorsEnum::Two, L"12", L"" },
                { NumbersAndOperatorsEnum::Three, L"123", L"" },
                { NumbersAndOperatorsEnum::Four, L"1,234", L"" },
                { NumbersAndOperatorsEnum::Five, L"12,345", L"" },
                { NumbersAndOperatorsEnum::Six, L"123,456", L"" },
                { NumbersAndOperatorsEnum::Seven, L"1,234,567", L"" },
                { NumbersAndOperatorsEnum::Eight, L"12,345,678", L"" },
                { NumbersAndOperatorsEnum::Nine, L"123,456,789", L"" },
                { NumbersAndOperatorsEnum::None, L"1", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items, true);
            VERIFY_ARE_EQUAL(m_viewModel->HexDisplayValue, StringReference(L"75B CD15"));
            VERIFY_ARE_EQUAL(m_viewModel->DecimalDisplayValue, StringReference(L"123,456,789"));
            VERIFY_ARE_EQUAL(m_viewModel->OctalDisplayValue, StringReference(L"726 746 425"));
            VERIFY_ARE_EQUAL(m_viewModel->BinaryDisplayValue, StringReference(L"0111 0101 1011 1100 1101 0001 0101"));
            auto val = ref new Platform::Collections::Vector<bool>(64, false);
            val->SetAt(0, true);
            val->SetAt(2, true);
            val->SetAt(4, true);
            val->SetAt(8, true);
            val->SetAt(10, true);
            val->SetAt(11, true);
            val->SetAt(14, true);
            val->SetAt(15, true);
            val->SetAt(16, true);
            val->SetAt(17, true);
            val->SetAt(19, true);
            val->SetAt(20, true);
            val->SetAt(22, true);
            val->SetAt(24, true);
            val->SetAt(25, true);
            val->SetAt(26, true);
            CompareVector<bool>(m_viewModel->BinaryDigits, val);
        }

        // Test Not functionality
        TEST_METHOD(ProgrammerModeNot)
        {
            TESTITEM none[] = { { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, none, true);
            m_viewModel->IsProgrammer = true;
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Not, L"-2", L"~(1)" },
                                 { NumbersAndOperatorsEnum::None, L"N/A", L"N/A" } };
            ValidateViewModelByCommands(m_viewModel, items, false);
            VERIFY_ARE_EQUAL(m_viewModel->HexDisplayValue, StringReference(L"FFFF FFFF FFFF FFFE"));
            VERIFY_ARE_EQUAL(m_viewModel->DecimalDisplayValue, StringReference(L"-2"));
            VERIFY_ARE_EQUAL(m_viewModel->OctalDisplayValue, StringReference(L"1 777 777 777 777 777 777 776"));
            VERIFY_ARE_EQUAL(
                m_viewModel->BinaryDisplayValue, StringReference(L"1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1110"));
            VERIFY_ARE_EQUAL(m_viewModel->DisplayValue, StringReference(L"-2"));
            auto val = ref new Platform::Collections::Vector<bool>(64, true);
            val->SetAt(0, false);
            CompareVector<bool>(m_viewModel->BinaryDigits, val);
        }

        // Test And Or functionality
        TEST_METHOD(ProgrammerModeAndOr)
        {
            TESTITEM none[] = { { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, none, true);
            m_viewModel->IsProgrammer = true;

            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Or, L"1", L"1 ||" },
                                 { NumbersAndOperatorsEnum::Two, L"2", L"1 ||" },
                                 { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                 { NumbersAndOperatorsEnum::None, L"3", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, false);

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                  { NumbersAndOperatorsEnum::And, L"1", L"1 &" },
                                  { NumbersAndOperatorsEnum::Two, L"2", L"1 &" },
                                  { NumbersAndOperatorsEnum::Equals, L"0", L"" },
                                  { NumbersAndOperatorsEnum::None, L"0", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, false);
        }

        // Test CE and C buttons functionality
        TEST_METHOD(ProgrammerModeClear)
        {
            TESTITEM none[] = { { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, none, true);
            m_viewModel->IsProgrammer = true;

            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Or, L"1", L"1 ||" },
                                 { NumbersAndOperatorsEnum::Two, L"2", L"1 ||" },
                                 { NumbersAndOperatorsEnum::ClearEntry, L"0", L"1 ||" },
                                 { NumbersAndOperatorsEnum::None, L"", L"1 ||" } };
            ValidateViewModelByCommands(m_viewModel, items, false);

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                  { NumbersAndOperatorsEnum::And, L"1", L"1 &" },
                                  { NumbersAndOperatorsEnum::Two, L"2", L"1 &" },
                                  { NumbersAndOperatorsEnum::Clear, L"0", L"" },
                                  { NumbersAndOperatorsEnum::None, L"0", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, false);
        }

        // Test unary operators
        TEST_METHOD(ButtonPressedUnaryOperatorTest)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::IsStandardMode, L"0", L"" },
                                 { NumbersAndOperatorsEnum::Five, L"5", L"" },
                                 { NumbersAndOperatorsEnum::Invert, L"0" + std::wstring(m_decimalSeparator->Data()) + L"2", L"reciproc(5)" },
                                 { NumbersAndOperatorsEnum::Equals, L"0" + std::wstring(m_decimalSeparator->Data()) + L"2", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                  { NumbersAndOperatorsEnum::Six, L"16", L"" },
                                  { NumbersAndOperatorsEnum::Sqrt, L"4", L"sqrt(16)" },
                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, false);

            TESTITEM items3[] = { { NumbersAndOperatorsEnum::Six, L"6", L"" },
                                  { NumbersAndOperatorsEnum::Negate, L"-6", L"" },
                                  { NumbersAndOperatorsEnum::Nine, L"-69", L"" },
                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items3, false);

            TESTITEM items4[] = { { NumbersAndOperatorsEnum::Clear, L"0", L"" },
                                  { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" },
                                  { NumbersAndOperatorsEnum::Five, L"5", L"" },
                                  { NumbersAndOperatorsEnum::InvSin, L"Invalid input", L"asind(5)" },
                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items4, false);

            TESTITEM items5[] = { { NumbersAndOperatorsEnum::Clear, L"0", L"" },
                                  { NumbersAndOperatorsEnum::Four, L"4", L"" },
                                  { NumbersAndOperatorsEnum::Factorial, L"24", L"fact(4)" },
                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items5, false);
        }

        // IsMemoryEmpty Property
        TEST_METHOD(IsMemoryEmptyTest)
        {
            StandardCalculatorViewModel ^ viewModel = ref new StandardCalculatorViewModel();
            viewModel->IsStandard = true;
            VERIFY_ARE_EQUAL(static_cast<unsigned int>(0), viewModel->MemorizedNumbers->Size);
            viewModel->OnMemoryButtonPressed();
            VERIFY_ARE_EQUAL(static_cast<unsigned int>(1), viewModel->MemorizedNumbers->Size);
            viewModel->ClearMemoryCommand->Execute(nullptr);
            VERIFY_ARE_EQUAL(static_cast<unsigned int>(0), viewModel->MemorizedNumbers->Size);
        }

        // IsOperatorCommand Property
        TEST_METHOD(IsOperatorCommandTest)
        {
            StandardCalculatorViewModel ^ viewModel = ref new StandardCalculatorViewModel();
            viewModel->IsStandard = true;
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::One);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Two);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Three);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Four);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Five);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Six);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Seven);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Eight);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Nine);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Decimal);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Zero);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Multiply);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, true);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Add);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, true);
            viewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Zero);
            VERIFY_ARE_EQUAL(viewModel->IsOperatorCommand, false);
        }

        // When memory button is pressed - verify if display value is being stored in vector
        TEST_METHOD(OnMemoryButtonPressed)
        {
            StandardCalculatorViewModel ^ viewModel = ref new StandardCalculatorViewModel();
            viewModel->IsStandard = true;
            viewModel->DisplayValue = L"1001";
            viewModel->OnMemoryButtonPressed();
            viewModel->OnMemoryButtonPressed();
            VERIFY_ARE_EQUAL((int)viewModel->MemorizedNumbers->Size, 2);
        }

        // When memory list is empty and M+ is pressed
        TEST_METHOD(OnMemoryAddWhenMemoryEmpty)
        {
            m_viewModel->IsStandard = true;
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"100", L"" },
                                 { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemoryAdd(ref new Platform::Box<int>(0));
            m_viewModel->OnMemoryItemPressed(ref new Platform::Box<int>(0));
            VERIFY_ARE_EQUAL(Platform::StringReference(L"1,001"), m_viewModel->DisplayValue);
        }

        // When memory list is empty and M- is pressed
        TEST_METHOD(OnMemorySubtractWhenMemoryEmpty)
        {
            m_viewModel->IsStandard = true;
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"100", L"" },
                                 { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemorySubtract(ref new Platform::Box<int>(0));
            m_viewModel->OnMemoryItemPressed(ref new Platform::Box<int>(0));
            VERIFY_ARE_EQUAL(Platform::StringReference(L"-1,001"), m_viewModel->DisplayValue);
        }

        // When negative number is saved in memory
        TEST_METHOD(OnNegativeEntryInMemory)
        {
            ChangeMode(m_viewModel, 0 /*Standard*/);
            TESTITEM items[] = {
                { NumbersAndOperatorsEnum::One, L"1", L"" },         { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                { NumbersAndOperatorsEnum::Zero, L"100", L"" },      { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                { NumbersAndOperatorsEnum::Negate, L"N/A", L"N/A" }, { NumbersAndOperatorsEnum::None, L"", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemoryButtonPressed();
            m_viewModel->OnMemoryItemPressed(ref new Platform::Box<int>(0));
            VERIFY_ARE_EQUAL(Platform::StringReference(L"-1,001"), m_viewModel->DisplayValue);
            MemoryItemViewModel ^ memorySlotStandard = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"-1,001"), memorySlotStandard->Value);
            ChangeMode(m_viewModel, 1 /*scientific*/);
            MemoryItemViewModel ^ memorySlotScientific = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"-1,001"), memorySlotScientific->Value);
            ChangeMode(m_viewModel, 2 /*Programmer*/);
            MemoryItemViewModel ^ memorySlotProgrammer = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"-1,001"), memorySlotProgrammer->Value);
        }

        // When decimal number is saved in memory
        TEST_METHOD(OnDecimalEntryInMemory)
        {
            ChangeMode(m_viewModel, 0 /*Standard*/);
            TESTITEM items[] = {
                { NumbersAndOperatorsEnum::One, L"1", L"" },          { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                { NumbersAndOperatorsEnum::Zero, L"100", L"" },       { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                { NumbersAndOperatorsEnum::Decimal, L"1,001.", L"" }, { NumbersAndOperatorsEnum::One, L"1,001.1", L"" },
                { NumbersAndOperatorsEnum::None, L"", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemoryButtonPressed();
            m_viewModel->OnMemoryItemPressed(ref new Platform::Box<int>(0));
            VERIFY_ARE_EQUAL(Platform::StringReference(L"1,001.1"), m_viewModel->DisplayValue);
            MemoryItemViewModel ^ memorySlotStandard = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"1,001.1"), memorySlotStandard->Value);
            ChangeMode(m_viewModel, 1 /*Scientific*/);
            MemoryItemViewModel ^ memorySlotScientific = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"1,001.1"), memorySlotScientific->Value);
            ChangeMode(m_viewModel, 2 /*Programmer*/);
            MemoryItemViewModel ^ memorySlotProgrammer = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"1,001"), memorySlotProgrammer->Value);
        }

        // When negative decimal number is saved in memory
        TEST_METHOD(OnNegativeDecimalInMemory)
        {
            m_viewModel->IsStandard = true;
            TESTITEM items[] = {
                { NumbersAndOperatorsEnum::One, L"1", L"" },          { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                { NumbersAndOperatorsEnum::Zero, L"100", L"" },       { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                { NumbersAndOperatorsEnum::Decimal, L"1,001.", L"" }, { NumbersAndOperatorsEnum::One, L"1,001.1", L"" },
                { NumbersAndOperatorsEnum::Negate, L"N/A", L"N/A" },  { NumbersAndOperatorsEnum::None, L"", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemoryButtonPressed();
            m_viewModel->OnMemoryItemPressed(ref new Platform::Box<int>(0));
            VERIFY_ARE_EQUAL(Platform::StringReference(L"-1,001.1"), m_viewModel->DisplayValue);
        }

        // When decimal number is added to the memory
        TEST_METHOD(OnDecimalAddedToMemory)
        {
            m_viewModel->IsStandard = true;
            TESTITEM items[] = {
                { NumbersAndOperatorsEnum::One, L"1", L"" },          { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                { NumbersAndOperatorsEnum::Zero, L"100", L"" },       { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                { NumbersAndOperatorsEnum::Decimal, L"1,001.", L"" }, { NumbersAndOperatorsEnum::One, L"1,001.1", L"" },
                { NumbersAndOperatorsEnum::None, L"", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemoryButtonPressed();
            TESTITEM items2[] = {
                { NumbersAndOperatorsEnum::One, L"1", L"" },          { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                { NumbersAndOperatorsEnum::Zero, L"100", L"" },       { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                { NumbersAndOperatorsEnum::Decimal, L"1,001.", L"" }, { NumbersAndOperatorsEnum::None, L"", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items2, false);
            m_viewModel->OnMemoryButtonPressed();
            m_viewModel->OnMemoryAdd(1);
            m_viewModel->OnMemoryItemPressed(1);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"2,002.1"), m_viewModel->DisplayValue);
        }

        // When memory is saved in programmer as Hex value and then we switch to standard mode, test to see that memory gets converted to decimal
        TEST_METHOD(OnMemorySavedInHexRadixAndSwitchedToStandardMode)
        {
            ChangeMode(m_viewModel, 2 /*programmer*/);
            TESTITEM items[] = {
                { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                { NumbersAndOperatorsEnum::F, L"F", L"" },
                { NumbersAndOperatorsEnum::F, L"FF", L"" },
                { NumbersAndOperatorsEnum::None, L"FF", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemoryButtonPressed();
            ChangeMode(m_viewModel, 1 /*Scientific*/);
            m_viewModel->OnMemoryItemPressed(ref new Box<int>(0));
            VERIFY_ARE_EQUAL(Platform::StringReference(L"255"), m_viewModel->DisplayValue);
            MemoryItemViewModel ^ memorySlot = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"255"), memorySlot->Value);
        }

        TEST_METHOD(OnMemorySavedInHexRadixAndRadixChanges)
        {
            ChangeMode(m_viewModel, 2 /*programmer*/);
            TESTITEM items[] = {
                { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                { NumbersAndOperatorsEnum::F, L"F", L"" },
                { NumbersAndOperatorsEnum::F, L"FF", L"" },
                { NumbersAndOperatorsEnum::None, L"FF", L"" },
            };
            ValidateViewModelByCommands(m_viewModel, items, true);
            m_viewModel->OnMemoryButtonPressed();
            m_viewModel->SwitchProgrammerModeBase(NumberBase::OctBase);
            MemoryItemViewModel ^ memorySlotOct = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"377"), memorySlotOct->Value);
            m_viewModel->SwitchProgrammerModeBase(NumberBase::DecBase);
            MemoryItemViewModel ^ memorySlotDec = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"255"), memorySlotDec->Value);
            m_viewModel->SwitchProgrammerModeBase(NumberBase::BinBase);
            MemoryItemViewModel ^ memorySlotBin = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"1111 1111"), memorySlotBin->Value);
        }

        // When memory button is pressed more than max number of slots allowed,
        // the MemorizedNumbers vector size should not increase.
        TEST_METHOD(OnMemoryButtonPressedMaxTimes)
        {
            StandardCalculatorViewModel ^ viewModel = ref new StandardCalculatorViewModel();
            viewModel->IsStandard = true;
            viewModel->DisplayValue = L"1001";
            for (int i = 0; i < 110; i++)
            {
                viewModel->OnMemoryButtonPressed();
            }
            VERIFY_ARE_EQUAL((int)viewModel->MemorizedNumbers->Size, 100);
        }

        // When memory slot is pressed verify if the display value is updated correctly
        TEST_METHOD(OnMemoryItemPressed)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"100", L"" },
                                 { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);

            m_viewModel->OnMemoryButtonPressed();

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::Two, L"2", L"" },
                                  { NumbersAndOperatorsEnum::Zero, L"20", L"" },
                                  { NumbersAndOperatorsEnum::Zero, L"200", L"" },
                                  { NumbersAndOperatorsEnum::Two, L"2,002", L"" },
                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, false);

            m_viewModel->OnMemoryButtonPressed();
            m_viewModel->OnMemoryItemPressed(1);

            VERIFY_ARE_EQUAL(Platform::StringReference(L"1,001"), m_viewModel->DisplayValue);
        }

        // Verify nothing happens if there is no memory and the memory slot pressed action is taken
        TEST_METHOD(OnMemoryItemPressedNoMemory)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Two, L"12", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);

            m_viewModel->OnMemoryItemPressed(ref new Platform::Box<int>(0));

            VERIFY_ARE_EQUAL(StringReference(L"12"), m_viewModel->DisplayValue);
            VERIFY_ARE_EQUAL((UINT)0, m_viewModel->MemorizedNumbers->Size);
        }

        // When memory slot is pressed verify if the display value is updated correctly
        TEST_METHOD(OnMemoryItemAddAndSubtract)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"100", L"" },
                                 { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);

            m_viewModel->OnMemoryButtonPressed();

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::Two, L"2", L"" },
                                  { NumbersAndOperatorsEnum::Zero, L"20", L"" },
                                  { NumbersAndOperatorsEnum::Zero, L"200", L"" },
                                  { NumbersAndOperatorsEnum::Two, L"2,002", L"" },
                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, false);

            m_viewModel->OnMemoryButtonPressed();
            m_viewModel->OnMemoryItemPressed(1);
            m_viewModel->OnMemoryAdd(ref new Platform::Box<int>(0));
            MemoryItemViewModel ^ memorySlot = (MemoryItemViewModel ^) m_viewModel->MemorizedNumbers->GetAt(0);
            VERIFY_ARE_EQUAL(Platform::StringReference(L"3,003"), memorySlot->Value);
        }

        // Verify that raw, unformatted numbers are provided correctly
        TEST_METHOD(VerifyRawFormatting)
        {
            m_viewModel->DisplayValue = L"1,001";
            VERIFY_ARE_EQUAL(StringReference(L"1001"), m_viewModel->GetRawDisplayValue());

            m_viewModel->DisplayValue = L"999";
            VERIFY_ARE_EQUAL(StringReference(L"999"), m_viewModel->GetRawDisplayValue());

            m_viewModel->DisplayValue = L"1,001,001";
            VERIFY_ARE_EQUAL(StringReference(L"1001001"), m_viewModel->GetRawDisplayValue());

            m_viewModel->DisplayValue = L"1,001, 001";
            VERIFY_ARE_EQUAL(StringReference(L"1001001"), m_viewModel->GetRawDisplayValue());
        }

        TEST_METHOD(VerifyAnnouncementAfterBinaryOperatorReceived)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"100", L"" },
                                 { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                                 { NumbersAndOperatorsEnum::Multiply, L"1,001", L"1001 x" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);

            VERIFY_ARE_EQUAL(StringReference(L"Display is 1,001 times"), m_viewModel->Announcement->Announcement);
        }

        TEST_METHOD(VerifyAnnouncementAfterMultipleBinaryOperatorsReceived)
        {
            TESTITEM items[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                 { NumbersAndOperatorsEnum::Zero, L"100", L"" },
                                 { NumbersAndOperatorsEnum::One, L"1,001", L"" },
                                 { NumbersAndOperatorsEnum::Multiply, L"1,001", L"1001 x" },
                                 { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items, true);

            VERIFY_ARE_EQUAL(StringReference(L"Display is 1,001 times"), m_viewModel->Announcement->Announcement);

            TESTITEM items2[] = { { NumbersAndOperatorsEnum::Divide, L"1,001", L"1001 ÷" }, { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items2, false /*reset*/);

            VERIFY_ARE_EQUAL(StringReference(L"Display is 1,001 divided by"), m_viewModel->Announcement->Announcement);

            TESTITEM items3[] = { { NumbersAndOperatorsEnum::Add, L"1,001", L"1001 +" }, { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items3, false /*reset*/);

            VERIFY_ARE_EQUAL(StringReference(L"Display is 1,001 plus"), m_viewModel->Announcement->Announcement);

            TESTITEM items4[] = { { NumbersAndOperatorsEnum::Subtract, L"1,001", L"1001 x" }, { NumbersAndOperatorsEnum::None, L"", L"" } };
            ValidateViewModelByCommands(m_viewModel, items4, false /*reset*/);

            VERIFY_ARE_EQUAL(StringReference(L"Display is 1,001 minus"), m_viewModel->Announcement->Announcement);
        }

    private:
        StandardCalculatorViewModel ^ m_viewModel;
        std::shared_ptr<EngineResourceProvider> m_engineResourceProvider;
        Platform::String ^ m_decimalSeparator;
    };
}
