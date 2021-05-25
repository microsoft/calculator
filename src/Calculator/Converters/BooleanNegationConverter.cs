// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalculatorApp
{
    namespace Converters
    {
        /// <summary>
        /// Value converter that translates true to false and vice versa.
        /// </summary>
        [Windows.Foundation.Metadata.WebHostHidden]
        public sealed class BooleanNegationConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var boxedBool = value as bool?;
                var boolValue = (boxedBool != null && boxedBool.Value);
                return !boolValue;
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                var boxedBool = (value as bool?);
                var boolValue = (boxedBool != null && boxedBool.Value);
                return !boolValue;
            }
        }
    }
}

