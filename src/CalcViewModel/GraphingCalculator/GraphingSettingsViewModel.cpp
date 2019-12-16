// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GraphingSettingsViewModel.h"
using namespace CalculatorApp::ViewModel;
using namespace GraphControl;

GraphingSettingsViewModel::GraphingSettingsViewModel()
    : m_XMin(0)
    , m_XMax(0)
    , m_YMin(0)
    , m_YMax(0)
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

        double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
        grapher->GetDisplayRanges(&xMin, &xMax, &yMin, &yMax);
        m_dontUpdateDisplayRange = true;
        XMin = xMin;
        XMax = xMax;
        YMin = yMin;
        YMax = yMax;
        m_dontUpdateDisplayRange = false;
    }
    Graph = grapher;
    RaisePropertyChanged(L"TrigUnit");
}
