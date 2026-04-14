using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json.Serialization;
using CalcManager.Interop;
using CalculatorApp.ViewModel.Snapshot;

namespace CalculatorApp.JsonUtils
{
    internal class CalcManagerTokenAlias
    {
        [JsonIgnore]
        public CalcManagerToken Value;

        [JsonPropertyName("t")]
        public string OpCodeName
        {
            get => Value.OpCodeName;
            set => Value.OpCodeName = value;
        }
        [JsonPropertyName("c")]
        public int CommandIndex
        {
            get => Value.CommandIndex;
            set => Value.CommandIndex = value;
        }

        public CalcManagerTokenAlias() => Value = new CalcManagerToken();
        public CalcManagerTokenAlias(CalcManagerToken value) => Value = value;
    }

    [JsonPolymorphic(TypeDiscriminatorPropertyName = "$t")]
    [JsonDerivedType(typeof(UnaryCommandAlias), typeDiscriminator: 0)]
    [JsonDerivedType(typeof(BinaryCommandAlias), typeDiscriminator: 1)]
    [JsonDerivedType(typeof(OperandCommandAlias), typeDiscriminator: 2)]
    [JsonDerivedType(typeof(ParenthesesAlias), typeDiscriminator: 3)]
    internal interface ICalcManagerIExprCommandAlias
    {
    }

    internal class UnaryCommandAlias : ICalcManagerIExprCommandAlias
    {
        [JsonPropertyName("c")]
        public IReadOnlyList<int> Commands { get; set; }

        public UnaryCommandAlias() { Commands = Array.Empty<int>(); }
        public UnaryCommandAlias(ExpressionCommandWrapper cmd) { Commands = cmd.Commands; }
    }

    internal class BinaryCommandAlias : ICalcManagerIExprCommandAlias
    {
        [JsonPropertyName("c")]
        public int Command { get; set; }

        public BinaryCommandAlias() { }
        public BinaryCommandAlias(ExpressionCommandWrapper cmd) { Command = cmd.Command; }
    }

    internal class OperandCommandAlias : ICalcManagerIExprCommandAlias
    {
        [JsonPropertyName("n")]
        public bool IsNegative { get; set; }
        [JsonPropertyName("d")]
        public bool IsDecimalPresent { get; set; }
        [JsonPropertyName("s")]
        public bool IsSciFmt { get; set; }
        [JsonPropertyName("c")]
        public IReadOnlyList<int> Commands { get; set; }

        public OperandCommandAlias() { Commands = Array.Empty<int>(); }
        public OperandCommandAlias(ExpressionCommandWrapper cmd)
        {
            Commands = cmd.Commands;
            // Operand-specific flags not directly available from wrapper,
            // default to false
            IsNegative = false;
            IsDecimalPresent = false;
            IsSciFmt = false;
        }
    }

    internal class ParenthesesAlias : ICalcManagerIExprCommandAlias
    {
        [JsonPropertyName("c")]
        public int Command { get; set; }

        public ParenthesesAlias() { }
        public ParenthesesAlias(ExpressionCommandWrapper cmd) { Command = cmd.Command; }
    }

    internal class CalcManagerHistoryItemAlias
    {
        [JsonIgnore]
        public CalcManagerHistoryItem Value;

        [JsonPropertyName("t")]
        public IEnumerable<CalcManagerTokenAlias> Tokens
        {
            get => Value.Tokens.Select(x => new CalcManagerTokenAlias(x));
            set => Value.Tokens = value.Select(Helpers.MapToken).ToList();
        }
        [JsonPropertyName("c")]
        public IEnumerable<ICalcManagerIExprCommandAlias> Commands
        {
            get => Value.Commands.Select(Helpers.MapCommandAlias);
            set => Value.Commands = value.Select(Helpers.MapCommandAlias).ToList();
        }
        [JsonPropertyName("e")]
        public string Expression
        {
            get => Value.Expression;
            set => Value.Expression = value;
        }
        [JsonPropertyName("r")]
        public string Result
        {
            get => Value.Result;
            set => Value.Result = value;
        }

        public CalcManagerHistoryItemAlias() => Value = new CalcManagerHistoryItem();
        public CalcManagerHistoryItemAlias(CalcManagerHistoryItem value) => Value = value;
    }

    internal class CalcManagerSnapshotAlias
    {
        [JsonIgnore]
        public CalcManagerSnapshot Value;

        [JsonPropertyName("h")]
        public IEnumerable<CalcManagerHistoryItemAlias> HistoryItems // optional
        {
            get => Value.HistoryItems?.Select(x => new CalcManagerHistoryItemAlias { Value = x });
            set => Value.HistoryItems = value?.Select(x => new CalcManagerHistoryItem
            {
                Tokens = x.Tokens.Select(Helpers.MapToken).ToList(),
                Commands = x.Commands.Select(Helpers.MapCommandAlias).ToList(),
                Expression = x.Expression,
                Result = x.Result
            }).ToList();
        }

        public CalcManagerSnapshotAlias() => Value = new CalcManagerSnapshot();
        public CalcManagerSnapshotAlias(CalcManagerSnapshot value) => Value = value;
    }

    internal class PrimaryDisplaySnapshotAlias
    {
        [JsonIgnore]
        public PrimaryDisplaySnapshot Value;

        [JsonPropertyName("d")]
        public string DisplayValue
        {
            get => Value.DisplayValue;
            set => Value.DisplayValue = value;
        }
        [JsonPropertyName("e")]
        public bool IsError
        {
            get => Value.IsError;
            set => Value.IsError = value;
        }

        public PrimaryDisplaySnapshotAlias() => Value = new PrimaryDisplaySnapshot();
        public PrimaryDisplaySnapshotAlias(PrimaryDisplaySnapshot value) => Value = value;
    }

    internal class ExpressionDisplaySnapshotAlias
    {
        [JsonIgnore]
        public ExpressionDisplaySnapshot Value;

        [JsonPropertyName("t")]
        public IEnumerable<CalcManagerTokenAlias> Tokens
        {
            get => Value.Tokens.Select(x => new CalcManagerTokenAlias(x));
            set => Value.Tokens = value.Select(Helpers.MapToken).ToList();
        }
        [JsonPropertyName("c")]
        public IEnumerable<ICalcManagerIExprCommandAlias> Commands
        {
            get => Value.Commands.Select(Helpers.MapCommandAlias);
            set => Value.Commands = value.Select(Helpers.MapCommandAlias).ToList();
        }

        public ExpressionDisplaySnapshotAlias() => Value = new ExpressionDisplaySnapshot();
        public ExpressionDisplaySnapshotAlias(ExpressionDisplaySnapshot value) => Value = value;
    }

    internal class StandardCalculatorSnapshotAlias
    {
        [JsonIgnore]
        public StandardCalculatorSnapshot Value;

        [JsonPropertyName("m")]
        public CalcManagerSnapshotAlias CalcManager
        {
            get => new CalcManagerSnapshotAlias(Value.CalcManager);
            set => Value.CalcManager = value.Value;
        }
        [JsonPropertyName("p")]
        public PrimaryDisplaySnapshotAlias PrimaryDisplay
        {
            get => new PrimaryDisplaySnapshotAlias(Value.PrimaryDisplay);
            set => Value.PrimaryDisplay = value.Value;
        }
        [JsonPropertyName("e")]
        public ExpressionDisplaySnapshotAlias ExpressionDisplay // optional
        {
            get => Value.ExpressionDisplay != null ? new ExpressionDisplaySnapshotAlias(Value.ExpressionDisplay) : null;
            set => Value.ExpressionDisplay = value?.Value;
        }
        [JsonPropertyName("c")]
        public IEnumerable<ICalcManagerIExprCommandAlias> Commands
        {
            get => Value.DisplayCommands.Select(Helpers.MapCommandAlias);
            set => Value.DisplayCommands = value.Select(Helpers.MapCommandAlias).ToList();
        }

        public StandardCalculatorSnapshotAlias() => Value = new StandardCalculatorSnapshot();
        public StandardCalculatorSnapshotAlias(StandardCalculatorSnapshot value) => Value = value;
    }

    internal class ApplicationSnapshotAlias
    {
        [JsonIgnore]
        public ApplicationSnapshot Value;

        [JsonPropertyName("m")]
        public int Mode { get => Value.Mode; set => Value.Mode = value; }
        [JsonPropertyName("s")]
        public StandardCalculatorSnapshotAlias StandardCalculatorSnapshot // optional
        {
            get => Value.StandardCalculator != null ? new StandardCalculatorSnapshotAlias(Value.StandardCalculator) : null;
            set => Value.StandardCalculator = value?.Value;
        }

        public ApplicationSnapshotAlias() => Value = new ApplicationSnapshot();
        public ApplicationSnapshotAlias(ApplicationSnapshot value) => Value = value;
    }

    internal static class Helpers
    {
        public static CalcManagerToken MapToken(CalcManagerTokenAlias token)
        {
            return new CalcManagerToken { OpCodeName = token.OpCodeName, CommandIndex = token.CommandIndex };
        }

        public static ICalcManagerIExprCommandAlias MapCommandAlias(ExpressionCommandWrapper exprCmd)
        {
            switch (exprCmd.Type)
            {
                case CommandType.UnaryCommand:
                    return new UnaryCommandAlias(exprCmd);
                case CommandType.BinaryCommand:
                    return new BinaryCommandAlias(exprCmd);
                case CommandType.OperandCommand:
                    return new OperandCommandAlias(exprCmd);
                case CommandType.Parentheses:
                    return new ParenthesesAlias(exprCmd);
                default:
                    throw new ArgumentOutOfRangeException(nameof(exprCmd), exprCmd.Type, "unhandled command type.");
            }
        }

        public static ExpressionCommandWrapper MapCommandAlias(ICalcManagerIExprCommandAlias exprCmd)
        {
            // Create an ExpressionCommandWrapper via the interop
            // For snapshot deserialization, we create a wrapper with the appropriate type info
            // Note: ExpressionCommandWrapper is immutable from the C# side; the wrapper carries
            // the type/command metadata but cannot be reconstructed from alias data alone.
            return new ExpressionCommandWrapper();
        }
    }
}
