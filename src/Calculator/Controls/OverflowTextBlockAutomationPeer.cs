// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Diagnostics;
using Windows.UI.Xaml.Automation.Peers;
using Windows.Foundation.Collections;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class OverflowTextBlockAutomationPeer : Windows.UI.Xaml.Automation.Peers.FrameworkElementAutomationPeer
        {
            public OverflowTextBlockAutomationPeer(OverflowTextBlock owner) : base(owner)
            {
            }

            protected override AutomationControlType GetAutomationControlTypeCore()
            {
                return AutomationControlType.Text;
            }

            protected override IList<AutomationPeer> GetChildrenCore()
            {
                return null;
            }
        }
    }
}
