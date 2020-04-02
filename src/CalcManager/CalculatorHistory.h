// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ExpressionCommandInterface.h"
#include "Header Files/IHistoryDisplay.h"

namespace CalculationManager
{
    struct HISTORYITEMVECTOR
    {
        std::shared_ptr<std::vector<std::pair<std::wstring, int>>> spTokens;
        std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> spCommands;
        std::wstring expression;
        std::wstring result;
    };

    struct HISTORYITEM
    {
        HISTORYITEMVECTOR historyItemVector;
    };

    class CalculatorHistory : public IHistoryDisplay
    {
    public:
        CalculatorHistory(const size_t maxSize);
        unsigned int AddToHistory(
            _In_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const& spTokens,
            _In_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const& spCommands,
            std::wstring_view result);
        std::vector<std::shared_ptr<HISTORYITEM>> const& GetHistory();
        std::shared_ptr<HISTORYITEM> const& GetHistoryItem(unsigned int uIdx);
        void ClearHistory();
        unsigned int AddItem(_In_ std::shared_ptr<HISTORYITEM> const& spHistoryItem);
        bool RemoveItem(unsigned int uIdx);
        size_t MaxHistorySize() const
        {
            return m_maxHistorySize;
        }

    private:
        std::vector<std::shared_ptr<HISTORYITEM>> m_historyItems;
        const size_t m_maxHistorySize;
    };
}
