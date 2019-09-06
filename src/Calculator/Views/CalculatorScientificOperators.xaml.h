// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificOperators.xaml.h
// Declaration of the CalculatorScientificOperators class
//

#pragma once

#include "Views/CalculatorScientificOperators.g.h"
#include "Views/NumberPad.xaml.h"
#include "Converters/BooleanNegationConverter.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class CalculatorScientificOperators sealed
    {
    public:
        CalculatorScientificOperators();
        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get() {
                return static_cast<CalculatorApp::ViewModel::StandardCalculatorViewModel^>(this->DataContext);
            }
        }

        DEPENDENCY_PROPERTY_OWNER(CalculatorScientificOperators);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(bool, IsErrorVisualState, false);

        void OpenParenthesisButton_GotFocus(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        Platform::String ^ ParenthesisCountToString(unsigned int count);

    private:
        void OnIsErrorVisualStatePropertyChanged(bool oldValue, bool newValue);
        void shiftButton_Check(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void shiftButton_Uncheck(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void trigFlyoutShift_Toggle(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void trigFlyoutHyp_Toggle(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void flyoutButton_Clicked(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);
        void shiftButton_IsEnabledChanged(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::DependencyPropertyChangedEventArgs ^ e);
        void SetOperatorRowVisibility();
        void SetTrigRowVisibility();
    };
}
