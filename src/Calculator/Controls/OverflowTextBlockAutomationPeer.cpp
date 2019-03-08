// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Controls/OverflowTextBlock.h"
#include "OverflowTextBlockAutomationPeer.h"

using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::Foundation::Collections;

namespace CalculatorApp::Controls
{
    OverflowTextBlockAutomationPeer::OverflowTextBlockAutomationPeer(OverflowTextBlock^ owner) :
        FrameworkElementAutomationPeer(owner)
    { }

    AutomationControlType OverflowTextBlockAutomationPeer::GetAutomationControlTypeCore()
    {
        return AutomationControlType::Text;
    }

    IVector<AutomationPeer^>^ OverflowTextBlockAutomationPeer::GetChildrenCore()
    {
        return nullptr;
    }
}
