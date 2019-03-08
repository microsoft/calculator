// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// NumberPad.xaml.h
// Declaration of the NumberPad class
//

#pragma once

#include "Views/NumberPad.g.h"
#include "CalcViewModel/Common/KeyboardShortcutManager.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class NumberPad sealed
    {
    public:
        NumberPad();
        DEPENDENCY_PROPERTY_OWNER(NumberPad);

        DEPENDENCY_PROPERTY(Windows::UI::Xaml::Style^, ButtonStyle);

        property bool IsErrorVisualState
        {
            bool get();
            void set(bool value);
        }

        void ProgModeRadixChange();

    private:
        bool m_isErrorVisualState;
    };
}
