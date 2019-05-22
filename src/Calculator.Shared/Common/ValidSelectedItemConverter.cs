// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.Common
{
	public sealed class ValidSelectedItemConverter : IValueConverter
	{
		public ValidSelectedItemConverter()
		{
		}

		public object Convert(object value, Type targetType, object parameter, string language)
		{
			// Pass through as we don't want to change the value from the source
			return value;
		}

		public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			if (value != null)
			{
				if (value is int i && i > 0)
				{
					return value;
				}
			}

			// Stop the binding if the object is null
			return Windows.UI.Xaml.DependencyProperty.UnsetValue;
		}
	}
}
