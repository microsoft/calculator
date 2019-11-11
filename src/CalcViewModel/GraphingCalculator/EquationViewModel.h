#pragma once

#include "../Common/Utils.h"

namespace CalculatorApp::ViewModel
{
public
    ref class EquationViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        EquationViewModel(GraphControl::Equation ^ equation);

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(GraphControl::Equation ^, GraphEquation);
        OBSERVABLE_PROPERTY_RW(Windows::UI::Xaml::Visibility, KeyGraphFeaturesVisibility);

        property Platform::String ^ Expression
        {
            Platform::String ^ get()
            {
                return GraphEquation->Expression;
            }
            void set(Platform::String ^ value)
            {
                if (GraphEquation->Expression != value)
                {
                    GraphEquation->Expression = value;
                    RaisePropertyChanged("Expression");
                }
            }
        }

        property Windows::UI::Xaml::Media::SolidColorBrush ^ LineColor
        {
            Windows::UI::Xaml::Media::SolidColorBrush ^ get()
            {
                return GraphEquation->LineColor;
            }
            void set(Windows::UI::Xaml::Media::SolidColorBrush ^ value)
            {
                if (GraphEquation->LineColor != value)
                {
                    GraphEquation->LineColor = value;
                    RaisePropertyChanged("LineColor");
                }
            }
        }
    };
}
