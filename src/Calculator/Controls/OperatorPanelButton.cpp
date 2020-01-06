// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "OperatorPanelButton.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;

DEPENDENCY_PROPERTY_INITIALIZATION(OperatorPanelButton, Text);
DEPENDENCY_PROPERTY_INITIALIZATION(OperatorPanelButton, Glyph);
DEPENDENCY_PROPERTY_INITIALIZATION(OperatorPanelButton, GlyphFontSize);
DEPENDENCY_PROPERTY_INITIALIZATION(OperatorPanelButton, ChevronFontSize);
DEPENDENCY_PROPERTY_INITIALIZATION(OperatorPanelButton, FlyoutMenu);

void OperatorPanelButton::OnApplyTemplate()
{
    if (FlyoutMenu != nullptr)
    {
        FlyoutMenu->Closed += ref new EventHandler<Object ^>(this, &OperatorPanelButton::FlyoutClosed);
    }
}

void OperatorPanelButton::OnToggle()
{
    ToggleButton::OnToggle();

    if (IsChecked)
    {
        FlyoutMenu->ShowAt(this);
    }
}

void OperatorPanelButton::FlyoutClosed(Object ^ sender, Object ^ args)
{
    IsChecked = false;
}
