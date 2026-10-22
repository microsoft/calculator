// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalcManager.Interop
{
    public sealed class HistoryToken
    {
        public string Value { get; set; }
        public int CommandIndex { get; set; }

        public HistoryToken()
        {
        }

        public HistoryToken(string value, int commandIndex)
        {
            Value = value;
            CommandIndex = commandIndex;
        }
    }
}
