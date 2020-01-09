// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorUITestFramework;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using OpenQA.Selenium.Interactions;
using System;
using System.Drawing;

namespace CalculatorUITests
{
    [TestClass]
    public class StandardModeFunctionalTests
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
            if ("0" != page.CalculatorResults.GetCalculatorResultText())
            {
                page.ClearAll();
            }
            page.CalculatorApp.EnsureCalculatorHasFocus();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.ClearAll();
        }

        #region Basic UI Functionality via Mouse Input Tests

        /// <summary>
        /// These automated tests verify clicking each of the buttons in the Calculator UI and getting an expected result
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>

        [TestMethod]
        [Priority(0)]
        public void MouseInput_AddSubtractClearClearEntryTwoThree()
        {
            //Verifies the +, -, CE, C, 2, and 3 button
            page.StandardOperators.NumberPad.Num2Button.Click();
            Assert.AreEqual("2", page.CalculatorResults.GetCalculatorResultText()); //verifies 2 button
            page.StandardOperators.PlusButton.Click();
            Assert.AreEqual("2 +", page.CalculatorResults.GetCalculatorExpressionText()); //verifies + button
            page.StandardOperators.NumberPad.Num2Button.Click();
            page.StandardOperators.MinusButton.Click();
            Assert.AreEqual("4", page.CalculatorResults.GetCalculatorResultText()); //verifies addition
            Assert.AreEqual("2 + 2 Minus (", page.CalculatorResults.GetCalculatorExpressionText()); //verifies - button
            page.StandardOperators.NumberPad.Num3Button.Click();
            Assert.AreEqual("3", page.CalculatorResults.GetCalculatorResultText()); //verifies 3 button
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText()); //verifies subtraction
            page.StandardOperators.ClearEntryButton.Click();
            Assert.AreEqual("0", page.CalculatorResults.GetCalculatorResultText()); //verifies the CE button
            page.StandardOperators.ClearButton.Click();
            page.CalculatorResults.CheckIfResultsExpressionIsClear(); //verifies the C button
        }

        [TestMethod]
        [Priority(0)]
        public void MouseInput_MultiplyDivideEqualFourFiveSix()
        {
            //Verifies the multiplication, and division, and the x, ÷, equal, 4, 5, and 6 button
            page.StandardOperators.NumberPad.Num4Button.Click();
            Assert.AreEqual("4", page.CalculatorResults.GetCalculatorResultText()); //verifies 4 button
            page.StandardOperators.MultiplyButton.Click();
            Assert.AreEqual("4 ×", page.CalculatorResults.GetCalculatorExpressionText()); //verifies x button
            page.StandardOperators.NumberPad.Num5Button.Click();
            Assert.AreEqual("5", page.CalculatorResults.GetCalculatorResultText()); //verifies 5 button
            page.StandardOperators.DivideButton.Click();
            Assert.AreEqual("20", page.CalculatorResults.GetCalculatorResultText()); //verifies multiplication
            Assert.AreEqual("4 × 5 ÷", page.CalculatorResults.GetCalculatorExpressionText()); //verifies ÷ button
            page.StandardOperators.NumberPad.Num6Button.Click();
            Assert.AreEqual("6", page.CalculatorResults.GetCalculatorResultText()); //verifies 6 button
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("3.333333333333333", page.CalculatorResults.GetCalculatorResultText()); //verifies division
            Assert.AreEqual("4 × 5 ÷ 6=", page.CalculatorResults.GetCalculatorExpressionText()); //verifies = button
        }

        [TestMethod]
        [Priority(0)]
        public void MouseInput_InvertSquaredSevenEightNine()
        {
            //Verifies the invert, squared, 7, 8, and 9 button
            page.StandardOperators.NumberPad.Num7Button.Click();
            Assert.AreEqual("7", page.CalculatorResults.GetCalculatorResultText()); //verifies 7 button
            page.StandardOperators.InvertButton.Click();
            Assert.AreEqual("0.1428571428571429", page.CalculatorResults.GetCalculatorResultText()); //verifies inversion
            Assert.AreEqual("1/(7)", page.CalculatorResults.GetCalculatorExpressionText()); //verifies the invert button
            page.StandardOperators.ClearButton.Click();
            page.StandardOperators.NumberPad.Num8Button.Click();
            Assert.AreEqual("8", page.CalculatorResults.GetCalculatorResultText()); //verifies 8 button
            page.StandardOperators.ClearButton.Click();
            page.StandardOperators.NumberPad.Num9Button.Click();
            Assert.AreEqual("9", page.CalculatorResults.GetCalculatorResultText()); //verifies 9 button
            page.StandardOperators.XPower2Button.Click();
            Assert.AreEqual("81", page.CalculatorResults.GetCalculatorResultText()); //verifies squaring
            Assert.AreEqual("square (9)", page.CalculatorResults.GetCalculatorExpressionText()); //verifies squared button
        }

        [TestMethod]
        [Priority(0)]
        public void MouseInput_PercentSquareRootBackspaceDecimalNegateOneZero()
        {
            //Verifies the %, square root, backspace, decimal, negate, 1, and 0 button
            page.StandardOperators.NumberPad.Num1Button.Click();
            page.StandardOperators.NumberPad.Num0Button.Click();
            page.StandardOperators.NumberPad.Num0Button.Click();
            page.StandardOperators.BackSpaceButton.Click();
            Assert.AreEqual("10", page.CalculatorResults.GetCalculatorResultText()); // verifies the 1 button, the 0 button, and the backspace button
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.PercentButton.Click();
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText()); //verifies percent calculation
            Assert.AreEqual("10 + 1", page.CalculatorResults.GetCalculatorExpressionText()); //verifies percent button
            page.StandardOperators.PercentButton.Click();
            page.StandardOperators.SquareRootButton.Click();
            Assert.AreEqual("0.3162277660168379", page.CalculatorResults.GetCalculatorResultText()); //verifies square root calculation
            Assert.AreEqual("10 + √(0.1)", page.CalculatorResults.GetCalculatorExpressionText()); //verifies 2√x button
            page.StandardOperators.NumberPad.DecimalButton.Click();
            Assert.AreEqual("0 point", page.CalculatorResults.GetCalculatorResultText()); //verifies decimal button
            page.StandardOperators.NumberPad.NegateButton.Click();
            page.StandardOperators.NumberPad.Num1Button.Click();
            Assert.AreEqual("-0.1", page.CalculatorResults.GetCalculatorResultText()); //verifies negate button
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("9.9", page.CalculatorResults.GetCalculatorResultText()); //verifies calculation with decimal point and negative number
        }

        [TestMethod]
        [Priority(0)]
        public void MouseInput_HistoryButtons()
        {
            //Verifies history buttons
            page.StandardOperators.NumberPad.Num4Button.Click();
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Num5Button.Click();
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Num6Button.Click();
            page.StandardOperators.EqualButton.Click();
            page.HistoryPanel.ResizeWindowToDisplayHistoryButton();
            page.HistoryPanel.HistoryButton.Click();
            var historyFlyoutItems = page.HistoryPanel.GetAllHistoryFlyoutListViewItems();
            Assert.IsTrue(historyFlyoutItems[0].Text.Equals("4 × 5 ÷ 6= 3.333333333333333", StringComparison.InvariantCultureIgnoreCase)); //verifies History button
            page.HistoryPanel.ResizeWindowToDisplayHistoryLabel();
            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("4 × 5 ÷ 6= 3.333333333333333", StringComparison.InvariantCultureIgnoreCase));
            page.HistoryPanel.ClearHistoryButton.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty")); //verifies the History panel's clear history button
        }

        [TestMethod]
        [Priority(0)]
        public void MouseInput_MemoryButtons()
        {
            //Verifies memory buttons
            page.StandardOperators.NumberPad.Num1Button.Click();
            page.MemoryPanel.MemButton.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("1", StringComparison.InvariantCultureIgnoreCase)); //verifies memory button
            page.MemoryPanel.MemPlus.Click();
            Assert.IsTrue(memoryItems[0].Text.Equals("2", StringComparison.InvariantCultureIgnoreCase)); //verifies memory plus button
            page.MemoryPanel.MemRecall.Click();
            Assert.AreEqual("2", page.CalculatorResults.GetCalculatorResultText()); //verifies memory recall button
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Num1Button.Click();
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemMinus.Click();
            Assert.IsTrue(memoryItems[0].Text.Equals("1", StringComparison.InvariantCultureIgnoreCase));
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText()); //verifies MemMinus button
            page.MemoryPanel.MemoryClear.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty")); //verifies the Memory panel's memory clear button
        }
        #endregion

        #region Basic Calculator Functionality via Keyboard Input Tests

        /// <summary>
        /// These automated tests verify the functionality of all hotkeys, that are common for all calculator modes, as well as the all keyboard input used in standard mode on the Standard Calculator.
        /// Via keyboard input, all basic UI functionality is checked.
        /// To-do:
        /// - Active issue "Bug 23811901: Clicking on the History Label causes the [Shift] + [Ctrl] + [D] hotkeys to break" causes KeyboardInput_HistoryHotkeys() to fail.
        /// It is currently commented out until the active bug is fixed.
        /// </summary>

        [TestMethod]
        [Priority(1)]
        public void KeyboardInput_HotkeysToChangeToBasicModes()
        {
            // Verify Hotkeys for changing modes
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "2" + Keys.Alt);
            Assert.AreEqual("Scientific", page.CalculatorApp.GetCalculatorHeaderText()); //verifies Scientific navigation hotkey
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "4" + Keys.Alt);
            Assert.AreEqual("Programmer", page.CalculatorApp.GetCalculatorHeaderText()); //verifies Programmer navigation hotkey
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "5" + Keys.Alt);
            Assert.AreEqual("Date Calculation", page.CalculatorApp.GetCalculatorHeaderText()); //verifies Date Calculation navigation hotkey
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "1" + Keys.Alt);
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText()); //verifies Standard navigation hotkey
        }

        [TestMethod]
        [Priority(1)]
        public void KeyboardInput_AddSubtractClearClearEntryTwoThree()
        {
            //Verifies the +, -, CE, C, 2, and 3 button
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("2");
            Assert.AreEqual("2", page.CalculatorResults.GetCalculatorResultText()); //verifies using 2 key
            page.CalculatorApp.Header.SendKeys(Keys.Add);
            Assert.AreEqual("2 +", page.CalculatorResults.GetCalculatorExpressionText()); //verifies using add key
            page.CalculatorApp.Header.SendKeys("2");
            page.CalculatorApp.Header.SendKeys(Keys.Subtract);
            Assert.AreEqual("4", page.CalculatorResults.GetCalculatorResultText()); //verifies addition
            Assert.AreEqual("2 + 2 Minus (", page.CalculatorResults.GetCalculatorExpressionText()); //verifies using subtraction key
            page.CalculatorApp.Header.SendKeys("3");
            Assert.AreEqual("3", page.CalculatorResults.GetCalculatorResultText()); //verifies using 3 key
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText()); //verifies subtraction
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Delete);
            Assert.AreEqual("0", page.CalculatorResults.GetCalculatorResultText()); //verifies the CE hotkey
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
            page.CalculatorResults.CheckIfResultsExpressionIsClear(); //verifies the C hotkey
        }

        [TestMethod]
        [Priority(1)]
        public void KeyboardInput_MultiplyDivideEqualFourFiveSix()
        {
            //Verifies the multiplication, division, and equal, 4, 5, and 6 key input
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("4");
            Assert.AreEqual("4", page.CalculatorResults.GetCalculatorResultText()); //verifies using 4 key
            page.CalculatorApp.Header.SendKeys(Keys.Multiply);
            Assert.AreEqual("4 ×", page.CalculatorResults.GetCalculatorExpressionText()); //verifies using multiplication key
            page.CalculatorApp.Header.SendKeys("5");
            Assert.AreEqual("5", page.CalculatorResults.GetCalculatorResultText()); //verifies using 5 key
            page.CalculatorApp.Header.SendKeys(Keys.Divide);
            Assert.AreEqual("20", page.CalculatorResults.GetCalculatorResultText()); //verifies multiplication
            Assert.AreEqual("4 × 5 ÷", page.CalculatorResults.GetCalculatorExpressionText()); //verifies using divide key
            page.CalculatorApp.Header.SendKeys("6");
            Assert.AreEqual("6", page.CalculatorResults.GetCalculatorResultText()); //verifies using 6 key
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("3.333333333333333", page.CalculatorResults.GetCalculatorResultText()); //verifies division
            Assert.AreEqual("4 × 5 ÷ 6=", page.CalculatorResults.GetCalculatorExpressionText()); //verifies equal key
        }

        [TestMethod]
        [Priority(1)]
        public void KeyboardInput_InvertSquaredSevenEightNine()
        {
            //Verifies the invert, squared, 7, 8, and 9 key input
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("7");
            Assert.AreEqual("7", page.CalculatorResults.GetCalculatorResultText()); //verifies using 7 key
            page.CalculatorApp.Header.SendKeys("r");
            Assert.AreEqual("0.1428571428571429", page.CalculatorResults.GetCalculatorResultText()); //verifies inversion
            Assert.AreEqual("1/(7)", page.CalculatorResults.GetCalculatorExpressionText()); //verifies the invert hotkey
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
            page.CalculatorApp.Header.SendKeys("8");
            Assert.AreEqual("8", page.CalculatorResults.GetCalculatorResultText()); //verifies using 8 key
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
            page.CalculatorApp.Header.SendKeys("9");
            Assert.AreEqual("9", page.CalculatorResults.GetCalculatorResultText()); //verifies using 9 key
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("q");
            Assert.AreEqual("81", page.CalculatorResults.GetCalculatorResultText()); //verifies squaring
            Assert.AreEqual("square (9)", page.CalculatorResults.GetCalculatorExpressionText()); //verifies squared hotkey
        }

        [TestMethod]
        [Priority(1)]
        public void KeyboardInput_PercentSquareRootBackspaceDecimalNegateOneZero()
        {
            //Verifies the %, square root, backspace, decimal, negate, 1, and 0 button
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("100");
            page.CalculatorApp.Header.SendKeys(Keys.Backspace);
            Assert.AreEqual("10", page.CalculatorResults.GetCalculatorResultText()); // verifies using the 1 key, the 0 key, and the backspace key
            page.CalculatorApp.Header.SendKeys(Keys.Add);
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Shift + "5" + Keys.Shift);
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText()); //verifies percent calculation
            Assert.AreEqual("10 + 1", page.CalculatorResults.GetCalculatorExpressionText()); //verifies percent hotkey
            page.CalculatorApp.Header.SendKeys(Keys.Shift);
            page.CalculatorApp.Header.SendKeys(Keys.Shift + "5" + Keys.Shift);
            page.CalculatorApp.Header.SendKeys(Keys.Shift);
            page.CalculatorApp.Header.SendKeys(Keys.Shift + "2" + Keys.Shift);
            Assert.AreEqual("0.3162277660168379", page.CalculatorResults.GetCalculatorResultText()); //verifies square root calculation
            Assert.AreEqual("10 + √(0.1)", page.CalculatorResults.GetCalculatorExpressionText()); //verifies 2√x hotkey
            page.CalculatorApp.Header.SendKeys(".");
            Assert.AreEqual("0 point", page.CalculatorResults.GetCalculatorResultText()); //verifies using decimal key
            page.CalculatorApp.Header.SendKeys(Keys.F9);
            page.CalculatorApp.Header.SendKeys("1");
            Assert.AreEqual("-0.1", page.CalculatorResults.GetCalculatorResultText()); //verifies negate hotkey
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("9.9", page.CalculatorResults.GetCalculatorResultText()); //verifies calculation with decimal point and negative number
        }

        [TestMethod]
        [Priority(1)]
        public void KeyboardInput_HistoryHotkeys()
        {
            ////Verifies history hotkeys
            /// To-do: Commenting this section out until following active issue in calculator is fixed
            /// - Active issue "Bug 23811901: Clicking on the History Label causes the [Shift] + [Ctrl] + [D] hotkeys to break" causes this case to fail

            ////Verifies history buttons
            //page.CalculatorApp.EnsureCalculatorHasFocus();
            //page.CalculatorApp.Header.SendKeys("4");
            //page.CalculatorApp.Header.SendKeys(Keys.Multiply);
            //page.CalculatorApp.Header.SendKeys("5");
            //page.CalculatorApp.Header.SendKeys(Keys.Divide);
            //page.CalculatorApp.Header.SendKeys("6");
            //page.CalculatorApp.Header.SendKeys(Keys.Equal);
            //page.HistoryPanel.ResizeWindowToDisplayHistoryButton();
            //page.CalculatorApp.Header.SendKeys(Keys.Control + "h" + Keys.Control);
            //var historyFlyoutItems = page.HistoryPanel.GetAllHistoryFlyoutListViewItems();
            //Assert.IsTrue(historyFlyoutItems[0].Text.Equals("4 × 5 ÷ 6= 3.333333333333333", StringComparison.InvariantCultureIgnoreCase)); //verifies History button hotkeys
            //page.HistoryPanel.ResizeWindowToDisplayHistoryLabel();
            //var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            //Assert.IsTrue(historyItems[0].Text.Equals("4 × 5 ÷ 6= 3.333333333333333", StringComparison.InvariantCultureIgnoreCase));
            //page.CalculatorApp.Header.SendKeys(Keys.Shift + Keys.Control + "h" + Keys.Control + Keys.Shift);
            //Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty")); //verifies the History panel's clear history button hotkeys
        }

        [TestMethod]
        [Priority(1)]
        public void KeyboardInput_MemoryHotkeys()
        {
            //Verifies memory buttons
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("1");
            page.CalculatorApp.Header.SendKeys(Keys.Control + "m" + Keys.Control);
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("1", StringComparison.InvariantCultureIgnoreCase)); //verifies memory hotkey
            page.CalculatorApp.Header.SendKeys(Keys.Control + "p" + Keys.Control);
            Assert.IsTrue(memoryItems[0].Text.Equals("2", StringComparison.InvariantCultureIgnoreCase)); //verifies memory plus hotkey
            page.CalculatorApp.Header.SendKeys(Keys.Control + "r" + Keys.Control);
            Assert.AreEqual("2", page.CalculatorResults.GetCalculatorResultText()); //verifies memory recall hotkey
            page.CalculatorApp.Header.SendKeys(Keys.Subtract);
            page.CalculatorApp.Header.SendKeys("1");
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            page.CalculatorApp.Header.SendKeys(Keys.Subtract);
            page.CalculatorApp.Header.SendKeys(Keys.Control + "q" + Keys.Control);
            Assert.IsTrue(memoryItems[0].Text.Equals("1", StringComparison.InvariantCultureIgnoreCase));
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText()); //verifies MemMinus hotkey
            page.CalculatorApp.Header.SendKeys(Keys.Control + "l" + Keys.Control);
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty")); //verifies the Memory panel's memory clear button hotkey
        }

        #endregion

        #region Arithmetic Operators Functionality via Mixed Input

        /// <summary>
        /// These automated test not only verify each opperator individually, but they also simultaneously verify mixed user input.
        /// Arithmetic Operators for standard calculator are also automated in: 
        /// - Region: Basic UI Functionality via Mouse input
        /// - Region: Basic Calculator Functionality via Keyboard input
        /// </summary>

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Addition()
        {

            //Verify Addition
            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.PlusButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("2");
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("4", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Subtraction()
        {
            //Verify Subtraction
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.MinusButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("2");
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Multiplication()
        {
            //Verify Multiplication
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.MultiplyButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("2");
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("6", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Division()
        {
            //Verify Division
            page.StandardOperators.NumberPad.Input(6);
            page.StandardOperators.MultiplyButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("2");
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("12", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Reciprocal()
        {
            //Verify Reciprocal
            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.InvertButton.Click();
            Assert.AreEqual("0.5", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);

        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Square()
        {
            //Verify Square
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.XPower2Button.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("9", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_SquareRoot()
        {
            //Verify Square root
            page.StandardOperators.NumberPad.Input(9);
            page.StandardOperators.XPower2Button.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("81", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_PercentAdditionSubtraction()
        {
            //Verify Percent addition/subtraction
            page.StandardOperators.NumberPad.Input(10);
            page.StandardOperators.MinusButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("10");
            page.StandardOperators.PercentButton.Click();
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("9", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_PercentMultiplicationDivision()
        {
            //Verify Percent multiplication/division
            page.StandardOperators.NumberPad.Input(10);
            page.StandardOperators.MultiplyButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("10");
            page.StandardOperators.PercentButton.Click();
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_PositiveNegative()
        {
            //Verify Positive/Negative (plus/minus)
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.MinusButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("2");
            page.StandardOperators.NegateButton.Click();
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("5", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Decimal()
        {
            //Verify Decimal
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.NumberPad.DecimalButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys("2");
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("3.2", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Equal()
        {
            //Verify Equal
            page.HistoryPanel.ClearHistory();
            page.StandardOperators.EqualButton.Click();
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("0", page.CalculatorResults.GetCalculatorResultText());
            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("0= 0", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("0= 0", StringComparison.InvariantCultureIgnoreCase));
            page.CalculatorApp.Header.SendKeys(Keys.Escape);
        }

        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_Delete()
        {
            //Verify Delete
            page.StandardOperators.NumberPad.Input(3);
            page.CalculatorApp.Header.SendKeys("3");
            Assert.AreEqual("33", page.CalculatorResults.GetCalculatorResultText());
            page.StandardOperators.BackSpaceButton.Click();
            Assert.AreEqual("3", page.CalculatorResults.GetCalculatorResultText());
            page.CalculatorApp.Header.SendKeys(Keys.Backspace);
            Assert.AreEqual("0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void MixedInput_Operators_ClearEntryClear()
        {
            //Verify Clear Entery
            page.StandardOperators.NumberPad.Input(3);
            page.CalculatorApp.Header.SendKeys(Keys.Add);
            page.CalculatorApp.Header.SendKeys("3");
            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Delete);
            Assert.AreEqual("0", page.CalculatorResults.GetCalculatorResultText());
            Assert.AreEqual("3 +", page.CalculatorResults.GetCalculatorExpressionText());

            page.StandardOperators.NumberPad.Input(9);
            page.CalculatorApp.Header.SendKeys(Keys.Subtract);
            page.CalculatorApp.Header.SendKeys("6");
            page.StandardOperators.ClearEntryButton.Click();
            Assert.AreEqual("0", page.CalculatorResults.GetCalculatorResultText());
            Assert.AreEqual("3 + 9 Minus (", page.CalculatorResults.GetCalculatorExpressionText());

            //Verify Clear
            page.StandardOperators.NumberPad.Input(6);
            page.CalculatorApp.Header.SendKeys(Keys.Subtract);
            page.CalculatorApp.Header.SendKeys("9");
            page.StandardOperators.ClearButton.Click();
            page.CalculatorResults.CheckIfResultsExpressionIsClear();
        }

        // Issue #817: Prefixed multiple zeros
        [TestMethod]
        [Priority(2)]
        public void MixedInput_Operators_DeletingDecimalDoesNotPrefixZeros()
        {
            page.StandardOperators.NumberPad.DecimalButton.Click(); // To enter decimal point
            page.CalculatorApp.Header.SendKeys("1");
            page.StandardOperators.BackSpaceButton.Click();
            page.StandardOperators.BackSpaceButton.Click();
            page.StandardOperators.NumberPad.Input(0);
            page.StandardOperators.NumberPad.Num0Button.Click();
            page.StandardOperators.NumberPad.Input(0);
            Assert.AreEqual("0", page.CalculatorResults.GetCalculatorResultText());
        }

        #endregion

        #region Standard Memory Tests

        /// <summary>
        /// These automated tests verify using memory related buttons in the Memory Panel, and in the memory flyout
        /// Memory control buttons are verified at the end of automated tested in MouseInput_MemoryButtons()
        /// </summary>

        [TestMethod]
        [Priority(2)]
        public void StandardMemory_Panel()
        {

            //Verify hover buttons MemMinusItem, MemPlusItem, and ClearMemoryItemButton, and verify the clear memory button in the Memory panel
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.MemoryPanel.MemButton.Click();
            page.MemoryPanel.MemButton.Click();
            page.MemoryPanel.OpenMemoryPanel();
            Actions moveToListView = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToListView.MoveToElement(page.MemoryPanel.ListViewItem);
            moveToListView.Perform();
            Actions clickSubtractFromMemoryItem = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickSubtractFromMemoryItem.MoveByOffset(125, 10).Click();
            clickSubtractFromMemoryItem.Perform();
            var memoryItems0 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems0[0].Text.Equals("0", StringComparison.InvariantCultureIgnoreCase));
            moveToListView.Perform();
            Actions clickAddToMemoryItem = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickAddToMemoryItem.MoveByOffset(100, 10).Click();
            clickAddToMemoryItem.Perform();
            Assert.IsTrue(memoryItems0[0].Text.Equals("3", StringComparison.InvariantCultureIgnoreCase));
            moveToListView.Perform();
            Actions clickClearMemoryItem = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickClearMemoryItem.MoveByOffset(50, 10).Click();
            clickClearMemoryItem.Perform();
            var memoryItems2 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems2[0].Text.Equals("3", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.ClearMemory.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        [TestMethod]
        [Priority(2)]
        public void StandardMemory_Flyout()
        {
            //Verify hover buttons MemMinusItem, MemPlusItem, and ClearMemoryItemButton, and verify the clear memory button in the dropdown Memory flyout
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.MemoryPanel.MemButton.Click();
            page.MemoryPanel.MemButton.Click();
            page.MemoryPanel.OpenMemoryFlyout();
            Actions moveToListView2 = new Actions(WinAppDriver.Instance.CalculatorSession);
            moveToListView2.MoveToElement(page.MemoryPanel.ListViewItem);
            moveToListView2.Perform();
            Actions clickSubtractFromMemoryItem2 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickSubtractFromMemoryItem2.MoveByOffset(190, 10).Click();
            clickSubtractFromMemoryItem2.Perform();
            var memoryItems = page.MemoryPanel.GetAllMemoryFlyoutListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("0", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.OpenMemoryFlyout();
            moveToListView2.Perform();
            Actions clickAddToMemoryItem2 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickAddToMemoryItem2.MoveByOffset(150, 10).Click();
            clickAddToMemoryItem2.Perform();
            Assert.IsTrue(memoryItems[0].Text.Equals("3", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.OpenMemoryFlyout();
            moveToListView2.Perform();
            Actions clickClearMemoryItem2 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickClearMemoryItem2.MoveByOffset(115, 10).Click();
            clickClearMemoryItem2.Perform();
            Assert.IsTrue(memoryItems[1].Text.Equals("3", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.OpenMemoryFlyout();
            page.MemoryPanel.ClearMemory.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
            page.MemoryPanel.ResizeWindowToDiplayMemoryLabel();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        #endregion

        #region Standard History Tests

        /// <summary>
        /// These automated tests (that replace this manual test case) verify using
        /// history related button/keyboard actions in the History Panel,
        /// and in the History flyout. The History button is verified in the automated test MouseInput_HistoryButtons().
        /// To-Do: Currently, an empty history panel on initial launch cannot be verified through automation.
        /// The automation cannot see the Windows element "HistoryEmpty."  This should be automated once the issue is resolved.
        /// </summary>

        [TestMethod]
        [Priority(2)]
        public void StandardHistory_Panel()
        {

            //Verify selecting history items form the history panel, and clearing the history panel
            page.HistoryPanel.OpenHistoryPanel();

            page.StandardOperators.NumberPad.Input(-3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(-2.6);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("2 Minus ( 3= Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("-3 + -2.6= Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            Actions clickHistoryItemsw1 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickHistoryItemsw1.Click(historyItems[1]);
            clickHistoryItemsw1.Perform();

            Assert.AreEqual("-5.6", page.CalculatorResults.GetCalculatorResultText());
            Assert.AreEqual("-3 + -2.6=", page.CalculatorResults.GetCalculatorExpressionText());

            Actions clickHistoryItemsw0 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickHistoryItemsw0.Click(historyItems[0]);
            clickHistoryItemsw0.Perform();

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            page.HistoryPanel.ClearHistory();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
        }

        [TestMethod]
        [Priority(2)]
        public void StandardHistory_Flyout()
        {

            //Verify selecting history items form the history flyout, using up/down arrows and [enter] to select history items,
            // and clearing the history flyout.  Additionally, verify history panel and history flyout have same information.

            page.HistoryPanel.OpenHistoryPanel();

            page.StandardOperators.NumberPad.Input(-3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(-2.6);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            var historyPanelItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyPanelItems[0].Text.Equals("2 Minus ( 3= Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyPanelItems[1].Text.Equals("-3 + -2.6= Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            var historyItems = page.HistoryPanel.GetAllHistoryFlyoutListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("2 Minus ( 3= Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("-3 + -2.6= Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            page.HistoryPanel.ListViewItem.SendKeys(Keys.ArrowDown + Keys.Enter);

            Assert.AreEqual("-5.6", page.CalculatorResults.GetCalculatorResultText());
            Assert.AreEqual("-3 + -2.6=", page.CalculatorResults.GetCalculatorExpressionText());

            page.HistoryPanel.OpenHistoryFlyout();
            page.HistoryPanel.ListViewItem.SendKeys(Keys.ArrowDown + Keys.ArrowUp + Keys.Enter);
            Assert.AreEqual("-1", page.CalculatorResults.GetCalculatorResultText());

            page.HistoryPanel.OpenHistoryFlyout();
            page.HistoryPanel.ClearHistoryButton.Click();

            page.HistoryPanel.OpenHistoryFlyout();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));

            page.HistoryPanel.OpenHistoryPanel();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
        }

        #endregion

        #region Error Tests
        /// <summary>
        /// These automated tests verify errors display gracefully
        /// Via mouse input, all basic UI functionality is checked 
        /// </summary>

        [TestMethod]
        [Priority(1)]
        public void Error_DivideByZero()
        {
            page.StandardOperators.InvertButton.Click();
            Assert.IsTrue(page.CalculatorResults.GetCalculatorResultText() == "Cannot divide by zero");
        }

        [TestMethod]
        [Priority(1)]
        public void Error_CubedNegativeNumber()
        {
            page.StandardOperators.NumberPad.Input(9);
            page.StandardOperators.NegateButton.Click();
            page.StandardOperators.SquareRootButton.Click();
            Assert.AreEqual("Invalid input", page.CalculatorResults.GetCalculatorResultText());
        }
        #endregion

        #region AoT (Always on top) Tests
        /// <summary>
        /// These automated tests verify:
        /// - Calculator can enter AoT mode, the AoT window is within a range of size, and it verifies exiting AoT mode
        /// - AoT button tooltips (in English)
        /// - Memory function cannot be triggered in AoT calculator
        /// - The history flyout cannot be opened in AoT mode and that history is tracked while in AoT mode and available upon exit of AoT mode
        /// - That Standard calculator has AoT button, but other major calculator modes do not
        /// - That the app can scale to smallest size, largest size, and to a medium size without crashing.  It does not test the visual appears of the UI while scaling. 
        /// - The window scale retention is maintained when switching between AoT and Non-AoT 
        ///     - To-do: Verify scale retention across calculator session
        /// - Error messaging while AoT mode
        ///     -  To-do: Need to verify "All operator buttons (except for "CE", "C", "Delete" and "="), and memory buttons are disabled" when in AoT mode
        /// - Verify app positioning after entering and exiting KOT mode
        /// </summary>

        [TestMethod]
        [Priority(0)]
        public void AoT_EnterExitKeepOnTop()
        {
            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            page.StandardAoTCalculatorPage.AoTWindowSizeWithinRange();
            page.StandardAoTCalculatorPage.AoTWindowPositionWithinRange();
            page.StandardAoTCalculatorPage.NavigateToStandardMode();

        }

        [TestMethod]
        [Priority(0)]
        public void AoT_Tootip()
        {
            Assert.AreEqual("Keep on top", page.StandardAoTCalculatorPage.GetAoTToolTipText());
            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            Assert.AreEqual("Back to full view", page.StandardAoTCalculatorPage.GetAoTToolTipText());
        }

        [TestMethod]
        [Priority(1)]
        public void AoT_NoMemoryFunction()
        {
            page.StandardOperators.NumberPad.Num9Button.Click();
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            page.CalculatorApp.Window.SendKeys(Keys.Enter);
            page.CalculatorApp.Window.SendKeys(Keys.Control + "P" + Keys.Control);
            page.CalculatorApp.Window.SendKeys(Keys.Shift); //Hotkeys such as "Delete" will break without this line
            page.StandardAoTCalculatorPage.NavigateToStandardMode();
            page.MemoryPanel.OpenMemoryPanel();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        [TestMethod]
        [Priority(1)]
        public void AoT_HistoryFunction()
        {
            page.HistoryPanel.ClearHistory();
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            page.CalculatorApp.Window.SendKeys(Keys.Enter);
            page.CalculatorApp.Window.SendKeys(Keys.Control + "H" + Keys.Control);
            string source = WinAppDriver.Instance.CalculatorSession.PageSource;
            if (source.Contains("HistoryFlyout"))
            {
                throw new NotFoundException("This test fails; history flyout is present");
            }
            else
            {
                page.StandardAoTCalculatorPage.NavigateToStandardMode();
                var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
                Assert.IsTrue(historyItems[0].Text.Equals("3 + 3= 6", StringComparison.InvariantCultureIgnoreCase));
            }
        }

        [TestMethod]
        [Priority(2)]
        public void AoT_ButtonOnlyInStandard()
        {

            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "2" + Keys.Alt);
            Assert.AreEqual("Scientific", page.CalculatorApp.GetCalculatorHeaderText());
            page.StandardAoTCalculatorPage.GetAoTPresence();
            Assert.AreEqual("False", page.StandardAoTCalculatorPage.GetAoTPresence());

            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "4" + Keys.Alt);
            Assert.AreEqual("Programmer", page.CalculatorApp.GetCalculatorHeaderText());
            page.StandardAoTCalculatorPage.GetAoTPresence();
            Assert.AreEqual("False", page.StandardAoTCalculatorPage.GetAoTPresence());

            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "5" + Keys.Alt);
            Assert.AreEqual("Date Calculation", page.CalculatorApp.GetCalculatorHeaderText());
            page.StandardAoTCalculatorPage.GetAoTPresence();
            Assert.AreEqual("False", page.StandardAoTCalculatorPage.GetAoTPresence());

            page.CalculatorApp.EnsureCalculatorHasFocus();
            page.CalculatorApp.Header.SendKeys(Keys.Alt + "1" + Keys.Alt);
            Assert.AreEqual("Standard", page.CalculatorApp.GetCalculatorHeaderText());
            page.StandardAoTCalculatorPage.GetAoTPresence();
            Assert.AreEqual("True", page.StandardAoTCalculatorPage.GetAoTPresence());
        }

        [TestMethod]
        [Priority(2)]
        public void AoT_Scaling()
        {
            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            page.StandardAoTCalculatorPage.AoTWindowSizeWithinRange();
            Size smallAoTWindowSize = new Size(161, 168);
            Size largeAoTWindowSize = new Size(502, 502);
            Size mediumAoTWindowSize = new Size(396, 322);
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = smallAoTWindowSize;
            Assert.AreEqual("(161, 168)", page.CalculatorApp.GetCalculatorWindowSize());
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = largeAoTWindowSize;
            Assert.AreEqual("(502, 502)", page.CalculatorApp.GetCalculatorWindowSize());
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = mediumAoTWindowSize;
            Assert.AreEqual("(396, 322)", page.CalculatorApp.GetCalculatorWindowSize());
            page.StandardAoTCalculatorPage.NavigateToStandardMode();
        }

        [TestMethod]
        [Priority(2)]
        public void AoT_ScaleRetention()
        {
            Size smallWindowSize = new Size(464, 502);
            Size largeAoTWindowSize = new Size(502, 502);

            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = smallWindowSize;
            Assert.AreEqual("(464, 502)", page.CalculatorApp.GetCalculatorWindowSize());

            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = largeAoTWindowSize;
            Assert.AreEqual("(502, 502)", page.CalculatorApp.GetCalculatorWindowSize());

            page.StandardAoTCalculatorPage.NavigateToStandardMode();
            Assert.AreEqual("(464, 502)", page.CalculatorApp.GetCalculatorWindowSize());

            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            Assert.AreEqual("(502, 502)", page.CalculatorApp.GetCalculatorWindowSize());

        }

        [TestMethod]
        [Priority(2)]
        public void AoT_ErrorMessage()
        {
            page.StandardAoTCalculatorPage.NavigateToStandardAoTMode();
            page.StandardAoTCalculatorPage.ResizeAoTWindowToDiplayInvertButton();
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Num0Button.Click();
            page.StandardOperators.EqualButton.Click();
            page.StandardAoTCalculatorPage.AoTModeCheck();
            Assert.AreEqual("True", page.StandardAoTCalculatorPage.AoTModeCheck());
            Assert.AreEqual("Result is undefined", page.CalculatorResults.GetAoTCalculatorResultText());
            page.StandardOperators.ClearButton.Click();
            page.StandardOperators.InvertButton.Click();
            page.StandardAoTCalculatorPage.AoTModeCheck();
            Assert.AreEqual("True", page.StandardAoTCalculatorPage.AoTModeCheck());
            Assert.AreEqual("Cannot divide by zero", page.CalculatorResults.GetAoTCalculatorResultText());
            page.StandardAoTCalculatorPage.NavigateToStandardMode();
            page.StandardAoTCalculatorPage.AoTModeCheck();
            Assert.AreEqual("False", page.StandardAoTCalculatorPage.AoTModeCheck());
            Assert.AreEqual("Cannot divide by zero", page.CalculatorResults.GetCalculatorResultText());
        }

        #endregion

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
