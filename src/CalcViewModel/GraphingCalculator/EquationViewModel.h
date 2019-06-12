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
        OBSERVABLE_PROPERTY_RW(Windows::UI::Xaml::Visibility, KeyGraphFeaturesVisibility);
    };
}
