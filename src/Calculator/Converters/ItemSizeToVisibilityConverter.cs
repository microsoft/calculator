// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalculatorApp
{
    namespace Converters
    {
        [Windows.Foundation.Metadata.WebHostHidden]
        public sealed class ItemSizeToVisibilityConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var boolValue = (value is int items && (items == 0));
                return BooleanToVisibilityConverter.Convert(boolValue);
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                throw new NotImplementedException();
            }
        }

        public sealed class ItemSizeToVisibilityNegationConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var boolValue = (value is int items && (items > 0));
                return BooleanToVisibilityConverter.Convert(boolValue);
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                throw new NotImplementedException();
            }
        }
    }
}

