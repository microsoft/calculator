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

CalculatorProgrammerDisplayPanel::CalculatorProgrammerDisplayPanel()
    : m_isErrorVisualState(false)
{
    InitializeComponent();
}

void CalculatorProgrammerDisplayPanel::ShowBitFlip(Object ^ sender, RoutedEventArgs ^ e)
{
    TraceLogger::GetInstance().LogBitFlipPaneClicked();
}

void CalculatorProgrammerDisplayPanel::OnBitLengthButtonPressed(Object ^ parameter)
{
    TraceLogger::GetInstance().LogBitLengthButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    String ^ buttonId = parameter->ToString();

    QwordButton->Visibility = ::Visibility::Collapsed;
    DwordButton->Visibility = ::Visibility::Collapsed;
    WordButton->Visibility = ::Visibility::Collapsed;
    ByteButton->Visibility = ::Visibility::Collapsed;
    if (buttonId == "0")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Dword);
        DwordButton->Visibility = ::Visibility::Visible;
        DwordButton->Focus(::FocusState::Programmatic);
        Model->IsQwordEnabled = false;
        Model->IsDwordEnabled = true;
        Model->IsWordEnabled = true;
    }
    else if (buttonId == "1")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Word);
        WordButton->Visibility = ::Visibility::Visible;
        WordButton->Focus(::FocusState::Programmatic);
        Model->IsQwordEnabled = false;
        Model->IsDwordEnabled = false;
        Model->IsWordEnabled = true;
    }
    else if (buttonId == "2")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Byte);
        ByteButton->Visibility = ::Visibility::Visible;
        ByteButton->Focus(::FocusState::Programmatic);
        Model->IsQwordEnabled = false;
        Model->IsDwordEnabled = false;
        Model->IsWordEnabled = false;
    }
    else if (buttonId == "3")
    {
        Model->ButtonPressed->Execute(NumbersAndOperatorsEnum::Qword);
        QwordButton->Visibility = ::Visibility::Visible;
        QwordButton->Focus(::FocusState::Programmatic);
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
        String ^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}
