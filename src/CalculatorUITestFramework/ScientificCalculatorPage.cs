// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Scientific Mode.
    /// </summary>
    public class ScientificCalculatorPage
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public ScientificOperatorsPanel ScientificOperators = new ScientificOperatorsPanel();
        public MemoryPanel MemoryPanel = new MemoryPanel();
        public HistoryPanel HistoryPanel = new HistoryPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");

        public WindowsElement CalculatorResult => this.session.TryFindElementByAccessibilityId("CalculatorResults");

        public void NavigateToScientificCalculator()
        {
            // Ensure that calculator is in scientific mode
            this.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ScientificCalculator);
            Assert.IsNotNull(CalculatorResult);
        }

        /// <summary>
        /// Clear the Calculator display, Memory Panel and optionally the History Panel
        /// </summary>
        public void ClearAll()
        {
            string source = this.session.PageSource;

            if (source.Contains("clearEntryButton"))
            {
                this.ScientificOperators.ClearEntryButton.Click();
                source = this.session.PageSource;
            }
            if (source.Contains("clearButton"))
            {
                this.ScientificOperators.ClearButton.Click();
            }
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
