// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "Utils.h"
#include <string>

namespace GraphControl
{
    [Windows::UI::Xaml::Data::Bindable] public ref class Equation sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        Equation();

        OBSERVABLE_OBJECT();
        OBSERVABLE_NAMED_PROPERTY_RW(Platform::String ^, Expression);
        OBSERVABLE_NAMED_PROPERTY_RW(bool, IsLineEnabled);
        OBSERVABLE_NAMED_PROPERTY_RW(bool, IsValidated);
        OBSERVABLE_NAMED_PROPERTY_RW(bool, HasGraphError);

        property Windows::UI::Color LineColor
        {
            Windows::UI::Color get();
            void set(Windows::UI::Color value);
        }

        static property Platform::String
            ^ LineColorPropertyName { Platform::String ^ get(); }

            public : Platform::String
                     ^ GetRequest();

        bool IsGraphableEquation();

    private:
        std::wstring GetExpression();

    private:
        Windows::UI::Color m_LineColor;
    };
}
