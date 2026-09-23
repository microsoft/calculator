// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Standard Mode.
    /// </summary>
    public class StandardCalculatorPage
    {
        public StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public MemoryPanel MemoryPanel = new MemoryPanel();
        public HistoryPanel HistoryPanel = new HistoryPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public StandardAoTCalculatorPage StandardAoTCalculatorPage = new StandardAoTCalculatorPage();
        public CalculatorResults CalculatorResults = new CalculatorResults();

        private WindowsDriver<WindowsElement> session => CalculatorDriver.Instance.CalculatorSession;

        /// <summary>
        /// Navigates the calculator to Standard mode and ensures that it is in standard mode
        /// </summary>
        public void NavigateToStandardCalculator()
        {
            // Ensure that calculator is in standard mode
            NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorResults.IsResultsDisplayPresent();
        }

        /// <summary>
        /// Clear the Calculator display, Memory Panel and optionally the History Panel
        /// </summary>
        public void ClearAll()
        {
            StandardAoTCalculatorPage.NavigateToStandardMode();
            MemoryPanel.ResizeWindowToDisplayMemoryLabel();
            StandardOperators.ClearButton.Click();
            MemoryPanel.NumberpadMCButton.Click();
            HistoryPanel.ClearHistory();
        }

        ///// <summary>
        ///// Ensures that the calculator result text is zero; if not, clears all
        ///// </summary>
        public void EnsureCalculatorResultTextIsZero()
        {
            if ("0" != CalculatorResults.GetCalculatorResultText())
            {
                ClearAll();
            }
        }

        ///// <summary>
        ///// Ensures that the calculator is in Standard Mode
        ///// </summary>
        public void EnsureCalculatorIsInStandardMode()
        {
            string source = CalculatorDriver.Instance.CalculatorSession.PageSource;
            if (source.Contains("Header"))
            {
                string header = CalculatorApp.Header.Text;
                if (header == "Standard")
                {
                    return;
                }
                else
                {
                    NavigateToStandardCalculator();
                }
            }
            else
            {
                StandardAoTCalculatorPage.NavigateToStandardMode();
            }
        }

    }

}
