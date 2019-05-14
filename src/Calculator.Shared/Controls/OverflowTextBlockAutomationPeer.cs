using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using Windows.Devices.Input;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.Controls
{
	sealed class OverflowTextBlockAutomationPeer : FrameworkElementAutomationPeer
	{
		public OverflowTextBlockAutomationPeer(OverflowTextBlock owner) :
			base(owner)
		{ }

		override protected AutomationControlType GetAutomationControlTypeCore()
		{
			return AutomationControlType.Text;
		}

		override protected IList<AutomationPeer> GetChildrenCore()
		{
			return null;
		}
	}
}
