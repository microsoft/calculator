// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculationResultAutomationPeer.h"

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation::Peers;

namespace CalculatorApp::Controls
{
    CalculationResultAutomationPeer::CalculationResultAutomationPeer(FrameworkElement ^ owner)
        : FrameworkElementAutomationPeer(owner)
    {
    }

    AutomationControlType CalculationResultAutomationPeer::GetAutomationControlTypeCore()
    {
        return AutomationControlType::Text;
    }

    Platform::Object ^ CalculationResultAutomationPeer::GetPatternCore(PatternInterface pattern)
    {
        if (pattern == PatternInterface::Invoke)
        {
            return this;
        }

        return FrameworkElementAutomationPeer::GetPatternCore(pattern);
    }

    void CalculationResultAutomationPeer::Invoke()
    {
        auto owner = static_cast<CalculationResult ^>(this->Owner);
        owner->ProgrammaticSelect();
    }
}
