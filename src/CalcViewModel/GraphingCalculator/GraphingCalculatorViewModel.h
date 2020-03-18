// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "../Common/Utils.h"
#include "EquationViewModel.h"
#include "VariableViewModel.h"

namespace CalculatorApp::ViewModel
{
    [Windows::UI::Xaml::Data::Bindable] public ref class GraphingCalculatorViewModel sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        GraphingCalculatorViewModel();

        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_R(bool, IsDecimalEnabled);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<EquationViewModel ^> ^, Equations);
        OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<VariableViewModel ^> ^, Variables);
        OBSERVABLE_PROPERTY_R(EquationViewModel ^, SelectedEquation);

        COMMAND_FOR_METHOD(ButtonPressed, GraphingCalculatorViewModel::OnButtonPressed);

        event Windows::Foundation::EventHandler<VariableChangedEventArgs> ^ VariableUpdated;

        void UpdateVariables(Windows::Foundation::Collections::IMap<Platform::String ^, GraphControl::Variable ^> ^ variables);

        void SetSelectedEquation(EquationViewModel ^ equation);
    private:
        void OnButtonPressed(Platform::Object ^ parameter);
    };
}
