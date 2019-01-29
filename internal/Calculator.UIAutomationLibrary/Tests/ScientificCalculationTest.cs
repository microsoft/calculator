// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Calculator.UIAutomationLibrary.Components;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WEX.TestExecution;

namespace Calculator.UIAutomationLibrary.Tests
{
    public static class ScientificCalculationTest
    {
        /// <summary>
        /// This test uses LFMs to calculate the sqrt(4) - 2.
        /// </summary>
        /// <param name="calculatorAppLfm">The LFM for the calculator app window.</param>
        public static void CalculateSqrt4Minus2(this CalculatorAppLfm calculatorAppLfm)
        {
            var mainPage = calculatorAppLfm.MainPageLfm;

            var scientificCalculator = mainPage.NavigateToScientificCalculator();

            scientificCalculator.Press4();
            scientificCalculator.PressSqrt();
            scientificCalculator.PressMinus();
            scientificCalculator.Press3();
            scientificCalculator.PressPlus();
            scientificCalculator.Press1();
            scientificCalculator.PressEquals();

            Verify.AreEqual("\u202D0\u202C", scientificCalculator.ObjectModel.Display.Name);
        }
    }
}

