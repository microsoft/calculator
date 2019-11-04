// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class OperatorPanelButton sealed : Windows::UI::Xaml::Controls::Primitives::ToggleButton
        {
        public:
            OperatorPanelButton()
            {
            }

            DEPENDENCY_PROPERTY_OWNER(OperatorPanelButton);

            DEPENDENCY_PROPERTY(Platform::String^, Text);
            DEPENDENCY_PROPERTY(Platform::String^, Glyph);
            DEPENDENCY_PROPERTY(double, GlyphFontSize);
            DEPENDENCY_PROPERTY(double, ChevronFontSize);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Controls::Flyout^, FlyoutMenu);

        protected:
            virtual void OnApplyTemplate() override;
            virtual void OnToggle() override;

        private:
            void FlyoutClosed(Platform::Object^ sender, Platform::Object^ args);
        };
    }
}
