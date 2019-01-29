// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Etw.Managed;
using Calculator.UIAutomationLibrary.Components;
using WEX.TestExecution;

namespace Calculator.UIAutomationLibrary.Tests
{
    public static class BasicCalculationTest
    {
        /// <summary>
        /// This test uses LFMs to add and then delete an alarm.
        /// </summary>
        /// <param name="calculatorAppLfm">The LFM for the alarms app window.</param>
        public static void CalculateOnePlusTwo(this CalculatorAppLfm calculatorAppLfm)
        {
            var mainPage = calculatorAppLfm.MainPageLfm;

            var standardCalculator = mainPage.NavigateToStandardCalculator();

            standardCalculator.OnePlusTwoEnter();

            Verify.AreEqual("\u202D3\u202C", standardCalculator.ObjectModel.Display.Name, "Ensure display value is 3");

            standardCalculator.Clear();

            Verify.AreEqual("\u202D0\u202C", standardCalculator.ObjectModel.Display.Name, "Ensure display value is 0");
        }
    }
}
