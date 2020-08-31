// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "OperatorsPanel.xaml.h"

using namespace CalculatorApp;
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
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Converters;

DEPENDENCY_PROPERTY_INITIALIZATION(OperatorsPanel, IsBitFlipChecked);
DEPENDENCY_PROPERTY_INITIALIZATION(OperatorsPanel, IsErrorVisualState);

OperatorsPanel::OperatorsPanel()
{
    InitializeComponent();
}

StandardCalculatorViewModel ^ OperatorsPanel::Model::get()
{
    return static_cast<CalculatorApp::ViewModel::StandardCalculatorViewModel ^>(this->DataContext);
}

void OperatorsPanel::OnIsBitFlipCheckedPropertyChanged(bool /*oldValue*/, bool newValue)
{
    if (newValue)
    {
        EnsureProgrammerBitFlipPanel();
    }
}

void OperatorsPanel::OnIsErrorVisualStatePropertyChanged(bool /*oldValue*/, bool newValue)
{
    if (Model->IsStandard)
    {
        StandardOperators->IsErrorVisualState = newValue;
    }
    else if (Model->IsScientific)
    {
        ScientificOperators->IsErrorVisualState = newValue;
    }
    else if (Model->IsProgrammer)
    {
        ProgrammerRadixOperators->IsErrorVisualState = newValue;
    }
}

void OperatorsPanel::EnsureScientificOps()
{
    if (!ScientificOperators)
    {
        this->FindName(L"ScientificOperators");
    }
}

void OperatorsPanel::EnsureProgrammerRadixOps()
{
    if (!ProgrammerRadixOperators)
    {
        this->FindName(L"ProgrammerRadixOperators");
    }

    ProgrammerRadixOperators->checkDefaultBitShift();
}

void OperatorsPanel::EnsureProgrammerBitFlipPanel()
{
    if (!BitFlipPanel)
    {
        this->FindName(L"BitFlipPanel");
    }
}
