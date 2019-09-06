// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/CalculatorProgrammerRadixOperators.g.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class CalculatorProgrammerRadixOperators sealed
    {
    public:
        CalculatorProgrammerRadixOperators();

        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::StandardCalculatorViewModel^>(this->DataContext);
            }
        }

        property bool IsErrorVisualState
        {
            bool get();
            void set(bool value);
        }
        Platform::String ^ ParenthesisCountToString(unsigned int count);

        DEPENDENCY_PROPERTY_OWNER(CalculatorProgrammerRadixOperators);

        void checkDefaultBitShift();

    private:
        void bitshiftFlyout_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void flyoutButton_Clicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void collapseBitshiftButtons();

        bool m_isErrorVisualState;
        void OpenParenthesisButton_GotFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
    };
}
