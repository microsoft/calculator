// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/MemoryListItem.g.h"
#include "CalcViewModel/MemoryItemViewModel.h"

namespace CalculatorApp
{
    /// <summary>
    /// Represents a single item in the Memory list.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class MemoryListItem sealed
    {
    public:
        MemoryListItem();

        DEPENDENCY_PROPERTY_OWNER(MemoryListItem);
        DEPENDENCY_PROPERTY(CalculatorApp::ViewModel::MemoryItemViewModel^, Model);

    protected:
        void OnPointerEntered(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) override;
        void OnPointerExited(Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e) override;

    private:
        void OnClearButtonClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnMemoryAddButtonClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnMemorySubtractButtonClicked(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnClearSwipeInvoked(_In_ Windows::UI::Xaml::Controls::SwipeItem^ sender, Windows::UI::Xaml::Controls::SwipeItemInvokedEventArgs^ e);
        void OnMemoryAddSwipeInvoked(_In_ Windows::UI::Xaml::Controls::SwipeItem^ sender, Windows::UI::Xaml::Controls::SwipeItemInvokedEventArgs^ e);
        void OnMemorySubtractSwipeInvoked(_In_ Windows::UI::Xaml::Controls::SwipeItem^ sender, Windows::UI::Xaml::Controls::SwipeItemInvokedEventArgs^ e);

    };
}
