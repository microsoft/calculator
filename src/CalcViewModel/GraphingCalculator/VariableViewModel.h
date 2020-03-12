// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../Common/Utils.h"
#include "EquationViewModel.h"

namespace CalculatorApp::ViewModel
{
public
    value struct VariableChangedEventArgs sealed
    {
        Platform::String ^ variableName;
        double newValue;
    };

    [Windows::UI::Xaml::Data::Bindable] public ref class VariableViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        VariableViewModel(Platform::String ^ name, double value)
            : m_Name(name)
            , m_Value(value)
            , m_SliderSettingsVisible(false)
            , m_Min(0.0)
            , m_Step(0.1)
            , m_Max(2.0)
        {
        }

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(Platform::String ^, Name);
        OBSERVABLE_PROPERTY_RW(double, Min);
        OBSERVABLE_PROPERTY_RW(double, Step);
        OBSERVABLE_PROPERTY_RW(double, Max);
        OBSERVABLE_PROPERTY_RW(bool, SliderSettingsVisible);

        event Windows::Foundation::EventHandler<VariableChangedEventArgs> ^ VariableUpdated;

        property double Value
        {
            double get()
            {
                return m_Value;
            }
            void set(double value)
            {
                if (value < Min)
                {
                    Min = value;
                    RaisePropertyChanged(L"Min");
                }
                else if (value > Max)
                {
                    Max = value;
                    RaisePropertyChanged(L"Max");
                }

                if (Value != value)
                {
                    m_Value = value;
                    VariableUpdated(this, VariableChangedEventArgs{ Name, value });
                    RaisePropertyChanged(L"Value");
                }
            }
        }

    private:
        double m_Value;
    };
}
