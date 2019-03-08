// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// HistoryList.xaml.h
// Declaration of the HistoryList class
//

#pragma once

#include "Views/HistoryList.g.h"
#include "Common/TitleBarHelper.h"
#include "Converters/ItemSizeToVisibilityConverter.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel/HistoryViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class HistoryList sealed
    {
    public:
        HistoryList();
        property CalculatorApp::ViewModel::HistoryViewModel^ Model
        {
            CalculatorApp::ViewModel::HistoryViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::HistoryViewModel^>(this->DataContext);
            }
        }

        void ScrollToBottom();
        DEPENDENCY_PROPERTY_OWNER(HistoryList);
        DEPENDENCY_PROPERTY(Windows::UI::Xaml::GridLength, RowHeight);

    private:
        Windows::Foundation::Rect m_visibleBounds;
        Windows::Foundation::Rect m_coreBounds;
        void ListView_ItemClick(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
        void OnDeleteMenuItemClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnDeleteSwipeInvoked(_In_ Microsoft::UI::Xaml::Controls::SwipeItem^ sender, _In_ Microsoft::UI::Xaml::Controls::SwipeItemInvokedEventArgs^ e);

        void HistoryList_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void HistoryList_Unloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        std::unique_ptr<CalculatorApp::Common::TitleBarHelper> m_titleBarHelper;
    };
}
