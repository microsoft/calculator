// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorStandardOperators.xaml.cpp
// Implementation of the CalculatorStandardOperators class
//

#include "pch.h"
#include "CalculatorStandardOperators.xaml.h"

using namespace CalculatorApp;

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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

CalculatorStandardOperators::CalculatorStandardOperators()
    : m_isErrorVisualState(false)
{
    InitializeComponent();
    this->SizeChanged += ref new SizeChangedEventHandler(this, &CalculatorStandardOperators::ShowHideStandardFunctions);
}

bool CalculatorStandardOperators::IsErrorVisualState::get()
{
    return m_isErrorVisualState;
}

void CalculatorStandardOperators::IsErrorVisualState::set(bool value)
{
    if (m_isErrorVisualState != value)
    {
        m_isErrorVisualState = value;
        String ^ newState = m_isErrorVisualState ? L"ErrorLayout" : L"NoErrorLayout";
        VisualStateManager::GoToState(this, newState, false);
        NumberPad->IsErrorVisualState = m_isErrorVisualState;
    }
}

void CalculatorStandardOperators::ShowHideStandardFunctions(Object ^ /*sender*/, SizeChangedEventArgs ^ /*e*/)
{
    Windows::Foundation::Rect bounds = Window::Current->Bounds;
    if (ApplicationView::GetForCurrentView()->ViewMode == ApplicationViewMode::CompactOverlay)
    {
        if (bounds.Width < 320 || bounds.Height < 394)
        {
            R0->Height = GridLength(0, GridUnitType::Star);
            StandardFunctions->Visibility = ::Visibility::Collapsed;
        }
        else
        {
            R0->Height = GridLength(1, GridUnitType::Star);
            StandardFunctions->Visibility = ::Visibility::Visible;
        }
    }
}
