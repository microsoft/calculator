// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Equation.h"
using namespace winrt::Windows::Foundation::Collections;

using namespace Platform;
using namespace std;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;

namespace GraphControl
{
    // Remove mml: formatting specific to RichEditBox control, which is not understood by the graph engine.
    static constexpr wstring_view s_mathPrefix = L"mml:";

    DependencyProperty ^ Equation::s_expressionProperty;
    static constexpr auto s_propertyName_Expression = L"Expression";

    DependencyProperty ^ Equation::s_lineColorProperty;
    static constexpr auto s_propertyName_LineColor = L"LineColor";

    DependencyProperty ^ Equation::s_isLineEnabledProperty;
    static constexpr auto s_propertyName_IsLineEnabled = L"IsLineEnabled";

    DependencyProperty ^ Equation::s_hasGraphErrorProperty;
    static constexpr auto s_propertyName_HasGraphError = L"HasGraphError";

    DependencyProperty ^ Equation::s_isValidatedProperty;
    static constexpr auto s_propertyName_IsValidated = L"IsValidated";

    DependencyProperty ^ Equation::s_xInterceptProperty;
    static constexpr auto s_propertyName_XIntercept = L"XIntercept";

    DependencyProperty ^ Equation::s_yInterceptProperty;
    static constexpr auto s_propertyName_YIntercept = L"YIntercept";

    DependencyProperty ^ Equation::s_parityProperty;
    static constexpr auto s_propertyName_Parity = L"Parity";

    DependencyProperty ^ Equation::s_periodicityDirectionProperty;
    static constexpr auto s_propertyName_PeriodicityDirection = L"PeriodicityDirection";

    DependencyProperty ^ Equation::s_periodicityExpressionProperty;
    static constexpr auto s_propertyName_PeriodicityExpression = L"PeriodicityExpression";

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

    DependencyProperty ^ Equation::s_verticalAsymptotesProperty;
    static constexpr auto s_propertyName_VerticalAsymptotes = L"VerticalAsymptotes";

    DependencyProperty ^ Equation::s_horizontalAsymptotesProperty;
    static constexpr auto s_propertyName_HorizontalAsymptotes = L"HorizontalAsymptotes";

    DependencyProperty ^ Equation::s_obliqueAsymptotesProperty;
    static constexpr auto s_propertyName_ObliqueAsymptotes = L"ObliqueAsymptotes";

    DependencyProperty ^ Equation::s_tooComplexFeaturesProperty;
    static constexpr auto s_propertyName_TooComplexFeatures = L"TooComplexFeatures";

    DependencyProperty ^ Equation::s_analysisErrorProperty;
    static constexpr auto s_propertyName_AnalysisError = L"AnalysisError";

    namespace EquationProperties
    {
        String ^ Expression = StringReference(s_propertyName_Expression);
        String ^ LineColor = StringReference(s_propertyName_LineColor);
        String ^ IsLineEnabled = StringReference(s_propertyName_IsLineEnabled);
        String ^ HasGraphError = StringReference(s_propertyName_HasGraphError);
        String ^ IsValidated = StringReference(s_propertyName_IsValidated);
        String ^ XIntercept = StringReference(s_propertyName_XIntercept);
        String ^ YIntercept = StringReference(s_propertyName_YIntercept);
        String ^ Parity = StringReference(s_propertyName_Parity);
        String ^ PeriodicityDirection = StringReference(s_propertyName_PeriodicityDirection);
        String ^ PeriodicityExpression = StringReference(s_propertyName_PeriodicityExpression);
        String ^ Minima = StringReference(s_propertyName_Minima);
        String ^ Maxima = StringReference(s_propertyName_Maxima);
        String ^ Domain = StringReference(s_propertyName_Domain);
        String ^ Range = StringReference(s_propertyName_Range);
        String ^ InflectionPoints = StringReference(s_propertyName_InflectionPoints);
        String ^ Monotonicity = StringReference(s_propertyName_Monotonicity);
        String ^ VerticalAsymptotes = StringReference(s_propertyName_VerticalAsymptotes);
        String ^ HorizontalAsymptotes = StringReference(s_propertyName_HorizontalAsymptotes);
        String ^ ObliqueAsymptotes = StringReference(s_propertyName_ObliqueAsymptotes);
        String ^ TooComplexFeatures = StringReference(s_propertyName_TooComplexFeatures);
        String ^ AnalysisError = StringReference(s_propertyName_AnalysisError);
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

            s_lineColorProperty = DependencyProperty::Register(
                EquationProperties::LineColor,
                SolidColorBrush::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_isLineEnabledProperty)
        {
            s_isLineEnabledProperty = DependencyProperty::Register(
                EquationProperties::IsLineEnabled,
                bool ::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_hasGraphErrorProperty)
        {
            s_hasGraphErrorProperty = DependencyProperty::Register(
                EquationProperties::HasGraphError,
                bool ::typeid,
                Equation::typeid,
                ref new PropertyMetadata(false, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_isValidatedProperty)
        {
            s_isValidatedProperty = DependencyProperty::Register(
                EquationProperties::IsValidated,
                bool ::typeid,
                Equation::typeid,
                ref new PropertyMetadata(false, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
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
                int ::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_periodicityDirectionProperty)
        {
            s_periodicityDirectionProperty = DependencyProperty::Register(
                EquationProperties::PeriodicityDirection,
                int ::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_periodicityExpressionProperty)
        {
            s_periodicityExpressionProperty = DependencyProperty::Register(
                EquationProperties::PeriodicityExpression,
                String::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_minimaProperty)
        {
            s_minimaProperty = DependencyProperty::Register(
                EquationProperties::Minima,
                IObservableVector<String ^>::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_maximaProperty)
        {
            s_maximaProperty = DependencyProperty::Register(
                EquationProperties::Maxima,
                IObservableVector<String ^>::typeid,
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
                IObservableVector<String ^>::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_monotonicityProperty)
        {
            s_monotonicityProperty = DependencyProperty::Register(
                EquationProperties::Monotonicity,
                IObservableMap<String ^, String ^>::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_verticalAsymptotesProperty)
        {
            s_verticalAsymptotesProperty = DependencyProperty::Register(
                EquationProperties::VerticalAsymptotes,
                IObservableVector<String ^>::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_horizontalAsymptotesProperty)
        {
            s_horizontalAsymptotesProperty = DependencyProperty::Register(
                EquationProperties::HorizontalAsymptotes,
                IObservableVector<String ^>::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_obliqueAsymptotesProperty)
        {
            s_obliqueAsymptotesProperty = DependencyProperty::Register(
                EquationProperties::ObliqueAsymptotes,
                IObservableVector<String ^>::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }
        if (!s_tooComplexFeaturesProperty)
        {
            s_tooComplexFeaturesProperty = DependencyProperty::Register(
                EquationProperties::TooComplexFeatures,
                int ::typeid,
                Equation::typeid,
                ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(&Equation::OnCustomDependencyPropertyChanged)));
        }

        if (!s_analysisErrorProperty)
        {
            s_analysisErrorProperty = DependencyProperty::Register(
                EquationProperties::AnalysisError,
                int ::typeid,
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
            else if (args->Property == s_isLineEnabledProperty)
            {
                propertyName = EquationProperties::IsLineEnabled;
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
            else if (args->Property == s_periodicityDirectionProperty)
            {
                propertyName = EquationProperties::PeriodicityDirection;
            }
            else if (args->Property == s_periodicityExpressionProperty)
            {
                propertyName = EquationProperties::PeriodicityExpression;
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
            else if (args->Property == s_verticalAsymptotesProperty)
            {
                propertyName = EquationProperties::VerticalAsymptotes;
            }
            else if (args->Property == s_horizontalAsymptotesProperty)
            {
                propertyName = EquationProperties::HorizontalAsymptotes;
            }
            else if (args->Property == s_obliqueAsymptotesProperty)
            {
                propertyName = EquationProperties::ObliqueAsymptotes;
            }
            else if (args->Property == s_tooComplexFeaturesProperty)
            {
                propertyName = EquationProperties::TooComplexFeatures;
            }
            else if (args->Property == s_analysisErrorProperty)
            {
                propertyName = EquationProperties::AnalysisError;
            }
            else if (args->Property == s_hasGraphErrorProperty)
            {
                propertyName = EquationProperties::HasGraphError;
            }
            else if (args->Property == s_isValidatedProperty)
            {
                propertyName = EquationProperties::IsValidated;
            }

            eq->PropertyChanged(eq, propertyName);
        }
    }

    wstring Equation::GetRequest()
    {
        wstringstream ss{};
        ss << GetRequestHeader() << GetExpression() << GetLineColor() << L"</mfenced></mrow>";

        return ss.str();
    }

    wstring Equation::GetRequestHeader()
    {
        wstring expr{ Expression->Data() };

        // Check for unicode characters of less than, less than or equal to, greater than and greater than or equal to.
        if (expr.find(L">&#x3E;<") != wstring::npos || expr.find(L">&#x3C;<") != wstring::npos || expr.find(L">&#x2265;<") != wstring::npos
            || expr.find(L">&#x2264;<") != wstring::npos)
        {
            return L"<mrow><mi>plotIneq2D</mi><mfenced separators=\"\">"s;
        }
        else if (expr.find(L">=<") != wstring::npos)
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

    bool Equation::IsGraphableEquation()
    {
        return !Expression->IsEmpty() && IsLineEnabled && !HasGraphError;
    }
}
