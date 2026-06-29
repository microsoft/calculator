// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using Microsoft.VisualStudio.TestTools.UnitTesting;

using OpenQA.Selenium.Appium.Windows;

using System.Text.RegularExpressions;

namespace CalculatorUITestFramework
{
    public class UnitConverterResults
    {
        private WindowsDriver<WindowsElement> session => CalculatorDriver.Instance.CalculatorSession;
        private WindowsElement CalculationResult1 => session.TryFindElementByAccessibilityId("Value1");

        private WindowsElement CalculationResult2 => session.TryFindElementByAccessibilityId("Value2");

        /// <summary>
        /// Gets the text from the Value1 control and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculationResult1Text()
        {
            return Regex.Replace(CalculationResult1.Text.Trim(), "[^0-9.]", "");
        }

        /// <summary>
        /// Verifies that CalculationResult1 and CalculationResult2 are not null
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public void IsResultsDisplayPresent()
        {
            Assert.IsNotNull(CalculationResult1);
            Assert.IsNotNull(CalculationResult2);
        }

        /// <summary>
        /// Gets the text from the Value2 control and removes the narrator text that is not displayed in the UI.
        /// </summary>
        /// <returns>The string shown in the UI.</returns>
        public string GetCalculationResult2Text()
        {
            return Regex.Replace(CalculationResult2.Text.Trim(), "[^0-9.]", "");
        }

    }
}
