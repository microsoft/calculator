// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorUITestFramework;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;

namespace CalculatorUITests
{
    [TestClass]
    public class StandardModeFunctionalTests
    {
        private static StandardCalculatorPage page = new StandardCalculatorPage();

        /// <summary>
        /// Initializes the WinAppDriver web driver session.
        /// </summary>
        /// <param name="context"></param>
        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            // Create session to launch a Calculator window
            WinAppDriver.Instance.SetupCalculatorSession(context);

            // Ensure that calculator is in standard mode
            page.NavigateToStandardCalculator();
        }

        /// <summary>
        /// Closes the app and WinAppDriver web driver session.
        /// </summary>
        [ClassCleanup]
        public static void ClassCleanup()
        {
            // Tear down Calculator session.
            WinAppDriver.Instance.TearDownCalculatorSession();
        }

        /// <summary>
        /// Ensures the calculator is in a cleared state
        /// </summary>
        [TestInitialize]
        public void TestInit()
        {
            if ("0" != page.GetCalculatorResultText())
            {
                page.ClearAll();
            }
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.ClearAll();
        }

        #region Smoke Tests
        [TestMethod]
        public void SmokeTest_Add()
        {
            page.StandardOperators.NumberPad.Input(3.5);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(0.25);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("3.75", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_Add_KeyboardInput()
        {
            page.Header.SendKeys("3+3=");
            Assert.AreEqual("6", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_Subtract()
        {
            page.StandardOperators.NumberPad.Input(4.3);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(2.6);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1.7", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_History()
        {
            page.StandardOperators.NumberPad.Input(-3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(-2.6);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("1 × 3 = 3", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("2 Minus ( 3 = Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[2].Text.Equals("-3 + -2.6 = Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

        }

        [TestMethod]
        public void SmokeTest_Memory()
        {
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(0);
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemButton.Click();

            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Input(9);
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemButton.Click();

            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();

            Assert.IsTrue(memoryItems[0].Text.Equals("63", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(memoryItems[1].Text.Equals("0.6666666666666667‬", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(memoryItems[2].Text.Equals("3", StringComparison.InvariantCultureIgnoreCase));
            return;

        }

        #endregion

        #region Basic Arthimetic Tests
        [TestMethod]
        public void Operator_Reciprocal()
        {
            page.StandardOperators.NumberPad.Input(4);
            page.StandardOperators.InvertButton.Click();
            Assert.AreEqual("0.25", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Squared()
        {
            page.StandardOperators.NumberPad.Input(-15.5);
            page.StandardOperators.XPower2Button.Click();
            Assert.AreEqual("240.25", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_SquareRoot()
        {
            page.StandardOperators.NumberPad.Input(144);
            page.StandardOperators.SquareRootButton.Click();
            Assert.AreEqual("12", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Cubed()
        {
            page.StandardOperators.NumberPad.Input(-3);
            page.StandardOperators.XPower3Button.Click();
            Assert.AreEqual("-27", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Percent()
        {
            page.StandardOperators.NumberPad.Input(600);
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Input(15);
            page.StandardOperators.PercentButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("90", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Delete()
        {
            page.StandardOperators.NumberPad.Input(-12345);
            page.StandardOperators.BackSpaceButton.Click();
            Assert.AreEqual("-1,234", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_ClearAll()
        {
            page.StandardOperators.NumberPad.Input(12345);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(6789);
            page.StandardOperators.ClearButton.Click();
            Assert.AreEqual("0", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_ClearEntry()
        {
            page.StandardOperators.NumberPad.Input(-12345);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(5678);
            page.StandardOperators.ClearEntryButton.Click();
            Assert.AreEqual("0", page.GetCalculatorResultText());
        }
        #endregion

        #region Keyboard Input Tests
        [TestMethod]
        public void KeyboardInput_Subtract()
        {
            page.Header.SendKeys("70090-890987=");
            Assert.AreEqual("-820,897", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Multiply()
        {
            page.Header.SendKeys("-15*3=");
            Assert.AreEqual("-45", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Divide()
        {
            page.Header.SendKeys("26/13=");
            Assert.AreEqual("2", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_SquareRoot()
        {
            page.Header.SendKeys("100@");
            Assert.AreEqual("10", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Cubed()
        {
            page.Header.SendKeys("3#");
            Assert.AreEqual("27", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Percent()
        {
            page.Header.SendKeys("34+7%=");
            Assert.AreEqual("36.38", page.GetCalculatorResultText());
        }

        #endregion

        #region Memory Tests
        [TestMethod]
        public void Memory_AddTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(15);
            page.MemoryPanel.MemPlus.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("19", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_SubtractTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(3.3);
            page.MemoryPanel.MemMinus.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("0.7", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_RecallTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(3.3);
            page.MemoryPanel.MemRecall.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("4", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_ClearTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(3.3);
            page.MemoryPanel.MemoryClear.Click();
            page.MemoryPanel.OpenMemoryPanel();
            Assert.IsNotNull(page.MemoryPanel.MemoryPaneEmptyLabel);
        }
        #endregion
    }
}
