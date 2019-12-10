// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "Calculator/Controls/MathRichEditBox.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        enum class EquationSubmissionSource
        {
            FOCUS_LOST,
            ENTER_KEY
        };

    public
        ref class EquationTextBox sealed : public Windows::UI::Xaml::Controls::Control
        {
        public:
            EquationTextBox();

            DEPENDENCY_PROPERTY_OWNER(EquationTextBox);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::SolidColorBrush ^, EquationColor);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Controls::Flyout ^, ColorChooserFlyout);
            DEPENDENCY_PROPERTY(Platform::String ^, EquationButtonContentIndex);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, HasError);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsAddEquationMode);

            PROPERTY_R(bool, HasFocus);

            event Windows::UI::Xaml::RoutedEventHandler ^ RemoveButtonClicked;
            event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesButtonClicked;
            event Windows::Foundation::EventHandler<EquationSubmissionSource> ^ EquationSubmitted;
            event Windows::UI::Xaml::RoutedEventHandler ^ EquationButtonClicked;

            Platform::String ^ GetEquationText();
            void SetEquationText(Platform::String ^ equationText);
            void FocusTextBox();

        protected:
            virtual void OnApplyTemplate() override;
            virtual void OnPointerEntered(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            virtual void OnPointerExited(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            virtual void OnPointerCanceled(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            virtual void OnPointerCaptureLost(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) override;
            virtual void OnLostFocus(Windows::UI::Xaml::RoutedEventArgs ^ e) override;
            void OnIsAddEquationModePropertyChanged(bool oldValue, bool newValue);

        private:
            void UpdateCommonVisualState();
            void UpdateButtonsVisualState();
            bool RichEditHasContent();

            void OnRichEditBoxGotFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnRichEditBoxLostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnRichEditBoxTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

            void OnDeleteButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnEquationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnKGFEquationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnRemoveButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnColorChooserButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnFunctionButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnRichEditMenuOpening(Platform::Object ^ sender, Platform::Object ^ args);

            void OnColorFlyoutOpened(Platform::Object ^ sender, Platform::Object ^ e);
            void OnColorFlyoutClosed(Platform::Object ^ sender, Platform::Object ^ e);

            void OnHasErrorPropertyChanged(bool oldValue, bool newValue);

            CalculatorApp::Controls::MathRichEditBox ^ m_richEditBox;
            Windows::UI::Xaml::Controls::Primitives::ToggleButton ^ m_equationButton;
            Windows::UI::Xaml::Controls::Button ^ m_kgfEquationButton;
            Windows::UI::Xaml::Controls::Button ^ m_deleteButton;
            Windows::UI::Xaml::Controls::Button ^ m_removeButton;
            Windows::UI::Xaml::Controls::Button ^ m_functionButton;
            Windows::UI::Xaml::Controls::Primitives::ToggleButton ^ m_colorChooserButton;

            Windows::UI::Xaml::Controls::MenuFlyout^ m_richEditContextMenu;
            Windows::UI::Xaml::Controls::MenuFlyoutItem^ m_kgfEquationMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem^ m_removeMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem^ m_colorChooserMenuItem;

            bool m_isPointerOver;
            bool m_isColorChooserFlyoutOpen;
			EquationSubmissionSource m_sourceSubmission;
		};
    }
}
