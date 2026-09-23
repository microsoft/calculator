// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium.Windows;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// UI elements and helper methods to perform common mathematical standard operations.
    /// </summary>
    public class StandardOperatorsPanel
    {
        private WindowsDriver<WindowsElement> session => CalculatorDriver.Instance.CalculatorSession;
        public NumberPad NumberPad = new NumberPad();

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
        public WindowsElement NegateButton => session.TryFindElementByAccessibilityId("negateButton");
    }
}
