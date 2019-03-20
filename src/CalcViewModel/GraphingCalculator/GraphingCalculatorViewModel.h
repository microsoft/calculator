#pragma once

#include "../Common/Utils.h"
#include "EquationViewModel.h"

namespace CalculatorApp::ViewModel
{
    [Windows::UI::Xaml::Data::Bindable]
    public ref class GraphingCalculatorViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GraphingCalculatorViewModel();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(bool, IsDecimalEnabled);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector< EquationViewModel^ >^, Equations);

        COMMAND_FOR_METHOD(ButtonPressed, GraphingCalculatorViewModel::OnButtonPressed);

    private:
        void OnButtonPressed(Platform::Object^ parameter);
    };
}
