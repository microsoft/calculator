// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingCalculatorViewModel.h"

using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Data;
using namespace GraphControl;

namespace CalculatorApp::ViewModel
{
    GraphingCalculatorViewModel::GraphingCalculatorViewModel()
        : m_IsDecimalEnabled{ true }
        , m_Equations{ ref new Vector<EquationViewModel ^>() }
        , m_Variables{ ref new Vector<VariableViewModel ^>() }
    {
    }

    void GraphingCalculatorViewModel::OnButtonPressed(Object ^ parameter)
    {
    }

    void GraphingCalculatorViewModel::UpdateVariables(IMap<String ^, Variable ^> ^ variables)
    {
        Variables->Clear();
        for (auto variablePair : variables)
        {
            auto variable = ref new VariableViewModel(variablePair->Key, variablePair->Value);
            variable->VariableUpdated += ref new EventHandler<VariableChangedEventArgs>([this, variable](Object ^ sender, VariableChangedEventArgs e) {
                VariableUpdated(variable, VariableChangedEventArgs{ e.variableName, e.newValue });
            });
            Variables->Append(variable);
        }
    }

    void GraphingCalculatorViewModel::SetSelectedEquation(EquationViewModel ^ equation)
    {
        SelectedEquation = equation;
    }
}
