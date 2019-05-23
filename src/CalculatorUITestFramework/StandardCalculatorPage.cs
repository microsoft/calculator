// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Standard Mode.
    /// </summary>
    public class StandardCalculatorPage
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public MemoryPanel MemoryPanel = new MemoryPanel();
        public HistoryPanel HistoryPanel = new HistoryPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement Header
        {
            get
            {
                try
                {
                    return this.session.TryFindElementByAccessibilityId("Header");
                }
                catch
                {
                    return this.session.TryFindElementByAccessibilityId("ContentPresenter");
                }
            }
        }
        public WindowsElement CalculatorResult => this.session.TryFindElementByAccessibilityId("CalculatorResults");

        public void StandardCalculatorSetup(TestContext context)
        {
            // Create session to launch a Calculator window
            WinAppDriver.Instance.SetupCalculatorSession(context);

            // Ensure that calculator is in standard mode
            this.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            Assert.IsNotNull(CalculatorResult);
        }

        public void StandardCalculatorTearDown()
        {
            // Tear down Calculator session.
            WinAppDriver.Instance.TearDownCalculatorSession();
        }

        /// <summary>
        /// Clear the Calculatory display, Memory Panel and optionally the History Panel
        /// </summary>
        /// <param name="clearHistory">Bool specifying if the History Panel should be cleared; true by default.</param>
        public void ClearAll()
        {
            this.StandardOperators.ClearButton.Click();
            this.MemoryPanel.MemoryClear.Click();
            this.HistoryPanel.ClearHistory();
        }

        /// <summary>
        /// Gets the text from the display control and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculatorResultText()
        {
            return this.CalculatorResult.Text.Replace("Display is", string.Empty).Trim();
        }
    }
}
