// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium.Appium.Windows;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices.ComTypes;

namespace CalculatorUITestFramework
{
    /// <summary>
    /// UI elements and helper methods to perform common mathematical standard operations.
    /// </summary>
    public class ScientificOperatorsPanel
    {
        private WindowsDriver<WindowsElement> session => WinAppDriver.Instance.CalculatorSession;
        public NumberPad NumberPad = new NumberPad();

        public WindowsElement XPower2Button => this.session.TryFindElementByAccessibilityId("xpower2Button");
        public WindowsElement XPower3Button => this.session.TryFindElementByAccessibilityId("xpower3Button");
        public WindowsElement XPowerYButton => this.session.TryFindElementByAccessibilityId("xpowerYButton");
        public WindowsElement PowerXButton => this.session.TryFindElementByAccessibilityId("powerXButton");
        public WindowsElement LogButton => this.session.TryFindElementByAccessibilityId("logBase10Button");
        public WindowsElement LnButton => this.session.TryFindElementByAccessibilityId("logBaseEButton");
        public WindowsElement PiButton => this.session.TryFindElementByAccessibilityId("piButton");
        public WindowsElement InvertButton => this.session.TryFindElementByAccessibilityId("invertButton");
        public WindowsElement EButton => this.session.TryFindElementByAccessibilityId("eButton");
        public WindowsElement AbsButton => this.session.TryFindElementByAccessibilityId("absButton");
        public WindowsElement ExpButton => this.session.TryFindElementByAccessibilityId("expButton");
        public WindowsElement ModButton => this.session.TryFindElementByAccessibilityId("modButton");
        public WindowsElement ParenthesesLeftButton => this.session.TryFindElementByAccessibilityId("parenthesesLeftButton");
        public WindowsElement ParenthesesRightButton => this.session.TryFindElementByAccessibilityId("parenthesesRightButton");
        public WindowsElement FactorialButton => this.session.TryFindElementByAccessibilityId("factorialButton");
        public WindowsElement DivideButton => this.session.TryFindElementByAccessibilityId("divideButton");
        public WindowsElement MultiplyButton => this.session.TryFindElementByAccessibilityId("multiplyButton");
        public WindowsElement MinusButton => this.session.TryFindElementByAccessibilityId("minusButton");
        public WindowsElement PlusButton => this.session.TryFindElementByAccessibilityId("plusButton");
        public WindowsElement EqualButton => this.session.TryFindElementByAccessibilityId("equalButton");
        public WindowsElement ClearEntryButton => this.session.TryFindElementByAccessibilityId("clearEntryButton");
        public WindowsElement ClearButton => this.session.TryFindElementByAccessibilityId("clearButton");
        public WindowsElement BackSpaceButton => this.session.TryFindElementByAccessibilityId("backSpaceButton");
        public WindowsElement DegButton => this.session.TryFindElementByAccessibilityId("degButton");
        public WindowsElement RadButton => this.session.TryFindElementByAccessibilityId("radButton");
        public WindowsElement GradButton => this.session.TryFindElementByAccessibilityId("gradButton");
        public WindowsElement AngleOperator => this.session.TryFindElementByAccessibilityId("ScientificAngleOperators");
        public WindowsElement TrigButton => this.session.TryFindElementByAccessibilityId("trigButton");
        public WindowsElement FuncButton => this.session.TryFindElementByAccessibilityId("funcButton");
        public WindowsElement SinButton => this.session.TryFindElementByAccessibilityId("sinButton");
        public WindowsElement CosButton => this.session.TryFindElementByAccessibilityId("cosButton");
        public WindowsElement TanButton => this.session.TryFindElementByAccessibilityId("tanButton");
        public WindowsElement CscButton => this.session.TryFindElementByAccessibilityId("cscButton");
        public WindowsElement SecButton => this.session.TryFindElementByAccessibilityId("secButton");
        public WindowsElement CotButton => this.session.TryFindElementByAccessibilityId("cotButton");
        public WindowsElement TrigShiftButton => this.session.TryFindElementByAccessibilityId("trigShiftButton");
        public WindowsElement HypShiftButton => this.session.TryFindElementByAccessibilityId("hypShiftButton");
        public WindowsElement InvSinButton => this.session.TryFindElementByAccessibilityId("invsinButton");
        public WindowsElement InvCosButton => this.session.TryFindElementByAccessibilityId("invcosButton");
        public WindowsElement InvTanButton => this.session.TryFindElementByAccessibilityId("invtanButton");
        public WindowsElement InvCscButton => this.session.TryFindElementByAccessibilityId("invcscButton");
        public WindowsElement InvSecButton => this.session.TryFindElementByAccessibilityId("invsecButton");
        public WindowsElement InvCotButton => this.session.TryFindElementByAccessibilityId("invcotButton");
        public WindowsElement SinhButton => this.session.TryFindElementByAccessibilityId("sinhButton");
        public WindowsElement CoshButton => this.session.TryFindElementByAccessibilityId("coshButton");
        public WindowsElement TanhButton => this.session.TryFindElementByAccessibilityId("cothButton");
        public WindowsElement CschButton => this.session.TryFindElementByAccessibilityId("cschButton");
        public WindowsElement SechButton => this.session.TryFindElementByAccessibilityId("sechButton");
        public WindowsElement CothButton => this.session.TryFindElementByAccessibilityId("cothButton");
        public WindowsElement InvSinhButton => this.session.TryFindElementByAccessibilityId("invsinhButton");
        public WindowsElement InvCoshButton => this.session.TryFindElementByAccessibilityId("invcoshButton");
        public WindowsElement InvTanhButton => this.session.TryFindElementByAccessibilityId("invtanhButton");
        public WindowsElement InvCschButton => this.session.TryFindElementByAccessibilityId("invcschButton");
        public WindowsElement InvSechButton => this.session.TryFindElementByAccessibilityId("invsechButton");
        public WindowsElement InvCothButton => this.session.TryFindElementByAccessibilityId("invcothButton");
    }
}
