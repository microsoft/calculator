// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium.Windows;
using System;
using System.Globalization;

namespace CalculatorUITestFramework
{
    public class NumberPad
    {
        public WindowsElement Num0Button => this.session.TryFindElementByAccessibilityId("num0Button");
        public WindowsElement Num1Button => this.session.TryFindElementByAccessibilityId("num1Button");
        public WindowsElement Num2Button => this.session.TryFindElementByAccessibilityId("num2Button");
        public WindowsElement Num3Button => this.session.TryFindElementByAccessibilityId("num3Button");
        public WindowsElement Num4Button => this.session.TryFindElementByAccessibilityId("num4Button");
        public WindowsElement Num5Button => this.session.TryFindElementByAccessibilityId("num5Button");
        public WindowsElement Num6Button => this.session.TryFindElementByAccessibilityId("num6Button");
        public WindowsElement Num7Button => this.session.TryFindElementByAccessibilityId("num7Button");
        public WindowsElement Num8Button => this.session.TryFindElementByAccessibilityId("num8Button");
        public WindowsElement Num9Button => this.session.TryFindElementByAccessibilityId("num9Button");
        public WindowsElement DecimalButton => this.session.TryFindElementByAccessibilityId("decimalSeparatorButton");
        public WindowsElement NegateButton => this.session.TryFindElementByAccessibilityId("negateButton");

        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;

        /// <summary>
        /// Translates a number into the Calculator button clicks.
        /// </summary>
        /// <param name="number">Number to be entered into the calculator.</param>
        public void Input(double number)
        {
            string numberStr = number.ToString(CultureInfo.InvariantCulture);
            if (numberStr.StartsWith("-"))
            {
                numberStr = numberStr.Substring(1) + "-";
            }
            foreach (char digit in numberStr)
            {
                switch (digit)
                {
                    case '0':
                        this.Num0Button.Click();
                        break;
                    case '1':
                        this.Num1Button.Click();
                        break;
                    case '2':
                        this.Num2Button.Click();
                        break;
                    case '3':
                        this.Num3Button.Click();
                        break;
                    case '4':
                        this.Num4Button.Click();
                        break;
                    case '5':
                        this.Num5Button.Click();
                        break;
                    case '6':
                        this.Num6Button.Click();
                        break;
                    case '7':
                        this.Num7Button.Click();
                        break;
                    case '8':
                        this.Num8Button.Click();
                        break;
                    case '9':
                        this.Num9Button.Click();
                        break;
                    case '.':
                        this.DecimalButton.Click();
                        break;
                    case '-':
                        this.NegateButton.Click();
                        break;
                    default:
                        throw (new ArgumentException(String.Format("{0} is not valid", digit)));
                }
            }
        }
    }
}
