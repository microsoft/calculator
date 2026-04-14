// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class HistoryTests
    {
        // Command constants matching C++ Command enum values
        private const int CommandNULL = 0;
        private const int CommandSIGN = 80;
        private const int CommandCLEAR = 81;
        private const int CommandCENTR = 82;
        private const int CommandBACK = 83;
        private const int CommandDIV = 91;
        private const int CommandMUL = 92;
        private const int CommandADD = 93;
        private const int CommandSUB = 94;
        private const int CommandSIN = 102;
        private const int CommandCOS = 103;
        private const int CommandTAN = 104;
        private const int CommandSQRT = 110;
        private const int CommandSQR = 111;
        private const int CommandREC = 114;
        private const int CommandPERCENT = 118;
        private const int CommandEQU = 121;
        private const int Command0 = 130;
        private const int Command1 = 131;
        private const int Command2 = 132;
        private const int Command3 = 133;
        private const int Command4 = 134;
        private const int Command5 = 135;
        private const int Command6 = 136;
        private const int Command7 = 137;
        private const int Command8 = 138;
        private const int Command9 = 139;
        private const int ModeBasic = 200;
        private const int ModeScientific = 201;
        private const int CommandASIN = 202;
        private const int CommandACOS = 203;
        private const int CommandATAN = 204;
        private const int ModeProgrammer = 209;
        private const int CommandDEG = 321;
        private const int CommandRAD = 322;
        private const int CommandGRAD = 323;

        private StandardCalculatorViewModel m_standardViewModel;
        private HistoryViewModel m_historyViewModel;

        private void Initialize()
        {
            m_standardViewModel = new StandardCalculatorViewModel();
            m_standardViewModel.IsStandard = true;
            m_historyViewModel = m_standardViewModel.HistoryVM;
        }

        private void Cleanup()
        {
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_historyViewModel.OnClearCommand(null);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.OnClearCommand(null);
            m_standardViewModel.ResetCalcManager(false);
        }

        private void HistoryStandardOrderOfOperationsHelper(string expectedResult, string expectedExpression, int[] testCommands)
        {
            Initialize();
            int initialSize = m_historyViewModel.ItemsCount;
            foreach (int command in testCommands)
            {
                if (command == CommandNULL)
                    break;
                m_standardViewModel.SendCommandToCalcManager(command);
            }
            int sizeAfterCommandsAdd = m_historyViewModel.ItemsCount;
            if (string.IsNullOrEmpty(expectedResult))
            {
                Assert.AreEqual(initialSize, sizeAfterCommandsAdd);
            }
            else
            {
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
                Assert.AreEqual(initialSize + 1, sizeAfterCommandsAdd);
                Assert.AreEqual(expectedExpression, historyItem.Expression);
                Assert.AreEqual(expectedResult, historyItem.Result);
            }
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemClicked()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command3);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("9", m_standardViewModel.DisplayValue);
            Assert.AreEqual("1", m_standardViewModel.ExpressionTokens[0].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[1].Token);
            Assert.AreEqual("+", m_standardViewModel.ExpressionTokens[2].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[3].Token);
            Assert.AreEqual("5", m_standardViewModel.ExpressionTokens[4].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[5].Token);
            Assert.AreEqual("+", m_standardViewModel.ExpressionTokens[6].Token);
            Assert.AreEqual(" ", m_standardViewModel.ExpressionTokens[7].Token);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemAddSingleItem()
        {
            Initialize();
            int initialSize = m_historyViewModel.ItemsCount;
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command8);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            int sizeAfterItemAdd = m_historyViewModel.ItemsCount;
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
            string expression = "1   +   8 =";
            Assert.AreEqual(initialSize + 1, sizeAfterItemAdd);
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual("9", historyItem.Result);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryItemAddMaxItems()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            for (ulong i = 1; i < m_historyViewModel.GetMaxItemSize(); i++)
            {
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(Command2);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }
            Assert.AreEqual((int)m_historyViewModel.GetMaxItemSize(), m_historyViewModel.ItemsCount);
            string expression = "1   +   1 =";
            int output = 2;
            string result = output.ToString();
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual(result, historyItem.Result);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            expression = "1   +   2 =";
            output = 3;
            result = output.ToString();
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            Assert.AreEqual(expression, historyItem.Expression);
            Assert.AreEqual(result, historyItem.Result);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryClearCommand()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            m_historyViewModel.OnClearCommand(null);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryClearCommandWithEmptyHistory()
        {
            Initialize();
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.OnClearCommand(null);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }

        [TestMethod]
        public void TestReLoadHistory()
        {
            Initialize();

            m_standardViewModel.ResetCalcManager(false);
            int scientificItems = 5;
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            for (int i = 0; i < scientificItems; i++)
            {
                int nextCommand = 130 + i;
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(nextCommand);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }

            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            int standardItems = 2;
            for (int i = 0; i < standardItems; i++)
            {
                int nextCommand = 130 + i;
                m_standardViewModel.SendCommandToCalcManager(Command1);
                m_standardViewModel.SendCommandToCalcManager(CommandADD);
                m_standardViewModel.SendCommandToCalcManager(nextCommand);
                m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            }

            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_historyViewModel.ReloadHistory(ViewMode.Scientific);
            Assert.AreEqual(scientificItems, m_historyViewModel.ItemsCount);
            for (int i = 0; i < scientificItems; i++)
            {
                string expr = "1   +   " + i.ToString() + " =";
                int output = 1 + i;
                string result = output.ToString();
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1 - i];
                Assert.AreEqual(expr, historyItem.Expression);
                Assert.AreEqual(result, historyItem.Result);
            }

            m_historyViewModel.ReloadHistory(ViewMode.Standard);
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            Assert.AreEqual(standardItems, m_historyViewModel.ItemsCount);
            for (int i = 0; i < standardItems; i++)
            {
                string expr = "1   +   " + i.ToString() + " =";
                int output = 1 + i;
                string result = output.ToString();
                var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1 - i];
                Assert.AreEqual(expr, historyItem.Expression);
                Assert.AreEqual(result, historyItem.Result);
            }
            Cleanup();
        }

        [TestMethod]
        [Ignore("Requires UWP ResourceLoader")]
        public void TestHistoryItemWithPrettyExpressions()
        {
            // This test requires Windows.ApplicationModel.Resources.ResourceLoader
            // which is not available in .NET 8.0 (non-UWP) test projects.
        }

        [TestMethod]
        [Ignore("Requires UWP ResourceLoader")]
        public void TestHistoryItemWithPrettyExpressionsMixedRadix()
        {
            // This test requires Windows.ApplicationModel.Resources.ResourceLoader
            // which is not available in .NET 8.0 (non-UWP) test projects.
        }

        [TestMethod]
        public void TestHistoryItemLoadAndContinueCalculation()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeBasic);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command3);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);

            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);

            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("11", m_standardViewModel.DisplayValue);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 1];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("6", m_standardViewModel.DisplayValue);

            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[m_historyViewModel.ItemsCount - 2];
            m_standardViewModel.SelectHistoryItem(historyItem);
            Assert.AreEqual("9", m_standardViewModel.DisplayValue);
            Cleanup();
        }

        [TestMethod]
        public void TestDisplayValueAutomationNames()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command8);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 9", m_standardViewModel.CalculationResultAutomationName);

            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command5);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 6", m_standardViewModel.CalculationResultAutomationName);

            m_standardViewModel.SendCommandToCalcManager(ModeProgrammer);
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command2);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("Display is 3", m_standardViewModel.CalculationResultAutomationName);

            Cleanup();
        }

        [TestMethod]
        public void TestRadixAutomationName()
        {
            Initialize();
            m_standardViewModel.SendCommandToCalcManager(ModeProgrammer);
            m_standardViewModel.IsProgrammer = true;
            m_standardViewModel.SendCommandToCalcManager(Command1);
            m_standardViewModel.SendCommandToCalcManager(CommandADD);
            m_standardViewModel.SendCommandToCalcManager(Command7);
            m_standardViewModel.SendCommandToCalcManager(CommandEQU);
            Assert.AreEqual("HexaDecimal 8", "HexaDecimal " + m_standardViewModel.HexDisplayValue);
            Assert.AreEqual("Octal 10", "Octal " + m_standardViewModel.OctalDisplayValue);
            Assert.AreEqual("Binary 1000", "Binary " + m_standardViewModel.BinaryDisplayValue);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryEmpty()
        {
            Initialize();
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            m_standardViewModel.SendCommandToCalcManager(ModeScientific);
            Assert.AreEqual(0, m_historyViewModel.ItemsCount);
            Cleanup();
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_1()
        {
            int[] commands = { Command1, CommandREC, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_2()
        {
            int[] commands = { Command4, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_3()
        {
            int[] commands = { Command1, CommandADD, Command4, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_4()
        {
            int[] commands = { Command1, CommandADD, Command4, CommandSQRT, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   \x221A( 4 ) =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_5()
        {
            int[] commands = { Command2, CommandMUL, Command4, CommandREC, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_6()
        {
            int[] commands = { Command5, CommandDIV, Command6, CommandPERCENT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_7()
        {
            int[] commands = { Command4, CommandSQRT, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_8()
        {
            int[] commands = { Command7, CommandSQR, CommandDIV, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_9()
        {
            int[] commands = { Command8, CommandSQR, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_10()
        {
            int[] commands = { Command1, Command0, CommandADD, Command2, CommandSUB, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("12", "10   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_11()
        {
            int[] commands = { Command3, CommandMUL, Command4, CommandDIV, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("12", "3   \x00D7   4 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_12()
        {
            int[] commands = { Command6, CommandDIV, Command3, CommandSUB, CommandADD, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("2", "6   \x00F7   3 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_13()
        {
            int[] commands = { Command7, CommandSUB, Command4, CommandDIV, CommandMUL, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "7   -   4 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_14()
        {
            int[] commands = { Command8, CommandMUL, Command2, CommandADD, CommandSQRT, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("16", "8   \x00D7   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_15()
        {
            int[] commands = { Command9, CommandADD, Command0, CommandMUL, CommandSIGN, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("9", "9   +   0 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_16()
        {
            int[] commands = { Command9, CommandSIGN, Command0, CommandADD, CommandMUL, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_17()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandEQU, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_18()
        {
            int[] commands = { Command2, Command0, CommandMUL, Command0, Command2, CommandEQU, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("40", "20   \x00D7   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_19()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandBACK, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_20()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandCLEAR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_21()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandADD, CommandCENTR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("3", "1   +   2 =", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_22()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandCLEAR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperations_23()
        {
            int[] commands = { Command1, CommandADD, Command2, CommandCENTR, CommandNULL };
            HistoryStandardOrderOfOperationsHelper("", "", commands);
        }

        [TestMethod]
        public void TestHistoryStandardOrderOfOperationsMultiple()
        {
            Initialize();
            int[] commands = { Command1, CommandMUL, Command2, CommandMUL, Command3, CommandMUL, Command4, CommandMUL, Command5, CommandMUL, CommandNULL };
            int initialSize = m_historyViewModel.ItemsCount;
            foreach (int command in commands)
            {
                if (command == CommandNULL)
                    break;
                m_standardViewModel.SendCommandToCalcManager(command);
            }
            int sizeAfterCommandsAdd = m_historyViewModel.ItemsCount;
            Assert.AreEqual(initialSize + 4, sizeAfterCommandsAdd);
            var historyItem = (HistoryItemViewModel)m_historyViewModel.Items[0];
            Assert.AreEqual("24   \x00D7   5 =", historyItem.Expression);
            Assert.AreEqual("120", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[1];
            Assert.AreEqual("6   \x00D7   4 =", historyItem.Expression);
            Assert.AreEqual("24", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[2];
            Assert.AreEqual("2   \x00D7   3 =", historyItem.Expression);
            Assert.AreEqual("6", historyItem.Result);
            historyItem = (HistoryItemViewModel)m_historyViewModel.Items[3];
            Assert.AreEqual("1   \x00D7   2 =", historyItem.Expression);
            Assert.AreEqual("2", historyItem.Result);
            Cleanup();
        }
    }
}
