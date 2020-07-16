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
        void BitshiftFlyout_Checked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void FlyoutButton_Clicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void CollapseBitshiftButtons();
        void LoadResourceStrings();

        bool m_isErrorVisualState;
        void OpenParenthesisButton_GotFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ClearEntryButton_LostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void ClearButton_LostFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        Platform::String ^ m_arithmeticShiftButtonContent;
        Platform::String ^ m_logicalShiftButtonContent;
        Platform::String ^ m_rotateCircularButtonContent;
        Platform::String ^ m_rotateCarryShiftButtonContent;
    };
}
