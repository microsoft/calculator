// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// HistoryList.xaml.cpp
// Implementation of the HistoryList class
//

#include "pch.h"
#include "HistoryList.xaml.h"
#include "CalcViewModel/Common/LocalizationService.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace std;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;

namespace MUXC = Microsoft::UI::Xaml::Controls;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=390556

DEPENDENCY_PROPERTY_INITIALIZATION(HistoryList, RowHeight);

HistoryList::HistoryList()
{
    InitializeComponent();

    HistoryEmpty->FlowDirection = LocalizationService::GetInstance()->GetFlowDirection();
}

void HistoryList::HistoryList_Loaded(Object^ sender, RoutedEventArgs^ e)
{
    // When transitioning between docked and undocked view states, the history list is
    // unloaded and then loaded, so we attempt to create the titlebarhelper every time
    // we are loaded, letting the util function check if we are docked or not.
    m_titleBarHelper = TitleBarHelper::CreateTitleBarHelperIfNotDocked(CustomTitleBar);
}

void HistoryList::HistoryList_Unloaded(Object^ sender, RoutedEventArgs^ e)
{
    m_titleBarHelper = nullptr;
}

void HistoryList::ListView_ItemClick(_In_ Object^ sender, _In_ ItemClickEventArgs^ e)
{
    HistoryViewModel^ historyVM = static_cast<HistoryViewModel^>(this->DataContext);
    HistoryItemViewModel^ clickedItem = safe_cast<HistoryItemViewModel^>(e->ClickedItem);

    // When the user clears the history list in the overlay view and presses enter, the clickedItem is nullptr
    if (clickedItem != nullptr)
    {
        historyVM->ShowItem(clickedItem);
    }
}

void HistoryList::OnDeleteMenuItemClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    auto clickedItem = safe_cast<HistoryItemViewModel^>(safe_cast<FrameworkElement^>(sender)->DataContext);

    Model->DeleteItem(clickedItem);
}

void HistoryList::OnDeleteSwipeInvoked(_In_ MUXC::SwipeItem^ sender, _In_ MUXC::SwipeItemInvokedEventArgs^ e)
{
    auto swipedItem = safe_cast<HistoryItemViewModel^>(e->SwipeControl->DataContext);

    Model->DeleteItem(swipedItem);
}

void HistoryList::ScrollToBottom()
{
    auto historyItems = this->HistoryListView->Items;
    if (historyItems->Size > 0)
    {
        this->HistoryListView->ScrollIntoView(historyItems->GetAt(historyItems->Size - 1));
    }
}
