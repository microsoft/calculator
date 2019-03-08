// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificOperators.xaml.cpp
// Implementation of the CalculatorScientificOperators class
//

#include "pch.h"
#include "CalculatorScientificOperators.xaml.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "Controls/CalculatorButton.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorScientificOperators, IsErrorVisualState);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorScientificOperators, IsWideLayout);

CalculatorScientificOperators::CalculatorScientificOperators()
{
    InitializeComponent();

    expButton->SetValue(Common::KeyboardShortcutManager::VirtualKeyProperty, Common::MyVirtualKey::E);
    Common::KeyboardShortcutManager::ShiftButtonChecked(false);
}

void CalculatorScientificOperators::ShortLayout_Completed(_In_ Platform::Object^ /*sender*/, _In_ Platform::Object^ /*e*/)
{
    IsWideLayout = false;
    SetOperatorRowVisibility();
    Common::KeyboardShortcutManager::ShiftButtonChecked(Model->IsShiftChecked);
}

void CalculatorScientificOperators::WideLayout_Completed(_In_ Platform::Object^ /*sender*/, _In_ Platform::Object^ /*e*/)
{
    IsWideLayout = true;
    SetOperatorRowVisibility();
    Common::KeyboardShortcutManager::ShiftButtonChecked(Model->IsShiftChecked);
}

void CalculatorScientificOperators::OnIsErrorVisualStatePropertyChanged(bool /*oldValue*/, bool newValue)
{
    String^ newState = newValue ? L"ErrorLayout" : L"NoErrorLayout";
    VisualStateManager::GoToState(this, newState, false);
    NumberPad->IsErrorVisualState = newValue;
}

void CalculatorScientificOperators::shiftButton_Check(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    bool isChecked = shiftButton->IsChecked->Value;
    Model->IsShiftChecked = isChecked;
    Common::KeyboardShortcutManager::ShiftButtonChecked(isChecked);
    SetOperatorRowVisibility();
}

void CalculatorScientificOperators::shiftButton_IsEnabledChanged(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ /*e*/)
{
    SetOperatorRowVisibility();
    Common::KeyboardShortcutManager::ShiftButtonChecked(shiftButton->IsEnabled && shiftButton->IsChecked->Value);
}

void CalculatorScientificOperators::SetOperatorRowVisibility()
{
    ::Visibility rowVis, invRowVis;
    if (IsWideLayout)
    {
        rowVis = ::Visibility::Visible;
        invRowVis = ::Visibility::Visible;
    }
    else if (shiftButton->IsChecked->Value)
    {
        rowVis = ::Visibility::Collapsed;
        invRowVis = ::Visibility::Visible;
    }
    else
    {
        rowVis = ::Visibility::Visible;
        invRowVis = ::Visibility::Collapsed;
    }

    Row1->Visibility = rowVis;
    Row2->Visibility = rowVis;
    InvRow1->Visibility = invRowVis;
    InvRow2->Visibility = invRowVis;
}
