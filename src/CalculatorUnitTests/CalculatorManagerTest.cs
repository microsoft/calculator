// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CalcManager;
using CalcManager.CalculationManager;
using CalcManager.UnitConversionManager;
using Command = CalcManager.CalculationManager.Command;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorManagerTest
{
    public class CalculatorManagerDisplayTester : ICalcDisplay
    {
        private string m_primaryDisplay;
        private string m_expression;
        private uint m_parenDisplay;
        private bool m_isError;
        private List<string> m_memorizedNumberStrings = new List<string>();
        private int m_maxDigitsCalledCount;
        private int m_binaryOperatorReceivedCallCount;

        public CalculatorManagerDisplayTester()
        {
            Reset();
        }

        public void Reset()
        {
            m_isError = false;
            m_maxDigitsCalledCount = 0;
            m_binaryOperatorReceivedCallCount = 0;
            m_primaryDisplay = string.Empty;
            m_expression = string.Empty;
            m_memorizedNumberStrings.Clear();
        }

        public void SetPrimaryDisplay(string text, bool isError)
        {
            m_primaryDisplay = text;
            m_isError = isError;
        }

        public void SetIsInError(bool isError)
        {
            m_isError = isError;
        }

        public void SetExpressionDisplay(List<Tuple<string, int>> tokens, List<IExpressionCommand> commands)
        {
            var sb = new StringBuilder();
            if (tokens != null)
            {
                foreach (var pair in tokens)
                {
                    sb.Append(pair.Item1);
                }
            }
            m_expression = sb.ToString();
        }

        public void SetMemorizedNumbers(List<string> numbers)
        {
            m_memorizedNumberStrings = numbers != null ? new List<string>(numbers) : new List<string>();
        }

        public void SetParenthesisNumber(uint parenthesisCount)
        {
            m_parenDisplay = parenthesisCount;
        }

        public void OnNoRightParenAdded()
        {
        }

        public string GetPrimaryDisplay() => m_primaryDisplay;
        public string GetExpression() => m_expression;
        public List<string> GetMemorizedNumbers() => m_memorizedNumberStrings;
        public bool GetIsError() => m_isError;

        public void OnHistoryItemAdded(uint addedItemIndex)
        {
        }

        public void MaxDigitsReached()
        {
            m_maxDigitsCalledCount++;
        }

        public void InputChanged()
        {
        }

        public int GetMaxDigitsCalledCount() => m_maxDigitsCalledCount;

        public void BinaryOperatorReceived()
        {
            m_binaryOperatorReceivedCallCount++;
        }

        public void MemoryItemChanged(uint indexOfMemory)
        {
        }

        public int GetBinaryOperatorReceivedCallCount() => m_binaryOperatorReceivedCallCount;
    }

    public static class TestDriver
    {
        private static CalculatorManagerDisplayTester m_displayTester;
        private static CalculatorManager m_calculatorManager;

        public static void Initialize(CalculatorManagerDisplayTester displayTester, CalculatorManager calculatorManager)
        {
            m_displayTester = displayTester;
            m_calculatorManager = calculatorManager;
        }

        public static void Test(string expectedPrimary, string expectedExpression, Command[] testCommands, bool cleanup = true, bool isScientific = false)
        {
            if (cleanup)
            {
                m_calculatorManager.Reset();
            }

            if (isScientific)
            {
                m_calculatorManager.SendCommand(Command.ModeScientific);
            }

            foreach (var cmd in testCommands)
            {
                if (cmd == Command.CommandNULL) break;
                m_calculatorManager.SendCommand(cmd);
            }

            Assert.AreEqual(expectedPrimary, m_displayTester.GetPrimaryDisplay());
            if (expectedExpression != "N/A")
            {
                Assert.AreEqual(expectedExpression, m_displayTester.GetExpression());
            }
        }
    }

    [TestClass]
    public class CalculatorManagerTest
    {
        private static CalculatorManager m_calculatorManager;
        private static IResourceProvider m_resourceProvider;
        private static CalculatorManagerDisplayTester m_calculatorDisplayTester;

        [TestInitialize]
        public void CommonSetup()
        {
            m_calculatorDisplayTester = new CalculatorManagerDisplayTester();
            m_resourceProvider = new CalculatorUnitTests.EngineResourceProvider();
            m_calculatorManager = new CalculatorManager(m_calculatorDisplayTester, m_resourceProvider);
            TestDriver.Initialize(m_calculatorDisplayTester, m_calculatorManager);
        }

        [TestCleanup]
        public void Cleanup()
        {
            m_calculatorManager?.Reset();
            m_calculatorDisplayTester?.Reset();
        }

        private void ExecuteCommands(IEnumerable<Command> commands)
        {
            foreach (var command in commands)
            {
                if (command == Command.CommandNULL) break;
                m_calculatorManager.SendCommand(command);
            }
        }

        private List<Command> CommandListFromStringInput(string input)
        {
            var result = new List<Command>();
            foreach (char ch in input)
            {
                Command asCommand = Command.CommandNULL;
                if (ch == '.')
                {
                    asCommand = Command.CommandPNT;
                }
                else if ('0' <= ch && ch <= '9')
                {
                    int diff = ch - '0';
                    asCommand = (Command)(diff + (int)Command.Command0);
                }

                if (asCommand != Command.CommandNULL)
                {
                    result.Add(asCommand);
                }
            }
            return result;
        }

        private void TestMaxDigitsReachedScenario(string constInput)
        {
            var pCalculatorDisplay = m_calculatorDisplayTester;

            Assert.AreEqual(0, pCalculatorDisplay.GetMaxDigitsCalledCount());

            var commands = CommandListFromStringInput(constInput);
            Assert.IsTrue(commands.Count > 0);

            Command finalInput = commands[commands.Count - 1];
            commands.RemoveAt(commands.Count - 1);
            string input = constInput.Substring(0, constInput.Length - 1);

            m_calculatorManager.SetStandardMode();
            ExecuteCommands(commands);

            string expectedDisplay = input;
            string display = pCalculatorDisplay.GetPrimaryDisplay();
            Assert.AreEqual(expectedDisplay, display);

            m_calculatorManager.SendCommand(finalInput);

            display = pCalculatorDisplay.GetPrimaryDisplay();
            Assert.AreEqual(expectedDisplay, display);

            Assert.IsTrue(pCalculatorDisplay.GetMaxDigitsCalledCount() > 0);
        }

        [TestMethod]
        public void CalculatorManagerTestStandard()
        {
            TestDriver.Test("123.456", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandPNT, Command.Command4, Command.Command5, Command.Command6, Command.CommandNULL });
            TestDriver.Test("0", "0 + ", new[] { Command.CommandADD, Command.CommandNULL });
            TestDriver.Test("0", "\u221A(0)", new[] { Command.CommandSQRT, Command.CommandNULL });
            TestDriver.Test("7", "4 + 3=", new[] { Command.Command2, Command.CommandADD, Command.Command3, Command.CommandEQU, Command.Command4, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("4", "4=", new[] { Command.Command4, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("2", "\u221A(\u221A(\u221A(256)))", new[] { Command.Command2, Command.Command5, Command.Command6, Command.CommandSQRT, Command.CommandSQRT, Command.CommandSQRT, Command.CommandNULL });
            TestDriver.Test("-9", "-3 \u00D7 3=", new[] { Command.Command3, Command.CommandSUB, Command.Command6, Command.CommandEQU, Command.CommandMUL, Command.Command3, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("46", "54 - 8=", new[] { Command.Command9, Command.CommandMUL, Command.Command6, Command.CommandSUB, Command.CommandCENTR, Command.Command8, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("0.36", "6 \u00D7 0.06=", new[] { Command.Command6, Command.CommandMUL, Command.Command6, Command.CommandPERCENT, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("60", "50 + 10=", new[] { Command.Command5, Command.Command0, Command.CommandADD, Command.Command2, Command.Command0, Command.CommandPERCENT, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("8", "4 + 4=", new[] { Command.Command4, Command.CommandADD, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("3", "5 \u00D7 ", new[] { Command.Command5, Command.CommandADD, Command.CommandMUL, Command.Command3, Command.CommandNULL });
            TestDriver.Test("Overflow", "1.e-9999 \u00F7 ", new[] { Command.Command1, Command.CommandEXP, Command.CommandSIGN, Command.Command9, Command.Command9, Command.Command9, Command.Command9, Command.CommandDIV, Command.Command1, Command.Command0, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("60", "50 + 10=", new[] { Command.Command5, Command.Command0, Command.CommandADD, Command.Command2, Command.Command0, Command.CommandPERCENT, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("Result is undefined", "0 \u00F7 ", new[] { Command.Command0, Command.CommandDIV, Command.Command0, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("Cannot divide by zero", "1 \u00F7 ", new[] { Command.Command1, Command.CommandDIV, Command.Command0, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("14", "14 + ", new[] { Command.Command1, Command.Command2, Command.CommandADD, Command.Command5, Command.CommandCENTR, Command.Command2, Command.CommandADD, Command.CommandNULL });
            TestDriver.Test("-0.01", "1/(-100)", new[] { Command.Command1, Command.Command0, Command.Command0, Command.CommandSIGN, Command.CommandREC, Command.CommandNULL });
            TestDriver.Test("1", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandBACK, Command.CommandBACK, Command.CommandNULL });
            TestDriver.Test("0", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandBACK, Command.CommandBACK, Command.CommandBACK, Command.CommandNULL });
            TestDriver.Test("0", "0 + ", new[] { Command.Command4, Command.CommandSQRT, Command.CommandSUB, Command.Command2, Command.CommandADD, Command.CommandNULL });
            TestDriver.Test("0", "0 + ", new[] { Command.Command1, Command.Command0, Command.Command2, Command.Command4, Command.CommandSQRT, Command.CommandSUB, Command.Command3, Command.Command2, Command.CommandADD, Command.CommandNULL });
            TestDriver.Test("0", "\u221A(2.25) - 1.5=", new[] { Command.Command2, Command.CommandPNT, Command.Command2, Command.Command5, Command.CommandSQRT, Command.CommandSUB, Command.Command1, Command.CommandPNT, Command.Command5, Command.CommandEQU, Command.CommandNULL });
        }

        [TestMethod]
        public void CalculatorManagerTestScientific()
        {
            TestDriver.Test("123.456", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandPNT, Command.Command4, Command.Command5, Command.Command6, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "0 + ", new[] { Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "\u221A(0)", new[] { Command.CommandSQRT, Command.CommandNULL }, true, true);
            TestDriver.Test("1", "1 + 0 \u00D7 2=", new[] { Command.Command1, Command.CommandADD, Command.Command0, Command.CommandMUL, Command.Command2, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("4", "4=", new[] { Command.Command4, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("2", "\u221A(\u221A(\u221A(256)))", new[] { Command.Command2, Command.Command5, Command.Command6, Command.CommandSQRT, Command.CommandSQRT, Command.CommandSQRT, Command.CommandNULL }, true, true);
            TestDriver.Test("-9", "-3 \u00D7 3 + ", new[] { Command.Command3, Command.CommandSUB, Command.Command6, Command.CommandEQU, Command.CommandMUL, Command.Command3, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("38", "9 \u00D7 6 - 8 \u00D7 2 + ", new[] { Command.Command9, Command.CommandMUL, Command.Command6, Command.CommandSUB, Command.CommandCENTR, Command.Command8, Command.CommandMUL, Command.Command2, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("Invalid input", "6 \u00D7 \u221A(-6)", new[] { Command.Command6, Command.CommandMUL, Command.Command6, Command.CommandSIGN, Command.CommandSQRT, Command.CommandNULL }, true, true);
            TestDriver.Test("50.05", "50 + 1/(20) - ", new[] { Command.Command5, Command.Command0, Command.CommandADD, Command.Command2, Command.Command0, Command.CommandREC, Command.CommandSUB, Command.CommandNULL }, true, true);
            TestDriver.Test("8", "4 + 4=", new[] { Command.Command4, Command.CommandADD, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("3", "5 \u00D7 ", new[] { Command.Command5, Command.CommandADD, Command.CommandMUL, Command.Command3, Command.CommandNULL }, true, true);
            TestDriver.Test("Overflow", "1.e-9999 \u00F7 ", new[] { Command.Command1, Command.CommandEXP, Command.CommandSIGN, Command.Command9, Command.Command9, Command.Command9, Command.Command9, Command.CommandDIV, Command.Command1, Command.Command0, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("60", "50 + 10=", new[] { Command.Command5, Command.Command0, Command.CommandADD, Command.Command2, Command.Command0, Command.CommandPERCENT, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("Result is undefined", "0 \u00F7 ", new[] { Command.Command0, Command.CommandDIV, Command.Command0, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("Cannot divide by zero", "1 \u00F7 ", new[] { Command.Command1, Command.CommandDIV, Command.Command0, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("14", "12 + 2 + ", new[] { Command.Command1, Command.Command2, Command.CommandADD, Command.Command5, Command.CommandCENTR, Command.Command2, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("-0.01", "1/(-100)", new[] { Command.Command1, Command.Command0, Command.Command0, Command.CommandSIGN, Command.CommandREC, Command.CommandNULL }, true, true);
            TestDriver.Test("1", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandBACK, Command.CommandBACK, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandBACK, Command.CommandBACK, Command.CommandBACK, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "\u221A(4) - 2 + ", new[] { Command.Command4, Command.CommandSQRT, Command.CommandSUB, Command.Command2, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "\u221A(0)", new[] { Command.Command0, Command.CommandSQRT, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "\u221A(1024) - 32 + ", new[] { Command.Command1, Command.Command0, Command.Command2, Command.Command4, Command.CommandSQRT, Command.CommandSUB, Command.Command3, Command.Command2, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("2.0009748976330773374220277351385", "\u221A(\u221A(\u221A(257)))", new[] { Command.Command2, Command.Command5, Command.Command7, Command.CommandSQRT, Command.CommandSQRT, Command.CommandSQRT, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "\u221A(2.25) - 1.5=", new[] { Command.Command2, Command.CommandPNT, Command.Command2, Command.Command5, Command.CommandSQRT, Command.CommandSUB, Command.Command1, Command.CommandPNT, Command.Command5, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "log(\u221A(2.25) \u00F7 1.5)", new[] { Command.CommandOPENP, Command.Command2, Command.CommandPNT, Command.Command2, Command.Command5, Command.CommandSQRT, Command.CommandDIV, Command.Command1, Command.CommandPNT, Command.Command5, Command.CommandCLOSEP, Command.CommandLOG, Command.CommandNULL }, true, true);
        }

        [TestMethod]
        public void CalculatorManagerTestScientific2()
        {
            TestDriver.Test("144", "sqr(12)", new[] { Command.Command1, Command.Command2, Command.CommandSQR, Command.CommandNULL }, true, true);
            TestDriver.Test("120", "fact(5)", new[] { Command.Command5, Command.CommandFAC, Command.CommandNULL }, true, true);
            TestDriver.Test("25", "5 ^ 2 + ", new[] { Command.Command5, Command.CommandPWR, Command.Command2, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("2", "8 yroot 3 \u00D7 ", new[] { Command.Command8, Command.CommandROOT, Command.Command3, Command.CommandMUL, Command.CommandNULL }, true, true);
            TestDriver.Test("512", "cube(8)", new[] { Command.Command8, Command.CommandCUB, Command.CommandNULL }, true, true);
            TestDriver.Test("8", "cuberoot(cube(8))", new[] { Command.Command8, Command.CommandCUB, Command.CommandCUBEROOT, Command.CommandNULL }, true, true);
            TestDriver.Test("1", "log(10)", new[] { Command.Command1, Command.Command0, Command.CommandLOG, Command.CommandNULL }, true, true);
            TestDriver.Test("100,000", "10^(5)", new[] { Command.Command5, Command.CommandPOW10, Command.CommandNULL }, true, true);
            TestDriver.Test("2.3025850929940456840179914546844", "ln(10)", new[] { Command.Command1, Command.Command0, Command.CommandLN, Command.CommandNULL }, true, true);
            TestDriver.Test("0.01745240643728351281941897851632", "sin\u2080(1)", new[] { Command.Command1, Command.CommandSIN, Command.CommandNULL }, true, true);
            TestDriver.Test("0.99984769515639123915701155881391", "cos\u2080(1)", new[] { Command.Command1, Command.CommandCOS, Command.CommandNULL }, true, true);
            TestDriver.Test("0.01745506492821758576512889521973", "tan\u2080(1)", new[] { Command.Command1, Command.CommandTAN, Command.CommandNULL }, true, true);
            TestDriver.Test("90", "sin\u2080\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandASIN, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "cos\u2080\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandACOS, Command.CommandNULL }, true, true);
            TestDriver.Test("45", "tan\u2080\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandATAN, Command.CommandNULL }, true, true);
            TestDriver.Test("7.389056098930650227230427460575", "e^(2)", new[] { Command.Command2, Command.CommandPOWE, Command.CommandNULL }, true, true);
            TestDriver.Test("1", "5 ^ 0 + ", new[] { Command.Command5, Command.CommandPWR, Command.Command0, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("1", "0 ^ 0 + ", new[] { Command.Command0, Command.CommandPWR, Command.Command0, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("-3", "-27 yroot 3 + ", new[] { Command.Command2, Command.Command7, Command.CommandSIGN, Command.CommandROOT, Command.Command3, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "8 ^ (2 \u00F7 3) - 4 + ", new[] { Command.Command8, Command.CommandPWR, Command.CommandOPENP, Command.Command2, Command.CommandDIV, Command.Command3, Command.CommandCLOSEP, Command.CommandSUB, Command.Command4, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "4 ^ (3 \u00F7 2) - 8 + ", new[] { Command.Command4, Command.CommandPWR, Command.CommandOPENP, Command.Command3, Command.CommandDIV, Command.Command2, Command.CommandCLOSEP, Command.CommandSUB, Command.Command8, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("17.161687912241792074207286679393", "10 ^ 1.23456 + ", new[] { Command.Command1, Command.Command0, Command.CommandPWR, Command.Command1, Command.CommandPNT, Command.Command2, Command.Command3, Command.Command4, Command.Command5, Command.Command6, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("1.0001523280439076654284264342126", "sec\u2080(1)", new[] { Command.Command1, Command.CommandSEC, Command.CommandNULL }, true, true);
            TestDriver.Test("57.298688498550183476612683735174", "csc\u2080(1)", new[] { Command.Command1, Command.CommandCSC, Command.CommandNULL }, true, true);
            TestDriver.Test("57.289961630759424687278147537113", "cot\u2080(1)", new[] { Command.Command1, Command.CommandCOT, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "sec\u2080\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandASEC, Command.CommandNULL }, true, true);
            TestDriver.Test("90", "csc\u2080\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandACSC, Command.CommandNULL }, true, true);
            TestDriver.Test("45", "cot\u2080\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandACOT, Command.CommandNULL }, true, true);
            TestDriver.Test("0.64805427366388539957497735322615", "sech(1)", new[] { Command.Command1, Command.CommandSECH, Command.CommandNULL }, true, true);
            TestDriver.Test("0.85091812823932154513384276328718", "csch(1)", new[] { Command.Command1, Command.CommandCSCH, Command.CommandNULL }, true, true);
            TestDriver.Test("1.3130352854993313036361612469308", "coth(1)", new[] { Command.Command1, Command.CommandCOTH, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "sech\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandASECH, Command.CommandNULL }, true, true);
            TestDriver.Test("0.88137358701954302523260932497979", "csch\u207B\u00B9(1)", new[] { Command.Command1, Command.CommandACSCH, Command.CommandNULL }, true, true);
            TestDriver.Test("0.54930614433405484569762261846126", "coth\u207B\u00B9(2)", new[] { Command.Command2, Command.CommandACOTH, Command.CommandNULL }, true, true);
            TestDriver.Test("256", "2^(8)", new[] { Command.Command8, Command.CommandPOW2, Command.CommandNULL }, true, true);
            TestDriver.Test("1", "N/A", new[] { Command.CommandRand, Command.CommandCeil, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "N/A", new[] { Command.CommandRand, Command.CommandFloor, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "N/A", new[] { Command.CommandRand, Command.CommandSIGN, Command.CommandCeil, Command.CommandNULL }, true, true);
            TestDriver.Test("-1", "N/A", new[] { Command.CommandRand, Command.CommandSIGN, Command.CommandFloor, Command.CommandNULL }, true, true);
            TestDriver.Test("3", "floor(3.8)", new[] { Command.Command3, Command.CommandPNT, Command.Command8, Command.CommandFloor, Command.CommandNULL }, true, true);
            TestDriver.Test("4", "ceil(3.8)", new[] { Command.Command3, Command.CommandPNT, Command.Command8, Command.CommandCeil, Command.CommandNULL }, true, true);
            TestDriver.Test("1.4649735207179271671970404076786", "5 log base 3 + ", new[] { Command.Command5, Command.CommandLogBaseY, Command.Command3, Command.CommandADD, Command.CommandNULL }, true, true);
        }

        [TestMethod]
        public void CalculatorManagerTestScientificParenthesis()
        {
            TestDriver.Test("3", "1 + (0 + 3)", new[] { Command.Command1, Command.CommandADD, Command.CommandOPENP, Command.CommandADD, Command.Command3, Command.CommandCLOSEP, Command.CommandNULL }, true, true);
            TestDriver.Test("12", "((12)", new[] { Command.CommandOPENP, Command.CommandOPENP, Command.Command1, Command.Command2, Command.CommandCLOSEP, Command.CommandNULL }, true, true);
            TestDriver.Test("12", "12 \u00D7 (", new[] { Command.Command1, Command.Command2, Command.CommandCLOSEP, Command.CommandCLOSEP, Command.CommandOPENP, Command.CommandNULL }, true, true);
            TestDriver.Test("4", "2 \u00D7 (2) + ", new[] { Command.Command2, Command.CommandOPENP, Command.Command2, Command.CommandCLOSEP, Command.CommandADD, Command.CommandNULL }, true, true);
            TestDriver.Test("8", "2 \u00D7 (2) + 4=", new[] { Command.Command2, Command.CommandOPENP, Command.Command2, Command.CommandCLOSEP, Command.CommandADD, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("16", "(8) \u00D7 2=", new[] { Command.CommandOPENP, Command.Command8, Command.CommandCLOSEP, Command.Command2, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("28", "(7 \u00D7 2) \u00D7 2=", new[] { Command.CommandOPENP, Command.Command7, Command.CommandMUL, Command.Command2, Command.CommandCLOSEP, Command.Command2, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("28", "(14) \u00D7 2=", new[] { Command.CommandOPENP, Command.Command7, Command.CommandMUL, Command.Command2, Command.CommandCLOSEP, Command.Command2, Command.CommandEQU, Command.CommandOPENP, Command.Command1, Command.Command4, Command.CommandCLOSEP, Command.Command2, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("4", "(8) \u00D7 0.5=", new[] { Command.CommandOPENP, Command.Command8, Command.CommandCLOSEP, Command.Command0, Command.CommandPNT, Command.Command5, Command.CommandEQU, Command.CommandNULL }, true, true);
            TestDriver.Test("4", "(8) \u00D7 0.5=", new[] { Command.CommandOPENP, Command.Command8, Command.CommandCLOSEP, Command.CommandPNT, Command.Command5, Command.CommandEQU, Command.CommandNULL }, true, true);
        }

        [TestMethod]
        public void CalculatorManagerTestScientificError()
        {
            var commands1 = new[] { Command.Command1, Command.CommandDIV, Command.Command0, Command.CommandEQU, Command.CommandNULL };
            TestDriver.Test("Cannot divide by zero", "1 \u00F7 ", commands1, true, true);
            Assert.IsTrue(m_calculatorDisplayTester.GetIsError());

            var commands2 = new[] { Command.Command2, Command.CommandSIGN, Command.CommandLOG, Command.CommandNULL };
            TestDriver.Test("Invalid input", "log(-2)", commands2, true, true);
            Assert.IsTrue(m_calculatorDisplayTester.GetIsError());

            var commands3 = new[] { Command.Command0, Command.CommandDIV, Command.Command0, Command.CommandEQU, Command.CommandNULL };
            TestDriver.Test("Result is undefined", "0 \u00F7 ", commands3, true, true);
            Assert.IsTrue(m_calculatorDisplayTester.GetIsError());

            // Basic calculator tests
            TestDriver.Test("Cannot divide by zero", "1 \u00F7 ", commands1);
            Assert.IsTrue(m_calculatorDisplayTester.GetIsError());
            TestDriver.Test("Invalid input", "log(-2)", commands2);
            Assert.IsTrue(m_calculatorDisplayTester.GetIsError());
            TestDriver.Test("Result is undefined", "0 \u00F7 ", commands3);
            Assert.IsTrue(m_calculatorDisplayTester.GetIsError());
        }

        [TestMethod]
        public void CalculatorManagerTestScientificModeChange()
        {
            TestDriver.Test("0", "N/A", new[] { Command.CommandRAD, Command.CommandPI, Command.CommandSIN, Command.CommandNULL }, true, true);
            TestDriver.Test("-1", "N/A", new[] { Command.CommandRAD, Command.CommandPI, Command.CommandCOS, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "N/A", new[] { Command.CommandRAD, Command.CommandPI, Command.CommandTAN, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "N/A", new[] { Command.CommandGRAD, Command.Command4, Command.Command0, Command.Command0, Command.CommandSIN, Command.CommandNULL }, true, true);
            TestDriver.Test("1", "N/A", new[] { Command.CommandGRAD, Command.Command4, Command.Command0, Command.Command0, Command.CommandCOS, Command.CommandNULL }, true, true);
            TestDriver.Test("0", "N/A", new[] { Command.CommandGRAD, Command.Command4, Command.Command0, Command.Command0, Command.CommandTAN, Command.CommandNULL }, true, true);
        }

        [TestMethod]
        public void CalculatorManagerTestModeChange()
        {
            TestDriver.Test("123", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandNULL }, true, false);
            TestDriver.Test("0", "", new[] { Command.ModeScientific, Command.CommandNULL }, true, false);
            TestDriver.Test("123", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandNULL }, true, false);
            TestDriver.Test("0", "", new[] { Command.ModeProgrammer, Command.CommandNULL }, true, false);
            TestDriver.Test("123", "", new[] { Command.Command1, Command.Command2, Command.Command3, Command.CommandNULL }, true, false);
            TestDriver.Test("0", "", new[] { Command.ModeScientific, Command.CommandNULL }, true, false);
            TestDriver.Test("67", "67 + ", new[] { Command.Command6, Command.Command7, Command.CommandADD, Command.CommandNULL }, true, false);
            TestDriver.Test("0", "", new[] { Command.ModeBasic, Command.CommandNULL }, true, false);
        }

        [TestMethod]
        public void CalculatorManagerTestProgrammer()
        {
            TestDriver.Test("-18", "53 NAND 83 AND ", new[] { Command.ModeProgrammer, Command.Command5, Command.Command3, Command.CommandNand, Command.Command8, Command.Command3, Command.CommandAnd, Command.CommandNULL }, true, false);
            TestDriver.Test("-120", "53 NOR 83 AND ", new[] { Command.ModeProgrammer, Command.Command5, Command.Command3, Command.CommandNor, Command.Command8, Command.Command3, Command.CommandAnd, Command.CommandNULL }, true, false);
            TestDriver.Test("10", "5 Lsh 1 AND ", new[] { Command.ModeProgrammer, Command.Command5, Command.CommandLSHF, Command.Command1, Command.CommandAnd, Command.CommandNULL }, true, false);
            TestDriver.Test("2", "5 Rsh 1 AND ", new[] { Command.ModeProgrammer, Command.Command5, Command.CommandRSHFL, Command.Command1, Command.CommandAnd, Command.CommandNULL }, true, false);
            TestDriver.Test("-128", "-9223372036854775808 Rsh 56 AND ", new[] { Command.ModeProgrammer, Command.CommandBINPOS63, Command.CommandRSHF, Command.Command5, Command.Command6, Command.CommandAnd, Command.CommandNULL }, true, false);
            TestDriver.Test("2", "RoL(1)", new[] { Command.ModeProgrammer, Command.Command1, Command.CommandROL, Command.CommandNULL }, true, false);
            TestDriver.Test("-9,223,372,036,854,775,808", "RoR(1)", new[] { Command.ModeProgrammer, Command.Command1, Command.CommandROR, Command.CommandNULL }, true, false);
            TestDriver.Test("0", "RoR(1)", new[] { Command.ModeProgrammer, Command.Command1, Command.CommandRORC, Command.CommandNULL }, true, false);
            TestDriver.Test("-9,223,372,036,854,775,808", "RoR(RoR(1))", new[] { Command.ModeProgrammer, Command.Command1, Command.CommandRORC, Command.CommandRORC, Command.CommandNULL }, true, false);
            TestDriver.Test("16,843,009", "4294967296 \u00F7 255=", new[] { Command.ModeProgrammer, Command.CommandDec, Command.Command4, Command.Command2, Command.Command9, Command.Command4, Command.Command9, Command.Command6, Command.Command7, Command.Command2, Command.Command9, Command.Command6, Command.CommandDIV, Command.Command2, Command.Command5, Command.Command5, Command.CommandEQU, Command.CommandNULL }, true, false);
            TestDriver.Test("16,843,009", "4294967303 \u00F7 255=", new[] { Command.ModeProgrammer, Command.CommandDec, Command.Command4, Command.Command2, Command.Command9, Command.Command4, Command.Command9, Command.Command6, Command.Command7, Command.Command3, Command.Command0, Command.Command3, Command.CommandDIV, Command.Command2, Command.Command5, Command.Command5, Command.CommandEQU, Command.CommandNULL }, true, false);
            TestDriver.Test("15,507", "1000000000 \u00F7 64487=", new[] { Command.ModeProgrammer, Command.CommandDec, Command.Command1, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.CommandDIV, Command.Command6, Command.Command4, Command.Command4, Command.Command8, Command.Command7, Command.CommandEQU, Command.CommandNULL }, true, false);
            TestDriver.Test("15,506", "1000000000 \u00F7 64488=", new[] { Command.ModeProgrammer, Command.CommandDec, Command.Command1, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.Command0, Command.CommandDIV, Command.Command6, Command.Command4, Command.Command4, Command.Command8, Command.Command8, Command.CommandEQU, Command.CommandNULL }, true, false);
        }

        [TestMethod]
        public void CalculatorManagerTestMemory()
        {
            var pCalculatorDisplay = m_calculatorDisplayTester;

            Cleanup();
            ExecuteCommands(new[] { Command.Command1, Command.CommandSTORE, Command.CommandNULL });
            Assert.AreEqual("1", pCalculatorDisplay.GetPrimaryDisplay());

            Cleanup();
            ExecuteCommands(new[] { Command.Command1, Command.CommandNULL });
            m_calculatorManager.MemorizeNumber();
            m_calculatorManager.SendCommand(Command.CommandCLEAR);
            m_calculatorManager.MemorizedNumberLoad(0);
            Assert.AreEqual("1", pCalculatorDisplay.GetPrimaryDisplay());

            Cleanup();
            m_calculatorManager.SendCommand(Command.Command1);
            m_calculatorManager.MemorizeNumber();
            m_calculatorManager.SendCommand(Command.CommandCLEAR);
            m_calculatorManager.SendCommand(Command.Command2);
            m_calculatorManager.MemorizeNumber();
            m_calculatorManager.SendCommand(Command.CommandCLEAR);
            m_calculatorManager.MemorizedNumberLoad(1);
            Assert.AreEqual("1", pCalculatorDisplay.GetPrimaryDisplay());

            m_calculatorManager.MemorizedNumberLoad(0);
            Assert.AreEqual("2", pCalculatorDisplay.GetPrimaryDisplay());

            Cleanup();
            m_calculatorManager.SendCommand(Command.Command1);
            m_calculatorManager.SendCommand(Command.CommandSIGN);
            m_calculatorManager.MemorizeNumber();
            m_calculatorManager.SendCommand(Command.CommandADD);
            m_calculatorManager.SendCommand(Command.Command2);
            m_calculatorManager.SendCommand(Command.CommandEQU);
            m_calculatorManager.MemorizeNumber();
            m_calculatorManager.SendCommand(Command.CommandMUL);
            m_calculatorManager.SendCommand(Command.Command2);
            m_calculatorManager.MemorizeNumber();

            var memorizedNumbers = pCalculatorDisplay.GetMemorizedNumbers();
            var expectedMemorizedNumbers = new List<string> { "2", "1", "-1" };
            CollectionAssert.AreEqual(expectedMemorizedNumbers, memorizedNumbers.Take(3).ToList());

            m_calculatorManager.SendCommand(Command.CommandCLEAR);
            m_calculatorManager.SendCommand(Command.Command2);
            m_calculatorManager.MemorizedNumberAdd(0);
            m_calculatorManager.MemorizedNumberAdd(1);
            m_calculatorManager.MemorizedNumberAdd(2);

            memorizedNumbers = pCalculatorDisplay.GetMemorizedNumbers();
            expectedMemorizedNumbers = new List<string> { "4", "3", "1" };
            CollectionAssert.AreEqual(expectedMemorizedNumbers, memorizedNumbers.Take(3).ToList());

            m_calculatorManager.SendCommand(Command.CommandCLEAR);
            m_calculatorManager.SendCommand(Command.Command1);
            m_calculatorManager.SendCommand(Command.CommandPNT);
            m_calculatorManager.SendCommand(Command.Command5);

            m_calculatorManager.MemorizedNumberSubtract(0);
            m_calculatorManager.MemorizedNumberSubtract(1);
            m_calculatorManager.MemorizedNumberSubtract(2);

            memorizedNumbers = pCalculatorDisplay.GetMemorizedNumbers();
            expectedMemorizedNumbers = new List<string> { "2.5", "1.5", "-0.5" };
            CollectionAssert.AreEqual(expectedMemorizedNumbers, memorizedNumbers.Take(3).ToList());

            Cleanup();
            for (int i = 0; i < 101; i++)
            {
                m_calculatorManager.SendCommand(Command.Command1);
                m_calculatorManager.MemorizeNumber();
            }

            memorizedNumbers = pCalculatorDisplay.GetMemorizedNumbers();
            Assert.AreEqual(100, memorizedNumbers.Count);

            m_calculatorManager.SendCommand(Command.Command2);
            m_calculatorManager.MemorizeNumber();
            memorizedNumbers = pCalculatorDisplay.GetMemorizedNumbers();
            Assert.AreEqual("2", memorizedNumbers[0]);

            m_calculatorManager.SendCommand(Command.Command2);
            m_calculatorManager.SendCommand(Command.CommandSIGN);
            m_calculatorManager.SendCommand(Command.CommandSQRT);
            m_calculatorManager.MemorizeNumber();
        }

        [TestMethod]
        public void CalculatorManagerTestMaxDigitsReached()
        {
            TestMaxDigitsReachedScenario("1,234,567,891,011,1213");
        }

        [TestMethod]
        public void CalculatorManagerTestMaxDigitsReached_LeadingDecimal()
        {
            TestMaxDigitsReachedScenario("0.12345678910111213");
        }

        [TestMethod]
        public void CalculatorManagerTestMaxDigitsReached_TrailingDecimal()
        {
            TestMaxDigitsReachedScenario("123,456,789,101,112.13");
        }

        [TestMethod]
        public void UnitConversionManagerNumberFormattingUtils_TrimTrailingZeros()
        {
            string number = "2.1032100000000";
            NumberFormattingUtils.TrimTrailingZeros(ref number);
            Assert.AreEqual("2.10321", number);

            number = "-122.123200";
            NumberFormattingUtils.TrimTrailingZeros(ref number);
            Assert.AreEqual("-122.1232", number);

            number = "0.0001200";
            NumberFormattingUtils.TrimTrailingZeros(ref number);
            Assert.AreEqual("0.00012", number);

            number = "12.000";
            NumberFormattingUtils.TrimTrailingZeros(ref number);
            Assert.AreEqual("12", number);

            number = "-12.00000";
            NumberFormattingUtils.TrimTrailingZeros(ref number);
            Assert.AreEqual("-12", number);

            number = "0.000";
            NumberFormattingUtils.TrimTrailingZeros(ref number);
            Assert.AreEqual("0", number);

            number = "322423";
            NumberFormattingUtils.TrimTrailingZeros(ref number);
            Assert.AreEqual("322423", number);
        }

        [TestMethod]
        public void UnitConversionManagerNumberFormattingUtils_GetNumberDigits()
        {
            Assert.AreEqual((uint)6, NumberFormattingUtils.GetNumberDigits("2.10321"));
            Assert.AreEqual((uint)7, NumberFormattingUtils.GetNumberDigits("-122.1232"));
            Assert.AreEqual((uint)4, NumberFormattingUtils.GetNumberDigits("-3432"));
            Assert.AreEqual((uint)1, NumberFormattingUtils.GetNumberDigits("0"));
            Assert.AreEqual((uint)8, NumberFormattingUtils.GetNumberDigits("0.0001223"));
        }

        [TestMethod]
        public void UnitConversionManagerNumberFormattingUtils_GetNumberDigitsWholeNumberPart()
        {
            Assert.AreEqual((uint)1, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(2.10321));
            Assert.AreEqual((uint)3, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(-122.1232));
            Assert.AreEqual((uint)4, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(-3432));
            Assert.AreEqual((uint)1, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(0));
            Assert.AreEqual((uint)15, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(324328412837382));
            Assert.AreEqual((uint)15, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(324328412837382.232213214324234));
            Assert.AreEqual((uint)1, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(0.032));
            Assert.AreEqual((uint)1, NumberFormattingUtils.GetNumberDigitsWholeNumberPart(0.00000000000000000001));
        }

        [TestMethod]
        public void UnitConversionManagerNumberFormattingUtils_RoundSignificantDigits()
        {
            Assert.AreEqual("12.342", NumberFormattingUtils.RoundSignificantDigits(12.342343242, 3));
            Assert.AreEqual("12.343", NumberFormattingUtils.RoundSignificantDigits(12.3429999, 3));
            Assert.AreEqual("12.343", NumberFormattingUtils.RoundSignificantDigits(12.342500001, 3));
            Assert.AreEqual("-2312.12442", NumberFormattingUtils.RoundSignificantDigits(-2312.1244243346454345, 5));
            Assert.AreEqual("0.34234", NumberFormattingUtils.RoundSignificantDigits(0.3423432423, 5));
            Assert.AreEqual("0.3423000", NumberFormattingUtils.RoundSignificantDigits(0.3423, 7));
        }

        [TestMethod]
        public void UnitConversionManagerNumberFormattingUtils_ToScientificNumber()
        {
            Assert.AreEqual("3.423000e+03", NumberFormattingUtils.ToScientificNumber(3423));
            Assert.AreEqual("-2.100000e+01", NumberFormattingUtils.ToScientificNumber(-21));
            Assert.AreEqual("2.320000e-02", NumberFormattingUtils.ToScientificNumber(0.0232));
            Assert.AreEqual("-9.210000e-03", NumberFormattingUtils.ToScientificNumber(-0.00921));
            Assert.AreEqual("2.343243e+12", NumberFormattingUtils.ToScientificNumber(2343243345677));
            Assert.AreEqual("-3.432474e+15", NumberFormattingUtils.ToScientificNumber(-3432474247332942));
            Assert.AreEqual("3.432432e-09", NumberFormattingUtils.ToScientificNumber(0.000000003432432));
            Assert.AreEqual("-3.432432e-09", NumberFormattingUtils.ToScientificNumber(-0.000000003432432));
        }

        [TestMethod]
        public void CalculatorManagerTestBinaryOperatorReceived()
        {
            var pCalculatorDisplay = m_calculatorDisplayTester;
            Assert.AreEqual(0, pCalculatorDisplay.GetBinaryOperatorReceivedCallCount());

            m_calculatorManager.SetStandardMode();
            ExecuteCommands(new[] { Command.Command1, Command.CommandADD });

            Assert.AreEqual("1", pCalculatorDisplay.GetPrimaryDisplay());
            Assert.AreEqual(1, pCalculatorDisplay.GetBinaryOperatorReceivedCallCount());
        }

        [TestMethod]
        public void CalculatorManagerTestBinaryOperatorReceived_Multiple()
        {
            var pCalculatorDisplay = m_calculatorDisplayTester;
            Assert.AreEqual(0, pCalculatorDisplay.GetBinaryOperatorReceivedCallCount());

            m_calculatorManager.SetStandardMode();
            ExecuteCommands(new[] { Command.Command1, Command.CommandADD, Command.CommandSUB, Command.CommandMUL });

            Assert.AreEqual("1", pCalculatorDisplay.GetPrimaryDisplay());
            Assert.AreEqual(3, pCalculatorDisplay.GetBinaryOperatorReceivedCallCount());
        }

        [TestMethod]
        public void CalculatorManagerTestBinaryOperatorReceived_LongInput()
        {
            var pCalculatorDisplay = m_calculatorDisplayTester;
            Assert.AreEqual(0, pCalculatorDisplay.GetBinaryOperatorReceivedCallCount());

            m_calculatorManager.SetStandardMode();
            ExecuteCommands(new[] {
                Command.Command1, Command.CommandADD, Command.Command2, Command.CommandMUL,
                Command.Command1, Command.Command0, Command.CommandSUB, Command.Command5,
                Command.CommandDIV, Command.Command5, Command.CommandEQU
            });

            Assert.AreEqual("5", pCalculatorDisplay.GetPrimaryDisplay());
            Assert.AreEqual(4, pCalculatorDisplay.GetBinaryOperatorReceivedCallCount());
        }

        [TestMethod]
        public void CalculatorManagerTestStandardOrderOfOperations()
        {
            TestDriver.Test("1", "1/(1)", new[] { Command.Command1, Command.CommandREC, Command.CommandNULL });
            TestDriver.Test("2", "\u221A(4)", new[] { Command.Command4, Command.CommandSQRT, Command.CommandNULL });
            TestDriver.Test("2", "1 + \u221A(4)", new[] { Command.Command1, Command.CommandADD, Command.Command4, Command.CommandSQRT, Command.CommandNULL });
            TestDriver.Test("3", "3 - ", new[] { Command.Command1, Command.CommandADD, Command.Command4, Command.CommandSQRT, Command.CommandSUB, Command.CommandNULL });
            TestDriver.Test("0.25", "2 \u00D7 1/(4)", new[] { Command.Command2, Command.CommandMUL, Command.Command4, Command.CommandREC, Command.CommandNULL });
            TestDriver.Test("0.06", "5 \u00F7 0.06", new[] { Command.Command5, Command.CommandDIV, Command.Command6, Command.CommandPERCENT, Command.CommandNULL });
            TestDriver.Test("2", "\u221A(4) - ", new[] { Command.Command4, Command.CommandSQRT, Command.CommandSUB, Command.CommandNULL });
            TestDriver.Test("49", "sqr(7) \u00F7 ", new[] { Command.Command7, Command.CommandSQR, Command.CommandDIV, Command.CommandNULL });
            TestDriver.Test("8", "\u221A(sqr(8))", new[] { Command.Command8, Command.CommandSQR, Command.CommandSQRT, Command.CommandNULL });
            TestDriver.Test("12", "12 - ", new[] { Command.Command1, Command.Command0, Command.CommandADD, Command.Command2, Command.CommandSUB, Command.CommandNULL });
            TestDriver.Test("12", "12 \u00F7 ", new[] { Command.Command3, Command.CommandMUL, Command.Command4, Command.CommandDIV, Command.CommandNULL });
            TestDriver.Test("2", "2 + ", new[] { Command.Command6, Command.CommandDIV, Command.Command3, Command.CommandSUB, Command.CommandADD, Command.CommandNULL });
            TestDriver.Test("3", "3 \u00D7 ", new[] { Command.Command7, Command.CommandSUB, Command.Command4, Command.CommandDIV, Command.CommandMUL, Command.CommandNULL });
            TestDriver.Test("4", "16 + \u221A(16)", new[] { Command.Command8, Command.CommandMUL, Command.Command2, Command.CommandADD, Command.CommandSQRT, Command.CommandNULL });
            TestDriver.Test("-9", "9 \u00D7 negate(9)", new[] { Command.Command9, Command.CommandADD, Command.Command0, Command.CommandMUL, Command.CommandSIGN, Command.CommandNULL });
            TestDriver.Test("-90", "-90 \u00D7 ", new[] { Command.Command9, Command.CommandSIGN, Command.Command0, Command.CommandADD, Command.CommandMUL, Command.CommandNULL });
            TestDriver.Test("3", "1 + 2=", new[] { Command.Command1, Command.CommandADD, Command.Command2, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("40", "20 \u00D7 2=", new[] { Command.Command2, Command.Command0, Command.CommandMUL, Command.Command0, Command.Command2, Command.CommandEQU, Command.CommandNULL });
            TestDriver.Test("3", "3 + ", new[] { Command.Command1, Command.CommandADD, Command.Command2, Command.CommandADD, Command.CommandBACK, Command.CommandNULL });
            TestDriver.Test("0", "", new[] { Command.Command1, Command.CommandADD, Command.Command2, Command.CommandADD, Command.CommandCLEAR, Command.CommandNULL });
            TestDriver.Test("0", "3 + ", new[] { Command.Command1, Command.CommandADD, Command.Command2, Command.CommandADD, Command.CommandCENTR, Command.CommandNULL });
            TestDriver.Test("0", "", new[] { Command.Command1, Command.CommandADD, Command.Command2, Command.CommandCLEAR, Command.CommandNULL });
            TestDriver.Test("0", "1 + ", new[] { Command.Command1, Command.CommandADD, Command.Command2, Command.CommandCENTR, Command.CommandNULL });
            TestDriver.Test("120", "120 \u00D7 ", new[] { Command.Command1, Command.CommandMUL, Command.Command2, Command.CommandMUL, Command.Command3, Command.CommandMUL, Command.Command4, Command.CommandMUL, Command.Command5, Command.CommandMUL, Command.CommandNULL });
        }
    }
}
