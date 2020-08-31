// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorUITestFramework;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Interactions;
using System;

namespace CalculatorUITests
{
    [TestClass]
    public class HistoryFunctionalTests
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

            // Ensure that calculator window is large enough to display the memory/history panel; a good size for most tests
            page.MemoryPanel.ResizeWindowToDisplayMemoryLabel();
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
            CalculatorApp.EnsureCalculatorHasFocus();
            page.EnsureCalculatorIsInStandardMode();
            page.EnsureCalculatorResultTextIsZero();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.EnsureCalculatorIsInStandardMode();
            page.ClearAll();
        }

        #region Standard History Tests

        /// <summary>
        /// These automated tests (that replace this manual test case) verify using
        /// history related button/keyboard actions in the History Panel,
        /// and in the History flyout. The History button is verified in the automated test MouseInput_HistoryButtons().
        /// To-Do: Currently, an empty history panel on initial launch cannot be verified through automation.
        /// The automation cannot see the Windows element "HistoryEmpty."  This should be automated once the issue is resolved.
        /// </summary>

        [TestMethod]
        [Priority(2)]
        public void StandardHistory_Panel()
        {

            //Verify selecting history items form the history panel, and clearing the history panel
            page.HistoryPanel.OpenHistoryPanel();

            page.StandardOperators.NumberPad.Input(-3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(-2.6);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].GetValue().Equals("-1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[0].GetExpression().Equals("2   -   3 =", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].GetValue().Equals("-5.6", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].GetExpression().Equals("-3   +   -2.6 =", StringComparison.InvariantCultureIgnoreCase));

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            Actions clickHistoryItemsw1 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickHistoryItemsw1.Click(historyItems[1].Item);
            clickHistoryItemsw1.Perform();

            Assert.AreEqual("-5.6", page.CalculatorResults.GetCalculatorResultText());
            Assert.AreEqual("-3 + -2.6=", page.CalculatorResults.GetCalculatorExpressionText());

            Actions clickHistoryItemsw0 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickHistoryItemsw0.Click(historyItems[0].Item);
            clickHistoryItemsw0.Perform();

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            page.HistoryPanel.ClearHistory();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
        }

        [TestMethod]
        [Priority(2)]
        public void StandardHistory_Flyout()
        {

            //Verify selecting history items form the history flyout, using up/down arrows and [enter] to select history items,
            // and clearing the history flyout.  Additionally, verify history panel and history flyout have same information.

            page.HistoryPanel.OpenHistoryPanel();

            page.StandardOperators.NumberPad.Input(-3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(-2.6);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            var historyPanelItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyPanelItems[0].GetValue().Equals("-1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyPanelItems[0].GetExpression().Equals("2   -   3 =", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyPanelItems[1].GetValue().Equals("-5.6", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyPanelItems[1].GetExpression().Equals("-3   +   -2.6 =", StringComparison.InvariantCultureIgnoreCase));

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            var historyItems = page.HistoryPanel.GetAllHistoryFlyoutListViewItems();
            Assert.IsTrue(historyItems[0].GetValue().Equals("-1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[0].GetExpression().Equals("2   -   3 =", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].GetValue().Equals("-5.6", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].GetExpression().Equals("-3   +   -2.6 =", StringComparison.InvariantCultureIgnoreCase));

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            page.HistoryPanel.ListViewItem.SendKeys(Keys.ArrowDown + Keys.Enter);

            Assert.AreEqual("-5.6", page.CalculatorResults.GetCalculatorResultText());
            Assert.AreEqual("-3 + -2.6=", page.CalculatorResults.GetCalculatorExpressionText());

            page.HistoryPanel.OpenHistoryFlyout();
            page.HistoryPanel.ListViewItem.SendKeys(Keys.ArrowDown + Keys.ArrowUp + Keys.Enter);
            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            page.HistoryPanel.OpenHistoryFlyout();
            page.HistoryPanel.ClearHistoryButton.Click();

            page.HistoryPanel.OpenHistoryFlyout();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));

            page.HistoryPanel.OpenHistoryPanel();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
        }

        #endregion
    }
}
