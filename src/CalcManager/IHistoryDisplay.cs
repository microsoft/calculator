// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalcManager
{
    // Callback interface to be implemented by the clients of CCalcEngine if they require equation history
    public interface IHistoryDisplay
    {
        uint AddToHistory(
            List<Tuple<string, int>> tokens,
            List<IExpressionCommand> commands,
            string result);
    }
}
