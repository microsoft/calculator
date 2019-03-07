// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// Memory.xaml.cpp
// Implementation of the Memory class
//

#include "pch.h"
#include "Memory.xaml.h"
#include "Controls/CalculatorButton.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/Common/LocalizationService.h"

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

DEPENDENCY_PROPERTY_INITIALIZATION(Memory, RowHeight);

Memory::Memory() :
    m_isErrorVisualState(false)
{
    InitializeComponent();
    m_memoryItemFlyout = safe_cast<MenuFlyout^>(Resources->Lookup("MemoryContextMenu"));

    MemoryPaneEmpty->FlowDirection = LocalizationService::GetInstance()->GetFlowDirection();
}

void Memory::MemoryListItemClick(_In_ Object^ sender, _In_ ItemClickEventArgs^ e)
{
    MemoryItemViewModel^ memorySlot = safe_cast<MemoryItemViewModel^>(e->ClickedItem);

    // In case the memory list is clicked and enter is pressed,
    // On Item clicked event gets fired and e->ClickedItem is Null.
    if (memorySlot != nullptr)
    {
        Model->OnMemoryItemPressed(memorySlot->Position);
    }
}

void Memory::OnContextRequested(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::Input::ContextRequestedEventArgs^ e)
{
    // Walk up the tree to find the ListViewItem.
    // There may not be one if the click wasn't on an item.
    auto requestedElement = safe_cast<FrameworkElement^>(e->OriginalSource);
    while ((requestedElement != sender) && !dynamic_cast<ListViewItem^>(requestedElement))
    {
        requestedElement = safe_cast<FrameworkElement^>(VisualTreeHelper::GetParent(requestedElement));
    }

    if (requestedElement != sender)
    {
        // The context menu request was for a ListViewItem.
        auto memorySlot = safe_cast<MemoryItemViewModel^>(MemoryListView->ItemFromContainer(requestedElement));
        Point point;
        if (e->TryGetPosition(requestedElement, &point))
        {
            m_memoryItemFlyout->ShowAt(requestedElement, point);
        }
        else
        {
            // Not invoked via pointer, so let XAML choose a default location.
            m_memoryItemFlyout->ShowAt(requestedElement);
        }

        e->Handled = true;
    }
}

void Memory::OnContextCanceled(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    m_memoryItemFlyout->Hide();
}

void Memory::OnClearMenuItemClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    GetMemoryItemForCurrentFlyout()->Clear();
}

void Memory::OnMemoryAddMenuItemClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    GetMemoryItemForCurrentFlyout()->MemoryAdd();
}

void Memory::OnMemorySubtractMenuItemClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    GetMemoryItemForCurrentFlyout()->MemorySubtract();
}

bool Memory::IsErrorVisualState::get()
{
    return m_isErrorVisualState;
}

void Memory::IsErrorVisualState::set(bool value)
{
    if (m_isErrorVisualState != value)
    {
        m_isErrorVisualState = value;
        String^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}

void Memory::MemoryList_Loaded(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    // When transitioning between docked and undocked view states, the memory list is
    // unloaded and then loaded, so we attempt to create the titlebarhelper every time
    // we are loaded, letting the util function check if we are docked or not.
    m_titleBarHelper = TitleBarHelper::CreateTitleBarHelperIfNotDocked(CustomTitleBar);
}

void Memory::MemoryList_Unloaded(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    m_titleBarHelper = nullptr;
}

MemoryItemViewModel^ Memory::GetMemoryItemForCurrentFlyout()
{
    auto listViewItem = m_memoryItemFlyout->Target;

    return safe_cast<MemoryItemViewModel^>(MemoryListView->ItemFromContainer(listViewItem));
}
