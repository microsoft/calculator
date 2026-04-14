// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    /// <summary>
    /// Represents a test command with expected display and expression results.
    /// </summary>
    internal struct TestItem
    {
        public NumbersAndOperatorsEnum Command;
        public string ExpectedPrimaryDisplay;
        public string ExpectedExpression;

        public TestItem(NumbersAndOperatorsEnum command, string expectedPrimaryDisplay, string expectedExpression)
        {
            Command = command;
            ExpectedPrimaryDisplay = expectedPrimaryDisplay;
            ExpectedExpression = expectedExpression;
        }
    }

    [TestClass]
    public class StandardCalculatorViewModelTests
    {
        private StandardCalculatorViewModel _viewModel;
        private string _decimalSeparator;

        [TestInitialize]
        public void InitializeViewModel()
        {
            _viewModel = new StandardCalculatorViewModel();
            _viewModel.IsStandard = true;
            var engineResourceProvider = new EngineResourceProvider();
            _decimalSeparator = engineResourceProvider.GetCEngineString("sDecimal");
        }

        #region Helper Methods

        private static void CompareVector(IList<bool> vecA, IList<bool> vecB)
        {
            Assert.AreEqual(vecA.Count, vecB.Count, "Vector sizes differ");
            for (int i = 0; i < vecA.Count; i++)
            {
                Assert.AreEqual(vecA[i], vecB[i], $"Mismatch at index {i}");
            }
        }

        private static void ChangeMode(StandardCalculatorViewModel viewModel, int mode)
        {
            if (mode == 0)
            {
                viewModel.IsStandard = true;
                viewModel.IsScientific = false;
                viewModel.IsProgrammer = false;
                viewModel.SetCalculatorType(ViewMode.Standard);
            }
            else if (mode == 1)
            {
                viewModel.IsScientific = true;
                viewModel.IsProgrammer = false;
                viewModel.IsStandard = false;
                viewModel.SetCalculatorType(ViewMode.Scientific);
            }
            else if (mode == 2)
            {
                viewModel.IsProgrammer = true;
                viewModel.IsScientific = false;
                viewModel.IsStandard = false;
                viewModel.SetCalculatorType(ViewMode.Programmer);
            }
        }

        private static object CommandParameterFromTestItem(TestItem item)
        {
            // Binary operators need to be wrapped with their auditory feedback
            var binOps = new (NumbersAndOperatorsEnum op, string feedback)[]
            {
                (NumbersAndOperatorsEnum.Add, "plus"),
                (NumbersAndOperatorsEnum.Subtract, "minus"),
                (NumbersAndOperatorsEnum.Multiply, "times"),
                (NumbersAndOperatorsEnum.Divide, "divided by")
            };

            foreach (var binOp in binOps)
            {
                if (item.Command == binOp.op)
                {
                    return new CalculatorButtonPressedEventArgs(binOp.feedback, binOp.op);
                }
            }

            return item.Command;
        }

        private static void ValidateViewModelByCommands(StandardCalculatorViewModel viewModel, TestItem[] items, bool doReset = false)
        {
            if (doReset)
            {
                viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Clear);
                viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.ClearEntry);
                viewModel.ClearMemoryCommand.Execute(null);
            }

            foreach (var item in items)
            {
                if (item.Command == NumbersAndOperatorsEnum.None)
                    break;

                object commandParam = CommandParameterFromTestItem(item);
                viewModel.ButtonPressed.Execute(commandParam);

                if (item.ExpectedPrimaryDisplay != "N/A")
                {
                    Assert.AreEqual(item.ExpectedPrimaryDisplay, viewModel.DisplayValue);
                }
            }
        }

        private void ValidateViewModelValue(string value)
        {
            if (value != null)
            {
                Assert.AreEqual(value, _viewModel.DisplayValue);
            }
        }

        private void ValidateViewModelValueAndSecondaryExpression(string value, string expression = null)
        {
            int nTokens = _viewModel.ExpressionTokens.Count;
            string displaySecondaryExpression = string.Empty;

            for (int i = 0; i < nTokens; i++)
            {
                var currentToken = _viewModel.ExpressionTokens[i];
                displaySecondaryExpression += currentToken.Token;
            }

            if (value != null)
            {
                Assert.AreEqual(value, _viewModel.DisplayValue);
            }

            if (expression != null)
            {
                Assert.AreEqual(expression, displaySecondaryExpression);
            }
        }

        #endregion

        #region Constructor Tests

        [TestMethod]
        public void ViewModelConstructorDisplayValueAndExpressionInitializedTest()
        {
            var vm = new StandardCalculatorViewModel();
            vm.IsStandard = true;
            Assert.AreEqual("0", vm.DisplayValue);
            Assert.AreEqual("Display is 0", vm.CalculationResultAutomationName);
        }

        [TestMethod]
        public void ViewModelConstructorButtonPressedInitializedTest()
        {
            var vm = new StandardCalculatorViewModel();
            vm.IsStandard = true;
            Assert.IsNotNull(vm.ButtonPressed);
        }

        #endregion

        #region Basic Arithmetic Tests

        /// <summary>Expression: 135</summary>
        [TestMethod]
        public void ButtonPressedLeftHandOperandEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Five, "135", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 13.</summary>
        [TestMethod]
        public void ButtonPressedLeftHandOperandAndDecimalEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Decimal, "13" + _decimalSeparator, ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 13==</summary>
        [TestMethod]
        public void ButtonPressedLeftHandOperandAndEqualsEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Equals, "13", ""),
                new(NumbersAndOperatorsEnum.Equals, "13", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 13+</summary>
        [TestMethod]
        public void ButtonPressedLeftHandOperandAndOperationEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Add, "13", "13 + "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 13+801</summary>
        [TestMethod]
        public void ButtonPressedRightHandOperandEnteredTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Three, "13", ""),
                new(NumbersAndOperatorsEnum.Add, "13", "13 + "),
                new(NumbersAndOperatorsEnum.Eight, "8", "13 + "),
                new(NumbersAndOperatorsEnum.Zero, "80", "13 + "),
                new(NumbersAndOperatorsEnum.One, "801", "13 + "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 1+2=</summary>
        [TestMethod]
        public void ButtonPressedAdditionWithEqualsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 1-2=</summary>
        [TestMethod]
        public void ButtonPressedSubtractionWithEqualsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Subtract, "1", "1 - "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 - "),
                new(NumbersAndOperatorsEnum.Equals, "-1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 3*5=</summary>
        [TestMethod]
        public void ButtonPressedMultiplyWithEqualsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Multiply, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Five, "5", "3 * "),
                new(NumbersAndOperatorsEnum.Equals, "15", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 9/3=</summary>
        [TestMethod]
        public void ButtonPressedDivideTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Nine, "9", ""),
                new(NumbersAndOperatorsEnum.Divide, "9", "9 / "),
                new(NumbersAndOperatorsEnum.Three, "3", "9 / "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 7.555*3=</summary>
        [TestMethod]
        public void ButtonPressedDecimalOperationTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Decimal, "7" + _decimalSeparator, ""),
                new(NumbersAndOperatorsEnum.Five, "7" + _decimalSeparator + "5", ""),
                new(NumbersAndOperatorsEnum.Five, "7" + _decimalSeparator + "55", ""),
                new(NumbersAndOperatorsEnum.Five, "7" + _decimalSeparator + "555", ""),
                new(NumbersAndOperatorsEnum.Multiply, "7" + _decimalSeparator + "555", "7" + _decimalSeparator + "555 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "7" + _decimalSeparator + "555 * "),
                new(NumbersAndOperatorsEnum.Equals, "22" + _decimalSeparator + "665", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 7/0</summary>
        [TestMethod]
        public void ButtonPressedDivideByZeroNegativeTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Divide, "7", "7 / "),
                new(NumbersAndOperatorsEnum.Zero, "0", "7 / "),
                new(NumbersAndOperatorsEnum.Equals, "Cannot divide by zero", "7 / "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 8/2*</summary>
        [TestMethod]
        public void ButtonPressedExpressionWithMultipleOperatorsTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Eight, "8", ""),
                new(NumbersAndOperatorsEnum.Divide, "8", "8 / "),
                new(NumbersAndOperatorsEnum.Two, "2", "8 / "),
                new(NumbersAndOperatorsEnum.Multiply, "4", "8 / 2 * "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 8/+*2*</summary>
        [TestMethod]
        public void ButtonPressedExpressionWithMultipleOperatorsInSuccessionTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Eight, "8", ""),
                new(NumbersAndOperatorsEnum.Divide, "8", "8 / "),
                new(NumbersAndOperatorsEnum.Add, "8", "8 + "),
                new(NumbersAndOperatorsEnum.Multiply, "8", "8 * "),
                new(NumbersAndOperatorsEnum.Two, "2", "8 * "),
                new(NumbersAndOperatorsEnum.Multiply, "16", "8 * 2 * "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 8*2==</summary>
        [TestMethod]
        public void ButtonPressedExpressionWithMultipleEqualsAfterEvaluateTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Eight, "8", ""),
                new(NumbersAndOperatorsEnum.Multiply, "8", "8 * "),
                new(NumbersAndOperatorsEnum.Two, "2", "8 * "),
                new(NumbersAndOperatorsEnum.Equals, "16", ""),
                new(NumbersAndOperatorsEnum.Equals, "32", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 7-6 and Backspace</summary>
        [TestMethod]
        public void ButtonPressedExpressionWithBackSpaceTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Subtract, "7", "7 - "),
                new(NumbersAndOperatorsEnum.Six, "6", "7 - "),
                new(NumbersAndOperatorsEnum.Backspace, "0", "7 - "),
                new(NumbersAndOperatorsEnum.Backspace, "0", "7 - "),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        /// <summary>Expression: 91-68 and Clear</summary>
        [TestMethod]
        public void ButtonPressedExpressionWithClearTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Nine, "9", ""),
                new(NumbersAndOperatorsEnum.One, "91", ""),
                new(NumbersAndOperatorsEnum.Subtract, "91", "91 - "),
                new(NumbersAndOperatorsEnum.Six, "6", "91 - "),
                new(NumbersAndOperatorsEnum.Eight, "68", "91 - "),
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
        }

        #endregion

        #region Paste Tests

        /// <summary>Low-level test of character mapping</summary>
        [TestMethod]
        public void VerifyCorrectCharacterMapping()
        {
            // Valid numbers
            var n = _viewModel.MapCharacterToButtonId('0').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Zero, n);

            n = _viewModel.MapCharacterToButtonId('1').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.One, n);

            // Valid operators
            n = _viewModel.MapCharacterToButtonId('+').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Add, n);

            n = _viewModel.MapCharacterToButtonId('=').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.Equals, n);

            n = _viewModel.MapCharacterToButtonId('a').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.A, n);

            // Invalid character
            n = _viewModel.MapCharacterToButtonId('$').ButtonId;
            Assert.AreEqual(NumbersAndOperatorsEnum.None, n);
        }

        /// <summary>Various strings get pasted</summary>
        [TestMethod]
        public void PasteExpressions()
        {
            _viewModel.IsScientific = false;

            _viewModel.OnPaste("-0.99");
            ValidateViewModelValue("-0" + _decimalSeparator + "99");

            _viewModel.OnPaste("1+1=");
            ValidateViewModelValue("2");

            // This result is not obvious: it's the result of the previous operation
            _viewModel.OnPaste("0=");
            ValidateViewModelValue("1");

            // Negative value
            _viewModel.OnPaste("-1");
            ValidateViewModelValue("-1");

            // Negated expression
            _viewModel.OnPaste("-(1+1)");
            ValidateViewModelValueAndSecondaryExpression("-2", "negate(1 + 1)");

            // More complicated Negated expression
            _viewModel.OnPaste("-(-(-1))");
            ValidateViewModelValueAndSecondaryExpression("-1", "negate(0 - (0 - 1))");

            // Switch to scientific mode
            _viewModel.IsScientific = true;

            Assert.IsFalse(_viewModel.IsFToEChecked);

            // Positive exponent
            _viewModel.OnPaste("1.23e+10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e+10");

            _viewModel.OnPaste("1.23e10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e+10");

            _viewModel.OnPaste("135e10");
            ValidateViewModelValue("135" + _decimalSeparator + "e+10");

            // Negative exponent
            _viewModel.OnPaste("1.23e-10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e-10");

            // Uppercase E (for exponent)
            _viewModel.OnPaste("1.23E-10");
            ValidateViewModelValue("1" + _decimalSeparator + "23e-10");

            _viewModel.OnPaste("135E10");
            ValidateViewModelValue("135" + _decimalSeparator + "e+10");
        }

        #endregion

        #region Automation Name Tests

        [TestMethod]
        public void CalculationResultAutomationNameVerification()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.Three, "123", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("Display is 123", _viewModel.CalculationResultAutomationName);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, true);
            Assert.AreEqual("Display is 7", _viewModel.CalculationResultAutomationName);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.InvSin, "Invalid input", "asind(5)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);
            Assert.AreEqual("Display is Invalid input", _viewModel.CalculationResultAutomationName);
        }

        #endregion

        #region Mode Switch Tests

        [TestMethod]
        public void ButtonPressedCalculatorModeSwitch()
        {
            // Standard mode: 1+2*3 = 9 (left-to-right evaluation)
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "9", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);

            // Scientific mode: 1+2*3 = 7 (operator precedence)
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "2", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "1 + 2 * "),
                new(NumbersAndOperatorsEnum.Equals, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, true);
        }

        #endregion

        #region Programmer Mode Tests

        [TestMethod]
        public void ProgrammerModeAutoConvertedValue()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, false);
            Assert.AreEqual("F", _viewModel.HexDisplayValue);
            Assert.AreEqual("15", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("17", _viewModel.OctalDisplayValue);
            Assert.AreEqual("1111", _viewModel.BinaryDisplayValue);

            var val = new bool[64];
            val[0] = true;
            val[1] = true;
            val[2] = true;
            val[3] = true;
            CompareVector(_viewModel.BinaryDigits, val);
        }

        /// <summary>Test Button disabling in different Radixes (commented out in original)</summary>
        [TestMethod]
        public void ProgrammerModeButtonsDisable()
        {
            // This test was commented out in the original C++ source.
            // Keeping as placeholder to match original test structure.
        }

        [TestMethod]
        public void ProgrammerModeRadixGrouping()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.Three, "123", ""),
                new(NumbersAndOperatorsEnum.Four, "1,234", ""),
                new(NumbersAndOperatorsEnum.Five, "12,345", ""),
                new(NumbersAndOperatorsEnum.Six, "123,456", ""),
                new(NumbersAndOperatorsEnum.Seven, "1,234,567", ""),
                new(NumbersAndOperatorsEnum.Eight, "12,345,678", ""),
                new(NumbersAndOperatorsEnum.Nine, "123,456,789", ""),
                new(NumbersAndOperatorsEnum.None, "1", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("75B CD15", _viewModel.HexDisplayValue);
            Assert.AreEqual("123,456,789", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("726 746 425", _viewModel.OctalDisplayValue);
            Assert.AreEqual("0111 0101 1011 1100 1101 0001 0101", _viewModel.BinaryDisplayValue);

            var val = new bool[64];
            val[0] = true;
            val[2] = true;
            val[4] = true;
            val[8] = true;
            val[10] = true;
            val[11] = true;
            val[14] = true;
            val[15] = true;
            val[16] = true;
            val[17] = true;
            val[19] = true;
            val[20] = true;
            val[22] = true;
            val[24] = true;
            val[25] = true;
            val[26] = true;
            CompareVector(_viewModel.BinaryDigits, val);
        }

        [TestMethod]
        public void ProgrammerModeNot()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Not, "-2", "~(1)"),
                new(NumbersAndOperatorsEnum.None, "N/A", "N/A")
            };
            ValidateViewModelByCommands(_viewModel, items, false);
            Assert.AreEqual("FFFF FFFF FFFF FFFE", _viewModel.HexDisplayValue);
            Assert.AreEqual("-2", _viewModel.DecimalDisplayValue);
            Assert.AreEqual("1 777 777 777 777 777 777 776", _viewModel.OctalDisplayValue);
            Assert.AreEqual("1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1110", _viewModel.BinaryDisplayValue);
            Assert.AreEqual("-2", _viewModel.DisplayValue);

            var val = Enumerable.Repeat(true, 64).ToArray();
            val[0] = false;
            CompareVector(_viewModel.BinaryDigits, val);
        }

        [TestMethod]
        public void ProgrammerModeAndOr()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Or, "1", "1 ||"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 ||"),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "3", "")
            };
            ValidateViewModelByCommands(_viewModel, items, false);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.And, "1", "1 &"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 &"),
                new(NumbersAndOperatorsEnum.Equals, "0", ""),
                new(NumbersAndOperatorsEnum.None, "0", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
        }

        [TestMethod]
        public void ProgrammerModeClear()
        {
            var none = new TestItem[] { new(NumbersAndOperatorsEnum.None, "", "") };
            ValidateViewModelByCommands(_viewModel, none, true);
            _viewModel.IsProgrammer = true;

            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Or, "1", "1 ||"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 ||"),
                new(NumbersAndOperatorsEnum.ClearEntry, "0", "1 ||"),
                new(NumbersAndOperatorsEnum.None, "", "1 ||")
            };
            ValidateViewModelByCommands(_viewModel, items, false);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.And, "1", "1 &"),
                new(NumbersAndOperatorsEnum.Two, "2", "1 &"),
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.None, "0", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
        }

        #endregion

        #region Unary Operator Tests

        [TestMethod]
        public void ButtonPressedUnaryOperatorTest()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.Invert, "0" + _decimalSeparator + "2", "reciproc(5)"),
                new(NumbersAndOperatorsEnum.Equals, "0" + _decimalSeparator + "2", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Six, "16", ""),
                new(NumbersAndOperatorsEnum.Sqrt, "4", "sqrt(16)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Six, "6", ""),
                new(NumbersAndOperatorsEnum.Negate, "-6", ""),
                new(NumbersAndOperatorsEnum.Nine, "-69", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);

            var items4 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.Five, "5", ""),
                new(NumbersAndOperatorsEnum.InvSin, "Invalid input", "asind(5)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items4, false);

            var items5 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Clear, "0", ""),
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Factorial, "24", "fact(4)"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items5, false);
        }

        #endregion

        #region Memory Tests

        [TestMethod]
        public void IsMemoryEmptyTest()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            Assert.AreEqual(0, viewModel.MemorizedNumbers.Count);
            viewModel.OnMemoryButtonPressed();
            Assert.AreEqual(1, viewModel.MemorizedNumbers.Count);
            viewModel.ClearMemoryCommand.Execute(null);
            Assert.AreEqual(0, viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void IsOperatorCommandTest()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.One);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Two);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Three);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Four);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Five);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Six);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Seven);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Eight);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Nine);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Decimal);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Zero);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Multiply);
            Assert.AreEqual(true, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Add);
            Assert.AreEqual(true, viewModel.IsOperatorCommand);
            viewModel.ButtonPressed.Execute(NumbersAndOperatorsEnum.Zero);
            Assert.AreEqual(false, viewModel.IsOperatorCommand);
        }

        [TestMethod]
        public void OnMemoryButtonPressed()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.DisplayValue = "1001";
            viewModel.OnMemoryButtonPressed();
            viewModel.OnMemoryButtonPressed();
            Assert.AreEqual(2, viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void OnMemoryAddWhenMemoryEmpty()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryAdd(0);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("1,001", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnMemorySubtractWhenMemoryEmpty()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemorySubtract(0);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnNegativeEntryInMemory()
        {
            ChangeMode(_viewModel, 0 /*Standard*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Negate, "N/A", "N/A"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001", _viewModel.DisplayValue);

            var memorySlotStandard = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotStandard.Value);

            ChangeMode(_viewModel, 1 /*Scientific*/);
            var memorySlotScientific = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotScientific.Value);

            ChangeMode(_viewModel, 2 /*Programmer*/);
            var memorySlotProgrammer = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("-1,001", memorySlotProgrammer.Value);
        }

        [TestMethod]
        public void OnDecimalEntryInMemory()
        {
            ChangeMode(_viewModel, 0 /*Standard*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("1,001.1", _viewModel.DisplayValue);

            var memorySlotStandard = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001.1", memorySlotStandard.Value);

            ChangeMode(_viewModel, 1 /*Scientific*/);
            var memorySlotScientific = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001.1", memorySlotScientific.Value);

            ChangeMode(_viewModel, 2 /*Programmer*/);
            var memorySlotProgrammer = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1,001", memorySlotProgrammer.Value);
        }

        [TestMethod]
        public void OnNegativeDecimalInMemory()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.Negate, "N/A", "N/A"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("-1,001.1", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnDecimalAddedToMemory()
        {
            _viewModel.IsStandard = true;
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.One, "1,001.1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Decimal, "1,001.", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryAdd(1);
            _viewModel.OnMemoryItemPressed(1);
            Assert.AreEqual("2,002.1", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnMemorySavedInHexRadixAndSwitchedToStandardMode()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();
            ChangeMode(_viewModel, 1 /*Scientific*/);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("255", _viewModel.DisplayValue);

            var memorySlot = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlot.Value);
        }

        [TestMethod]
        public void OnMemorySavedInHexRadixAndRadixChanges()
        {
            ChangeMode(_viewModel, 2 /*Programmer*/);
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "FF", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            _viewModel.SwitchProgrammerModeBase(NumberBase.OctBase);
            var memorySlotOct = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("377", memorySlotOct.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.DecBase);
            var memorySlotDec = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("255", memorySlotDec.Value);

            _viewModel.SwitchProgrammerModeBase(NumberBase.BinBase);
            var memorySlotBin = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("1111 1111", memorySlotBin.Value);
        }

        [TestMethod]
        public void OnMemoryButtonPressedMaxTimes()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.DisplayValue = "1001";
            for (int i = 0; i < 110; i++)
            {
                viewModel.OnMemoryButtonPressed();
            }
            Assert.AreEqual(100, viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void OnMemoryItemPressed()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Two, "2", ""),
                new(NumbersAndOperatorsEnum.Zero, "20", ""),
                new(NumbersAndOperatorsEnum.Zero, "200", ""),
                new(NumbersAndOperatorsEnum.Two, "2,002", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(1);

            Assert.AreEqual("1,001", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void OnMemoryItemPressedNoMemory()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Two, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("12", _viewModel.DisplayValue);
            Assert.AreEqual(0, _viewModel.MemorizedNumbers.Count);
        }

        [TestMethod]
        public void OnMemoryItemAddAndSubtract()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            _viewModel.OnMemoryButtonPressed();

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Two, "2", ""),
                new(NumbersAndOperatorsEnum.Zero, "20", ""),
                new(NumbersAndOperatorsEnum.Zero, "200", ""),
                new(NumbersAndOperatorsEnum.Two, "2,002", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryItemPressed(1);
            _viewModel.OnMemoryAdd(0);

            var memorySlot = _viewModel.MemorizedNumbers[0];
            Assert.AreEqual("3,003", memorySlot.Value);
        }

        #endregion

        #region Raw Formatting Tests

        [TestMethod]
        public void VerifyRawFormatting()
        {
            // GetRawDisplayValue is private, so we test via the LocalizationSettings
            // approach used internally: remove group separators from display value.
            var settings = LocalizationSettings.GetInstance();

            _viewModel.DisplayValue = "1,001";
            Assert.AreEqual("1001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "999";
            Assert.AreEqual("999", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001,001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));

            _viewModel.DisplayValue = "1,001, 001";
            Assert.AreEqual("1001001", settings.RemoveGroupSeparators(_viewModel.DisplayValue));
        }

        #endregion

        #region Announcement Tests

        [TestMethod]
        public void VerifyAnnouncementAfterBinaryOperatorReceived()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Multiply, "1,001", "1001 x"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("Display is 1,001 times", _viewModel.Announcement?.Announcement);
        }

        [TestMethod]
        public void VerifyAnnouncementAfterMultipleBinaryOperatorsReceived()
        {
            var items = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Zero, "100", ""),
                new(NumbersAndOperatorsEnum.One, "1,001", ""),
                new(NumbersAndOperatorsEnum.Multiply, "1,001", "1001 x"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items, true);
            Assert.AreEqual("Display is 1,001 times", _viewModel.Announcement?.Announcement);

            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Divide, "1,001", "1001 \u00f7"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items2, false);
            Assert.AreEqual("Display is 1,001 divided by", _viewModel.Announcement?.Announcement);

            var items3 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Add, "1,001", "1001 +"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items3, false);
            Assert.AreEqual("Display is 1,001 plus", _viewModel.Announcement?.Announcement);

            var items4 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Subtract, "1,001", "1001 x"),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(_viewModel, items4, false);
            Assert.AreEqual("Display is 1,001 minus", _viewModel.Announcement?.Announcement);
        }

        #endregion
    }
}
