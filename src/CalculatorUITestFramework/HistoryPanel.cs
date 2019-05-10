using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting.Logging;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CalculatorUITestFramework
{
    public class HistoryPanel : ApplicationBase
    {
        public WindowsElement HistoryLabel => TryFindElementByAccessibilityId("HistoryLabel");
        public WindowsElement HistoryListView => TryFindElementByAccessibilityId("HistoryListView");
        public WindowsElement ClearHistoryButton => TryFindElementByAccessibilityId("ClearHistory");

        /// <summary>
        /// Opens the History Pane by clicking the History pivot label.
        /// </summary>
        public void OpenHistoryPanel()
        {
            HistoryLabel.Click();
        }

        /// <summary>
        /// Gets all of the history items listed in the History Pane.
        /// </summary>
        /// <returns>A readonly collection of history items.</returns>
        public ReadOnlyCollection<AppiumWebElement> GetAllHistoryListViewItems()
        {
            OpenHistoryPanel();
            HistoryListView.WaitForDisplayed();
            return HistoryListView.FindElementsByClassName("ListViewItem");
        }

        /// <summary>
        /// Opens the History Pane and clicks the delete button if it is visible.
        /// </summary>
        public void ClearHistory()
        {
            OpenHistoryPanel();
            if (ClearHistoryButton != null)
            {
                ClearHistoryButton.Click();
            }
        }
    }
}
