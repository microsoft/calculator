// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Automation.Provider;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp
{
    namespace Controls
    {
        // OperatorPanelButton opens a flyout menu. Because it derives from Button (not
        // ToggleButton), this peer builds on ButtonAutomationPeer, which does not advertise the
        // Toggle pattern that would make Narrator announce a misleading On/Off state for a menu
        // opener. It adds the ExpandCollapse pattern (collapsed/expanded) on top.
        public sealed class OperatorPanelButtonAutomationPeer : ButtonAutomationPeer, IExpandCollapseProvider
        {
            public OperatorPanelButtonAutomationPeer(Button owner)
                : base(owner)
            {
            }

            public ExpandCollapseState ExpandCollapseState =>
                ((OperatorPanelButton)Owner).IsChecked ? ExpandCollapseState.Expanded : ExpandCollapseState.Collapsed;

            public void Expand()
            {
                var owner = (OperatorPanelButton)Owner;
                if (!owner.IsChecked)
                {
                    owner.IsChecked = true;
                    owner.FlyoutMenu?.ShowAt(owner);
                }
            }

            public void Collapse()
            {
                var owner = (OperatorPanelButton)Owner;
                if (owner.IsChecked)
                {
                    owner.FlyoutMenu?.Hide();
                }
            }

            public void RaiseExpandCollapseAutomationEvent(ExpandCollapseState oldState, ExpandCollapseState newState)
            {
                RaisePropertyChangedEvent(ExpandCollapsePatternIdentifiers.ExpandCollapseStateProperty, oldState, newState);
            }

            protected override object GetPatternCore(PatternInterface pattern)
            {
                if (pattern == PatternInterface.ExpandCollapse)
                {
                    return this;
                }

                return base.GetPatternCore(pattern);
            }
        }
    }
}
