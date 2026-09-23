// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalcManager.Interop;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Snapshot;

namespace Calculator.Tests
{
    [TestClass]
    public class SnapshotRoundTripTests
    {
        private const int Command0 = 130;
        private const int Command1 = 131;
        private const int Command2 = 132;
        private const int Command3 = 133;
        private const int Command5 = 135;
        private const int Command9 = 139;
        private const int CommandCLEAR = 81;
        private const int CommandPNT = 84;
        private const int CommandDIV = 91;
        private const int CommandADD = 93;
        private const int CommandMUL = 92;
        private const int CommandSIN = 102;
        private const int CommandEQU = 121;
        private const int ModeBasic = 200;
        private const int ModeScientific = 201;
        private const int ModeProgrammer = 209;

        [TestMethod]
        public void InvalidSnapshotModeDoesNotReplaceCurrentMode()
        {
            var viewModel = new ApplicationViewModel();
            viewModel.Initialize(ViewMode.Standard);
            var snapshot = new ApplicationSnapshot { Mode = int.MaxValue };

            Assert.ThrowsException<ArgumentOutOfRangeException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            Assert.AreEqual(ViewMode.Standard, viewModel.Mode);
        }

        [TestMethod]
        public void NonCalculatorSnapshotIgnoresStaleCalculatorState()
        {
            var viewModel = new ApplicationViewModel();
            SetModeWithoutNavigation(viewModel, ViewMode.Date);
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Date,
                StandardCalculator = CreateStandardViewModel().Snapshot
            };

            viewModel.RestoreFromSnapshot(snapshot);

            Assert.AreEqual(ViewMode.Date, viewModel.Mode);
            Assert.IsNull(viewModel.CalculatorViewModel);
        }

        [TestMethod]
        public void NonCalculatorSnapshotDoesNotCaptureStaleCalculatorState()
        {
            var viewModel = new ApplicationViewModel
            {
                CalculatorViewModel = CreateStandardViewModel()
            };
            SetModeWithoutNavigation(viewModel, ViewMode.Date);

            var snapshot = viewModel.Snapshot;

            Assert.AreEqual((int)ViewMode.Date, snapshot.Mode);
            Assert.IsNull(snapshot.StandardCalculator);
        }

        [TestMethod]
        public void SnapshotRestoresHistory()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            Evaluate(source, Command3, CommandMUL, Command3);

            var captured = source.Snapshot;

            Assert.IsNotNull(
                captured.CalcManager.HistoryItems,
                "No history was captured, so resuming from Recall would come back with an empty history list.");
            Assert.AreEqual(2, captured.CalcManager.HistoryItems.Count);

            StandardCalculatorViewModel restored = CreateStandardViewModel();
            restored.Snapshot = captured;

            var recaptured = restored.Snapshot;
            Assert.IsNotNull(recaptured.CalcManager.HistoryItems, "The restored calculator captured no history.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Expression).ToArray(),
                recaptured.CalcManager.HistoryItems.Select(item => item.Expression).ToArray(),
                "Expressions did not survive the round trip.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Result).ToArray(),
                recaptured.CalcManager.HistoryItems.Select(item => item.Result).ToArray(),
                "Results did not survive the round trip.");

            // Display history is newest-first while the snapshot is oldest-first.
            var items = restored.HistoryVM.Items;
            Assert.AreEqual(2, items.Count, "The restored calculator did not get its history back.");
            CollectionAssert.AreEqual(
                captured.CalcManager.HistoryItems.Select(item => item.Result).Reverse().ToArray(),
                items.Select(item => item.Result).ToArray(),
                "The restored history list does not show what was captured.");
        }

        [TestMethod]
        public void CapturedHistoryCarriesItsTokensAndCommands()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);

            var item = source.Snapshot.CalcManager.HistoryItems.Single();

            Assert.IsTrue(item.Tokens.Count > 0, "The captured history item carried no tokens.");
            Assert.IsTrue(item.Commands.Count > 0, "The captured history item carried no commands.");
        }

        [TestMethod]
        public void ExpressionCommandWrapperCarriesTheOperandFlags()
        {
            var command = new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[] { Command1, Command2 },
                isNegative: true,
                isDecimalPresent: true,
                isSciFmt: true);

            Assert.AreEqual(CommandType.OperandCommand, command.Type);
            CollectionAssert.AreEqual(new[] { Command1, Command2 }, command.Commands);
            Assert.IsTrue(command.IsNegative, "IsNegative did not survive.");
            Assert.IsTrue(command.IsDecimalPresent, "IsDecimalPresent did not survive.");
            Assert.IsTrue(command.IsSciFmt, "IsSciFmt did not survive.");
        }

        [TestMethod]
        public void HistoryItemWrapperIsConstructibleFromManagedData()
        {
            var token = new HistoryToken { Value = "1", CommandIndex = 0 };
            var command = new ExpressionCommandWrapper(
                CommandType.BinaryCommand, CommandADD, System.Array.Empty<int>(), false, false, false);

            var item = new HistoryItemWrapper(new[] { token }, new[] { command }, "1 + 2 =", "3");

            Assert.AreEqual("1 + 2 =", item.Expression);
            Assert.AreEqual("3", item.Result);
            Assert.AreEqual(1, item.Tokens.Length);
            Assert.AreEqual("1", item.Tokens[0].Value);
            Assert.AreEqual(1, item.Commands.Length);
            Assert.AreEqual(CommandADD, item.Commands[0].Command);
        }

        // A recalled session must not inherit memory from the current one.
        [TestMethod]
        public void RestoringASnapshotClearsMemory()
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var captured = source.Snapshot;

            StandardCalculatorViewModel restored = CreateStandardViewModel();
            Evaluate(restored, Command3);
            restored.OnMemoryButtonPressed();
            Assert.IsTrue(restored.IsMemoryEmpty == false, "Memory was not set up, so the test proves nothing.");

            restored.Snapshot = captured;

            Assert.IsTrue(restored.IsMemoryEmpty, "Restoring a snapshot left the previous session's memory behind.");
        }

        [TestMethod]
        public void RestoringAMalformedHistoryCommandThrowsInsteadOfCrashing()
        {
            var malformed = new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false);
            var item = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem
            {
                Expression = "1 + 2 =",
                Result = "3"
            };
            item.Commands.Add(malformed);

            var snapshot = CreateStandardViewModel().Snapshot;
            snapshot.CalcManager.HistoryItems = new List<CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem> { item };

            StandardCalculatorViewModel target = CreateStandardViewModel();

            Assert.ThrowsException<ArgumentException>(
                () => target.Snapshot = snapshot,
                "A malformed unary command has to be rejected with a catchable exception.");
        }

        [TestMethod]
        public void FailedRestoreResetsTheBoundCalculator()
        {
            var viewModel = new ApplicationViewModel();
            viewModel.Initialize(ViewMode.Standard);
            var originalCalculator = CreateStandardViewModel();
            viewModel.CalculatorViewModel = originalCalculator;
            Evaluate(originalCalculator, Command3);
            originalCalculator.OnMemoryButtonPressed();

            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var standardSnapshot = source.Snapshot;
            var malformed = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem();
            malformed.Commands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false));
            standardSnapshot.CalcManager.HistoryItems.Add(malformed);
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Standard,
                StandardCalculator = standardSnapshot
            };

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            Assert.AreEqual(ViewMode.Standard, viewModel.Mode);
            Assert.AreSame(originalCalculator, viewModel.CalculatorViewModel);
            Assert.AreEqual("0", viewModel.CalculatorViewModel.DisplayValue);
            Assert.IsTrue(viewModel.CalculatorViewModel.IsMemoryEmpty);
            Assert.AreEqual(0, viewModel.CalculatorViewModel.HistoryVM.Items.Count);
            Assert.IsNull(viewModel.CalculatorViewModel.Snapshot.ExpressionDisplay);
            Assert.AreEqual(0, viewModel.CalculatorViewModel.Snapshot.DisplayCommands.Count);
        }

        [TestMethod]
        public void FailedCrossModeRestoreClearsSnapshotModeHistory()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Standard);
            var viewModel = CreateApplicationViewModel(ViewMode.Standard, calculator);
            StandardCalculatorViewModel source = CreateCalculatorViewModel(ViewMode.Scientific);
            Evaluate(source, Command1, CommandADD, Command2);
            var standardSnapshot = source.Snapshot;
            standardSnapshot.DisplayCommands = new List<ExpressionCommandWrapper> { null };
            var snapshot = new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Scientific,
                StandardCalculator = standardSnapshot
            };

            Assert.ThrowsException<NullReferenceException>(
                () => viewModel.RestoreFromSnapshot(snapshot));

            calculator.SendCommandToCalcManager(ModeScientific);
            calculator.HistoryVM.ReloadHistory(ViewMode.Scientific);
            Assert.AreEqual(0, calculator.HistoryVM.Items.Count);
        }

        [TestMethod]
        public void FailedScientificRestoreKeepsTheScientificEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            AssertScientificOrderOfOperations(calculator);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Assert.AreEqual(NumbersAndOperatorsEnum.Radians, calculator.GetCurrentAngleType());
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Scientific)));

            Assert.AreEqual(NumbersAndOperatorsEnum.Degree, calculator.GetCurrentAngleType());
            AssertScientificOrderOfOperations(calculator);
        }

        [TestMethod]
        public void FailedScientificRestoreResetsErroredEngineAngleMode()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Evaluate(calculator, Command1, CommandDIV, Command0);
            Assert.IsTrue(calculator.IsInError);
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Scientific)));

            Evaluate(calculator, Command9, Command0, CommandSIN);
            Assert.AreEqual("1", calculator.DisplayValue);
        }

        [TestMethod]
        public void FailedProgrammerRestoreKeepsTheProgrammerEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            AssertProgrammerIgnoresDecimalPoint(calculator);
            calculator.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.AreEqual(NumberBase.HexBase, calculator.CurrentRadixType);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Programmer)));

            Assert.AreEqual(NumberBase.DecBase, calculator.CurrentRadixType);
            AssertProgrammerIgnoresDecimalPoint(calculator);
        }

        [TestMethod]
        public void FailedProgrammerRestoreResetsBitLength()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            calculator.ValueBitLength = BitLength.BitLengthByte;
            Evaluate(calculator, Command1, CommandDIV, Command0);
            Assert.IsTrue(calculator.IsInError);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            Assert.ThrowsException<ArgumentException>(
                () => viewModel.RestoreFromSnapshot(CreateMalformedSnapshot(ViewMode.Programmer)));

            Assert.AreEqual(BitLength.BitLengthQWord, calculator.ValueBitLength);
            Evaluate(calculator, Command2, Command5, Command5, CommandADD, Command1);
            Assert.AreEqual("256", calculator.DisplayValue);
        }

        [TestMethod]
        public void SuccessfulScientificRestoreKeepsTheScientificEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Scientific);
            calculator.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Radians);
            Assert.AreEqual(NumbersAndOperatorsEnum.Radians, calculator.GetCurrentAngleType());
            var viewModel = CreateApplicationViewModel(ViewMode.Scientific, calculator);

            viewModel.RestoreFromSnapshot(new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Scientific,
                StandardCalculator = CreateCalculatorViewModel(ViewMode.Scientific).Snapshot
            });

            Assert.AreEqual(NumbersAndOperatorsEnum.Degree, calculator.GetCurrentAngleType());
            AssertScientificOrderOfOperations(calculator);
        }

        [TestMethod]
        public void SuccessfulProgrammerRestoreKeepsTheProgrammerEngine()
        {
            StandardCalculatorViewModel calculator = CreateCalculatorViewModel(ViewMode.Programmer);
            calculator.SwitchProgrammerModeBase(NumberBase.HexBase);
            Assert.AreEqual(NumberBase.HexBase, calculator.CurrentRadixType);
            var viewModel = CreateApplicationViewModel(ViewMode.Programmer, calculator);

            viewModel.RestoreFromSnapshot(new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Programmer,
                StandardCalculator = CreateCalculatorViewModel(ViewMode.Programmer).Snapshot
            });

            Assert.AreEqual(NumberBase.DecBase, calculator.CurrentRadixType);
            AssertProgrammerIgnoresDecimalPoint(calculator);
        }

        private static StandardCalculatorViewModel CreateStandardViewModel()
        {
            var viewModel = new StandardCalculatorViewModel();
            viewModel.IsStandard = true;
            viewModel.SendCommandToCalcManager(ModeBasic);
            viewModel.HistoryVM.ClearCommand.Execute(null);
            return viewModel;
        }

        private static StandardCalculatorViewModel CreateCalculatorViewModel(ViewMode mode)
        {
            StandardCalculatorViewModel viewModel = CreateStandardViewModel();
            viewModel.SetCalculatorType(mode);
            viewModel.SendCommandToCalcManager(
                mode == ViewMode.Scientific ? ModeScientific : ModeProgrammer);
            return viewModel;
        }

        private static ApplicationViewModel CreateApplicationViewModel(
            ViewMode mode,
            StandardCalculatorViewModel calculator)
        {
            var viewModel = new ApplicationViewModel
            {
                CalculatorViewModel = calculator
            };
            SetModeWithoutNavigation(viewModel, mode);
            return viewModel;
        }

        private static ApplicationSnapshot CreateMalformedSnapshot(ViewMode mode)
        {
            StandardCalculatorViewModel source = CreateStandardViewModel();
            Evaluate(source, Command1, CommandADD, Command2);
            var snapshot = source.Snapshot;
            var malformed = new CalculatorApp.ViewModel.Snapshot.CalcManagerHistoryItem();
            malformed.Commands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, System.Array.Empty<int>(), false, false, false));
            snapshot.CalcManager.HistoryItems.Add(malformed);
            return new ApplicationSnapshot
            {
                Mode = (int)mode,
                StandardCalculator = snapshot
            };
        }

        private static void AssertScientificOrderOfOperations(StandardCalculatorViewModel viewModel)
        {
            viewModel.SendCommandToCalcManager(CommandCLEAR);
            Evaluate(viewModel, Command1, CommandADD, Command2, CommandMUL, Command3);
            Assert.AreEqual("7", viewModel.DisplayValue);
        }

        private static void AssertProgrammerIgnoresDecimalPoint(StandardCalculatorViewModel viewModel)
        {
            viewModel.SendCommandToCalcManager(CommandCLEAR);
            viewModel.SendCommandToCalcManager(Command1);
            viewModel.SendCommandToCalcManager(CommandPNT);
            viewModel.SendCommandToCalcManager(Command5);
            Assert.AreEqual("15", viewModel.DisplayValue);
        }

        private static void SetModeWithoutNavigation(ApplicationViewModel viewModel, ViewMode mode)
        {
            typeof(ApplicationViewModel)
                .GetField("_mode", BindingFlags.Instance | BindingFlags.NonPublic)
                .SetValue(viewModel, mode);
        }

        private static void Evaluate(StandardCalculatorViewModel viewModel, params int[] commands)
        {
            foreach (int command in commands)
            {
                viewModel.SendCommandToCalcManager(command);
            }

            viewModel.SendCommandToCalcManager(CommandEQU);
        }
    }
}
