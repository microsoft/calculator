// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using OpenQA.Selenium;
using OpenQA.Selenium.Appium.Windows;

using System;

namespace CalculatorUITestFramework
{
    public enum AngleOperatorState
    {
        Degrees,
        Radians,
        Gradians
    }

    public enum FEButtonState
    {
        Normal,
        Exponential
    }

    /// <summary>
    /// UI elements unique to Scientific mode
    /// </summary>
    public class ScientificOperatorsPanel
    {
        private WindowsDriver<WindowsElement> session => CalculatorDriver.Instance.CalculatorSession;
        public WindowsElement XPower3Button => session.TryFindElementByAccessibilityId("xpower3Button");
        public WindowsElement XPowerYButton => session.TryFindElementByAccessibilityId("powerButton");
        public WindowsElement PowerOf10Button => session.TryFindElementByAccessibilityId("powerOf10Button");
        public WindowsElement LogButton => session.TryFindElementByAccessibilityId("logBase10Button");
        public WindowsElement LnButton => session.TryFindElementByAccessibilityId("logBaseEButton");
        public WindowsElement PiButton => session.TryFindElementByAccessibilityId("piButton");
        public WindowsElement EulerButton => session.TryFindElementByAccessibilityId("eulerButton");
        public WindowsElement AbsButton => session.TryFindElementByAccessibilityId("absButton");
        public WindowsElement ExpButton => session.TryFindElementByAccessibilityId("expButton");
        public WindowsElement ModButton => session.TryFindElementByAccessibilityId("modButton");
        public WindowsElement ParenthesisLeftButton => session.TryFindElementByAccessibilityId("openParenthesisButton");
        public WindowsElement ParenthesisRightButton => session.TryFindElementByAccessibilityId("closeParenthesisButton");
        public WindowsElement FactorialButton => session.TryFindElementByAccessibilityId("factorialButton");
        public WindowsElement BackSpaceButton => session.TryFindElementByAccessibilityId("backSpaceButton");
        public WindowsElement DegButton => session.TryFindElementByAccessibilityId("degButton");
        public WindowsElement RadButton => session.TryFindElementByAccessibilityId("radButton");
        public WindowsElement GradButton => session.TryFindElementByAccessibilityId("gradButton");
        public WindowsElement AngleOperator => session.TryFindElementByAccessibilityId("ScientificAngleOperators");
        public WindowsElement TrigButton => session.TryFindElementByAccessibilityId("trigButton");
        public WindowsElement FuncButton => session.TryFindElementByAccessibilityId("funcButton");
        public WindowsElement SinButton => session.TryFindElementByAccessibilityId("sinButton");
        public WindowsElement CosButton => session.TryFindElementByAccessibilityId("cosButton");
        public WindowsElement TanButton => session.TryFindElementByAccessibilityId("tanButton");
        public WindowsElement CscButton => session.TryFindElementByAccessibilityId("cscButton");
        public WindowsElement SecButton => session.TryFindElementByAccessibilityId("secButton");
        public WindowsElement CotButton => session.TryFindElementByAccessibilityId("cotButton");
        public WindowsElement TrigShiftButton => session.TryFindElementByAccessibilityId("trigShiftButton");
        public WindowsElement HypShiftButton => session.TryFindElementByAccessibilityId("hypShiftButton");
        public WindowsElement InvSinButton => session.TryFindElementByAccessibilityId("invsinButton");
        public WindowsElement InvCosButton => session.TryFindElementByAccessibilityId("invcosButton");
        public WindowsElement InvTanButton => session.TryFindElementByAccessibilityId("invtanButton");
        public WindowsElement InvCscButton => session.TryFindElementByAccessibilityId("invcscButton");
        public WindowsElement InvSecButton => session.TryFindElementByAccessibilityId("invsecButton");
        public WindowsElement InvCotButton => session.TryFindElementByAccessibilityId("invcotButton");
        public WindowsElement SinhButton => session.TryFindElementByAccessibilityId("sinhButton");
        public WindowsElement CoshButton => session.TryFindElementByAccessibilityId("coshButton");
        public WindowsElement TanhButton => session.TryFindElementByAccessibilityId("tanhButton");
        public WindowsElement CschButton => session.TryFindElementByAccessibilityId("cschButton");
        public WindowsElement SechButton => session.TryFindElementByAccessibilityId("sechButton");
        public WindowsElement CothButton => session.TryFindElementByAccessibilityId("cothButton");
        public WindowsElement InvSinhButton => session.TryFindElementByAccessibilityId("invsinhButton");
        public WindowsElement InvCoshButton => session.TryFindElementByAccessibilityId("invcoshButton");
        public WindowsElement InvTanhButton => session.TryFindElementByAccessibilityId("invtanhButton");
        public WindowsElement InvCschButton => session.TryFindElementByAccessibilityId("invcschButton");
        public WindowsElement InvSechButton => session.TryFindElementByAccessibilityId("invsechButton");
        public WindowsElement InvCothButton => session.TryFindElementByAccessibilityId("invcothButton");
        public WindowsElement FloorButton => session.TryFindElementByAccessibilityId("floorButton");
        public WindowsElement CeilButton => session.TryFindElementByAccessibilityId("ceilButton");
        public WindowsElement RandButton => session.TryFindElementByAccessibilityId("randButton");
        public WindowsElement DmsButton => session.TryFindElementByAccessibilityId("dmsButton");
        public WindowsElement DegreesButton => session.TryFindElementByAccessibilityId("degreesButton");
        public WindowsElement FixedToExponentialButton => session.TryFindElementByAccessibilityId("ftoeButton");
        public WindowsElement NegateButton => session.TryFindElementByAccessibilityId("negateButton");
        public WindowsElement ShiftButton => session.TryFindElementByAccessibilityId("shiftButton");
        public WindowsElement TrigFlyout => session.TryFindElementByAccessibilityId("Trigflyout");
        public WindowsElement LightDismiss => session.TryFindElementByAccessibilityId("Light Dismiss");
        private WindowsElement DegRadGradButton => GetAngleOperatorButton();
        private WindowsElement GetAngleOperatorButton()
        {
            string source = session.PageSource;
            if (source.Contains("degButton"))
            {
                return DegButton;
            }
            else if (source.Contains("gradButton"))
            {
                return GradButton;
            }
            else if (source.Contains("radButton"))
            {
                return RadButton;
            }

            throw new NotFoundException("Could not find deg, rad or grad button in page source");
        }

        /// <summary>
        /// Set the state of the degrees, radians and gradians buttons.
        /// </summary>
        public void SetAngleOperator(AngleOperatorState value)
        {
            //set the desired string value for the button
            string desiredId = value switch
            {
                AngleOperatorState.Degrees => "degButton",
                AngleOperatorState.Gradians => "gradButton",
                AngleOperatorState.Radians => "radButton",
                _ => throw new NotImplementedException()
            };
            while (DegRadGradButton.GetAttribute("AutomationId") != desiredId)
            {
                DegRadGradButton.Click();
            }
        }

        public void ResetFEButton(FEButtonState value)
        {
            if (FixedToExponentialButton.GetAttribute("Toggle.ToggleState") != "0")
            {
                FixedToExponentialButton.Click();
            }
        }

        public WindowsElement ResetTrigDropdownToggles()
        {
            TrigButton.Click();
            string source = session.PageSource;
            if (source.Contains("sinButton"))
            {
                LightDismiss.Click();
            }
            else if (source.Contains("invsinButton"))
            {
                TrigShiftButton.Click();
            }
            else if (source.Contains("sinhButton"))
            {
                HypShiftButton.Click();
            }
            else if (source.Contains("invsinhButton"))
            {
                TrigShiftButton.Click();
                HypShiftButton.Click();
            }
            throw new NotFoundException("Could not find trig drop-down buttons in page source");
        }
    }
}
