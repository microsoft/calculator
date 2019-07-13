// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/Common/CalculatorButtonUser.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        ref class FlipButtons sealed : Windows::UI::Xaml::Controls::Primitives::ToggleButton
        {
        public:
            DEPENDENCY_PROPERTY_OWNER(FlipButtons);

            DEPENDENCY_PROPERTY_WITH_CALLBACK(NumbersAndOperatorsEnum, ButtonId);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, HoverBackground);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, HoverForeground);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, PressBackground);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, PressForeground);

        protected:
            virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) override;
            virtual void OnKeyUp(Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) override;

        private:
            void OnButtonIdPropertyChanged(NumbersAndOperatorsEnum oldValue, NumbersAndOperatorsEnum newValue);
        };
    }
}
