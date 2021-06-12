// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include "UnitConverterViewModelUnitTests.h"
#include "DateUtils.h"

#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/UnitConverterViewModel.h"
#include "CalcViewModel/DateCalculatorViewModel.h"
#include "CalcViewModel/DataLoaders/UnitConverterDataLoader.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace CalculationManager;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Devices::Input;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Utils;
using namespace DateCalculationUnitTests;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    extern void ChangeMode(StandardCalculatorViewModel ^ viewModel, int mode);
    extern void ValidateViewModelByCommands(StandardCalculatorViewModel ^ viewModel, TESTITEM item[], bool doReset = false);

    // Validated the Mode set for a given instance of Standard Calculator View Model
    void ValidateViewModelMode(StandardCalculatorViewModel ^ viewModel, int mode)
    {
        // Standard
        if (mode == 0)
        {
            VERIFY_IS_TRUE(viewModel->IsStandard);
            VERIFY_IS_FALSE(viewModel->IsScientific);
            VERIFY_IS_FALSE(viewModel->IsProgrammer);
        }
        // Scientific
        else if (mode == 1)
        {
            VERIFY_IS_FALSE(viewModel->IsStandard);
            VERIFY_IS_TRUE(viewModel->IsScientific);
            VERIFY_IS_FALSE(viewModel->IsProgrammer);
        }
        // Programmer
        else if (mode == 2)
        {
            VERIFY_IS_FALSE(viewModel->IsStandard);
            VERIFY_IS_FALSE(viewModel->IsScientific);
            VERIFY_IS_TRUE(viewModel->IsProgrammer);
        }
    }

    // Test class containing Test Methods to validate Multi Window
    TEST_CLASS(MultiWindowUnitTests){ public:
                                          // Create 3 instances of StandardCalculatorViewModel
                                          TEST_METHOD(InitializeMultipleInstancesTest){ std::vector<StandardCalculatorViewModel ^> viewModels(3);

    // Create 3 instances of StandardCalculatorViewModel
    for (int i = 0; i < 3; i++)
    {
        viewModels[i] = ref new StandardCalculatorViewModel();
        viewModels[i]->IsStandard = true;
    }

    // Assert that the Display Value is "0" for all instances
    for (int i = 0; i < 3; i++)
    {
        VERIFY_IS_TRUE("0" == viewModels[i]->DisplayValue);
    }
}

// Create 3 separate instances of Calculator in different modes
TEST_METHOD(InitializeMultipleModeInstancesTest)
{
    std::vector<StandardCalculatorViewModel ^> viewModels(3);

    // Create 3 instances of StandardCalculatorViewModel in different modes
    for (int i = 0; i < 3; i++)
    {
        viewModels[i] = ref new StandardCalculatorViewModel();

        ChangeMode(viewModels[i], i);
    }

    // Assert for the Modes and DisplayValues
    for (int i = 0; i < 3; i++)
    {
        ValidateViewModelMode(viewModels[i], i);

        VERIFY_IS_TRUE("0" == viewModels[i]->DisplayValue);
    }
}

// Perform calculations on different calculator modes and verify that they work independently
TEST_METHOD(MultipleModesCalculationTest)
{
    std::vector<StandardCalculatorViewModel ^> viewModels(3);

    // Create 3 instances of StandardCalculatorViewModel in different modes
    for (int i = 0; i < 3; i++)
    {
        viewModels[i] = ref new StandardCalculatorViewModel();

        ChangeMode(viewModels[i], i);
    }

    // Perform Calculations on all instances and check that they work independently

    // Standard Mode: Expression 1+2=
    TESTITEM standardModeTestItems[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                         { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                         { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                         { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                         { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[0], standardModeTestItems, true);

    // Scientific Mode: Expression 1+2*3
    TESTITEM scientificModeTestItems[] = { { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" }, { NumbersAndOperatorsEnum::One, L"1", L"" },
                                           { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },          { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                           { NumbersAndOperatorsEnum::Multiply, L"2", L"1 + 2 * " }, { NumbersAndOperatorsEnum::Three, L"3", L"1 + 2 * " },
                                           { NumbersAndOperatorsEnum::Equals, L"7", L"" },           { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[1], scientificModeTestItems, true);

    // Programmer Mode: Expression F
    TESTITEM programmerModeTestItems[] = { { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                                           { NumbersAndOperatorsEnum::F, L"F", L"" },
                                           { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[2], programmerModeTestItems, false);
    VERIFY_ARE_EQUAL(viewModels[2]->HexDisplayValue, StringReference(L"F"));
    VERIFY_ARE_EQUAL(viewModels[2]->DecimalDisplayValue, StringReference(L"15"));
    VERIFY_ARE_EQUAL(viewModels[2]->OctalDisplayValue, StringReference(L"17"));
    VERIFY_ARE_EQUAL(viewModels[2]->BinaryDisplayValue, StringReference(L"1111"));

    // Assert that Standard and Scientific mode Display Values are unchanged
    VERIFY_ARE_EQUAL(viewModels[0]->DisplayValue, StringReference(L"3"));
    VERIFY_ARE_EQUAL(viewModels[1]->DisplayValue, StringReference(L"7"));
}

// Perform calculations on 2 instances of Calculator in Standard Mode and verify that they work independently
TEST_METHOD(MultipleStandardModeCalculationTest)
{
    // Create 2 instances of Standard Mode
    StandardCalculatorViewModel ^ standardViewModel1 = ref new StandardCalculatorViewModel();
    StandardCalculatorViewModel ^ standardViewModel2 = ref new StandardCalculatorViewModel();

    ChangeMode(standardViewModel1, 0);
    ChangeMode(standardViewModel2, 0);

    ValidateViewModelMode(standardViewModel1, 0);
    ValidateViewModelMode(standardViewModel2, 0);

    // Perform Calculations on the 2 instances and check that they work independently

    // Standard Mode 1: Expression 3-2=
    TESTITEM standardModeTestItems1[] = { { NumbersAndOperatorsEnum::Three, L"3", L"" },
                                          { NumbersAndOperatorsEnum::Subtract, L"3", L"3 - " },
                                          { NumbersAndOperatorsEnum::Two, L"2", L"3 - " },
                                          { NumbersAndOperatorsEnum::Equals, L"1", L"" },
                                          { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(standardViewModel1, standardModeTestItems1, true);

    // Standard Mode 2: Expression 4*5=
    TESTITEM standardModeTestItems2[] = { { NumbersAndOperatorsEnum::Four, L"4", L"" },
                                          { NumbersAndOperatorsEnum::Multiply, L"4", L"4 * " },
                                          { NumbersAndOperatorsEnum::Five, L"5", L"4 * " },
                                          { NumbersAndOperatorsEnum::Equals, L"20", L"" },
                                          { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(standardViewModel2, standardModeTestItems2, true);

    // Assert that the Display Value of 1st instance is unchanged
    VERIFY_ARE_EQUAL(standardViewModel1->DisplayValue, StringReference(L"1"));
}

// Perform calculations on 2 instances of Calculator in Scientific Mode and verify that they work independently
TEST_METHOD(MultipleScientificModeCalculationTest)
{
    // Create 2 instances of Standard Mode
    StandardCalculatorViewModel ^ scientificViewModel1 = ref new StandardCalculatorViewModel();
    StandardCalculatorViewModel ^ scientificViewModel2 = ref new StandardCalculatorViewModel();

    ChangeMode(scientificViewModel1, 1);
    ChangeMode(scientificViewModel2, 1);

    ValidateViewModelMode(scientificViewModel1, 1);
    ValidateViewModelMode(scientificViewModel2, 1);

    // Perform Calculations on the 2 instances and check that they work independently

    // Standard Mode 1: Expression 3-2=
    TESTITEM scientificModeTestItems1[] = { { NumbersAndOperatorsEnum::Three, L"3", L"" },
                                            { NumbersAndOperatorsEnum::Subtract, L"3", L"3 - " },
                                            { NumbersAndOperatorsEnum::Two, L"2", L"3 - " },
                                            { NumbersAndOperatorsEnum::Equals, L"1", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(scientificViewModel1, scientificModeTestItems1, true);

    // Standard Mode 2: Expression 4*5=
    TESTITEM scientificModeTestItems2[] = { { NumbersAndOperatorsEnum::Four, L"4", L"" },
                                            { NumbersAndOperatorsEnum::Multiply, L"4", L"4 * " },
                                            { NumbersAndOperatorsEnum::Five, L"5", L"4 * " },
                                            { NumbersAndOperatorsEnum::Equals, L"20", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(scientificViewModel2, scientificModeTestItems2, true);

    // Assert that the Display Value of 1st instance is unchanged
    VERIFY_ARE_EQUAL(scientificViewModel1->DisplayValue, StringReference(L"1"));
}

// Perform calculations on 2 instances of Calculator in Scientific Mode
// (with different Angle types, HYP and F-E settings) and verify that they work independently
TEST_METHOD(MultipleScientificModeWithDifferentSettingsTest)
{
    // Create 2 instances of Standard Mode
    StandardCalculatorViewModel ^ scientificViewModel1 = ref new StandardCalculatorViewModel();
    StandardCalculatorViewModel ^ scientificViewModel2 = ref new StandardCalculatorViewModel();

    ChangeMode(scientificViewModel1, 1);
    ChangeMode(scientificViewModel2, 1);

    ValidateViewModelMode(scientificViewModel1, 1);
    ValidateViewModelMode(scientificViewModel2, 1);

    // Perform Calculations on the 2 instances and check that they work independently

    // Scientific Mode 1: Degrees with HYP checked
    TESTITEM scientificModeInitializeItems1[] = { { NumbersAndOperatorsEnum::Degree, L"0", L"" },
                                                  { NumbersAndOperatorsEnum::Hyp, L"0", L"" },
                                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(scientificViewModel1, scientificModeInitializeItems1, true);

    // Scientific Mode 2: Radians with F-E checked
    TESTITEM scientificModeInitializeItems2[] = { { NumbersAndOperatorsEnum::Radians, L"0", L"" },
                                                  { NumbersAndOperatorsEnum::FToE, L"0.e+0", L"" },
                                                  { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(scientificViewModel2, scientificModeInitializeItems2, true);

    // Scientific Mode 1: Expression CosH(0 degrees)
    TESTITEM scientificModeTestItems1[] = { { NumbersAndOperatorsEnum::Zero, L"0", L"" },
                                            { NumbersAndOperatorsEnum::Cosh, L"1", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(scientificViewModel1, scientificModeTestItems1, true);

    // Scientific Mode 2: Expression Cos(pi radians)
    TESTITEM scientificModeTestItems2[] = { { NumbersAndOperatorsEnum::Pi, L"3.1415926535897932384626433832795e+0", L"" },
                                            { NumbersAndOperatorsEnum::Cos, L"-1.e+0", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(scientificViewModel2, scientificModeTestItems2, true);
}

// Perform calculations on 2 instances of Calculator in Programmer Mode and verify that they work independently
TEST_METHOD(MultipleProgrammerModeCalculationTest)
{
    // Create 2 instances of Standard Mode
    StandardCalculatorViewModel ^ programmerViewModel1 = ref new StandardCalculatorViewModel();
    StandardCalculatorViewModel ^ programmerViewModel2 = ref new StandardCalculatorViewModel();

    ChangeMode(programmerViewModel1, 2);
    ChangeMode(programmerViewModel2, 2);

    ValidateViewModelMode(programmerViewModel1, 2);
    ValidateViewModelMode(programmerViewModel2, 2);

    // Perform Calculations on the 2 instances and check that they work independently

    // Radix Type: Hexadecimal, Expression: F
    TESTITEM programmerModeTestItems1[] = { { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                                            { NumbersAndOperatorsEnum::F, L"F", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };

    ValidateViewModelByCommands(programmerViewModel1, programmerModeTestItems1, false);
    VERIFY_ARE_EQUAL(programmerViewModel1->HexDisplayValue, StringReference(L"F"));
    VERIFY_ARE_EQUAL(programmerViewModel1->DecimalDisplayValue, StringReference(L"15"));
    VERIFY_ARE_EQUAL(programmerViewModel1->OctalDisplayValue, StringReference(L"17"));
    VERIFY_ARE_EQUAL(programmerViewModel1->BinaryDisplayValue, StringReference(L"1111"));

    // Radix Type: Octal, Expression: 7
    TESTITEM programmerModeTestItems2[] = { { NumbersAndOperatorsEnum::OctButton, L"0", L"" },
                                            { NumbersAndOperatorsEnum::Seven, L"7", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };

    ValidateViewModelByCommands(programmerViewModel2, programmerModeTestItems2, false);
    VERIFY_ARE_EQUAL(programmerViewModel2->HexDisplayValue, StringReference(L"7"));
    VERIFY_ARE_EQUAL(programmerViewModel2->DecimalDisplayValue, StringReference(L"7"));
    VERIFY_ARE_EQUAL(programmerViewModel2->OctalDisplayValue, StringReference(L"7"));
    VERIFY_ARE_EQUAL(programmerViewModel2->BinaryDisplayValue, StringReference(L"0111"));

    // Assert that displayed values of 1st instance are unchanged
    VERIFY_ARE_EQUAL(programmerViewModel1->DisplayValue, StringReference(L"F"));
    VERIFY_ARE_EQUAL(programmerViewModel1->HexDisplayValue, StringReference(L"F"));
    VERIFY_ARE_EQUAL(programmerViewModel1->DecimalDisplayValue, StringReference(L"15"));
    VERIFY_ARE_EQUAL(programmerViewModel1->OctalDisplayValue, StringReference(L"17"));
    VERIFY_ARE_EQUAL(programmerViewModel1->BinaryDisplayValue, StringReference(L"1111"));
}

// Perform calculations on 2 instances of Calculator in Programmer Mode
// (with different Bit lengths and Radix types) and verify that they work independently
TEST_METHOD(MultipleProgrammerModeWithDifferentSettingsTest)
{
    // Create 2 instances of Standard Mode
    StandardCalculatorViewModel ^ programmerViewModel1 = ref new StandardCalculatorViewModel();
    StandardCalculatorViewModel ^ programmerViewModel2 = ref new StandardCalculatorViewModel();

    ChangeMode(programmerViewModel1, 2);
    ChangeMode(programmerViewModel2, 2);

    ValidateViewModelMode(programmerViewModel1, 2);
    ValidateViewModelMode(programmerViewModel2, 2);

    // Perform Calculations on the 2 instances and check that they work independently

    // Bit length: Byte & Radix Type: Hex
    TESTITEM programmerModeInitializeItems1[] = { { NumbersAndOperatorsEnum::Byte, L"0", L"" },
                                                  { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                                                  { NumbersAndOperatorsEnum::None, L"", L"" } };

    ValidateViewModelByCommands(programmerViewModel1, programmerModeInitializeItems1, false);

    // Bit Length: Word & Radix Type: Oct
    TESTITEM programmerModeInitializeItems2[] = { { NumbersAndOperatorsEnum::Word, L"0", L"" },
                                                  { NumbersAndOperatorsEnum::OctButton, L"0", L"" },
                                                  { NumbersAndOperatorsEnum::None, L"", L"" } };

    ValidateViewModelByCommands(programmerViewModel2, programmerModeInitializeItems2, false);

    TESTITEM programmerModeTestItems1[] = { { NumbersAndOperatorsEnum::F, L"F", L"" },
                                            { NumbersAndOperatorsEnum::F, L"FF", L"" },
                                            // One more F shouldn't have any effect, testing for precision
                                            { NumbersAndOperatorsEnum::F, L"FF", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };

    ValidateViewModelByCommands(programmerViewModel1, programmerModeTestItems1, false);
    VERIFY_ARE_EQUAL(programmerViewModel1->HexDisplayValue, StringReference(L"FF"));
    VERIFY_ARE_EQUAL(programmerViewModel1->DecimalDisplayValue, StringReference(L"-1"));
    VERIFY_ARE_EQUAL(programmerViewModel1->OctalDisplayValue, StringReference(L"377"));
    VERIFY_ARE_EQUAL(programmerViewModel1->BinaryDisplayValue, StringReference(L"1111 1111"));

    TESTITEM programmerModeTestItems2[] = { { NumbersAndOperatorsEnum::Seven, L"7", L"" },
                                            { NumbersAndOperatorsEnum::Seven, L"77", L"" },
                                            { NumbersAndOperatorsEnum::Seven, L"777", L"" },
                                            { NumbersAndOperatorsEnum::Seven, L"7 777", L"" },
                                            { NumbersAndOperatorsEnum::Seven, L"77 777", L"" },
                                            // One more '7' shouldn't have any effect, testing for precision
                                            { NumbersAndOperatorsEnum::Seven, L"77 777", L"" },
                                            { NumbersAndOperatorsEnum::None, L"", L"" } };

    ValidateViewModelByCommands(programmerViewModel2, programmerModeTestItems2, false);
    VERIFY_ARE_EQUAL(programmerViewModel2->HexDisplayValue, StringReference(L"7FFF"));
    VERIFY_ARE_EQUAL(programmerViewModel2->DecimalDisplayValue, StringReference(L"32,767"));
    VERIFY_ARE_EQUAL(programmerViewModel2->OctalDisplayValue, StringReference(L"77 777"));
    VERIFY_ARE_EQUAL(programmerViewModel2->BinaryDisplayValue, StringReference(L"0111 1111 1111 1111"));
}

// Perform calculations on 2 separate instances of Calculator and verify that their History list items are maintained separately
TEST_METHOD(MultipleModesHistoryAddItemTest)
{
    std::vector<StandardCalculatorViewModel ^> viewModels(2);

    // Create 3 instances of StandardCalculatorViewModel in Standard and Scientific mode
    for (int i = 0; i < 2; i++)
    {
        viewModels[i] = ref new StandardCalculatorViewModel();

        ChangeMode(viewModels[i], i);

        // Validate that the history items list is initially empty
        VERIFY_IS_TRUE(0 == viewModels[i]->HistoryVM->ItemsCount);
    }

    // Perform Calculations on both the instances and check that the History items work independently

    // Standard Mode: Expression 1+2=
    TESTITEM standardModeTestItems[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                         { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                         { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                         { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                         { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[0], standardModeTestItems, true);

    // Scientific Mode: Expression 1+2*3
    TESTITEM scientificModeTestItems[] = { { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" }, { NumbersAndOperatorsEnum::One, L"1", L"" },
                                           { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },          { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                           { NumbersAndOperatorsEnum::Multiply, L"2", L"1 + 2 * " }, { NumbersAndOperatorsEnum::Three, L"3", L"1 + 2 * " },
                                           { NumbersAndOperatorsEnum::Equals, L"7", L"" },           { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[1], scientificModeTestItems, true);

    // Assert for the history list items of 1st instance
    VERIFY_IS_TRUE(1 == viewModels[0]->HistoryVM->ItemsCount);

    auto item1 = static_cast<HistoryItemViewModel ^>(viewModels[0]->HistoryVM->Items->GetAt(0));
    String ^ expression1 = L"1   +   2 =";
    String ^ result1 = L"3";

    VERIFY_ARE_EQUAL(expression1, item1->Expression);
    VERIFY_ARE_EQUAL(result1, item1->Result);

    // Assert for the history list items of 2nd instance
    VERIFY_IS_TRUE(1 == viewModels[1]->HistoryVM->ItemsCount);

    auto item2 = static_cast<HistoryItemViewModel ^>(viewModels[1]->HistoryVM->Items->GetAt(0));
    String ^ expression2 = L"1   +   2   " + UtfUtils::MUL + L"   3 =";
    String ^ result2 = L"7";

    VERIFY_ARE_EQUAL(expression2, item2->Expression);
    VERIFY_ARE_EQUAL(result2, item2->Result);
}

// Perform calculations on 2 separate instances of Standard Modes and verify that their History list items are maintained separately
TEST_METHOD(MultipleStandardModesHistoryAddItemTest)
{
    std::vector<StandardCalculatorViewModel ^> viewModels(2);

    // Create 2 instances of StandardCalculatorViewModel in Standard mode
    for (int i = 0; i < 2; i++)
    {
        viewModels[i] = ref new StandardCalculatorViewModel();

        // Standard Mode
        ChangeMode(viewModels[i], 0);

        // Validate that the history items list is initially empty
        VERIFY_IS_TRUE(0 == viewModels[i]->HistoryVM->ItemsCount);
    }

    // Perform Calculations on both the instances and check that the History items work independently

    TESTITEM standardModeTestItems[2][8] = { { { NumbersAndOperatorsEnum::IsStandardMode, L"0", L"" },
                                               { NumbersAndOperatorsEnum::Three, L"3", L"" },
                                               { NumbersAndOperatorsEnum::Add, L"3", L"3 + " },
                                               { NumbersAndOperatorsEnum::Two, L"2", L"3 + " },
                                               { NumbersAndOperatorsEnum::Multiply, L"5", L"5 * " },
                                               { NumbersAndOperatorsEnum::One, L"1", L"5 * " },
                                               { NumbersAndOperatorsEnum::Equals, L"5", L"" },
                                               { NumbersAndOperatorsEnum::None, L"", L"" } },
                                             { { NumbersAndOperatorsEnum::IsStandardMode, L"0", L"" },
                                               { NumbersAndOperatorsEnum::One, L"1", L"" },
                                               { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                               { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                               { NumbersAndOperatorsEnum::Multiply, L"3", L"3 * " },
                                               { NumbersAndOperatorsEnum::Three, L"3", L"3 * " },
                                               { NumbersAndOperatorsEnum::Equals, L"9", L"" },
                                               { NumbersAndOperatorsEnum::None, L"", L"" } } };

    // Run the commands
    for (int i = 0; i < 2; i++)
    {
        ValidateViewModelByCommands(viewModels[i], standardModeTestItems[i], true);
    }

    String ^ expression[2][2] = { { L"5   " + UtfUtils::MUL + L"   1 =", L"3   +   2 =" }, { L"3   " + UtfUtils::MUL + L"   3 =", L"1   +   2 =" } };
    String ^ result[2][2] = { { L"5", L"5" }, { L"9", L"3" } };

    // Assert for the history list items of the instances
    for (int i = 0; i < 2; i++)
    {
        VERIFY_IS_TRUE(2 == viewModels[i]->HistoryVM->ItemsCount);

        for (int j = 0; j < 2; j++)
        {
            auto item = static_cast<HistoryItemViewModel ^>(viewModels[i]->HistoryVM->Items->GetAt(j));

            VERIFY_ARE_EQUAL(expression[i][j], item->Expression);
            VERIFY_ARE_EQUAL(result[i][j], item->Result);
        }
    }
}

// Perform calculations on 2 separate instances of Scientific Modes and verify that their History list items are maintained separately
TEST_METHOD(MultipleScientificModesHistoryAddItemTest)
{
    std::vector<StandardCalculatorViewModel ^> viewModels(2);

    // Create 2 instances of StandardCalculatorViewModel in Scientific mode
    for (int i = 0; i < 2; i++)
    {
        viewModels[i] = ref new StandardCalculatorViewModel();

        // Scientific Mode
        ChangeMode(viewModels[i], 1);

        // Validate that the history items list is initially empty
        VERIFY_IS_TRUE(0 == viewModels[i]->HistoryVM->ItemsCount);
    }

    // Perform Calculations on both the instances and check that the History items work independently

    TESTITEM scientificModeTestItems[2][8] = { { { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" },
                                                 { NumbersAndOperatorsEnum::One, L"1", L"" },
                                                 { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                                 { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                                 { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                                 { NumbersAndOperatorsEnum::None, L"", L"" } },
                                               { { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" },
                                                 { NumbersAndOperatorsEnum::One, L"1", L"" },
                                                 { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                                 { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                                 { NumbersAndOperatorsEnum::Multiply, L"2", L"1 + 2 * " },
                                                 { NumbersAndOperatorsEnum::Three, L"3", L"1 + 2 * " },
                                                 { NumbersAndOperatorsEnum::Equals, L"7", L"" },
                                                 { NumbersAndOperatorsEnum::None, L"", L"" } } };

    // Run the commands
    for (int i = 0; i < 2; i++)
    {
        ValidateViewModelByCommands(viewModels[i], scientificModeTestItems[i], true);
    }

    String ^ expression[] = { L"1   +   2 =", L"1   +   2   " + UtfUtils::MUL + L"   3 =" };
    String ^ result[] = { L"3", L"7" };

    // Assert for the history list items of the instances
    for (int i = 0; i < 2; i++)
    {
        VERIFY_IS_TRUE(1 == viewModels[i]->HistoryVM->ItemsCount);

        auto item = static_cast<HistoryItemViewModel ^>(viewModels[i]->HistoryVM->Items->GetAt(0));

        VERIFY_ARE_EQUAL(expression[i], item->Expression);
        VERIFY_ARE_EQUAL(result[i], item->Result);
    }
}

// Perform calculations on 3 separate instances of Calculator and verify that their Memory List items are maintained separately
TEST_METHOD(MultipleModesMemoryAddItemTest)
{
    std::vector<StandardCalculatorViewModel ^> viewModels(3);

    // Create 3 instances of StandardCalculatorViewModel in Standard and Scientific mode
    for (int i = 0; i < 3; i++)
    {
        viewModels[i] = ref new StandardCalculatorViewModel();

        ChangeMode(viewModels[i], i);

        // Validate that the history items list is initially empty
        VERIFY_IS_TRUE(0 == viewModels[i]->MemorizedNumbers->Size);
    }

    // Perform Calculations on both the instances and check that the Memory items work independently

    // Standard Mode: Expression 1+2=
    TESTITEM standardModeTestItems[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                         { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                         { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                         { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                         { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[0], standardModeTestItems, true);

    // Scientific Mode: Expression 1+2*3
    TESTITEM scientificModeTestItems[] = { { NumbersAndOperatorsEnum::IsScientificMode, L"0", L"" }, { NumbersAndOperatorsEnum::One, L"1", L"" },
                                           { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },          { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                           { NumbersAndOperatorsEnum::Multiply, L"2", L"1 + 2 * " }, { NumbersAndOperatorsEnum::Three, L"3", L"1 + 2 * " },
                                           { NumbersAndOperatorsEnum::Equals, L"7", L"" },           { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[1], scientificModeTestItems, true);

    // Programmer Mode: Expression F
    TESTITEM programmerModeTestItems[] = { { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                                           { NumbersAndOperatorsEnum::F, L"F", L"" },
                                           { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[2], programmerModeTestItems, false);

    // Press the Memory Button to save Values to Memory
    for (int i = 0; i < 3; i++)
    {
        viewModels[i]->ButtonPressed->Execute(NumbersAndOperatorsEnum::Memory);
    }

    String ^ expectedMemoryValues[] = { L"3", L"7", L"F" };

    // Validate that only one item is present in the memory
    // Also assert for their value
    for (int i = 0; i < 3; i++)
    {
        VERIFY_IS_TRUE(1 == viewModels[i]->MemorizedNumbers->Size);
        auto memorySlot = viewModels[i]->MemorizedNumbers->GetAt(0);
        VERIFY_ARE_EQUAL(expectedMemoryValues[i], memorySlot->Value);
    }
}

TEST_METHOD(MultipleDateCalculatorTest)
{
    // TODO - MSFT 10331900, fix this test
    // This test mostly passes, but the last set of VERIFYs succeeds for
    // the check of viewModels[2]->StrDateResult when in a UTC- time,
    // however, both the expected and actual results are incorrect.  Test
    // needs to be updated with correct expected value and viewmodel needs
    // to be updated to calculate correct value.

    // DateCalculatorViewModel^ viewModels[4];

    //// Initialize the view models
    // for (int i = 0; i < 4; i++)
    //{
    //    auto vm = ref new DateCalculatorViewModel();
    //    vm->Initialize();

    //    viewModels[i] = vm;
    //}

    // viewModels[2]->IsDateDiffMode = false;
    // viewModels[3]->IsDateDiffMode = false;

    // viewModels[2]->IsAddMode = true;
    // viewModels[3]->IsAddMode = false;

    //// Verify the initialization
    // for (int i = 0; i < 2; i++)
    //{
    //    VERIFY_IS_TRUE(viewModels[i]->IsDateDiffMode);
    //    VERIFY_IS_TRUE(viewModels[i]->FromDate.UniversalTime != 0);
    //    VERIFY_IS_TRUE(viewModels[i]->ToDate.UniversalTime != 0);
    //    VERIFY_ARE_EQUAL(StringReference(L"Same dates"), viewModels[i]->StrDateDiffResult);
    //    VERIFY_IS_NULL(viewModels[i]->StrDateDiffResultInDays);
    //}

    // for (int i = 2; i < 4; i++)
    //{
    //    VERIFY_IS_FALSE(viewModels[i]->IsDateDiffMode);
    //    VERIFY_IS_TRUE(viewModels[i]->DaysOffset == 0);
    //    VERIFY_IS_TRUE(viewModels[i]->MonthsOffset == 0);
    //    VERIFY_IS_TRUE(viewModels[i]->YearsOffset == 0);
    //    VERIFY_IS_NOT_NULL(viewModels[i]->StrDateResult);
    //    VERIFY_IS_TRUE(StringReference(L"") != viewModels[i]->StrDateResult);
    //}

    // VERIFY_IS_TRUE(viewModels[2]->IsAddMode);
    // VERIFY_IS_FALSE(viewModels[3]->IsAddMode);

    //// Perform some calculations
    //// Diff in viewModels[0]
    // SYSTEMTIME date1, date2, resultDate;
    ///* 01-10-2015 */ date1.wDay = 1; date1.wMonth = 10; date1.wYear = 2015; date1.wDayOfWeek = 4; date1.wHour = 0; date1.wMinute = 0; date1.wSecond = 0;
    /// date1.wMilliseconds = 0;
    ///* 15-02-2016 */ date2.wDay = 15; date2.wMonth = 2; date2.wYear = 2016; date2.wDayOfWeek = 1; date2.wHour = 0; date2.wMinute = 0; date2.wSecond = 0;
    /// date2.wMilliseconds = 0;

    // viewModels[0]->FromDate = DateUtils::SystemTimeToDateTime(date1);
    // viewModels[0]->ToDate = DateUtils::SystemTimeToDateTime(date2);

    //// Diff in viewModels[1]
    ///* 12-12-2015 */ date1.wDay = 12; date1.wMonth = 12; date1.wYear = 2015; date1.wDayOfWeek = 6;
    ///* 15-12-2015 */ date2.wDay = 15; date2.wMonth = 12; date2.wYear = 2015; date2.wDayOfWeek = 2;
    ///* 17-01-2018 */ resultDate.wDay = 17; resultDate.wMonth = 1; resultDate.wYear = 2018; resultDate.wDayOfWeek = 3; resultDate.wHour = 0; resultDate.wMinute
    ///= 0; resultDate.wSecond = 0; resultDate.wMilliseconds = 0;

    // viewModels[1]->FromDate = DateUtils::SystemTimeToDateTime(date1);
    // viewModels[1]->ToDate = DateUtils::SystemTimeToDateTime(date2);

    //// Add in viewModels[2]
    // viewModels[2]->StartDate = DateUtils::SystemTimeToDateTime(date1);
    // viewModels[2]->DaysOffset = 5;
    // viewModels[2]->MonthsOffset = 1;
    // viewModels[2]->YearsOffset = 2;

    //// Subtract OOB in viewModels[3]
    // viewModels[3]->StartDate = DateUtils::SystemTimeToDateTime(date2);
    // viewModels[3]->DaysOffset = 5;
    // viewModels[3]->MonthsOffset = 1;
    // viewModels[3]->YearsOffset = 500;

    //// Assert for the result
    // VERIFY_IS_FALSE(viewModels[0]->IsDiffInDays);
    // VERIFY_ARE_EQUAL(StringReference(L"137 days"), viewModels[0]->StrDateDiffResultInDays);
    // VERIFY_ARE_EQUAL(StringReference(L"4 months, 2 weeks"), viewModels[0]->StrDateDiffResult);

    // VERIFY_IS_TRUE(viewModels[1]->IsDiffInDays);
    // VERIFY_ARE_EQUAL(StringReference(L"3 days"), viewModels[1]->StrDateDiffResult);
    // VERIFY_IS_NULL(viewModels[1]->StrDateDiffResultInDays);

    //// TODO - MSFT 10331900 : both GetLongDate and viewmodel return incorrect values!
    // VERIFY_ARE_EQUAL(DateUtils::GetLongDate(resultDate), viewModels[2]->StrDateResult);
    // VERIFY_ARE_EQUAL(StringReference(L"Date out of Bound"), viewModels[3]->StrDateResult);
}

TEST_METHOD(InitializeMultipleConverterTest)
{
    std::shared_ptr<UnitConverterMock> unitConverterMocks[3];
    UnitConverterViewModel ^ viewModels[3];

    for (int i = 0; i < 3; i++)
    {
        unitConverterMocks[i] = std::make_shared<UnitConverterMock>();
        viewModels[i] = ref new UnitConverterViewModel(unitConverterMocks[i]);
        IObservableVector<Category ^> ^ cats = viewModels[i]->Categories;
        VERIFY_ARE_EQUAL((UINT)1, unitConverterMocks[i]->m_getCategoriesCallCount);
        VERIFY_ARE_EQUAL((UINT)4, cats->Size);
        // Verify that we match current category
        VERIFY_IS_TRUE(CAT2 == viewModels[i]->CurrentCategory->GetModelCategory());
    }

    // Change category of 1st instance to CAT1 and that of 2nd instance to CAT2
    viewModels[0]->CurrentCategory = viewModels[0]->Categories->GetAt(0);
    viewModels[2]->CurrentCategory = viewModels[1]->Categories->GetAt(2);

    // Verify that the instance properties were set independently
    for (int i = 0; i < 2; i++)
    {
        VERIFY_ARE_EQUAL((UINT)4, viewModels[i]->Categories->Size);
        VERIFY_ARE_EQUAL((UINT)3, viewModels[i]->Units->Size);
    }

    VERIFY_IS_TRUE(CAT1 == viewModels[0]->CurrentCategory->GetModelCategory());
    VERIFY_IS_TRUE(CAT2 == viewModels[1]->CurrentCategory->GetModelCategory());
    VERIFY_IS_TRUE(CAT3 == viewModels[2]->CurrentCategory->GetModelCategory());

    VERIFY_IS_TRUE(UNIT1 == viewModels[0]->Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT2 == viewModels[0]->Unit2->GetModelUnit());

    VERIFY_IS_TRUE(UNIT4 == viewModels[1]->Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT6 == viewModels[1]->Unit2->GetModelUnit());

    VERIFY_IS_TRUE(UNIT9 == viewModels[2]->Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT7 == viewModels[2]->Unit2->GetModelUnit());
}

TEST_METHOD(MultipleConverterModeCalculationTest)
{
    UnitConverterViewModel ^ viewModels[3];
    ResourceLoader ^ resLoader = ResourceLoader::GetForViewIndependentUse("Test");

    for (int i = 0; i < 3; i++)
    {
        viewModels[i] = ref new UnitConverterViewModel(
            std::make_shared<UnitConversionManager::UnitConverter>(std::make_shared<UnitConverterDataLoader>(ref new GeographicRegion()), nullptr));
        IObservableVector<Category ^> ^ categories = viewModels[i]->Categories;
        VERIFY_ARE_EQUAL((UINT)13, categories->Size);
    }

    IObservableVector<Category ^> ^ categoryList = viewModels[0]->Categories;
    IObservableVector<Unit ^> ^ unitsList[3];

    // viewModels 0 & 1 have same category(Volume) and viewModel 2 has different category(Length)
    int volumeIndex = NavCategory::GetIndexInGroup(ViewMode::Volume, CategoryGroupType::Converter);
    int lengthIndex = NavCategory::GetIndexInGroup(ViewMode::Length, CategoryGroupType::Converter);
    viewModels[0]->CurrentCategory = categoryList->GetAt(volumeIndex);
    viewModels[1]->CurrentCategory = categoryList->GetAt(volumeIndex);
    viewModels[2]->CurrentCategory = categoryList->GetAt(lengthIndex);

    for (int i = 0; i < 3; i++)
    {
        unitsList[i] = viewModels[i]->Units;
    }

    // Milliliters
    viewModels[0]->Unit1 = unitsList[0]->GetAt(0);
    // Cubic centimeters
    viewModels[0]->Unit2 = unitsList[0]->GetAt(1);

    // Liters
    viewModels[1]->Unit1 = unitsList[1]->GetAt(2);
    // Cubic meters
    viewModels[1]->Unit2 = unitsList[1]->GetAt(3);

    // Nanometers
    viewModels[2]->Unit1 = unitsList[2]->GetAt(0);
    // Microns
    viewModels[2]->Unit2 = unitsList[2]->GetAt(1);

    // Check that the units in multiple instances got set correctly
    VERIFY_IS_TRUE(unitsList[0]->GetAt(0) == viewModels[0]->Unit1);
    VERIFY_IS_TRUE(unitsList[0]->GetAt(1) == viewModels[0]->Unit2);

    VERIFY_IS_TRUE(unitsList[1]->GetAt(2) == viewModels[1]->Unit1);
    VERIFY_IS_TRUE(unitsList[1]->GetAt(3) == viewModels[1]->Unit2);

    VERIFY_IS_TRUE(unitsList[2]->GetAt(0) == viewModels[2]->Unit1);
    VERIFY_IS_TRUE(unitsList[2]->GetAt(1) == viewModels[2]->Unit2);

    // Perform conversions
    viewModels[0]->ButtonPressed->Execute(NumbersAndOperatorsEnum::One);

    viewModels[1]->ButtonPressed->Execute(NumbersAndOperatorsEnum::Two);

    viewModels[2]->ButtonPressed->Execute(NumbersAndOperatorsEnum::Three);

    // Validate Value1 and Value2 which is the result
    for (int i = 0; i < 3; i++)
    {
        auto expectedValue1 = (i + 1).ToString();
        auto actualValue1 = viewModels[i]->Value1;

        VERIFY_ARE_EQUAL(expectedValue1, actualValue1);

        std::wstring unit1Name = viewModels[i]->Unit1->Name->Data();
        std::wstring unit2Name = viewModels[i]->Unit2->Name->Data();

        auto resKey = String::Concat(ref new String((unit1Name + L"-" + unit2Name + L"-").c_str()), expectedValue1);
        String ^ expectedResult = resLoader->GetString(resKey);

        String ^ actualResult = viewModels[i]->Value2;

        VERIFY_ARE_EQUAL(expectedResult, actualResult);
    }
}

TEST_METHOD(TestStandardUnitConverterAndDateViewModels)
{
    // Create instances of SCVM in Standard Mode and UnitConverterViewModel
    StandardCalculatorViewModel ^ standardViewModel = ref new StandardCalculatorViewModel();
    UnitConverterViewModel ^ unitConverterViewModel = ref new UnitConverterViewModel(
        std::make_shared<UnitConversionManager::UnitConverter>(std::make_shared<UnitConverterDataLoader>(ref new GeographicRegion()), nullptr));
    DateCalculatorViewModel ^ dateCalcViewModel = ref new DateCalculatorViewModel();

    // Initialize Standard Calculator
    ChangeMode(standardViewModel, 0);

    // Initialize Date Calculator
    dateCalcViewModel->IsDateDiffMode = false;
    dateCalcViewModel->IsAddMode = true;

    // Initialize Unit Converter
    int volumeCategoryIndex = NavCategory::GetIndexInGroup(ViewMode::Volume, CategoryGroupType::Converter);
    IObservableVector<Category ^> ^ categories = unitConverterViewModel->Categories;
    unitConverterViewModel->CurrentCategory = categories->GetAt(volumeCategoryIndex);
    unitConverterViewModel->Unit1 = unitConverterViewModel->Units->GetAt(2);
    unitConverterViewModel->Unit2 = unitConverterViewModel->Units->GetAt(3);

    // Validate Calculator mode and UC Category and Units
    ValidateViewModelMode(standardViewModel, 0);

    VERIFY_ARE_EQUAL(categories->GetAt(volumeCategoryIndex), unitConverterViewModel->CurrentCategory);
    VERIFY_ARE_EQUAL(unitConverterViewModel->Units->GetAt(2), unitConverterViewModel->Unit1);
    VERIFY_ARE_EQUAL(unitConverterViewModel->Units->GetAt(3), unitConverterViewModel->Unit2);

    // Perform Calculations on these instances and check that they work independently

    // Standard Mode 1: Expression 3-2=
    TESTITEM standardModeTestItems1[] = { { NumbersAndOperatorsEnum::Three, L"3", L"" },
                                          { NumbersAndOperatorsEnum::Subtract, L"3", L"3 - " },
                                          { NumbersAndOperatorsEnum::Two, L"2", L"3 - " },
                                          { NumbersAndOperatorsEnum::Equals, L"1", L"" },
                                          { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(standardViewModel, standardModeTestItems1, true);

    SYSTEMTIME startDate, endDate;
    /* 01-10-2015 */ startDate.wDay = 1;
    startDate.wMonth = 10;
    startDate.wYear = 2015;
    startDate.wDayOfWeek = 4;
    startDate.wHour = 0;
    startDate.wMinute = 0;
    startDate.wSecond = 0;
    startDate.wMilliseconds = 0;
    /* 02-12-2018 */ endDate.wDay = 2;
    endDate.wMonth = 12;
    endDate.wYear = 2018;
    endDate.wDayOfWeek = 0;
    endDate.wHour = 0;
    endDate.wMinute = 0;
    endDate.wSecond = 0;
    endDate.wMilliseconds = 0;

    dateCalcViewModel->StartDate = DateUtils::SystemTimeToDateTime(startDate);
    dateCalcViewModel->DaysOffset = 1;
    dateCalcViewModel->MonthsOffset = 2;
    dateCalcViewModel->YearsOffset = 3;

    unitConverterViewModel->ButtonPressed->Execute(NumbersAndOperatorsEnum::Two);

    // Validate the Result
    VERIFY_ARE_EQUAL(StringReference(L"2"), unitConverterViewModel->Value1);
    VERIFY_ARE_EQUAL(StringReference(L"0.002"), unitConverterViewModel->Value2);

    // Assert that the Display Value of Standard Calc instance is unchanged
    VERIFY_ARE_EQUAL(standardViewModel->DisplayValue, StringReference(L"1"));

    // Again perform calculations on Standard Calc instance and validate that the Converter remains unaffected

    // Standard Mode: Expression 1+2=
    TESTITEM standardModeTestItems2[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },
                                          { NumbersAndOperatorsEnum::Add, L"1", L"1 + " },
                                          { NumbersAndOperatorsEnum::Two, L"2", L"1 + " },
                                          { NumbersAndOperatorsEnum::Equals, L"3", L"" },
                                          { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(standardViewModel, standardModeTestItems2, true);
    VERIFY_ARE_EQUAL(StringReference(L"3"), standardViewModel->DisplayValue);

    // Validate the Date Calculator
    VERIFY_ARE_EQUAL(DateUtils::GetLongDate(endDate), dateCalcViewModel->StrDateResult);

    // Validate the Unit Converter
    VERIFY_ARE_EQUAL(StringReference(L"2"), unitConverterViewModel->Value1);
    VERIFY_ARE_EQUAL(StringReference(L"0.002"), unitConverterViewModel->Value2);
}

// Change the radix in programmer mode and check that other modes are not affected
TEST_METHOD(MultipleModesWithChangeInProgrammerRadix)
{
    std::vector<StandardCalculatorViewModel ^> viewModels(3);

    // Create 2 instances of StandardCalculatorViewModel in Standard and Programmer modes
    viewModels[0] = ref new StandardCalculatorViewModel();
    ChangeMode(viewModels[0], 0);

    viewModels[1] = ref new StandardCalculatorViewModel();
    ChangeMode(viewModels[1], 2);

    // Change the programmer mode radix to hex
    TESTITEM programmerModeTestItems[] = { { NumbersAndOperatorsEnum::HexButton, L"0", L"" },
                                           { NumbersAndOperatorsEnum::F, L"F", L"" },
                                           { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[1], programmerModeTestItems, false /*doReset*/);
    VERIFY_ARE_EQUAL(viewModels[1]->HexDisplayValue, StringReference(L"F"));

    // Standard Mode: Expression 10+2=
    // Radix should be decimal, not hex
    TESTITEM standardModeTestItems[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },       { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                         { NumbersAndOperatorsEnum::Add, L"10", L"10 + " }, { NumbersAndOperatorsEnum::Two, L"2", L"10 + " },
                                         { NumbersAndOperatorsEnum::Equals, L"12", L"" },   { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[0], standardModeTestItems, false /*doReset*/);

    // Launch a new instance in standard mode
    viewModels[2] = ref new StandardCalculatorViewModel();
    ChangeMode(viewModels[2], 0);

    // Standard Mode: Expression 10+2=
    // Radix will be decimal by default
    TESTITEM standardModeTestItemsNew[] = { { NumbersAndOperatorsEnum::One, L"1", L"" },       { NumbersAndOperatorsEnum::Zero, L"10", L"" },
                                            { NumbersAndOperatorsEnum::Add, L"10", L"10 + " }, { NumbersAndOperatorsEnum::Two, L"2", L"10 + " },
                                            { NumbersAndOperatorsEnum::Equals, L"12", L"" },   { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[2], standardModeTestItemsNew, false /*doReset*/);

    // Radix in the programmer mode launched should still be hex.
    // A + 1 = B
    TESTITEM programmerModeTestItemsNew[] = { { NumbersAndOperatorsEnum::A, L"A", L"" },
                                              { NumbersAndOperatorsEnum::Add, L"A", L"A + " },
                                              { NumbersAndOperatorsEnum::One, L"1", L"" },
                                              { NumbersAndOperatorsEnum::Equals, L"B", L"" },
                                              { NumbersAndOperatorsEnum::None, L"", L"" } };
    ValidateViewModelByCommands(viewModels[1], programmerModeTestItemsNew, true /*doReset*/);
    VERIFY_ARE_EQUAL(viewModels[1]->HexDisplayValue, StringReference(L"B"));
}
}
;
}
