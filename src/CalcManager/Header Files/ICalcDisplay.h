// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../ExpressionCommandInterface.h"

// Callback interface to be implemented by the clients of CCalcEngine
class ICalcDisplay
{
public:
    virtual void SetPrimaryDisplay(const std::wstring& pszText, bool isError) = 0;
    virtual void SetIsInError(bool isInError) = 0;
    virtual void SetExpressionDisplay(
        _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& tokens,
        _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& commands) = 0;
    virtual void SetParenthesisNumber(_In_ unsigned int count) = 0;
    virtual void OnNoRightParenAdded() = 0;
    virtual void MaxDigitsReached() = 0; // not an error but still need to inform UI layer.
    virtual void BinaryOperatorReceived() = 0;
    virtual void OnHistoryItemAdded(_In_ unsigned int addedItemIndex) = 0;
    virtual void SetMemorizedNumbers(const std::vector<std::wstring>& memorizedNumbers) = 0;
    virtual void MemoryItemChanged(unsigned int indexOfMemory) = 0;
    virtual void InputChanged() = 0;
};
