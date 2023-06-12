// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingSettingsViewModel.h"

using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::ViewModel::Common;
using namespace GraphControl;
using namespace std;
using namespace Platform;
using namespace Windows::UI::Xaml;

GraphingSettingsViewModel::GraphingSettingsViewModel()
    : m_XMinValue(0)
    , m_XMaxValue(0)
    , m_YMinValue(0)
    , m_YMaxValue(0)
    , m_XMinError(false)
    , m_XMaxError(false)
    , m_YMinError(false)
    , m_YMaxError(false)
    , m_dontUpdateDisplayRange()
    , m_XIsMinLastChanged(true)
    , m_YIsMinLastChanged(true)
{
}

void GraphingSettingsViewModel::SetGrapher(Grapher ^ grapher)
{
    if (grapher != nullptr)
    {
        if (grapher->TrigUnitMode == (int)Graphing::EvalTrigUnitMode::Invalid)
        {
            grapher->TrigUnitMode = (int)Graphing::EvalTrigUnitMode::Radians;
        }
    }
    Graph = grapher;

    InitRanges();
    RaisePropertyChanged(L"TrigUnit");
}

void GraphingSettingsViewModel::InitRanges()
{
    double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    if (m_Graph != nullptr)
    {
        m_Graph->GetDisplayRanges(&xMin, &xMax, &yMin, &yMax);
    }
    m_dontUpdateDisplayRange = true;
    m_XMinValue = xMin;
    m_XMaxValue = xMax;
    m_YMinValue = yMin;
    m_YMaxValue = yMax;

    std::wostringstream xMinStr;
    xMinStr <<  m_XMinValue;
    XMin = ref new String(xMinStr.str().c_str());

    std::wostringstream xMaxStr;
    xMaxStr << m_XMaxValue;
    XMax = ref new String(xMaxStr.str().c_str());

    std::wostringstream yMinStr;
    yMinStr << m_YMinValue;
    YMin = ref new String(yMinStr.str().c_str());

    std::wostringstream yMaxStr;
    yMaxStr << m_YMaxValue;
    YMax = ref new String(yMaxStr.str().c_str());

    m_dontUpdateDisplayRange = false;
}

void GraphingSettingsViewModel::ResetView()
{
    if (m_Graph != nullptr)
    {
        m_Graph->ResetGrid();
        InitRanges();
        m_XMinError = false;
        m_XMaxError = false;
        m_YMinError = false;
        m_YMaxError = false;

        RaisePropertyChanged("XError");
        RaisePropertyChanged("XMin");
        RaisePropertyChanged("XMax");
        RaisePropertyChanged("YError");
        RaisePropertyChanged("YMin");
        RaisePropertyChanged("YMax");
    }
}

void GraphingSettingsViewModel::UpdateDisplayRange()
{
    if (m_Graph == nullptr || m_dontUpdateDisplayRange || HasError())
    {
        return;
    }

    m_Graph->SetDisplayRanges(m_XMinValue, m_XMaxValue, m_YMinValue, m_YMaxValue);

    CalculatorApp::ViewModel::Common::TraceLogger::GetInstance()->LogGraphSettingsChanged(GraphSettingsType::Grid, L"");
}

bool GraphingSettingsViewModel::HasError()
{
    return m_XMinError || m_YMinError || m_XMaxError || m_YMaxError || XError || YError;
}
