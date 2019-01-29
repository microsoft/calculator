// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Converters
    {
        [Windows::UI::Xaml::Data::Bindable]
        public ref class ExpressionItemContainerStyle sealed : public Windows::UI::Xaml::Controls::StyleSelector
        {
        public:
            virtual Windows::UI::Xaml::Style^ SelectStyleCore(Platform::Object^ item, Windows::UI::Xaml::DependencyObject^ container) override;

            property Windows::UI::Xaml::Style^ EditableOperatorStyle
            {
                Windows::UI::Xaml::Style^ get()
                {
                    return m_editableOperatorStyle;
                }
                void set(Windows::UI::Xaml::Style^ val)
                {
                    m_editableOperatorStyle = val;
                }
            }

            property Windows::UI::Xaml::Style^ OperandStyle
            {
                Windows::UI::Xaml::Style^ get()
                {
                    return m_operandStyle;
                }
                void set(Windows::UI::Xaml::Style^ val)
                {
                    m_operandStyle = val;
                }
            }

            property Windows::UI::Xaml::Style^ SeparatorStyle
            {
                Windows::UI::Xaml::Style^ get()
                {
                    return m_separatorStyle;
                }
                void set(Windows::UI::Xaml::Style^ val)
                {
                    m_separatorStyle = val;
                }
            }

            property Windows::UI::Xaml::Style^ NonEditableOperatorStyle
            {
                Windows::UI::Xaml::Style^ get()
                {
                    return m_nonEditableOperatorStyle;
                }
                void set(Windows::UI::Xaml::Style^ val)
                {
                    m_nonEditableOperatorStyle = val;
                }
            }

        private:
            Windows::UI::Xaml::Style^ m_editableOperatorStyle;
            Windows::UI::Xaml::Style^ m_nonEditableOperatorStyle;
            Windows::UI::Xaml::Style^ m_operandStyle;
            Windows::UI::Xaml::Style^ m_separatorStyle;
        };
    }
}
