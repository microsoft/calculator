#include "pch.h"
#include "GraphingCalculatorViewModel.h"

using namespace CalculatorApp::ViewModel;
using namespace Platform::Collections;

namespace CalculatorApp::ViewModel
{
    GraphingCalculatorViewModel::GraphingCalculatorViewModel()
        : m_IsDecimalEnabled{ true }
        , m_Equations{ ref new Vector< EquationViewModel^ >() }
    {
    }

    void GraphingCalculatorViewModel::OnButtonPressed(Object^ parameter)
    {
    }
}
