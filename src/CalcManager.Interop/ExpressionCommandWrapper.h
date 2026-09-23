// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ExpressionCommandWrapper.g.h"

namespace winrt::CalcManager::Interop::implementation
{
    struct ExpressionCommandWrapper : ExpressionCommandWrapperT<ExpressionCommandWrapper>
    {
        ExpressionCommandWrapper() = default;
        ExpressionCommandWrapper(const std::shared_ptr<IExpressionCommand>& command);
        ExpressionCommandWrapper(
            CalcManager::Interop::CommandType type,
            int32_t command,
            array_view<int32_t const> commands,
            bool isNegative,
            bool isDecimalPresent,
            bool isSciFmt);

        CalcManager::Interop::CommandType Type();
        int32_t Command();
        com_array<int32_t> Commands();
        bool IsNegative();
        bool IsDecimalPresent();
        bool IsSciFmt();

        std::shared_ptr<IExpressionCommand> ToUnderlying() const;

    private:
        CalcManager::Interop::CommandType m_type{ CalcManager::Interop::CommandType::UnaryCommand };
        int32_t m_command{ 0 };
        std::vector<int32_t> m_commands;
        bool m_isNegative{ false };
        bool m_isDecimalPresent{ false };
        bool m_isSciFmt{ false };
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct ExpressionCommandWrapper : ExpressionCommandWrapperT<ExpressionCommandWrapper, implementation::ExpressionCommandWrapper>
    {
    };
}
