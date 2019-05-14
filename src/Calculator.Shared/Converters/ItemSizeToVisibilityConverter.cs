using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Interop;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.Converters
{
	sealed public class ItemSizeToVisibilityConverter : IValueConverter
	{
		public Object Convert(Object value, Type targetType, Object parameter, String language)
		{
			var items = (int)value;
			var boolValue = items == 0;

			return BooleanToVisibilityConverter.Convert(boolValue);
		}

		public object ConvertBack(Object value, Type targetType, Object parameter, String language)
		{
			throw new NotImplementedException();
		}

	}

	public class ItemSizeToVisibilityNegationConverter : IValueConverter
	{
		public Object Convert(Object value, Type targetType, Object parameter, String language)
		{
			var items = (int)value;
			var boolValue = items > 0;

			return BooleanToVisibilityConverter.Convert(boolValue);
		}
		public Object ConvertBack(Object value, Type targetType, Object parameter, String language)
		{
			throw new NotImplementedException();
		}
	}
}