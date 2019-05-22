// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using WindowsCalculator.Shared.Common;

namespace CalculatorApp
{
    namespace Common
    {
		class AlwaysSelectedCollectionViewConverter : Windows.UI.Xaml.Data.IValueConverter
		{
			public AlwaysSelectedCollectionViewConverter()
			{
			}

			public object Convert(
					object value,
					Type targetType,
					object parameter,
					string language)
			{
				var result = value as IEnumerable;
				if (result != null)
				{
					return new AlwaysSelectedCollectionView(result);
				}
				return Windows.UI.Xaml.DependencyProperty.UnsetValue; // Can't convert
			}

			public object ConvertBack(
			   object value,
			   Type targetType,
			   object parameter,
			   string language)
			{
				return Windows.UI.Xaml.DependencyProperty.UnsetValue;
			}
		}
	}
}
