using System;
using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    public class NumerPad
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public WindowsElement Num0Button => session.TryFindElementByAccessibilityId("num0Button");
        public WindowsElement Num1Button => session.TryFindElementByAccessibilityId("num1Button");
        public WindowsElement Num2Button => session.TryFindElementByAccessibilityId("num2Button");
        public WindowsElement Num3Button => session.TryFindElementByAccessibilityId("num3Button");
        public WindowsElement Num4Button => session.TryFindElementByAccessibilityId("num4Button");
        public WindowsElement Num5Button => session.TryFindElementByAccessibilityId("num5Button");
        public WindowsElement Num6Button => session.TryFindElementByAccessibilityId("num6Button");
        public WindowsElement Num7Button => session.TryFindElementByAccessibilityId("num7Button");
        public WindowsElement Num8Button => session.TryFindElementByAccessibilityId("num8Button");
        public WindowsElement Num9Button => session.TryFindElementByAccessibilityId("num9Button");
        public WindowsElement DecimalButton => session.TryFindElementByAccessibilityId("decimalSeparatorButton");
        public WindowsElement NegateButton => session.TryFindElementByAccessibilityId("negateButton");

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
