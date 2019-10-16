#include "pch.h"
#include "EquationViewModel.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;

namespace CalculatorApp::ViewModel
{
    EquationViewModel::EquationViewModel()
        : m_LineColor{ nullptr }
        , m_Expression{ "" }
        , m_Domain{ "" }
        , m_Range{ "" }
        , m_XIntercept{ "" }
        , m_YIntercept{ "" }
        , m_Parity{ -1 }
        , m_Periodicity{ ref new Map<String ^, String ^>() }
        , m_Minima{ ref new Vector<String ^>() }
        , m_Maxima{ ref new Vector<String ^>() }
        , m_InflectionPoints{ ref new Vector<String ^>() }
        , m_Monotonicity{ ref new Map<String ^, String ^>() }
        , m_VerticalAsymptotes{ ref new Vector<String ^>() }
        , m_HorizontalAsymptotes{ ref new Vector<String ^>() }
        , m_ObliqueAsymptotes{ ref new Vector<String ^>() }
        , m_TooComplexFeatures{ -1 }
        , m_AnalysisError{ 0 }
    {
    }
}
