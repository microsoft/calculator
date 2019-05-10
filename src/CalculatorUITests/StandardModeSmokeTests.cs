using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;
using System;
using CalculatorUITestFramework;

namespace CalculatorUITests
{
    [TestClass]
    public class StandardModeSmokeTests
    {
        private StandardOperatorsPanel standardOperators => StandardCalculatorPage.StandardOperators;

        [TestMethod]
        public void SmokeTest_Add()
        {
            // Find the buttons by their names and click them in sequence to peform 1 + 7 = 8
            standardOperators.Add(new List<double>() { 3.5, 0.25 });
            Assert.AreEqual("3.75", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_Add_KeyboardInput()
        {
            StandardCalculatorPage.SendKeys("3+3=");
            Assert.AreEqual("6", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_Subtract()
        {
            // Find the buttons by their names and click them in sequence to peform 1 + 7 = 8
            standardOperators.Subtract(new List<double>() { 4.3, 2.6 });
            Assert.AreEqual("1.7", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_History()
        {
            standardOperators.Add(new List<double>() { -3, -2.6 });
            standardOperators.Subtract(new List<double>() { 2, 3 });
            standardOperators.Multiply(new List<double>() { 1, 3 });
            var historyItems = StandardCalculatorPage.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("1 × 3 = 3", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("2 Minus ( 3 = Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[2].Text.Equals("-3 + -2.6 = Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

        }

        [TestMethod]
        public void SmokeTest_Memory()
        {
            standardOperators.Add(new List<double>() { 3, 0 });
            StandardCalculatorPage.MemoryPanel.MemButton.Click();
            standardOperators.Divide(new List<double>() { 2, 3 });
            StandardCalculatorPage.MemoryPanel.MemButton.Click();
            standardOperators.Multiply(new List<double>() { 7, 9 });
            StandardCalculatorPage.MemoryPanel.MemButton.Click();
            var memoryItems = StandardCalculatorPage.MemoryPanel.GetAllMemoryListViewItems();

            Assert.IsTrue(memoryItems[0].Text.Equals("63", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(memoryItems[1].Text.Equals("0.6666666666666667‬", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(memoryItems[2].Text.Equals("3", StringComparison.InvariantCultureIgnoreCase));
            return;

        }

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            StandardCalculatorPage.StandardCalculatorSetup(context);
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            StandardCalculatorPage.TearDown();
        }

        [TestInitialize]
        public void TestInit()
        {
            StandardCalculatorPage.ClearAll(false);
            Assert.AreEqual("0", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestCleanup]
        public void TestCleanup()
        {
            StandardCalculatorPage.ClearAll();
        }
    }
}
