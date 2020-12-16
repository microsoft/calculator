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
    public class MemoryFunctionalTests
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
        #region Standard Memory Tests

        /// <summary>
        /// These automated tests verify using memory related buttons in the Memory Panel, and in the memory flyout
        /// Memory control buttons are verified at the end of automated tested in MouseInput_MemoryButtons()
        /// </summary>

        [TestMethod]
        [Priority(2)]
        public void StandardMemory_Panel()
        {
            //Verify context menu MC, M+, M-, and verify the clear memory button in the Memory panel
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.MemoryPanel.NumberpadMSButton.Click();
            page.MemoryPanel.NumberpadMSButton.Click();
            page.MemoryPanel.OpenMemoryPanel();

            Actions moveToListView = new Actions(WinAppDriver.Instance.CalculatorSession);
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            moveToListView.MoveToElement(memoryItems[0].Item);
            moveToListView.ContextClick(memoryItems[0].Item);
            moveToListView.Perform();
            CalculatorApp.Window.SendKeys(Keys.ArrowDown + Keys.ArrowDown + Keys.Enter);
            Assert.IsTrue(memoryItems[0].GetValue().Equals("0", StringComparison.InvariantCultureIgnoreCase));

            moveToListView.Perform();
            CalculatorApp.Window.SendKeys(Keys.ArrowUp + Keys.ArrowUp + Keys.Enter);
            Assert.IsTrue(memoryItems[0].GetValue().Equals("3", StringComparison.InvariantCultureIgnoreCase));

            moveToListView.Perform();
            CalculatorApp.Window.SendKeys(Keys.ArrowDown + Keys.ArrowUp + Keys.Enter);
            var memoryItems2 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems2[0].GetValue().Equals("3", StringComparison.InvariantCultureIgnoreCase));

            page.MemoryPanel.PanelClearMemoryButton.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        [TestMethod]
        [Priority(2)]
        public void StandardMemory_Flyout()
        {
            //Verify context menu MC, M+, M-, and ClearMemoryItemButton, and verify the clear memory button in the Memory flyout
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.MemoryPanel.NumberpadMSButton.Click();
            page.MemoryPanel.NumberpadMSButton.Click();

            page.MemoryPanel.OpenMemoryFlyout();
            var memoryItems = page.MemoryPanel.GetAllMemoryFlyoutListViewItems();
            Actions moveToListView = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToListView.MoveToElement(memoryItems[0].Item);
            moveToListView.ContextClick(memoryItems[0].Item);
            moveToListView.Perform();
            CalculatorApp.Window.SendKeys(Keys.ArrowDown + Keys.ArrowDown + Keys.Enter);
            Assert.IsTrue(memoryItems[0].GetValue().Equals("0", StringComparison.InvariantCultureIgnoreCase));

            page.MemoryPanel.OpenMemoryFlyout();
            moveToListView.Perform();
            CalculatorApp.Window.SendKeys(Keys.ArrowUp + Keys.ArrowUp + Keys.Enter);
            Assert.IsTrue(memoryItems[0].GetValue().Equals("3", StringComparison.InvariantCultureIgnoreCase));

            page.MemoryPanel.OpenMemoryFlyout();
            moveToListView.Perform();
            CalculatorApp.Window.SendKeys(Keys.ArrowDown + Keys.ArrowUp + Keys.Enter);
            var memoryItems2 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems2[0].GetValue().Equals("3", StringComparison.InvariantCultureIgnoreCase));

            page.MemoryPanel.OpenMemoryFlyout();
            page.MemoryPanel.PanelClearMemoryButton.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));

            page.MemoryPanel.ResizeWindowToDisplayMemoryLabel();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        #endregion
    }
}
