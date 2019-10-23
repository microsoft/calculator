// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System.Collections.ObjectModel;
using OpenQA.Selenium.Interactions;
using OpenQA.Selenium;
using System.Windows.Input;

namespace CalculatorUITestFramework
{
    public class CalcMemoryFlyout
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");
        public WindowsElement AppName => this.session.TryFindElementByAccessibilityId("AppName");
        public WindowsElement MemoryButton => this.session.TryFindElementByAccessibilityId("MemoryButton");
        public WindowsElement MemoryFlyout => this.session.TryFindElementByAccessibilityId("MemoryFlyout");
        public AppiumWebElement ListViewItem => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByClassName("ListViewItem");
        public AppiumWebElement MemoryListView => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByAccessibilityId("MemoryFlyout").FindElementByAccessibilityId("MemoryListView");
        public AppiumWebElement MemPlusItem => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByAccessibilityId("MemoryFlyout").FindElementByAccessibilityId("MAddButton");
        public AppiumWebElement MemMinusItem => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByAccessibilityId("MemoryFlyout").FindElementByAccessibilityId("MSubButton");
        public AppiumWebElement ClearMemoryItemButton => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByAccessibilityId("MemoryFlyout").FindElementByAccessibilityId("MClearButton");
        public AppiumWebElement ClearMemory => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByAccessibilityId("MemoryFlyout").FindElementByAccessibilityId("ClearMemory");
        public AppiumWebElement MemoryPaneEmptyLabel => WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryFlyout").FindElementByAccessibilityId("MemoryPaneEmpty");

        /// <summary>
        /// Opens the Memory Flyout.
        /// </summary>
        public void OpenMemoryFlyout()
        {
            this.AppName.Click();
            Actions moveToMemoryButton = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToMemoryButton.MoveToElement(MemoryButton);
            moveToMemoryButton.Perform();
            this.Header.SendKeys(Keys.Alt + "m" + Keys.Alt);
            Actions moveToListView2 = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToListView2.MoveToElement(MemoryFlyout);
            moveToListView2.Perform();
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
    }
}
