// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/CalculatorProgrammerDisplayPanel.g.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CalculatorProgrammerDisplayPanel sealed
    {
    public:
        CalculatorProgrammerDisplayPanel();

        COMMAND_FOR_METHOD(BitLengthButtonPressed, CalculatorProgrammerDisplayPanel::OnBitLengthButtonPressed);

        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::StandardCalculatorViewModel^>(this->DataContext);
            }
        }

        property bool IsErrorVisualState {
            bool get();
            void set(bool value);
        }

    private:
        void ShowBitFlip(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnBitLengthButtonPressed(Platform::Object^ parameter);

        bool m_isErrorVisualState;
    };
}
