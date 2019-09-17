#pragma once

#include "../Common/Utils.h"
#include "EquationViewModel.h"

namespace CalculatorApp::ViewModel
{

        [Windows::UI::Xaml::Data::Bindable] public ref class ViewStateViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        ViewStateViewModel(int xMin, int yMin, int xMax, int yMax)
            : m_XMin(xMin)
            , m_YMin(yMin)
            , m_XMax(xMax)
            , m_YMax(yMax)
        {
        }

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);

        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(int, XMin);
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(int, YMin);
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(int, XMax);
        OBSERVABLE_PROPERTY_RW_ALWAYS_NOTIFY(int, YMax);

    private:
        void OnPropertyChanged(Platform::String ^ propertyName)
        {
            OutputDebugString(L"OnPropertyChanged");
            // VariableUpdated(this, VariableChangedEventArgs{ Name, Value });
        }
    };




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
        //OBSERVABLE_PROPERTY_R(ViewStateViewModel^, ViewState);

        property ViewStateViewModel^ ViewState
        {
            ViewStateViewModel^ get() {return m_ViewState;}
            void set(ViewStateViewModel^ value)
            {
                m_ViewState = value;
            }

        }


        COMMAND_FOR_METHOD(ButtonPressed, GraphingCalculatorViewModel::OnButtonPressed);

        event Windows::Foundation::EventHandler<VariableChangedEventArgs> ^ VariableUpdated;

        void UpdateVariables(Windows::Foundation::Collections::IMap<Platform::String ^, double> ^ variables);

    private:
        void OnButtonPressed(Platform::Object ^ parameter);
        ViewStateViewModel ^ m_ViewState;

    };


}
