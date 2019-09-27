#include "pch.h"
#include "EquationViewModel.h"

using namespace Platform;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;

namespace CalculatorApp::ViewModel
{
    EquationViewModel::EquationViewModel()
        : m_LineColor{ Colors::Transparent }
        , m_Expression{ "" }
        , m_Domain{ "" }
        , m_Range{ "" }
        , m_XIntercept{ "" }
        , m_YIntercept{ "" }
        , m_Parity{ -1 }
        , m_ParityString{ "" }
        , m_Periodicity{ -1 }
        , m_PeriodicityString{ "" }
        , m_Minima{ "" }
        , m_Maxima{ "" }
        , m_InflectionPoints{ "" }
        , m_Monotonicity{ "" }
        , m_VerticalAsymptotes{ "" }
        , m_HorizontalAsymptotes{ "" }
        , m_ObliqueAsymptotes{ "" }
    {
    }

    void EquationViewModel::OnPropertyChanged(String ^ prop)
    {
        if (prop == L"Parity")
        {
            SetParityStringProperty();
        }
        if (prop == L"Periodicity")
        {
            SetPeriodicityStringProperty();
        }
    }

    void EquationViewModel::SetParityStringProperty()
    {
        switch (Parity)
        {
        case 0:
            ParityString = L"The function parity is unknown";
            break;
        case 1:
            ParityString = L"The function is odd";
            break;
        case 2:
            ParityString = L"The function is even";
            break;
        case 3:
            ParityString = L"The function is neither even nor odd";
            break;
        default:
            ParityString = L"Unable to determine the parity of the function";
        }
    }

    void EquationViewModel::SetPeriodicityStringProperty()
    {
        switch (Periodicity)
        {
        case 0:
            PeriodicityString = L"The function periodicity is unknown";
            break;
        case 1:
            PeriodicityString = L"The function is periodic";
            break;
        case 2:
            PeriodicityString = L"The function is not periodic";
            break;
        default:
            PeriodicityString = L"Periodicity is not supported for this function";
        }
    }
}
