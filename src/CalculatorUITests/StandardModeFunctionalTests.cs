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
            if ("0" != page.GlobalCalculatorUI.GetCalculatorResultText())
            {
                page.ClearAll();
            }
            page.GlobalCalculatorUI.AppName.Click(); //This click is to ensure app focus
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.StandardOperators.EqualButton.Click();
            page.ClearAll();
        }

        #region Smoke Tests

        [TestMethod]
        public void SmokeTest_Add()
        {
            page.StandardOperators.NumberPad.Input(3.5);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(0.25);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("3.75", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_Add_KeyboardInput()
        {
            page.GlobalCalculatorUI.Header.SendKeys("3+3=");
            Assert.AreEqual("6", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_Subtract()
        {
            page.StandardOperators.NumberPad.Input(4.3);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(2.6);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1.7", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void SmokeTest_History()
        {
            page.StandardOperators.NumberPad.Input(-3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(-2.6);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("1 × 3= 3", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("2 Minus ( 3= Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[2].Text.Equals("-3 + -2.6= Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

        }

        [TestMethod]
        public void SmokeTest_Memory()
        {
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(0);
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemButton.Click();

            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemButton.Click();

            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Input(9);
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemButton.Click();

            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();

            Assert.IsTrue(memoryItems[0].Text.Equals("63", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(memoryItems[1].Text.Equals("0.6666666666666667‬", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(memoryItems[2].Text.Equals("3", StringComparison.InvariantCultureIgnoreCase));
            return;

        }

        #endregion

        #region Basic Arthimetic Tests
        [TestMethod]
        public void Operator_Reciprocal()
        {
            page.StandardOperators.NumberPad.Input(4);
            page.StandardOperators.InvertButton.Click();
            Assert.AreEqual("0.25", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Squared()
        {
            page.StandardOperators.NumberPad.Input(-15.5);
            page.StandardOperators.XPower2Button.Click();
            Assert.AreEqual("240.25", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_SquareRoot()
        {
            page.StandardOperators.NumberPad.Input(144);
            page.StandardOperators.SquareRootButton.Click();
            Assert.AreEqual("12", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Percent()
        {
            page.StandardOperators.NumberPad.Input(600);
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Input(15);
            page.StandardOperators.PercentButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("90", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_Delete()
        {
            page.StandardOperators.NumberPad.Input(-12345);
            page.StandardOperators.BackSpaceButton.Click();
            Assert.AreEqual("-1,234", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_ClearAll()
        {
            page.StandardOperators.NumberPad.Input(12345);
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Input(6789);
            page.StandardOperators.ClearButton.Click();
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void Operator_ClearEntry()
        {
            page.StandardOperators.NumberPad.Input(-12345);
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Input(5678);
            page.StandardOperators.ClearEntryButton.Click();
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());
        }
        #endregion

        #region Keyboard Input Tests
        [TestMethod]
        public void KeyboardInput_Subtract()
        {
            page.GlobalCalculatorUI.Header.SendKeys("70090");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Subtract);
            page.GlobalCalculatorUI.Header.SendKeys("890987");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("-820,897", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Multiply()
        {
            page.GlobalCalculatorUI.Header.SendKeys("-15*3=");
            page.GlobalCalculatorUI.CalculatorResult.Click();
            Assert.AreEqual("-45", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Divide()
        {
            page.GlobalCalculatorUI.Header.SendKeys("26/13=");
            Assert.AreEqual("2", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_SquareRoot()
        {
            page.GlobalCalculatorUI.Header.SendKeys("100@");
            Assert.AreEqual("10", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        [TestMethod]
        public void KeyboardInput_Percent()
        {
            page.GlobalCalculatorUI.Header.SendKeys("34+7%=");
            Assert.AreEqual("36.38", page.GlobalCalculatorUI.GetCalculatorResultText());
        }

        #endregion

        #region Memory Tests
        [TestMethod]
        public void Memory_AddTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(15);
            page.MemoryPanel.MemPlus.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("19", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_SubtractTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(3.3);
            page.MemoryPanel.MemMinus.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("0.7", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_RecallTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(3.3);
            page.MemoryPanel.MemRecall.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("4", StringComparison.InvariantCultureIgnoreCase));
        }

        [TestMethod]
        public void Memory_ClearTest()
        {
            page.StandardOperators.NumberPad.Input(12);
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.EqualButton.Click();

            page.MemoryPanel.MemButton.Click();
            page.StandardOperators.NumberPad.Input(3.3);
            page.MemoryPanel.MemoryClear.Click();
            page.MemoryPanel.OpenMemoryPanel();
            Assert.IsNotNull(page.MemoryPanel.MemoryPaneEmptyLabel);
        }
        #endregion

        #region Error Tests
        [TestMethod]
        [Priority(1)]
        public void Error_DivideByZero()
        {
            page.StandardOperators.InvertButton.Click();
            Assert.IsTrue(page.GlobalCalculatorUI.GetCalculatorResultText() == "Cannot divide by zero");
        }

        [TestMethod]
        [Priority(1)]
        public void Error_CubedNegativeNumber()
        {
            page.StandardOperators.NumberPad.Input(9);
            page.StandardOperators.NegateButton.Click();
            page.StandardOperators.SquareRootButton.Click();
            Assert.IsTrue(page.GlobalCalculatorUI.GetCalculatorResultText() == "Invalid input");
        }
        #endregion

        #region KOT(Keep on top) Tests
        [TestMethod]
        [Priority(0)]
        public void KOT_EnterExitKeepOnTop()
        {
            /// <summary>
            /// Test Case 23456961: BVT: KOT: Verify entering and exiting KOT mode
            /// This automated test verifies the caculator can enter KOT mode and that the window within KOT size range
            /// and it verifies exiting KOT mode
            /// </summary>
            page.StandardKoTMode();
            page.KOTWindowSizeWithinRange();
            page.StandardNonKoTMode();

        }

        [TestMethod]
        [Priority(0)]
        public void KOT_Tootip()
        {
            /// <summary>
            /// Test Case 23456329: BVT: KOT: Verify tooltip
            /// This automated test verifies english KOT button tooltips
            /// </summary>
            Assert.AreEqual("Keep on top", page.GetKOTToolTipText());
            page.StandardKoTMode();
            Assert.AreEqual("Back to full view", page.GetKOTToolTipText());
        }

        [TestMethod]
        [Priority(1)]
        public void KOT_NoMemoryFunction()
        {
            /// <summary>
            /// Test Case 23458498: BVT: KOT: Verify Memory function is not accessible while in KOT mode
            /// This automated test verifies memory function cannot be triggered in KOT calculator
            /// </summary>
            ///
            page.StandardOperators.NumberPad.Num9Button.Click();
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.StandardKoTMode();
            page.GlobalCalculatorUI.Window.SendKeys(Keys.Enter);
            page.GlobalCalculatorUI.Window.SendKeys(Keys.Control + "P" + Keys.Control);
            page.StandardNonKoTMode();
            page.MemoryPanel.OpenMemoryPanel();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        [TestMethod]
        [Priority(1)]
        public void KOT_HistoryFunction()
        {
            /// <summary>
            /// Test Case 23458436: BVT: KOT: Verify History is not accessible while in KOT mode, but it is still tracked
            /// This automated test verifies the history flyout cannot be opened in KOT mode and
            /// that history is tracked while in KOT mode and avalible upon ext of KOT mode
            /// </summary>
            page.HistoryPanel.ClearHistory();
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.StandardKoTMode();
            page.GlobalCalculatorUI.Window.SendKeys(Keys.Enter);
            page.GlobalCalculatorUI.Window.SendKeys(Keys.Control + "H" + Keys.Control);
            string sourceI = WinAppDriver.Instance.CalculatorSession.PageSource;
            if (sourceI.Contains("HistoryFlyout"))
            {
                throw new NotFoundException("This test fails; history flyout is present");
            }
            else
            {
                page.StandardNonKoTMode();
                var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
                Assert.IsTrue(historyItems[0].Text.Equals("3 + 3= 6", StringComparison.InvariantCultureIgnoreCase));
            }
        }

        [TestMethod]
        [Priority(2)]
        public void KOT_ButtonOnlyInStandard()
        {
            /// <summary>
            /// Test Case 23459163: BVT: KOT: Verify KOT button is only available in Standard mode
            /// This automated test verifies that Sandard calculator has KOT button, but other major
            /// calculator modes do not
            /// </summary>

            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "2" + Keys.Alt);
            Assert.AreEqual("Scientific", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.GetKOTPresence();
            Assert.AreEqual("False", page.GetKOTPresence());

            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "3" + Keys.Alt);
            Assert.AreEqual("Programmer", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.GetKOTPresence();
            Assert.AreEqual("False", page.GetKOTPresence());

            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "4" + Keys.Alt);
            Assert.AreEqual("Date Calculation", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.GetKOTPresence();
            Assert.AreEqual("False", page.GetKOTPresence());

            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "1" + Keys.Alt);
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.GetKOTPresence();
            Assert.AreEqual("True", page.GetKOTPresence());
        }

        [TestMethod]
        [Priority(2)]
        public void KOT_Scaling()
        {
            /// <summary>
            /// Test Case 23456757: BVT: KOT: Verify UI while scaling
            /// This automated test verifies that the app can scale to smallest size, largest size, and to a medium size 
            /// without crashing
            /// </summary>
            page.StandardKoTMode();
            page.KOTWindowSizeWithinRange();
            Size smallKOTWindowSize = new Size(161, 168);
            Size largeKOTWindowSize = new Size(502, 502);
            Size mediumKOTWindowSize = new Size(396, 322);
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = smallKOTWindowSize;
            Assert.AreEqual("(161, 168)", page.GetCalculatorWindowSize());
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = largeKOTWindowSize;
            Assert.AreEqual("(502, 502)", page.GetCalculatorWindowSize());
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = mediumKOTWindowSize;
            Assert.AreEqual("(396, 322)", page.GetCalculatorWindowSize());
            page.StandardNonKoTMode();
        }

        [TestMethod]
        [Priority(2)]
        public void KOT_ScaleRetention()
        {
            /// <summary>
            /// (Half) Test Case 23458956: BVT: KOT: Verify scale retention
            /// This automated test verifies the window scale retention is maintained when switching between KOT and Non-KOT
            /// Note: This does not verify scale retention across calculator session
            /// </summary>

            Size smallWindowSize = new Size(464, 502);
            Size largeKOTWindowSize = new Size(502, 502);

            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = smallWindowSize;
            Assert.AreEqual("(464, 502)", page.GetCalculatorWindowSize());

            page.StandardKoTMode();
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = largeKOTWindowSize;
            Assert.AreEqual("(502, 502)", page.GetCalculatorWindowSize());

            page.StandardNonKoTMode();
            Assert.AreEqual("(464, 502)", page.GetCalculatorWindowSize());

            page.StandardKoTMode();
            Assert.AreEqual("(502, 502)", page.GetCalculatorWindowSize());

        }

        [TestMethod]
        [Priority(2)]
        public void KOT_ErrorMessage()
        {
            /// <summary>
            /// Test Case 23459118: BVT: KOT: Verify error message display
            /// This automated test verifies the window scale retention is maintained when switching between KOT and Non-KOT
            /// Note: This test case is in progress: Need to verify " All operator buttons (except for "CE", "C", "Delete" and "="), and memory buttons are disabled"
            /// </summary>

            Size largeKOTWindowSize = new Size(502, 502);
            Point largeKOTWindowPosition = new Point(0, 0);
            page.StandardKoTMode();
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Position = largeKOTWindowPosition;
            WinAppDriver.Instance.CalculatorSession.Manage().Window.Size = largeKOTWindowSize;
            page.GlobalCalculatorUI.Window.SendKeys("/");
            page.GlobalCalculatorUI.Window.SendKeys("0");
            page.GlobalCalculatorUI.Window.SendKeys(Keys.Enter);
            page.KOTModeCheck();
            Assert.AreEqual("True", page.KOTModeCheck());
            Assert.IsTrue(page.GetKOTCalculatorResultText() == "Result is undefined");
            page.StandardOperators.ClearButton.Click();
            page.StandardOperators.InvertButton.Click();
            page.KOTModeCheck();
            Assert.AreEqual("True", page.KOTModeCheck());
            Assert.IsTrue(page.GetKOTCalculatorResultText() == "Cannot divide by zero");
            page.StandardNonKoTMode();
            page.KOTModeCheck();
            Assert.AreEqual("False", page.KOTModeCheck());
            Assert.IsTrue(page.GlobalCalculatorUI.GetCalculatorResultText() == "Cannot divide by zero");
        }

        #endregion

        #region BVT
        [TestMethod]
        [Priority(1)]
        public void BVT_Hotkeys()
        {
            /// <summary>
            /// Test Case 23466694: BVT: All: Verify hotkeys common to all modes
            /// This automated test verifies the functionality of all hotkeys, that are common for all calculator modes, for the standard calculator modes.
            /// Note:
            /// - Active issue "Bug 23811901: Clicking on the History Label causes the [Shift] + [Ctrl] + [D] hotkeys to break" causes this case to fail
            /// - Keyboard Numberpad input testing is not part of this automated test to unstability.  WinAppDrive command does not toggle Num Lock.  
            /// </summary>

            // Verify Hotkeys for changing modes
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "2" + Keys.Alt);
            Assert.AreEqual("Scientific", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "3" + Keys.Alt);
            Assert.AreEqual("Programmer", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "4" + Keys.Alt);
            Assert.AreEqual("Date Calculation", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Alt + "1" + Keys.Alt);
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());

            //Verifies the +, -, CE, C, 2, and 3 input
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Add);
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Subtract);
            page.GlobalCalculatorUI.Header.SendKeys("3");
            Assert.AreEqual("3", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Delete);
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("2 + 2 Minus (", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verifies the multiplication, division, and equal, 4, 5, and 6 key input
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("4");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Multiply);
            page.GlobalCalculatorUI.Header.SendKeys("5");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Divide);
            page.GlobalCalculatorUI.Header.SendKeys("6");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("3.333333333333333", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verifies the invert, squared, 7, 8, and 9 key input
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("7");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);
            page.GlobalCalculatorUI.Header.SendKeys("7");
            page.GlobalCalculatorUI.Header.SendKeys("r");
            Assert.AreEqual("0.1428571428571429", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("1/(7)", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);
            page.GlobalCalculatorUI.Header.SendKeys("8");
            Assert.AreEqual("8", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);
            page.StandardOperators.NumberPad.Num9Button.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("q");
            Assert.AreEqual("81", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("square (9)", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verifies the %, square root, backspace, decimal, negate, +, 1, and 0 key input
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("100");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Backspace);
            Assert.AreEqual("10", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Add);
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Shift + "5" + Keys.Shift);
            Assert.AreEqual("1", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 + 1", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Shift);
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Shift + "5" + Keys.Shift);
            Assert.AreEqual("0.1", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 + 0.1", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Shift);
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Shift + "2" + Keys.Shift);
            Assert.AreEqual("0.3162277660168379", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 + √(0.1)", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.GlobalCalculatorUI.Header.SendKeys(".");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.F9);
            Assert.AreEqual("-0 point", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 +", page.GlobalCalculatorUI.GetCalculatorExpressionText());

            ////Verifies history hotkeys
            /// Note: Commenting this section out until following active issue in calculator is fixed
            /// - Active issue "Bug 23811901: Clicking on the History Label causes the [Shift] + [Ctrl] + [D] hotkeys to break" causes this case to fail
            //page.GlobalCalculatorUI.AppName.Click();
            //var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            //Assert.IsTrue(historyItems[0].Text.Equals("4 × 5 ÷ 6= 3.333333333333333", StringComparison.InvariantCultureIgnoreCase));
            //page.GlobalCalculatorUI.Header.SendKeys(Keys.Shift + Keys.Control + "D" + Keys.Control + Keys.Shift);
            //Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
            page.StandardOperators.ClearButton.Click();

            //Verifies memory hotkeys
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("1");
            page.MemoryPanel.MemButton.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("1", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.MemPlus.Click();
            var memoryItems1 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems1[0].Text.Equals("2", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.MemRecall.Click();
            Assert.AreEqual("2", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys("1");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Subtract);
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Subtract);
            var memoryItems2 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems2[0].Text.Equals("2", StringComparison.InvariantCultureIgnoreCase));
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Control + "L" + Keys.Control);
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        [TestMethod]
        [Priority(0)]
        public void BVT_MouseInput()
        {
            /// <summary>
            /// Test Case 23624584: BVT: All: Verify mouse input
            /// This automated test verifies clicking each of the buttons and getting an expected result.
            /// </summary>

            //Verifies the +, -, CE, C, 2, and 3 button
            page.StandardOperators.NumberPad.Num2Button.Click();
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.NumberPad.Num2Button.Click();
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.NumberPad.Num3Button.Click();
            Assert.AreEqual("3", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.StandardOperators.ClearEntryButton.Click();
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("2 + 2 Minus (", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.StandardOperators.ClearButton.Click();
            //Verifies the multiplication, division, and equal, 4, 5, and 6 button
            page.StandardOperators.NumberPad.Num4Button.Click();
            page.StandardOperators.MultiplyButton.Click();
            page.StandardOperators.NumberPad.Num5Button.Click();
            page.StandardOperators.DivideButton.Click();
            page.StandardOperators.NumberPad.Num6Button.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("3.333333333333333", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.StandardOperators.ClearButton.Click();
            //Verifies the invert, squared, 7, 8, and 9 button
            page.StandardOperators.NumberPad.Num7Button.Click();
            page.StandardOperators.InvertButton.Click();
            Assert.AreEqual("0.1428571428571429", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("1/(7)", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.StandardOperators.ClearButton.Click();
            page.StandardOperators.NumberPad.Num8Button.Click();
            Assert.AreEqual("8", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.StandardOperators.ClearButton.Click();
            page.StandardOperators.NumberPad.Num9Button.Click();
            page.StandardOperators.XPower2Button.Click();
            Assert.AreEqual("81", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("square (9)", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.StandardOperators.ClearButton.Click();
            //Verifies the %, square root, backspace, decimal, negate, +, 1, and 0 button
            page.StandardOperators.NumberPad.Num1Button.Click();
            page.StandardOperators.NumberPad.Num0Button.Click();
            page.StandardOperators.NumberPad.Num0Button.Click();
            page.StandardOperators.BackSpaceButton.Click();
            Assert.AreEqual("10", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.StandardOperators.PlusButton.Click();
            page.StandardOperators.PercentButton.Click();
            Assert.AreEqual("1", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 + 1", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.StandardOperators.PercentButton.Click();
            Assert.AreEqual("0.1", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 + 0.1", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.StandardOperators.SquareRootButton.Click();
            Assert.AreEqual("0.3162277660168379", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 + √(0.1)", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            page.StandardOperators.NumberPad.DecimalButton.Click();
            page.StandardOperators.NumberPad.NegateButton.Click();
            Assert.AreEqual("-0 point", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("10 +", page.GlobalCalculatorUI.GetCalculatorExpressionText());
            //Verifies history buttons
            page.HistoryPanel.ResizeWindowToNotDiplayHistoryLabel();
            page.StandardOperators.HistoryButton.Click();
            page.HistoryPanel.ResizeWindowToDiplayHistoryLabel();
            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("4 × 5 ÷ 6= 3.333333333333333", StringComparison.InvariantCultureIgnoreCase));
            page.HistoryPanel.ClearHistoryButton.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
            page.StandardOperators.ClearButton.Click();
            //Verifies memory buttons
            page.StandardOperators.NumberPad.Num1Button.Click();
            page.MemoryPanel.MemButton.Click();
            var memoryItems = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems[0].Text.Equals("1", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.MemPlus.Click();
            var memoryItems1 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems1[0].Text.Equals("2", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.MemRecall.Click();
            Assert.AreEqual("2", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.StandardOperators.NumberPad.Num1Button.Click();
            page.StandardOperators.MinusButton.Click();
            page.StandardOperators.EqualButton.Click();
            page.MemoryPanel.MemMinus.Click();
            var memoryItems2 = page.MemoryPanel.GetAllMemoryListViewItems();
            Assert.IsTrue(memoryItems2[0].Text.Equals("2", StringComparison.InvariantCultureIgnoreCase));
            page.MemoryPanel.MemoryClear.Click();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("MemoryPaneEmpty"));
        }

        [TestMethod]
        [Priority(2)]
        public void BVT_VerifyMemoryFunction()
        {
            /// <summary>
            /// Test Case 23624587: BVT: All: Verify memory functions
            /// This automated test verifies using memory related buttons in the Memory Panel, and in the memory flyout
            /// Memory control buttons are veriifed at the end of automated test "BVT_VerifyMouseInput"
            /// </summary>

            //Verify hover buttons MemMinusItem, MemPlusItem, and ClearMemoryItemButton, and verify the clear memory button in the Memory panel
            page.StandardOperators.NumberPad.Num3Button.Click();
            page.MemoryPanel.MemButton.Click();
            page.MemoryPanel.MemButton.Click();
            page.MemoryPanel.OpenMemoryPanel();
            page.MemoryPanel.MemoryListView.WaitForDisplayed();
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

            //Verify hover buttons MemMinusItem, MemPlusItem, and ClearMemoryItemButton, and verify the clear memory button in the dropdown Memory flyout
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

        [TestMethod]
        [Priority(2)]
        public void BVT_HistoryPanel()
        {

            /// <summary>
            /// Test Case 23480026: BVT: Standard/Scientific: Verify history panel
            /// This automated test verifies using using the history panel in Standard mode
            /// Note: An empty history on initial launch cannot be verified.  The automation cannot see the Windows element "HistoryEmpty."
            /// Even though this element is there; I see it, and Inspect sees it, WinAppDriver cannot find it.
            /// The only time the automation is successful at finding this element is after creating and deleting history items.
            /// Will this part of the test later, once issue is solved.
            /// </summary>

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

            Assert.AreEqual("-1", page.GlobalCalculatorUI.GetCalculatorResultText());

            Actions clickHistoryItemsw1 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickHistoryItemsw1.Click(historyItems[1]);
            clickHistoryItemsw1.Perform();

            Assert.AreEqual("-5.6", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("-3 + -2.6=", page.GlobalCalculatorUI.GetCalculatorExpressionText());

            Actions clickHistoryItemsw0 = new Actions(WinAppDriver.Instance.CalculatorSession);
            clickHistoryItemsw0.Click(historyItems[0]);
            clickHistoryItemsw0.Perform();

            Assert.AreEqual("-1", page.GlobalCalculatorUI.GetCalculatorResultText());

            page.HistoryPanel.ClearHistory();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
        }

        [TestMethod]
        [Priority(2)]
        public void BVT_HistoryFlyout()
        {

            /// <summary>
            /// Test Case 23466739: BVT: Standard/Scientific: Verify history flyout
            /// This automated test verifies using using the history history flyout in Standard mode
            /// </summary>

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

            Assert.AreEqual("-1", page.GlobalCalculatorUI.GetCalculatorResultText());

            var historyItems = page.HistoryPanel.GetAllHistoryFlyoutListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("2 Minus ( 3= Minus (1", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("-3 + -2.6= Minus (5.6", StringComparison.InvariantCultureIgnoreCase));

            Assert.AreEqual("-1", page.GlobalCalculatorUI.GetCalculatorResultText());

            page.HistoryPanel.ListViewItem.SendKeys(Keys.ArrowDown + Keys.Enter);

            Assert.AreEqual("-5.6", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("-3 + -2.6=", page.GlobalCalculatorUI.GetCalculatorExpressionText());

            page.HistoryPanel.OpenHistoryFlyout();
            page.HistoryPanel.ListViewItem.SendKeys(Keys.ArrowDown + Keys.ArrowUp + Keys.Enter);
            Assert.AreEqual("-1", page.GlobalCalculatorUI.GetCalculatorResultText());

            page.HistoryPanel.OpenHistoryFlyout();
            page.HistoryPanel.ClearHistoryButton.Click();

            page.HistoryPanel.OpenHistoryFlyout();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));

            page.HistoryPanel.OpenHistoryPanel();
            Assert.IsNotNull(WinAppDriver.Instance.CalculatorSession.FindElementByAccessibilityId("HistoryEmpty"));
        }

        [TestMethod]
        [Priority(2)]
        public void BVT_ArithmeticOperators()
        {
            /// <summary>
            /// Arithmetic Operators for standard calculator are also automated in: 
            /// 	public void BVT_MouseInput()
            /// 	public void BVT_Hotkeys()
            /// This automated test not only verifies each opperator individually, but it also simultaneously verifies mixed user input.
            /// Test Case 17416430: BVT: Standard: Verify arithmetic operators
            /// </summary>

            //Verify Addition
            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.PlusButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("4", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Subtraction
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.MinusButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("1", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Multiplication
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.MultiplyButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("6", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Division
            page.StandardOperators.NumberPad.Input(6);
            page.StandardOperators.MultiplyButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("12", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Reciprocal
            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.InvertButton.Click();
            Assert.AreEqual("0.5", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Square
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.XPower2Button.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("9", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Square root
            page.StandardOperators.NumberPad.Input(9);
            page.StandardOperators.XPower2Button.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("81", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Percent addition/subtraction
            page.StandardOperators.NumberPad.Input(10);
            page.StandardOperators.MinusButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("10");
            page.StandardOperators.PercentButton.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("9", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Percent multiplication/division
            page.StandardOperators.NumberPad.Input(10);
            page.StandardOperators.MultiplyButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("10");
            page.StandardOperators.PercentButton.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("1", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Positive/Negative (plus/minus)
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.MinusButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.StandardOperators.NegateButton.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("5", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Decimal
            page.StandardOperators.NumberPad.Input(3);
            page.StandardOperators.NumberPad.DecimalButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys("2");
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("3.2", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Equal
            page.HistoryPanel.ClearHistory();
            page.StandardOperators.EqualButton.Click();
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Equal);
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());
            var historyItems = page.HistoryPanel.GetAllHistoryListViewItems();
            Assert.IsTrue(historyItems[0].Text.Equals("0= 0", StringComparison.InvariantCultureIgnoreCase));
            Assert.IsTrue(historyItems[1].Text.Equals("0= 0", StringComparison.InvariantCultureIgnoreCase));
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Escape);

            //Verify Delete
            page.StandardOperators.NumberPad.Input(3);
            page.GlobalCalculatorUI.Header.SendKeys("3");
            Assert.AreEqual("33", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.StandardOperators.BackSpaceButton.Click();
            Assert.AreEqual("3", page.GlobalCalculatorUI.GetCalculatorResultText());
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Backspace);
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());

            //Verify Clear Entery
            page.StandardOperators.NumberPad.Input(3);
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Add);
            page.GlobalCalculatorUI.Header.SendKeys("3");
            page.GlobalCalculatorUI.AppName.Click();
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Delete);
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("3 +", page.GlobalCalculatorUI.GetCalculatorExpressionText());

            page.StandardOperators.NumberPad.Input(9);
            page.GlobalCalculatorUI.Header.SendKeys(Keys.Subtract);
            page.GlobalCalculatorUI.Header.SendKeys("6");
            page.StandardOperators.ClearEntryButton.Click();
            Assert.AreEqual("0", page.GlobalCalculatorUI.GetCalculatorResultText());
            Assert.AreEqual("3 + 9 Minus (", page.GlobalCalculatorUI.GetCalculatorExpressionText());

            //Verify Clear
            page.StandardOperators.ClearButton.Click();
            string sourceD = WinAppDriver.Instance.CalculatorSession.PageSource;
            if (sourceD.Contains("CalculatorExpression"))
            {
                throw new NotFoundException("This test fails; the Calculator Expression should be cleared");
            }
            else
            {
                return;
            }
        }

        [TestMethod]
        [Priority(3)]
        public void BVT_SwitchBetweenStandardAndOtherTypes()
        {
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ScientificCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Scientific", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.ProgrammerCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Programmer", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.DateCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Date Calculation", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Currency);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Currency", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Volume);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Volume", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Length);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Length", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Length);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Length", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Weight);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Weight and Mass", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Weight);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Weight and Mass", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Temperature);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Temperature", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Energy);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Energy", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Area);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Area", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Speed);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Speed", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Time);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Time", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Power);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Power", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Data);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Data", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Pressure);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Pressure", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.Angle);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Angle", page.GlobalCalculatorUI.GetCalculatorHeaderText());
            page.NavigationMenu.ChangeCalculatorMode(CalculatorMode.StandardCalculator);
            page.GlobalCalculatorUI.AppName.Click();
            Assert.AreEqual("Standard", page.GlobalCalculatorUI.GetCalculatorHeaderText());
        }

        #endregion
    }
}
