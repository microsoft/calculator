// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using CalcManager.Interop;

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
}
