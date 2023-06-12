// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using System;

namespace CalculatorApp
{
    namespace Converters
    {
        /// <summary>
        /// Value converter that translates true to false and vice versa.
        /// </summary>
        [Windows.Foundation.Metadata.WebHostHidden]
        public sealed class RadixToStringConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var boxedInt = (value as int?);
                string convertedValue = null;
                var resourceLoader = AppResourceProvider.GetInstance();
                switch (boxedInt.Value)
                {
                    case (int)RadixType.Binary:
                        {
                            convertedValue = resourceLoader.GetResourceString("Bin");
                            break;
                        }
                    case (int)RadixType.Octal:
                        {
                            convertedValue = resourceLoader.GetResourceString("Oct");
                            break;
                        }
                    case (int)RadixType.Decimal:
                        {
                            convertedValue = resourceLoader.GetResourceString("Dec");
                            break;
                        }
                    case (int)RadixType.Hex:
                        {
                            convertedValue = resourceLoader.GetResourceString("Hex");
                            break;
                        }
                    default:
                        break;
                }

                return convertedValue;
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                throw new NotImplementedException();
            }
        }
    }
}

