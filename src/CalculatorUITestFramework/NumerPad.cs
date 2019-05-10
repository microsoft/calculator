using System;
using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    public class NumerPad : ApplicationBase
    {
        public WindowsElement Num0Button => TryFindElementByAccessibilityId("num0Button");
        public WindowsElement Num1Button => TryFindElementByAccessibilityId("num1Button");
        public WindowsElement Num2Button => TryFindElementByAccessibilityId("num2Button");
        public WindowsElement Num3Button => TryFindElementByAccessibilityId("num3Button");
        public WindowsElement Num4Button => TryFindElementByAccessibilityId("num4Button");
        public WindowsElement Num5Button => TryFindElementByAccessibilityId("num5Button");
        public WindowsElement Num6Button => TryFindElementByAccessibilityId("num6Button");
        public WindowsElement Num7Button => TryFindElementByAccessibilityId("num7Button");
        public WindowsElement Num8Button => TryFindElementByAccessibilityId("num8Button");
        public WindowsElement Num9Button => TryFindElementByAccessibilityId("num9Button");
        public WindowsElement DecimalButton => TryFindElementByAccessibilityId("decimalSeparatorButton");
        public WindowsElement NegateButton => TryFindElementByAccessibilityId("negateButton");

        /// <summary>
        /// Translates a number into the Calculator button clicks.
        /// </summary>
        /// <param name="number">Number to be entered into the calculator.</param>
        public void TranslateNumberToButtonClicks(double number)
        {
            string numberStr = number.ToString();
            if (numberStr.StartsWith("-"))
            {
                numberStr = numberStr.Remove(0, 1);
                numberStr += '-';
            }
            foreach (char digit in numberStr)
            {
                switch (digit)
                {
                    case '0':
                        Num0Button.Click();
                        break;
                    case '1':
                        Num1Button.Click();
                        break;
                    case '2':
                        Num2Button.Click();
                        break;
                    case '3':
                        Num3Button.Click();
                        break;
                    case '4':
                        Num4Button.Click();
                        break;
                    case '5':
                        Num5Button.Click();
                        break;
                    case '6':
                        Num6Button.Click();
                        break;
                    case '7':
                        Num7Button.Click();
                        break;
                    case '8':
                        Num8Button.Click();
                        break;
                    case '9':
                        Num9Button.Click();
                        break;
                    case '.':
                        DecimalButton.Click();
                        break;
                    case '-':
                        NegateButton.Click();
                        break;
                }
            }
        }
    }
}
