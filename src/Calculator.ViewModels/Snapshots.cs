// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using CalcManager.Interop;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel.Snapshot
{
    public sealed class CalcManagerToken
    {
        public string OpCodeName { get; set; }
        public int CommandIndex { get; set; }

        public CalcManagerToken()
        {
            OpCodeName = string.Empty;
            CommandIndex = 0;
        }

        internal CalcManagerToken(string opCodeName, int cmdIndex)
        {
            OpCodeName = opCodeName ?? throw new ArgumentNullException(nameof(opCodeName));
            CommandIndex = cmdIndex;
        }
    }

    public sealed class CalcManagerHistoryItem
    {
        public IList<CalcManagerToken> Tokens { get; set; }
        public IList<ExpressionCommandWrapper> Commands { get; set; }
        public string Expression { get; set; }
        public string Result { get; set; }

        public CalcManagerHistoryItem()
        {
            Tokens = new List<CalcManagerToken>();
            Commands = new List<ExpressionCommandWrapper>();
            Expression = string.Empty;
            Result = string.Empty;
        }
    }

    public sealed class CalcManagerSnapshot
    {
        public IList<CalcManagerHistoryItem> HistoryItems { get; set; }

        public CalcManagerSnapshot()
        {
            HistoryItems = null;
        }
    }

    public sealed class PrimaryDisplaySnapshot
    {
        public string DisplayValue { get; set; }
        public bool IsError { get; set; }

        public PrimaryDisplaySnapshot()
        {
            DisplayValue = string.Empty;
            IsError = false;
        }

        internal PrimaryDisplaySnapshot(string display, bool isError)
        {
            DisplayValue = display ?? string.Empty;
            IsError = isError;
        }
    }

    public sealed class ExpressionDisplaySnapshot
    {
        public IList<CalcManagerToken> Tokens { get; set; }
        public IList<ExpressionCommandWrapper> Commands { get; set; }

        public ExpressionDisplaySnapshot()
        {
            Tokens = new List<CalcManagerToken>();
            Commands = new List<ExpressionCommandWrapper>();
        }
    }

    public sealed class StandardCalculatorSnapshot
    {
        public CalcManagerSnapshot CalcManager { get; set; }
        public PrimaryDisplaySnapshot PrimaryDisplay { get; set; }
        public ExpressionDisplaySnapshot ExpressionDisplay { get; set; }
        public IList<ExpressionCommandWrapper> DisplayCommands { get; set; }

        public StandardCalculatorSnapshot()
        {
            CalcManager = new CalcManagerSnapshot();
            PrimaryDisplay = new PrimaryDisplaySnapshot();
            ExpressionDisplay = null;
            DisplayCommands = new List<ExpressionCommandWrapper>();
        }
    }

    public sealed class ApplicationSnapshot
    {
        public int Mode { get; set; }
        public StandardCalculatorSnapshot StandardCalculator { get; set; }
    }

    internal static class SnapshotValidator
    {
        internal static void Validate(ApplicationSnapshot snapshot)
        {
            if (snapshot == null)
            {
                throw new ArgumentNullException(nameof(snapshot));
            }

            var mode = (ViewMode)snapshot.Mode;
            if (!NavCategoryStates.IsValidViewMode(mode) || !NavCategoryStates.IsViewModeEnabled(mode))
            {
                throw new ArgumentOutOfRangeException(nameof(snapshot), snapshot.Mode, "Invalid calculator mode.");
            }

            if (NavCategory.IsCalculatorViewMode(mode) && snapshot.StandardCalculator == null)
            {
                throw new ArgumentException("Calculator snapshot state is missing.", nameof(snapshot));
            }

            if (snapshot.StandardCalculator != null
                && (snapshot.StandardCalculator.PrimaryDisplay == null
                    || snapshot.StandardCalculator.PrimaryDisplay.DisplayValue == null))
            {
                throw new ArgumentException("Primary display state is missing.", nameof(snapshot));
            }

            var historyItems = snapshot.StandardCalculator?.CalcManager?.HistoryItems;
            if (historyItems != null)
            {
                for (int i = 0; i < historyItems.Count; i++)
                {
                    var item = historyItems[i]
                        ?? throw new ArgumentException($"History item {i} is null.", nameof(snapshot));
                    ValidateTokenIndexes(item.Tokens, item.Commands, $"history item {i}");
                }
            }

            var expression = snapshot.StandardCalculator?.ExpressionDisplay;
            if (expression != null)
            {
                ValidateTokenIndexes(expression.Tokens, expression.Commands, "expression");
            }
        }

        internal static void ValidateProtocol(ApplicationSnapshot snapshot)
        {
            Validate(snapshot);

            var historyItems = snapshot.StandardCalculator?.CalcManager?.HistoryItems;
            if (historyItems != null)
            {
                for (int i = 0; i < historyItems.Count; i++)
                {
                    ValidateCommands(historyItems[i].Commands, $"history item {i}");
                }
            }

            var expression = snapshot.StandardCalculator?.ExpressionDisplay;
            if (expression != null)
            {
                ValidateCommands(expression.Commands, "expression");
            }

            ValidateCommands(snapshot.StandardCalculator?.DisplayCommands, "display");
        }

        private static void ValidateTokenIndexes(
            IList<CalcManagerToken> tokens,
            IList<ExpressionCommandWrapper> commands,
            string location)
        {
            if (tokens == null || commands == null)
            {
                throw new ArgumentException($"{location} has no token or command list.");
            }

            for (int i = 0; i < tokens.Count; i++)
            {
                var token = tokens[i]
                    ?? throw new ArgumentException($"{location} token {i} is null.");
                if (token.CommandIndex < -1 || token.CommandIndex >= commands.Count)
                {
                    throw new ArgumentOutOfRangeException(
                        nameof(token.CommandIndex),
                        token.CommandIndex,
                        $"{location} token {i} does not reference a command.");
                }
            }
        }

        private static void ValidateCommands(
            IList<ExpressionCommandWrapper> commands,
            string location)
        {
            if (commands == null)
            {
                return;
            }

            for (int i = 0; i < commands.Count; i++)
            {
                var command = commands[i]
                    ?? throw new ArgumentException($"{location} command {i} is null.");

                bool isValid;
                switch (command.Type)
                {
                    case CommandType.UnaryCommand:
                        isValid = IsValidUnaryCommand(command.Commands);
                        break;
                    case CommandType.BinaryCommand:
                        isValid = IsValidBinaryCommand(command.Command);
                        break;
                    case CommandType.OperandCommand:
                        isValid = IsValidOperandCommand(command.Commands);
                        break;
                    case CommandType.Parentheses:
                        isValid = command.Command == (int)CalculatorCommand.CommandOPENP
                            || command.Command == (int)CalculatorCommand.CommandCLOSEP;
                        break;
                    default:
                        isValid = false;
                        break;
                }

                if (!isValid)
                {
                    throw new ArgumentException($"{location} command {i} is invalid.");
                }
            }
        }

        private static bool IsValidUnaryCommand(IReadOnlyList<int> commands)
        {
            if (commands == null || commands.Count < 1 || commands.Count > 2)
            {
                return false;
            }

            if (commands.Count == 2)
            {
                return IsAngleCommand(commands[0]) && IsUnaryOperator(commands[1]);
            }

            return IsUnaryOperator(commands[0]);
        }

        private static bool IsUnaryOperator(int command)
        {
            return command == (int)CalculatorCommand.CommandSIGN
                || (command >= (int)CalculatorCommand.CommandCHOP
                    && command <= (int)CalculatorCommand.CommandPERCENT)
                || (command >= (int)CalculatorCommand.CommandASIN
                    && command <= (int)CalculatorCommand.CommandATANH)
                || command == (int)NumbersAndOperatorsEnum.Degrees
                || (command >= (int)CalculatorCommand.CommandSEC
                    && command <= (int)CalculatorCommand.CommandRORC);
        }

        private static bool IsAngleCommand(int command)
        {
            return command >= (int)NumbersAndOperatorsEnum.Degree
                && command <= (int)NumbersAndOperatorsEnum.Grads;
        }

        private static bool IsValidBinaryCommand(int command)
        {
            return (command >= (int)CalculatorCommand.CommandAnd
                    && command <= (int)CalculatorCommand.CommandPWR)
                || (command >= (int)CalculatorCommand.CommandLogBaseY
                    && command <= (int)CalculatorCommand.CommandNor)
                || command == (int)CalculatorCommand.CommandRSHFL;
        }

        private static bool IsValidOperandCommand(IReadOnlyList<int> commands)
        {
            if (commands == null || commands.Count == 0)
            {
                return false;
            }

            for (int i = 0; i < commands.Count; i++)
            {
                int command = commands[i];
                if (command != (int)CalculatorCommand.CommandSIGN
                    && command != (int)CalculatorCommand.CommandPNT
                    && command != (int)CalculatorCommand.CommandEXP
                    && (command < (int)CalculatorCommand.Command0
                        || command > (int)CalculatorCommand.CommandF))
                {
                    return false;
                }
            }

            return true;
        }
    }
}
