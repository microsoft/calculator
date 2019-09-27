// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/GraphingCalculator/EquationViewModel.h"
#include "Calculator/Controls/EquationTextBox.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class KeyGraphFeaturesControl sealed : public Windows::UI::Xaml::Controls::Control
        {
        public:
            KeyGraphFeaturesControl()
            {
            }

        protected:
            virtual void OnApplyTemplate() override;
        private:

        };
    }
}
