// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;
using System;
using Windows.UI.Xaml;

namespace CalculatorApp
{
    namespace Converters
    {
        [Windows.UI.Xaml.Data.Bindable]
        public sealed class ExpressionItemTemplateSelector : Windows.UI.Xaml.Controls.DataTemplateSelector
        {
            protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
            {
                DisplayExpressionToken token = (item as DisplayExpressionToken);
                if (token != null)
                {
                    CalculatorApp.ViewModel.Common.TokenType type = token.Type;

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
                get
                {
                    return m_operatorTemplate;
                }

                set
                {
                    m_operatorTemplate = value;
                }
            }

            public Windows.UI.Xaml.DataTemplate OperandTemplate
            {
                get
                {
                    return m_operandTemplate;
                }

                set
                {
                    m_operandTemplate = value;
                }
            }

            public Windows.UI.Xaml.DataTemplate SeparatorTemplate
            {
                get
                {
                    return m_separatorTemplate;
                }

                set
                {
                    m_separatorTemplate = value;
                }
            }

            private Windows.UI.Xaml.DataTemplate m_operatorTemplate;
            private Windows.UI.Xaml.DataTemplate m_operandTemplate;
            private Windows.UI.Xaml.DataTemplate m_separatorTemplate;
        }
    }
}

