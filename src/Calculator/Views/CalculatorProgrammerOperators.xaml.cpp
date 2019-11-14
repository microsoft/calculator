// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculatorProgrammerOperators.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/Common/CopyPasteManager.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace CalculatorApp::ViewModel;
using namespace Windows::Devices::Input;
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
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Core;

DEPENDENCY_PROPERTY_INITIALIZATION(CalculatorProgrammerOperators, SymbolButtonStyle);

CalculatorProgrammerOperators::CalculatorProgrammerOperators()
{
    InitializeComponent();

    CopyMenuItem->Text = AppResourceProvider::GetInstance()->GetResourceString(L"copyMenuItem");
}

void CalculatorProgrammerOperators::HexButtonChecked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::HexButton, ViewMode::Programmer);
    if (Model)
    {
        Model->SwitchProgrammerModeBase(NumberBase::HexBase);
    }
}

void CalculatorProgrammerOperators::DecButtonChecked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::DecButton, ViewMode::Programmer);
    if (Model)
    {
        Model->SwitchProgrammerModeBase(NumberBase::DecBase);
    }
}

void CalculatorProgrammerOperators::OctButtonChecked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::OctButton, ViewMode::Programmer);
    if (Model)
    {
        Model->SwitchProgrammerModeBase(NumberBase::OctBase);
    }
}

void CalculatorProgrammerOperators::BinButtonChecked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    TraceLogger::GetInstance()->UpdateButtonUsage(NumbersAndOperatorsEnum::BinButton, ViewMode::Programmer);
    if (Model)
    {
        Model->SwitchProgrammerModeBase(NumberBase::BinBase);
    }
}

void CalculatorProgrammerOperators::SetRadixButton(NumberBase numberBase)
{
    switch (numberBase)
    {
    case NumberBase::DecBase:
    {
        DecimalButton->IsChecked = true;
        break;
    }
    case NumberBase::HexBase:
    {
        HexButton->IsChecked = true;
        break;
    }
    case NumberBase::OctBase:
    {
        OctButton->IsChecked = true;
        break;
    }
    case NumberBase::BinBase:
    {
        BinaryButton->IsChecked = true;
        break;
    }
    default:
        assert(false);
    }
}

void CalculatorProgrammerOperators::OnCopyMenuItemClicked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    auto source = safe_cast<RadixButton ^>(ProgrammerOperatorsContextMenu->Target);

    CopyPasteManager::CopyToClipboard(source->GetRawDisplayValue());
}
