// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingSettingsViewModel.h"
using namespace CalculatorApp::ViewModel;
using namespace GraphControl;
using namespace std;
using namespace Platform;

GraphingSettingsViewModel::GraphingSettingsViewModel()
    : m_XMinValue(0)
    , m_XMaxValue(0)
    , m_YMinValue(0)
    , m_YMaxValue(0)
    , m_XMinError(false)
    , m_XMaxError(false)
    , m_YMinError(false)
    , m_YMaxError(false)
    , m_dontUpdateDisplayRange(false)
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
    m_Graph->GetDisplayRanges(&xMin, &xMax, &yMin, &yMax);
    m_dontUpdateDisplayRange = true;
    m_XMinValue = xMin;
    m_XMaxValue = xMax;
    m_YMinValue = yMin;
    m_YMaxValue = yMax;
    XMin = ref new String(to_wstring(m_XMinValue).c_str());
    XMax = ref new String(to_wstring(m_XMaxValue).c_str());
    YMin = ref new String(to_wstring(m_YMinValue).c_str());
    YMax = ref new String(to_wstring(m_YMaxValue).c_str());
    m_dontUpdateDisplayRange = false;
}

void GraphingSettingsViewModel::RefreshPosition()
{
    if (m_XMinError || m_XMaxError || m_YMinError || m_YMaxError)
    {
        InitRanges();
    }
    else
    {
        m_Graph->SetDisplayRanges(m_XMinValue, m_XMaxValue, m_YMinValue, m_YMaxValue);
    }
}

void GraphingSettingsViewModel::UpdateDisplayRange()
{
    if (m_dontUpdateDisplayRange || m_XMinError || m_XMaxError || m_YMinError || m_YMaxError || m_Graph == nullptr)
    {
        return;
    }

    m_Graph->SetDisplayRanges(m_XMinValue, m_XMaxValue, m_YMinValue, m_YMaxValue);
}

bool GraphingSettingsViewModel::HasError()
{
    return m_XMinError || m_YMinError || m_XMaxError || m_YMaxError || XError || YError;
}
