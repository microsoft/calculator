// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;

namespace CalcManager.CalculationManager
{
    public class HistoryItemVector
    {
        public List<Tuple<string, int>> SpTokens { get; set; }
        public List<IExpressionCommand> SpCommands { get; set; }
        public string Expression { get; set; }
        public string Result { get; set; }
    }

    public class HistoryItem
    {
        public HistoryItemVector HistoryItemVector { get; set; } = new HistoryItemVector();
    }

    public class CalculatorHistory : IHistoryDisplay
    {
        private readonly List<HistoryItem> m_historyItems = new List<HistoryItem>();
        private readonly int m_maxHistorySize;

        public CalculatorHistory(int maxSize)
        {
            m_maxHistorySize = maxSize;
        }

        private static string GetGeneratedExpression(List<Tuple<string, int>> tokens)
        {
            if (tokens == null) return string.Empty;

            var sb = new StringBuilder();
            bool isFirst = true;

            foreach (var token in tokens)
            {
                if (isFirst)
                {
                    isFirst = false;
                }
                else
                {
                    sb.Append(' ');
                }
                sb.Append(token.Item1);
            }

            return sb.ToString();
        }

        public uint AddToHistory(
            List<Tuple<string, int>> spTokens,
            List<IExpressionCommand> spCommands,
            string result)
        {
            var spHistoryItem = new HistoryItem();
            spHistoryItem.HistoryItemVector.SpTokens = spTokens != null ? new List<Tuple<string, int>>(spTokens) : new List<Tuple<string, int>>();
            spHistoryItem.HistoryItemVector.SpCommands = spCommands != null ? new List<IExpressionCommand>(spCommands) : new List<IExpressionCommand>();
            spHistoryItem.HistoryItemVector.Expression = GetGeneratedExpression(spTokens);
            spHistoryItem.HistoryItemVector.Result = result;
            return AddItem(spHistoryItem);
        }

        public uint AddItem(HistoryItem spHistoryItem)
        {
            if (m_historyItems.Count >= m_maxHistorySize && m_historyItems.Count > 0)
            {
                m_historyItems.RemoveAt(0);
            }

            m_historyItems.Add(spHistoryItem);
            return (uint)(m_historyItems.Count - 1);
        }

        public bool RemoveItem(uint uIdx)
        {
            if (uIdx < m_historyItems.Count)
            {
                m_historyItems.RemoveAt((int)uIdx);
                return true;
            }

            return false;
        }

        public List<HistoryItem> GetHistory() => m_historyItems;

        public HistoryItem GetHistoryItem(uint uIdx)
        {
            if (uIdx < m_historyItems.Count)
            {
                return m_historyItems[(int)uIdx];
            }
            return null;
        }

        public void ClearHistory()
        {
            m_historyItems.Clear();
        }

        public int MaxHistorySize => m_maxHistorySize;
    }
}
