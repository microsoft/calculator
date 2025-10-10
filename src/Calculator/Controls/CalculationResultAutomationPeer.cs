// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class CalculationResultAutomationPeer : Windows.UI.Xaml.Automation.Peers.FrameworkElementAutomationPeer,
                                                              Windows.UI.Xaml.Automation.Provider.IInvokeProvider
        {
            public CalculationResultAutomationPeer(FrameworkElement owner) : base(owner)
            {
            }

            protected override AutomationControlType GetAutomationControlTypeCore()
            {
                return AutomationControlType.Text;
            }

            protected override object GetPatternCore(PatternInterface pattern)
            {
                return pattern == PatternInterface.Invoke ? this : base.GetPatternCore(pattern);
            }

            public void Invoke()
            {
                var owner = (CalculationResult)this.Owner;
                owner.ProgrammaticSelect();
            }
        }
    }
}
