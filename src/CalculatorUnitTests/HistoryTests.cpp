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

        bool IsHistoryContainerEmpty(_In_ String ^ historyContainerKey)
        {
            ApplicationDataContainer ^ localSettings = ApplicationData::Current->LocalSettings;
            return !(localSettings->Containers->HasKey(historyContainerKey));
        }

        String^ GetHistoryContainerKeyHelper(CalculationManager::CalculatorMode cMode)
        {
            ValueType^ modeValue = static_cast<int>(cMode);
            return String::Concat(modeValue->ToString(), L"_History");
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
            VERIFY_IS_TRUE(IsHistoryContainerEmpty(GetHistoryContainerKeyHelper(CalculatorMode::Standard)));
            VERIFY_IS_TRUE(IsHistoryContainerEmpty(GetHistoryContainerKeyHelper(CalculatorMode::Scientific)));
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
            VERIFY_ARE_EQUAL(StringReference(L"14"), m_standardViewModel->DisplayValue);
            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 1));
            m_standardViewModel->SelectHistoryItem(historyItem);
            VERIFY_ARE_EQUAL(StringReference(L"9"), m_standardViewModel->DisplayValue);

            historyItem = static_cast<HistoryItemViewModel ^>(m_historyViewModel->Items->GetAt(m_historyViewModel->ItemsCount - 2));
            m_standardViewModel->SelectHistoryItem(historyItem);
            VERIFY_ARE_EQUAL(StringReference(L"14"), m_standardViewModel->DisplayValue);
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
}
