// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorUITestFramework;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorUITests
{
    [TestClass]
    public class CalculatorAppStressTests
    {
        private static StandardCalculatorPage page = new StandardCalculatorPage();

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
            page.NavigateToStandardCalculator();

            // Ensure that calculator window is large enough to display the memory/history panel; a good size for most tests
            page.MemoryPanel.ResizeWindowToDiplayMemoryLabel();
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
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.EnsureCalculatorIsInStandardMode();
            page.EnsureCalculatorResultTextIsZero();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.EnsureCalculatorIsInStandardMode();
            page.ClearAll();
        }

        #region Stress Tests
        /// <summary>
        /// These automated tests mildly stress that app to verify stability
        /// To-Do: Verify multiple sessions
        /// </summary>
        [TestMethod]
        [Priority(3)]
        public void Stress_SwitchBetweenStandardAndOtherTypes()
        {
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ScientificCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Scientific", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ProgrammerCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Programmer", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.DateCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Date Calculation", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Currency);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Currency", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Volume);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Volume", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Length);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Length", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Length);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Length", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Weight);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Weight and Mass", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Weight);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Weight and Mass", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Temperature);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Temperature", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Energy);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Energy", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Area);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Area", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Speed);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Speed", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Time);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Time", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Power);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Power", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Data);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Data", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Pressure);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Pressure", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Angle);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Angle", page.CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
        }

        #endregion
    }
}
