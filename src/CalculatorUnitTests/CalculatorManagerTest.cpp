// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>
#include <stdexcept>

#include "CalcManager/CalculatorHistory.h"
#include "EngineResourceProvider.h"
#include "CalcManager/NumberFormattingUtils.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculationManager;
using namespace UnitConversionManager::NumberFormattingUtils;
using namespace Platform;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorManagerTest
{
    class CalculatorManagerDisplayTester final : public ICalcDisplay
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
            m_parenDisplay = 0;
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
        void SetExpressionDisplay(
            _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
            _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& /*commands*/) override
        {
            m_expression.clear();

            for (const auto& currentPair : *tokens)
            {
                m_expression += currentPair.first;
            }
        }
        void SetMemorizedNumbers(const vector<wstring>& numbers) override
        {
            m_memorizedNumberStrings = numbers;
        }

        void SetParenthesisNumber(unsigned int parenthesisCount) override
        {
            m_parenDisplay = parenthesisCount;
        }

        void OnNoRightParenAdded() override
        {
            // This method is used to create a narrator announcement when a close parenthesis cannot be added because there are no open parentheses
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
        unsigned int GetParenthesisCount() const
        {
            return m_parenDisplay;
        }

        void OnHistoryItemAdded(_In_ unsigned int /*addedItemIndex */) override
        {
        }

        void MaxDigitsReached() override
        {
            m_maxDigitsCalledCount++;
        }

        void InputChanged() override
        {
        }

        int GetMaxDigitsCalledCount()
        {
            return m_maxDigitsCalledCount;
        }

        void BinaryOperatorReceived() override
        {
            m_binaryOperatorReceivedCallCount++;
        }

        void MemoryItemChanged(unsigned int /*indexOfMemory*/) override
        {
        }

        int GetBinaryOperatorReceivedCallCount()
        {
            return m_binaryOperatorReceivedCallCount;
        }

    private:
        wstring m_primaryDisplay;
        wstring m_expression;
        unsigned int m_parenDisplay;
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

        static void Test(wstring expectedPrimary, wstring expectedExpression, Command testCommands[], bool cleanup = true, bool isScientific = false)
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

            wstring context = L"Commands:";
            for (auto command = testCommands; *command != Command::CommandNULL; ++command)
            {
                context += L" " + to_wstring(static_cast<int>(*command));
            }
            Assert::AreEqual(expectedPrimary, m_displayTester->GetPrimaryDisplay(), context.c_str());
            if (expectedExpression != L"N/A")
            {
                Assert::AreEqual(expectedExpression, m_displayTester->GetExpression(), context.c_str());
            }
        }
    };

    // Declare this class as a TestClass, and supply metadata if necessary.
    TEST_CLASS(CalculatorManagerTest)
    {
    public:
        TEST_METHOD_INITIALIZE(CommonSetup);

        TEST_METHOD(CalculatorManagerTestStandard);

        TEST_METHOD(CalculatorManagerTestScientific);
        TEST_METHOD(CalculatorManagerTestScientific2);
        TEST_METHOD(CalculatorManagerTestScientificParenthesis);
        TEST_METHOD(CalculatorManagerTestScientificError);
        TEST_METHOD(CalculatorManagerTestScientificModeChange);
        TEST_METHOD(ScientificRoutingDoesNotChangeLegacyModes);
        TEST_METHOD(ScientificUnsupportedCommandsPreserveInput);
        TEST_METHOD(ScientificMemoryIsIsolated);
        TEST_METHOD(ScientificHistoryIsReadOnly);
        TEST_METHOD(ScientificInputAndErrorQueries);
        TEST_METHOD(ScientificResetPreservesLegacyMemory);
        TEST_METHOD(ScientificIncompleteInputAutoCompletes);

        TEST_METHOD(CalculatorManagerTestProgrammer);

        TEST_METHOD(CalculatorManagerTestModeChange);

        TEST_METHOD(CalculatorManagerTestMemory);

        TEST_METHOD(CalculatorManagerTestMaxDigitsReached);
        TEST_METHOD(CalculatorManagerTestMaxDigitsReached_LeadingDecimal);
        TEST_METHOD(CalculatorManagerTestMaxDigitsReached_TrailingDecimal);

        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_GetNumberDigits);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits);
        TEST_METHOD(UnitConversionManagerNumberFormattingUtils_ToScientificNumber);

        TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived);
        TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived_Multiple);
        TEST_METHOD(CalculatorManagerTestBinaryOperatorReceived_LongInput);

        TEST_METHOD(CalculatorManagerTestStandardOrderOfOperations);

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
            CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

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
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandPNT,
                                Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3);

        Command commands4[] = { Command::Command2, Command::CommandADD, Command::Command3,   Command::CommandEQU,
                                Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"7", L"4 + 3=", commands4);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"4=", commands5);

        Command commands6[] = { Command::Command2,    Command::Command5,    Command::Command6,   Command::CommandSQRT,
                                Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6);

        Command commands7[] = { Command::Command3,   Command::CommandSUB, Command::Command6,   Command::CommandEQU,
                                Command::CommandMUL, Command::Command3,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"-9", L"-3 \x00D7 3=", commands7);

        Command commands8[] = { Command::Command9,     Command::CommandMUL, Command::Command6,   Command::CommandSUB,
                                Command::CommandCENTR, Command::Command8,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"46", L"54 - 8=", commands8);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0.36", L"6 \x00D7 0.06=", commands9);

        Command commands10[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands10);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"4 + 4=", commands11);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command9, Command::Command9,   Command::Command9,
                                 Command::Command9, Command::CommandDIV, Command::Command1,    Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13);

        Command commands14[] = { Command::Command5, Command::Command0,       Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandPERCENT, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"60", L"50 + 10=", commands14);

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16);

        Command commands17[] = { Command::Command1,     Command::Command2, Command::CommandADD, Command::Command5,
                                 Command::CommandCENTR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"14 + ", commands17);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0, Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19);

        Command commands20[] = { Command::Command1,    Command::Command2,    Command::Command3,   Command::CommandBACK,
                                 Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands21);

        Command commands22[] = { Command::Command1,   Command::Command0, Command::Command2, Command::Command4,   Command::CommandSQRT,
                                 Command::CommandSUB, Command::Command3, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands22);

        Command commands23[] = { Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5,   Command::CommandSQRT, Command::CommandSUB,
                                 Command::Command1, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(2.25) - 1.5=", commands23);
    }

    void CalculatorManagerTest::CalculatorManagerTestScientific()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandPNT,
                                Command::Command4, Command::Command5, Command::Command6, Command::CommandNULL };
        TestDriver::Test(L"123.456", L"", commands1, true, true);

        Command commands2[] = { Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"0 + ", commands2, true, true);

        Command commands3[] = { Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands3, true, true);

        Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command0,   Command::CommandMUL,
                                Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"1", L"1 + 0 \x00D7 2=", commands4, true, true);

        Command commands5[] = { Command::Command4, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"4=", commands5, true, true);

        Command commands6[] = { Command::Command2,    Command::Command5,    Command::Command6,   Command::CommandSQRT,
                                Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(\x221A(\x221A(256)))", commands6, true, true);

        Command commands7[] = { Command::Command3,   Command::CommandSUB, Command::Command6,   Command::CommandEQU,
                                Command::CommandMUL, Command::Command3,   Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"-9", L"-3 \x00D7 3 + ", commands7, true, true);

        Command commands8[] = { Command::Command9, Command::CommandMUL, Command::Command6, Command::CommandSUB, Command::CommandCENTR,
                                Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"38", L"9 \x00D7 6 - 8 \x00D7 2 + ", commands8, true, true);

        Command commands9[] = { Command::Command6, Command::CommandMUL, Command::Command6, Command::CommandSIGN, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"6 \x00D7 \x221A(-6)", commands9, true, true);

        Command commands10[] = { Command::Command5, Command::Command0,   Command::CommandADD, Command::Command2,
                                 Command::Command0, Command::CommandREC, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"50.05", L"50 + 1/(20) - ", commands10, true, true);

        Command commands11[] = { Command::Command4, Command::CommandADD, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"8", L"4 + 4=", commands11, true, true);

        Command commands12[] = { Command::Command5, Command::CommandADD, Command::CommandMUL, Command::Command3, Command::CommandNULL };
        TestDriver::Test(L"3", L"5 \x00D7 ", commands12, true, true);

        Command commands13[] = { Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command9, Command::Command9,   Command::Command9,
                                 Command::Command9, Command::CommandDIV, Command::Command1,    Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Overflow", L"1.e-9999 \x00F7 ", commands13, true, true);

        Command commands14[] = { Command::Command5, Command::Command0, Command::CommandADD, Command::Command2, Command::Command0, Command::CommandNULL };
        TestDriver::Test(L"20", L"50 + ", commands14, true, true);
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->SendCommand(Command::CommandPERCENT); });
        VERIFY_ARE_EQUAL(wstring(L"20"), m_calculatorDisplayTester->GetPrimaryDisplay());
        m_calculatorManager->SendCommand(Command::CommandEQU);
        VERIFY_ARE_EQUAL(wstring(L"70"), m_calculatorDisplayTester->GetPrimaryDisplay());

        Command commands15[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Result is undefined", L"0 \x00F7 ", commands15, true, true);

        Command commands16[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands16, true, true);

        Command commands17[] = { Command::Command1,     Command::Command2, Command::CommandADD, Command::Command5,
                                 Command::CommandCENTR, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"14", L"12 + 2 + ", commands17, true, true);

        Command commands18[] = { Command::Command1, Command::Command0, Command::Command0, Command::CommandSIGN, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"-0.01", L"1/(-100)", commands18, true, true);

        Command commands19[] = { Command::Command1, Command::Command2, Command::Command3, Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"1", L"", commands19, true, true);

        Command commands20[] = { Command::Command1,    Command::Command2,    Command::Command3,   Command::CommandBACK,
                                 Command::CommandBACK, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20, true, true);

        Command commands21[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(4) - 2 + ", commands21, true, true);

        Command commands22[] = { Command::Command0, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(0)", commands22, true, true);

        Command commands23[] = { Command::Command1,   Command::Command0, Command::Command2, Command::Command4,   Command::CommandSQRT,
                                 Command::CommandSUB, Command::Command3, Command::Command2, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(1024) - 32 + ", commands23, true, true);

        Command commands24[] = { Command::Command2,    Command::Command5,    Command::Command7,   Command::CommandSQRT,
                                 Command::CommandSQRT, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2.0009748976330773374220277351385", L"\x221A(\x221A(\x221A(257)))", commands24, true, true);

        Command commands25[] = { Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5,   Command::CommandSQRT, Command::CommandSUB,
                                 Command::Command1, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"0", L"\x221A(2.25) - 1.5=", commands25, true, true);

        Command commands26[] = { Command::CommandOPENP, Command::Command2,   Command::CommandPNT, Command::Command2,   Command::Command5,
                                 Command::CommandSQRT,  Command::CommandDIV, Command::Command1,   Command::CommandPNT, Command::Command5,
                                 Command::CommandCLOSEP, Command::CommandLOG,  Command::CommandNULL };
        TestDriver::Test(L"0", L"log(\x221A(2.25) \x00F7 1.5)", commands26, true, true);
    }

    // Scientific functions from the scientific calculator
    void CalculatorManagerTest::CalculatorManagerTestScientific2()
    {
        Command commands1[] = { Command::Command1, Command::Command2, Command::CommandSQR, Command::CommandNULL };
        TestDriver::Test(L"144", L"sqr(12)", commands1, true, true);

        Command commands2[] = { Command::Command1, Command::Command0, Command::Command0, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"2", L"log(100)", commands2, true, true);

        Command commands3[] = { Command::Command1, Command::CommandLN, Command::CommandNULL };
        TestDriver::Test(L"0", L"ln(1)", commands3, true, true);

        Command commands4[] = { Command::Command1, Command::Command0, Command::CommandLN, Command::CommandNULL };
        TestDriver::Test(L"2.3025850929940456840179914546844", L"ln(10)", commands4, true, true);

        // Independently verified 32-significant-digit Epsilon result.
        Command commands5[] = { Command::Command1, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0.017452406437283512819418978516316", L"sin\x2080(1)", commands5, true, true);

        Command commands6[] = { Command::Command1, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"0.99984769515639123915701155881391", L"cos\x2080(1)", commands6, true, true);

        Command commands7[] = { Command::Command1, Command::CommandTAN, Command::CommandNULL };
        TestDriver::Test(L"0.017455064928217585765128895219728", L"tan\x2080(1)", commands7, true, true);

        Command commands8[] = { Command::Command0, Command::CommandPOWE, Command::CommandNULL };
        TestDriver::Test(L"1", L"e^(0)", commands8, true, true);

        Command commands9[] = { Command::Command2, Command::CommandPOWE, Command::CommandNULL };
        TestDriver::Test(L"7.389056098930650227230427460575", L"e^(2)", commands9, true, true);

        Command commands10[] = { Command::Command3, Command::CommandSIGN, Command::CommandSQR, Command::CommandNULL };
        TestDriver::Test(L"9", L"sqr(-3)", commands10, true, true);

        Command commands11[] = { Command::Command8, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"0.125", L"1/(8)", commands11, true, true);
    }

    void CalculatorManagerTest::CalculatorManagerTestScientificParenthesis()
    {
        Command commands1[] = { Command::Command1, Command::CommandADD,    Command::CommandOPENP, Command::CommandADD,
                                Command::Command3, Command::CommandCLOSEP, Command::CommandNULL };
        TestDriver::Test(L"3", L"1 + (0 + 3)", commands1, true, true);

        Command commands2[] = {
            Command::CommandOPENP, Command::CommandOPENP, Command::Command1, Command::Command2, Command::CommandCLOSEP, Command::CommandNULL
        };
        TestDriver::Test(L"12", L"((12)", commands2, true, true);

        Command commands3[] = { Command::Command1,      Command::Command2,     Command::CommandCLOSEP,
                                Command::CommandCLOSEP, Command::CommandOPENP, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 \x00D7 (", commands3, true, true);

        Command commands4[] = {
            Command::Command2, Command::CommandOPENP, Command::Command2, Command::CommandCLOSEP, Command::CommandADD, Command::CommandNULL
        };
        TestDriver::Test(L"4", L"2 \x00D7 (2) + ", commands4, true, true);

        Command commands5[] = { Command::Command2,   Command::CommandOPENP, Command::Command2,   Command::CommandCLOSEP,
                                Command::CommandADD, Command::CommandEQU,   Command::CommandNULL };
        TestDriver::Test(L"8", L"2 \x00D7 (2) + 4=", commands5, true, true);

        Command commands6[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"16", L"(8) \x00D7 2=", commands6, true, true);

        Command commands7[] = { Command::CommandOPENP,  Command::Command7, Command::CommandMUL, Command::Command2,
                                Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"28", L"(7 \x00D7 2) \x00D7 2=", commands7, true, true);

        Command commands8[] = { Command::CommandOPENP,  Command::Command7, Command::CommandMUL, Command::Command2,
                                Command::CommandCLOSEP, Command::Command2, Command::CommandEQU,
                                Command::CommandOPENP, Command::Command1, Command::Command4, Command::CommandCLOSEP, Command::Command2, Command::CommandEQU, Command::CommandNULL};
        TestDriver::Test(L"28", L"(14) \x00D7 2=", commands8, true, true);

        Command commands9[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP,
                                Command::Command0, Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"(8) \x00D7 0.5=", commands9, true, true);

        Command commands10[] = { Command::CommandOPENP, Command::Command8, Command::CommandCLOSEP,
                                 Command::CommandPNT, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"4", L"(8) \x00D7 0.5=", commands10, true, true);
    }

    void CalculatorManagerTest::CalculatorManagerTestScientificError()
    {
        Command commands1[] = { Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"Cannot divide by zero", L"1 \x00F7 ", commands1, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands2[] = { Command::Command2, Command::CommandSIGN, Command::CommandLOG, Command::CommandNULL };
        TestDriver::Test(L"Invalid input", L"log(-2)", commands2, true, true);
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());

        Command commands3[] = { Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU, Command::CommandNULL };
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

        Command commands4[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandSIN, Command::CommandNULL };
        TestDriver::Test(L"0", L"N/A", commands4, true, true);

        Command commands5[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandCOS, Command::CommandNULL };
        TestDriver::Test(L"1", L"N/A", commands5, true, true);

        Command commands6[] = { Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandTAN, Command::CommandNULL };
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

    void CalculatorManagerTest::ScientificRoutingDoesNotChangeLegacyModes()
    {
        const vector<Command> expression = {
            Command::Command2, Command::CommandADD, Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandEQU
        };

        for (int iteration = 0; iteration < 4; ++iteration)
        {
            m_calculatorManager->SetScientificMode();
            VERIFY_IS_FALSE(m_calculatorManager->IsMemorySupported());
            VERIFY_IS_TRUE(m_calculatorManager->IsHistoryReadOnly());
            ExecuteCommands(expression);
            VERIFY_ARE_EQUAL(wstring(L"14"), m_calculatorDisplayTester->GetPrimaryDisplay());
            VERIFY_IS_FALSE(m_calculatorDisplayTester->GetIsError());
            VERIFY_IS_TRUE(m_calculatorManager->GetDisplayCommandsSnapshot().empty());

            m_calculatorManager->SendCommand(Command::ModeBasic);
            VERIFY_IS_TRUE(m_calculatorManager->IsMemorySupported());
            VERIFY_IS_FALSE(m_calculatorManager->IsHistoryReadOnly());
            ExecuteCommands(expression);
            VERIFY_ARE_EQUAL(wstring(L"20"), m_calculatorDisplayTester->GetPrimaryDisplay());

            m_calculatorManager->SendCommand(Command::ModeProgrammer);
            m_calculatorManager->SetRadix(RadixType::Hex);
            ExecuteCommands(vector<Command>{ Command::CommandF, Command::CommandAnd, Command::Command3, Command::CommandEQU });
            VERIFY_ARE_EQUAL(wstring(L"3"), m_calculatorDisplayTester->GetPrimaryDisplay());
        }
    }

    void CalculatorManagerTest::ScientificUnsupportedCommandsPreserveInput()
    {
        m_calculatorManager->SetScientificMode();
        ExecuteCommands(vector<Command>{ Command::Command4, Command::Command2 });

        const vector<Command> unsupported = {
            Command::CommandPWR,       Command::CommandROOT,  Command::CommandCUB,     Command::CommandCUBEROOT,
            Command::CommandFAC,       Command::CommandMOD,   Command::CommandASIN,    Command::CommandACOS,
            Command::CommandATAN,      Command::CommandSINH,  Command::CommandCOSH,    Command::CommandTANH,
            Command::CommandASINH,     Command::CommandACOSH, Command::CommandATANH,   Command::CommandHYP,
            Command::CommandINV,       Command::CommandSEC,   Command::CommandCSC,     Command::CommandCOT,
            Command::CommandASEC,      Command::CommandACSC,  Command::CommandACOT,    Command::CommandSECH,
            Command::CommandCSCH,      Command::CommandCOTH,  Command::CommandASECH,   Command::CommandACSCH,
            Command::CommandACOTH,     Command::CommandDMS,   Command::CommandCHOP,    Command::CommandCOM,
            Command::CommandLogBaseY,  Command::CommandPOW10, Command::CommandPOW2,    Command::CommandRand,
            Command::CommandAbs,       Command::CommandFloor, Command::CommandCeil,    Command::CommandPERCENT,
            Command::CommandSTORE,     Command::CommandRECALL, Command::CommandMCLEAR, Command::CommandMPLUS,
            Command::CommandMMINUS,    Command::CommandAnd,   Command::CommandHex,     Command::CommandA
        };
        for (Command command : unsupported)
        {
            VERIFY_IS_FALSE(m_calculatorManager->IsCommandSupported(command));
            Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->SendCommand(command); });
            VERIFY_ARE_EQUAL(wstring(L"42"), m_calculatorDisplayTester->GetPrimaryDisplay());
            VERIFY_IS_FALSE(m_calculatorDisplayTester->GetIsError());
        }

        VERIFY_IS_TRUE(m_calculatorManager->IsCommandSupported(Command::ModeBasic));
        VERIFY_IS_TRUE(m_calculatorManager->IsCommandSupported(Command::ModeProgrammer));
        ExecuteCommands(vector<Command>{ Command::CommandADD, Command::Command1, Command::CommandEQU });
        VERIFY_ARE_EQUAL(wstring(L"43"), m_calculatorDisplayTester->GetPrimaryDisplay());
    }

    void CalculatorManagerTest::ScientificMemoryIsIsolated()
    {
        m_calculatorManager->SetStandardMode();
        ExecuteCommands(vector<Command>{ Command::Command4, Command::Command2 });
        m_calculatorManager->MemorizeNumber();
        const auto memory = m_calculatorDisplayTester->GetMemorizedNumbers();
        VERIFY_ARE_EQUAL(size_t(1), memory.size());

        m_calculatorManager->SetScientificMode();
        m_calculatorManager->SendCommand(Command::Command9);
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->MemorizeNumber(); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->MemorizedNumberLoad(0); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->MemorizedNumberAdd(0); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->MemorizedNumberSubtract(0); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->MemorizedNumberClear(0); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->MemorizedNumberClearAll(); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->SetMemorizedNumbersString(); });
        VERIFY_ARE_EQUAL(wstring(L"9"), m_calculatorDisplayTester->GetPrimaryDisplay());
        VERIFY_IS_TRUE(memory == m_calculatorDisplayTester->GetMemorizedNumbers());

        m_calculatorManager->SetStandardMode();
        m_calculatorManager->MemorizedNumberLoad(0);
        VERIFY_ARE_EQUAL(wstring(L"42"), m_calculatorDisplayTester->GetPrimaryDisplay());
        m_calculatorManager->MemorizedNumberAdd(0);
        m_calculatorManager->MemorizedNumberLoad(0);
        VERIFY_ARE_EQUAL(wstring(L"84"), m_calculatorDisplayTester->GetPrimaryDisplay());
    }

    void CalculatorManagerTest::ScientificHistoryIsReadOnly()
    {
        m_calculatorManager->SetStandardMode();
        ExecuteCommands(vector<Command>{ Command::Command6, Command::CommandMUL, Command::Command7, Command::CommandEQU });
        const auto legacyHistory = m_calculatorManager->GetHistoryItems();
        VERIFY_IS_FALSE(legacyHistory.front()->historyItemVector.spCommands->empty());

        m_calculatorManager->SetScientificMode();
        ExecuteCommands(vector<Command>{ Command::Command2, Command::CommandADD, Command::Command3, Command::CommandEQU });
        VERIFY_ARE_EQUAL(size_t(1), m_calculatorManager->GetHistoryItems().size());
        VERIFY_IS_FALSE(m_calculatorManager->GetHistoryItem(0)->historyItemVector.expression.empty());
        VERIFY_ARE_EQUAL(wstring(L"5"), m_calculatorManager->GetHistoryItem(0)->historyItemVector.result);
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->RemoveHistoryItem(0); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->ClearHistory(); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->SetInHistoryItemLoadMode(true); });
        VERIFY_ARE_EQUAL(wstring(L"5"), m_calculatorDisplayTester->GetPrimaryDisplay());

        m_calculatorManager->SetHistoryItems(legacyHistory);
        VERIFY_ARE_EQUAL(size_t(2), m_calculatorManager->GetHistoryItems().size());
        for (const auto& history : m_calculatorManager->GetHistoryItems())
        {
            VERIFY_IS_TRUE(history->historyItemVector.spCommands->empty());
            for (const auto& token : *history->historyItemVector.spTokens)
            {
                VERIFY_ARE_EQUAL(-1, token.second);
            }
        }
        VERIFY_IS_FALSE(legacyHistory.front()->historyItemVector.spCommands->empty());

        m_calculatorManager->SetStandardMode();
        m_calculatorManager->ClearHistory();
        VERIFY_IS_TRUE(m_calculatorManager->GetHistoryItems().empty());
        VERIFY_ARE_EQUAL(size_t(2), m_calculatorManager->GetHistoryItems(CalculatorMode::Scientific).size());

        m_calculatorManager->SetScientificMode();
        m_calculatorManager->SetProgrammerMode();
        VERIFY_IS_FALSE(m_calculatorManager->IsHistoryReadOnly());
        m_calculatorManager->ClearHistory();
        VERIFY_ARE_EQUAL(size_t(2), m_calculatorManager->GetHistoryItems(CalculatorMode::Scientific).size());
    }

    void CalculatorManagerTest::ScientificInputAndErrorQueries()
    {
        m_calculatorManager->SetScientificMode();
        VERIFY_IS_TRUE(m_calculatorManager->IsInputEmpty());
        VERIFY_ARE_EQUAL(L'.', m_calculatorManager->DecimalSeparator());
        m_calculatorManager->SetPrecision(32);
        m_calculatorManager->SetRadix(RadixType::Decimal);
        m_calculatorManager->UpdateMaxIntDigits();
        ExecuteCommands(vector<Command>{ Command::Command1, Command::Command2, Command::Command3 });
        VERIFY_IS_FALSE(m_calculatorManager->IsInputEmpty());
        VERIFY_ARE_EQUAL(wstring(L"123"), m_calculatorManager->GetResultForRadix(10, 32, true));
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->SetRadix(RadixType::Hex); });
        Assert::ExpectException<invalid_argument>([&]() { m_calculatorManager->GetResultForRadix(16, 32, true); });
        VERIFY_ARE_EQUAL(wstring(L"123"), m_calculatorDisplayTester->GetPrimaryDisplay());
        m_calculatorManager->DisplayPasteError();
        VERIFY_IS_TRUE(m_calculatorDisplayTester->GetIsError());
        m_calculatorManager->SendCommand(Command::CommandCLEAR);
        VERIFY_IS_FALSE(m_calculatorDisplayTester->GetIsError());
        VERIFY_IS_TRUE(m_calculatorManager->IsInputEmpty());
    }

    void CalculatorManagerTest::ScientificResetPreservesLegacyMemory()
    {
        m_calculatorManager->SetStandardMode();
        ExecuteCommands(vector<Command>{ Command::Command4, Command::Command2 });
        m_calculatorManager->MemorizeNumber();
        m_calculatorManager->SetScientificMode();
        ExecuteCommands(vector<Command>{ Command::CommandRAD, Command::CommandFE, Command::Command9 });
        m_calculatorManager->Reset(false);
        VERIFY_IS_TRUE(m_calculatorManager->IsMemorySupported());
        VERIFY_ARE_EQUAL(static_cast<int>(Command::CommandDEG), static_cast<int>(m_calculatorManager->GetCurrentDegreeMode()));
        m_calculatorManager->MemorizedNumberLoad(0);
        VERIFY_ARE_EQUAL(wstring(L"42"), m_calculatorDisplayTester->GetPrimaryDisplay());
        m_calculatorManager->SetScientificMode();
        ExecuteCommands(vector<Command>{ Command::Command9, Command::CommandSQRT });
        VERIFY_ARE_EQUAL(wstring(L"3"), m_calculatorDisplayTester->GetPrimaryDisplay());
    }

    void CalculatorManagerTest::ScientificIncompleteInputAutoCompletes()
    {
        m_calculatorManager->SetScientificMode();
        ExecuteCommands(vector<Command>{ Command::CommandOPENP, Command::Command2, Command::CommandADD, Command::Command3, Command::CommandEQU });
        VERIFY_ARE_EQUAL(wstring(L"5"), m_calculatorDisplayTester->GetPrimaryDisplay());
        VERIFY_ARE_EQUAL(0u, m_calculatorDisplayTester->GetParenthesisCount());

        ExecuteCommands(vector<Command>{ Command::CommandCLEAR, Command::Command4, Command::CommandADD, Command::CommandEQU });
        VERIFY_ARE_EQUAL(wstring(L"8"), m_calculatorDisplayTester->GetPrimaryDisplay());
        const auto historyCount = m_calculatorManager->GetHistoryItems().size();
        ExecuteCommands(vector<Command>{ Command::CommandEQU, Command::CommandEQU });
        VERIFY_ARE_EQUAL(wstring(L"8"), m_calculatorDisplayTester->GetPrimaryDisplay());
        VERIFY_ARE_EQUAL(historyCount, m_calculatorManager->GetHistoryItems().size());

        ExecuteCommands(vector<Command>{
            Command::CommandCLEAR, Command::CommandOPENP, Command::CommandOPENP, Command::Command2, Command::CommandADD,
            Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandEQU
        });
        VERIFY_ARE_EQUAL(wstring(L"14"), m_calculatorDisplayTester->GetPrimaryDisplay());
        VERIFY_ARE_EQUAL(0u, m_calculatorDisplayTester->GetParenthesisCount());

        ExecuteCommands(vector<Command>{ Command::CommandCLEAR, Command::Command8, Command::CommandDIV, Command::CommandEQU });
        VERIFY_ARE_EQUAL(wstring(L"1"), m_calculatorDisplayTester->GetPrimaryDisplay());
    }

    void CalculatorManagerTest::CalculatorManagerTestProgrammer()
    {
        Command commands1[] = { Command::ModeProgrammer, Command::Command5, Command::Command3,   Command::CommandNand,
                                Command::Command8,       Command::Command3, Command::CommandAnd, Command::CommandNULL };
        TestDriver::Test(L"-18", L"53 NAND 83 AND ", commands1, true, false);

        Command commands2[] = { Command::ModeProgrammer, Command::Command5, Command::Command3,   Command::CommandNor,
                                Command::Command8,       Command::Command3, Command::CommandAnd, Command::CommandNULL };
        TestDriver::Test(L"-120", L"53 NOR 83 AND ", commands2, true, false);

        Command commands3[] = {
            Command::ModeProgrammer, Command::Command5, Command::CommandLSHF, Command::Command1, Command::CommandAnd, Command::CommandNULL
        };
        TestDriver::Test(L"10", L"5 Lsh 1 AND ", commands3, true, false);

        Command commands5[] = {
            Command::ModeProgrammer, Command::Command5, Command::CommandRSHFL, Command::Command1, Command::CommandAnd, Command::CommandNULL
        };
        TestDriver::Test(L"2", L"5 Rsh 1 AND ", commands5, true, false);

        Command commands6[] = { Command::ModeProgrammer, Command::CommandBINPOS63, Command::CommandRSHF, Command::Command5,
                                Command::Command6,       Command::CommandAnd,      Command::CommandNULL };
        TestDriver::Test(L"-128", L"-9223372036854775808 Rsh 56 AND ", commands6, true, false);

        Command commands7[] = { Command::ModeProgrammer, Command::Command1, Command::CommandROL, Command::CommandNULL };
        TestDriver::Test(L"2", L"RoL(1)", commands7, true, false);

        Command commands8[] = { Command::ModeProgrammer, Command::Command1, Command::CommandROR, Command::CommandNULL };
        TestDriver::Test(L"-9,223,372,036,854,775,808", L"RoR(1)", commands8, true, false);

        Command commands9[] = { Command::ModeProgrammer, Command::Command1, Command::CommandRORC, Command::CommandNULL };
        TestDriver::Test(L"0", L"RoR(1)", commands9, true, false);

        Command commands10[] = { Command::ModeProgrammer, Command::Command1, Command::CommandRORC, Command::CommandRORC, Command::CommandNULL };
        TestDriver::Test(L"-9,223,372,036,854,775,808", L"RoR(RoR(1))", commands10, true, false);

        Command commands11[] = { Command::ModeProgrammer, Command::CommandDec, Command::Command4, Command::Command2, Command::Command9,   Command::Command4,
                                Command::Command9,       Command::Command6,   Command::Command7, Command::Command2, Command::Command9,   Command::Command6,
                                Command::CommandDIV,     Command::Command2,   Command::Command5, Command::Command5, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"16,843,009", L"4294967296 \x00F7 255=", commands11, true, false);

        Command commands12[] = {
            Command::ModeProgrammer, Command::CommandDec, Command::Command4, Command::Command2, Command::Command9,   Command::Command4,
            Command::Command9,       Command::Command6,   Command::Command7, Command::Command3, Command::Command0,   Command::Command3,
            Command::CommandDIV,     Command::Command2,   Command::Command5, Command::Command5, Command::CommandEQU, Command::CommandNULL
        };
        TestDriver::Test(L"16,843,009", L"4294967303 \x00F7 255=", commands12, true, false);

        Command commands13[] = {
            Command::ModeProgrammer, Command::CommandDec, Command::Command1, Command::Command0, Command::Command0, Command::Command0,
            Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::Command0, Command::CommandDIV,
            Command::Command6, Command::Command4, Command::Command4, Command::Command8, Command::Command7, Command::CommandEQU, Command::CommandNULL
        };
        TestDriver::Test(L"15,507", L"1000000000 \x00F7 64487=", commands13, true, false);

        Command commands14[] = { Command::ModeProgrammer, Command::CommandDec, Command::Command1,   Command::Command0,   Command::Command0,
                                 Command::Command0,       Command::Command0,   Command::Command0,   Command::Command0,   Command::Command0,
                                 Command::Command0,       Command::Command0,   Command::CommandDIV, Command::Command6,   Command::Command4,
                                 Command::Command4,       Command::Command8,   Command::Command8,   Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"15,506", L"1000000000 \x00F7 64488=", commands14, true, false);
    }

    void CalculatorManagerTest::CalculatorManagerTestMemory()
    {
        Command scientificCalculatorTest52[] = { Command::Command1, Command::CommandSTORE, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific52(L"1");
        wstring expectedExpressionDisplayTestScientific52(L"");

        Command scientificCalculatorTest53[] = { Command::Command1, Command::CommandNULL };
        wstring expectedPrimaryDisplayTestScientific53(L"1");
        wstring expectedExpressionDisplayTestScientific53(L"");

        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();
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

    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros()
    {
        wstring number = L"2.1032100000000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"2.10321");
        number = L"-122.123200";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"-122.1232");
        number = L"0.0001200";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"0.00012");
        number = L"12.000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"12");
        number = L"-12.00000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"-12");
        number = L"0.000";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"0");
        number = L"322423";
        TrimTrailingZeros(number);
        VERIFY_ARE_EQUAL(number, L"322423");
    }

    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_GetNumberDigits()
    {
        wstring number = L"2.10321";
        unsigned int digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 6);
        number = L"-122.1232";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 7);
        number = L"-3432";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 4);
        number = L"0";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        number = L"0.0001223";
        digitsCount = GetNumberDigits(number);
        VERIFY_ARE_EQUAL(digitsCount, 8);
    }

    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart()
    {
        unsigned int digitsCount = GetNumberDigitsWholeNumberPart(2.10321);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(-122.1232);
        VERIFY_ARE_EQUAL(digitsCount, 3);
        digitsCount = GetNumberDigitsWholeNumberPart(-3432);
        VERIFY_ARE_EQUAL(digitsCount, 4);
        digitsCount = GetNumberDigitsWholeNumberPart(0);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(324328412837382);
        VERIFY_ARE_EQUAL(digitsCount, 15);
        digitsCount = GetNumberDigitsWholeNumberPart(324328412837382.232213214324234);
        VERIFY_ARE_EQUAL(digitsCount, 15);
        digitsCount = GetNumberDigitsWholeNumberPart(0.032);
        VERIFY_ARE_EQUAL(digitsCount, 1);
        digitsCount = GetNumberDigitsWholeNumberPart(0.00000000000000000001);
        VERIFY_ARE_EQUAL(digitsCount, 1);
    }

    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits()
    {
        wstring result = RoundSignificantDigits(12.342343242, 3);
        VERIFY_ARE_EQUAL(result, L"12.342");
        result = RoundSignificantDigits(12.3429999, 3);
        VERIFY_ARE_EQUAL(result, L"12.343");
        result = RoundSignificantDigits(12.342500001, 3);
        VERIFY_ARE_EQUAL(result, L"12.343");
        result = RoundSignificantDigits(-2312.1244243346454345, 5);
        VERIFY_ARE_EQUAL(result, L"-2312.12442");
        result = RoundSignificantDigits(0.3423432423, 5);
        VERIFY_ARE_EQUAL(result, L"0.34234");
        result = RoundSignificantDigits(0.3423, 7);
        VERIFY_ARE_EQUAL(result, L"0.3423000");
    }

    void CalculatorManagerTest::UnitConversionManagerNumberFormattingUtils_ToScientificNumber()
    {
        wstring result = ToScientificNumber(3423);
        VERIFY_ARE_EQUAL(result, L"3.423000e+03");
        result = ToScientificNumber(-21);
        VERIFY_ARE_EQUAL(result, L"-2.100000e+01");
        result = ToScientificNumber(0.0232);
        VERIFY_ARE_EQUAL(result, L"2.320000e-02");
        result = ToScientificNumber(-0.00921);
        VERIFY_ARE_EQUAL(result, L"-9.210000e-03");
        result = ToScientificNumber(2343243345677);
        VERIFY_ARE_EQUAL(result, L"2.343243e+12");
        result = ToScientificNumber(-3432474247332942);
        VERIFY_ARE_EQUAL(result, L"-3.432474e+15");
        result = ToScientificNumber(0.000000003432432);
        VERIFY_ARE_EQUAL(result, L"3.432432e-09");
        result = ToScientificNumber(-0.000000003432432);
        VERIFY_ARE_EQUAL(result, L"-3.432432e-09");
    }

    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1, Command::CommandADD });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"1", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(1, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }

    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_Multiple()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1, Command::CommandADD, Command::CommandSUB, Command::CommandMUL });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"1", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(3, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }

    void CalculatorManagerTest::CalculatorManagerTestBinaryOperatorReceived_LongInput()
    {
        CalculatorManagerDisplayTester* pCalculatorDisplay = (CalculatorManagerDisplayTester*)m_calculatorDisplayTester.get();

        VERIFY_ARE_EQUAL(0, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());

        m_calculatorManager->SetStandardMode();
        ExecuteCommands({ Command::Command1,
                          Command::CommandADD,
                          Command::Command2,
                          Command::CommandMUL,
                          Command::Command1,
                          Command::Command0,
                          Command::CommandSUB,
                          Command::Command5,
                          Command::CommandDIV,
                          Command::Command5,
                          Command::CommandEQU });

        wstring display = pCalculatorDisplay->GetPrimaryDisplay();
        VERIFY_ARE_EQUAL(L"5", display);

        // Verify BinaryOperatorReceived
        VERIFY_ARE_EQUAL(4, pCalculatorDisplay->GetBinaryOperatorReceivedCallCount());
    }

    void CalculatorManagerTest::CalculatorManagerTestStandardOrderOfOperations()
    {
        Command commands1[] = { Command::Command1, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"1", L"1/(1)", commands1);

        Command commands2[] = { Command::Command4, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(4)", commands2);

        Command commands3[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"2", L"1 + \x221A(4)", commands3);

        Command commands4[] = { Command::Command1, Command::CommandADD, Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 - ", commands4);

        Command commands5[] = { Command::Command2, Command::CommandMUL, Command::Command4, Command::CommandREC, Command::CommandNULL };
        TestDriver::Test(L"0.25", L"2 \x00D7 1/(4)", commands5);

        Command commands6[] = { Command::Command5, Command::CommandDIV, Command::Command6, Command::CommandPERCENT, Command::CommandNULL };
        TestDriver::Test(L"0.06", L"5 \x00F7 0.06", commands6);

        Command commands7[] = { Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"2", L"\x221A(4) - ", commands7);

        Command commands8[] = { Command::Command7, Command::CommandSQR, Command::CommandDIV, Command::CommandNULL };
        TestDriver::Test(L"49", L"sqr(7) \x00F7 ", commands8);

        Command commands9[] = { Command::Command8, Command::CommandSQR, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"8", L"\x221A(sqr(8))", commands9);

        Command commands10[] = { Command::Command1, Command::Command0, Command::CommandADD, Command::Command2, Command::CommandSUB, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 - ", commands10);

        Command commands11[] = { Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandDIV, Command::CommandNULL };
        TestDriver::Test(L"12", L"12 \x00F7 ", commands11);

        Command commands12[] = { Command::Command6, Command::CommandDIV, Command::Command3, Command::CommandSUB, Command::CommandADD, Command::CommandNULL };
        TestDriver::Test(L"2", L"2 + ", commands12);

        Command commands13[] = { Command::Command7, Command::CommandSUB, Command::Command4, Command::CommandDIV, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 \x00D7 ", commands13);

        Command commands14[] = { Command::Command8, Command::CommandMUL, Command::Command2, Command::CommandADD, Command::CommandSQRT, Command::CommandNULL };
        TestDriver::Test(L"4", L"16 + \x221A(16)", commands14);

        Command commands15[] = { Command::Command9, Command::CommandADD, Command::Command0, Command::CommandMUL, Command::CommandSIGN, Command::CommandNULL };
        TestDriver::Test(L"-9", L"9 \x00D7 negate(9)", commands15);

        Command commands16[] = { Command::Command9, Command::CommandSIGN, Command::Command0, Command::CommandADD, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"-90", L"-90 \x00D7 ", commands16);

        Command commands17[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"3", L"1 + 2=", commands17);

        Command commands18[] = { Command::Command2, Command::Command0,   Command::CommandMUL, Command::Command0,
                                 Command::Command2, Command::CommandEQU, Command::CommandNULL };
        TestDriver::Test(L"40", L"20 \x00D7 2=", commands18);

        Command commands19[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandBACK, Command::CommandNULL };
        TestDriver::Test(L"3", L"3 + ", commands19);

        Command commands20[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCLEAR, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands20);

        Command commands21[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandADD, Command::CommandCENTR, Command::CommandNULL };
        TestDriver::Test(L"0", L"3 + ", commands21);

        Command commands22[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCLEAR, Command::CommandNULL };
        TestDriver::Test(L"0", L"", commands22);

        Command commands23[] = { Command::Command1, Command::CommandADD, Command::Command2, Command::CommandCENTR, Command::CommandNULL };
        TestDriver::Test(L"0", L"1 + ", commands23);

        Command commands24[] = { Command::Command1, Command::CommandMUL, Command::Command2, Command::CommandMUL, Command::Command3,   Command::CommandMUL,
                                 Command::Command4, Command::CommandMUL, Command::Command5, Command::CommandMUL, Command::CommandNULL };
        TestDriver::Test(L"120", L"120 \x00D7 ", commands24);
    }
} /* namespace CalculationManagerUnitTests */
