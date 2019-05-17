using CalculatorUITestFramework;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Remote;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

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
            
            page.StandardCalculatorSetup(context);
        }

        /// <summary>
        /// Closes the app and WinAppDriver web driver session.
        /// </summary>
        [ClassCleanup]
        public static void ClassCleanup()
        {
            page.StandardCalculatorTearDown();
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
            // Find the buttons by their names and click them in sequence to peform 1 + 7 = 8
            page.StandardOperators.Add(new List<double>() { 3.5, 0.25 });
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
            // Find the buttons by their names and click them in sequence to peform 1 + 7 = 8
            page.StandardOperators.Subtract(new List<double>() { 4.3, 2.6 });
            Assert.AreEqual("1.7", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_History()
        {
            page.StandardOperators.Add(new List<double>() { -3, -2.6 });
            page.StandardOperators.Subtract(new List<double>() { 2, 3 });
            page.StandardOperators.Multiply(new List<double>() { 1, 3 });
            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("1 × 3 = 3", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("2 Minus ( 3 = Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[2].Text.Equals("-3 + -2.6 = Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

        }

        [TestMethod]
        public void SmokeTest_Memory()
        {
            page.StandardOperators.Add(new List<double>() { 3, 0 });
            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.Divide(new List<double>() { 2, 3 });
            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.Multiply(new List<double>() { 7, 9 });
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
            page.StandardOperators.Reciprocal(4);
            Assert.AreEqual("0.25", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Squared()
        {
            page.StandardOperators.Square(-15.5);
            Assert.AreEqual("240.25", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_SquareRoot()
        {
            page.StandardOperators.SquareRoot(144);
            Assert.AreEqual("12", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Cubed()
        {
            page.StandardOperators.Cube(-3);
            Assert.AreEqual("-27", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Percent()
        {
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(600);
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(15);
            page.StandardOperators.PercentButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("90", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Delete()
        {
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(-12345);
            page.StandardOperators.BackSpaceButton.Click();
            Assert.AreEqual("-1,234", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_ClearAll()
        {
            page.StandardOperators.Add(new List<double>() { 12345, 6789 });
            page.StandardOperators.ClearButton.Click();
            Assert.AreEqual("0", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_ClearEntry()
        {
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(-12345);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(5678);
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
        public void KeyboardInput_Reciprocal()
        {
            page.Header.SendKeys("10r");
            Assert.AreEqual("0.1", page.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Squared()
        {
            page.Header.SendKeys("3q");
            Assert.AreEqual("9", page.GetCalculatorResultText());
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
            page.StandardOperators.Divide(new List<double>() { 12, 3 });
            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(15);
            page.MemoryPanel.MemPlus.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("19", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_SubtractTest()
        {
            page.StandardOperators.Divide(new List<double>() { 12, 3 });
            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(3.3);
            page.MemoryPanel.MemMinus.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("0.7", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_RecallTest()
        {
            page.StandardOperators.Divide(new List<double>() { 12, 3 });
            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(3.3);
            page.MemoryPanel.MemRecall.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("4", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_ClearTest()
        {
            page.StandardOperators.Divide(new List<double>() { 12, 3 });
            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.TranslateNumberToButtonClicks(3.3);
            page.MemoryPanel.MemoryClear.Click();
            page.MemoryPanel.OpenMemoryPanel();
            Assert.IsNotNull(page.MemoryPanel.MemoryPaneEmptyLabel);
        }
        #endregion
    }
}
