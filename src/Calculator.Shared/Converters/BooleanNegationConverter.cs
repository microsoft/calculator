using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.Converters
{
	class BooleanNegationConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, string culture)
		{
			bool? boxedBool = value as bool?;

			if (boxedBool != null)
			{
				return !boxedBool;
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
