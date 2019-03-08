// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MemoryListItem.xaml.h"
#include "Controls/CalculatorButton.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;

DEPENDENCY_PROPERTY_INITIALIZATION(MemoryListItem, Model);

MemoryListItem::MemoryListItem()
{
    InitializeComponent();
}

void MemoryListItem::OnPointerEntered(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    Control::OnPointerEntered(e);

    // Only show hover buttons when the user is using mouse or pen.
    if (e->Pointer->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Mouse || e->Pointer->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Pen)
    {
        VisualStateManager::GoToState(this, "MemoryButtonsVisible", true);
    }
}

void MemoryListItem::OnPointerExited(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
    Control::OnPointerExited(e);

    VisualStateManager::GoToState(this, "MemoryButtonsHidden", true);
}

void MemoryListItem::OnClearButtonClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    Model->Clear();
}

void MemoryListItem::OnMemoryAddButtonClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    Model->MemoryAdd();
}

void MemoryListItem::OnMemorySubtractButtonClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    Model->MemorySubtract();
}

void MemoryListItem::OnClearSwipeInvoked(_In_ SwipeItem^ sender, SwipeItemInvokedEventArgs^ e)
{
    Model->Clear();
}

void MemoryListItem::OnMemoryAddSwipeInvoked(_In_ SwipeItem^ sender, SwipeItemInvokedEventArgs^ e)
{
    Model->MemoryAdd();
}

void MemoryListItem::OnMemorySubtractSwipeInvoked(_In_ SwipeItem^ sender, SwipeItemInvokedEventArgs^ e)
{
    Model->MemorySubtract();
}

