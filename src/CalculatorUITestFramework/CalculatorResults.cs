// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium.Windows;
using System;
using OpenQA.Selenium.Interactions;

namespace CalculatorUITestFramework
{
    public class CalculatorResults
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        private WindowsElement CalculatorAlwaysOnTopResults => this.session.TryFindElementByAccessibilityId("CalculatorAlwaysOnTopResults");
        private WindowsElement CalculatorResult => this.session.TryFindElementByAccessibilityId("CalculatorResults");
        private WindowsElement CalculatorExpression => this.session.TryFindElementByAccessibilityId("CalculatorExpression");
        private WindowsElement MenuItemCopy => this.session.WaitForElementByAccessibilityId("CopyMenuItem");
        private WindowsElement MenuItemPaste => this.session.WaitForElementByAccessibilityId("PasteMenuItem");

        /// <summary>
        /// Gets the text from the display control in AoT mode and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetAoTCalculatorResultText()
        {
            return this.CalculatorAlwaysOnTopResults.Text.Replace("Display is", string.Empty).Trim();
        }

        /// <summary>
        /// Gets the text from the display control and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculatorResultText()
        {
            return this.CalculatorResult.Text.Replace("Display is", string.Empty).Trim();
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
        /// Verifies that CalculatorResult is not null
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public void IsResultsDisplayPresent()
        {
            Assert.IsNotNull(this.CalculatorResult);
        }

        /// <summary>
        /// Verifies that Calculator Expression is clear
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public void IsResultsExpressionClear()
        {
            string source = WinAppDriver.Instance.CalculatorSession.PageSource;
            if (source.Contains("CalculatorExpression"))
            {
                throw new Exception("The Calculator Expression is not clear");
            }
        }

        /// <summary>
        /// Opens the context menu in order to be able to click its items
        /// </summary>
        private void OpenContextMenu()
        {
            Actions actions = new Actions(CalculatorResult.WrappedDriver);
            // It is important to move not to the centre in order to avoid click on the text
            actions.MoveToElement(CalculatorResult, 1,1).ContextClick().Perform();
        }

        /// <summary>
        /// Opens the context menu and clicks the "Copy" item there
        /// </summary>
        public void ContextMenuItemCopyClick()
        {
            OpenContextMenu();
            MenuItemCopy.Click();
        }

        /// <summary>
        /// Opens the context menu and clicks the "Paste" item there
        /// </summary>
        public void ContextMenuItemPasteClick()
        {
            OpenContextMenu();
            MenuItemPaste.Click();
        }
    }
}
