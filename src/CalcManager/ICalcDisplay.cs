// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalcManager
{
    // Callback interface to be implemented by the clients of CCalcEngine
    public interface ICalcDisplay
    {
        void SetPrimaryDisplay(string pszText, bool isError);
        void SetIsInError(bool isInError);
        void SetExpressionDisplay(
            List<Tuple<string, int>> tokens,
            List<IExpressionCommand> commands);
        void SetParenthesisNumber(uint count);
        void OnNoRightParenAdded();
        void MaxDigitsReached(); // not an error but still need to inform UI layer.
        void BinaryOperatorReceived();
        void OnHistoryItemAdded(uint addedItemIndex);
        void SetMemorizedNumbers(List<string> memorizedNumbers);
        void MemoryItemChanged(uint indexOfMemory);
        void InputChanged();
    }
}
