// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ExpressionItemTemplateSelector.h"
#include "CalcViewModel/Common/DisplayExpressionToken.h"

using namespace CalculatorApp::Common;

namespace CalculatorApp
{
    namespace Converters
    {
        Windows::UI::Xaml::DataTemplate^ ExpressionItemTemplateSelector::SelectTemplateCore(Platform::Object^ item, Windows::UI::Xaml::DependencyObject^ container)
        {
            DisplayExpressionToken^ token = dynamic_cast<DisplayExpressionToken^>(item);
            if (token != nullptr)
            {
                Common::TokenType type = token->Type;

                switch (type)
                {
                case TokenType::Operator:
                    return m_operatorTemplate;
                case TokenType::Operand:
                    return m_operandTemplate;
                case TokenType::Separator:
                    return m_separatorTemplate;
                default:
                    throw ref new Platform::Exception(E_FAIL, L"Invalid token type");
                }
            }

            return m_separatorTemplate;
        }
    }
}
