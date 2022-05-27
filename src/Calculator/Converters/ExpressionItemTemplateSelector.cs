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
                            return OperatorTemplate;
                        case TokenType.Operand:
                            return OperandTemplate;
                        case TokenType.Separator:
                            return SeparatorTemplate;
                        default:
                            throw new Exception("Invalid token type");
                    }
                }

                return SeparatorTemplate;
            }

            public Windows.UI.Xaml.DataTemplate OperatorTemplate { get; set; }

            public Windows.UI.Xaml.DataTemplate OperandTemplate { get; set; }

            public Windows.UI.Xaml.DataTemplate SeparatorTemplate { get; set; }
        }
    }
}

