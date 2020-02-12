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
            CalculatorApp.EnsureCalculatorHasFocus();
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
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ScientificCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Scientific", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ProgrammerCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Programmer", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.DateCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Date Calculation", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Currency);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Currency", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Volume);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Volume", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Length);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Length", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Length);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Length", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Weight);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Weight and Mass", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Weight);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Weight and Mass", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Temperature);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Temperature", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Energy);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Energy", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Area);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Area", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Speed);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Speed", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Time);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Time", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Power);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Power", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Data);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Data", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Pressure);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Pressure", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Angle);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Angle", CalculatorApp.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            CalculatorApp.EnsureCalculatorHasFocus();
            Assert.AreEqual("Standard", CalculatorApp.GetCalculatorHeaderText());
        }

        #endregion
    }
}
