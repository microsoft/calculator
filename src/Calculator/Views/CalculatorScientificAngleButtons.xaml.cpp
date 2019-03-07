// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificAngleButtons.xaml.cpp
// Implementation of the CalculatorScientificAngleButtons class
//

#include "pch.h"
#include "CalculatorScientificAngleButtons.xaml.h"
#include "CalcViewModel/Common/TraceLogger.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

CalculatorScientificAngleButtons::CalculatorScientificAngleButtons() :
    m_isErrorVisualState(false)
{
    InitializeComponent();
}

void CalculatorScientificAngleButtons::HypButton_Toggled(_In_ Object^ sender, _In_ RoutedEventArgs^ e)
{
    TraceLogger::GetInstance().LogHypButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
}

void CalculatorScientificAngleButtons::FToEButton_Toggled(_In_ Object^ sender,_In_ RoutedEventArgs^ e)
{
    auto viewModel = safe_cast<StandardCalculatorViewModel^>(this->DataContext);
    viewModel->FtoEButtonToggled();
}

void CalculatorApp::CalculatorScientificAngleButtons::OnAngleButtonPressed(_In_ Object^ commandParameter)
{
    TraceLogger::GetInstance().LogAngleButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    String^ buttonId = static_cast<String^>(commandParameter);

    degreeButton->Visibility = ::Visibility::Collapsed;
    radianButton->Visibility = ::Visibility::Collapsed;
    gradsButton->Visibility = ::Visibility::Collapsed;

    if (buttonId == L"0")
    {
        Model->SwitchAngleType(NumbersAndOperatorsEnum::Radians);
        radianButton->Visibility = ::Visibility::Visible;
        radianButton->Focus(::FocusState::Programmatic);
    }
    else if (buttonId == L"1")
    {
        Model->SwitchAngleType(NumbersAndOperatorsEnum::Grads);
        gradsButton->Visibility = ::Visibility::Visible;
        gradsButton->Focus(::FocusState::Programmatic);
    }
    else if (buttonId == L"2")
    {
        Model->SwitchAngleType(NumbersAndOperatorsEnum::Degree);
        degreeButton->Visibility = ::Visibility::Visible;
        degreeButton->Focus(::FocusState::Programmatic);
    }
}

bool CalculatorScientificAngleButtons::IsErrorVisualState::get()
{
    return m_isErrorVisualState;
}

void CalculatorScientificAngleButtons::IsErrorVisualState::set(bool value)
{
    if (m_isErrorVisualState != value)
    {
        m_isErrorVisualState = value;
        String^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}
