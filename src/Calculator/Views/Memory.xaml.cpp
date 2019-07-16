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

Memory::Memory()
    : m_isErrorVisualState(false)
{
    InitializeComponent();

    MemoryPaneEmpty->FlowDirection = LocalizationService::GetInstance()->GetFlowDirection();
}

void Memory::MemoryListItemClick(_In_ Object ^ sender, _In_ ItemClickEventArgs ^ e)
{
    MemoryItemViewModel ^ memorySlot = safe_cast<MemoryItemViewModel ^>(e->ClickedItem);

    // In case the memory list is clicked and enter is pressed,
    // On Item clicked event gets fired and e->ClickedItem is Null.
    if (memorySlot != nullptr)
    {
        Model->OnMemoryItemPressed(memorySlot->Position);
    }
}

void Memory::OnClearMenuItemClicked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    auto memoryItem = GetMemoryItemForCurrentFlyout();
    if (memoryItem != nullptr)
    {
        memoryItem->Clear();
    }
}

void Memory::OnMemoryAddMenuItemClicked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    auto memoryItem = GetMemoryItemForCurrentFlyout();
    if (memoryItem != nullptr)
    {
        memoryItem->MemoryAdd();
    }
}

void Memory::OnMemorySubtractMenuItemClicked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    auto memoryItem = GetMemoryItemForCurrentFlyout();
    if (memoryItem != nullptr)
    {
        memoryItem->MemorySubtract();
    }
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
        String ^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}

MemoryItemViewModel ^ Memory::GetMemoryItemForCurrentFlyout()
{
    auto listViewItem = MemoryContextMenu->Target;
    return dynamic_cast<MemoryItemViewModel ^>(MemoryListView->ItemFromContainer(listViewItem));
}
