// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificAngleButtons.xaml.h
// Declaration of the CalculatorScientificAngleButtons class
//

#pragma once

#include "Views/CalculatorScientificAngleButtons.g.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CalculatorScientificAngleButtons sealed
    {
    public:
        CalculatorScientificAngleButtons();
        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::StandardCalculatorViewModel^>(this->DataContext);
            }
        }

        COMMAND_FOR_METHOD(ButtonPressed, CalculatorScientificAngleButtons::OnAngleButtonPressed)

        property bool IsErrorVisualState {
            bool get();
            void set(bool value);
        }

    private:
        void OnAngleButtonPressed(_In_ Platform::Object^ commandParameter);
        void FToEButton_Toggled(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void HypButton_Toggled(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);

        bool m_isErrorVisualState;
	};
}
