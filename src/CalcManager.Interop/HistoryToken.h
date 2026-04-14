#pragma once
#include "HistoryToken.g.h"

namespace winrt::CalcManager::Interop::implementation
{
    struct HistoryToken : HistoryTokenT<HistoryToken>
    {
        HistoryToken() = default;

        hstring Value() { return m_value; }
        void Value(hstring const& value) { m_value = value; }
        int32_t CommandIndex() { return m_commandIndex; }
        void CommandIndex(int32_t value) { m_commandIndex = value; }

    private:
        hstring m_value;
        int32_t m_commandIndex = 0;
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct HistoryToken : HistoryTokenT<HistoryToken, implementation::HistoryToken>
    {
    };
}
