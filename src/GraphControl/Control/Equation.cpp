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

    DependencyProperty ^ Equation::s_expressionProperty;
    static constexpr auto s_propertyName_Expression = L"Expression";

    DependencyProperty ^ Equation::s_lineColorProperty;
    static constexpr auto s_propertyName_LineColor = L"LineColor";

    DependencyProperty ^ Equation::s_xInterceptProperty;
    static constexpr auto s_propertyName_XIntercept = L"XIntercept";

    DependencyProperty ^ Equation::s_yInterceptProperty;
    static constexpr auto s_propertyName_YIntercept = L"YIntercept";

    DependencyProperty ^ Equation::s_parityProperty;
    static constexpr auto s_propertyName_Parity = L"Parity";

    DependencyProperty ^ Equation::s_periodicityProperty;
    static constexpr auto s_propertyName_Periodicity = L"Periodicity";

    DependencyProperty ^ Equation::s_minimaProperty;
    static constexpr auto s_propertyName_Minima = L"Minima";

    DependencyProperty ^ Equation::s_maximaProperty;
    static constexpr auto s_propertyName_Maxima = L"Maxima";

    DependencyProperty ^ Equation::s_domainProperty;
    static constexpr auto s_propertyName_Domain = L"Domain";

    DependencyProperty ^ Equation::s_rangeProperty;
    static constexpr auto s_propertyName_Range = L"Range";

    DependencyProperty ^ Equation::s_inflectionPointsProperty;
    static constexpr auto s_propertyName_InflectionPoints = L"InflectionPoints";

    DependencyProperty ^ Equation::s_monotonicityProperty;
    static constexpr auto s_propertyName_Monotonicity = L"Monotonicity";

    namespace EquationProperties
    {
        String ^ Expression = StringReference(s_propertyName_Expression);
        String ^ LineColor = StringReference(s_propertyName_LineColor);
        String ^ XIntercept = StringReference(s_propertyName_XIntercept);
        String ^ YIntercept = StringReference(s_propertyName_YIntercept);
        String ^ Parity = StringReference(s_propertyName_Parity);
        String ^ Periodicity = StringReference(s_propertyName_Periodicity);
        String ^ Minima = StringReference(s_propertyName_Minima);
        String ^ Maxima = StringReference(s_propertyName_Maxima);
        String ^ Domain = StringReference(s_propertyName_Domain);
        String ^ Range = StringReference(s_propertyName_Range);
        String ^ InflectionPoints = StringReference(s_propertyName_InflectionPoints);
        String ^ Monotonicity = StringReference(s_propertyName_Monotonicity);
    }

    void Equation::RegisterDependencyProperties()
    {
        if (!s_expressionProperty)
        {
            s_expressionProperty = DependencyProperty::Register(
                EquationProperties::Expression,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
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
                ref new PropertyMetadata(accentColor, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_xInterceptProperty)
        {
            s_xInterceptProperty = DependencyProperty::Register(
                EquationProperties::XIntercept,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_yInterceptProperty)
        {
            s_yInterceptProperty = DependencyProperty::Register(
                EquationProperties::YIntercept,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_parityProperty)
        {
            s_parityProperty = DependencyProperty::Register(
                EquationProperties::Parity,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_periodicityProperty)
        {
            s_periodicityProperty = DependencyProperty::Register(
                EquationProperties::Periodicity,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_minimaProperty)
        {
            s_minimaProperty = DependencyProperty::Register(
                EquationProperties::Minima,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_maximaProperty)
        {
            s_maximaProperty = DependencyProperty::Register(
                EquationProperties::Maxima,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_domainProperty)
        {
            s_domainProperty = DependencyProperty::Register(
                EquationProperties::Domain,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_rangeProperty)
        {
            s_rangeProperty = DependencyProperty::Register(
                EquationProperties::Range,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_inflectionPointsProperty)
        {
            s_inflectionPointsProperty = DependencyProperty::Register(
                EquationProperties::InflectionPoints,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_monotonicityProperty)
        {
            s_monotonicityProperty = DependencyProperty::Register(
                EquationProperties::Monotonicity,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }
    }

    void Equation::OnCustomDependencyPropertyChanged(DependencyObject ^ obj, DependencyPropertyChangedEventArgs ^ args)
    {
        if (auto eq = static_cast<Equation ^>(obj))
        {
            String ^ propertyName = nullptr;
            if (args->Property == s_expressionProperty)
            {
                propertyName = EquationProperties::Expression;
            }
            else if (args->Property == s_lineColorProperty)
            {
                propertyName = EquationProperties::LineColor;
            }
            else if (args->Property == s_xInterceptProperty)
            {
                propertyName = EquationProperties::XIntercept;
            }
            else if (args->Property == s_yInterceptProperty)
            {
                propertyName = EquationProperties::YIntercept;
            }
            else if (args->Property == s_parityProperty)
            {
                propertyName = EquationProperties::Parity;
            }
            else if (args->Property == s_periodicityProperty)
            {
                propertyName = EquationProperties::Periodicity;
            }
            else if (args->Property == s_minimaProperty)
            {
                propertyName = EquationProperties::Minima;
            }
            else if (args->Property == s_maximaProperty)
            {
                propertyName = EquationProperties::Maxima;
            }
            else if (args->Property == s_domainProperty)
            {
                propertyName = EquationProperties::Domain;
            }
            else if (args->Property == s_rangeProperty)
            {
                propertyName = EquationProperties::Range;
            }
            else if (args->Property == s_inflectionPointsProperty)
            {
                propertyName = EquationProperties::InflectionPoints;
            }
            else if (args->Property == s_monotonicityProperty)
            {
                propertyName = EquationProperties::Monotonicity;
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
