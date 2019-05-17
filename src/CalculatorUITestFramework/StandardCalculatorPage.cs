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
                    return session.TryFindElementByAccessibilityId("Header");
                }
                catch
                {
                    return session.TryFindElementByAccessibilityId("ContentPresenter");
                }
            }
        }
        public WindowsElement CalculatorResult => session.TryFindElementByAccessibilityId("CalculatorResults");

        public void StandardCalculatorSetup(TestContext context)
        {
            // Create session to launch a Calculator window
            WinAppDriver.Instance.SetupCalculatorSession(context);

            // Ensure that calculator is in standard mode
            NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
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
            StandardOperators.ClearButton.Click();
            MemoryPanel.MemoryClear.Click();
            HistoryPanel.ClearHistory();
        }

        /// <summary>
        /// Gets the text from the display control and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculatorResultText()
        {
            return CalculatorResult.Text.Replace("Display is", string.Empty).Trim();
        }
    }
}
