// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class RadixButton sealed : public Windows::UI::Xaml::Controls::RadioButton
        {
        public:
            RadixButton(){};
            internal : Platform::String ^ GetRawDisplayValue();
        };
    }
}
