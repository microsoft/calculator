using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp.Controls
{
	public partial class SupplementaryItemsControl : ItemsControl
	{
		// PROTECTED 
		protected override DependencyObject GetContainerForItemOverride()
		{
            return new SupplementaryContentPresenter();
        }

        protected override void PrepareContainerForItemOverride(DependencyObject element, Object item)
		{
            base.PrepareContainerForItemOverride(element, item);

            // UNO TODO
            //var supplementaryResult = (SupplementaryResult)(item);
            //if (supplementaryResult)
            //{
            //    AutomationProperties.SetName(element, supplementaryResult.GetLocalizedAutomationName());
            //}
        }
    }

	public sealed partial class SupplementaryContentPresenter : ContentPresenter
	{

		protected override AutomationPeer OnCreateAutomationPeer()
		{
            return null;
		}
	}

	public sealed partial class SupplementaryContentPresenterAP : FrameworkElementAutomationPeer
    {
        protected override AutomationControlType GetAutomationControlTypeCore()
        {
            return AutomationControlType.Text;
        }

		protected override IList<AutomationPeer> GetChildrenCore()
        {
            return null;
        }

		internal SupplementaryContentPresenterAP(SupplementaryContentPresenter owner) :
			base(owner)
        {
		}
	}
}
