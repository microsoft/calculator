// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public MemoryPanel MemoryPanel = new MemoryPanel();
        public HistoryPanel HistoryPanel = new HistoryPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");

        public CalculatorResults CalculatorResults = new CalculatorResults();

        public void NavigateToScientificCalculator()
        {
            // Ensure that calculator is in scientific mode
            this.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ScientificCalculator);
        }

        /// <summary>
        /// Clear the Calculator display, Memory Panel and optionally the History Panel
        /// </summary>
        public void ClearAll()
        {
            string source = this.session.PageSource;

            if (source.Contains("clearEntryButton"))
            {
                this.StandardOperators.ClearEntryButton.Click();
                source = this.session.PageSource;
            }
            if (source.Contains("clearButton"))
            {
                this.StandardOperators.ClearButton.Click();
            }
            MemoryPanel.ResizeWindowToDisplayMemoryLabel();
            HistoryPanel.ClearHistory();
        }
    }
}
