// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        delegate void SelectedEventHandler(Platform::Object ^ sender);

    public
        ref class CalculationResult sealed : public Windows::UI::Xaml::Controls::Control
        {
        public:
            CalculationResult();

            DEPENDENCY_PROPERTY_OWNER(CalculationResult);

            DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, MinFontSize, 0.0);
            DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(double, MaxFontSize, 30.0);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Thickness, DisplayMargin);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsActive);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(Platform::String ^, DisplayValue);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsInError);
            DEPENDENCY_PROPERTY_WITH_DEFAULT(bool, IsOperatorCommand, false);

            event SelectedEventHandler ^ Selected;
            void ProgrammaticSelect();

            internal : void UpdateTextState();
            Platform::String ^ GetRawDisplayValue();

        protected:
            virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) override;
            virtual void OnApplyTemplate() override;
            virtual void OnTapped(Windows::UI::Xaml::Input::TappedRoutedEventArgs ^ e) override;
            virtual void OnRightTapped(Windows::UI::Xaml::Input::RightTappedRoutedEventArgs ^ e) override;
 
            virtual Windows::UI::Xaml::Automation::Peers::AutomationPeer ^ OnCreateAutomationPeer() override;

        private:
            void OnIsActivePropertyChanged(bool oldValue, bool newValue);
            void OnDisplayValuePropertyChanged(Platform::String ^ oldValue, Platform::String ^ newValue);
            void OnIsInErrorPropertyChanged(bool oldValue, bool newValue);
            void OnMinFontSizePropertyChanged(double oldValue, double newValue);
            void OnMaxFontSizePropertyChanged(double oldValue, double newValue);
            void OnTextContainerSizeChanged(Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);
            void OnTextBlockSizeChanged(Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);
            void OnTextContainerLayoutUpdated(Object ^ sender, Object ^ e);
            void OnTextContainerOnViewChanged(Object ^ sender, Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs ^ e);
            void UpdateVisualState();
            void UpdateAllState();
            void OnScrollLeftClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnScrollRightClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnPointerEntered(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e);
            void OnPointerExited(Platform::Object ^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs ^ e);
            void ModifyFontAndMargin(Windows::UI::Xaml::Controls::TextBlock ^ textBlock, double fontChange);
            void UpdateScrollButtons();
            void ScrollLeft();
            void ScrollRight();
            void RaiseSelectedEvent();

            Windows::UI::Xaml::Controls::ScrollViewer ^ m_textContainer;
            Windows::UI::Xaml::Controls::TextBlock ^ m_textBlock;
            Windows::UI::Xaml::Controls::HyperlinkButton ^ m_scrollLeft;
            Windows::UI::Xaml::Controls::HyperlinkButton ^ m_scrollRight;
            bool m_isScalingText;
            bool m_haveCalculatedMax;
            Windows::Foundation::EventRegistrationToken m_textContainerLayoutChangedToken;
            Windows::Foundation::EventRegistrationToken m_textContainerViewChangedToken;
            Windows::Foundation::EventRegistrationToken m_textContainerSizeChangedToken;
            Windows::Foundation::EventRegistrationToken m_scrollRightClickToken;
            Windows::Foundation::EventRegistrationToken m_scrollLeftClickToken;
            Windows::Foundation::EventRegistrationToken m_textBlockSizeChangedToken;
        };
    }
}
