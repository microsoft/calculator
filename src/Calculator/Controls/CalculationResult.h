// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
        public delegate void SelectedEventHandler(Platform::Object^ sender);

        public ref class CalculationResult sealed: public Windows::UI::Xaml::Controls::Control
        {
        public:
            CalculationResult();

            DEPENDENCY_PROPERTY_OWNER(CalculationResult);

            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Visibility, ExpressionVisibility);
            DEPENDENCY_PROPERTY(double, MinFontSize);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Thickness, DisplayMargin);
            DEPENDENCY_PROPERTY(int, MaxExpressionHistoryCharacters);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsActive);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(Windows::UI::Xaml::Media::Brush^, AccentColor);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(Platform::String^, DisplayValue);
            DEPENDENCY_PROPERTY(Platform::String^, DisplayStringExpression);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsInError);
            DEPENDENCY_PROPERTY_WITH_DEFAULT(bool, IsOperatorCommand, false);

            event SelectedEventHandler^ Selected;
            void ProgrammaticSelect();

        internal:
            void UpdateTextState();
            Platform::String^ GetRawDisplayValue();

        protected:
            virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e) override;
            virtual void OnApplyTemplate() override;
            virtual void OnTapped(Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e) override;
            virtual void OnPointerPressed(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) override;
            virtual void OnRightTapped(Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e) override;
            virtual void OnGotFocus(Windows::UI::Xaml::RoutedEventArgs^ e) override;
            virtual void OnLostFocus(Windows::UI::Xaml::RoutedEventArgs^ e) override;

            virtual Windows::UI::Xaml::Automation::Peers::AutomationPeer^ OnCreateAutomationPeer() override;

        private:
            void OnIsActivePropertyChanged(bool oldValue, bool newValue);
            void OnAccentColorPropertyChanged(Windows::UI::Xaml::Media::Brush^ oldValue, Windows::UI::Xaml::Media::Brush^ newValue);
            void OnDisplayValuePropertyChanged(Platform::String^ oldValue, Platform::String^ newValue);
            void OnIsInErrorPropertyChanged(bool oldValue, bool newValue);
            void TextContainerSizeChanged(Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
            void OnTextContainerLayoutUpdated(Object^ sender, Object^ e);
            void UpdateVisualState();
            void UpdateAllState();
            void OnScrollClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnPointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void OnPointerExited(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void ModifyFontAndMargin(Windows::UI::Xaml::Controls::TextBlock^ textBlock, double fontChange);
            void ShowHideScrollButtons(Windows::UI::Xaml::Visibility vLeft, Windows::UI::Xaml::Visibility vRight);
            void UpdateScrollButtons();
            void ScrollLeft();
            void ScrollRight();
            void RaiseSelectedEvent();

            // Visual states for focused
            static Platform::StringReference s_FocusedState;
            static Platform::StringReference s_UnfocusedState;

            Windows::UI::Xaml::Controls::ScrollViewer^ m_textContainer;
            Windows::UI::Xaml::Controls::TextBlock^ m_textBlock;
            Windows::UI::Xaml::Controls::HyperlinkButton^ m_scrollLeft;
            Windows::UI::Xaml::Controls::HyperlinkButton^ m_scrollRight;
            double m_startingFontSize;
            double scrollRatio = 0.7;
            bool m_isScalingText;
            bool m_haveCalculatedMax;
            Windows::Foundation::EventRegistrationToken m_textContainerLayoutChangedToken;
        };
    }
}
