// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Programmer Mode.
    /// </summary>
    public class ProgrammerCalculatorPage
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public ScientificOperatorsPanel ScientificOperators = new ScientificOperatorsPanel();
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public ProgrammerOperatorsPanel ProgrammerOperators = new ProgrammerOperatorsPanel();
        public MemoryPanel MemoryPanel = new MemoryPanel();
        public HistoryPanel HistoryPanel = new HistoryPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");

        public CalculatorResults CalculatorResults = new CalculatorResults();

        public void NavigateToProgrammerCalculator()
        {
            // Ensure that calculator is in scientific mode
            NavigationMenu.ChangeCalculatorMode(CalculatorMode.ProgrammerCalculator);
        }

        /// <summary>
        /// Clear the Calculator display, Memory Panel and optionally the History Panel
        /// </summary>
        public void ClearAll()
        {
            string source = this.session.PageSource;

            if (source.Contains("clearEntryButton"))
            {
                StandardOperators.ClearEntryButton.Click();
                StandardOperators.ClearButton.Click();
            }
            if (source.Contains("clearButton"))
            {
                StandardOperators.ClearButton.Click();
            }
            MemoryPanel.ResizeWindowToDisplayMemoryLabel();
            if (source.Contains("ClearMemory"))
            {
                MemoryPanel.NumberpadMCButton.Click();
            }
        }
    }
}
