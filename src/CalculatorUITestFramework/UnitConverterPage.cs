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

        /// <summary>
        /// Ensure Units1 and Units2 are the same 
        /// </summary>
        public void EnsureSameUnitsAreSelected()
        {
            CalculatorApp.ClickOnWindow();
            UnitConverterOperators.Units1.SendKeys(OpenQA.Selenium.Keys.Home);
            UnitConverterOperators.Units1.SendKeys(OpenQA.Selenium.Keys.Enter);

            CalculatorApp.ClickOnWindow();
            UnitConverterOperators.Units2.SendKeys(OpenQA.Selenium.Keys.Home);
            UnitConverterOperators.Units2.SendKeys(OpenQA.Selenium.Keys.Enter);

            CalculatorApp.ClickOnWindow();
        }

        /// <summary>
        /// Select value in Units1 ComboBox
        /// </summary>
        /// <param name="value">Value in ComboBox Units1</param>
        public void SelectUnits1(string value)
        {
            CalculatorApp.ClickOnWindow();
            UnitConverterOperators.Units1.SendKeys(value);
            UnitConverterOperators.Units1.SendKeys(OpenQA.Selenium.Keys.Enter);
            CalculatorApp.ClickOnWindow();
        }

        /// <summary>
        /// Select value in Units2 ComboBox
        /// </summary>
        /// <param name="value">Value in ComboBox Units2</param>
        public void SelectUnits2(string value)
        {
            CalculatorApp.ClickOnWindow();
            UnitConverterOperators.Units2.SendKeys(value);
            UnitConverterOperators.Units2.SendKeys(OpenQA.Selenium.Keys.Enter);
            CalculatorApp.ClickOnWindow();
        }
    }
}
