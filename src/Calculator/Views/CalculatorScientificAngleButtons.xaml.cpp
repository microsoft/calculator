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

CalculatorScientificAngleButtons::CalculatorScientificAngleButtons()
    : m_isErrorVisualState(false)
{
    InitializeComponent();
}

void CalculatorScientificAngleButtons::HypButton_Toggled(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    TraceLogger::GetInstance().LogHypButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
}

void CalculatorScientificAngleButtons::FToEButton_Toggled(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
<<<<<<< HEAD
    auto viewModel = safe_cast<StandardCalculatorViewModel ^>(this->DataContext);
    viewModel->FtoEButtonToggled();
=======
    auto viewModel = safe_cast<StandardCalculatorViewModel^>(this->DataContext);
    if (!viewModel->IsFToEAuto)
    {
        viewModel->FtoEButtonToggled();
    }
    else if (viewModel->IsFToEAuto && !viewModel->IsFToEChecked)
    {
        std::wstring displayValue(viewModel->DisplayValue->Data());
        if (displayValue.find(L"e") != std::wstring::npos)
        {
            viewModel->IsFToEAuto = false;
            viewModel->FtoEButtonToggled();
            viewModel->FtoEButtonToggled();
        }
    }
>>>>>>> 8591c856c4765ceabb9ec7820bff0c37d5f4862a
}

void CalculatorApp::CalculatorScientificAngleButtons::OnAngleButtonPressed(_In_ Object ^ commandParameter)
{
    TraceLogger::GetInstance().LogAngleButtonUsed(ApplicationView::GetApplicationViewIdForWindow(CoreWindow::GetForCurrentThread()));
    String ^ buttonId = static_cast<String ^>(commandParameter);

    DegreeButton->Visibility = ::Visibility::Collapsed;
    RadianButton->Visibility = ::Visibility::Collapsed;
    GradsButton->Visibility = ::Visibility::Collapsed;

    if (buttonId == L"0")
    {
        Model->SwitchAngleType(NumbersAndOperatorsEnum::Radians);
        RadianButton->Visibility = ::Visibility::Visible;
        RadianButton->Focus(::FocusState::Programmatic);
    }
    else if (buttonId == L"1")
    {
        Model->SwitchAngleType(NumbersAndOperatorsEnum::Grads);
        GradsButton->Visibility = ::Visibility::Visible;
        GradsButton->Focus(::FocusState::Programmatic);
    }
    else if (buttonId == L"2")
    {
        Model->SwitchAngleType(NumbersAndOperatorsEnum::Degree);
        DegreeButton->Visibility = ::Visibility::Visible;
        DegreeButton->Focus(::FocusState::Programmatic);
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
        String ^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
    }
}
