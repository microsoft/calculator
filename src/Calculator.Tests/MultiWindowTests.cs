// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class MultiWindowUnitTests
    {
        #region Helper Methods

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

        private static void ValidateViewModelMode(StandardCalculatorViewModel viewModel, int mode)
        {
            if (mode == 0)
            {
                Assert.IsTrue(viewModel.IsStandard);
                Assert.IsFalse(viewModel.IsScientific);
                Assert.IsFalse(viewModel.IsProgrammer);
            }
            else if (mode == 1)
            {
                Assert.IsFalse(viewModel.IsStandard);
                Assert.IsTrue(viewModel.IsScientific);
                Assert.IsFalse(viewModel.IsProgrammer);
            }
            else if (mode == 2)
            {
                Assert.IsFalse(viewModel.IsStandard);
                Assert.IsFalse(viewModel.IsScientific);
                Assert.IsTrue(viewModel.IsProgrammer);
            }
        }

        private static object CommandParameterFromTestItem(TestItem item)
        {
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

        #endregion

        [TestMethod]
        public void InitializeMultipleInstancesTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                viewModels[i].IsStandard = true;
            }

            for (int i = 0; i < 3; i++)
            {
                Assert.AreEqual("0", viewModels[i].DisplayValue);
            }
        }

        [TestMethod]
        public void InitializeMultipleModeInstancesTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
            }

            for (int i = 0; i < 3; i++)
            {
                ValidateViewModelMode(viewModels[i], i);
                Assert.AreEqual("0", viewModels[i].DisplayValue);
            }
        }

        [TestMethod]
        public void MultipleModesCalculationTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
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
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Programmer Mode: Hex F = 15
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], programmerItems, false);
            Assert.AreEqual("F", viewModels[2].HexDisplayValue);
            Assert.AreEqual("15", viewModels[2].DecimalDisplayValue);
            Assert.AreEqual("17", viewModels[2].OctalDisplayValue);
            Assert.AreEqual("1111", viewModels[2].BinaryDisplayValue);

            // Assert Standard and Scientific display values unchanged
            Assert.AreEqual("3", viewModels[0].DisplayValue);
            Assert.AreEqual("7", viewModels[1].DisplayValue);
        }

        [TestMethod]
        public void MultipleStandardModeCalculationTest()
        {
            var standardViewModel1 = new StandardCalculatorViewModel();
            var standardViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(standardViewModel1, 0);
            ChangeMode(standardViewModel2, 0);

            ValidateViewModelMode(standardViewModel1, 0);
            ValidateViewModelMode(standardViewModel2, 0);

            // Standard 1: 3-2=1
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Subtract, "3", "3 - "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 - "),
                new(NumbersAndOperatorsEnum.Equals, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(standardViewModel1, items1, true);

            // Standard 2: 4*5=20
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Multiply, "4", "4 * "),
                new(NumbersAndOperatorsEnum.Five, "5", "4 * "),
                new(NumbersAndOperatorsEnum.Equals, "20", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(standardViewModel2, items2, true);

            Assert.AreEqual("1", standardViewModel1.DisplayValue);
        }

        [TestMethod]
        public void MultipleScientificModeCalculationTest()
        {
            var scientificViewModel1 = new StandardCalculatorViewModel();
            var scientificViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(scientificViewModel1, 1);
            ChangeMode(scientificViewModel2, 1);

            ValidateViewModelMode(scientificViewModel1, 1);
            ValidateViewModelMode(scientificViewModel2, 1);

            // Scientific 1: 3-2=1
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Subtract, "3", "3 - "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 - "),
                new(NumbersAndOperatorsEnum.Equals, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, items1, true);

            // Scientific 2: 4*5=20
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Four, "4", ""),
                new(NumbersAndOperatorsEnum.Multiply, "4", "4 * "),
                new(NumbersAndOperatorsEnum.Five, "5", "4 * "),
                new(NumbersAndOperatorsEnum.Equals, "20", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, items2, true);

            Assert.AreEqual("1", scientificViewModel1.DisplayValue);
        }

        [TestMethod]
        public void MultipleScientificModeWithDifferentSettingsTest()
        {
            var scientificViewModel1 = new StandardCalculatorViewModel();
            var scientificViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(scientificViewModel1, 1);
            ChangeMode(scientificViewModel2, 1);

            ValidateViewModelMode(scientificViewModel1, 1);
            ValidateViewModelMode(scientificViewModel2, 1);

            // Scientific 1: Degrees with HYP checked
            var initItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Degree, "0", ""),
                new(NumbersAndOperatorsEnum.Hyp, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, initItems1, true);

            // Scientific 2: Radians with F-E checked
            var initItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Radians, "0", ""),
                new(NumbersAndOperatorsEnum.FToE, "0.e+0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, initItems2, true);

            // Scientific 1: CosH(0 degrees)
            var testItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Zero, "0", ""),
                new(NumbersAndOperatorsEnum.Cosh, "1", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel1, testItems1, true);

            // Scientific 2: Cos(pi radians)
            var testItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Pi, "3.1415926535897932384626433832795e+0", ""),
                new(NumbersAndOperatorsEnum.Cos, "-1.e+0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(scientificViewModel2, testItems2, true);
        }

        [TestMethod]
        public void MultipleProgrammerModeCalculationTest()
        {
            var programmerViewModel1 = new StandardCalculatorViewModel();
            var programmerViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(programmerViewModel1, 2);
            ChangeMode(programmerViewModel2, 2);

            ValidateViewModelMode(programmerViewModel1, 2);
            ValidateViewModelMode(programmerViewModel2, 2);

            // Programmer 1: Hex F
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, items1, false);
            Assert.AreEqual("F", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("15", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("17", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111", programmerViewModel1.BinaryDisplayValue);

            // Programmer 2: Oct 7
            var items2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.OctButton, "0", ""),
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, items2, false);
            Assert.AreEqual("7", programmerViewModel2.HexDisplayValue);
            Assert.AreEqual("7", programmerViewModel2.DecimalDisplayValue);
            Assert.AreEqual("7", programmerViewModel2.OctalDisplayValue);
            Assert.AreEqual("0111", programmerViewModel2.BinaryDisplayValue);

            // Assert 1st instance unchanged
            Assert.AreEqual("F", programmerViewModel1.DisplayValue);
            Assert.AreEqual("F", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("15", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("17", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111", programmerViewModel1.BinaryDisplayValue);
        }

        [TestMethod]
        public void MultipleProgrammerModeWithDifferentSettingsTest()
        {
            var programmerViewModel1 = new StandardCalculatorViewModel();
            var programmerViewModel2 = new StandardCalculatorViewModel();

            ChangeMode(programmerViewModel1, 2);
            ChangeMode(programmerViewModel2, 2);

            ValidateViewModelMode(programmerViewModel1, 2);
            ValidateViewModelMode(programmerViewModel2, 2);

            // Programmer 1: Byte & Hex
            var initItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Byte, "0", ""),
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, initItems1, false);

            // Programmer 2: Word & Oct
            var initItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Word, "0", ""),
                new(NumbersAndOperatorsEnum.OctButton, "0", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, initItems2, false);

            // Programmer 1: FF (byte max)
            var testItems1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                // One more F shouldn't have any effect, testing for precision
                new(NumbersAndOperatorsEnum.F, "FF", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel1, testItems1, false);
            Assert.AreEqual("FF", programmerViewModel1.HexDisplayValue);
            Assert.AreEqual("-1", programmerViewModel1.DecimalDisplayValue);
            Assert.AreEqual("377", programmerViewModel1.OctalDisplayValue);
            Assert.AreEqual("1111 1111", programmerViewModel1.BinaryDisplayValue);

            // Programmer 2: 77777 (word, octal)
            var testItems2 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.Seven, "7", ""),
                new(NumbersAndOperatorsEnum.Seven, "77", ""),
                new(NumbersAndOperatorsEnum.Seven, "777", ""),
                new(NumbersAndOperatorsEnum.Seven, "7 777", ""),
                new(NumbersAndOperatorsEnum.Seven, "77 777", ""),
                // One more '7' shouldn't have any effect, testing for precision
                new(NumbersAndOperatorsEnum.Seven, "77 777", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(programmerViewModel2, testItems2, false);
            Assert.AreEqual("7FFF", programmerViewModel2.HexDisplayValue);
            Assert.AreEqual("32,767", programmerViewModel2.DecimalDisplayValue);
            Assert.AreEqual("77 777", programmerViewModel2.OctalDisplayValue);
            Assert.AreEqual("0111 1111 1111 1111", programmerViewModel2.BinaryDisplayValue);
        }

        [TestMethod]
        public void MultipleModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
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
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Assert history for Standard instance
            Assert.AreEqual(1, viewModels[0].HistoryVM.ItemsCount);
            var item1 = (HistoryItemViewModel)viewModels[0].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2 =", item1.Expression);
            Assert.AreEqual("3", item1.Result);

            // Assert history for Scientific instance
            Assert.AreEqual(1, viewModels[1].HistoryVM.ItemsCount);
            var item2 = (HistoryItemViewModel)viewModels[1].HistoryVM.Items[0];
            Assert.AreEqual("1   +   2   \u00D7   3 =", item2.Expression);
            Assert.AreEqual("7", item2.Result);
        }

        [TestMethod]
        public void MultipleStandardModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], 0);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Instance 0: 3+2=5, then 5*1=5
            var items0 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.Three, "3", ""),
                new(NumbersAndOperatorsEnum.Add, "3", "3 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "3 + "),
                new(NumbersAndOperatorsEnum.Multiply, "5", "5 * "),
                new(NumbersAndOperatorsEnum.One, "1", "5 * "),
                new(NumbersAndOperatorsEnum.Equals, "5", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], items0, true);

            // Instance 1: 1+2=3, then 3*3=9
            var items1 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsStandardMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Multiply, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Three, "3", "3 * "),
                new(NumbersAndOperatorsEnum.Equals, "9", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], items1, true);

            // Verify history items
            string[,] expressions = {
                { "5   \u00D7   1 =", "3   +   2 =" },
                { "3   \u00D7   3 =", "1   +   2 =" }
            };
            string[,] results = {
                { "5", "5" },
                { "9", "3" }
            };

            for (int i = 0; i < 2; i++)
            {
                Assert.AreEqual(2, viewModels[i].HistoryVM.ItemsCount);

                for (int j = 0; j < 2; j++)
                {
                    var item = (HistoryItemViewModel)viewModels[i].HistoryVM.Items[j];
                    Assert.AreEqual(expressions[i, j], item.Expression);
                    Assert.AreEqual(results[i, j], item.Result);
                }
            }
        }

        [TestMethod]
        public void MultipleScientificModesHistoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[2];

            for (int i = 0; i < 2; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], 1);
                Assert.AreEqual(0, viewModels[i].HistoryVM.ItemsCount);
            }

            // Instance 0: 1+2=3
            var items0 = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.IsScientificMode, "0", ""),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], items0, true);

            // Instance 1: 1+2*3=7
            var items1 = new TestItem[]
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
            ValidateViewModelByCommands(viewModels[1], items1, true);

            string[] expectedExpressions = { "1   +   2 =", "1   +   2   \u00D7   3 =" };
            string[] expectedResults = { "3", "7" };

            for (int i = 0; i < 2; i++)
            {
                Assert.AreEqual(1, viewModels[i].HistoryVM.ItemsCount);
                var item = (HistoryItemViewModel)viewModels[i].HistoryVM.Items[0];
                Assert.AreEqual(expectedExpressions[i], item.Expression);
                Assert.AreEqual(expectedResults[i], item.Result);
            }
        }

        [TestMethod]
        public void MultipleModesMemoryAddItemTest()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            for (int i = 0; i < 3; i++)
            {
                viewModels[i] = new StandardCalculatorViewModel();
                ChangeMode(viewModels[i], i);
                Assert.AreEqual(0, viewModels[i].MemorizedNumbers.Count);
            }

            // Standard Mode: 1+2=3
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Add, "1", "1 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "1 + "),
                new(NumbersAndOperatorsEnum.Equals, "3", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, true);

            // Scientific Mode: 1+2*3=7
            var scientificItems = new TestItem[]
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
            ValidateViewModelByCommands(viewModels[1], scientificItems, true);

            // Programmer Mode: Hex F
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], programmerItems, false);

            // Press Memory button to save values
            for (int i = 0; i < 3; i++)
            {
                viewModels[i].ButtonPressed.Execute(NumbersAndOperatorsEnum.Memory);
            }

            string[] expectedMemoryValues = { "3", "7", "F" };

            for (int i = 0; i < 3; i++)
            {
                Assert.AreEqual(1, viewModels[i].MemorizedNumbers.Count);
                var memorySlot = viewModels[i].MemorizedNumbers[0];
                Assert.AreEqual(expectedMemoryValues[i], memorySlot.Value);
            }
        }

        [TestMethod]
        [Ignore("TODO - MSFT 10331900")]
        public void MultipleDateCalculatorTest()
        {
            // This test is commented out in the C++ source due to incorrect expected values
            // in UTC- time zones. The test creates 4 DateCalculatorViewModel instances,
            // sets up date diff and add/subtract modes, performs calculations, and verifies results.
        }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock")]
        public void InitializeMultipleConverterTest()
        {
            // Creates 3 UnitConverterViewModel instances with UnitConverterMock,
            // verifies categories, changes categories on instances 0 and 2,
            // and asserts that category, unit1, and unit2 are set independently.
        }

        [TestMethod]
        [Ignore("Requires UWP ResourceLoader")]
        public void MultipleConverterModeCalculationTest()
        {
            // Creates 3 UnitConverterViewModel instances with UnitConverterDataLoader,
            // sets categories (Volume, Volume, Length) and units,
            // performs button presses (1, 2, 3), and validates conversion results
            // using resource strings.
        }

        [TestMethod]
        [Ignore("Requires UWP ResourceLoader")]
        public void TestStandardUnitConverterAndDateViewModels()
        {
            // Creates StandardCalculatorViewModel, UnitConverterViewModel, and DateCalculatorViewModel.
            // Performs 3-2=1 on Standard, sets date offset on DateCalc, presses 2 on converter.
            // Validates converter shows 2 -> 0.002, Standard stays at 1.
            // Then performs 1+2=3 on Standard and validates all instances remain independent.
        }

        [TestMethod]
        public void MultipleModesWithChangeInProgrammerRadix()
        {
            var viewModels = new StandardCalculatorViewModel[3];

            // Standard mode
            viewModels[0] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[0], 0);

            // Programmer mode
            viewModels[1] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[1], 2);

            // Change programmer radix to hex and enter F
            var programmerItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.HexButton, "0", ""),
                new(NumbersAndOperatorsEnum.F, "F", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], programmerItems, false);
            Assert.AreEqual("F", viewModels[1].HexDisplayValue);

            // Standard Mode: 10+2=12 (radix should be decimal, not hex)
            var standardItems = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Add, "10", "10 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "10 + "),
                new(NumbersAndOperatorsEnum.Equals, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[0], standardItems, false);

            // Launch a new Standard instance
            viewModels[2] = new StandardCalculatorViewModel();
            ChangeMode(viewModels[2], 0);

            // New Standard: 10+2=12
            var standardItemsNew = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Zero, "10", ""),
                new(NumbersAndOperatorsEnum.Add, "10", "10 + "),
                new(NumbersAndOperatorsEnum.Two, "2", "10 + "),
                new(NumbersAndOperatorsEnum.Equals, "12", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[2], standardItemsNew, false);

            // Programmer radix should still be hex: A+1=B
            var programmerItemsNew = new TestItem[]
            {
                new(NumbersAndOperatorsEnum.A, "A", ""),
                new(NumbersAndOperatorsEnum.Add, "A", "A + "),
                new(NumbersAndOperatorsEnum.One, "1", ""),
                new(NumbersAndOperatorsEnum.Equals, "B", ""),
                new(NumbersAndOperatorsEnum.None, "", "")
            };
            ValidateViewModelByCommands(viewModels[1], programmerItemsNew, true);
            Assert.AreEqual("B", viewModels[1].HexDisplayValue);
        }
    }
}
