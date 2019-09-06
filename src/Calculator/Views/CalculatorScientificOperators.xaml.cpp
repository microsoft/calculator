// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificOperators.xaml.cpp
// Implementation of the CalculatorScientificOperators class
//

#include "pch.h"
#include "CalculatorScientificOperators.xaml.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "Controls/CalculatorButton.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;

using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core;

DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorScientificOperators, IsErrorVisualState);

CalculatorScientificOperators::CalculatorScientificOperators()
{
    InitializeComponent();

    ExpButton->SetValue(Common::KeyboardShortcutManager::VirtualKeyProperty, Common::MyVirtualKey::E);
    Common::KeyboardShortcutManager::ShiftButtonChecked(false);
}

void CalculatorScientificOperators::OnIsErrorVisualStatePropertyChanged(bool /*oldValue*/, bool newValue)
{
    String ^ newState = newValue ? L"ErrorLayout" : L"NoErrorLayout";
    VisualStateManager::GoToState(this, newState, false);
    NumberPad->IsErrorVisualState = newValue;
}

void CalculatorScientificOperators::shiftButton_Check(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    SetOperatorRowVisibility();
}

void CalculatorScientificOperators::shiftButton_Uncheck(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    ShiftButton->IsChecked = false;
    SetOperatorRowVisibility();
    ShiftButton->Focus(::FocusState::Programmatic);
}

void CalculatorScientificOperators::trigFlyoutShift_Toggle(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    SetTrigRowVisibility();
    Common::KeyboardShortcutManager::ShiftButtonChecked(TrigShiftButton->IsEnabled && TrigShiftButton->IsChecked->Value);
}

void CalculatorScientificOperators::trigFlyoutHyp_Toggle(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    SetTrigRowVisibility();
}

void CalculatorScientificOperators::flyoutButton_Clicked(_In_ Platform::Object^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs^ /*e*/)
{
    this->HypButton->IsChecked = false;
    this->TrigShiftButton->IsChecked = false;
    this->Trigflyout->Hide();
    this->FuncFlyout->Hide();
}

void CalculatorScientificOperators::shiftButton_IsEnabledChanged(
    _In_ Platform::Object ^ /*sender*/,
    _In_ Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ /*e*/)
{
    SetOperatorRowVisibility();
}

void CalculatorScientificOperators::SetTrigRowVisibility()
{
    bool isShiftChecked = TrigShiftButton->IsChecked->Value;
    bool isHypeChecked = HypButton->IsChecked->Value;

    InverseHyperbolicTrigFunctions->Visibility = ::Visibility::Collapsed;
    InverseTrigFunctions->Visibility = ::Visibility::Collapsed;
    HyperbolicTrigFunctions->Visibility = ::Visibility::Collapsed;
    TrigFunctions->Visibility = ::Visibility::Collapsed;

    if (isShiftChecked && isHypeChecked)
    {
        InverseHyperbolicTrigFunctions->Visibility = ::Visibility::Visible;
    }
    else if (isShiftChecked && !isHypeChecked)
    {
        InverseTrigFunctions->Visibility = ::Visibility::Visible;
    }
    else if (!isShiftChecked && isHypeChecked)
    {
        HyperbolicTrigFunctions->Visibility = ::Visibility::Visible;
    }
    else
    {
        TrigFunctions->Visibility = ::Visibility::Visible;
    }
}

void CalculatorScientificOperators::SetOperatorRowVisibility()
{
    ::Visibility rowVis, invRowVis;
    if (ShiftButton->IsChecked->Value)
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
    InvRow1->Visibility = invRowVis;
}

void CalculatorScientificOperators::OpenParenthesisButton_GotFocus(Object ^ sender, RoutedEventArgs ^ e)
{
    Model->SetOpenParenthesisCountNarratorAnnouncement();
}

String ^ CalculatorScientificOperators::ParenthesisCountToString(unsigned int count)
{
    return (count == 0) ? ref new String() : ref new String(to_wstring(count).data());
}
