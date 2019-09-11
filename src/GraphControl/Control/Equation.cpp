#include "pch.h"
#include "Equation.h"

using namespace Platform;
using namespace std;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;

namespace GraphControl
{
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

            s_lineColorProperty = DependencyProperty::Register(
                EquationProperties::LineColor,
                SolidColorBrush::typeid,
                Equation::typeid,
                ref new PropertyMetadata(
                    nullptr,
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
            << L")";

        return ss.str();
    }

    wstring Equation::GetRequestHeader()
    {
        wstring expr{ Expression->Data() };
        if (expr.find(L"=") != wstring::npos)
        {
            return L"plotEq2d("s;
        }
        else
        {
            return L"plot2d("s;
        }
    }

    wstring Equation::GetExpression()
    {
        return Expression->Data();
    }

    wstring Equation::GetLineColor()
    {
        return L""s;
    }
}
