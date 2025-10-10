// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel;

using System.Collections.Generic;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class SupplementaryItemsControl : ItemsControl
        {
            public SupplementaryItemsControl()
            {
            }

            protected override DependencyObject GetContainerForItemOverride()
            {
                return new SupplementaryContentPresenter();
            }

            protected override void PrepareContainerForItemOverride(DependencyObject element, object item)
            {
                base.PrepareContainerForItemOverride(element, item);

                if (item is SupplementaryResult supplementaryResult)
                {
                    AutomationProperties.SetName(element, supplementaryResult.GetLocalizedAutomationName());
                }
            }
        }

        public sealed class SupplementaryContentPresenter : ContentPresenter
        {
            public SupplementaryContentPresenter()
            {
            }

            protected override AutomationPeer OnCreateAutomationPeer()
            {
                return new SupplementaryContentPresenterAP(this);
            }
        }

        internal sealed class SupplementaryContentPresenterAP : FrameworkElementAutomationPeer
        {
            protected override AutomationControlType GetAutomationControlTypeCore()
            {
                return AutomationControlType.Text;
            }

            protected override IList<AutomationPeer> GetChildrenCore()
            {
                return null;
            }

            internal SupplementaryContentPresenterAP(SupplementaryContentPresenter owner)
                : base(owner)
            {
            }
        }
    }
}
