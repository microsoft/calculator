// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using System;
using System.Diagnostics;
using System.Threading;

using OpenQA.Selenium;
using OpenQA.Selenium.Appium.Windows;

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// Drives the unit picker flyout that opens when a unit chip is clicked in the redesigned
    /// converter. The flyout hosts a search box, a category list (UnitPickerCategories), and a
    /// unit list (UnitPickerResults). Selecting a category switches the units shown in the picker;
    /// clicking a unit commits the selection and closes the flyout.
    /// </summary>
    public class UnitPickerFlyout
    {
        private const string CategoryListAutomationId = "UnitPickerCategories";
        private const string UnitListAutomationId = "UnitPickerResults";

        /// <summary>
        /// Budget for the dismiss poll. The same wait the driver already applies to every other
        /// element lookup, so this is not a second, independently tuned number. The poll returns as
        /// soon as the flyout is gone, so the budget only bounds a genuine failure.
        /// </summary>
        private static readonly TimeSpan DismissTimeout = CalculatorDriver.ImplicitWait;

        private WindowsDriver<WindowsElement> session => CalculatorDriver.Instance.CalculatorSession;

        public WindowsElement SearchBox =>
            this.session.TryFindElementByAccessibilityId("UnitPickerSearchBox");

        public WindowsElement FirstUnit =>
            this.session.FindElementByXPath($"(//*[@AutomationId='{UnitListAutomationId}']//ListItem)[1]");

        public WindowsElement FocusedCategory =>
            this.session.FindElementByXPath(
                $"//*[@AutomationId='{CategoryListAutomationId}']//ListItem[@HasKeyboardFocus='True']");

        public WindowsElement FocusedUnit =>
            this.session.FindElementByXPath(
                $"//*[@AutomationId='{UnitListAutomationId}']//ListItem[@HasKeyboardFocus='True']");

        public void Open(WindowsElement chip)
        {
            chip.SendKeys(Keys.Enter);
            this.SearchBox.WaitForDisplayed();
        }

        public void Dismiss()
        {
            CalculatorApp.Window.SendKeys(Keys.Escape);
            this.WaitForPickerToClose();
        }

        /// <summary>
        /// Opens the picker for the given chip and commits the unit whose accessible name matches
        /// the supplied "Region - Unit" value (the " - " separator is dropped to match the
        /// unit's accessible name, as in the pre-redesign combo box).
        /// </summary>
        /// <param name="chip">The Units1 or Units2 chip button.</param>
        /// <param name="value">The unit value in "Region - Unit" form.</param>
        public void SelectUnit(WindowsElement chip, string value)
        {
            chip.Click();
            string accessibleName = value.Replace(" - ", " ");
            var item = session.FindElementByXPath(
                $"//*[@AutomationId='{UnitListAutomationId}']//ListItem[@Name={ToXPathLiteral(accessibleName)}]");
            item.Click();
            this.WaitForPickerToClose();
        }

        /// <summary>
        /// Opens the picker for the given chip, switches to the named category, and commits its
        /// first unit, leaving the converter in the selected category.
        /// </summary>
        /// <param name="chip">The Units1 or Units2 chip button.</param>
        /// <param name="categoryName">The localized category name, for example "Currency".</param>
        public void SelectCategoryAndFirstUnit(WindowsElement chip, string categoryName)
        {
            chip.Click();
            var category = session.FindElementByXPath(
                $"//*[@AutomationId='{CategoryListAutomationId}']//ListItem[@Name={ToXPathLiteral(categoryName)}]");
            category.Click();
            var item = session.FindElementByXPath(
                $"(//*[@AutomationId='{UnitListAutomationId}']//ListItem)[1]");
            item.Click();
            this.WaitForPickerToClose();
        }

        /// <summary>
        /// Waits for the flyout to finish dismissing after a unit is committed. Clicking the next
        /// chip while the flyout is still closing leaves that click swallowed or the chip reported
        /// as not interactable. The implicit wait is dropped to zero for the poll, otherwise each
        /// negative probe would block for the full implicit timeout, then restored to the value
        /// CalculatorDriver applies when it creates the session.
        /// </summary>
        private void WaitForPickerToClose()
        {
            var timeouts = this.session.Manage().Timeouts();
            timeouts.ImplicitWait = TimeSpan.Zero;
            try
            {
                var timer = Stopwatch.StartNew();
                while (timer.Elapsed < DismissTimeout)
                {
                    if (this.session.FindElementsByAccessibilityId(UnitListAutomationId).Count == 0)
                    {
                        return;
                    }

                    Thread.Sleep(100);
                }
            }
            finally
            {
                timeouts.ImplicitWait = CalculatorDriver.ImplicitWait;
            }

            Assert.Fail(
                $"The unit picker flyout was still open {DismissTimeout.TotalSeconds} seconds after a unit was "
                + "committed. Without this check the next chip click fails as 'element not interactable' instead.");
        }

        /// <summary>
        /// Quotes a value for use as an XPath string literal. Unit names are localized and some
        /// contain an apostrophe (it-IT "Miglia all'ora", nl-NL "BTU's/minuut"), which would
        /// otherwise close the literal early and produce an invalid expression.
        /// </summary>
        private static string ToXPathLiteral(string value)
        {
            if (!value.Contains("'"))
            {
                return $"'{value}'";
            }

            if (!value.Contains("\""))
            {
                return $"\"{value}\"";
            }

            return "concat('" + value.Replace("'", "', \"'\", '") + "')";
        }
    }
}
