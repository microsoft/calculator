// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorUITestFramework;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using OpenQA.Selenium;
using System;

namespace CalculatorUITests
{
    [TestClass]
    public class ProgrammerModeFunctionalTests
    {
        private static ProgrammerCalculatorPage page = new ProgrammerCalculatorPage();

        /// <summary>
        /// Initializes the WinAppDriver web driver session.
        /// </summary>
        /// <param name="context"></param>
        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            // Create session to launch a Calculator window
            WinAppDriver.Instance.SetupCalculatorSession(context);

            // Ensure that calculator is in programmer mode
            page.NavigateToProgrammerCalculator();

            // Ensure that calculator window is large enough to display the memory/history panel; a good size for most tests
            page.MemoryPanel.ResizeWindowToDisplayMemoryLabel();
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
        /// Ensures the calculator is in a cleared state with the correct default options
        /// </summary>
        [TestInitialize]
        public void TestInit()
        {
            if ("0" != page.CalculatorResults.GetCalculatorResultText())
            {
                page.ClearAll();
            }
            CalculatorApp.EnsureCalculatorHasFocus();
            page.ProgrammerOperators.SetArithmeticShift();
            page.ProgrammerOperators.ResetWordSize();
            page.ProgrammerOperators.ResetNumberSystem();
        }

        [TestCleanup]
        public void TestCleanup()
        {
            page.ClearAll();
        }

        /// <summary>
        /// Arithmetic shift, decimal notation, QWord
        /// </summary>
        #region Smoke Tests
        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_LeftShift()
        {
            page.StandardOperators.NumberPad.Input(5);
            page.ProgrammerOperators.LeftShiftButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("10", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_RightShift()
        {
            page.StandardOperators.NumberPad.Input(25);
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RightShiftButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("-13", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_And()
        {
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.AndButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_Nand()
        {
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NandButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("-2", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_Or()
        {
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.OrButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("31", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_Nor()
        {
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NorButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("-32", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_Not()
        {
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NotButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("-26", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(0)]
        public void SmokeTest_Arithmetic_Decimal_Xor()
        {
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.XorButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("30", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Checks for the highest value bit that should be enabled according to the Word size setting
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void SmokeTest_WordSize()
        {
            page.ProgrammerOperators.BitFlip.Click();
            if (page.ProgrammerOperators.Bit63.GetAttribute("IsEnabled") != "True")
            {
                throw new NotFoundException("According to the current Word size setting, some bits that should be enabled are disabled");
            }
            else
            {
                page.ProgrammerOperators.QWordButton.Click();
                if (page.ProgrammerOperators.Bit31.GetAttribute("IsEnabled") != "True")
                {
                    throw new NotFoundException("According to the current Word size setting, some bits that should be enabled are disabled");
                }
                else
                {
                    page.ProgrammerOperators.DWordButton.Click();
                    if (page.ProgrammerOperators.Bit15.GetAttribute("IsEnabled") != "True")
                    {
                        throw new NotFoundException("According to the current Word size setting, some bits that should be enabled are disabled");
                    }
                    else
                    {
                        page.ProgrammerOperators.WordButton.Click();
                        if (page.ProgrammerOperators.Bit7.GetAttribute("IsEnabled") != "True")
                        {
                            throw new NotFoundException("According to the current Word size setting, some bits that should be enabled are disabled");
                        }
                        else
                        {
                            page.ProgrammerOperators.ByteButton.Click();
                            if (page.ProgrammerOperators.Bit63.GetAttribute("IsEnabled") != "True")
                            {
                                throw new NotFoundException("According to the current Word size setting, some bits that should be enabled are disabled");
                            }
                        }
                    }
                }
                page.ProgrammerOperators.FullKeypad.Click();
            }
        }


        /// <summary>
        /// Toggles each bit on and off
        /// </summary>
        [TestMethod]
        [Priority(0)]
        public void SmokeTest_BitFlipKeypad()
        {
            page.ProgrammerOperators.BitFlip.Click();
            page.ProgrammerOperators.Bit63.Click();
            Assert.AreEqual("-9,223,372,036,854,775,808", page.CalculatorResults.GetCalculatorResultText());
            page.ProgrammerOperators.FullKeypad.Click();
        }
        #endregion

        /// <summary>
        /// Arithmetic shift, octal notation, QWord
        /// </summary>
        #region Arithmetic logic operators
        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_LeftShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.ProgrammerOperators.LeftShiftButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 6", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_RightShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(25);
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RightShiftButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 6 5", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_And()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(16);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.AndButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("6", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_Nand()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(16);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NandButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 7 1", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_Or()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(16);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.OrButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 7", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_Nor()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(16);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NorButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 6 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_Not()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(16);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NotButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 6 1", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Octal_Xor()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.StandardOperators.NumberPad.Input(16);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.XorButton.Click();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 1", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Arithmetic shift, binary notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_LeftShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.LeftShiftButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  0 1 0 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_RightShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.RightShiftButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 0 1", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_And()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.AndButton.Click();
            page.StandardOperators.NumberPad.Input(1000);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 0 0 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_Nand()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NandButton.Click();
            page.StandardOperators.NumberPad.Input(1000);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  0 1 1 1", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_Or()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.OrButton.Click();
            page.StandardOperators.NumberPad.Input(1100);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 1 1 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_Nor()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NorButton.Click();
            page.StandardOperators.NumberPad.Input(1100);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  0 0 0 1", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_Not()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NotButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  0 1 0 1", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Binary_Xor()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.XorButton.Click();
            page.StandardOperators.NumberPad.Input(1100);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 1 0", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Arithmetic shift, hexadecimal notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_LeftShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.LeftShiftButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("5 0 0 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_RightShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.FButton.Click();
            page.ProgrammerOperators.RightShiftButton.Click();
            page.StandardOperators.NumberPad.Input(2);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("3", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_And()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.ProgrammerOperators.CButton.Click();
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.AndButton.Click();
            page.ProgrammerOperators.FButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("C", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_Nand()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.ProgrammerOperators.CButton.Click();
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NandButton.Click();
            page.ProgrammerOperators.FButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("F F F F  F F F F  F F F F  F F F 3", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_Or()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.ProgrammerOperators.CButton.Click();
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.OrButton.Click();
            page.ProgrammerOperators.FButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("A B F", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_Nor()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.ProgrammerOperators.CButton.Click();
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NorButton.Click();
            page.ProgrammerOperators.FButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("F F F F  F F F F  F F F F  F 5 4 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_Not()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.ProgrammerOperators.CButton.Click();
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.NotButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("F F F F  F F F F  F F F F  F 5 4 3", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Arithmetic_Operator_Hex_Xor()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.ProgrammerOperators.CButton.Click();
            page.ProgrammerOperators.BitwiseButton.Click();
            page.ProgrammerOperators.XorButton.Click();
            page.ProgrammerOperators.FButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("A B 3", page.CalculatorResults.GetCalculatorResultText());
        }
        #endregion

        /// <summary>
        /// Logical shift, decimal notation, QWord - AND/NAND/OR/NOR/NOT/XOR will always be the same regardless of what shift setting is selected, so the previous section should have those covered
        /// </summary>
        #region Logical-shift operators
        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Decimal_LeftShift()
        {
            page.ProgrammerOperators.DecButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.StandardOperators.NumberPad.Input(7);
            page.ProgrammerOperators.LeftShiftLogicalButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("14", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Decimal_RightShift()
        {
            page.ProgrammerOperators.DecButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.StandardOperators.NumberPad.Input(16);
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RightShiftLogicalButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("9,223,372,036,854,775,800", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Logical shift, octal notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Octal_LeftShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.StandardOperators.NumberPad.Input(7);
            page.ProgrammerOperators.LeftShiftLogicalButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 6", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Octal_RightShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.StandardOperators.NumberPad.Input(16);
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RightShiftLogicalButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 7 7  7 7 1", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Logical shift, binary notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Binary_LeftShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.LeftShiftLogicalButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  0 1 0 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Binary_RightShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.StandardOperators.NumberPad.Input(1010);
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RightShiftLogicalButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.IsTrue(String.Equals(page.CalculatorResults.GetCalculatorResultText(), "1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 1 1 1  1 0 1 1", StringComparison.OrdinalIgnoreCase));
        }

        /// <summary>
        /// Logical shift, hexadecimal notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Hex_LeftShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.LeftShiftLogicalButton.Click();
            page.ProgrammerOperators.BButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("5 0 0 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Logical_Operator_Hex_RightShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.SetLogicalShift();
            page.ProgrammerOperators.FButton.Click();
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RightShiftLogicalButton.Click();
            page.StandardOperators.NumberPad.Input(1);
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("7 F F F  F F F F  F F F F  F F F 8", page.CalculatorResults.GetCalculatorResultText());
        }
        #endregion

        /// <summary>
        /// Rotate circular shift, decimal notation, QWord
        /// </summary>
        #region Rotate-Circular-shift operators
        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Decimal_LeftShift()
        {
            page.ProgrammerOperators.SetRotateCircularShift();
            page.StandardOperators.NumberPad.Input(7);
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("14", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Decimal_RightShift()
        {
            page.ProgrammerOperators.SetRotateCircularShift();
            page.StandardOperators.NumberPad.Input(16);
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RoRButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("9,223,372,036,854,775,800", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Rotate circular shift, octal notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Octal_LeftShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.ProgrammerOperators.SetRotateCircularShift();
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("5 2", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Octal_RightShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.ProgrammerOperators.SetRotateCircularShift();
            page.StandardOperators.NumberPad.Input(25);
            page.ProgrammerOperators.RoRButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  0 0 0  0 0 0  0 0 0  0 0 0  0 0 0  0 0 0  0 1 2", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Rotate circular shift, binary notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Binary_LeftShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.ProgrammerOperators.SetRotateCircularShift();
            page.StandardOperators.NumberPad.Input(1011);
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  0 1 1 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Binary_RightShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.ProgrammerOperators.SetRotateCircularShift();
            page.StandardOperators.NumberPad.Input(1011);
            page.ProgrammerOperators.RoRButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 0 0 0  0 1 0 1", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Rotate circular shift, hexadecimal notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Hex_LeftShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.SetRotateCircularShift();
            page.ProgrammerOperators.AButton.Click();
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 4", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Circular_Operator_Hex_RightShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.SetRotateCircularShift();
            page.ProgrammerOperators.FButton.Click();
            page.ProgrammerOperators.RoRButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("8 0 0 0  0 0 0 0  0 0 0 0  0 0 0 7", page.CalculatorResults.GetCalculatorResultText());
        }
        #endregion

        /// <summary>
        /// Rotate through carry circular shift, decimal notation, QWord
        /// </summary>
        #region Rotate-Through-Carry-Circular-shift operators
        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Decimal_LeftShift()
        {
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(7);
            page.StandardOperators.NegateButton.Click();
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("-14", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Decimal_RightShift()
        {
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(17);
            page.ProgrammerOperators.RoRCarryButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("8", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Rotate through carry circular shift, octal notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Octal_LeftShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(111);
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("2 2 2", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Octal_RightShift()
        {
            page.ProgrammerOperators.OctButton.Click();
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(111);
            page.ProgrammerOperators.RoRCarryButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("4 4", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Rotate through carry circular shift, binary notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Binary_LeftShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1  0 1 0 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Binary_RightShift()
        {
            page.ProgrammerOperators.BinButton.Click();
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(1011);
            page.ProgrammerOperators.RoRCarryButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("1 0 1", page.CalculatorResults.GetCalculatorResultText());
        }

        /// <summary>
        /// Rotate through carry circular shift, hexadecimal notation, QWord
        /// </summary>
        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Hex_LeftShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.RoLButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("2 0 2 0", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void ThroughCarry_Operator_Hex_RightShift()
        {
            page.ProgrammerOperators.HexButton.Click();
            page.ProgrammerOperators.SetRotateThroughCarryCircularShift();
            page.StandardOperators.NumberPad.Input(1010);
            page.ProgrammerOperators.RoRCarryButton.Click();
            page.StandardOperators.EqualButton.Click();
            Assert.AreEqual("8 0 8", page.CalculatorResults.GetCalculatorResultText());
        }
        #endregion

        /// <summary>
        /// Copy and Paste the numbers into/from the calculator
        /// </summary>
        #region Copy-Paste operations
        [TestMethod]
        [Priority(1)]
        public void Copy_And_Paste_Simple_Number()
        {
            page.ProgrammerOperators.BitFlip.Click();
            page.ProgrammerOperators.Bit1.Click();
            CalculatorApp.Window.SendKeys(Keys.Control + "c" + Keys.Control);
            page.ProgrammerOperators.FullKeypad.Click();
            page.StandardOperators.ClearEntryButton.Click();
            CalculatorApp.Window.SendKeys(Keys.Control + "v" + Keys.Control);
            Assert.AreEqual("2", page.CalculatorResults.GetCalculatorResultText());
        }

        [TestMethod]
        [Priority(1)]
        public void Copy_And_Paste_Invalid_Number()
        {
            page.ProgrammerOperators.BitFlip.Click();
            page.ProgrammerOperators.Bit63.Click();
            CalculatorApp.Window.SendKeys(Keys.Control + "c" + Keys.Control);
            page.ProgrammerOperators.FullKeypad.Click();
            page.StandardOperators.ClearEntryButton.Click();
            page.ProgrammerOperators.QWordButton.Click();
            CalculatorApp.Window.SendKeys(Keys.Control + "v" + Keys.Control);
            Assert.AreEqual("Invalid input", page.CalculatorResults.GetCalculatorResultText());
        }
        #endregion
    }
}
