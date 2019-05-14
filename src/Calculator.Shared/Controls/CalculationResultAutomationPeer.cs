using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Automation.Provider;

namespace CalculatorApp
{
	sealed class CalculationResultAutomationPeer : FrameworkElementAutomationPeer /* UNO TODO , IInvokeProvider*/
    {
        public CalculationResultAutomationPeer(FrameworkElement owner) : base(owner)
		{

		}

		protected override AutomationControlType GetAutomationControlTypeCore()
		{
			return AutomationControlType.Text;
		}

        // UNO TODO
		//override virtual object GetPatternCore(PatternInterface pattern)
		//{
		//	if (pattern == PatternInterface.Invoke)
		//	{
		//		return this;
		//	}

		//	return FrameworkElementAutomationPeer.GetPatternCore(pattern);
		//}

  //      virtual void Invoke()
		//{
		//	var owner = Owner as CalculationResult;
		//	owner.ProgrammaticSelect();
		//}
	}
}
