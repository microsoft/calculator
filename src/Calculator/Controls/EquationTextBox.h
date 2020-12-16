// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "Calculator/Controls/MathRichEditBox.h"
#include "CalcViewModel/Common/TraceLogger.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class EquationTextBox sealed : public Windows::UI::Xaml::Controls::Control
        {
        public:
            EquationTextBox();

            DEPENDENCY_PROPERTY_OWNER(EquationTextBox);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::SolidColorBrush ^, EquationColor);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::SolidColorBrush ^, EquationButtonForegroundColor);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Controls::Flyout ^, ColorChooserFlyout);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(Platform::String ^, EquationButtonContentIndex);
            DEPENDENCY_PROPERTY(Platform::String ^, MathEquation);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, HasError);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsAddEquationMode);
            DEPENDENCY_PROPERTY(Platform::String ^, ErrorText);
            DEPENDENCY_PROPERTY(bool, IsEquationLineDisabled);

            PROPERTY_R(bool, HasFocus);

            event Windows::UI::Xaml::RoutedEventHandler ^ RemoveButtonClicked;
            event Windows::UI::Xaml::RoutedEventHandler ^ KeyGraphFeaturesButtonClicked;
            event Windows::Foundation::EventHandler<MathRichEditBoxSubmission ^> ^ EquationSubmitted;
            event Windows::Foundation::EventHandler<MathRichEditBoxFormatRequest ^> ^ EquationFormatRequested;
            event Windows::UI::Xaml::RoutedEventHandler ^ EquationButtonClicked;

            void SetEquationText(Platform::String ^ equationText);
            void FocusTextBox();

        protected:
            virtual void OnApplyTemplate() override;
            virtual void OnPointerEntered(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            virtual void OnPointerExited(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            virtual void OnPointerCanceled(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            virtual void OnPointerCaptureLost(Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e) override;
            void OnIsAddEquationModePropertyChanged(bool oldValue, bool newValue);

        private:
            void UpdateCommonVisualState();
            void UpdateButtonsVisualState();
            bool RichEditHasContent();

            void OnRichEditBoxGotFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnRichEditBoxLostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnRichEditTextChanged(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

            void OnDeleteButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnEquationButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnRemoveButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnColorChooserButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnFunctionButtonClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnFunctionMenuButtonClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnRichEditMenuOpened(Platform::Object ^ sender, Platform::Object ^ args);

            void OnCutClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnCopyClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnPasteClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnUndoClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnSelectAllClicked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

            void OnColorFlyoutOpened(Platform::Object ^ sender, Platform::Object ^ e);
            void OnColorFlyoutClosed(Platform::Object ^ sender, Platform::Object ^ e);

            void OnHasErrorPropertyChanged(bool oldValue, bool newValue);
            void OnEquationButtonContentIndexPropertyChanged(Platform::String ^ /*oldValue*/, Platform::String ^ newValue);

            void SetEquationButtonTooltipAndAutomationName();

            CalculatorApp::Controls::MathRichEditBox ^ m_richEditBox;
            Windows::UI::Xaml::Controls::Primitives::ToggleButton ^ m_equationButton;
            Windows::UI::Xaml::Controls::Button ^ m_deleteButton;
            Windows::UI::Xaml::Controls::Button ^ m_removeButton;
            Windows::UI::Xaml::Controls::Button ^ m_functionButton;
            Windows::UI::Xaml::Controls::Primitives::ToggleButton ^ m_colorChooserButton;

            Windows::UI::Xaml::Controls::MenuFlyout^ m_richEditContextMenu;
            Windows::UI::Xaml::Controls::MenuFlyoutItem ^ m_cutMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem ^ m_copyMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem ^ m_pasteMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem ^ m_undoMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem ^ m_selectAllMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem^ m_kgfEquationMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem^ m_removeMenuItem;
            Windows::UI::Xaml::Controls::MenuFlyoutItem^ m_colorChooserMenuItem;

            bool m_isPointerOver;
            bool m_isColorChooserFlyoutOpen;
            void OnEquationSubmitted(Platform::Object ^ sender, CalculatorApp::Controls::MathRichEditBoxSubmission ^ args);
            void OnEquationFormatRequested(Platform::Object ^ sender, CalculatorApp::Controls::MathRichEditBoxFormatRequest ^ args);
        };
    }
}
