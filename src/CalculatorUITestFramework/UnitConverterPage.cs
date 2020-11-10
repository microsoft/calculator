// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorUITestFramework
{
    public class UnitConverterPage
    {
        public UnitConverterOperatorsPanel UnitConverterOperators = new UnitConverterOperatorsPanel();
        public NavigationMenu NavigationMenu = new NavigationMenu();
        public UnitConverterResults UnitConverterResults = new UnitConverterResults();

        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;

        /// <summary>
        /// Clear the Calculator display
        /// </summary>
        public void ClearAll()
        {
            this.UnitConverterOperators.ClearButton.Click();
        }

        ///// <summary>
        ///// Ensures that the calculator result text is zero; if not, clears all
        ///// </summary>
        public void EnsureCalculatorResultTextIsZero()
        {
            if ("0" != this.UnitConverterResults.GetCalculationResult1Text())
            {
                this.ClearAll();
            }
        }

        /// <summary>
        /// Navigates the calculator is in currency mode
        /// </summary>
        public void NavigateToUnitConverter()
        {
            // Ensure that calculator is in Currency Mode
            this.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Currency);
            this.UnitConverterResults.IsResultsDisplayPresent();
        }

        ///// <summary>
        ///// Ensures that the calculator is in Currency Mode
        ///// </summary>
        public void EnsureCalculatorIsCurrencyMode()
        {
            string source = WinAppDriver.Instance.CalculatorSession.PageSource;
            if (source.Contains("Header"))
            {
                string header = CalculatorApp.Header.Text;
                if (header == "Currency")
                {
                    return;
                }
                else
                {
                    this.NavigateToUnitConverter();
                }
            }
        }

    }
}
