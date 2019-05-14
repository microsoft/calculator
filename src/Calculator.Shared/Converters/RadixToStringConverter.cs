using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Interop;
using Windows.Foundation.Metadata;
using Windows.UI.Xaml.Data;
using CalculationManager;
using CalculatorApp;

namespace CalculatorApp.Converters
{
	[WebHostHidden]
	class RadixToStringConverter : IValueConverter
	{
		public object Convert(Object value, Type targetType, Object parameter, String language)
		{
			var boxedInt = (RADIX_TYPE)(int)value;
			string convertedValue = "";
			var resourceLoader = AppResourceProvider.GetInstance();

			switch (boxedInt)
			{
				case RADIX_TYPE.BIN_RADIX:
					convertedValue = resourceLoader.GetResourceString("Bin");
					break;
				case RADIX_TYPE.OCT_RADIX:
					convertedValue = resourceLoader.GetResourceString("Oct");
					break;
				case RADIX_TYPE.DEC_RADIX:
					convertedValue = resourceLoader.GetResourceString("Dec");
					break;
				case RADIX_TYPE.HEX_RADIX:
					convertedValue = resourceLoader.GetResourceString("Hex");
					break;
				default:
					break;

			};

			return convertedValue;
		}

		public virtual object ConvertBack(Object value, Type targetType, Object parameter, String language)
		{
			throw new NotImplementedException();
		}
	}
}