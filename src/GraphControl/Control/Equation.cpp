#include "pch.h"
#include "Equation.h"

using namespace Platform;
using namespace std;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;

namespace GraphControl
{
    // Remove mml: formatting specific to RichEditBox control, which is not understood by the graph engine.
    static constexpr wstring_view s_mathPrefix = L"mml:";

    DependencyProperty^ Equation::s_expressionProperty;
    static constexpr auto s_propertyName_Expression = L"Expression";

    DependencyProperty^ Equation::s_lineColorProperty;
    static constexpr auto s_propertyName_LineColor = L"LineColor";

    namespace EquationProperties
    {
        String^ Expression = StringReference(s_propertyName_Expression);
        String^ LineColor = StringReference(s_propertyName_LineColor);
    }

    void Equation::RegisterDependencyProperties()
    {
        if (!s_expressionProperty)
        {
            s_expressionProperty = DependencyProperty::Register(
                EquationProperties::Expression,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(
                    nullptr,
                    ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_lineColorProperty)
        {
            // Default line color should be the user's accent color
            auto uiSettings = ref new UISettings();
            Color accentColor = uiSettings->GetColorValue(UIColorType::Accent);

            s_lineColorProperty = DependencyProperty::Register(
                EquationProperties::LineColor,
                Color::typeid,
                Equation::typeid,
                ref new PropertyMetadata(
                    accentColor,
                    ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }
    }

    void Equation::OnCustomDependencyPropertyChanged(DependencyObject^ obj, DependencyPropertyChangedEventArgs^ args)
    {
        if (auto eq = static_cast<Equation^>(obj))
        {
            String^ propertyName = nullptr;
            if (args->Property == s_expressionProperty)
            {
                propertyName = EquationProperties::Expression;
            }
            else if (args->Property == s_lineColorProperty)
            {
                propertyName = EquationProperties::LineColor;
            }

            eq->PropertyChanged(eq, propertyName);
        }
    }

    wstring Equation::GetRequest()
    {
        wstringstream ss{};
        ss  << GetRequestHeader()
            << GetExpression()
            << GetLineColor()
            << L"</mfenced></mrow>";

        return ss.str();
    }

    wstring Equation::GetRequestHeader()
    {
        wstring expr{ Expression->Data() };
        if (expr.find(L">=<") != wstring::npos)
        {
            return L"<mrow><mi>plotEq2d</mi><mfenced separators=\"\">"s;
        }
        else
        {
            return L"<mrow><mi>plot2d</mi><mfenced separators=\"\">"s;
        }
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

    wstring Equation::GetLineColor()
    {
        return L""s;
    }
}
