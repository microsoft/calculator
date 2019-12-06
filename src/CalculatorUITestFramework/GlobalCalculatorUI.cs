// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using OpenQA.Selenium.Appium.Windows;
using System.Drawing;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in any mode.
    /// </summary>
    public class GlobalCalculatorUI
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");
        public WindowsElement Minimize => this.session.TryFindElementByAccessibilityId("Minimize");
        public WindowsElement Maximize => this.session.TryFindElementByAccessibilityId("Maximize");
        private WindowsElement AppName => this.session.TryFindElementByAccessibilityId("AppName");
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
        ///// Gets the calculators window's X and Y position and returns it as a string. e.g. "(0, 0)"
        ///// </summary>
        public string GetCalculatorWindowPosition()
        {
            Point getCalculatorWindowPosition = WinAppDriver.Instance.CalculatorSession.Manage().Window.Position;
            int X = getCalculatorWindowPosition.X;
            int Y = getCalculatorWindowPosition.Y;
            var CalculatorWindowPosition = (X, Y);
            return CalculatorWindowPosition.ToString();
        }
        ///// <summary>
        ///// Gets the calculators window's width and height and returns it as a string. e.g. "(322, 396)"
        ///// </summary>
        public string GetCalculatorWindowSize()
        {
            Size getCalculatorWindowSize = WinAppDriver.Instance.CalculatorSession.Manage().Window.Size;
            int Width = getCalculatorWindowSize.Width;
            int Height = getCalculatorWindowSize.Height;
            var CalculatorWindowSize = (Width, Height);
            return CalculatorWindowSize.ToString();
        }
        ///// <summary>
        ///// Clicks the AppName element on Windows Calculator to ensure that the app has focus
        ///// </summary>
        public void EnsureCalculatorHasFocus()
        {
            this.AppName.Click();
        }
    }

}
