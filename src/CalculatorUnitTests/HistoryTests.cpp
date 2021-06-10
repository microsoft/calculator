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
        TEST_METHOD(TestHistoryStandardOrderOfOperations_1);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_2);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_3);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_4);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_5);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_6);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_7);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_8);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_9);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_10);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_11);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_12);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_13);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_14);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_15);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_16);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_17);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_18);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_19);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_20);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_21);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_22);
        TEST_METHOD(TestHistoryStandardOrderOfOperations_23);
        TEST_METHOD(TestHistoryStandardOrderOfOperationsMultiple);

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

        void HistoryStandardOrderOfOperationsHelper(String ^ expectedResult, String ^ expectedExpression, Command testCommands[])
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

        void HistoryStandardOrderOfOperations_1()
        {
            Command commands[] = { Command::Command1, Command::CommandREC, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_2()
        {
            Command commands[] = { Command::Command4, Command::CommandSQRT, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_3()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_4()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"3", L"1   +   \x221A( 4 ) =", commands);
        }

        void HistoryStandardOrderOfOperations_5()
        {
            Command commands[] = { Command::Command2, Command::CommandMUL, Command::Command4, Command::CommandREC, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_6()
        {
            Command commands[] = { Command::Command5, Command::CommandDIV, Command::Command6, Command::CommandPERCENT, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_7()
        {
            Command commands[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_8()
        {
            Command commands[] = { Command::Command7, Command::CommandSQR, Command::CommandDIV, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_9()
        {
            Command commands[] = { Command::Command8, Command::CommandSQR, Command::CommandSQRT, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_10()
        {
            Command commands[] = { Command::Command1, Command::Command0, Command::CommandADD, Command::Command2, Command::CommandSUB, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"12", L"10   +   2 =", commands);
        }

        void HistoryStandardOrderOfOperations_11()
        {
            Command commands[] = { Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandDIV, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"12", L"3   \x00D7   4 =", commands);
        }

        void HistoryStandardOrderOfOperations_12()
        {
            Command commands[] = { Command::Command6, Command::CommandDIV, Command::Command3, Command::CommandSUB, Command::CommandADD, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"2", L"6   \x00F7   3 =", commands);
        }

        void HistoryStandardOrderOfOperations_13()
        {
            Command commands[] = { Command::Command7, Command::CommandSUB, Command::Command4, Command::CommandDIV, Command::CommandMUL, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"3", L"7   -   4 =", commands);
        }

        void HistoryStandardOrderOfOperations_14()
        {
            Command commands[] = { Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandSQRT, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"16", L"8   \x00D7   2 =", commands);
        }

        void HistoryStandardOrderOfOperations_15()
        {
            Command commands[] = { Command::Command9, Command::CommandADD, Command::Command0, Command::CommandMUL, Command::CommandSIGN, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"9", L"9   +   0 =", commands);
        }

        void HistoryStandardOrderOfOperations_16()
        {
            Command commands[] = { Command::Command9, Command::CommandSIGN, Command::Command0, Command::CommandADD, Command::CommandMUL, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_17()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandEQU, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"3", L"1   +   2 =", commands);
        }

        void HistoryStandardOrderOfOperations_18()
        {
            Command commands[] = { Command::Command2, Command::Command0, Command::CommandMUL, Command::Command0, Command::Command2, Command::CommandEQU, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"40", L"20   \x00D7   2 =", commands);
        }

        void HistoryStandardOrderOfOperations_19()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandBACK, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"3", L"1   +   2 =", commands);
        }

        void HistoryStandardOrderOfOperations_20()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCLEAR, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"3", L"1   +   2 =", commands);
        }

        void HistoryStandardOrderOfOperations_21()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCENTR, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"3", L"1   +   2 =", commands);
        }

        void HistoryStandardOrderOfOperations_22()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCLEAR, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperations_23()
        {
            Command commands[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCENTR, Command::CommandNULL };
            HistoryStandardOrderOfOperationsHelper(L"", L"", commands);
        }

        void HistoryStandardOrderOfOperationsMultiple()
        {
            Initialize();
            Command commands[] = { Command::Command1,   Command::CommandMUL, Command::Command2,   Command::CommandMUL, Command::Command3,
                                   Command::CommandMUL, Command::Command4,   Command::CommandMUL, Command::Command5,   Command::CommandMUL, Command::CommandNULL };
            int initialSize = m_historyViewModel->ItemsCount;
            Command* currentCommand = commands;
            while (*currentCommand != Command::CommandNULL)
            {
                m_standardViewModel->SendCommandToCalcManager(static_cast<int>(*currentCommand++));
            }
            int sizeAfterCommandsAdd = m_historyViewModel->ItemsCount;
            VERIFY_ARE_EQUAL(initialSize + 4, sizeAfterCommandsAdd);
            auto historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(0));
            VERIFY_ARE_EQUAL(historyItem->Expression, L"24   \x00D7   5 =");
            VERIFY_ARE_EQUAL(historyItem->Result, L"120");
            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(1));
            VERIFY_ARE_EQUAL(historyItem->Expression, L"6   \x00D7   4 =");
            VERIFY_ARE_EQUAL(historyItem->Result, L"24");
            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(2));
            VERIFY_ARE_EQUAL(historyItem->Expression, L"2   \x00D7   3 =");
            VERIFY_ARE_EQUAL(historyItem->Result, L"6");
            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(3));
            VERIFY_ARE_EQUAL(historyItem->Expression, L"1   \x00D7   2 =");
            VERIFY_ARE_EQUAL(historyItem->Result, L"2");
            Cleanup();
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

    void HistoryTests::TestHistoryStandardOrderOfOperations_1()
    {
        HistoryStandardOrderOfOperations_1();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_2()
    {
        HistoryStandardOrderOfOperations_2();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_3()
    {
        HistoryStandardOrderOfOperations_3();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_4()
    {
        HistoryStandardOrderOfOperations_4();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_5()
    {
        HistoryStandardOrderOfOperations_5();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_6()
    {
        HistoryStandardOrderOfOperations_6();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_7()
    {
        HistoryStandardOrderOfOperations_7();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_8()
    {
        HistoryStandardOrderOfOperations_8();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_9()
    {
        HistoryStandardOrderOfOperations_9();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_10()
    {
        HistoryStandardOrderOfOperations_10();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_11()
    {
        HistoryStandardOrderOfOperations_11();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_12()
    {
        HistoryStandardOrderOfOperations_12();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_13()
    {
        HistoryStandardOrderOfOperations_13();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_14()
    {
        HistoryStandardOrderOfOperations_14();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_15()
    {
        HistoryStandardOrderOfOperations_15();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_16()
    {
        HistoryStandardOrderOfOperations_16();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_17()
    {
        HistoryStandardOrderOfOperations_17();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_18()
    {
        HistoryStandardOrderOfOperations_18();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_19()
    {
        HistoryStandardOrderOfOperations_19();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_20()
    {
        HistoryStandardOrderOfOperations_20();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_21()
    {
        HistoryStandardOrderOfOperations_21();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_22()
    {
        HistoryStandardOrderOfOperations_22();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperations_23()
    {
        HistoryStandardOrderOfOperations_23();
    }

    void HistoryTests::TestHistoryStandardOrderOfOperationsMultiple()
    {
        HistoryStandardOrderOfOperationsMultiple();
    }
}
