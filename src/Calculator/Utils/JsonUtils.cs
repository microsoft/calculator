using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json.Serialization;
using CalculatorApp.ViewModel.Snapshot;
using Windows.ApplicationModel;

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
        [JsonIgnore]
        public UnaryCommand Value;

        [JsonPropertyName("c")]
        public IReadOnlyList<int> Commands
        {
            get => Value.Commands;
            set => Value.Commands = value;
        }

        public UnaryCommandAlias() => Value = new UnaryCommand();
        public UnaryCommandAlias(UnaryCommand value) => Value = value;
    }

    internal class BinaryCommandAlias : ICalcManagerIExprCommandAlias
    {
        [JsonIgnore]
        public BinaryCommand Value;

        [JsonPropertyName("c")]
        public int Command
        {
            get => Value.Command;
            set => Value.Command = value;
        }

        public BinaryCommandAlias() => Value = new BinaryCommand();
        public BinaryCommandAlias(BinaryCommand value) => Value = value;
    }

    internal class OperandCommandAlias : ICalcManagerIExprCommandAlias
    {
        [JsonIgnore]
        public OperandCommand Value;

        [JsonPropertyName("n")]
        public bool IsNegative
        {
            get => Value.IsNegative;
            set => Value.IsNegative = value;
        }
        [JsonPropertyName("d")]
        public bool IsDecimalPresent
        {
            get => Value.IsDecimalPresent;
            set => Value.IsDecimalPresent = value;
        }
        [JsonPropertyName("s")]
        public bool IsSciFmt
        {
            get => Value.IsSciFmt;
            set => Value.IsSciFmt = value;
        }
        [JsonPropertyName("c")]
        public IReadOnlyList<int> Commands
        {
            get => Value.Commands;
            set => Value.Commands = value;
        }

        public OperandCommandAlias() => Value = new OperandCommand();
        public OperandCommandAlias(OperandCommand value) => Value = value;
    }

    internal class ParenthesesAlias : ICalcManagerIExprCommandAlias
    {
        [JsonIgnore]
        public Parentheses Value;

        [JsonPropertyName("c")]
        public int Command
        {
            get => Value.Command;
            set => Value.Command = value;
        }

        public ParenthesesAlias() => Value = new Parentheses();
        public ParenthesesAlias(Parentheses value) => Value = value;
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

        public static ICalcManagerIExprCommandAlias MapCommandAlias(ICalcManagerIExprCommand exprCmd)
        {
            if (exprCmd is UnaryCommand unary)
            {
                return new UnaryCommandAlias(unary);
            }
            else if (exprCmd is BinaryCommand binary)
            {
                return new BinaryCommandAlias(binary);
            }
            else if (exprCmd is OperandCommand operand)
            {
                return new OperandCommandAlias(operand);
            }
            else if (exprCmd is Parentheses paren)
            {
                return new ParenthesesAlias(paren);
            }
            throw new NotImplementedException("unhandled command type.");
        }

        public static ICalcManagerIExprCommand MapCommandAlias(ICalcManagerIExprCommandAlias exprCmd)
        {
            if (exprCmd is UnaryCommandAlias unary)
            {
                return new UnaryCommand { Commands = unary.Commands };
            }
            else if (exprCmd is BinaryCommandAlias binary)
            {
                return new BinaryCommand { Command = binary.Command };
            }
            else if (exprCmd is OperandCommandAlias operand)
            {
                return new OperandCommand
                {
                    IsNegative = operand.IsNegative,
                    IsDecimalPresent = operand.IsDecimalPresent,
                    IsSciFmt = operand.IsSciFmt,
                    Commands = operand.Commands
                };
            }
            else if (exprCmd is ParenthesesAlias paren)
            {
                return new Parentheses { Command = paren.Command };
            }
            throw new NotImplementedException("unhandled command type.");
        }
    }
}
