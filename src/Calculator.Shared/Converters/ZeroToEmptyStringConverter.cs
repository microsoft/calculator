using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.Converters
{
	class ZeroToEmptyStringConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, string culture)
		{
			string myValue = value.ToString();

			if (myValue == "0")
			{
				return "";
			}
			else
			{
				return value;
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, string culture)
		{
			return value;
		}
	}
}
