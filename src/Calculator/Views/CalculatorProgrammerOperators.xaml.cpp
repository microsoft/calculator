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

    CopyMenuItem->Text = AppResourceProvider::GetInstance().GetResourceString(L"copyMenuItem");
}

void CalculatorProgrammerOperators::HexButtonChecked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    TraceLogger::GetInstance().LogRadixButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    if (Model)
    {
        Model->SwitchProgrammerModeBase(RADIX_TYPE::HEX_RADIX);
    }
}

void CalculatorProgrammerOperators::DecButtonChecked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    TraceLogger::GetInstance().LogRadixButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    if (Model)
    {
        Model->SwitchProgrammerModeBase(RADIX_TYPE::DEC_RADIX);
    }
}

void CalculatorProgrammerOperators::OctButtonChecked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    TraceLogger::GetInstance().LogRadixButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    if (Model)
    {
        Model->SwitchProgrammerModeBase(RADIX_TYPE::OCT_RADIX);
    }
}

void CalculatorProgrammerOperators::BinButtonChecked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    TraceLogger::GetInstance().LogRadixButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    if (Model)
    {
        Model->SwitchProgrammerModeBase(RADIX_TYPE::BIN_RADIX);
    }
}

void CalculatorProgrammerOperators::SetRadixButton(RADIX_TYPE radixType)
{
    switch (radixType)
    {
    case RADIX_TYPE::DEC_RADIX:
    {
        decimalButton->IsChecked = true;
        break;
    }
    case RADIX_TYPE::HEX_RADIX:
    {
        hexButton->IsChecked = true;
        break;
    }
    case RADIX_TYPE::OCT_RADIX:
    {
        octButton->IsChecked = true;
        break;
    }
    case RADIX_TYPE::BIN_RADIX:
    {
        binaryButton->IsChecked = true;
        break;
    }
    default:
        assert(false);
    }
}

void CalculatorProgrammerOperators::OnCopyMenuItemClicked(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    auto source = safe_cast<RadixButton^>(ProgrammerOperatorsContextMenu->Target);

    CopyPasteManager::CopyToClipboard(source->GetRawDisplayValue());
}
