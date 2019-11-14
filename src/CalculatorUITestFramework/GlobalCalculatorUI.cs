// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Appium;
using OpenQA.Selenium.Appium.Windows;
using OpenQA.Selenium.Interactions;
using System;
using System.Drawing;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// This class contains the UI automation objects and helper methods available when the Calculator is in any mode.
    /// </summary>
    public class GlobalCalculatorUI
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement CalculatorResult => this.session.TryFindElementByAccessibilityId("CalculatorResults");
        public WindowsElement CalculatorExpression => this.session.TryFindElementByAccessibilityId("CalculatorExpression");
        public WindowsElement Header => this.session.TryFindElementByAccessibilityId("Header");
        public WindowsElement Minimize => this.session.TryFindElementByAccessibilityId("Minimize");
        public WindowsElement Maximize => this.session.TryFindElementByAccessibilityId("Maximize");
        public WindowsElement AppName => this.session.TryFindElementByAccessibilityId("AppName");
        public WindowsElement ScrollLeft => this.session.TryFindElementByAccessibilityId("CalculationResultScrollLeft");
        public WindowsElement ScrollRight => this.session.TryFindElementByAccessibilityId("CalculationResultScrollRight");
        public WindowsElement Window => this.session.FindElementByClassName("Windows.UI.Core.CoreWindow");

        /// <summary>
        /// Gets the text from the Header
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculatorHeaderText()
        {
            return this.Header.Text;
        }
        /// <summary>
        /// Gets the text from the Calculator Expression control and removes narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculatorExpressionText()
        {
            return this.CalculatorExpression.Text.Replace("Expression is", string.Empty).Trim();
        }
        /// <summary>
        /// Gets the text from the display control and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculatorResultText()
        {
            return this.CalculatorResult.Text.Replace("Display is", string.Empty).Trim();
        }
    }

}
