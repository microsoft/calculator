// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Waiters;
using System;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using System.Runtime.InteropServices;
using Microsoft.OneCoreUap.Test.AppModel;
using System.IO;
using Calculator.UITests;
using Calculator.UIAutomationLibrary;

namespace Calculator.UITests
{
    [TestClass]
    public class LaunchTests
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
        public void NormalLaunchTest()
        {
            var calculatorLfm = CalculatorAppLauncher.Launch();
            calculatorLfm.Close();
        }
    }
}
