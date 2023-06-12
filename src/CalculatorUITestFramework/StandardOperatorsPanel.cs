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

        public WindowsElement PercentButton => this.session.TryFindElementByAccessibilityId("percentButton");
        public WindowsElement SquareRootButton => this.session.TryFindElementByAccessibilityId("squareRootButton");
        public WindowsElement XPower2Button => this.session.TryFindElementByAccessibilityId("xpower2Button");
        public WindowsElement XPower3Button => this.session.TryFindElementByAccessibilityId("xpower3Button");
        public WindowsElement InvertButton => this.session.TryFindElementByAccessibilityId("invertButton");
        public WindowsElement DivideButton => this.session.TryFindElementByAccessibilityId("divideButton");
        public WindowsElement MultiplyButton => this.session.TryFindElementByAccessibilityId("multiplyButton");
        public WindowsElement MinusButton => this.session.TryFindElementByAccessibilityId("minusButton");
        public WindowsElement PlusButton => this.session.TryFindElementByAccessibilityId("plusButton");
        public WindowsElement EqualButton => this.session.TryFindElementByAccessibilityId("equalButton");
        public WindowsElement ClearEntryButton => this.session.TryFindElementByAccessibilityId("clearEntryButton");
        public WindowsElement ClearButton => this.session.TryFindElementByAccessibilityId("clearButton");
        public WindowsElement BackSpaceButton => this.session.TryFindElementByAccessibilityId("backSpaceButton");
        public WindowsElement NegateButton => this.session.TryFindElementByAccessibilityId("negateButton");
    }
}
