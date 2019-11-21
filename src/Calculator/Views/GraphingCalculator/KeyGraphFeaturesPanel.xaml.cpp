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
}

void KeyGraphFeaturesPanel::OnPropertyChanged(String ^ propertyName)
{
    if (propertyName == L"ViewModel")
    {
        if (ViewModel == nullptr)
        {
            return;
        }

        SetEquationTextBoxProperties();
    }
}

void KeyGraphFeaturesPanel::EquationButtonClicked(Object ^ sender, RoutedEventArgs ^ e)
{
    KeyGraphFeaturesClosed(this, ref new RoutedEventArgs());
}

void KeyGraphFeaturesPanel::EquationInputTextBox_Loaded(Object ^ sender, RoutedEventArgs ^ e)
{
    if (ViewModel != nullptr)
    {
        SetEquationTextBoxProperties();
    }
}

void KeyGraphFeaturesPanel::SetEquationTextBoxProperties()
{
    EquationInputTextBox->SetEquationText(ViewModel->Expression);
}
