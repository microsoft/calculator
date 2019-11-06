// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using System.Collections.ObjectModel;
using OpenQA.Selenium.Interactions;
using System.Windows.Input;

namespace CalculatorUITestFramework
{
    public class CalcHistoryFlyout
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public HistoryPanel HistoryPanel = new HistoryPanel();
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");
        public WindowsElement AppName => this.session.TryFindElementByAccessibilityId("AppName");
        public WindowsElement HistoryFlyout => this.session.TryFindElementByAccessibilityId("HistoryFlyout");
        public AppiumWebElement ListViewItem => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByClassName("ListViewItem");
        public AppiumWebElement HistoryListView => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByAccessibilityId("HistoryFlyout").FindElementByAccessibilityId("HistoryListView");
        public AppiumWebElement ClearHistoryButton => WinAppDriver.Instance.CalculatorSession.FindElementByClassName("Popup").FindElementByAccessibilityId("HistoryFlyout").FindElementByAccessibilityId("ClearHistory");
        public AppiumWebElement HistoryEmpty => WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryFlyout").FindElementByAccessibilityId("HistoryEmpty");
        public WindowsElement ExprTextBlock => this.session.TryFindElementByAccessibilityId("ExprTextBlock");
        public WindowsElement ResultTextBlock => this.session.TryFindElementByAccessibilityId("ResultTextBlock");


        /// <summary>
        /// Opens the History Flyout.
        /// </summary>
        public void OpenHistoryFlyout()
        {
            this.HistoryPanel.ResizeWindowToDiplayHistoryButton();
            this.AppName.Click();
            this.Header.SendKeys(Keys.Control + "h" + Keys.Control);
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
