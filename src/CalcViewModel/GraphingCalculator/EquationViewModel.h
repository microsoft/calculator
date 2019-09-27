#pragma once

#include "../Common/Utils.h"

namespace CalculatorApp::ViewModel
{
public
    ref class EquationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        EquationViewModel();

        OBSERVABLE_OBJECT_CALLBACK(OnPropertyChanged);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Expression);
        OBSERVABLE_PROPERTY_RW(Windows::UI::Color, LineColor);

        // Key Graph Features
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Domain);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Range);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, XIntercept);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, YIntercept);
        OBSERVABLE_PROPERTY_RW(int, Parity);
        OBSERVABLE_PROPERTY_R(Platform::String ^, ParityString);
        OBSERVABLE_PROPERTY_RW(int, Periodicity);
        OBSERVABLE_PROPERTY_R(Platform::String ^, PeriodicityString);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Minima);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Maxima);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, InflectionPoints);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Monotonicity);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, VerticalAsymptotes);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, HorizontalAsymptotes);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, ObliqueAsymptotes);

    private:
        void OnPropertyChanged(Platform::String ^ prop);
        void SetParityStringProperty();
        void SetPeriodicityStringProperty();

    };
}
