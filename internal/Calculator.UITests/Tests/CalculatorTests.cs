// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Calculator.UIAutomationLibrary;
using Calculator.UIAutomationLibrary.Tests;
using WEX.TestExecution.Markup;

namespace Calculator.UITests
{
    [TestClass]
    public class CalculatorTests
    {
        [TestInitialize]
        [TestProperty("RunAs", "ElevatedUserOrSystem")]
        public void MethodSetup()
        {
            Utilities.KillExistingCalculatorProcesses();
        }

        [TestCleanup]
        [TestProperty("RunAs", "ElevatedUserOrSystem")]
        public void MethodCleanup()
        {
            Utilities.KillExistingCalculatorProcesses();
        }

        [TestMethod]
        [TestProperty("RunAs", "User")]
        public void OnePlusTwoTest()
        {
            var calculatorLfm = CalculatorAppLauncher.Launch();

            calculatorLfm.CalculateOnePlusTwo();

            calculatorLfm.Close();
        }

        [TestMethod]
        [TestProperty("RunAs", "User")]
        public void Sqrt4Minus2Test()
        {
            var calculatorLfm = CalculatorAppLauncher.Launch();

            calculatorLfm.CalculateSqrt4Minus2();

            calculatorLfm.Close();
        }
    }
}
