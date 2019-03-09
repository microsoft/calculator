// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>

#include "CalcManager/CalculatorHistory.h"
#include "CalcViewModel/Common/EngineResourceProvider.h"

using namespace CalculatorApp;
using namespace CalculationManager;
using namespace Platform;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorManagerTest
{
    class CalculatorManagerDisplayTester : public ICalcDisplay
    {
    public:
        CalculatorManagerDisplayTester()
        {
            Reset();
        }

        void Reset()
        {
            m_isError = false;
            m_maxDigitsCalledCount = 0;
            m_binaryOperatorReceivedCallCount = 0;
        }

        void SetPrimaryDisplay(const wstring& text, bool isError) override
        {
            m_primaryDisplay = text;
            m_isError = isError;
        }
        void SetIsInError(bool isError) override
        {
            m_isError = isError;
        }
        void SetExpressionDisplay(_Inout_ std::shared_ptr<CalculatorVector<std::pair<std::wstring, int>>> const &tokens, _Inout_ std::shared_ptr<CalculatorVector<std::shared_ptr<IExpressionCommand>>> const & /*commands*/)
        {
            m_expression.clear();
            unsigned int nTokens = 0;
            std::pair <std::wstring, int> currentPair;
            tokens->GetSize(&nTokens);
            for (unsigned int i = 0; i < nTokens; ++i)
            {
                tokens->GetAt(i, &currentPair);
                m_expression += currentPair.first;
            }
        }
        void SetMemorizedNumbers(const vector<wstring>& numbers) override
        {
            m_memorizedNumberStrings = numbers;
        }

        void SetParenDisplayText(const std::wstring& parenthesisCount) override
        {
            m_parenDisplay = parenthesisCount;
        }

        const wstring& GetPrimaryDisplay() const
        {
            return m_primaryDisplay;
        }
        const wstring& GetExpression() const
        {
            return m_expression;
        }
        const vector<wstring>& GetMemorizedNumbers() const
        {
            return m_memorizedNumberStrings;
        }
        bool GetIsError() const
        {
            return m_isError;
        }

        void OnHistoryItemAdded(_In_ unsigned int /*addedItemIndex */)
        {
        }

        void MaxDigitsReached() override
        {
            m_maxDigitsCalledCount++;
        }

        int GetMaxDigitsCalledCount()
        {
            return m_maxDigitsCalledCount;
        }

        void BinaryOperatorReceived()
        {
            m_binaryOperatorReceivedCallCount++;
        }

        void MemoryItemChanged(unsigned int /*indexOfMemory*/)
        {
        }

        int GetBinaryOperatorReceivedCallCount()
        {
            return m_binaryOperatorReceivedCallCount;
        }

    private:
        wstring m_primaryDisplay;
        wstring m_expression;
        wstring m_parenDisplay;
        bool m_isError;
        vector<wstring> m_memorizedNumberStrings;
        int m_maxDigitsCalledCount;
        int m_binaryOperatorReceivedCallCount;
    };


    class TestDriver
    {
    private:
        static shared_ptr<CalculatorManagerDisplayTester> m_displayTester;
        static shared_ptr<CalculationManager::CalculatorManager> m_calculatorManager;

    public:
        static void Initialize(shared_ptr<CalculatorManagerDisplayTester> displayTester, shared_ptr<CalculatorManager> calculatorManager)
        {
            m_displayTester = displayTester;
            m_calculatorManager = calculatorManager;
        }

        static void Test(wstring expectedPrimary, wstring expectedExpression, Command testCommands[],
            bool cleanup = true, bool isScientific = false)
        {
            if (cleanup)
            {
                m_calculatorManager->Reset();
            }

            if (isScientific)
            {
                m_calculatorManager->SendCommand(Command::ModeScientific);
            }

            Command* currentCommand = testCommands;
            while (*currentCommand != Command::CommandNULL)
            {
                m_calculatorManager->SendCommand(*currentCommand++);
            }

            VERIFY_ARE_EQUAL(expectedPrimary, m_displayTester->GetPrimaryDisplay());
            if (expectedExpression != L"N/A")
            {
                VERIFY_ARE_EQUAL(expectedExpression, m_displayTester->GetExpression());
            }
        }
    };

    // Declare this class as a TestClass, and supply metadata if necessary.
    TEST_CLASS(CalculatorManagerTest)
    {
    public:
        TEST_CLASS_INITIALIZE(CommonSetup);

        TEST_METHOD(CalculatorManagerTestStandard);

        TEST_METHOD(CalculatorManagerTestScientific);
        TEST_METHOD(CalculatorManagerTestScientific2);
        TEST_METHOD(CalculatorManagerTestScientificParenthesis);
        TEST_METHOD(CalculatorManagerTestScientificError);
        TEST_METHOD(CalculatorManagerTestScientificModeChange);

        TEST_METHOD(CalculatorManagerTestModeChange);

        TEST_METHOD(CalculatorManagerTestMemory);

        TEST_METHOD(CalculatorManagerTestSerialize);
        TEST_METHOD(CalculatorManagerTestSerializePrecision);
        TEST_METHOD(CalculatorManagerTestSerializeMultiple);
        TEST_METHOD(CalculatorManagerTestSerializeDegreeMode);
        TEST_METHOD(CalculatorManagerTestSerializeMemory);

        TEST_METHOD(CalculatorManagerTestMaxDigitsReached);
        TEST_METHOD(CalculatorManagerTestMaxDigitsReached_LeadingDecimal);
        TEST_METHOD(CalculatorManagerTestMaxDigitsReached_TrailingDecimal);

        // TODO re-enable when cause of failure is determined. Bug 20226670
        //TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived);
        //TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived_Multiple);
        //TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived_LongInput);

        TEST_METHOD_CLEANUP(Cleanup);


    private:
        static std::shared_ptr<CalculatorManager> m_calculatorManager;
        static std::shared_ptr<IResourceProvider> m_resourceProvider;
        static std::shared_ptr<CalculatorManagerDisplayTester> m_calculatorDisplayTester;
        void ExecuteCommands(Command commands[]);
        void ExecuteCommands(const vector<Command>& commands);

        vector<Command> CommandListFromStringInput(const wstring& input)
        {
            vector<Command> result{};
            for (auto iter = input.begin(); iter != input.end(); iter++)
            {
                wchar_t ch = *iter;
                Command asCommand = Command::CommandNULL;
                if (ch == L'.')
                {
                    asCommand = Command::CommandPNT;
                }
                else if (L'0' <= ch && ch <= L'9')
                {
                    int diff = static_cast<int>(ch) - static_cast<int>(L'0');
                    asCommand = static_cast<Command>(diff + static_cast<int>(Command::Command0));
                }

                if (asCommand != Command::CommandNULL)
                {
                    result.push_back(asCommand);
                }
            }

            return result;
        }

        void TestMaxDigitsReachedScenario(const wstring& constInput)
        {
            CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester *)m_calculatorDisplayTester.get();

            // Make sure we're in a clean state.
            VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetMaxDigitsCalledCount());

            vector<Command> commands = CommandListFromStringInput(constInput);
            VERIFY_IS_FALSE(commands.empty());

            // The last element in the list should always cause MaxDigitsReached
            // Remember the command but remove from the actual input that is sent
            Command finalInput = commands[commands.size() - 1];
            commands.pop_back();
            wstring input = constInput.substr(0, constInput.length() - 1);

            m_calculatorManager->SetStandardMode();
            ExecuteCommands(commands);

            wstring expectedDisplay = input;
            wstring display = pCalculatorDisplay->GetPrimaryDisplay();
            VERIFY_ARE_EQUAL(expectedDisplay, display);

            m_calculatorManager->SendCommand(finalInput);

            // Verify MaxDigitsReached
            display = pCalculatorDisplay->GetPrimaryDisplay();
            VERIFY_ARE_EQUAL(expectedDisplay, display);

            // MaxDigitsReached should have been called once
            VERIFY_IS_LESS_THAN(0, pCalculatorDisplay->GetMaxDigitsCalledCount());
        }

        void SerializeAndDeSerialize()
        {
            auto serializedCommands = m_calculatorManager->SerializeCommands();
            auto serializedMemory = m_calculatorManager->GetSerializedMemory();
            auto serializedDisplay = m_calculatorManager->GetSerializedPrimaryDisplay();

            Cleanup();

            m_calculatorManager->DeSerializePrimaryDisplay(serializedDisplay);
            m_calculatorManager->DeSerializeMemory(serializedMemory);
            m_calculatorManager->DeSerializeCommands(serializedCommands);
        }

        void VerifyPersistence()
        {
            auto savedPrimary = m_calculatorDisplayTester->GetPrimaryDisplay();
            auto savedExpression = m_calculatorDisplayTester->GetExpression();
            auto savedMemory = m_calculatorDisplayTester->GetMemorizedNumbers();
            SerializeAndDeSerialize();
            VERIFY_ARE_EQUAL(savedPrimary, m_calculatorDisplayTester->GetPrimaryDisplay());
            VERIFY_ARE_EQUAL(savedExpression, m_calculatorDisplayTester->GetExpression());
            auto loadedMemory = m_calculatorDisplayTester->GetMemorizedNumbers();
            VERIFY_ARE_EQUAL(savedMemory.size(), loadedMemory.size());
            for (unsigned int i = 0; i < savedMemory.size(); i++)
            {
                VERIFY_ARE_EQUAL(savedMemory.at(i), loadedMemory.at(i));
            }
        }
    };

    std::shared_ptr<CalculatorManager> CalculatorManagerTest::m_calculatorManager;
    std::shared_ptr<CalculatorManagerDisplayTester> CalculatorManagerTest::m_calculatorDisplayTester;
    std::shared_ptr<IResourceProvider> CalculatorManagerTest::m_resourceProvider;
    std::shared_ptr<CalculatorManagerDisplayTester> TestDriver::m_displayTester;
    std::shared_ptr<CalculatorManager> TestDriver::m_calculatorManager;

    // Creates instance of CalculationManager before running tests
    void CalculatorManagerTest::CommonSetup()
    {
        m_calculatorDisplayTester = std::make_shared<CalculatorManagerDisplayTester>();
        m_resourceProvider = std::make_shared<EngineResourceProvider>();
        m_calculatorManager = std::make_shared<CalculatorManager>(m_calculatorDisplayTester.get(), m_resourceProvider.get());
        TestDriver::Initialize(m_calculatorDisplayTester, m_calculatorManager);
    }

    // Resets calculator state to start state after each test
    void CalculatorManagerTest::Cleanup()
    {
        m_calculatorManager->Reset();
        m_calculatorDisplayTester->Reset();
    }

    void CalculatorManagerTest::ExecuteCommands(Command commands[])
    {
        Command* itr = commands;
        while (*itr != Command::CommandNULL)
        {
            m_calculatorManager->SendCommand(*itr);
            itr++;
        }
    }

    void CalculatorManagerTest::ExecuteCommands(const vector<Command>& commands)
    {
        for (const Command& command : commands)
        {
            if (command == Command::CommandNULL)
            {
                break;
            }

            m_calculatorManager->SendCommand(command);
        }
    }

    void CalculatorManagerTest::CalculatorManagerTestStandard()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3,
            Command::CommandPNT, Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3);

        Command commands4[] = { Command::Command2, Command::CommandADD, Command::Command3,
            Command::CommandEQU, Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"7", L"", commands4);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"", commands5);

        Command commands6[] = { Command::Command2, Command::Command5, Command::Command6,
            Command::CommandSQRT, Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6);

        Command commands7[] = { Command::Command3, Command::CommandSUB, Command::Command6,
            Command::CommandEQU, Command::CommandMUL, Command::Command3,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"-9", L"", commands7);

        Command commands8[] = { Command::Command9, Command::CommandMUL, Command::Command6,
            Command::CommandSUB, Command::CommandCENTR, Command::Command8,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"46", L"", commands8);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6,
            Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0.36", L"", commands9);

        Command commands10[] = { Command::Command5, Command::Command0, Command::CommandADD,
            Command::Command2, Command::Command0, Command::CommandPERCENT,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"", commands10);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"", commands11);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL,
            Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN,
            Command::Command9, Command::Command9, Command::Command9,
            Command::Command9, Command::CommandDIV, Command::Command1,
            Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13);

        Command commands14[] = { Command::Command5, Command::Command0, Command::CommandADD,
            Command::Command2, Command::Command0, Command::CommandPERCENT,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"", commands14);

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16);

        Command commands17[] = { Command::Command1, Command::Command2, Command::CommandADD,
            Command::Command5, Command::CommandCENTR, Command::Command2,
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"12 + 2 + ", commands17);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0,
            Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3,
            Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19);

        Command commands20[] = { Command::Command1, Command::Command2, Command::Command3,
            Command::CommandBACK, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB,
            Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(4) - 2 + ", commands21);

        Command commands22[] = { Command::Command1, Command::Command0, Command::Command2, Command::Command4,
            Command::CommandSQRT, Command::CommandSUB, Command::Command3, Command::Command2,
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(1024) - 32 + ", commands22);
    }

    void CalculatorManagerTest::CalculatorManagerTestScientific()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3,
            Command::CommandPNT, Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1, true, true);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2, true, true);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3, true, true);

        Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command0,
            Command::CommandMUL, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands4, true, true);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"", commands5, true, true);

        Command commands6[] = { Command::Command2, Command::Command5, Command::Command6,
            Command::CommandSQRT, Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6, true, true);

        Command commands7[] = { Command::Command3, Command::CommandSUB, Command::Command6,
            Command::CommandEQU, Command::CommandMUL, Command::Command3, \
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"-9", L"-3 \x00D7 3 + ", commands7, true, true);

        Command commands8[] = { Command::Command9, Command::CommandMUL, Command::Command6,
            Command::CommandSUB, Command::CommandCENTR, Command::Command8,
            Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"38", L"9 \x00D7 6 - 8 \x00D7 2 + ", commands8, true, true);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6,
            Command::CommandSIGN, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"6 \x00D7 \x221A(-6)", commands9, true, true);

        Command commands10[] = { Command::Command5, Command::Command0, Command::CommandADD,
            Command::Command2, Command::Command0, Command::CommandREC,
            Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"50.05", L"50 + 1/(20) - ", commands10, true, true);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"", commands11, true, true);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL,
            Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12, true, true);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN,
            Command::Command9, Command::Command9, Command::Command9,
            Command::Command9, Command::CommandDIV, Command::Command1,
            Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13, true, true);

        Command commands14[] = { Command::Command5, Command::Command0, Command::CommandADD,
            Command::Command2, Command::Command0, Command::CommandPERCENT,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"", commands14, true, true);

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15, true, true);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16, true, true);

        Command commands17[] = { Command::Command1, Command::Command2, Command::CommandADD,
            Command::Command5, Command::CommandCENTR, Command::Command2,
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"12 + 2 + ", commands17, true, true);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0,
            Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18, true, true);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3,
            Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19, true, true);

        Command commands20[] = { Command::Command1, Command::Command2, Command::Command3,
            Command::CommandBACK, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20, true, true);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB,
            Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(4) - 2 + ", commands21);

        Command commands22[] = { Command::Command0, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands22);
                
        Command commands23[] = { Command::Command1, Command::Command0, Command::Command2, Command::Command4,
            Command::CommandSQRT, Command::CommandSUB, Command::Command3, Command::Command2,
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(1024) - 32 + ", commands23);

        Command commands24[] = { Command::Command2, Command::Command5, Command::Command7,
            Command::CommandSQRT, Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2.0009748976330773374220277351385", L"\x221A(\x221A(\x221A(257)))", commands24, true, true);
    }

    // Scientific functions from the scientific calculator
    void CalculatorManagerTest::CalculatorManagerTestScientific2()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::CommandSQR, Command::CommandNULL };
        TestDriver::Test(L"144", L"sqr(12)", commands1, true, true);

        Command commands2[] = { Command::Command5, Command::CommandFAC, Command::CommandNULL };
        TestDriver::Test(L"120", L"fact(5)", commands2, true, true);

        Command commands3[] = { Command::Command5, Command::CommandPWR, Command::Command2,
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"25", L"5 ^ 2 + ", commands3, true, true);

        Command commands4[] = { Command::Command8, Command::CommandROOT, Command::Command3,
            Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"2", L"8 yroot 3 \x00D7 ", commands4, true, true);

        Command commands5[] = { Command::Command8, Command::CommandCUB, Command::CommandNULL };
        TestDriver::Test(L"512", L"cube(8)", commands5, true, true);
/*
        Command commands6[] = { Command::Command8, Command::CommandCUB, Command::CommandCUBEROOT, Command::CommandNULL };
        TestDriver::Test(L"8", L"cuberoot(cube(8))", commands6, true, true);*/

        Command commands7[] = { Command::Command1, Command::Command0, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"1", L"log(10)", commands7, true, true);

        Command commands8[] = { Command::Command5, Command::CommandPOW10, Command::CommandNULL };
        TestDriver::Test(L"100,000", L"10^(5)", commands8, true, true);

        Command commands9[] = { Command::Command1, Command::Command0, Command::CommandLN, Command::CommandNULL };
        TestDriver::Test(L"2.3025850929940456840179914546844", L"ln(10)", commands9, true, true);

        Command commands10[] = { Command::Command1, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0.01745240643728351281941897851632", L"sin\x2080(1)", commands10, true, true);

        Command commands11[] = { Command::Command1, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"0.99984769515639123915701155881391", L"cos\x2080(1)", commands11, true, true);

        Command commands12[] = { Command::Command1, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0.01745506492821758576512889521973", L"tan\x2080(1)", commands12, true, true);

        Command commands13[] = { Command::Command1, Command::CommandASIN, Command::CommandNULL };
        TestDriver::Test(L"90", L"sin\x2080\x207B\x00B9(1)", commands13, true, true);

        Command commands14[] = { Command::Command1, Command::CommandACOS, Command::CommandNULL };
        TestDriver::Test(L"0", L"cos\x2080\x207B\x00B9(1)", commands14, true, true);

        Command commands15[] = { Command::Command1, Command::CommandATAN, Command::CommandNULL };
        TestDriver::Test(L"45", L"tan\x2080\x207B\x00B9(1)", commands15, true, true);

        Command commands16[] = { Command::Command2, Command::CommandPOWE, Command::CommandNULL };
        TestDriver::Test(L"7.389056098930650227230427460575", L"e^(2)", commands16, true, true);

        Command commands17[] = { Command::Command5, Command::CommandPWR, Command::Command0,
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1", L"5 ^ 0 + ", commands17);
        
        Command commands18[] = { Command::Command0, Command::CommandPWR, Command::Command0,
            Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"1", L"0 ^ 0 + ", commands18);
        
        Command commands19[] = { Command::Command2, Command::Command7, Command::CommandSIGN, Command::CommandROOT,
            Command::Command3, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"-3", L"-27 yroot 3 + ", commands19, true, true);

        Command commands20[] = { Command::Command8, Command::CommandPWR, Command::CommandOPENP,
            Command::Command2, Command::CommandDIV, Command::Command3, Command::CommandCLOSEP,
            Command::CommandSUB, Command::Command4, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"8 ^ (2 \x00F7 3) - 4 + ", commands20, true, true);

        Command commands21[] = { Command::Command4, Command::CommandPWR, Command::CommandOPENP,
            Command::Command3, Command::CommandDIV, Command::Command2, Command::CommandCLOSEP,
            Command::CommandSUB, Command::Command8, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"4 ^ (3 \x00F7 2) - 8 + ", commands21, true, true);

        Command commands22[] = { Command::Command1, Command::Command0, Command::CommandPWR, Command::Command1,
            Command::CommandPNT, Command::Command2, Command::Command3, Command::Command4, Command::Command5,
            Command::Command6, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"17.161687912241792074207286679393", L"10 ^ 1.23456 + ", commands22, true, true);
    }

    void CalculatorManagerTest::CalculatorManagerTestScientificParenthesis()
    {
        Command commands1[] = { Command::Command1, Command::CommandADD, Command::CommandOPENP,
            Command::CommandADD, Command::Command3, Command::CommandCLOSEP, Command::CommandNULL };
        TestDriver::Test(L"3", L"1 + (0 + 3)", commands1, true, true);

        Command commands2[] = { Command::CommandOPENP, Command::CommandOPENP, Command::Command1,
            Command::Command2, Command::CommandCLOSEP, Command::CommandNULL };
        TestDriver::Test(L"12", L"((12)", commands2, true, true);

        Command commands3[] = { Command::Command1, Command::Command2, Command::CommandCLOSEP,
            Command::CommandCLOSEP, Command::CommandOPENP, Command::CommandNULL };
        TestDriver::Test(L"12", L"(", commands3, true, true);

        Command commands4[] = { Command::Command2, Command::CommandOPENP, Command::Command2,
            Command::CommandCLOSEP, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"2", L"(2) + ", commands4, true, true);

        Command commands5[] = { Command::Command2, Command::CommandOPENP, Command::Command2,
            Command::CommandCLOSEP, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"", commands5, true, true);
    }

    void CalculatorManagerTest::CalculatorManagerTestScientificError()
    {
        Command commands1[] = { Command::Command1, Command::CommandDIV, Command::Command0,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands1, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands2[] = { Command::Command2, Command::CommandSIGN, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"log(-2)", commands2, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands3[] = { Command::Command0, Command::CommandDIV, Command::Command0,
            Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands3, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        // Do the same tests for the basic calculator
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands1);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
        TestDriver::Test(L"Invalid input", L"log(-2)", commands2);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands3);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
    }

    // Radians and Grads Test
    void CalculatorManagerTest::CalculatorManagerTestScientificModeChange()
    {
        Command commands1[] = { Command::CommandRAD, Command::CommandPI, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands1, true, true);

        Command commands2[] = { Command::CommandRAD, Command::CommandPI, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"-1", L"N/A", commands2, true, true);

        Command commands3[] = { Command::CommandRAD, Command::CommandPI, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands3, true, true);

        Command commands4[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0,
            Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands4, true, true);

        Command commands5[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0,
            Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"1", L"N/A", commands5, true, true);

        Command commands6[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0,
            Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands6, true, true);
    }

    void CalculatorManagerTest::CalculatorManagerTestModeChange()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands1, true, false);

        Command commands2[] = { Command::ModeScientific, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands2, true, false);

        Command commands3[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands3, true, false);

        Command commands4[] = { Command::ModeProgrammer, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands4, true, false);

        Command commands5[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"123", L"", commands5, true, false);

        Command commands6[] = { Command::ModeScientific, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands6, true, false);

        Command commands7[] = { Command::Command6, Command::Command7, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"67", L"67 + ", commands7, true, false);

        Command commands8[] = { Command::ModeBasic, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands8, true, false);
    }

    void CalculatorManagerTest::CalculatorManagerTestMemory()
    {
        Command scientificCalculatorTest52[] = { Command::Command1, Command::CommandSTORE, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific52(L"1");
        wstring expectedExpressionDisplayTestScientific52(L"");

        Command scientificCalculatorTest53[] = { Command::Command1, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific53(L"1");
        wstring expectedExpressionDisplayTestScientific53(L"");

        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester *)m_calculatorDisplayTester.get();
        wstring resultPrimary = L"";
        wstring resultExpression = L"";

        Cleanup();
        ExecuteCommands(scientificCalculatorTest52);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        resultExpression = pCalculatorDisplay->GetExpression();
        VERIFY_ARE_EQUAL(expectedPrimaryDisplayTestScientific52, resultPrimary);

        Cleanup();
        ExecuteCommands(scientificCalculatorTest53);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->MemorizedNumberLoad(0);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        resultExpression = pCalculatorDisplay->GetExpression();
        VERIFY_ARE_EQUAL(expectedPrimaryDisplayTestScientific52, resultPrimary);

        Cleanup();
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->MemorizedNumberLoad(1);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(wstring(L"1"), resultPrimary);

        m_calculatorManager->MemorizedNumberLoad(0);
        resultPrimary = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(wstring(L"2"), resultPrimary);

        Cleanup();
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandADD);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::CommandEQU);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandMUL);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();

        vector<wstring> memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        vector<wstring> expectedMemorizedNumbers;
        expectedMemorizedNumbers.push_back(L"2");
        expectedMemorizedNumbers.push_back(L"1");
        expectedMemorizedNumbers.push_back(L"-1");


        bool isEqual = false;
        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizedNumberAdd(0);
        m_calculatorManager->MemorizedNumberAdd(1);
        m_calculatorManager->MemorizedNumberAdd(2);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        expectedMemorizedNumbers.clear();
        expectedMemorizedNumbers.push_back(L"4");
        expectedMemorizedNumbers.push_back(L"3");
        expectedMemorizedNumbers.push_back(L"1");

        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandPNT);
        m_calculatorManager->SendCommand(Command::Command5);

        m_calculatorManager->MemorizedNumberSubtract(0);
        m_calculatorManager->MemorizedNumberSubtract(1);
        m_calculatorManager->MemorizedNumberSubtract(2);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();

        expectedMemorizedNumbers.clear();
        expectedMemorizedNumbers.push_back(L"2.5");
        expectedMemorizedNumbers.push_back(L"1.5");
        expectedMemorizedNumbers.push_back(L"-0.5");

        if (memorizedNumbers.size() < expectedMemorizedNumbers.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), expectedMemorizedNumbers.begin());
        }
        else
        {
            isEqual = std::equal(expectedMemorizedNumbers.begin(), expectedMemorizedNumbers.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        // Memorizing 101 numbers, which exceeds the limit.
        Cleanup();
        for (int i = 0; i < 101; i++)
        {
            m_calculatorManager->SendCommand(Command::Command1);
            m_calculatorManager->MemorizeNumber();
        }

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        VERIFY_ARE_EQUAL((size_t)100, memorizedNumbers.size());

        // Memorizing new number, which should show up at the top of the memory
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();
        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        VERIFY_ARE_EQUAL(wstring(L"2"), memorizedNumbers.at(0));

        // Test for trying to memorize invalid value 
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->SendCommand(Command::CommandSQRT);
        m_calculatorManager->MemorizeNumber();
    }

    void CalculatorManagerTest::CalculatorManagerTestSerializeMemory()
    {
        Cleanup();

        Command commands[] = { Command::Command1, Command::CommandNULL };
        ExecuteCommands(commands);

        for (int i = 0; i < 110; i++)
        {
            m_calculatorManager->MemorizeNumber();
        }

        VerifyPersistence();
    }

    void CalculatorManagerTest::CalculatorManagerTestSerializeDegreeMode()
    {
        Cleanup();

        Command commands[] = { Command::Command1, Command::CommandRAD, Command::CommandSIN, Command::CommandADD,
            Command::Command1, Command::CommandGRAD, Command::CommandCOS, Command::CommandADD,
            Command::Command1, Command::CommandDEG, Command::CommandTAN, Command::CommandADD,
            Command::CommandNULL };
        ExecuteCommands(commands);

        VerifyPersistence();
    }

    // 1 + 2 then serialize and deserialize 3 times
    // Check if the values are persisted correctly
    void CalculatorManagerTest::CalculatorManagerTestSerializeMultiple()
    {
        Cleanup();

        Command commands[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandNULL };
        ExecuteCommands(commands);

        VerifyPersistence();
        VerifyPersistence();
        VerifyPersistence();
    }

    // Calculate 1/3 then serialize and deserialize
    // Multiply by 3 and check if the result is 1 not 0.999999999999999999...
    void CalculatorManagerTest::CalculatorManagerTestSerializePrecision()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester *)m_calculatorDisplayTester.get();
        wstring resultPrimary = L"";
        wstring resultExpression = L"";
        Cleanup();

        Command commands[] = { Command::Command1, Command::CommandDIV, Command::Command3, Command::CommandEQU, Command::CommandNULL };
        ExecuteCommands(commands);

        SerializeAndDeSerialize();

        Command commands2[] = { Command::CommandMUL, Command::Command3, Command::CommandEQU, Command::CommandNULL };
        ExecuteCommands(commands2);

        VERIFY_ARE_EQUAL(StringReference(L"1"), ref new String(pCalculatorDisplay->GetPrimaryDisplay().c_str()));
    }

    void CalculatorManagerTest::CalculatorManagerTestSerialize()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester *)m_calculatorDisplayTester.get();
        wstring resultPrimary = L"";
        wstring resultExpression = L"";
        Cleanup();

        m_calculatorManager->SendCommand(Command::ModeScientific);
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandADD);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::CommandMUL);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::CommandEQU);
        m_calculatorManager->MemorizeNumber();

        vector<wstring> memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        wstring primaryDisplay = pCalculatorDisplay->GetPrimaryDisplay();
        wstring expressionDisplay = pCalculatorDisplay->GetExpression();

        SerializeAndDeSerialize();

        vector<wstring> memorizedNumbersAfterDeSerialize = pCalculatorDisplay->GetMemorizedNumbers();
        wstring primaryDisplayAfterDeSerialize = pCalculatorDisplay->GetPrimaryDisplay();
        wstring expressionDisplayAfterDeSerialize = pCalculatorDisplay->GetExpression();

        VERIFY_ARE_EQUAL(primaryDisplay, primaryDisplayAfterDeSerialize);
        VERIFY_ARE_EQUAL(expressionDisplay, expressionDisplayAfterDeSerialize);

        bool isEqual = false;

        if (memorizedNumbers.size() < memorizedNumbersAfterDeSerialize.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), memorizedNumbersAfterDeSerialize.begin());
        }
        else
        {
            isEqual = std::equal(memorizedNumbersAfterDeSerialize.begin(), memorizedNumbersAfterDeSerialize.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::CommandGRAD);
        m_calculatorManager->SendCommand(Command::Command5);
        m_calculatorManager->SendCommand(Command::Command0);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->SendCommand(Command::CommandMUL);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        primaryDisplay = pCalculatorDisplay->GetPrimaryDisplay();
        expressionDisplay = pCalculatorDisplay->GetExpression();

        SerializeAndDeSerialize();

        memorizedNumbersAfterDeSerialize = pCalculatorDisplay->GetMemorizedNumbers();
        primaryDisplayAfterDeSerialize = pCalculatorDisplay->GetPrimaryDisplay();
        expressionDisplayAfterDeSerialize = pCalculatorDisplay->GetExpression();

        VERIFY_ARE_EQUAL(primaryDisplay, primaryDisplayAfterDeSerialize);
        VERIFY_ARE_EQUAL(expressionDisplay, expressionDisplayAfterDeSerialize);

        isEqual = false;
        if (memorizedNumbers.size() < memorizedNumbersAfterDeSerialize.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), memorizedNumbersAfterDeSerialize.begin());
        }
        else
        {
            isEqual = std::equal(memorizedNumbersAfterDeSerialize.begin(), memorizedNumbersAfterDeSerialize.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);

        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::Command3);
        m_calculatorManager->SendCommand(Command::CommandPNT);
        m_calculatorManager->SendCommand(Command::Command8);
        m_calculatorManager->SendCommand(Command::CommandSIGN);
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SendCommand(Command::Command2);
        m_calculatorManager->SendCommand(Command::Command3);
        m_calculatorManager->MemorizedNumberAdd(0);
        m_calculatorManager->SendCommand(Command::CommandCENTR);
        m_calculatorManager->SendCommand(Command::Command3);
        m_calculatorManager->SendCommand(Command::Command1);
        m_calculatorManager->SendCommand(Command::CommandSIN);
        m_calculatorManager->MemorizedNumberSubtract(2);
        m_calculatorManager->MemorizedNumberLoad(2);

        memorizedNumbers = pCalculatorDisplay->GetMemorizedNumbers();
        primaryDisplay = pCalculatorDisplay->GetPrimaryDisplay();
        expressionDisplay = pCalculatorDisplay->GetExpression();

        SerializeAndDeSerialize();

        memorizedNumbersAfterDeSerialize = pCalculatorDisplay->GetMemorizedNumbers();
        primaryDisplayAfterDeSerialize = pCalculatorDisplay->GetPrimaryDisplay();
        expressionDisplayAfterDeSerialize = pCalculatorDisplay->GetExpression();

        VERIFY_ARE_EQUAL(primaryDisplay, primaryDisplayAfterDeSerialize);
        VERIFY_ARE_EQUAL(expressionDisplay, expressionDisplayAfterDeSerialize);

        isEqual = false;
        if (memorizedNumbers.size() < memorizedNumbersAfterDeSerialize.size())
        {
            isEqual = std::equal(memorizedNumbers.begin(), memorizedNumbers.end(), memorizedNumbersAfterDeSerialize.begin());
        }
        else
        {
            isEqual = std::equal(memorizedNumbersAfterDeSerialize.begin(), memorizedNumbersAfterDeSerialize.end(), memorizedNumbers.begin());
        }
        VERIFY_IS_TRUE(isEqual);
    }

    // Send 12345678910111213 and verify MaxDigitsReached
    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached()
    {
        TestMaxDigitsReachedScenario(L"1,234,567,891,011,1213");
    }

    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached_LeadingDecimal()
    {
        TestMaxDigitsReachedScenario(L"0.12345678910111213");
    }

    void CalculatorManagerTest::CalculatorManagerTestMaxDigitsReached_TrailingDecimal()
    {
        TestMaxDigitsReachedScenario(L"123,456,789,101,112.13");
    }

    // TODO re-enable when cause of failure is determined. Bug 20226670
    // void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived()
    // {
    //     CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester *)m_calculatorDisplayTester.get();

    //     VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

    //     ExecuteCommands({
    //         Command::Command1,
    //         Command::CommandADD
    //     });

    //     wstring display = pCalculatorDisplay->GetPrimaryDisplay();
    //     VERIFY_ARE_EQUAL(wstring(L"1"), display);

    //     // Verify BinaryOperatorReceived
    //     VERIFY_ARE_EQUAL(1, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    // }

    // TODO re-enable when cause of failure is determined. Bug 20226670
    // void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_Multiple()
    // {
    //     CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester *)m_calculatorDisplayTester.get();

    //     VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

    //     ExecuteCommands({
    //         Command::Command1,
    //         Command::CommandADD,
    //         Command::CommandSUB,
    //         Command::CommandMUL
    //     });

    //     wstring display = pCalculatorDisplay->GetPrimaryDisplay();
    //     VERIFY_ARE_EQUAL(wstring(L"1"), display);

    //     // Verify BinaryOperatorReceived
    //     VERIFY_ARE_EQUAL(3, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    // }

    // TODO re-enable when cause of failure is determined. Bug 20226670
    // void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_LongInput()
    // {
    //     CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester *)m_calculatorDisplayTester.get();

    //     VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

    //     ExecuteCommands({
    //         Command::Command1,
    //         Command::CommandADD,
    //         Command::Command2,
    //         Command::CommandMUL,
    //         Command::Command1,
    //         Command::Command0,
    //         Command::CommandSUB,
    //         Command::Command5,
    //         Command::CommandDIV,
    //         Command::Command5,
    //         Command::CommandEQU
    //     });

    //     wstring display = pCalculatorDisplay->GetPrimaryDisplay();
    //     VERIFY_ARE_EQUAL(wstring(L"5"), display);

    //     // Verify BinaryOperatorReceived
    //     VERIFY_ARE_EQUAL(4, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    // }

} /* namespace CalculationManagerUnitTests */
