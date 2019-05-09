// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "FlipButtons.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Controls;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Data;
using namespace Platform;
using namespace Windows::Foundation::Collections;

DEPENDENCY_PROPERTY_INITIALIZATION(FlipButtons, ButtonId);
DEPENDENCY_PROPERTY_INITIALIZATION(FlipButtons, HoverBackground);
DEPENDENCY_PROPERTY_INITIALIZATION(FlipButtons, HoverForeground);
DEPENDENCY_PROPERTY_INITIALIZATION(FlipButtons, PressBackground);
DEPENDENCY_PROPERTY_INITIALIZATION(FlipButtons, PressForeground);

void FlipButtons::OnKeyDown(KeyRoutedEventArgs ^ e)
{
    // Ignore the Enter key
    if (e->Key == VirtualKey::Enter)
    {
        return;
    }

    ToggleButton::OnKeyDown(e);
}

void FlipButtons::OnKeyUp(KeyRoutedEventArgs ^ e)
{
    // Ignore the Enter key
    if (e->Key == VirtualKey::Enter)
    {
        return;
    }

    ToggleButton::OnKeyUp(e);
}

void FlipButtons::OnButtonIdPropertyChanged(NumbersAndOperatorsEnum /*oldValue*/, NumbersAndOperatorsEnum newValue)
{
    this->CommandParameter = newValue;
}
