// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// NumberPad.xaml.h
// Declaration of the NumberPad class
//

#pragma once

#include "Views/NumberPad.g.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"
#include "CalcManager/Header Files/RadixType.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden] public ref class NumberPad sealed
    {
    public:
        NumberPad();
        DEPENDENCY_PROPERTY_OWNER(NumberPad);

        DEPENDENCY_PROPERTY(Windows::UI::Xaml::Style ^, ButtonStyle);
        DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(int, CurrentRadixType, safe_cast<int>(::RADIX_TYPE::DEC_RADIX));

        property bool IsErrorVisualState
        {
            bool get();
            void set(bool value);
        }

    private:
        void OnCurrentRadixTypePropertyChanged(int oldValue, int newValue);


        bool m_isErrorVisualState;
    };
}
