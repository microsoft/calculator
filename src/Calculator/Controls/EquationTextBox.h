// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
        public ref class EquationTextBox sealed : public Windows::UI::Xaml::Controls::TextBox
        {
        public:
            EquationTextBox();

            event Windows::UI::Xaml::RoutedEventHandler^ FunctionButtonClicked;
            event Windows::UI::Xaml::RoutedEventHandler^ ColorChangeButtonClicked;
            event Windows::UI::Xaml::RoutedEventHandler^ RemoveButtonClicked;

        protected:
            virtual void OnApplyTemplate() override;

        private:
            void OnPointerEnteredButton(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void OnPointerExitedButton(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);

            Windows::UI::Core::CoreCursor^ m_cursorBeforePointerEntered;
            Windows::UI::Core::CoreCursor^ m_buttonCursor;
            Windows::UI::Xaml::Controls::Button^ m_removeButton;
            Windows::UI::Xaml::Controls::Button^ m_colorChooserButton;
            Windows::UI::Xaml::Controls::Button^ m_functionButton;
        };
    }
}
