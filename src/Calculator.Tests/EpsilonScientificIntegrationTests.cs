// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Globalization;
using System.Linq;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;

namespace Calculator.Tests
{
    [TestClass]
    public class EpsilonScientificIntegrationTests
    {
        private StandardCalculatorViewModel _viewModel;

        [TestInitialize]
        public void InitializeScientificViewModel()
        {
            _viewModel = new StandardCalculatorViewModel();
            Assert.IsTrue(
                _viewModel.IsCalculatorManagerInitialized,
                $"The native CalculatorManagerWrapper did not initialize: {_viewModel.CalculatorManagerInitializationError}");

            _viewModel.SetCalculatorType(ViewMode.Scientific);
            Assert.IsTrue(_viewModel.IsScientific);
        }

        [TestMethod]
        public void ScientificCapabilitiesComeFromNativeManager()
        {
            Assert.IsTrue(_viewModel.IsCommandSupported(NumbersAndOperatorsEnum.XPower2));
            Assert.IsTrue(_viewModel.IsCommandSupported(NumbersAndOperatorsEnum.EPowerX));
            Assert.IsTrue(_viewModel.IsCommandSupported(NumbersAndOperatorsEnum.Exp));
            Assert.IsFalse(_viewModel.IsCommandSupported(NumbersAndOperatorsEnum.XPowerY));
            Assert.IsFalse(_viewModel.IsCommandSupported(NumbersAndOperatorsEnum.Factorial));
            Assert.IsFalse(_viewModel.IsMemorySupported);
            Assert.IsTrue(_viewModel.IsHistoryReadOnly);
            Assert.IsFalse(_viewModel.HistoryVM.AreHistoryShortcutsEnabled);
        }

        [TestMethod]
        public void ScientificButtonCommandsHonorPrecedenceAndParentheses()
        {
            Press(
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Multiply,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("14", _viewModel.DisplayValue);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.OpenParenthesis,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.CloseParenthesis,
                NumbersAndOperatorsEnum.Multiply,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("20", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void ScientificEqualsCompletesIncompleteButtonAndPasteInputOnce()
        {
            Press(
                NumbersAndOperatorsEnum.OpenParenthesis,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("5", _viewModel.DisplayValue);

            Press(NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("5", _viewModel.DisplayValue);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("8", _viewModel.DisplayValue);

            _viewModel.OnPaste("(2+3");
            Assert.AreEqual("3", _viewModel.DisplayValue);
            Press(NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("5", _viewModel.DisplayValue);

            _viewModel.OnPaste("4+");
            Assert.AreEqual("4", _viewModel.DisplayValue);
            Press(NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("8", _viewModel.DisplayValue);
            Press(NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("8", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void ScientificUnaryAndErrorCallbacksReachViewModel()
        {
            Press(NumbersAndOperatorsEnum.Nine, NumbersAndOperatorsEnum.Sqrt);
            Assert.AreEqual("3", _viewModel.DisplayValue);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Zero,
                NumbersAndOperatorsEnum.Sin);
            Assert.AreEqual(0.5, ParseDisplay(), 1e-14);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Divide,
                NumbersAndOperatorsEnum.Zero,
                NumbersAndOperatorsEnum.Equals);
            Assert.IsTrue(_viewModel.IsInError, "Divide-by-zero did not reach the managed error callback.");

            Press(NumbersAndOperatorsEnum.Clear);
            Assert.IsFalse(_viewModel.IsInError);
            Assert.AreEqual("0", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void ScientificExponentialAndLogCommandsUseDistinctSupportedPaths()
        {
            Press(NumbersAndOperatorsEnum.FToE);
            Assert.AreEqual("0.e+0", _viewModel.DisplayValue);
            Press(NumbersAndOperatorsEnum.FToE, NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Zero,
                NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.FToE);
            Assert.AreEqual("1.e+3", _viewModel.DisplayValue);
            Press(NumbersAndOperatorsEnum.FToE, NumbersAndOperatorsEnum.Clear);

            Press(NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.EPowerX);
            Assert.AreEqual("1", _viewModel.DisplayValue);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Exp,
                NumbersAndOperatorsEnum.Negate,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual(0.001, ParseDisplay(), 1e-18);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Zero,
                NumbersAndOperatorsEnum.Zero,
                NumbersAndOperatorsEnum.LogBase10);
            Assert.AreEqual("2", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void UnsupportedButtonAndPastePreserveCurrentInput()
        {
            Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two);
            string inputBeforeRejection = _viewModel.DisplayValue;

            Press(NumbersAndOperatorsEnum.XPowerY);
            Assert.AreEqual(inputBeforeRejection, _viewModel.DisplayValue);
            Assert.IsTrue(NarratorAnnouncement.IsValid(_viewModel.Announcement));

            _viewModel.OnPaste("2^3");
            Assert.AreEqual(inputBeforeRejection, _viewModel.DisplayValue);
            Assert.IsTrue(NarratorAnnouncement.IsValid(_viewModel.Announcement));

            foreach (string invalidInput in new[] { "sin(30)", "2xyz3", "   ", new string('9', (int)CopyPasteManager.MaxPasteableLength + 1) })
            {
                _viewModel.OnPaste(invalidInput);
                Assert.AreEqual(inputBeforeRejection, _viewModel.DisplayValue);
                Assert.IsTrue(NarratorAnnouncement.IsValid(_viewModel.Announcement));
            }

            _viewModel.SendCommandToCalcManager((int)NumbersAndOperatorsEnum.Factorial);
            Assert.AreEqual(inputBeforeRejection, _viewModel.DisplayValue);
        }

        [TestMethod]
        public void ScientificHistoryIsAppendOnlyAndUsesDisplayTokens()
        {
            Press(
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Equals);

            Assert.AreEqual(1, _viewModel.HistoryVM.ItemsCount);
            HistoryItemViewModel item = _viewModel.HistoryVM.Items[0];
            Assert.AreEqual("5", item.Result);
            Assert.AreEqual(0, item.GetCommands().Count);
            Assert.IsTrue(item.GetTokens().All(token => token.CommandIndex == -1));

            _viewModel.HistoryVM.ShowItem(item);
            _viewModel.HistoryVM.DeleteItem(item);
            _viewModel.HistoryVM.ClearCommand.Execute(null);
            Assert.AreEqual(1, _viewModel.HistoryVM.ItemsCount);
            Assert.AreEqual("5", _viewModel.DisplayValue);
            Assert.IsTrue(NarratorAnnouncement.IsValid(_viewModel.HistoryVM.HistoryAnnouncement));
        }

        [TestMethod]
        public void ScientificSnapshotDoesNotPersistLegacyReplayCommands()
        {
            Press(
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Equals);

            var snapshot = _viewModel.Snapshot;
            Assert.AreEqual(0, snapshot.DisplayCommands.Count);
            Assert.IsNull(snapshot.ExpressionDisplay);
            Assert.AreEqual(0, snapshot.CalcManager.HistoryItems[0].Commands.Count);
            Assert.IsTrue(snapshot.CalcManager.HistoryItems[0].Tokens.All(token => token.CommandIndex == -1));
            snapshot.CalcManager.HistoryItems[0].Commands = null;

            var restored = new StandardCalculatorViewModel();
            Assert.IsTrue(restored.IsCalculatorManagerInitialized);
            restored.SetCalculatorType(ViewMode.Standard);
            restored.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Four);
            restored.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Two);
            restored.OnMemoryButtonPressed();
            restored.SetCalculatorType(ViewMode.Scientific);
            restored.Snapshot = snapshot;
            restored.HistoryVM.ReloadHistory(ViewMode.Scientific);

            Assert.AreEqual("0", restored.DisplayValue);
            Assert.AreEqual(1, restored.HistoryVM.ItemsCount);
            Assert.AreEqual(0, restored.HistoryVM.Items[0].GetCommands().Count);
            Assert.AreEqual("5", restored.HistoryVM.Items[0].Result);
            restored.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Add);
            restored.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            restored.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("1", restored.DisplayValue, "The live result must match the native retained value.");
            restored.SetCalculatorType(ViewMode.Standard);
            restored.OnMemoryItemPressed(0);
            Assert.AreEqual("42", restored.DisplayValue, "Scientific snapshot restoration must not clear legacy memory.");
        }

        [TestMethod]
        public void ScientificClearEntryAndConstantsStaySynchronized()
        {
            Press(
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.ClearEntry);
            Assert.AreEqual("0", _viewModel.DisplayValue);
            Assert.IsTrue(_viewModel.IsInputEmpty, "CE must reveal the existing C control.");
            Press(NumbersAndOperatorsEnum.Five, NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("17", _viewModel.DisplayValue);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.Pi,
                NumbersAndOperatorsEnum.ClearEntry,
                NumbersAndOperatorsEnum.Euler);
            Assert.AreEqual("2.7182818284590452353602874713527", _viewModel.DisplayValue);

            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.OpenParenthesis,
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.ClearEntry);
            Assert.IsTrue(_viewModel.IsInputEmpty);
            Press(
                NumbersAndOperatorsEnum.Clear,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("5", _viewModel.DisplayValue);
            Assert.AreEqual(0u, _viewModel.OpenParenthesisCount);
        }

        [TestMethod]
        public void ScientificLiveGroupingDoesNotBecomeCalculationInput()
        {
            Press(NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four, NumbersAndOperatorsEnum.Five, NumbersAndOperatorsEnum.Six,
                NumbersAndOperatorsEnum.Seven);
            Assert.AreEqual("1,234,567", _viewModel.DisplayValue);
            Press(NumbersAndOperatorsEnum.Backspace, NumbersAndOperatorsEnum.Add, NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("123,457", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void RawModeCommandsKeepManagedAndNativeModesAligned()
        {
            _viewModel.SendCommandToCalcManager((int)NumbersAndOperatorsEnum.IsProgrammerMode);
            Assert.IsTrue(_viewModel.IsProgrammer);
            Assert.IsFalse(_viewModel.IsScientific);
            Assert.IsTrue(_viewModel.IsMemorySupported);

            _viewModel.SendCommandToCalcManager((int)NumbersAndOperatorsEnum.IsScientificMode);
            Assert.IsTrue(_viewModel.IsScientific);
            Assert.IsFalse(_viewModel.IsProgrammer);
            Assert.IsFalse(_viewModel.IsMemorySupported);
            Press(
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Multiply,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("14", _viewModel.DisplayValue);

            _viewModel.SendCommandToCalcManager((int)NumbersAndOperatorsEnum.IsStandardMode);
            Assert.IsTrue(_viewModel.IsStandard);
            Assert.IsTrue(_viewModel.IsMemorySupported);
            Press(
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Add,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Multiply,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Equals);
            Assert.AreEqual("20", _viewModel.DisplayValue);
        }

        [TestMethod]
        public void ScientificMemoryIsSuppressedWithoutLosingLegacyMemory()
        {
            _viewModel.SetCalculatorType(ViewMode.Standard);
            Press(NumbersAndOperatorsEnum.Clear, NumbersAndOperatorsEnum.Four, NumbersAndOperatorsEnum.Two);
            _viewModel.OnMemoryButtonPressed();
            Assert.AreEqual(1, _viewModel.MemorizedNumbers.Count);
            string savedValue = _viewModel.MemorizedNumbers[0].Value;

            _viewModel.SetCalculatorType(ViewMode.Scientific);
            Assert.IsFalse(_viewModel.IsMemorySupported);
            BitLength bitLengthBeforeRejection = _viewModel.ValueBitLength;
            _viewModel.ValueBitLength = BitLength.BitLengthDWord;
            Assert.AreEqual(bitLengthBeforeRejection, _viewModel.ValueBitLength);
            _viewModel.OnMemoryButtonPressed();
            _viewModel.OnMemoryAdd(0);
            _viewModel.OnMemorySubtract(0);
            _viewModel.OnMemoryItemPressed(0);
            _viewModel.OnMemoryClear(0);
            _viewModel.ClearMemoryCommand.Execute(null);
            Assert.AreEqual(1, _viewModel.MemorizedNumbers.Count);
            Assert.AreEqual(savedValue, _viewModel.MemorizedNumbers[0].Value);

            _viewModel.SetCalculatorType(ViewMode.Standard);
            Assert.IsTrue(_viewModel.IsMemorySupported);
            _viewModel.OnMemoryItemPressed(0);
            Assert.AreEqual("42", _viewModel.DisplayValue);

            _viewModel.SetCalculatorType(ViewMode.Programmer);
            Assert.IsTrue(_viewModel.IsMemorySupported);
        }

        private void Press(params NumbersAndOperatorsEnum[] commands)
        {
            foreach (NumbersAndOperatorsEnum command in commands)
            {
                _viewModel.ButtonPressedCommand.Execute(command);
            }
        }

        private double ParseDisplay()
        {
            string value = LocalizationSettings.GetInstance().RemoveGroupSeparators(_viewModel.DisplayValue);
            value = LocalizationSettings.GetInstance().GetEnglishValueFromLocalizedDigits(value);
            return double.Parse(value, CultureInfo.InvariantCulture);
        }
    }
}
