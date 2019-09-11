#include "pch.h"
#include "EquationViewModel.h"

using namespace Windows::UI;
using namespace Windows::UI::Xaml;

namespace CalculatorApp::ViewModel
{
    EquationViewModel::EquationViewModel()
        : m_LineColor{ Colors::Transparent }, m_Expression{ "" }
        , m_Domain{ "" }
        , m_Range{ "" }
        , m_XIntercept{ "" }
        , m_YIntercept{ "" }
        , m_Minima{ "" }
        , m_Maxima{ "" }
        , m_InflectionPoints{ "" }
        , m_Monotonicity{ "" }
    {
    }
}
