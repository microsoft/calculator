// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "CalcViewModel/Common/Utils.h"

namespace CalculatorApp
{
    namespace Controls
    {
    public
        enum class EquationSubmissionSource
        {
            FOCUS_LOST,
            ENTER_KEY,
        };

    public
        ref class MathRichEditBoxSubmission sealed
        {
        public:
            PROPERTY_R(bool, HasTextChanged);
            PROPERTY_R(EquationSubmissionSource, Source);
        public:
            MathRichEditBoxSubmission(bool hasTextChanged, EquationSubmissionSource source)
                : m_HasTextChanged(hasTextChanged)
                , m_Source(source)
            {
            }
        };

    public
        ref class MathRichEditBox sealed : Windows::UI::Xaml::Controls::RichEditBox
        {
        public:
            MathRichEditBox();

            DEPENDENCY_PROPERTY_OWNER(MathRichEditBox);
            DEPENDENCY_PROPERTY_WITH_DEFAULT_AND_CALLBACK(Platform::String ^, MathText, L"");

            event Windows::Foundation::EventHandler<MathRichEditBoxSubmission^> ^ EquationSubmitted;
            void OnMathTextPropertyChanged(Platform::String ^ oldValue, Platform::String ^ newValue);

        private:
            Platform::String ^ GetMathTextProperty();
            void SetMathTextProperty(Platform::String ^ newValue);

            void OnLosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
            void OnKeyUp(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        };
    }
}
