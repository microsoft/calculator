// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        enum class OverflowButtonPlacement : int
        {
            InLine,
            Above
        };

    public
        ref class OverflowTextBlock sealed : public Windows::UI::Xaml::Controls::Control
        {
        public:
            OverflowTextBlock()
                : m_isAccessibilityViewControl(false)
                , m_ignoreViewChanged(false)
                , m_expressionContent(nullptr)
                , m_itemsControl(nullptr)
                , m_expressionContainer(nullptr)
                , m_scrollLeft(nullptr)
                , m_scrollRight(nullptr)
            {
            }

            DEPENDENCY_PROPERTY_OWNER(OverflowTextBlock);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, TokensUpdated);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(OverflowButtonPlacement, ScrollButtonsPlacement);
            DEPENDENCY_PROPERTY(bool, IsActive);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Style ^, TextStyle);
            DEPENDENCY_PROPERTY(double, ScrollButtonsWidth);
            DEPENDENCY_PROPERTY(double, ScrollButtonsFontSize);

            void OnTokensUpdatedPropertyChanged(bool oldValue, bool newValue);
            void OnScrollButtonsPlacementPropertyChanged(OverflowButtonPlacement oldValue, OverflowButtonPlacement newValue);
            void UpdateScrollButtons();
            void UnregisterEventHandlers();

        protected:
            virtual void OnApplyTemplate() override;
            virtual Windows::UI::Xaml::Automation::Peers::AutomationPeer ^ OnCreateAutomationPeer() override;

        private:
            void OnScrollLeftClick(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnScrollRightClick(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
            void OnViewChanged(_In_opt_ Platform::Object ^ sender, _In_opt_ Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs ^ args);

            void UpdateVisualState();
            void UpdateAllState();
            void ScrollLeft();
            void ScrollRight();

            bool m_isAccessibilityViewControl;
            bool m_ignoreViewChanged;
            Windows::UI::Xaml::FrameworkElement ^ m_expressionContent;
            Windows::UI::Xaml::Controls::ItemsControl ^ m_itemsControl;
            Windows::UI::Xaml::Controls::ScrollViewer ^ m_expressionContainer;
            Windows::UI::Xaml::Controls::Button ^ m_scrollLeft;
            Windows::UI::Xaml::Controls::Button ^ m_scrollRight;

            Windows::Foundation::EventRegistrationToken m_scrollLeftClickEventToken;
            Windows::Foundation::EventRegistrationToken m_scrollRightClickEventToken;
            Windows::Foundation::EventRegistrationToken m_containerViewChangedToken;
        };
    }
}
