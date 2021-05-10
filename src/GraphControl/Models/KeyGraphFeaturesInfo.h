// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "Utils.h"

namespace Graphing
{
    struct IGraphFunctionAnalysisData;
}

namespace CalculatorApp
{
    enum AnalysisErrorType;
}

namespace GraphControl
{
public
    ref class KeyGraphFeaturesInfo sealed
    {
    public:
        PROPERTY_R(Platform::String ^, XIntercept);
        PROPERTY_R(Platform::String ^, YIntercept);
        PROPERTY_R(int, Parity);
        PROPERTY_R(int, PeriodicityDirection);
        PROPERTY_R(Platform::String ^, PeriodicityExpression);
        PROPERTY_R(Windows::Foundation::Collections::IVector<Platform::String ^> ^, Minima);
        PROPERTY_R(Windows::Foundation::Collections::IVector<Platform::String ^> ^, Maxima);
        PROPERTY_R(Platform::String ^, Domain);
        PROPERTY_R(Platform::String ^, Range);
        PROPERTY_R(Windows::Foundation::Collections::IVector<Platform::String ^> ^, InflectionPoints);
        PROPERTY_R(SINGLE_ARG(Windows::Foundation::Collections::IObservableMap<Platform::String ^, Platform::String ^> ^), Monotonicity);
        PROPERTY_R(Windows::Foundation::Collections::IVector<Platform::String ^> ^, VerticalAsymptotes);
        PROPERTY_R(Windows::Foundation::Collections::IVector<Platform::String ^> ^, HorizontalAsymptotes);
        PROPERTY_R(Windows::Foundation::Collections::IVector<Platform::String ^> ^, ObliqueAsymptotes);
        PROPERTY_R(int, TooComplexFeatures);
        PROPERTY_R(int, AnalysisError);

    internal:
        static KeyGraphFeaturesInfo ^ Create(Graphing::IGraphFunctionAnalysisData data);
        static KeyGraphFeaturesInfo ^ Create(CalculatorApp::AnalysisErrorType type);

    private:
        static Windows::Foundation::Collections::IObservableVector<Platform::String ^> ^ ConvertWStringVector(std::vector<std::wstring> inVector);
        static Windows::Foundation::Collections::
                IObservableMap<Platform::String ^, Platform::String ^> ^ ConvertWStringIntMap(std::map<std::wstring, int> inMap);
    };
}
