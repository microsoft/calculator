// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Converters
    {
        [Windows::UI::Xaml::Data::Bindable] public ref class ExpressionItemTemplateSelector sealed : public Windows::UI::Xaml::Controls::DataTemplateSelector
        {
        public:
            virtual Windows::UI::Xaml::DataTemplate ^ SelectTemplateCore(Platform::Object ^ item, Windows::UI::Xaml::DependencyObject ^ container) override;

            property Windows::UI::Xaml::DataTemplate^ OperatorTemplate
            {
                Windows::UI::Xaml::DataTemplate^ get()
                {
                    return m_operatorTemplate;
                }
                void set(Windows::UI::Xaml::DataTemplate^ val)
                {
                    m_operatorTemplate = val;
                }
            }

            property Windows::UI::Xaml::DataTemplate^ OperandTemplate
            {
                Windows::UI::Xaml::DataTemplate^ get()
                {
                    return m_operandTemplate;
                }
                void set(Windows::UI::Xaml::DataTemplate^ val)
                {
                    m_operandTemplate = val;
                }
            }

            property Windows::UI::Xaml::DataTemplate^ SeparatorTemplate
            {
                Windows::UI::Xaml::DataTemplate^ get()
                {
                    return m_separatorTemplate;
                }
                void set(Windows::UI::Xaml::DataTemplate^ val)
                {
                    m_separatorTemplate = val;
                }
            }

        private:
            Windows::UI::Xaml::DataTemplate^ m_operatorTemplate;
            Windows::UI::Xaml::DataTemplate ^ m_operandTemplate;
            Windows::UI::Xaml::DataTemplate ^ m_separatorTemplate;
        };
    }
}
