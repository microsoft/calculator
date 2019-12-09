// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingNumPad.xaml.h"
#include "Views/NumberPad.xaml.h"

using namespace CalculatorApp;

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

GraphingNumPad::GraphingNumPad()
{
    InitializeComponent();
}

void GraphingNumPad::ShiftButton_Check(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs ^ /*e*/)
{
    SetOperatorRowVisibility();
}

void GraphingNumPad::ShiftButton_Uncheck(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs ^ /*e*/)
{
    ShiftButton->IsChecked = false;
    SetOperatorRowVisibility();
    ShiftButton->Focus(::FocusState::Programmatic);
}

void GraphingNumPad::TrigFlyoutShift_Toggle(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs ^ /*e*/)
{
    SetTrigRowVisibility();
}

void GraphingNumPad::TrigFlyoutHyp_Toggle(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs ^ /*e*/)
{
    SetTrigRowVisibility();
}

void GraphingNumPad::FlyoutButton_Clicked(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Xaml::RoutedEventArgs ^ /*e*/)
{
    this->HypButton->IsChecked = false;
    this->TrigShiftButton->IsChecked = false;
    this->Trigflyout->Hide();
    this->FuncFlyout->Hide();
    this->InequalityFlyout->Hide();
}

void GraphingNumPad::ShiftButton_IsEnabledChanged(_In_ Platform::Object ^ /*sender*/, _In_ Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ /*e*/)
{
    SetOperatorRowVisibility();
}

void GraphingNumPad::SetTrigRowVisibility()
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

void GraphingNumPad::SetOperatorRowVisibility()
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
