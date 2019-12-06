// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    public class CalculatorResults
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        private WindowsElement CalculatorAlwaysOnTopResults => this.session.TryFindElementByAccessibilityId("CalculatorAlwaysOnTopResults");
        private WindowsElement CalculatorResult => this.session.TryFindElementByAccessibilityId("CalculatorResults");
        private WindowsElement CalculatorExpression => this.session.TryFindElementByAccessibilityId("CalculatorExpression");

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
        public void CalculatorResultsDisplayIsPresent()
        {
            Assert.IsNotNull(this.CalculatorResult);
        }
    }
}
