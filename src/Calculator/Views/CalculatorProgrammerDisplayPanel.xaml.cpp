// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculatorProgrammerDisplayPanel.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
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

void CalculatorProgrammerDisplayPanel::OnBitLengthButtonPressed(Object ^ parameter)
{
    String ^ buttonId = parameter->ToString();

    QwordButton->Visibility = ::Visibility::Collapsed;
    DwordButton->Visibility = ::Visibility::Collapsed;
    WordButton->Visibility = ::Visibility::Collapsed;
    ByteButton->Visibility = ::Visibility::Collapsed;
    if (buttonId == "0")
    {
        Model->ValueBitLength = BitLength::BitLengthDWord;
        DwordButton->Visibility = ::Visibility::Visible;
        DwordButton->Focus(::FocusState::Programmatic);
    }
    else if (buttonId == "1")
    {
        Model->ValueBitLength = BitLength::BitLengthWord;
        WordButton->Visibility = ::Visibility::Visible;
        WordButton->Focus(::FocusState::Programmatic);
    }
    else if (buttonId == "2")
    {
        Model->ValueBitLength = BitLength::BitLengthByte;
        ByteButton->Visibility = ::Visibility::Visible;
        ByteButton->Focus(::FocusState::Programmatic);
    }
    else if (buttonId == "3")
    {
        Model->ValueBitLength = BitLength::BitLengthQWord;
        QwordButton->Visibility = ::Visibility::Visible;
        QwordButton->Focus(::FocusState::Programmatic);
    }

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
