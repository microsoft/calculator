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
        wstringstream ss;
        wstring expr{ Expression->Data() };

        // Check for unicode characters of less than, less than or equal to, greater than and greater than or equal to.
        if (expr.find(L">&#x3E;<") != wstring::npos || expr.find(L">&#x3C;<") != wstring::npos || expr.find(L">&#x2265;<") != wstring::npos
            || expr.find(L">&#x2264;<") != wstring::npos)
        {
            return L"<mrow><mi>plotIneq2D</mi><mfenced separators=\"\">"s;
        }
        else if (expr.find(L">=<") != wstring::npos)
        {
            ss << L"<mrow><mi>plotEq2d</mi><mfenced separators=\"\">";
        }
        else
        {
            ss << L"<mrow><mi>plot2d</mi><mfenced separators=\"\">";
        }
        ss << GetExpression() << L"</mfenced></mrow>";

        return ref new String(ss.str().c_str());
    }

    wstring Equation::GetExpression()
    {
        wstring mathML = Expression->Data();

        size_t mathPrefix = 0;
        while ((mathPrefix = mathML.find(s_mathPrefix, mathPrefix)) != std::string::npos)
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
