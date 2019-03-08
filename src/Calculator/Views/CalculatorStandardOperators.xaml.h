// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorStandardOperators.xaml.h
// Declaration of the CalculatorStandardOperators class
//

#pragma once

#include "Views/CalculatorStandardOperators.g.h"
#include "Views/NumberPad.xaml.h"

namespace CalculatorApp
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class CalculatorStandardOperators sealed
    {
    public:
        CalculatorStandardOperators();

        DEPENDENCY_PROPERTY_OWNER(CalculatorStandardOperators);

        property bool IsErrorVisualState
        {
            bool get();
            void set(bool value);
        }

    private:
        bool m_isErrorVisualState;
    };
}
