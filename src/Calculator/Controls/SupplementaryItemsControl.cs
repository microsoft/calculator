// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Diagnostics;
using CalculatorApp;
using CalculatorApp.Controls;
using CalculatorApp.ViewModel;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;

using Windows.Foundation.Collections;

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

                var supplementaryResult = item as SupplementaryResult;
                if (supplementaryResult != null)
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

        sealed class SupplementaryContentPresenterAP : FrameworkElementAutomationPeer
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
