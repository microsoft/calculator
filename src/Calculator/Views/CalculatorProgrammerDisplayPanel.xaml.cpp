// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculatorProgrammerDisplayPanel.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"

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
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Core;

CalculatorProgrammerDisplayPanel::CalculatorProgrammerDisplayPanel() :
    m_isErrorVisualState(false)
{
    InitializeComponent();
}

void CalculatorProgrammerDisplayPanel::ShowBitFlip(Object^ sender, RoutedEventArgs^ e)
{
    TraceLogger::GetInstance().LogBitFlipPaneClicked();
}

void CalculatorProgrammerDisplayPanel::OnBitLengthButtonPressed(Object^ parameter)
{
    TraceLogger::GetInstance().LogBitLengthButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    String^ buttonId = parameter->ToString();

    qwordButton->Visibility = ::Visibility::Collapsed;
    dwordButton->Visibility = ::Visibility::Collapsed;
    wordButton->Visibility = ::Visibility::Collapsed;
    byteButton->Visibility = ::Visibility::Collapsed;
    if (buttonId == "0")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Dword);
        dwordButton->Visibility = ::Visibility::Visible;
        dwordButton->Focus(::FocusState::Programmatic);
        Model->IsQwordEnabled = false;
        Model->IsDwordEnabled = true;
        Model->IsWordEnabled = true;
    }
    else if (buttonId == "1")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Word);
        wordButton->Visibility = ::Visibility::Visible;
        wordButton->Focus(::FocusState::Programmatic);
        Model->IsQwordEnabled = false;
        Model->IsDwordEnabled = false;
        Model->IsWordEnabled = true;
    }
    else if (buttonId == "2")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Byte);
        byteButton->Visibility = ::Visibility::Visible;
        byteButton->Focus(::FocusState::Programmatic);
        Model->IsQwordEnabled = false;
        Model->IsDwordEnabled = false;
        Model->IsWordEnabled = false;
    }
    else if (buttonId == "3")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Qword);
        qwordButton->Visibility = ::Visibility::Visible;
        qwordButton->Focus(::FocusState::Programmatic);
        Model->IsQwordEnabled = true;
        Model->IsDwordEnabled = true;
        Model->IsWordEnabled = true;
    }
    // update memory list according to bit length
    Model->SetMemorizedNumbersString();
}

bool CalculatorProgrammerDisplayPanel::IsErrorVisualState::get()
{
    return m_isErrorVisualState;
}

void CalculatorProgrammerDisplayPanel::IsErrorVisualState::set(bool value)
{
    if (m_isErrorVisualState != value)
    {
        m_isErrorVisualState = value;
        String^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}
