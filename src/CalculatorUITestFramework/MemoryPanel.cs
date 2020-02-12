// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Interactions;
using System.Collections.ObjectModel;
using System.Drawing;

namespace CalculatorUITestFramework
{
    public class MemoryPanel
    {
        public WindowsElement MemoryClear => this.session.TryFindElementByAccessibilityId("ClearMemoryButton");
        public WindowsElement MemRecall => this.session.TryFindElementByAccessibilityId("MemRecall");
        public WindowsElement MemPlus => this.session.TryFindElementByAccessibilityId("MemPlus");
        public WindowsElement MemMinus => this.session.TryFindElementByAccessibilityId("MemMinus");
        public WindowsElement MemButton => this.session.TryFindElementByAccessibilityId("memButton");
        public WindowsElement MemoryButton => this.session.TryFindElementByAccessibilityId("MemoryButton");
        public WindowsElement ClearMemoryItemButton => this.session.TryFindElementByAccessibilityId("MClearButton");
        public WindowsElement MemMinusItem => this.session.TryFindElementByAccessibilityId("MSubButton");
        public WindowsElement MemPlusItem => this.session.TryFindElementByAccessibilityId("MAddButton");
        public WindowsElement ClearMemory => this.session.TryFindElementByAccessibilityId("ClearMemory");
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
                if (this.session.PageSource.Contains("ClearMemoryButton"))
                {
                    this.ClearMemory.Click();
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
        /// Two attempts are made, the the lable is not found a "not found" exception is thrown
        /// </summary>
        public void ResizeWindowToDiplayMemoryLabel()
        {
            // Put the calculator in the upper left region of the screen
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            GrowWindowToMemoryLabel(WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Width);
        }

        /// <summary>
        /// If the Memory button is not displayed, resize the window
        /// </summary>
        public void ResizeWindowToDiplayMemoryButton()
        {
            // Put the calculator in the upper left region of the screen
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            ShrinkWindowToMemoryButton(WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Width);
        }

        /// <summary>
        /// Opens the Memory Flyout.
        /// </summary>
        public void OpenMemoryFlyout()
        {
            this.ResizeWindowToDiplayMemoryButton();
            CalculatorApp.EnsureCalculatorHasFocus();
            Actions moveToMemoryButton = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToMemoryButton.MoveToElement(MemoryButton);
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
        public ReadOnlyCollection<AppiumWebElement> GetAllMemoryFlyoutListViewItems()
        {
            OpenMemoryFlyout();
            return this.MemoryListView.FindElementsByClassName("ListViewItem");
        }

        /// <summary>
        /// Increases the size of the window until Memory label for the Memory panel is visible
        /// </summary>
        private void GrowWindowToMemoryLabel(int width)
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
                GrowWindowToMemoryLabel(width + 100);
            }
        }

        /// <summary>
        /// Decreases the size of the window until Memory button is visible
        /// </summary>
        private void ShrinkWindowToMemoryButton(int width)
        {
            if (width < 464)
            {
                throw new NotFoundException("Could not the Memory Button");
            }

            if (!this.session.PageSource.Contains("MemoryButton"))
            {
                var height = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size.Height;
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(width, height);
                //give window time to render new size
                System.Threading.Thread.Sleep(10);
                ShrinkWindowToMemoryButton(width - 100);
            }
        }
    }
}
