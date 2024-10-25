using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text.Json.Serialization;
using CalculatorApp.ViewModel.Snapshot;

namespace CalculatorApp.SerdeUtils
{
    internal class CalcManagerHistoryTokenAlias
    {
        [JsonIgnore]
        public CalcManagerHistoryToken Value;

        [JsonPropertyName("t")]
        public string OpCodeName
        {
            get => Value.OpCodeName;
            set => Value.OpCodeName = value;
        }
        [JsonPropertyName("c")]
        public int CommandIndex { get => Value.CommandIndex; }
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
        [JsonIgnore]
        public UnaryCommand Value;

        [JsonPropertyName("c")]
        public IList<int> Commands { get => Value.Commands.ToList(); }
    }

    internal class BinaryCommandAlias : ICalcManagerIExprCommandAlias
    {
        [JsonIgnore]
        public BinaryCommand Value;

        [JsonPropertyName("c")]
        public int Command { get => Value.Command; }
    }

    internal class OperandCommandAlias : ICalcManagerIExprCommandAlias
    {
        [JsonIgnore]
        public OperandCommand Value;

        [JsonPropertyName("n")]
        public bool IsNegative { get => Value.IsNegative; }
        [JsonPropertyName("d")]
        public bool IsDecimalPresent { get => Value.IsDecimalPresent; }
        [JsonPropertyName("s")]
        public bool IsSciFmt { get => Value.IsSciFmt; }
        [JsonPropertyName("c")]
        public IList<int> Commands { get => Value.Commands.ToList(); }
    }

    internal class ParenthesesAlias : ICalcManagerIExprCommandAlias
    {
        [JsonIgnore]
        public Parentheses Value;

        [JsonPropertyName("c")]
        public int Command { get => Value.Command; }
    }

    internal class CalcManagerHistoryItemAlias
    {
        [JsonIgnore]
        public CalcManagerHistoryItem Value;

        [JsonPropertyName("t")]
        public IList<CalcManagerHistoryTokenAlias> Tokens
        {
            get => Value.Tokens.Select(x => new CalcManagerHistoryTokenAlias { Value = x }).ToList();
        }

        [JsonPropertyName("c")]
        public IList<ICalcManagerIExprCommandAlias> Commands
        {
            get => Value.Commands.Select(Helpers.MapCommandAlias).ToList();
        }

        [JsonPropertyName("e")]
        public string Expression { get => Value.Expression; }

        [JsonPropertyName("r")]
        public string Result { get => Value.Result; }

    }

    internal class CalcManagerSnapshotAlias
    {
        [JsonIgnore]
        public CalcManagerSnapshot Value;

        [JsonPropertyName("h")]
        public IList<CalcManagerHistoryItemAlias> HistoryItems { get => Value.HistoryItems.Select(x => new CalcManagerHistoryItemAlias { Value = x }).ToList(); }
    }

    internal class PrimaryDisplaySnapshotAlias
    {
        [JsonIgnore]
        public PrimaryDisplaySnapshot Value;

        [JsonPropertyName("d")]
        public string DisplayValue { get => Value.DisplayValue; }
        [JsonPropertyName("e")]
        public bool IsError { get => Value.IsError; }
    }

    internal class ExpressionDisplaySnapshotAlias
    {
        [JsonIgnore]
        public ExpressionDisplaySnapshot Value;

        [JsonPropertyName("t")]
        public IList<CalcManagerHistoryTokenAlias> Tokens { get => Value.Tokens.Select(x => new CalcManagerHistoryTokenAlias { Value = x }).ToList(); }
        [JsonPropertyName("c")]
        public IList<ICalcManagerIExprCommandAlias> Commands { get => Value.Commands.Select(Helpers.MapCommandAlias).ToList(); }
    }

    internal class StandardCalculatorSnapshotAlias
    {
        [JsonIgnore]
        public StandardCalculatorSnapshot Value;

        [JsonPropertyName("m")]
        public CalcManagerSnapshotAlias CalcManager { get => new CalcManagerSnapshotAlias { Value = Value.CalcManager }; }
        [JsonPropertyName("p")]
        public PrimaryDisplaySnapshotAlias PrimaryDisplay { get => new PrimaryDisplaySnapshotAlias { Value = Value.PrimaryDisplay }; }
        [JsonPropertyName("e")]
        public ExpressionDisplaySnapshotAlias ExpressionDisplay { get => new ExpressionDisplaySnapshotAlias { Value = Value.ExpressionDisplay }; }
        [JsonPropertyName("c")]
        public IList<ICalcManagerIExprCommandAlias> Commands { get => Value.DisplayCommands.Select(Helpers.MapCommandAlias).ToList(); }
    }

    internal class ApplicationSnapshotAlias
    {
        [JsonIgnore]
        public ApplicationSnapshot Value;

        [JsonPropertyName("m")]
        public int Mode { get => Value.Mode; }
        [JsonPropertyName("s")]
        public StandardCalculatorSnapshotAlias StandardCalculatorSnapshot { get => new StandardCalculatorSnapshotAlias { Value = Value.StandardCalculator }; }
    }

    internal static class Helpers
    {
        public static ICalcManagerIExprCommandAlias MapCommandAlias(ICalcManagerIExprCommand exprCmd)
        {
            if (exprCmd is UnaryCommand unary)
            {
                return new UnaryCommandAlias { Value = unary };
            }
            else if (exprCmd is BinaryCommand binary)
            {
                return new BinaryCommandAlias { Value = binary };
            }
            else if (exprCmd is OperandCommand operand)
            {
                return new OperandCommandAlias { Value = operand };
            }
            else if (exprCmd is Parentheses paren)
            {
                return new ParenthesesAlias { Value = paren };
            }
            throw new NotImplementedException("unhandled command type.");
        }
    }
}
