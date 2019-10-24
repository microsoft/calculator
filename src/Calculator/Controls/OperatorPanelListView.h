// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class OperatorPanelListView sealed : public Windows::UI::Xaml::Controls::ListView
        {
        public:
            OperatorPanelListView();

        protected:
            virtual void OnApplyTemplate() override;

        private:
            void OnScrollClick(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnPointerEntered(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void OnPointerExited(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void OnButtonPointerExited(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
            void ScrollViewChanged(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs^ args);
            void ShowHideScrollButtons(Windows::UI::Xaml::Visibility vLeft, Windows::UI::Xaml::Visibility vRight);
            void UpdateScrollButtons();

            void ScrollLeft();
            void ScrollRight();

            double scrollRatio = 0.7;

            bool m_isPointerEntered;

            Windows::UI::Xaml::Controls::ItemsPresenter^ m_content;
            Windows::UI::Xaml::Controls::ScrollViewer^ m_scrollViewer;
            Windows::UI::Xaml::Controls::Button^ m_scrollLeft;
            Windows::UI::Xaml::Controls::Button^ m_scrollRight;
        };
    }
}
