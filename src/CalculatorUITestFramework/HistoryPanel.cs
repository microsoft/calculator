// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System.Collections.ObjectModel;

namespace CalculatorUITestFramework
{
    public class HistoryPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement HistoryLabel => this.session.TryFindElementByAccessibilityId("HistoryLabel");
        public WindowsElement HistoryListView => this.session.TryFindElementByAccessibilityId("HistoryListView");
        public WindowsElement ClearHistoryButton => this.session.TryFindElementByAccessibilityId("ClearHistory");
        public WindowsElement HistoryEmptyLabel => this.session.TryFindElementByAccessibilityId("HistoryEmpty");


        /// <summary>
        /// Gets all of the history items listed in the History Pane.
        /// </summary>
        /// <returns>A readonly collection of history items.</returns>
        public ReadOnlyCollection<AppiumWebElement> GetAllHistoryListViewItems()
        {
            this.HistoryLabel.Click();
            this.HistoryListView.WaitForDisplayed();
            return this.HistoryListView.FindElementsByClassName("ListViewItem");
        }

        /// <summary>
        /// Opens the History Pane and clicks the delete button if it is visible.
        /// </summary>
        public void ClearHistory()
        {
            this.HistoryLabel.Click();

            try
            {
                this.ClearHistoryButton.Click();
            }
            catch(WebDriverException ex)
            {
                if (ex.Message.Contains("element could not be located"))
                {
                    Assert.IsNotNull(this.HistoryEmptyLabel);
                    return;
                }

                throw;
            }
        }
    }
}
