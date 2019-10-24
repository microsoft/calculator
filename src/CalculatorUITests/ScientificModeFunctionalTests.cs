// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorUITestFramework;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;

namespace CalculatorUITests
{
    [TestClass]
    public class ScientificModeFunctionalTests
    {
        private static ScientificCalculatorPage page = new ScientificCalculatorPage();

        /// <summary>
        /// Initializes the WinAppDriver web driver session.
        /// </summary>
        /// <param name="context"></param>
        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            // Create session to launch a Calculator window
            WinAppDriver.Instance.SetupCalculatorSession(context);

            // Ensure that calculator is in standard mode
            page.NavigateToCalculatorMode(CalculatorMode.ScientificCalculator);
        }

        /// <summary>
        /// Closes the app and WinAppDriver web driver session.
        /// </summary>
        [ClassCleanup]
        public static void ClassCleanup()
        {
            // Tear down Calculator session.
            WinAppDriver.Instance.TearDownCalculatorSession();
        }

        /// <summary>
        /// Ensures the calculator is in a cleared state
        /// </summary>
        [TestInitialize]
        public void TestInit()
        {
            if ("0" != page.GetCalculatorResultText())
            {
                page.ClearAll();
            }
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.ClearAll();
        }
        #region Smoke Tests
        [TestMethod]
        public void SmokeTest_Add()
        {
            page.ScientificOperators.NumberPad.Input(5.5);
            page.ScientificOperators.PlusButton.Click();
            page.ScientificOperators.NumberPad.Input(0.75);
            page.ScientificOperators.EqualButton.Click();
            Assert.AreEqual("6.25", page.GetCalculatorResultText());
        }
        [TestMethod]
        public void SmokeTest_Trig()
        {
            page.ScientificOperators.NumberPad.Input(90);

            page.ScientificOperators.SinButton.Click();
            page.ScientificOperators.EqualButton.Click();
            Assert.AreEqual("1", page.GetCalculatorResultText());
        }
        #endregion
    }
}
