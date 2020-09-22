// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// HistoryList.xaml.cpp
// Implementation of the HistoryList class
//

#include "pch.h"
#include "HistoryList.xaml.h"
#include "CalcViewModel/Common/CopyPasteManager.h"
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

void HistoryList::ListView_ItemClick(_In_ Object ^ sender, _In_ ItemClickEventArgs ^ e)
{
    HistoryViewModel^ historyVM = dynamic_cast<HistoryViewModel ^>(this->DataContext);
    HistoryItemViewModel^ clickedItem = dynamic_cast<HistoryItemViewModel ^>(e->ClickedItem);

    // When the user clears the history list in the overlay view and presses enter, the clickedItem is nullptr
    if (clickedItem != nullptr && historyVM != nullptr)
    {
        historyVM->ShowItem(clickedItem);
    }
}

void HistoryList::OnCopyMenuItemClicked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    auto listViewItem = HistoryContextMenu->Target;
    auto itemViewModel = dynamic_cast<HistoryItemViewModel ^>(HistoryListView->ItemFromContainer(listViewItem));
    if (itemViewModel != nullptr)
    {
        CopyPasteManager::CopyToClipboard(itemViewModel->Result);
    }
}

void HistoryList::OnDeleteMenuItemClicked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    auto listViewItem = HistoryContextMenu->Target;
    auto itemViewModel = dynamic_cast<HistoryItemViewModel ^>(HistoryListView->ItemFromContainer(listViewItem));
    if (itemViewModel != nullptr)
    {
        Model->DeleteItem(itemViewModel);
    }
}

void HistoryList::OnDeleteSwipeInvoked(_In_ MUXC::SwipeItem ^ sender, _In_ MUXC::SwipeItemInvokedEventArgs ^ e)
{
    auto swipedItem = dynamic_cast<HistoryItemViewModel ^>(e->SwipeControl->DataContext);
    if (swipedItem != nullptr)
    {
        Model->DeleteItem(swipedItem);
    }
}

void HistoryList::ScrollToBottom()
{
    auto historyItems = this->HistoryListView->Items;
    if (historyItems->Size > 0)
    {
        this->HistoryListView->ScrollIntoView(historyItems->GetAt(historyItems->Size - 1));
    }
}
