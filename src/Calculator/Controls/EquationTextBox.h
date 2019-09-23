// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
        public ref class EquationTextBox sealed : public Windows::UI::Xaml::Controls::Control
        {
        public:
            EquationTextBox()
            {
            }

            DEPENDENCY_PROPERTY_OWNER(EquationTextBox);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::SolidColorBrush^, EquationColor);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Controls::Flyout^, ColorChooserFlyout);

            event Windows::UI::Xaml::RoutedEventHandler^ RemoveButtonClicked;
            event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesButtonClicked;
            event Windows::UI::Xaml::RoutedEventHandler ^ EquationSubmitted;

            Platform::String^ GetEquationText();
            void SetEquationText(Platform::String^ equationText);

        protected:
            virtual void OnApplyTemplate() override;
            virtual void OnPointerEntered(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) override;
            virtual void OnPointerExited(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) override;
            virtual void OnPointerCanceled(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) override;
            virtual void OnPointerCaptureLost(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) override;
            virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e) override;
            virtual void OnLostFocus(Windows::UI::Xaml::RoutedEventArgs^ e) override;

        private:
            void UpdateCommonVisualState();
            void UpdateDeleteButtonVisualState();
            bool ShouldDeleteButtonBeVisible();

            void OnRichEditBoxLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnRichEditBoxGotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnRichEditBoxLostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnRichEditBoxLosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ e);
            void OnRichEditBoxTextChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            void OnDeleteButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnEquationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnRemoveButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnColorChooserButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnFunctionButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            void OnColorFlyoutOpened(Platform::Object^ sender, Platform::Object^ e);
            void OnColorFlyoutClosed(Platform::Object^ sender, Platform::Object^ e);

            Windows::UI::Xaml::Controls::RichEditBox^ m_richEditBox;
            Windows::UI::Xaml::Controls::Button^ m_equationButton;
            Windows::UI::Xaml::Controls::Button^ m_deleteButton;
            Windows::UI::Xaml::Controls::Button^ m_removeButton;
            Windows::UI::Xaml::Controls::Button^ m_functionButton;
            Windows::UI::Xaml::Controls::Primitives::ToggleButton^ m_colorChooserButton;

            bool m_isFocused;
            bool m_isPointerOver;
            bool m_isColorChooserFlyoutOpen;
        };
    }
}
