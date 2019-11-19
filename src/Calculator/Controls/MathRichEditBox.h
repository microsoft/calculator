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

            static property Windows::UI::Xaml::DependencyProperty ^ MathTextProperty
            {
                Windows::UI::Xaml::DependencyProperty ^ get() {
                    return s_MathTextProperty;
                }
            }      
            property Platform::String ^ MathText
            {
                Platform::String ^ get()
                {
                    return GetMathTextProperty();
                }
                void set(Platform::String^ value)
                {
                    SetMathTextProperty(value);
                }
            
            }

        private :
            Platform::String ^ GetMathTextProperty();
            void SetMathTextProperty(Platform::String ^ newValue);

            static Windows::UI::Xaml::DependencyProperty ^ s_MathTextProperty;
            static Windows::UI::Xaml::DependencyProperty ^ InitializeMathTextProperty()
            {
                return Utils::RegisterDependencyProperty<DependencyPropertiesOwner, Platform::String ^>(L"MathText");
            }
        };
    }
}
