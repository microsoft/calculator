// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Controls/CalculationResult.h"

namespace CalculatorApp
{
    namespace Controls
    {
        public ref class CalculationResultAutomationPeer sealed : Windows::UI::Xaml::Automation::Peers::FrameworkElementAutomationPeer,
            Windows::UI::Xaml::Automation::Provider::IInvokeProvider
        {
        public:
            CalculationResultAutomationPeer(Windows::UI::Xaml::FrameworkElement^ owner);

            virtual Windows::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() override;
            virtual Platform::Object^ GetPatternCore(Windows::UI::Xaml::Automation::Peers::PatternInterface pattern) override;
            virtual void Invoke();
        };
    }
}
