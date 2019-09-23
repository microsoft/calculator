#pragma once

#include "../Common/Utils.h"

namespace CalculatorApp::ViewModel
{
    public ref class EquationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        EquationViewModel();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_RW(Platform::String^, Expression);
        OBSERVABLE_PROPERTY_RW(Windows::UI::Color, LineColor);

        // Key Graph Features
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Domain);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Range);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, XIntercept);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, YIntercept);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Parity);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Periodicity);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Minima);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Maxima);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, InflectionPoints);
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Monotonicity);
    };
}
