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
            PROGRAMMATIC
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
        ref class MathRichEditBoxFormatRequest sealed
        {
        public:
            PROPERTY_R(Platform::String^, OriginalText);
            PROPERTY_RW(Platform::String ^, FormattedText);

        public:
            MathRichEditBoxFormatRequest(Platform::String^ originalText)
                : m_OriginalText(originalText)
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

            event Windows::Foundation::EventHandler<MathRichEditBoxFormatRequest ^> ^ FormatRequest;
            event Windows::Foundation::EventHandler<MathRichEditBoxSubmission^> ^ EquationSubmitted;
            void OnMathTextPropertyChanged(Platform::String ^ oldValue, Platform::String ^ newValue);
            void InsertText(Platform::String ^ text, int cursorOffSet, int selectionLength);
            void SubmitEquation(EquationSubmissionSource source);
            void BackSpace();

        protected:
            void OnKeyDown(Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e) override;

        private:
            Platform::String ^ GetMathTextProperty();
            void SetMathTextProperty(Platform::String ^ newValue);

            void OnLosingFocus(Windows::UI::Xaml::UIElement ^ sender, Windows::UI::Xaml::Input::LosingFocusEventArgs ^ args);
            void OnKeyUp(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e);
        };
    }
}
