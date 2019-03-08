// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculatorButton.h"
#include "CalcViewModel/Common/CalculatorButtonPressedEventArgs.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Data;
using namespace Platform;
using namespace Windows::Foundation::Collections;

DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorButton, ButtonId);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorButton, AuditoryFeedback);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorButton, HoverBackground);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorButton, HoverForeground);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorButton, PressBackground);
DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorButton, PressForeground);

CalculatorButton::CalculatorButton()
{
    // Set the default bindings for this button, these can be overwritten by Xaml if needed
    // These are a replacement for binding in styles
    Binding^ commandBinding = ref new Binding();
    commandBinding->Path = ref new PropertyPath("ButtonPressed");
    this->SetBinding(Button::CommandProperty, commandBinding);
}

void CalculatorButton::OnKeyDown(KeyRoutedEventArgs^ e)
{
    // Ignore the Enter key
    if (e->Key == VirtualKey::Enter)
    {
        return;
    }

    Button::OnKeyDown(e);
}

void CalculatorButton::OnKeyUp(KeyRoutedEventArgs^ e)
{
    // Ignore the Enter key
    if (e->Key == VirtualKey::Enter)
    {
        return;
    }

    Button::OnKeyUp(e);
}

void CalculatorButton::OnButtonIdPropertyChanged(
    NumbersAndOperatorsEnum /*oldValue*/, 
    NumbersAndOperatorsEnum newValue)
{
    this->CommandParameter = ref new CalculatorButtonPressedEventArgs(AuditoryFeedback, newValue);
}

void CalculatorButton::OnAuditoryFeedbackPropertyChanged(
    String^ /*oldValue*/,
    String^ newValue)
{
    this->CommandParameter = ref new CalculatorButtonPressedEventArgs(newValue, ButtonId);
}
