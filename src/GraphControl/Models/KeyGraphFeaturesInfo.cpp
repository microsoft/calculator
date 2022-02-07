// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "KeyGraphFeaturesInfo.h"
#include "../../CalcViewModel/GraphingCalculatorEnums.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace GraphControl;
using namespace Graphing;

IObservableVector<String ^> ^ KeyGraphFeaturesInfo::ConvertWStringVector(vector<wstring> inVector)
{
    auto outVector = ref new Vector<String ^>();

    for (const auto& v : inVector)
    {
        outVector->Append(ref new String(v.c_str()));
    }

    return outVector;
}

IObservableMap<String ^, String ^> ^ KeyGraphFeaturesInfo::ConvertWStringIntMap(map<wstring, int> inMap)
{
    Map<String ^, String ^> ^ outMap = ref new Map<String ^, String ^>();
    
    for (const auto& m : inMap)
    {
        outMap->Insert(ref new String(m.first.c_str()), m.second.ToString());
    }

    return outMap;
}

KeyGraphFeaturesInfo ^ KeyGraphFeaturesInfo::Create(IGraphFunctionAnalysisData data)
{
    auto res = ref new KeyGraphFeaturesInfo();
    res->XIntercept = ref new String(data.Zeros.c_str());
    res->YIntercept = ref new String(data.YIntercept.c_str());
    res->Domain = ref new String(data.Domain.c_str());
    res->Range = ref new String(data.Range.c_str());
    res->Parity = data.Parity;
    res->PeriodicityDirection = data.PeriodicityDirection;
    res->PeriodicityExpression = ref new String(data.PeriodicityExpression.c_str());
    res->Minima = ConvertWStringVector(data.Minima);
    res->Maxima = ConvertWStringVector(data.Maxima);
    res->InflectionPoints = ConvertWStringVector(data.InflectionPoints);
    res->Monotonicity = ConvertWStringIntMap(data.MonotoneIntervals);
    res->VerticalAsymptotes = ConvertWStringVector(data.VerticalAsymptotes);
    res->HorizontalAsymptotes = ConvertWStringVector(data.HorizontalAsymptotes);
    res->ObliqueAsymptotes = ConvertWStringVector(data.ObliqueAsymptotes);
    res->TooComplexFeatures = data.TooComplexFeatures;
    res->AnalysisError = CalculatorApp::AnalysisErrorType::NoError;

    TraceLogger::GetInstance()->LogFunctionAnalysisPerformed(CalculatorApp::AnalysisErrorType::NoError, res->TooComplexFeatures);
    return res;
}

KeyGraphFeaturesInfo ^ KeyGraphFeaturesInfo::Create(CalculatorApp::AnalysisErrorType type)
{
    auto res = ref new KeyGraphFeaturesInfo();
    res->AnalysisError = type;

    TraceLogger::GetInstance()->LogFunctionAnalysisPerformed(type, 0);
    return res;
}
