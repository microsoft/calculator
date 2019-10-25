// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System.Collections.ObjectModel;
using OpenQA.Selenium.Interactions;
using OpenQA.Selenium;
using System.Windows.Input;

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
    }
}
