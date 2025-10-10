// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

using Windows.UI.Xaml;

namespace CalculatorApp
{
    namespace Common
    {
        /// <summary>
        /// Value converter that translates Visible to Collapsed and vice versa
        /// </summary>
        [Windows.Foundation.Metadata.WebHostHidden]
        public sealed class VisibilityNegationConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, string language)
            {
                if (value is Visibility boxedVisibility && boxedVisibility == Visibility.Collapsed)
                {
                    return Visibility.Visible;
                }
                return Visibility.Collapsed;
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                return Convert(value, targetType, parameter, language);
            }
        }
    }
}
