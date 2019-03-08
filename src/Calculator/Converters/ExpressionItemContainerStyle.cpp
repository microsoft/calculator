// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ExpressionItemContainerStyle.h"
#include "CalcViewModel/Common/DisplayExpressionToken.h"

using namespace CalculatorApp::Common;

namespace CalculatorApp
{
    namespace Converters
    {
        Windows::UI::Xaml::Style^ ExpressionItemContainerStyle::SelectStyleCore(Platform::Object^ item, Windows::UI::Xaml::DependencyObject^ container)
        {
            DisplayExpressionToken^ token = dynamic_cast<DisplayExpressionToken^>(item);
            if (token != nullptr)
            {
                Common::TokenType type = token->Type;

                switch (type)
                {
                case TokenType::Operator:
                    if (token->IsTokenEditable)
                    {
                        return m_editableOperatorStyle;
                    }
                    else
                    {
                        return m_nonEditableOperatorStyle;
                    }
                case TokenType::Operand:
                    return m_operandStyle;
                case TokenType::Separator:
                    return m_separatorStyle;
                default:
                    throw ref new Platform::Exception(E_FAIL, L"Invalid token type");
                }
            }

            return m_separatorStyle;
        }
    }
}
