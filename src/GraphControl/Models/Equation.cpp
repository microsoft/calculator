// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Equation.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;

namespace GraphControl
{
    // Remove mml: formatting specific to RichEditBox control, which is not understood by the graph engine.
    static constexpr wstring_view s_mathPrefix = L"mml:";

    Equation::Equation()
    {
    }

    String ^ Equation::GetRequest()
    {
        wstring request;
        wstring_view expr{ Expression->Data() };
        IsInequality = false;

        // Check for unicode characters of less than, less than or equal to, greater than and greater than or equal to.
        if (expr.find(L">&#x3E;<") != wstring_view::npos || expr.find(L">&#x3C;<") != wstring_view::npos || expr.find(L">&#x2265;<") != wstring_view::npos
            || expr.find(L">&#x2264;<") != wstring_view::npos || expr.find(8805) != wstring_view::npos || expr.find(8804) != wstring_view::npos
            || expr.find(L">&lt;<") != wstring_view::npos || expr.find(L">&gt;<") != wstring_view::npos)
        {
            request = L"<mrow><mi>plotIneq2D</mi><mfenced separators=\"\">";
            IsInequality = true;
            EquationStyle = EquationLineStyle::Dash;
        }
        else if (expr.find(L">=<") != wstring_view::npos)
        {
            request = L"<mrow><mi>plotEq2d</mi><mfenced separators=\"\">";
        }
        // If the expression contains both x and y but no equal or inequality sign, then that cannot be graphed.
        else if (expr.find(L">x<") != wstring_view::npos && (expr.find(L">y<") != wstring_view::npos))
        {
            return nullptr;
        }
        // Else default to plot2d
        else
        {
            request = L"<mrow><mi>plot2d</mi><mfenced separators=\"\">";
        }
        request += GetExpression();
        request += L"</mfenced></mrow>";

        return ref new String(request.c_str());
    }

    wstring Equation::GetExpression()
    {
        wstring mathML = Expression->Data();

        size_t mathPrefix = 0;
        while ((mathPrefix = mathML.find(s_mathPrefix, mathPrefix)) != wstring::npos)
        {
            mathML.replace(mathPrefix, s_mathPrefix.length(), L"");
            mathPrefix += s_mathPrefix.length();
        }

        return mathML;
    }

    Color Equation::LineColor::get()
    {
        return m_LineColor;
    }
    void Equation::LineColor::set(Color value)
    {
        if (m_LineColor.R != value.R || m_LineColor.G != value.G || m_LineColor.B != value.B || m_LineColor.A != value.A)
        {
            m_LineColor = value;
            RaisePropertyChanged(L"LineColor");
        }
    }

    Platform::String ^ Equation::LineColorPropertyName::get()
    {
        return Platform::StringReference(L"LineColor");
    }

    bool Equation::IsGraphableEquation()
    {
        return !Expression->IsEmpty() && IsLineEnabled && !HasGraphError;
    }
}
