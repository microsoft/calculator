using OpenQA.Selenium.Appium.Windows;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// UI elements and helper methods to perform common mathematical standard operations.
    /// </summary>
    public class StandardOperatorsPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public NumerPad NumberPad = new NumerPad();

        public WindowsElement PercentButton => session.TryFindElementByAccessibilityId("percentButton");
        public WindowsElement SquareRootButton => session.TryFindElementByAccessibilityId("squareRootButton");
        public WindowsElement XPower2Button => session.TryFindElementByAccessibilityId("xpower2Button");
        public WindowsElement XPower3Button => session.TryFindElementByAccessibilityId("xpower3Button");
        public WindowsElement InvertButton => session.TryFindElementByAccessibilityId("invertButton");
        public WindowsElement DivideButton => session.TryFindElementByAccessibilityId("divideButton");
        public WindowsElement MultiplyButton => session.TryFindElementByAccessibilityId("multiplyButton");
        public WindowsElement MinusButton => session.TryFindElementByAccessibilityId("minusButton");
        public WindowsElement PlusButton => session.TryFindElementByAccessibilityId("plusButton");
        public WindowsElement EqualButton => session.TryFindElementByAccessibilityId("equalButton");
        public WindowsElement ClearEntryButton => session.TryFindElementByAccessibilityId("clearEntryButton");
        public WindowsElement ClearButton => session.TryFindElementByAccessibilityId("clearButton");
        public WindowsElement BackSpaceButton => session.TryFindElementByAccessibilityId("backSpaceButton");

        /// <summary>
        /// Uses the Calculator UI to add a list of numbers.
        /// </summary>
        /// <param name="numbers">List of numbers to be added.</param>
        public void Add(List<double> numbers)
        {
            for (int i = 0; i < numbers.Count; i++)
            {
                NumberPad.TranslateNumberToButtonClicks(numbers[i]);

                if (i != numbers.Count - 1)
                {
                    PlusButton.Click();
                }
                else
                {
                    EqualButton.Click();
                }
            }
        }

        /// <summary>
        /// Uses the Calculator UI to subtract a list of numbers.
        /// </summary>
        /// <param name="numbers">List of numbers to be subtracted.</param>
        public void Subtract(List<double> numbers)
        {
            for (int i = 0; i < numbers.Count; i++)
            {
                NumberPad.TranslateNumberToButtonClicks(numbers[i]);

                if (i != numbers.Count - 1)
                {
                    MinusButton.Click();
                }
                else
                {
                    EqualButton.Click();
                }
            }
        }

        /// <summary>
        /// Uses the Calculator UI to multiply a list of numbers.
        /// </summary>
        /// <param name="numbers">List of numbers to be multiplied.</param>
        public void Multiply(List<double> numbers)
        {
            for (int i = 0; i < numbers.Count; i++)
            {
                NumberPad.TranslateNumberToButtonClicks(numbers[i]);

                if (i != numbers.Count - 1)
                {
                    MultiplyButton.Click();
                }
                else
                {
                    EqualButton.Click();
                }
            }
        }

        /// <summary>
        /// Uses the Calculator UI to divide a list of numbers.
        /// </summary>
        /// <param name="numbers">List of numbers to be divided.</param>
        public void Divide(List<double> numbers)
        {
            for (int i = 0; i < numbers.Count; i++)
            {
                NumberPad.TranslateNumberToButtonClicks(numbers[i]);

                if (i != numbers.Count - 1)
                {
                    DivideButton.Click();
                }
                else
                {
                    EqualButton.Click();
                }
            }
        }

        /// <summary>
        /// Uses the Calculator UI to take the reciprocal of a number.
        /// </summary>
        /// <param name="number">Number to take the reciprocal of.</param>
        public void Reciprocal(double number)
        {
                NumberPad.TranslateNumberToButtonClicks(number);
                InvertButton.Click();
        }

        /// <summary>
        /// Uses the Calculator UI to square a number.
        /// </summary>
        /// <param name="number">The number to be squared.</param>
        public void Square(double number)
        {
            NumberPad.TranslateNumberToButtonClicks(number);
            XPower2Button.Click();
        }

        /// <summary>
        /// Uses the Calculator UI to cube a number.
        /// </summary>
        /// <param name="number">The number to be cubed.</param>
        public void Cube(double number)
        {
            NumberPad.TranslateNumberToButtonClicks(number);
            XPower3Button.Click();
        }

        /// <summary>
        /// Uses the Calculator UI to take the square root of a number.
        /// </summary>
        /// <param name="number">Number to take the square root of.</param>
        public void SquareRoot(double number)
        {
            NumberPad.TranslateNumberToButtonClicks(number);
            SquareRootButton.Click();
        }
    }
}
