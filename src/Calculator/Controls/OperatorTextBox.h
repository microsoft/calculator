// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Controls
    {
        public ref class OperatorTextBox sealed : public Windows::UI::Xaml::Controls::TextBox
        {
        public:
            OperatorTextBox() { }

        protected:
            virtual void OnApplyTemplate() override;
        };
    }
}
