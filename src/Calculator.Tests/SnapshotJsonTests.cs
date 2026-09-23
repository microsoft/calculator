// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Linq;
using System.Text.Json;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalcManager.Interop;
using CalculatorApp;
using CalculatorApp.JsonUtils;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Snapshot;

namespace Calculator.Tests
{
    // Snapshot JSON is the untrusted Recall activation boundary.
    [TestClass]
    public class SnapshotJsonTests
    {
        [DataTestMethod]
        [DataRow(-1)]
        [DataRow(int.MaxValue)]
        public void InvalidModeSetsLaunchError(int mode)
        {
            var alias = new ApplicationSnapshotAlias();
            alias.Mode = mode;

            var result = SnapshotLaunchArguments.FromJson(JsonSerializer.Serialize(alias));

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow((int)ViewMode.Standard)]
        [DataRow((int)ViewMode.Scientific)]
        [DataRow((int)ViewMode.Programmer)]
        public void CalculatorModeWithoutStateSetsLaunchError(int mode)
        {
            var alias = new ApplicationSnapshotAlias();
            alias.Mode = mode;

            var result = SnapshotLaunchArguments.FromJson(JsonSerializer.Serialize(alias));

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow(-2)]
        [DataRow(1)]
        public void InvalidHistoryTokenIndexSetsLaunchError(int commandIndex)
        {
            var snapshot = CreateApplicationSnapshot();
            var item = new CalcManagerHistoryItem();
            item.Commands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));
            item.Tokens.Add(new CalcManagerToken("+", commandIndex));
            snapshot.StandardCalculator.CalcManager.HistoryItems = new List<CalcManagerHistoryItem> { item };

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow(-2)]
        [DataRow(1)]
        public void InvalidExpressionTokenIndexSetsLaunchError(int commandIndex)
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.ExpressionDisplay = new ExpressionDisplaySnapshot();
            snapshot.StandardCalculator.ExpressionDisplay.Commands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));
            snapshot.StandardCalculator.ExpressionDisplay.Tokens.Add(new CalcManagerToken("+", commandIndex));

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [TestMethod]
        public void NullPrimaryDisplayValueSetsLaunchError()
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.PrimaryDisplay.DisplayValue = null;

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [DataTestMethod]
        [DataRow((int)CalculatorCommand.ModeProgrammer)]
        [DataRow(int.MaxValue)]
        public void InvalidDisplayCommandSetsLaunchError(int command)
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand,
                command,
                System.Array.Empty<int>(),
                false,
                false,
                false));

            var result = ParseSnapshot(snapshot);

            Assert.IsTrue(result.HasError);
            Assert.IsNull(result.Snapshot);
        }

        [TestMethod]
        public void ValidDisplayCommandsAreAccepted()
        {
            var snapshot = CreateApplicationSnapshot();
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.UnaryCommand,
                0,
                new[] { (int)NumbersAndOperatorsEnum.Degree, (int)CalculatorCommand.CommandSIN },
                false,
                false,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.BinaryCommand,
                (int)CalculatorCommand.CommandADD,
                System.Array.Empty<int>(),
                false,
                false,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[]
                {
                    (int)CalculatorCommand.Command1,
                    (int)CalculatorCommand.CommandPNT,
                    (int)CalculatorCommand.Command2
                },
                false,
                true,
                false));
            snapshot.StandardCalculator.DisplayCommands.Add(new ExpressionCommandWrapper(
                CommandType.Parentheses,
                (int)CalculatorCommand.CommandOPENP,
                System.Array.Empty<int>(),
                false,
                false,
                false));

            var result = ParseSnapshot(snapshot);

            Assert.IsFalse(result.HasError);
            Assert.IsNotNull(result.Snapshot);
        }

        [TestMethod]
        public void UnaryCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.UnaryCommand, 0, new[] { 91, 92 }, false, false, false));

            Assert.AreEqual(CommandType.UnaryCommand, restored.Type);
            CollectionAssert.AreEqual(new[] { 91, 92 }, restored.Commands);
        }

        [TestMethod]
        public void BinaryCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.BinaryCommand, 93, System.Array.Empty<int>(), false, false, false));

            Assert.AreEqual(CommandType.BinaryCommand, restored.Type);
            Assert.AreEqual(93, restored.Command);
        }

        [TestMethod]
        public void ParenthesesCommandSurvivesTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.Parentheses, 106, System.Array.Empty<int>(), false, false, false));

            Assert.AreEqual(CommandType.Parentheses, restored.Type);
            Assert.AreEqual(106, restored.Command);
        }

        [TestMethod]
        public void OperandCommandCarriesItsFlagsThroughTheRoundTrip()
        {
            var restored = RoundTrip(new ExpressionCommandWrapper(
                CommandType.OperandCommand,
                0,
                new[] { 131, 132 },
                isNegative: true,
                isDecimalPresent: true,
                isSciFmt: true));

            Assert.AreEqual(CommandType.OperandCommand, restored.Type);
            CollectionAssert.AreEqual(new[] { 131, 132 }, restored.Commands);
            Assert.IsTrue(restored.IsNegative, "IsNegative was lost in the round trip.");
            Assert.IsTrue(restored.IsDecimalPresent, "IsDecimalPresent was lost in the round trip.");
            Assert.IsTrue(restored.IsSciFmt, "IsSciFmt was lost in the round trip.");
        }

        [TestMethod]
        public void MalformedUnaryCommandIsRejectedDuringDeserialization()
        {
            Assert.ThrowsException<System.ArgumentOutOfRangeException>(
                () => Helpers.MapCommandAlias(new UnaryCommandAlias()),
                "A unary command with no sub-commands has to be rejected at the JSON boundary.");
        }

        [TestMethod]
        public void TokenSurvivesTheRoundTrip()
        {
            var token = new CalculatorApp.ViewModel.Snapshot.CalcManagerToken("+", 3);

            var json = JsonSerializer.Serialize(new CalcManagerTokenAlias(token));
            var restored = Helpers.MapToken(JsonSerializer.Deserialize<CalcManagerTokenAlias>(json));

            Assert.AreEqual("+", restored.OpCodeName);
            Assert.AreEqual(3, restored.CommandIndex);
        }

        // Serializes through the alias exactly as the activation URI does, then maps back.
        private static ExpressionCommandWrapper RoundTrip(ExpressionCommandWrapper command)
        {
            ICalcManagerIExprCommandAlias alias = Helpers.MapCommandAlias(command);
            string json = JsonSerializer.Serialize(alias);
            var deserialized = JsonSerializer.Deserialize<ICalcManagerIExprCommandAlias>(json);
            return Helpers.MapCommandAlias(deserialized);
        }

        private static ApplicationSnapshot CreateApplicationSnapshot()
        {
            return new ApplicationSnapshot
            {
                Mode = (int)ViewMode.Standard,
                StandardCalculator = new StandardCalculatorSnapshot()
            };
        }

        private static SnapshotLaunchArguments ParseSnapshot(ApplicationSnapshot snapshot)
        {
            return SnapshotLaunchArguments.FromJson(
                JsonSerializer.Serialize(new ApplicationSnapshotAlias(snapshot)));
        }
    }
}
