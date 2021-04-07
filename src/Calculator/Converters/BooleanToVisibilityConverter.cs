// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.UI.Xaml;

namespace CalculatorApp
{
    namespace Converters
    {
        /// <summary>
        /// Value converter that translates true to <see cref="Visibility.Visible"/> and false
        /// to <see cref="Visibility.Collapsed"/>.
        /// </summary>
        public sealed class BooleanToVisibilityConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public static Windows.UI.Xaml.Visibility Convert(bool visibility)
            {
                return visibility ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;
            }

            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var boxedBool = (value as bool?);
                var boolValue = (boxedBool != null && boxedBool.Value);
                return BooleanToVisibilityConverter.Convert(boolValue);
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                var visibility = (value as Visibility?);
                return (visibility != null && visibility.Value == Visibility.Visible);
            }
        }

        /// <summary>
        /// Value converter that translates false to <see cref="Visibility.Visible"/> and true
        /// to <see cref="Visibility.Collapsed"/>.
        /// </summary>
        public sealed class BooleanToVisibilityNegationConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var boxedBool = (value as bool?);
                var boolValue = (boxedBool != null && boxedBool.Value);
                return BooleanToVisibilityConverter.Convert(!boolValue);
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                var visibility = (value as Visibility?);
                return (visibility != null && visibility.Value != Visibility.Visible);
            }
        }
    }
}

