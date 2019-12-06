// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Interactions;
using System.Collections.ObjectModel;
using System.Drawing;

namespace CalculatorUITestFramework
{
    public class HistoryPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public GlobalCalculatorUI GlobalCalculatorUI = new GlobalCalculatorUI();
        public WindowsElement HistoryButton => this.session.TryFindElementByAccessibilityId("HistoryButton");
        private WindowsElement HistoryLabel => this.session.TryFindElementByAccessibilityId("HistoryLabel");
        private AppiumWebElement HistoryEmpty => this.session.TryFindElementByAccessibilityId("DockPivot").FindElementByAccessibilityId("HistoryEmpty");
        private WindowsElement HistoryListView => this.session.TryFindElementByAccessibilityId("HistoryListView");
        public WindowsElement ListViewItem => this.session.FindElementByClassName("ListViewItem");
        private WindowsElement ResultTextBlock => this.session.TryFindElementByAccessibilityId("ResultTextBlock");
        public WindowsElement ClearHistoryButton => this.session.TryFindElementByAccessibilityId("ClearHistory");
        private WindowsElement HistoryFlyout => this.session.TryFindElementByAccessibilityId("HistoryFlyout");

        /// <summary>
        /// Opens the Memory Pane by clicking the Memory pivot label.
        /// </summary>
        public void OpenHistoryPanel()
        {
            this.ResizeWindowToDisplayHistoryLabel();
            this.HistoryLabel.Click();
        }
        /// <summary>
        /// Gets all of the history items listed in the History Pane.
        /// </summary>
        /// <returns>A readonly collection of history items.</returns>
        public ReadOnlyCollection<AppiumWebElement> GetAllHistoryListViewItems()
        {
            OpenHistoryPanel();
            return this.HistoryListView.FindElementsByClassName("ListViewItem");
        }
        /// <summary>
        /// Opens the History Pane and clicks the delete button if it is visible.
        /// </summary>
        public void ClearHistory()
        {

            this.HistoryLabel.Click();
            string source = this.session.PageSource;
            if (source.Contains("ClearHistory"))
            {
                this.ClearHistoryButton.Click();
            }
        }
        /// <summary>
        /// If the History label is not displayed, resize the window
        /// Two attempts are made; the label is not found a "not found" exception is thrown
        /// </summary>
        public void ResizeWindowToDisplayHistoryLabel()
        {
            Point newWindowPostion = new Point(0, 0);
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = newWindowPostion;
            string source0 = this.session.PageSource;
            if (source0.Contains("HistoryLabel"))
            {
                return;
            }
            else
            {
                Size newWindowSize = new Size(1200, 1050);
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize;
                string source1 = this.session.PageSource;
                if (source1.Contains("HistoryLabel"))
                {
                    return;
                }
                else
                {
                    Size newWindowSize2 = new Size(2097, 1282);
                    WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize2;
                }
                string source2 = this.session.PageSource;
                if (source2.Contains("HistoryLabel"))
                {
                    return;
                }
                else
                {
                    throw new NotFoundException("Could not the History Label");
                }
            }
        }
        ///// <summary>
        ///// If the History button is not displayed, resize the window
        ///// </summary>
        public void ResizeWindowToNotDisplayHistoryLabel()
        {
            Point newWindowPostion = new Point(0, 0);
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = newWindowPostion;
            string source1 = this.session.PageSource;
            if (source1.Contains("HistoryLabel"))
            {
                Size newWindowSize = new Size(464, 464);
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize;
                string source2 = this.session.PageSource;
                if (source2.Contains("HistoryButton"))
                {
                    return;
                }
                else
                {
                    throw new NotFoundException("Could not the History Button");
                }
            }
        }
        /// <summary>
        /// Opens the History Flyout.
        /// </summary>
        public void OpenHistoryFlyout()
        {
            this.ResizeWindowToNotDisplayHistoryLabel();
            this.GlobalCalculatorUI.EnsureCalculatorHasFocus();
            this.GlobalCalculatorUI.Header.SendKeys(Keys.Control + "h" + Keys.Control);
            Actions moveToHistoryFlyout = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToHistoryFlyout.MoveToElement(HistoryFlyout);
            moveToHistoryFlyout.Perform();
        }
        /// <summary>
        /// Gets all of the History items listed in the History Flyout.
        /// </summary>
        /// <returns> A read only collection of History items.</returns>
        public ReadOnlyCollection<AppiumWebElement> GetAllHistoryFlyoutListViewItems()
        {
            OpenHistoryFlyout();
            return this.HistoryListView.FindElementsByClassName("ListViewItem");
        }
    }
}
