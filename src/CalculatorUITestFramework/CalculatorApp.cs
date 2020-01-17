// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using OpenQA.Selenium;
using OpenQA.Selenium.Appium.Windows;
using System.Drawing;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in any mode.
    /// </summary>
    public class CalculatorApp
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");
        public WindowsElement Minimize => this.session.TryFindElementByAccessibilityId("Minimize");
        public WindowsElement Maximize => this.session.TryFindElementByAccessibilityId("Maximize");
        private WindowsElement AppName => this.session.TryFindElementByAccessibilityId("AppName");
        public WindowsElement DockPanel => this.session.TryFindElementByAccessibilityId("DockPanel");
        public WindowsElement Window => this.session.FindElementByClassName("Windows.UI.Core.CoreWindow");

        /// <summary>
        /// Gets the text from the Header
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculatorHeaderText()
        {
            return this.Header.Text;
        }
        ///// <summary>
        ///// Clicks the AppName element on Windows Calculator to ensure that the app has focus
        ///// </summary>
        public void EnsureCalculatorHasFocus()
        {
            this.AppName.Click();
        }
        /// <summary>
        /// If the the Dock Panel for the History and Memory lists is not displayed, resize the window
        /// Two attempts are made, the the lable is not found a "not found" exception is thrown
        /// </summary>
        public void ResizeWindowToDiplayMemoryHistoryDockPanel()
        {
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = new Point(8, 8);
            if (this.session.PageSource.Contains("DockPanel"))
            {
                return;
            }
            else
            {
                WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(1200, 1050);
                if (this.session.PageSource.Contains("DockPanel"))
                {
                    return;
                }
                else
                {
                    WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = new Size(2097, 1282);
                }
                if (this.session.PageSource.Contains("DockPanel"))
                {
                    return;
                }
                else
                {
                    throw new NotFoundException("Could not the Dock Panel for the History and Memory lists");
                }
            }
        }
    }

}
