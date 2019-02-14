// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
        public ref class OverflowTextBlock sealed : public Windows::UI::Xaml::Controls::Control
        {
        public:
            OverflowTextBlock() 
            {
            }

            DEPENDENCY_PROPERTY_OWNER(OverflowTextBlock);

            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, TokensUpdated);
            DEPENDENCY_PROPERTY(bool, IsActive);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Style^, TextStyle);
            void UpdateScrollButtons();
            void UnregisterEventHandlers();

        protected:
            virtual void OnApplyTemplate() override;
            virtual Windows::UI::Xaml::Automation::Peers::AutomationPeer^ OnCreateAutomationPeer() override;

        private:
            void OnScrollClick(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnPointerEntered(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void OnPointerExited(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void ShowHideScrollButtons(Windows::UI::Xaml::Visibility vLeft, Windows::UI::Xaml::Visibility vRight);
            void OnTokensUpdatedPropertyChanged(bool oldValue, bool newValue);

            void UpdateVisualState();
            void UpdateExpressionState();
            void UpdateAllState();
            void ScrollLeft();
            void ScrollRight();

            double scrollRatio = 0.7;
            bool m_scrollingLeft;
            bool m_scrollingRight;
            Windows::UI::Xaml::Controls::ListView^ m_listView;
            Windows::UI::Xaml::Controls::ScrollViewer^ m_expressionContainer;
            Windows::UI::Xaml::Controls::Button^ m_scrollLeft;
            Windows::UI::Xaml::Controls::Button^ m_scrollRight;

            Windows::Foundation::EventRegistrationToken m_scrollLeftClickEventToken;
            Windows::Foundation::EventRegistrationToken m_scrollRightClickEventToken;
            Windows::Foundation::EventRegistrationToken m_pointerEnteredEventToken;
            Windows::Foundation::EventRegistrationToken m_pointerExitedEventToken;
        };
    }
}
