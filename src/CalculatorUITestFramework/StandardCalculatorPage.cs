using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Remote;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in Standard Mode.
    /// </summary>
    public class StandardCalculatorPage : ApplicationBase
    {
        public static StandardOperatorsPanel StandardOperators = new StandardOperatorsPanel();
        public static MemoryPanel MemoryPanel = new MemoryPanel();
        public static HistoryPanel HistoryPanel = new HistoryPanel();

        public static void StandardCalculatorSetup(TestContext context)
        {
            // Create session to launch a Calculator window
            ApplicationSetup(context);
            // Identify calculator mode by locating the header


            // Ensure that calculator is in standard mode
            NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            var source = CalculatorSession.PageSource;
            Assert.IsNotNull(CalculatorResult);
        }

        /// <summary>
        /// Clear the Calculatory display, Memory Panel and optionally the History Panel
        /// </summary>
        /// <param name="clearHistory">Bool specifying if the History Panel should be cleared; true by default.</param>
        public static void ClearAll(bool clearHistory = true)
        {
            StandardOperators.ClearButton.Click();
            MemoryPanel.MemoryClear.Click();
            if (clearHistory)
            {
                HistoryPanel.ClearHistory();
            }
        }

        /// <summary>
        /// Gets the text from the display control and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public static string GetCalculatorResultText()
        {
            return CalculatorResult.Text.Replace("Display is", string.Empty).Trim();
        }
    }
}
