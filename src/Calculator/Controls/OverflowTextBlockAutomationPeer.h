// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Controls/OverflowTextBlock.h"

namespace CalculatorApp
{
    namespace Controls
    {
        public ref class OverflowTextBlockAutomationPeer sealed : Windows::UI::Xaml::Automation::Peers::FrameworkElementAutomationPeer
        {
        public:
            OverflowTextBlockAutomationPeer(OverflowTextBlock^ owner);

            virtual Windows::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() override;
            virtual Windows::Foundation::Collections::IVector<Windows::UI::Xaml::Automation::Peers::AutomationPeer^>^ GetChildrenCore() override;
        };
    }
}
