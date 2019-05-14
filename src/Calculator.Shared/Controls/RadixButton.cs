using CalculatorApp.Common;
using System;
using System.Collections.Generic;
using System.Text;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.Controls
{
	public partial class RadixButton : RadioButton
	{
		public RadixButton()
		{

		}

		internal string GetRawDisplayValue()
		{
			string rawValue = null;
			string radixContent = Content.ToString();
			LocalizationSettings.GetInstance().RemoveGroupSeparators(radixContent, radixContent.Length, ref rawValue);

			return rawValue;
		}
	}
}
