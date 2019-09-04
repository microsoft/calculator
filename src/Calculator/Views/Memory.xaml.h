// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// Memory.xaml.h
// Declaration of the Memory class
//

#pragma once

#include "Views/Memory.g.h"
#include "Converters/BooleanNegationConverter.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class Memory sealed
    {
    public:
        Memory();
        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::StandardCalculatorViewModel^>(this->DataContext);
            }
        }

        DEPENDENCY_PROPERTY_OWNER(Memory);
        DEPENDENCY_PROPERTY(Windows::UI::Xaml::GridLength, RowHeight);

        property bool IsErrorVisualState
        {
            bool get();
            void set(bool value);
        }

    private:
        Windows::Foundation::Rect m_visibleBounds;
        Windows::Foundation::Rect m_coreBounds;
        bool m_isErrorVisualState;

        void MemoryListItemClick(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::Controls::ItemClickEventArgs ^ e);
        void OnClearMenuItemClicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnMemoryAddMenuItemClicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnMemorySubtractMenuItemClicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);

        CalculatorApp::ViewModel::MemoryItemViewModel ^ GetMemoryItemForCurrentFlyout();
    };
}
