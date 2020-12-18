// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

#include "CalcViewModel/HistoryViewModel.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

using namespace CalculationManager;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorUnitTests;
using namespace Platform;
using namespace std;
using namespace Windows::Storage;
using namespace Windows::ApplicationModel::Resources;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorFunctionalTests
{
    TEST_CLASS(HistoryTests)
    {
    public:
        TEST_METHOD(TestHistoryItemClicked);
        TEST_METHOD(TestHistoryItemAddSingleItem);
        TEST_METHOD(TestHistoryItemAddMaxItems);
        TEST_METHOD(TestHistoryClearCommand);
        TEST_METHOD(TestHistoryClearCommandWithEmptyHistory);
        TEST_METHOD(TestReLoadHistory);
        TEST_METHOD(TestHistoryItemWithPrettyExpressions);
        TEST_METHOD(TestHistoryItemWithPrettyExpressionsMixedRadix);
        TEST_METHOD(TestHistoryItemLoadAndContinueCalculation);
        TEST_METHOD(TestDisplayValueAutomationNames);
        TEST_METHOD(TestRadixAutomationName);
        TEST_METHOD(TestHistoryEmpty);
        TEST_METHOD(TestHistoryStandardOrderOfOperations);

    private:
        HistoryViewModel ^ m_historyViewModel;
        StandardCalculatorViewModel ^ m_standardViewModel;

        void Initialize()
        {
            m_standardViewModel = ref new StandardCalculatorViewModel();
            m_standardViewModel->IsStandard = true;
            m_historyViewModel = m_standardViewModel->HistoryVM;
        }

        void Cleanup()
        {
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeBasic));
            m_historyViewModel->OnClearCommand(nullptr);
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            m_historyViewModel->OnClearCommand(nullptr);
            m_standardViewModel->ResetCalcManager(false);
        }

        void AddSingleHistoryItem()
        {
            Initialize();
            int initialSize = m_historyViewModel->ItemsCount;
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command8));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            int sizeAfterItemAdd = m_historyViewModel->ItemsCount;
            auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(0));
            String ^ expression = L"1   +   8 =";
            VERIFY_ARE_EQUAL(initialSize + 1, sizeAfterItemAdd);
            VERIFY_ARE_EQUAL(historyItem->Expression, expression);
            VERIFY_ARE_EQUAL(historyItem->Result, L"9");
            Cleanup();
        }

        void AddMaxHistoryItems()
        {
            Initialize();
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            for (unsigned long long i = 1; i < m_historyViewModel->GetMaxItemSize(); i++)
            {
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command2));
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            }
            VERIFY_ARE_EQUAL((size_t)m_historyViewModel->ItemsCount, m_historyViewModel->GetMaxItemSize());
            String ^ expression = L"1   +   1 =";
            int output = 2;
            String ^ result = output.ToString();
            auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1));
            VERIFY_ARE_EQUAL(historyItem->Expression, expression);
            VERIFY_ARE_EQUAL(historyItem->Result, result);
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command5));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            expression = L"1   +   2 =";
            output = 3;
            result = output.ToString();
            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1));
            VERIFY_ARE_EQUAL(historyItem->Expression, expression);
            VERIFY_ARE_EQUAL(historyItem->Result, result);
            Cleanup();
        }

        void ReloadHistory()
        {
            Initialize();

            m_standardViewModel->ResetCalcManager(false);
            int scientificItems = 5;
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            for (int i = 0; i < scientificItems; i++)
            {
                Command nextCommand = Command(130 + i);
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
                m_standardViewModel->SendCommandToCalcManager((int)nextCommand);
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            }

            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeBasic));
            int standardItems = 2;
            for (int i = 0; i < standardItems; i++)
            {
                Command nextCommand = Command(130 + i);
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
                m_standardViewModel->SendCommandToCalcManager((int)nextCommand);
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            }

            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            m_historyViewModel->ReloadHistory(ViewMode::Scientific);
            VERIFY_ARE_EQUAL(scientificItems, m_historyViewModel->ItemsCount);
            for (int i = 0; i < scientificItems; i++)
            {
                wstring expr = L"1   +   " + wstring(i.ToString()->Data()) + L" =";
                int output = 1 + i;
                String ^ result = output.ToString();
                auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1 - i));
                VERIFY_ARE_EQUAL(historyItem->Expression, StringReference(expr.c_str()));
                VERIFY_ARE_EQUAL(historyItem->Result, result);
            }

            m_historyViewModel->ReloadHistory(ViewMode::Standard);
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeBasic));
            VERIFY_ARE_EQUAL(standardItems, m_historyViewModel->ItemsCount);
            for (int i = 0; i < standardItems; i++)
            {
                wstring expr = L"1   +   " + wstring(i.ToString()->Data()) + L" =";
                int output = 1 + i;
                String ^ result = output.ToString();
                auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1 - i));
                VERIFY_ARE_EQUAL(historyItem->Expression, StringReference(expr.c_str()));
                VERIFY_ARE_EQUAL(historyItem->Result, result);
            }
            Cleanup();
        }

        void ClearHistory()
        {
            Initialize();
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command2));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeBasic));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command2));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            m_historyViewModel->OnClearCommand(nullptr);
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemsCount);
            Cleanup();
        }

        void HistoryItemWithPrettyExpressions()
        {
            Initialize();
            Command commands[] = { Command::CommandSIN,  Command::CommandCOS,  Command::CommandTAN,
                                   Command::CommandASIN, Command::CommandACOS, Command::CommandATAN };
            Command mode[] = { Command::CommandDEG, Command::CommandRAD, Command::CommandGRAD };
            int modes = sizeof(mode) / sizeof(Command);
            int commandsSize = sizeof(commands) / sizeof(Command);
            ResourceLoader ^ m_uiResourceLoader = ResourceLoader::GetForViewIndependentUse(L"CEngineStrings");
            int itemIndex = 0;
            int commandResource = 67;
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            for (int index = 0; index < modes; index++)
            {
                m_standardViewModel->SendCommandToCalcManager((int)mode[index]);
                for (int command = 0; command < commandsSize; command++)
                {
                    m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
                    m_standardViewModel->SendCommandToCalcManager(static_cast<int>(commands[command]));
                    m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
                    auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1 - itemIndex));
                    String ^ expression = m_uiResourceLoader->GetString(commandResource.ToString());
                    expression += L"( 1 ) =";
                    wstring expr = wstring(expression->Data());
                    VERIFY_ARE_EQUAL(historyItem->Expression, StringReference(expr.c_str()));
                    commandResource++;
                    itemIndex++;
                }
            }
            Cleanup();
        }

        void HistoryItemWithPrettyExpressionsMixedRadix()
        {
            Initialize();
            ResourceLoader ^ m_uiResourceLoader = ResourceLoader::GetForViewIndependentUse(L"CEngineStrings");
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandDEG));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandSIN));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandRAD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandSIN));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandGRAD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandSIN));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1));
            String ^ expression = m_uiResourceLoader->GetString(L"67");
            expression += L"( 1 )   +   ";
            expression += m_uiResourceLoader->GetString(L"73");
            expression += L"( 1 )   +   ";
            expression += m_uiResourceLoader->GetString(L"79");
            expression += L"( 1 ) =";
            wstring expr = wstring(expression->Data());
            VERIFY_ARE_EQUAL(historyItem->Expression, StringReference(expr.c_str()));

            Cleanup();
        }

        void HistoryItemClicked()
        {
            Initialize();
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command5));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command3));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1));
            m_standardViewModel->SelectHistoryItem(historyItem);
            VERIFY_ARE_EQUAL(StringReference(L"9"), m_standardViewModel->DisplayValue);
            VERIFY_ARE_EQUAL(StringReference(L"1"), m_standardViewModel->ExpressionTokens->GetAt(0)->Token);
            VERIFY_ARE_EQUAL(StringReference(L" "), m_standardViewModel->ExpressionTokens->GetAt(1)->Token);
            VERIFY_ARE_EQUAL(StringReference(L"+"), m_standardViewModel->ExpressionTokens->GetAt(2)->Token);
            VERIFY_ARE_EQUAL(StringReference(L" "), m_standardViewModel->ExpressionTokens->GetAt(3)->Token);
            VERIFY_ARE_EQUAL(StringReference(L"5"), m_standardViewModel->ExpressionTokens->GetAt(4)->Token);
            VERIFY_ARE_EQUAL(StringReference(L" "), m_standardViewModel->ExpressionTokens->GetAt(5)->Token);
            VERIFY_ARE_EQUAL(StringReference(L"+"), m_standardViewModel->ExpressionTokens->GetAt(6)->Token);
            VERIFY_ARE_EQUAL(StringReference(L" "), m_standardViewModel->ExpressionTokens->GetAt(7)->Token);
            Cleanup();
        }

        void HistoryItemLoadAndContinueCalculation()
        {
            Initialize();
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeBasic));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command5));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command3));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));

            auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1));
            m_standardViewModel->SelectHistoryItem(historyItem);

            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command5));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            VERIFY_ARE_EQUAL(StringReference(L"11"), m_standardViewModel->DisplayValue);
            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1));
            m_standardViewModel->SelectHistoryItem(historyItem);
            VERIFY_ARE_EQUAL(StringReference(L"6"), m_standardViewModel->DisplayValue);

            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 2));
            m_standardViewModel->SelectHistoryItem(historyItem);
            VERIFY_ARE_EQUAL(StringReference(L"9"), m_standardViewModel->DisplayValue);
            Cleanup();
        }

        void DisplayValueAutomationNames()
        {
            Initialize();
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command8));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            String ^ expression = StringReference(L"Display is 9");
            VERIFY_ARE_EQUAL(expression, m_standardViewModel->CalculationResultAutomationName);

            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command5));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            expression = StringReference(L"Display is 6");
            VERIFY_ARE_EQUAL(expression, m_standardViewModel->CalculationResultAutomationName);

            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeProgrammer));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command2));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            expression = StringReference(L"Display is 3");
            VERIFY_ARE_EQUAL(expression, m_standardViewModel->CalculationResultAutomationName);

            Cleanup();
        }

        void RadixAutomationName()
        {
            Initialize();
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeProgrammer));
            m_standardViewModel->IsProgrammer = true;
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command1));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandADD));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::Command7));
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::CommandEQU));
            String ^ expression = L"HexaDecimal" + L" 8";
            String ^ result = L"HexaDecimal " + m_standardViewModel->HexDisplayValue;
            VERIFY_ARE_EQUAL(expression, result);
            expression = StringReference(L"Octal 10");
            result = L"Octal " + m_standardViewModel->OctalDisplayValue;
            VERIFY_ARE_EQUAL(expression, result);
            expression = StringReference(L"Binary 1000");
            result = L"Binary " + m_standardViewModel->BinaryDisplayValue;
            VERIFY_ARE_EQUAL(expression, result);
            Cleanup();
        }

        void HistoryEmpty()
        {
            Initialize();
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemsCount);
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemsCount);
            Cleanup();
        }

        void HistoryClearCommandWithEmptyHistory()
        {
            Initialize();
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemsCount);
            m_standardViewModel->SendCommandToCalcManager(static_cast<int>(Command::ModeScientific));
            m_historyViewModel->OnClearCommand(nullptr);
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemsCount);
            Cleanup();
        }

        void HistoryStandardOrderOfOperationsHelperSingle(String ^ expectedResult, String ^ expectedExpression, Command testCommands[])
        {
            Initialize();
            int initialSize = m_historyViewModel->ItemsCount;
            Command* currentCommand = testCommands;
            while (*currentCommand != Command::CommandNULL)
            {
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(*currentCommand++));
            }
            int sizeAfterCommandsAdd = m_historyViewModel->ItemsCount;
            if (expectedResult->IsEmpty())
            {
                VERIFY_ARE_EQUAL(initialSize, sizeAfterCommandsAdd);
            }
            else
            {
                auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(0));
                VERIFY_ARE_EQUAL(initialSize + 1, sizeAfterCommandsAdd);
                VERIFY_ARE_EQUAL(historyItem->Expression, expectedExpression);
                VERIFY_ARE_EQUAL(historyItem->Result, expectedResult);
            }
            Cleanup();
        }

        void HistoryStandardOrderOfOperationsHelperMultiple(String ^ expectedResults[], String ^ expectedExpressions[], int numResults, Command testCommands[])
        {
            Initialize();
            int initialSize = m_historyViewModel->ItemsCount;
            Command* currentCommand = testCommands;
            while (*currentCommand != Command::CommandNULL)
            {
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(*currentCommand++));
            }
            int sizeAfterCommandsAdd = m_historyViewModel->ItemsCount;
            VERIFY_ARE_EQUAL(initialSize + numResults, sizeAfterCommandsAdd);
            for (int i = 0; i < numResults; i++)
            {
                auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(i));
                VERIFY_ARE_EQUAL(historyItem->Expression, expectedExpressions[i]);
                VERIFY_ARE_EQUAL(historyItem->Result, expectedResults[i]);
            }
            Cleanup();
        }

        void HistoryStandardOrderOfOperations() {
            Command commands1[] = { Command::Command1, Command::CommandREC };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands1);

            Command commands2[] = { Command::Command4, Command::CommandSQRT };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands2);

            Command commands3[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands3);

            Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandSUB };
            HistoryStandardOrderOfOperationsHelperSingle(L"3", L"1   +   \x221A( 4 ) =", commands4);

            Command commands5[] = { Command::Command2, Command::CommandMUL, Command::Command4, Command::CommandREC };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands5);

            Command commands6[] = { Command::Command5, Command::CommandDIV, Command::Command6, Command::CommandPERCENT };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands6);

            Command commands7[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands7);

            Command commands8[] = { Command::Command7, Command::CommandSQR, Command::CommandDIV };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands8);

            Command commands9[] = { Command::Command8, Command::CommandSQR, Command::CommandSQRT };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands9);

            Command commands10[] = { Command::Command1, Command::Command0, Command::CommandADD, Command::Command2, Command::CommandSUB };
            HistoryStandardOrderOfOperationsHelperSingle(L"12", L"10   +   2 =", commands10);

            Command commands11[] = { Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandDIV };
            HistoryStandardOrderOfOperationsHelperSingle(L"12", L"3   \x00D7   4 =", commands11);

            Command commands12[] = { Command::Command6, Command::CommandDIV, Command::Command3, Command::CommandSUB, Command::CommandADD };
            HistoryStandardOrderOfOperationsHelperSingle(L"2", L"6   \x00F7   3 =", commands12);

            Command commands13[] = { Command::Command7, Command::CommandSUB, Command::Command4, Command::CommandDIV, Command::CommandMUL };
            HistoryStandardOrderOfOperationsHelperSingle(L"3", L"7   -   4 =", commands13);

            Command commands14[] = { Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandSQRT };
            HistoryStandardOrderOfOperationsHelperSingle(L"16", L"8   \x00D7   2 =", commands14);

            Command commands15[] = { Command::Command9, Command::CommandADD, Command::Command0, Command::CommandMUL, Command::CommandSIGN };
            HistoryStandardOrderOfOperationsHelperSingle(L"9", L"9   +   0 =", commands15);

            Command commands16[] = { Command::Command9, Command::CommandSIGN, Command::Command0, Command::CommandADD, Command::CommandMUL };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands16);

            Command commands17[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandEQU };
            HistoryStandardOrderOfOperationsHelperSingle(L"3", L"1   +   2 =", commands17);

            Command commands18[] = { Command::Command2, Command::Command0, Command::CommandMUL, Command::Command0, Command::Command2, Command::CommandEQU };
            HistoryStandardOrderOfOperationsHelperSingle(L"40", L"20   \x00D7   2 =", commands18);

            Command commands19[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandBACK };
            HistoryStandardOrderOfOperationsHelperSingle(L"3", L"1   +   2 =", commands19);

            Command commands20[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCLEAR };
            HistoryStandardOrderOfOperationsHelperSingle(L"3", L"1   +   2 =", commands20);

            Command commands21[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCENTR };
            HistoryStandardOrderOfOperationsHelperSingle(L"3", L"1   +   2 =", commands21);

            Command commands22[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCLEAR };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands22);

            Command commands23[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCENTR };
            HistoryStandardOrderOfOperationsHelperSingle(L"", L"", commands23);

            Command commands24[] = { Command::Command1, Command::CommandMUL, Command::Command2, Command::CommandMUL, Command::Command3,   Command::CommandMUL, Command::Command4, Command::CommandMUL, Command::Command5, Command::CommandMUL };
            String ^ expectedResults24[] = { L"120", L"24", L"6", L"2" };
            String ^ expectedExpressions24[] = { L"24   \x00D7   5 =", L"6   \x00D7   4 =", L"2   \x00D7   3 =", L"1   \x00D7   2 =" };
            HistoryStandardOrderOfOperationsHelperMultiple(expectedResults24, expectedExpressions24, 4, commands24);
        }
    };

    void HistoryTests::TestHistoryItemAddSingleItem()
    {
        AddSingleHistoryItem();
    }

    void HistoryTests::TestHistoryItemAddMaxItems()
    {
        AddMaxHistoryItems();
    }

    void HistoryTests::TestReLoadHistory()
    {
        ReloadHistory();
    }

    void HistoryTests::TestHistoryClearCommand()
    {
        ClearHistory();
    }

    void HistoryTests::TestHistoryItemWithPrettyExpressions()
    {
        HistoryItemWithPrettyExpressions();
    }

    void HistoryTests::TestHistoryItemWithPrettyExpressionsMixedRadix()
    {
        HistoryItemWithPrettyExpressionsMixedRadix();
    }

    void HistoryTests::TestHistoryItemClicked()
    {
        HistoryItemClicked();
    }

    void HistoryTests::TestHistoryItemLoadAndContinueCalculation()
    {
        HistoryItemLoadAndContinueCalculation();
    }

    void HistoryTests::TestDisplayValueAutomationNames()
    {
        DisplayValueAutomationNames();
    }

    void HistoryTests::TestRadixAutomationName()
    {
        RadixAutomationName();
    }

    void HistoryTests::TestHistoryEmpty()
    {
        HistoryEmpty();
    }

    void HistoryTests::TestHistoryClearCommandWithEmptyHistory()
    {
        HistoryClearCommandWithEmptyHistory();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations(){
        HistoryStandardOrderOfOperations();
    }
}
