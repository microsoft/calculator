// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Views/OperatorsPanel.g.h"
#include "Views/CalculatorStandardOperators.xaml.h"
#include "Views/CalculatorScientificOperators.xaml.h"
#include "Views/CalculatorProgrammerBitFlipPanel.xaml.h"
#include "Views/CalculatorProgrammerRadixOperators.xaml.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class OperatorsPanel sealed
    {
    public:
        OperatorsPanel();
        property CalculatorApp::ViewModel::StandardCalculatorViewModel^ Model
        {
            CalculatorApp::ViewModel::StandardCalculatorViewModel^ get();
        }

        DEPENDENCY_PROPERTY_OWNER(OperatorsPanel);
        DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsBitFlipChecked);
        DEPENDENCY_PROPERTY_WITH_CALLBACK(bool, IsErrorVisualState);

        void EnsureProgrammerBitFlipPanel();
        void EnsureScientificOps();
        void EnsureProgrammerRadixOps();

    private:
        void OnIsBitFlipCheckedPropertyChanged(bool oldValue, bool newValue);
        void OnIsErrorVisualStatePropertyChanged(bool oldValue, bool newValue);
    };
}
