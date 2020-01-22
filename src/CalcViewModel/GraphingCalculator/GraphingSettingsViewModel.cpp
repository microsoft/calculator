// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingSettingsViewModel.h"
#include <CalcManager\NumberFormattingUtils.h>

using namespace CalculatorApp::ViewModel;
using namespace CalcManager::NumberFormattingUtils;
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
    auto valueStr = to_wstring(m_XMinValue);
    TrimTrailingZeros(valueStr);
    m_XMin = ref new String(valueStr.c_str());

    valueStr = to_wstring(m_XMaxValue);
    TrimTrailingZeros(valueStr);
    m_XMax = ref new String(valueStr.c_str());

    valueStr = to_wstring(m_YMinValue);
    TrimTrailingZeros(valueStr);
    m_YMin = ref new String(valueStr.c_str());

    valueStr = to_wstring(m_YMaxValue);
    TrimTrailingZeros(valueStr);
    m_YMax = ref new String(valueStr.c_str());

    m_dontUpdateDisplayRange = false;
}

void GraphingSettingsViewModel::UpdateDisplayRange(bool XValuesModified)
{
    if (m_Graph == nullptr || m_dontUpdateDisplayRange || HasError())
    {
        return;
    }

    if (m_Graph->ForceProportionalAxes)
    {
        // If ForceProportionalAxes is set, the graph will try to automatically adjust ranges to remain proportional.
        // but without a logic to choose which values can be modified or not.
        // To solve this problem, we calculate the new ranges here, taking care to not modify the current axis and
        // modifying only the least recently updated value of the other axis.

        if (XValuesModified)
        {
            if (m_YIsMinLastChanged)
            {
                auto yMaxValue = m_YMinValue + (m_XMaxValue - m_XMinValue) * m_Graph->ActualHeight / m_Graph->ActualWidth;
                if (m_YMaxValue != yMaxValue)
                {
                    m_YMaxValue = yMaxValue;
                    auto valueStr = to_wstring(m_YMaxValue);
                    TrimTrailingZeros(valueStr);
                    m_YMax = ref new String(valueStr.c_str());
                    RaisePropertyChanged("YMax");
                }
            }
            else
            {
                auto yMinValue = m_YMaxValue - (m_XMaxValue - m_XMinValue) * m_Graph->ActualHeight / m_Graph->ActualWidth;
                if (m_YMinValue != yMinValue)
                {
                    m_YMinValue = yMinValue;
                    auto valueStr = to_wstring(m_YMinValue);
                    TrimTrailingZeros(valueStr);
                    m_YMin = ref new String(valueStr.c_str());
                    RaisePropertyChanged("YMin");
                }
            }
        }
        else
        {
            if (m_XIsMinLastChanged)
            {
                auto xMaxValue = m_XMinValue + (m_YMaxValue - m_YMinValue) * m_Graph->ActualWidth / m_Graph->ActualHeight;
                if (m_XMaxValue != xMaxValue)
                {
                    m_XMaxValue = xMaxValue;
                    auto valueStr = to_wstring(m_XMaxValue);
                    TrimTrailingZeros(valueStr);
                    m_XMax = ref new String(valueStr.c_str());
                    RaisePropertyChanged("XMax");
                }
            }
            else
            {
                auto xMinValue = m_XMaxValue - (m_YMaxValue - m_YMinValue) * m_Graph->ActualWidth / m_Graph->ActualHeight;
                if (m_XMinValue != xMinValue)
                {
                    m_XMinValue = xMinValue;
                    auto valueStr = to_wstring(m_XMinValue);
                    TrimTrailingZeros(valueStr);
                    m_XMin = ref new String(valueStr.c_str());
                    RaisePropertyChanged("XMin");
                }
            }
        }
    }
    m_Graph->SetDisplayRanges(m_XMinValue, m_XMaxValue, m_YMinValue, m_YMaxValue);
}

bool GraphingSettingsViewModel::HasError()
{
    return m_XMinError || m_YMinError || m_XMaxError || m_YMaxError || XError || YError;
}
