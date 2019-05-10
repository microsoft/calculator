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
        private StandardOperatorsPanel standardOperators => StandardCalculatorPage.StandardOperators;

        /// <summary>
        /// Initializes the WinAppDriver web driver session.
        /// </summary>
        /// <param name="context"></param>
        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            StandardCalculatorPage.StandardCalculatorSetup(context);
        }

        /// <summary>
        /// Closes the app and WinAppDriver web driver session.
        /// </summary>
        [ClassCleanup]
        public static void ClassCleanup()
        {
            StandardCalculatorPage.TearDown();
        }

        /// <summary>
        /// Ensures the calculator is in a cleared state
        /// </summary>
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

        #region Basic Arthimetic Tests
        [TestMethod]
        public void BVT_Reciprocal()
        {
            standardOperators.Reciprocal(4);
            Assert.AreEqual("0.25", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void BVT_Squared()
        {
            standardOperators.Square(-15.5);
            Assert.AreEqual("240.25", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void BVT_SquareRoot()
        {
            standardOperators.SquareRoot(144);
            Assert.AreEqual("12", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void BVT_Cubed()
        {
            standardOperators.Cube(-3);
            Assert.AreEqual("-27", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void BVT_Percent()
        {
            standardOperators.NumberPad.TranslateNumberToButtonClicks(600);
            standardOperators.MultiplyButton.Click();
            standardOperators.NumberPad.TranslateNumberToButtonClicks(15);
            standardOperators.PercentButton.Click();
            standardOperators.EqualButton.Click();
            Assert.AreEqual("90", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void BVT_Delete()
        {
            standardOperators.NumberPad.TranslateNumberToButtonClicks(-12345);
            standardOperators.BackSpaceButton.Click();
            Assert.AreEqual("-1,234", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void BVT_ClearAll()
        {
            standardOperators.Add(new List<double>() { 12345, 6789 });
            standardOperators.ClearButton.Click();
            Assert.AreEqual("0", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void BVT_ClearEntry()
        {
            standardOperators.NumberPad.TranslateNumberToButtonClicks(-12345);
            standardOperators.MinusButton.Click();
            standardOperators.NumberPad.TranslateNumberToButtonClicks(5678);
            standardOperators.ClearEntryButton.Click();
            Assert.AreEqual("0", StandardCalculatorPage.GetCalculatorResultText());
        }
        #endregion

        #region Keyboard Input Tests
        [TestMethod]
        public void KeyboardInput_Subtract()
        {
            StandardCalculatorPage.SendKeys("70090-890987=");
            Assert.AreEqual("-820,897", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Multiply()
        {
            StandardCalculatorPage.SendKeys("-15*3=");
            Assert.AreEqual("-45", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Divide()
        {
            StandardCalculatorPage.SendKeys("26/13=");
            Assert.AreEqual("2", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Reciprocal()
        {
            StandardCalculatorPage.SendKeys("10r");
            Assert.AreEqual("0.1", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Squared()
        {
            StandardCalculatorPage.SendKeys("3q");
            Assert.AreEqual("9", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_SquareRoot()
        {
            StandardCalculatorPage.SendKeys("100@");
            Assert.AreEqual("10", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Cubed()
        {
            StandardCalculatorPage.SendKeys("3#");
            Assert.AreEqual("27", StandardCalculatorPage.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Percent()
        {
            StandardCalculatorPage.SendKeys("34+7%=");
            Assert.AreEqual("36.38", StandardCalculatorPage.GetCalculatorResultText());
        }

        [Ignore]
        [TestMethod]
        public void KeyboardInput_Delete()
        {
            // Sending the Backspace key is not working yet.

            //StandardCalculatorPage.SendKeys("34+700");
            //Assert.AreEqual("700", StandardCalculatorPage.GetCalculatorResultText());
            //StandardCalculatorPage.SendKeys(Keys.Backspace);
            //Assert.AreEqual("70", StandardCalculatorPage.GetCalculatorResultText());
        }

        [Ignore]
        [TestMethod]
        public void KeyboardInput_ClearAll()
        {
            // Sending the Escape key is not working yet.

            //StandardCalculatorPage.SendKeys("3+3=");
            //Assert.AreEqual("6", StandardCalculatorPage.GetCalculatorResultText());
            //StandardCalculatorPage.SendKeys(Keys.Escape);
            //Assert.AreEqual("0", StandardCalculatorPage.GetCalculatorResultText());
        }

        [Ignore]
        [TestMethod]
        public void KeyboardInput_ClearEntry()
        {
            //Sending the Delete key is not working yet.

            //StandardCalculatorPage.SendKeys("3+3");
            //Assert.AreEqual("3", StandardCalculatorPage.GetCalculatorResultText());
            //StandardCalculatorPage.SendKeys(Keys.Delete);
            //Assert.AreEqual("0", StandardCalculatorPage.GetCalculatorResultText());
        }

        #endregion

        #region Memory Tests
        [TestMethod]
        public void Memory_AddTest()
        {
            standardOperators.Divide(new List<double>() { 12, 3 });
            StandardCalculatorPage.MemoryPanel.MemButton.Click();
            standardOperators.NumberPad.TranslateNumberToButtonClicks(15);
            StandardCalculatorPage.MemoryPanel.MemPlus.Click();
            var memoryItems = StandardCalculatorPage.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("19", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_SubtractTest()
        {
            standardOperators.Divide(new List<double>() { 12, 3 });
            StandardCalculatorPage.MemoryPanel.MemButton.Click();
            standardOperators.NumberPad.TranslateNumberToButtonClicks(3.3);
            StandardCalculatorPage.MemoryPanel.MemMinus.Click();
            var memoryItems = StandardCalculatorPage.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("0.7", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_RecallTest()
        {
            standardOperators.Divide(new List<double>() { 12, 3 });
            StandardCalculatorPage.MemoryPanel.MemButton.Click();
            standardOperators.NumberPad.TranslateNumberToButtonClicks(3.3);
            StandardCalculatorPage.MemoryPanel.MemRecall.Click();
            var memoryItems = StandardCalculatorPage.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("4", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_ClearTest()
        {
            standardOperators.Divide(new List<double>() { 12, 3 });
            StandardCalculatorPage.MemoryPanel.MemButton.Click();
            standardOperators.NumberPad.TranslateNumberToButtonClicks(3.3);
            StandardCalculatorPage.MemoryPanel.MemoryClear.Click();
            try
            {
                StandardCalculatorPage.MemoryPanel.GetAllMemoryListViewItems();
            }
            catch (NullReferenceException ex)
            {
                if (!ex.StackTrace.Contains("MemoryPanel.GetAllMemoryListViewItems()"))
                {
                    throw;
                }
            }
        }
        #endregion
    }
}
