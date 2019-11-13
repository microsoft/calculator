// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System.Collections.ObjectModel;
using System.Drawing;
using OpenQA.Selenium.Interactions;
using System;

namespace CalculatorUITestFramework
{
    public class HistoryPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement HistoryButton => this.session.TryFindElementByAccessibilityId("HistoryButton");
        public WindowsElement HistoryLabel => this.session.TryFindElementByAccessibilityId("HistoryLabel");
        public AppiumWebElement HistoryEmpty => this.session.TryFindElementByAccessibilityId("DockPivot").FindElementByAccessibilityId("HistoryEmpty");
        public WindowsElement HistoryListView => this.session.TryFindElementByAccessibilityId("HistoryListView");
        public WindowsElement HistoryList => this.session.TryFindElementByAccessibilityId("HistoryList");
        public WindowsElement ListViewItem => this.session.FindElementByClassName("ListViewItem");
        public WindowsElement ExprTextBlock => this.session.TryFindElementByAccessibilityId("ExprTextBlock");
        public WindowsElement ResultTextBlock => this.session.TryFindElementByAccessibilityId("ResultTextBlock");
        public WindowsElement ClearHistoryButton => this.session.TryFindElementByAccessibilityId("ClearHistory");

        /// <summary>
        /// Opens the Memory Pane by clicking the Memory pivot label.
        /// </summary>
        public void OpenHistoryPanel()
        {
            this.ResizeWindowToDiplayHistoryLabel();
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
            try
            {
                string sourceB = this.session.PageSource;
                if (sourceB.Contains("ClearHistory"))
                {
                    this.ClearHistoryButton.Click();
                }
                else
                {
                    return;
                }
            }
            catch (WebDriverException ex)
            {
                if (ex.Message.Contains("element could not be located"))
                {
                    Assert.IsNotNull(this.HistoryEmpty);
                    return;
                }

                throw;
            }
        }

        /// <summary>
        /// If the History label is not displayed, resize the window
        /// Two attempts are made, the the lable is not found a "not found" exception is thrown
        /// </summary>
        public void ResizeWindowToDiplayHistoryLabel()
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
                string source2 = this.session.PageSource;
                if (source2.Contains("HistoryLabel"))
                {
                    return;
                }
                else
                {
                    Size newWindowSize2 = new Size(2097, 1282);
                    WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize2;
                }
                string source9 = this.session.PageSource;
                if (source9.Contains("HistoryLabel"))
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
        public void ResizeWindowToNotDiplayHistoryLabel()
        {
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
            else
            {
                return;
            }
        }
    }
}
