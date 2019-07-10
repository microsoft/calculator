#pragma once

#include "../Common/Utils.h"
#include "EquationViewModel.h"

namespace CalculatorApp::ViewModel
{
    public value struct VariableChangedEventArgs sealed
    {
        Platform::String^ variableName;
        double newValue;
    };

    [Windows::UI::Xaml::Data::Bindable]
    public ref class VariableViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        VariableViewModel(Platform::String^ name, double value) :
            m_Name(name),
            m_Value(value),
            m_SliderSettingsVisible(false),
            m_Min(0.0),
            m_Step(0.1),
            m_Max(2.0)
        { }

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_R(Platform::String^, Name);
        OBSERVABLE_PROPERTY_RW(double, Value);
        OBSERVABLE_PROPERTY_RW(bool, SliderSettingsVisible);
        OBSERVABLE_PROPERTY_RW(double, Min);
        OBSERVABLE_PROPERTY_RW(double, Step);
        OBSERVABLE_PROPERTY_RW(double, Max);

        event Windows::Foundation::EventHandler<VariableChangedEventArgs>^ VariableUpdated;

    private:
        void OnPropertyChanged(Platform::String^ propertyName)
        {
            VariableUpdated(this, VariableChangedEventArgs{ Name, Value });
        }
    };

    [Windows::UI::Xaml::Data::Bindable]
    public ref class GraphingCalculatorViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GraphingCalculatorViewModel();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(bool, IsDecimalEnabled);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector< EquationViewModel^ >^, Equations);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector< VariableViewModel^ >^, Variables);

        COMMAND_FOR_METHOD(ButtonPressed, GraphingCalculatorViewModel::OnButtonPressed);

        event Windows::Foundation::EventHandler<VariableChangedEventArgs>^ VariableUpdated;

        void UpdateVariables(Windows::Foundation::Collections::IMap<Platform::String^, double>^ variables);
    private:
        void OnButtonPressed(Platform::Object^ parameter);
    };
}
