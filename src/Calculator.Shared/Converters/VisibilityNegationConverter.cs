using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Interop;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml;

namespace CalculatorApp.Converters
{
	[WebHostHidden]
	public sealed class VisibilityNegationConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, String language)
		{
			var boxedVisibility = (Visibility)value;
			Visibility visibility = Visibility.Collapsed;

			if (boxedVisibility == Visibility.Collapsed)
			{
				visibility = Visibility.Visible;
			}

			return visibility;
		}

		public object ConvertBack(Object value, Type targetType, Object parameter, String language)
		{
			return Convert(value, targetType, parameter, language);
		}
	}
}
