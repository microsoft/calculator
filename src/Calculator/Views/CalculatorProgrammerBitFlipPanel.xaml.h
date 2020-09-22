// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorProgrammerBitFlipPanel.xaml.h
// Declaration of the CalculatorProgrammerBitFlipPanel class
//

#pragma once

#include "Views/CalculatorProgrammerBitFlipPanel.g.h"
#include "Controls/FlipButtons.h"
#include "CalcViewModel/StandardCalculatorViewModel.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class CalculatorProgrammerBitFlipPanel sealed
    {
    public:
        CalculatorProgrammerBitFlipPanel();

        bool ShouldEnableBit(CalculatorApp::Common::BitLength length, int index);

        property CalculatorApp::ViewModel::StandardCalculatorViewModel
            ^ Model { CalculatorApp::ViewModel::StandardCalculatorViewModel ^ get(); } 

            private : void OnLoaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);
        void OnUnloaded(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e);

        void SubscribePropertyChanged();
        void UnsubscribePropertyChanged();
        void OnPropertyChanged(Platform::Object ^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs ^ e);

        void AssignFlipButtons();

        void OnBitToggled(_In_ Platform::Object ^ sender, _In_ Windows::UI::Xaml::RoutedEventArgs ^ e);

    private:
        Windows::Foundation::EventRegistrationToken m_propertyChangedToken;
        Platform::String ^ GenerateAutomationPropertiesName(int position, bool value);
        void UpdateCheckedStates(bool updateAutomationPropertiesNames);
        void UpdateAutomationPropertiesNames();
        int GetIndexOfLastBit(CalculatorApp::Common::BitLength length) const;

        static const unsigned int s_numBits = 64;
        std::array<CalculatorApp::Controls::FlipButtons ^, s_numBits> m_flipButtons;
        bool m_updatingCheckedStates;
        CalculatorApp::Common::BitLength m_currentValueBitLength;
    };
}
