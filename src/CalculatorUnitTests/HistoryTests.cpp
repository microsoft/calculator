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
        TEST_METHOD(TestSaveAndReloadHistory);
        TEST_METHOD(TestSerializeDeSerializeHistoryItem);
        TEST_METHOD(TestHistoryItemWithPrettyExpressions);
        TEST_METHOD(TestHistoryItemWithPrettyExpressionsMixedRadix);
        TEST_METHOD(TestHistoryItemLoadAndContinueCalculation);
        TEST_METHOD(TestDisplayValueAutomationNames);
        TEST_METHOD(TestRadixAutomationName);
        TEST_METHOD(TestHistoryEmpty);

    private:
        HistoryViewModel^ m_historyViewModel;
        StandardCalculatorViewModel^ m_standardViewModel;

        void Initialize()
        {
            m_standardViewModel = ref new StandardCalculatorViewModel();
            m_standardViewModel->IsStandard = true;
            m_historyViewModel = ref new HistoryViewModel(&(m_standardViewModel->m_standardCalculatorManager));
            m_historyViewModel->SetCalculatorDisplay(m_standardViewModel->m_calculatorDisplay);
        }

        void Cleanup()
        {
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeBasic);
            m_historyViewModel->OnClearCommand(nullptr);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_historyViewModel->OnClearCommand(nullptr);
            m_standardViewModel->m_standardCalculatorManager.Reset();
        }

        bool IsHistoryContainerEmpty(_In_ String^ historyContainerKey)
        {
            ApplicationDataContainer^ localSettings = ApplicationData::Current->LocalSettings;
            return !(localSettings->Containers->HasKey(historyContainerKey));
        }

        String^ GetHistoryContainerKeyHelper(CalculationManager::CALCULATOR_MODE cMode)
        {
            ValueType^ modeValue = static_cast<int>(cMode);
            return String::Concat(modeValue->ToString(), L"_History");
        }

        void MockOnHistoryItemClicked(CalculatorApp::ViewModel::HistoryItemViewModel^ e)
        {
            m_standardViewModel->SetHistoryExpressionDisplay(e->GetTokens(), e->GetCommands());
            m_standardViewModel->SetExpressionDisplay(e->GetTokens(), e->GetCommands());
            m_standardViewModel->SetPrimaryDisplay(e->Result->Data(), false/*IsError*/);
            m_standardViewModel->IsFToEEnabled = false;
        }

        void AddSingleHistoryItem()
        {
            Initialize();
            int initialSize = m_historyViewModel->ItemSize;
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command8);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            int sizeAfterItemAdd = m_historyViewModel->ItemSize;
            auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            String^ expression = UtfUtils::LRO + L"1   +   8 =" + UtfUtils::PDF;
            String ^result = StringReference(L"9");
            VERIFY_ARE_EQUAL(initialSize + 1, sizeAfterItemAdd);
            VERIFY_ARE_EQUAL(expression, StringReference(historyItem->historyItemVector.expression.c_str()));
            VERIFY_ARE_EQUAL(result, StringReference(historyItem->historyItemVector.result.c_str()));
            Cleanup();
        }

        void AddMaxHistoryItems()
        {
            Initialize();
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            for (size_t i = 1; i < m_standardViewModel->m_standardCalculatorManager.MaxHistorySize(); i++)
            {
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command2);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            }
            VERIFY_ARE_EQUAL((size_t)m_historyViewModel->ItemSize, m_standardViewModel->m_standardCalculatorManager.MaxHistorySize());
            String ^expression = UtfUtils::LRO + L"1   +   1 =" + UtfUtils::PDF;
            int output = 2;
            String ^result = output.ToString();
            auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            VERIFY_ARE_EQUAL(expression, StringReference(historyItem->historyItemVector.expression.c_str()));
            VERIFY_ARE_EQUAL(result, StringReference(historyItem->historyItemVector.result.c_str()));
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command5);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            VERIFY_ARE_EQUAL((size_t)m_historyViewModel->ItemSize, m_standardViewModel->m_standardCalculatorManager.MaxHistorySize());
            expression = UtfUtils::LRO + L"1   +   2 =" + UtfUtils::PDF;
            output = 3;
            result = output.ToString();
            historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            VERIFY_ARE_EQUAL(expression, StringReference(historyItem->historyItemVector.expression.c_str()));
            VERIFY_ARE_EQUAL(result, StringReference(historyItem->historyItemVector.result.c_str()));
            Cleanup();
        }

        void ReloadHistory()
        {
            Initialize();

            m_standardViewModel->m_standardCalculatorManager.Reset();
            int scientificItems = 5;
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            for (int i = 0; i < scientificItems; i++)
            {
                Command nextCommand = Command(130 + i);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(nextCommand);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            }

            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeBasic);
            int standardItems = 2;
            for (int i = 0; i < standardItems; i++)
            {
                Command nextCommand = Command(130 + i);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(nextCommand);
                m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            }

            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_historyViewModel->ReloadHistory(ViewMode::Scientific);
            VERIFY_ARE_EQUAL(scientificItems, m_historyViewModel->ItemSize);
            for (int i = 0; i < scientificItems; i++)
            {
                wstring expr = L"1   +   " + wstring(i.ToString()->Data()) + L" =";
                expr = UtfUtils::LRO + expr + UtfUtils::PDF;
                int output = 1 + i;
                String ^result = output.ToString();
                auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(i);
                VERIFY_ARE_EQUAL(expr, historyItem->historyItemVector.expression);
                VERIFY_ARE_EQUAL(result, StringReference(historyItem->historyItemVector.result.c_str()));
            }

            m_historyViewModel->ReloadHistory(ViewMode::Standard);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeBasic);
            VERIFY_ARE_EQUAL(standardItems, m_historyViewModel->ItemSize);
            for (int i = 0; i < standardItems; i++)
            {
                wstring expr = L"1   +   " + wstring(i.ToString()->Data()) + L" =";
                expr = UtfUtils::LRO + expr + UtfUtils::PDF;
                int output = 1 + i;
                String ^result = output.ToString();
                auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(i);
                VERIFY_ARE_EQUAL(expr, historyItem->historyItemVector.expression);
                VERIFY_ARE_EQUAL(result, StringReference(historyItem->historyItemVector.result.c_str()));
            }
            Cleanup();
        }

        void ClearHistory()
        {
            Initialize();
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command2);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeBasic);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command2);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            m_historyViewModel->OnClearCommand(nullptr);
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemSize);
            VERIFY_IS_TRUE(IsHistoryContainerEmpty(GetHistoryContainerKeyHelper(CM_STD)));
            VERIFY_IS_TRUE(IsHistoryContainerEmpty(GetHistoryContainerKeyHelper(CM_SCI)));
            Cleanup();
        }

        void SerializeDeSerializeHistoryItem()
        {
            Initialize();
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command2);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            auto itemBeforeSerializeDeserialize = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            m_historyViewModel->SaveHistory();
            m_historyViewModel->ReloadHistory(ViewMode::Scientific);
            auto itemAfterSerializeDeserialize = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            VERIFY_IS_TRUE((itemBeforeSerializeDeserialize->historyItemVector.expression == itemAfterSerializeDeserialize->historyItemVector.expression) && (itemBeforeSerializeDeserialize->historyItemVector.result == itemAfterSerializeDeserialize->historyItemVector.result) && (itemBeforeSerializeDeserialize->historyItemVector.spCommands == itemAfterSerializeDeserialize->historyItemVector.spCommands) && (itemBeforeSerializeDeserialize->historyItemVector.spTokens == itemAfterSerializeDeserialize->historyItemVector.spTokens));
            Cleanup();
        }

        void SaveAndReloadHistory()
        {
            Initialize();
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command8);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command2);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeBasic);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command6);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            int itemsBeforeSaveAndReload = m_historyViewModel->ItemSize;
            m_historyViewModel->SaveHistory();
            m_historyViewModel->ReloadHistory(ViewMode::Scientific);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            wstring expr = L"1   +   8 =";
            // add double quotes around the expression
            expr = UtfUtils::LRO + expr + UtfUtils::PDF;
            String ^result = StringReference(L"9");
            int itemsAfterSaveAndReload = m_historyViewModel->ItemSize;
            auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);

            VERIFY_ARE_EQUAL(expr, historyItem->historyItemVector.expression);
            VERIFY_ARE_EQUAL(result, StringReference(historyItem->historyItemVector.result.c_str()));
            VERIFY_ARE_NOT_EQUAL(itemsBeforeSaveAndReload, itemsAfterSaveAndReload);
            VERIFY_ARE_EQUAL(itemsBeforeSaveAndReload, itemsAfterSaveAndReload + 1);
            Cleanup();
        }

        void HistoryItemWithPrettyExpressions()
        {
            Initialize();
            Command commands[] = { Command::CommandSIN, Command::CommandCOS, Command::CommandTAN, Command::CommandASIN, Command::CommandACOS, Command::CommandATAN };
            Command mode[] = { Command::CommandDEG, Command::CommandRAD, Command::CommandGRAD };
            int modes = sizeof(mode) / sizeof(Command);
            int commandsSize = sizeof(commands) / sizeof(Command);
            ResourceLoader^ m_uiResourceLoader = ResourceLoader::GetForViewIndependentUse(L"CEngineStrings");
            int itemIndex = 0;
            int commandResource = 67;
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            for (int index = 0; index < modes; index++)
            {
                m_standardViewModel->m_standardCalculatorManager.SendCommand(mode[index]);
                for (int command = 0; command < commandsSize; command++)
                {
                    m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
                    m_standardViewModel->m_standardCalculatorManager.SendCommand(commands[command]);
                    m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
                    auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(itemIndex);
                    String^ expression = m_uiResourceLoader->GetString(commandResource.ToString());
                    expression += L"( 1 ) =";
                    wstring expr = wstring(expression->Data());
                    expr = UtfUtils::LRO + expr + UtfUtils::PDF;
                    VERIFY_ARE_EQUAL(historyItem->historyItemVector.expression, expr);
                    commandResource++;
                    itemIndex++;
                }
            }
            Cleanup();
        }

        void HistoryItemWithPrettyExpressionsMixedRadix()
        {
            Initialize();
            ResourceLoader^ m_uiResourceLoader = ResourceLoader::GetForViewIndependentUse(L"CEngineStrings");
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandDEG);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandSIN);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandRAD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandSIN);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandGRAD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandSIN);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            String^ expression = m_uiResourceLoader->GetString(L"67");
            expression += L"( 1 )   +   ";
            expression += m_uiResourceLoader->GetString(L"73");
            expression += L"( 1 )   +   ";
            expression += m_uiResourceLoader->GetString(L"79");
            expression += L"( 1 ) =";
            wstring expr = wstring(expression->Data());
            expr = UtfUtils::LRO + expr + UtfUtils::PDF;
            VERIFY_ARE_EQUAL(historyItem->historyItemVector.expression, expr);

            Cleanup();
        }

        void HistoryItemClicked()
        {
            Initialize();
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command5);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command3);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            String^ expression = StringReference(historyItem->historyItemVector.expression.c_str());
            String^ result = StringReference(historyItem->historyItemVector.result.c_str());
            HistoryItemViewModel ^ item = ref new HistoryItemViewModel(expression, result, historyItem->historyItemVector.spTokens, historyItem->historyItemVector.spCommands);
            MockOnHistoryItemClicked(item);
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
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeBasic);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command5);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command3);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);

            auto historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            String^ expression = StringReference(historyItem->historyItemVector.expression.c_str());
            String^ result = StringReference(historyItem->historyItemVector.result.c_str());
            HistoryItemViewModel ^ item = ref new HistoryItemViewModel(expression, result, historyItem->historyItemVector.spTokens, historyItem->historyItemVector.spCommands);
            MockOnHistoryItemClicked(item);

            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command5);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            VERIFY_ARE_EQUAL(StringReference(L"14"), m_standardViewModel->DisplayValue);
            historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(0);
            expression = StringReference(historyItem->historyItemVector.expression.c_str());
            result = StringReference(historyItem->historyItemVector.result.c_str());
            item = ref new HistoryItemViewModel(expression, result, historyItem->historyItemVector.spTokens, historyItem->historyItemVector.spCommands);
            MockOnHistoryItemClicked(item);
            VERIFY_ARE_EQUAL(StringReference(L"9"), m_standardViewModel->DisplayValue);

            historyItem = m_standardViewModel->m_standardCalculatorManager.GetHistoryItem(1);
            expression = StringReference(historyItem->historyItemVector.expression.c_str());
            result = StringReference(historyItem->historyItemVector.result.c_str());
            item = ref new HistoryItemViewModel(expression, result, historyItem->historyItemVector.spTokens, historyItem->historyItemVector.spCommands);
            MockOnHistoryItemClicked(item);
            VERIFY_ARE_EQUAL(StringReference(L"14"), m_standardViewModel->DisplayValue);
            Cleanup();
        }

        void DisplayValueAutomationNames()
        {
            Initialize();
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command8);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            String ^expression = StringReference(L"Display is 9");
            VERIFY_ARE_EQUAL(expression, m_standardViewModel->CalculationResultAutomationName);

            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command5);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            expression = StringReference(L"Display is 6");
            VERIFY_ARE_EQUAL(expression, m_standardViewModel->CalculationResultAutomationName);

            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeProgrammer);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command2);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            expression = StringReference(L"Display is 3");
            VERIFY_ARE_EQUAL(expression, m_standardViewModel->CalculationResultAutomationName);

            Cleanup();
        }

        void RadixAutomationName()
        {
            Initialize();
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeProgrammer);
            m_standardViewModel->IsProgrammer = true;
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command1);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandADD);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::Command7);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::CommandEQU);
            String ^expression = L"HexaDecimal" + L" 8";
            String ^result = L"HexaDecimal " + Utils::GetStringValue(m_standardViewModel->HexDisplayValue);
            VERIFY_ARE_EQUAL(expression, result);
            expression = StringReference(L"Octal 10");
            result = L"Octal " + Utils::GetStringValue(m_standardViewModel->OctalDisplayValue);
            VERIFY_ARE_EQUAL(expression, result);
            expression = StringReference(L"Binary 1000");
            result = L"Binary " + Utils::GetStringValue(m_standardViewModel->BinaryDisplayValue);
            VERIFY_ARE_EQUAL(expression, result);
            Cleanup();
        }

        void HistoryEmpty()
        {
            Initialize();
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemSize);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemSize);
            Cleanup();
        }

        void HistoryClearCommandWithEmptyHistory()
        {
            Initialize();
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemSize);
            m_standardViewModel->m_standardCalculatorManager.SendCommand(Command::ModeScientific);
            m_historyViewModel->OnClearCommand(nullptr);
            VERIFY_ARE_EQUAL(0, m_historyViewModel->ItemSize);
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

    void HistoryTests::TestSerializeDeSerializeHistoryItem()
    {
        SerializeDeSerializeHistoryItem();
    }

    void HistoryTests::TestSaveAndReloadHistory()
    {
        SaveAndReloadHistory();
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
