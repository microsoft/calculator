using System;
using System.Collections.Generic;
using System.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.Converters
{
    public class BooleanToVisibilityConverter : IValueConverter
    {
        public static Visibility Convert(bool visibility)
        {
            return visibility ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;
        }

        public object Convert(object value, Type targetType, object parameter, string culture)
        {
            bool? boxedBool = value as bool?;

            if (boxedBool != null)
            {
                return boxedBool.Value ? Visibility.Visible : Visibility.Collapsed;
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

    public class BooleanToVisibilityNegationConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string culture)
        {
            bool? boxedBool = value as bool?;

            if (boxedBool != null)
            {
                return boxedBool.Value ? Visibility.Collapsed : Visibility.Visible;
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
