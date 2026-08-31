// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    public class UnitConverterPage
    {
        public UnitConverterOperatorsPanel UnitConverterOperators = new UnitConverterOperatorsPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public UnitConverterResults UnitConverterResults = new UnitConverterResults();
        public UnitPickerFlyout UnitPicker = new UnitPickerFlyout();

        private WindowsDriver<WindowsElement> session => CalculatorDriver.Instance.CalculatorSession;

        // The redesigned converter collapses the 13 converter modes into a single "Converter"
        // navigation entry whose automation id is the default converter mode (Length). Selecting it
        // opens the last-used converter; the Currency category is then chosen through the unit picker.
        private const string ConverterNavEntryAutomationId = "Length";

        // The Currency category name shown in the picker's category list.
        private const string CurrencyCategoryName = "Currency";

        // The header displays "Converters" for every converter, so its visible text no longer
        // identifies the active converter. The accessible name keeps the per-category context, and
        // that is what distinguishes Currency.
        private const string CurrencyHeaderAccessibleName = "Converter: Currency";

        /// <summary>
        /// Clear the Calculator display
        /// </summary>
        public void ClearAll()
        {
            UnitConverterOperators.ClearButton.Click();
        }

        ///// <summary>
        ///// Ensures that the calculator result text is zero; if not, clears all
        ///// </summary>
        public void EnsureCalculatorResultTextIsZero()
        {
            if ("0" != UnitConverterResults.GetCalculationResult1Text())
            {
                ClearAll();
            }
        }

        /// <summary>
        /// Opens the collapsed Converter navigation entry.
        /// </summary>
        public void OpenConverter()
        {
            this.NavigationMenu.NavigationMenuButton.Click();
            this.NavigationMenu.NavigationMenuPane.WaitForDisplayed();
            this.session.TryFindElementByAccessibilityId(ConverterNavEntryAutomationId).Click();
        }

        /// <summary>
        /// Opens the collapsed Converter entry and selects the Currency category through the
        /// from-unit picker, leaving the calculator in the Currency converter.
        /// </summary>
        public void NavigateToUnitConverter()
        {
            this.OpenConverter();
            this.UnitPicker.SelectCategoryAndFirstUnit(this.UnitConverterOperators.Units1, CurrencyCategoryName);
            this.UnitConverterResults.IsResultsDisplayPresent();
        }

        ///// <summary>
        ///// Ensures that the calculator is in Currency Mode
        ///// </summary>
        public void EnsureCalculatorIsCurrencyMode()
        {
            string source = CalculatorDriver.Instance.CalculatorSession.PageSource;
            if (source.Contains("Header")
                && CalculatorApp.GetCalculatorHeaderAccessibleName() == CurrencyHeaderAccessibleName)
            {
                return;
            }

            this.NavigateToUnitConverter();
        }

        /// <summary>
        /// Puts the two chips on a known pair of distinct currencies, giving the currency tests a
        /// deterministic baseline.
        /// <para>
        /// This replaces the pre-redesign "send Home to both combo boxes" setup, which left both
        /// sides on the same unit so every conversion was 1:1. The converter now deliberately
        /// prevents both chips from showing the same unit, because converting a unit into itself is
        /// not a useful conversion, so tests can no longer rely on that 1:1 baseline.
        /// </para>
        /// </summary>
        /// <param name="fromCurrency">From-unit value in "Region - Unit" form.</param>
        /// <param name="toCurrency">To-unit value in "Region - Unit" form.</param>
        public void SelectCurrencyPair(string fromCurrency, string toCurrency)
        {
            this.UnitPicker.SelectUnit(this.UnitConverterOperators.Units1, fromCurrency);
            this.UnitPicker.SelectUnit(this.UnitConverterOperators.Units2, toCurrency);
        }

        /// <summary>
        /// Select a unit for the "from" (Units1) chip through the picker flyout.
        /// </summary>
        /// <param name="value">Unit value in "Region - Unit" form.</param>
        public void SelectUnits1(string value)
        {
            this.UnitPicker.SelectUnit(this.UnitConverterOperators.Units1, value);
        }

        /// <summary>
        /// Select a unit for the "to" (Units2) chip through the picker flyout.
        /// </summary>
        /// <param name="value">Unit value in "Region - Unit" form.</param>
        public void SelectUnits2(string value)
        {
            this.UnitPicker.SelectUnit(this.UnitConverterOperators.Units2, value);
        }
    }
}
