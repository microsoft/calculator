// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// NumberPad.xaml.h
// Declaration of the NumberPad class
//

#pragma once

#include "Views/NumberPad.g.h"
#include "Common/KeyboardShortcutManager.h"
#include "CalcViewModel/Common/NumberBase.h"
#include "CalcManager/Header Files/RadixType.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class NumberPad sealed
    {
    public:
        NumberPad();
        DEPENDENCY_PROPERTY_OWNER(NumberPad);

        DEPENDENCY_PROPERTY(Windows::UI::Xaml::Style ^, ButtonStyle);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(CalculatorApp::Common::NumberBase, CurrentRadixType, CalculatorApp::Common::NumberBase::DecBase);

        property bool IsErrorVisualState
        {
            bool get();
            void set(bool value);
        }

    private:
        void OnCurrentRadixTypePropertyChanged(CalculatorApp::Common::NumberBase oldValue, CalculatorApp::Common::NumberBase newValue);


        bool m_isErrorVisualState;
    };
}
