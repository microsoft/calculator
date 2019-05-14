// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Common;
using System;

namespace CalculatorApp
{
    namespace Converters
    {
        [Windows.UI.Xaml.Data.Bindable]
        public sealed class ExpressionItemTemplateSelector : Windows.UI.Xaml.Controls.DataTemplateSelector
        {
            Windows.UI.Xaml.DataTemplate SelectTemplateCore(object item, Windows.UI.Xaml.DependencyObject container)
            {
                DisplayExpressionToken token = (DisplayExpressionToken)(item);
                if (token != null)
                {
                    Common.TokenType type = token.Type;

                    switch (type)
                    {
                        case TokenType.Operator:
                            return m_operatorTemplate;
                        case TokenType.Operand:
                            return m_operandTemplate;
                        case TokenType.Separator:
                            return m_separatorTemplate;
                        default:
                            throw new Exception("Invalid token type");
                    }
                }

                return m_separatorTemplate;
            }

            public Windows.UI.Xaml.DataTemplate OperatorTemplate
            {
                get => m_operatorTemplate;
                set => m_operatorTemplate = value;
            }

            public Windows.UI.Xaml.DataTemplate OperandTemplate
            {
                get => m_operandTemplate;
                set => m_operandTemplate = value;
            }

            public Windows.UI.Xaml.DataTemplate SeparatorTemplate
            {
                get => m_separatorTemplate;
                set => m_separatorTemplate = value;
            }

            private Windows.UI.Xaml.DataTemplate m_operatorTemplate;
            private Windows.UI.Xaml.DataTemplate m_operandTemplate;
            private Windows.UI.Xaml.DataTemplate m_separatorTemplate;
        };
    }
}
