// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorProgrammerBitFlipPanel.xaml.h
// Declaration of the CalculatorProgrammerBitFlipPanel class
//

#pragma once

#include "Views/CalculatorProgrammerBitFlipPanel.g.h"
#include "Controls/FlipButtons.h"
#include "Converters/BitFlipAutomationNameConverter.h"
#include "Converters/BooleanNegationConverter.h"
#include "Converters/VisibilityNegationConverter.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CalculatorProgrammerBitFlipPanel sealed
    {
    public:
        CalculatorProgrammerBitFlipPanel();

        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get();
        }

    private:
        void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void SubscribePropertyChanged();
        void UnsubscribePropertyChanged();
        void OnPropertyChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ e);

        void AssignFlipButtons();

        void SetVisibilityBinding(_In_ Windows::UI::Xaml::FrameworkElement^ element, _In_ Platform::String^ path, _In_ Windows::UI::Xaml::Data::IValueConverter^ converter);
        void OnBitToggled(_In_ Platform::Object^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs^ e);
        void UpdateCheckedStates();

    private:
        Windows::Foundation::EventRegistrationToken m_propertyChangedToken;

        static const unsigned int s_numBits = 64;
        std::array<CalculatorApp::Controls::FlipButtons^, s_numBits> m_flipButtons;
        bool m_updatingCheckedStates;
    };
}
