// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class MathRichEditBox sealed : Windows::UI::Xaml::Controls::RichEditBox
        {
        public:
            MathRichEditBox();

            DEPENDENCY_PROPERTY_OWNER(MathRichEditBox);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(Platform::String ^, Text);

        private:
            void OnTextPropertyChanged(Platform::String ^, Platform::String ^ newValue);
        };
    }
}
