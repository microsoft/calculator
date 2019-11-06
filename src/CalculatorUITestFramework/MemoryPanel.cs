// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System.Collections.ObjectModel;
using OpenQA.Selenium.Interactions;
using OpenQA.Selenium;
using System.Windows.Input;
using System.Drawing;
using System;

namespace CalculatorUITestFramework
{
    public class MemoryPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement MemoryClear => this.session.TryFindElementByAccessibilityId("ClearMemoryButton");
        public WindowsElement MemRecall => this.session.TryFindElementByAccessibilityId("MemRecall");
        public WindowsElement MemPlus => this.session.TryFindElementByAccessibilityId("MemPlus");
        public WindowsElement MemMinus => this.session.TryFindElementByAccessibilityId("MemMinus");
        public WindowsElement MemButton => this.session.TryFindElementByAccessibilityId("memButton");
        public WindowsElement MemoryButton => this.session.TryFindElementByAccessibilityId("MemoryButton");
        public WindowsElement MemoryPane => this.session.TryFindElementByAccessibilityId("MemoryPanel");
        public WindowsElement MemoryLabel => this.session.TryFindElementByAccessibilityId("MemoryLabel");
        public WindowsElement MemoryListView => this.session.TryFindElementByAccessibilityId("MemoryListView");
        public WindowsElement MemoryPaneEmptyLabel => this.session.TryFindElementByAccessibilityId("MemoryPaneEmpty");
        public WindowsElement ClearMemoryItemButton => this.session.TryFindElementByAccessibilityId("MClearButton");
        public WindowsElement MemMinusItem => this.session.TryFindElementByAccessibilityId("MSubButton");
        public WindowsElement MemPlusItem => this.session.TryFindElementByAccessibilityId("MAddButton");
        public WindowsElement ClearMemory => this.session.TryFindElementByAccessibilityId("ClearMemory");
        public WindowsElement ListViewItem => this.session.FindElementByClassName("ListViewItem");



        /// <summary>
        /// Opens the Memory Pane by clicking the Memory pivot label.
        /// </summary>
        public void OpenMemoryPanel()
        {
            this.ResizeWindowToDiplayMemoryLabel();
            this.MemoryLabel.Click();
            this.MemoryPane.WaitForDisplayed();
        }

        /// <summary>
        /// Gets all of the memory items listed in the Memory Pane.
        /// </summary>
        /// <returns>A readonly collection of memory items.</returns>
        public ReadOnlyCollection<AppiumWebElement> GetAllMemoryListViewItems()
        {
            OpenMemoryPanel();
            return this.MemoryListView.FindElementsByClassName("ListViewItem");
        }

        /// <summary>
        /// Opens the Memory Panel and clicks the delete button if it is visible
        /// </summary>
        public void ClearMemoryPanel()
        {
            this.MemoryLabel.Click();

            try
            {
                this.ClearMemory.Click();
            }
            catch (WebDriverException ex)
            {
                if (ex.Message.Contains("element could not be located"))
                {
                    Assert.IsNotNull(this.MemoryPaneEmptyLabel);
                    return;
                }

                throw;
            }
        }

        /// <summary>
        /// If the Memory label is not displayed, resize the window
        /// Two attempts are made, the the lable is not found a "not found" exception is thrown
        /// </summary>
        public void ResizeWindowToDiplayMemoryLabel()
        {
            string source3 = this.session.PageSource;
            if (source3.Contains("MemoryLabel"))
            {
                return;
            }
            else
            {
                Size newWindowSize = new Size(1200, 1050);
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize;
                string source4 = this.session.PageSource;
                if (source4.Contains("MemoryLabel"))
                {
                    return;
                }
                else
                {
                    Size newWindowSize2 = new Size(2097, 1282);
                    WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize2;
                }
                string source7 = this.session.PageSource;
                if (source7.Contains("MemoryLabel"))
                {
                    return;
                }
                else
                {
                    throw new NotFoundException("Could not the Memory Label");
                }
            }
        }

        /// <summary>
        /// If the Memory button is not displayed, resize the window
        /// </summary>
        public void ResizeWindowToDiplayMemoryButton()
        {
            string source5 = this.session.PageSource;
            if (source5.Contains("Alt, M"))
            {
                return;
            }
            else
            {
                //this.MemButton.Click();
                string source6 = this.session.PageSource;
                if (source6.Contains("Alt, M"))
                {
                    return;
                }
                else
                {
                    Size newWindowSize2 = new Size(464, 464);
                    WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = newWindowSize2;
                    string source8 = this.session.PageSource;
                    if (source8.Contains("Alt, M"))
                    {
                        return;
                    }
                    else
                    {
                        throw new NotFoundException("Could not the History Button");
                    }
                }
            }
        }
    }
}
