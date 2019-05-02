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
        ref class CalculatorButton sealed : Windows::UI::Xaml::Controls::Button
        {
        public:
            CalculatorButton();
            DEPENDENCY_PROPERTY_OWNER(CalculatorButton);

            DEPENDENCY_PROPERTY_WITH_CALLBACK(NumbersAndOperatorsEnum, ButtonId);
            DEPENDENCY_PROPERTY_WITH_CALLBACK(Platform::String ^, AuditoryFeedback);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, HoverBackground);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, HoverForeground);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, PressBackground);
            DEPENDENCY_PROPERTY(Windows::UI::Xaml::Media::Brush ^, PressForeground);

        protected:
            virtual void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) override;
            virtual void OnKeyUp(Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) override;

        private:
            void OnButtonIdPropertyChanged(NumbersAndOperatorsEnum oldValue, NumbersAndOperatorsEnum newValue);
            void OnAuditoryFeedbackPropertyChanged(Platform::String ^ oldValue, Platform::String ^ newValue);
        };
    }
}
