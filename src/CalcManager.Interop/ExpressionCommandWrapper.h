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

        CalcManager::Interop::CommandType Type();
        int32_t Command();
        com_array<int32_t> Commands();
        bool IsNegative();

    private:
        CalcManager::Interop::CommandType m_type{ CalcManager::Interop::CommandType::UnaryCommand };
        int32_t m_command{ 0 };
        std::vector<int32_t> m_commands;
        bool m_isNegative{ false };
    };
}

namespace winrt::CalcManager::Interop::factory_implementation
{
    struct ExpressionCommandWrapper : ExpressionCommandWrapperT<ExpressionCommandWrapper, implementation::ExpressionCommandWrapper>
    {
    };
}
