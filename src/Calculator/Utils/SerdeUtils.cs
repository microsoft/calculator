using System.Collections.Generic;
using System.Linq;
using System.Text.Json.Serialization;
using CalculatorApp.ViewModel.Snapshot;

namespace CalculatorApp
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

    internal class CalcManagerIExprCommandAlias
    {
        [JsonPropertyName("t")]
        public string Type;

        [JsonPropertyName("c")]
        public string CmdString;
    }

    internal class CalcManagerHistoryItemAlias
    {
        [JsonIgnore]
        public CalcManagerHistoryItem Value;

        [JsonPropertyName("t")]
        public IList<CalcManagerHistoryTokenAlias> Tokens
        {
            get => Value.Tokens.Select(x => new CalcManagerHistoryTokenAlias { Value = x }).ToList();
            set => Value.Tokens = value.Select(x => new CalcManagerHistoryToken(x.OpCodeName, x.CommandIndex)).ToList();
        }

        //public IList<CalcManagerIExprCommandAlias> Commands
        //{
        //    get => Value.Commands;
        //}
    }
}
