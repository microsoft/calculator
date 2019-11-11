#include "pch.h"
#include "EquationViewModel.h"

using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace GraphControl;

namespace CalculatorApp::ViewModel
{
    EquationViewModel::EquationViewModel(GraphControl::Equation ^ equation)
        : m_KeyGraphFeaturesVisibility{ ::Visibility::Collapsed }
    {
        GraphEquation = equation;
    }
}
