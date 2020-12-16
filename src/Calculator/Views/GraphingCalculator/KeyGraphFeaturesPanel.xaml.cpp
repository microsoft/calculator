// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "KeyGraphFeaturesPanel.xaml.h"
#include "Controls/MathRichEditBox.h"
#include "CalcViewModel/GraphingCalculatorEnums.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

KeyGraphFeaturesPanel::KeyGraphFeaturesPanel()
{
    InitializeComponent();
    this->Loaded += ref new RoutedEventHandler(this, &KeyGraphFeaturesPanel::KeyGraphFeaturesPanel_Loaded);
}

void KeyGraphFeaturesPanel::KeyGraphFeaturesPanel_Loaded(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    BackButton->Focus(::FocusState::Programmatic);
}

void KeyGraphFeaturesPanel::BackButton_Click(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyGraphFeaturesClosed(this, ref new RoutedEventArgs());
}
