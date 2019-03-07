// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Linq;
using Etw.Managed;
using Microsoft.Windows.Apps.Performance;
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using Calculator.UIAutomationLibrary;

namespace Calculator.PerfTests
{
    [TestClass]
    public class AppLifecycleTests
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

        /// <summary>
        /// This method executes the AppLifecycle performance test.
        /// The test launches the application, suspend and resumes it and then terminates the app.
        /// 
        /// A TAEF data source is used to specify the Windows Performance Recorder profile and
        /// regions of interest file to use and defines three different configurations to execute this test with.
        /// Cold: The initial run to get dlls loaded into memory and to execute any first-run app logic.
        /// Warm: The run that will produce consistent results and that would be used for measures.
        /// Memory: A run that is focused on collecting more info on allocations.
        /// </summary>
        [TestMethod]
        [TestProperty("RunAs", "User")]
        [TestProperty("Category", "Performance")]
        [TestProperty(WinperfConstants.DataSource, PerfConstants.AppLifecycleIterationsSource)]
        public void AppLifecycleTest()
        {
            AppLifecycle.Run(Constants.PackageAppUserModelId);
        }
    }
}
