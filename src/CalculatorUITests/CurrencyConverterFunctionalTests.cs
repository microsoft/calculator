// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using CalculatorUITestFramework;

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace CalculatorUITests
{
    [TestClass]
    public class CurrencyConverterFunctionalTests
    {
        private static readonly UnitConverterPage page = new UnitConverterPage();

        public TestContext TestContext { get; set; }

        /// <summary>
        /// Initializes the WinAppDriver web driver session.
        /// </summary>
        /// <param name="context"></param>
        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            // Create session to launch a Calculator window
            CalculatorDriver.Instance.SetupCalculatorSession(context);
        }

        /// <summary>
        /// Closes the app and WinAppDriver web driver session.
        /// </summary>
        [ClassCleanup]
        public static void ClassCleanup()
        {
            // Tear down Calculator session.
            CalculatorDriver.Instance.TearDownCalculatorSession();
        }

        /// <summary>
        /// Ensures the calculator is in a cleared state
        /// </summary>
        [TestInitialize]
        public void TestInit()
        {
            CalculatorApp.EnsureCalculatorHasFocus();
            page.EnsureCalculatorIsCurrencyMode();
            page.EnsureCalculatorResultTextIsZero();
            page.EnsureSameUnitsAreSelected();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.ClearAll();
        }

        private string NormalizeCurrencyText(string realValue, int fractionDigits)
        {
            if (!realValue.Contains('.')) return realValue;

            var parts = realValue.Split('.');
            if (parts[1].Length < fractionDigits)
            {
                parts[1] += new string('0', fractionDigits - parts[1].Length);
            }
            else
            {
                parts[1] = parts[1][..fractionDigits];
            }

            return $"{parts[0]}.{parts[1]}".TrimEnd('.');
        }

        #region Basic UI Functionality via Mouse Input Tests
        /// <summary>
        /// These automated tests verify clicking each of the buttons in the Calculator UI and getting an expected result
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void MouseInput_EnterInputAndCheckTheResult()
        {
            //Verifies the 2 is entered and clear is functional
            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 2 button
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 2 button
        }

        /// <summary>
        /// These automated tests verify clicking each of the buttons in the Calculator UI and getting an expected result
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void MouseInput_EnterInputWithFullDecimalAndClear()
        {
            //Verifies the 20.42 is entered and clear is functional
            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 2 button
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 2 button
            page.UnitConverterOperators.NumberPad.Num0Button.Click();
            Assert.AreEqual("20", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 2 button
            Assert.AreEqual("20", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 2 button
            page.UnitConverterOperators.NumberPad.DecimalButton.Click();
            Assert.AreEqual("20.00", page.UnitConverterResults.GetCalculationResult1Text()); //verifies decimal button
            Assert.AreEqual("20", page.UnitConverterResults.GetCalculationResult2Text()); //verifies decimal button
            page.UnitConverterOperators.NumberPad.Num4Button.Click();
            Assert.AreEqual("20.40", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 4 button
            Assert.AreEqual("20.40", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 4 button
            page.UnitConverterOperators.NumberPad.Num3Button.Click();
            Assert.AreEqual("20.43", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 3 button
            Assert.AreEqual("20.43", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 3 button
            page.UnitConverterOperators.NumberPad.Num3Button.Click();
            page.UnitConverterOperators.ClearButton.Click();
            Assert.AreEqual("0", page.UnitConverterResults.GetCalculationResult1Text()); //verifies Clear button
            Assert.AreEqual("0", page.UnitConverterResults.GetCalculationResult2Text()); //verifies Clear button
        }

        /// <summary>
        /// These automated tests verify clicking each of the buttons in the Calculator UI and getting an expected result
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void MouseInput_EnterInputWithFullDecimalAndClearWithBackspace()
        {
            //Verifies the 20.42 is entered and clear is functional
            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 2 button
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 2 button
            page.UnitConverterOperators.NumberPad.Num0Button.Click();
            Assert.AreEqual("20", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 2 button
            Assert.AreEqual("20", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 2 button
            page.UnitConverterOperators.NumberPad.DecimalButton.Click();
            Assert.AreEqual("20.00", page.UnitConverterResults.GetCalculationResult1Text()); //verifies decimal button
            Assert.AreEqual("20", page.UnitConverterResults.GetCalculationResult2Text()); //verifies decimal button
            page.UnitConverterOperators.NumberPad.Num4Button.Click();
            Assert.AreEqual("20.40", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 4 button
            Assert.AreEqual("20.40", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 4 button
            page.UnitConverterOperators.NumberPad.Num3Button.Click();
            Assert.AreEqual("20.43", page.UnitConverterResults.GetCalculationResult1Text()); //verifies 3 button
            Assert.AreEqual("20.43", page.UnitConverterResults.GetCalculationResult2Text()); //verifies 3 button
            page.UnitConverterOperators.NumberPad.Num3Button.Click();
            page.UnitConverterOperators.BackSpaceButton.Click();
            page.UnitConverterOperators.BackSpaceButton.Click();
            page.UnitConverterOperators.BackSpaceButton.Click();
            page.UnitConverterOperators.BackSpaceButton.Click();
            page.UnitConverterOperators.BackSpaceButton.Click();
            Assert.AreEqual("0", page.UnitConverterResults.GetCalculationResult1Text()); //verifies Backspace button clicks
            Assert.AreEqual("0", page.UnitConverterResults.GetCalculationResult2Text()); //verifies Backspace button clicks
        }

        /// <summary>
        /// These automated tests verify the currency has been formatted to 3 fractional digits
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void MouseInput_SelectCurrencyWith3FractionalDigitsEnterInputAndCheckTheFormat()
        {
            var currency = (string)TestContext.Properties["CurrencyWith3FractionalDigits"];
            var fractionDigits = 3;

            //Verifies fraction digits in given currency
            page.SelectUnits1(currency);
            Assert.AreEqual(currency.Replace(" - ", " "), page.UnitConverterOperators.Units1.Text); // Text is the AccessibleName of Unit
            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            Assert.AreEqual(NormalizeCurrencyText("2", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies 2 button
            page.UnitConverterOperators.NumberPad.DecimalButton.Click();
            Assert.AreEqual(NormalizeCurrencyText("2.", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies decimal button
            page.UnitConverterOperators.NumberPad.Num4Button.Click();
            Assert.AreEqual(NormalizeCurrencyText("2.4", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies 4 button
            page.UnitConverterOperators.NumberPad.Num3Button.Click();
            Assert.AreEqual(NormalizeCurrencyText("2.43", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies 3 button
            page.UnitConverterOperators.NumberPad.Num5Button.Click();
            Assert.AreEqual(NormalizeCurrencyText("2.435", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies 5 button
            page.UnitConverterOperators.NumberPad.Num6Button.Click();
            Assert.AreEqual(NormalizeCurrencyText("2.4356", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies 6 button
        }

        /// <summary>
        /// These automated tests verify the currency has been formatted to no fractional digit
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void MouseInput_SelectCurrencyWithoutFractionalDigitEnterInputAndCheckTheFormat()
        {
            var currency = (string)TestContext.Properties["CurrencyWithoutFractionalDigits"];
            var fractionDigits = 0;

            //Verifies fraction digits in given currency
            page.SelectUnits1(currency);
            Assert.AreEqual(currency.Replace(" - ", " "), page.UnitConverterOperators.Units1.Text); // Text is the AccessibleName of Unit
            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            Assert.AreEqual(NormalizeCurrencyText("2", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies 2 button
            page.UnitConverterOperators.NumberPad.DecimalButton.Click(); // It should be disabled, so no decimal will be displayed
            Assert.AreEqual(NormalizeCurrencyText("2", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies decimal button
            page.UnitConverterOperators.NumberPad.Num4Button.Click(); // As decimal is disabled, 4 won't be part of fraction digits
            Assert.AreEqual(NormalizeCurrencyText("24", fractionDigits), page.UnitConverterResults.GetCalculationResult1Text()); //verifies 4 button
        }

        /// <summary>
        /// These automated tests verify the currency format is updated after switching
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void MouseInput_SwitchCurrencyWithDifferentFractionalDigitsAndCheckTheFormat()
        {
            var currencyWith3FractionalDigits = (string)TestContext.Properties["CurrencyWith3FractionalDigits"];
            var currencyWithoutFractionalDigits = (string)TestContext.Properties["CurrencyWithoutFractionalDigits"];

            //Verifies fraction digits in given currency
            page.SelectUnits1(currencyWith3FractionalDigits);
            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            page.UnitConverterOperators.NumberPad.DecimalButton.Click();
            page.UnitConverterOperators.NumberPad.Num4Button.Click();
            page.UnitConverterOperators.NumberPad.Num3Button.Click();
            page.UnitConverterOperators.NumberPad.Num5Button.Click();
            Assert.AreEqual("2.435", page.UnitConverterResults.GetCalculationResult1Text());

            page.SelectUnits1(currencyWithoutFractionalDigits);
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult1Text());

            // The digits will be truncated forever, even if swiching back
            page.SelectUnits1(currencyWith3FractionalDigits);
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult1Text());
        }

        /// <summary>
        /// These automated tests verify input is not blocked after swiching to currency with less fractional digits
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void MouseInput_SwitchCurrencyWithLessFractionalDigitsAndEnterInput()
        {
            var currencyWith3FractionalDigits = (string)TestContext.Properties["CurrencyWith3FractionalDigits"];
            var currencyWithoutFractionalDigits = (string)TestContext.Properties["CurrencyWithoutFractionalDigits"];

            //Verifies fraction digits in given currency
            page.SelectUnits1(currencyWith3FractionalDigits);
            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            page.UnitConverterOperators.NumberPad.DecimalButton.Click();
            page.UnitConverterOperators.NumberPad.Num4Button.Click();
            page.UnitConverterOperators.NumberPad.Num3Button.Click();
            page.UnitConverterOperators.NumberPad.Num5Button.Click();
            Assert.AreEqual("2.435", page.UnitConverterResults.GetCalculationResult1Text());

            page.SelectUnits1(currencyWithoutFractionalDigits);
            Assert.AreEqual("2", page.UnitConverterResults.GetCalculationResult1Text());

            page.UnitConverterOperators.NumberPad.Num4Button.Click(); // Enter new number will replace the previous value
            Assert.AreEqual("4", page.UnitConverterResults.GetCalculationResult1Text());

            page.SelectUnits1(currencyWith3FractionalDigits);
            Assert.AreEqual("4", page.UnitConverterResults.GetCalculationResult1Text());
        }

        /// <summary>
        /// These automated tests verify the result consists after swiching currency
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void MouseInput_SwitchCurrencyWithLessFractionalDigitsAndCheckIfTheResultIsConsistent()
        {
            var currencyWith3FractionalDigits = (string)TestContext.Properties["CurrencyWith3FractionalDigits"];
            var currencyWithoutFractionalDigits = (string)TestContext.Properties["CurrencyWithoutFractionalDigits"];

            page.SelectUnits1(currencyWith3FractionalDigits);
            page.SelectUnits2(currencyWith3FractionalDigits);

            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            page.UnitConverterOperators.NumberPad.Num0Button.Click();
            page.UnitConverterOperators.NumberPad.Num0Button.Click();
            page.UnitConverterOperators.NumberPad.DecimalButton.Click();
            page.UnitConverterOperators.NumberPad.Num9Button.Click();
            page.UnitConverterOperators.NumberPad.Num9Button.Click();
            page.UnitConverterOperators.NumberPad.Num9Button.Click();

            Assert.AreEqual("200.999", page.UnitConverterResults.GetCalculationResult1Text());
            Assert.AreEqual("200.999", page.UnitConverterResults.GetCalculationResult2Text());

            page.SelectUnits1(currencyWithoutFractionalDigits);
            Assert.AreEqual("200", page.UnitConverterResults.GetCalculationResult1Text());
            var result = page.UnitConverterResults.GetCalculationResult2Text();

            page.UnitConverterOperators.ClearButton.Click();

            page.UnitConverterOperators.NumberPad.Num2Button.Click();
            page.UnitConverterOperators.NumberPad.Num0Button.Click();
            page.UnitConverterOperators.NumberPad.Num0Button.Click();

            Assert.AreEqual("200", page.UnitConverterResults.GetCalculationResult1Text());
            Assert.AreEqual(result, page.UnitConverterResults.GetCalculationResult2Text());
        }

        #endregion
    }
}
