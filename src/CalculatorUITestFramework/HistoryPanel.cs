// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Interactions;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing;
using System.Linq;

namespace CalculatorUITestFramework
{
    public class HistoryPanel
    {
        public WindowsElement HistoryButton => this.session.TryFindElementByAccessibilityId("HistoryButton");
        public WindowsElement ListViewItem => this.session.FindElementByClassName("ListViewItem");
        public WindowsElement ClearHistoryButton => this.session.TryFindElementByAccessibilityId("ClearHistory");

        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        private WindowsElement HistoryLabel => this.session.TryFindElementByAccessibilityId("HistoryLabel");
        private WindowsElement HistoryListView => this.session.TryFindElementByAccessibilityId("HistoryListView");
        private WindowsElement HistoryFlyout => this.session.TryFindElementByAccessibilityId("HistoryFlyout");

        /// <summary>
        /// Opens the History Pane by clicking the History pivot label.
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
        public List<HistoryItem> GetAllHistoryListViewItems()
        {
            OpenHistoryPanel();
            return (from item in this.HistoryListView.FindElementsByClassName("ListViewItem") select new HistoryItem(item)).ToList();
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
            // Put the calculator in the upper left region of the screen
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            GrowWindowToShowHistoryLabel(WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Width);
        }

        ///// <summary>
        ///// If the History button is not displayed, resize the window
        ///// </summary>
        public void ResizeWindowToDisplayHistoryButton()
        {
            // Put the calculator in the upper left region of the screen
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            ShrinkWindowToShowHistoryButton(WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Width);
        }

        /// <summary>
        /// Opens the History Flyout.
        /// </summary>
        public void OpenHistoryFlyout()
        {
            this.ResizeWindowToDisplayHistoryButton();
            CalculatorApp.EnsureCalculatorHasFocus();
            CalculatorApp.Window.SendKeys(Keys.Control + "h" + Keys.Control);
            Actions moveToHistoryFlyout = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToHistoryFlyout.MoveToElement(HistoryFlyout);
            moveToHistoryFlyout.Perform();
        }

        /// <summary>
        /// Gets all of the History items listed in the History Flyout.
        /// </summary>
        /// <returns> A read only collection of History items.</returns>
        public List<HistoryItem> GetAllHistoryFlyoutListViewItems()
        {
            OpenHistoryFlyout();
            return (from item in this.HistoryListView.FindElementsByClassName("ListViewItem") select new HistoryItem(item)).ToList();
        }

        /// <summary>
        /// Increases the size of the window until History label for the History panel is visible
        /// </summary>
        private void GrowWindowToShowHistoryLabel(int width)
        {
            if (width > 2100)
            {
                throw new NotFoundException("Could not the History Label");
            }

            if (!this.session.PageSource.Contains("HistoryLabel"))
            {
                var height = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Height;
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(width, height);
                //give window time to render new size
                System.Threading.Thread.Sleep(10);
                GrowWindowToShowHistoryLabel(width + 100);
            }
        }

        /// <summary>
        /// Decreases the size of the window until History button is visible
        /// </summary>
        private void ShrinkWindowToShowHistoryButton(int width)
        {
            if (width < 200)
            {
                throw new NotFoundException("Could not find the History Button");
            }

            if (!this.session.PageSource.Contains("HistoryButton"))
            {
                var height = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Height;
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(width, height);
                //give window time to render new size
                System.Threading.Thread.Sleep(10);
                ShrinkWindowToShowHistoryButton(width - 100);
            }
        }
    }
}
