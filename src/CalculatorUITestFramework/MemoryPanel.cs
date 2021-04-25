// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
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
    public class MemoryPanel
    {
        public WindowsElement NumberpadMCButton => this.session.TryFindElementByAccessibilityId("ClearMemoryButton");
        public WindowsElement NumberpadMRButton => this.session.TryFindElementByAccessibilityId("MemRecall");
        public WindowsElement NumberpadMPlusButton => this.session.TryFindElementByAccessibilityId("MemPlus");
        public WindowsElement NumberpadMMinusButton => this.session.TryFindElementByAccessibilityId("MemMinus");
        public WindowsElement NumberpadMSButton => this.session.TryFindElementByAccessibilityId("memButton");
        public WindowsElement MemoryFlyoutButton => this.session.TryFindElementByAccessibilityId("MemoryButton");
        public WindowsElement PanelClearMemoryButton => this.session.TryFindElementByAccessibilityId("ClearMemory");
        public WindowsElement ListViewItem => this.session.FindElementByClassName("ListViewItem");

        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        private WindowsElement MemoryPane => this.session.TryFindElementByAccessibilityId("MemoryPanel");
        private WindowsElement MemoryLabel => this.session.TryFindElementByAccessibilityId("MemoryLabel");
        private WindowsElement MemoryListView => this.session.TryFindElementByAccessibilityId("MemoryListView");
        private WindowsElement MemoryPaneEmptyLabel => this.session.TryFindElementByAccessibilityId("MemoryPaneEmpty");
        private WindowsElement MemoryFlyout => this.session.TryFindElementByAccessibilityId("MemoryFlyout");

        /// <summary>
        /// Opens the Memory Pane by clicking the Memory pivot label.
        /// </summary>
        public void OpenMemoryPanel()
        {
            this.ResizeWindowToDisplayMemoryLabel();
            this.MemoryLabel.Click();
            this.MemoryPane.WaitForDisplayed();
        }

        /// <summary>
        /// Gets all of the memory items listed in the Memory Pane.
        /// </summary>
        /// <returns>A read-only collection of memory items.</returns>
        public List<MemoryItem> GetAllMemoryListViewItems()
        {
            OpenMemoryPanel();
            return (from item in this.MemoryListView.FindElementsByClassName("ListViewItem") select new MemoryItem(item)).ToList();
        }

        /// <summary>
        /// Opens the Memory Panel and clicks the delete button if it is visible
        /// </summary>
        public void ClearMemoryPanel()
        {
            this.MemoryLabel.Click();

            try
            {
                if (this.session.PageSource.Contains("ClearMemoryButton"))
                {
                    this.PanelClearMemoryButton.Click();
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
                    Assert.IsNotNull(this.MemoryPaneEmptyLabel);
                    return;
                }
                throw;
            }
        }

        /// <summary>
        /// If the Memory label is not displayed, resize the window
        /// Two attempts are made, and if the label is not found, a "not found" exception is thrown
        /// </summary>
        public void ResizeWindowToDisplayMemoryLabel()
        {
            // Put the calculator in the upper left region of the screen
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            GrowWindowToShowMemoryLabel(WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Width);
        }

        /// <summary>
        /// If the Memory button is not displayed, resize the window
        /// </summary>
        public void ResizeWindowToDisplayMemoryButton()
        {
            // Put the calculator in the upper left region of the screen
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            ShrinkWindowToShowMemoryButton(WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Width);
        }

        /// <summary>
        /// Opens the Memory Flyout.
        /// </summary>
        public void OpenMemoryFlyout()
        {
            this.ResizeWindowToDisplayMemoryButton();
            CalculatorApp.EnsureCalculatorHasFocus();
            Actions moveToMemoryButton = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToMemoryButton.MoveToElement(MemoryFlyoutButton);
            moveToMemoryButton.Perform();
            CalculatorApp.Window.SendKeys(Keys.Alt + "m" + Keys.Alt);
            Actions moveToMemoryFlyout = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToMemoryFlyout.MoveToElement(MemoryFlyout);
            moveToMemoryFlyout.Perform();
        }

        /// <summary>
        /// Gets all of the memory items listed in the Memory Flyout.
        /// </summary>
        /// <returns> A read only collection of memory items.</returns>
        public List<MemoryItem> GetAllMemoryFlyoutListViewItems()
        {
            OpenMemoryFlyout();
            return (from item in this.MemoryListView.FindElementsByClassName("ListViewItem") select new MemoryItem(item)).ToList();
        }

        /// <summary>
        /// Increases the size of the window until Memory label for the Memory panel is visible
        /// </summary>
        private void GrowWindowToShowMemoryLabel(int width)
        {
            if (width > 2100)
            {
                throw new NotFoundException("Could not the Memory Label");
            }

            if (!this.session.PageSource.Contains("MemoryLabel"))
            {
                var height = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Height;
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(width, height);
                //give window time to render new size
                System.Threading.Thread.Sleep(10);
                GrowWindowToShowMemoryLabel(width + 100);
            }
        }

        /// <summary>
        /// Decreases the size of the window until Memory button is visible
        /// </summary>
        private void ShrinkWindowToShowMemoryButton(int width)
        {
            if (width < 200)
            {
                throw new NotFoundException("Could not find the Memory Button");
            }

            //Page source contains differnt memory button types, using hotkey info is for this specific memory button
            if (!this.session.PageSource.Contains("Alt, M"))
            {
                var height = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Height;
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(width, height);
                //give window time to render new size
                System.Threading.Thread.Sleep(10);
                ShrinkWindowToShowMemoryButton(width - 100);
            }
        }
    }
}
