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

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);

        OBSERVABLE_PROPERTY_R(Platform::String ^, Name);

        // TODO: Consider removing this work around and manually set the textbox text.
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(double, Value);
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(double, Min);
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(double, Step);
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(double, Max);
        OBSERVABLE_PROPERTY_RW(bool, SliderSettingsVisible);

        event Windows::Foundation::EventHandler<VariableChangedEventArgs> ^ VariableUpdated;

        void SetValue(double value)
        {
            if (value < Min)
            {
                value = Min;
            }
            else if (value > Max)
            {
                value = Max;
            }

            Value = value;
        }

    private:
        void OnPropertyChanged(Platform::String ^ propertyName)
        {
            if (propertyName == "Value")
            {
                VariableUpdated(this, VariableChangedEventArgs{ Name, Value });
            }
        }
    };

    [Windows::UI::Xaml::Data::Bindable] public ref class GraphingCalculatorViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GraphingCalculatorViewModel();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(bool, IsDecimalEnabled);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<EquationViewModel ^> ^, Equations);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<VariableViewModel ^> ^, Variables);

        COMMAND_FOR_METHOD(ButtonPressed, GraphingCalculatorViewModel::OnButtonPressed);

        event Windows::Foundation::EventHandler<VariableChangedEventArgs> ^ VariableUpdated;

        void UpdateVariables(Windows::Foundation::Collections::IMap<Platform::String ^, double> ^ variables);

    private:
        void OnButtonPressed(Platform::Object ^ parameter);
    };
}
