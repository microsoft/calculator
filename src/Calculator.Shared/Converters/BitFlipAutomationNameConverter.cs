// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Common;
using System;
using System.Text;

namespace CalculatorApp
{
    namespace Converters
    {
        /// <summary>
        /// Value converter that translates binary value to automation name for a bit.
        /// </summary>
        public class BitFlipAutomationNameConverter : Windows.UI.Xaml.Data.IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, string language)
            {
                var resourceLoader = AppResourceProvider.GetInstance();

                // initialising the updated display with 64 bits of zeros
                char[] updatedBinaryDisplay = new char[64];

                var localizationSettings = LocalizationSettings.GetInstance();
                char ch0 = localizationSettings.GetDigitSymbolFromEnUsDigit('0');
                char ch1 = localizationSettings.GetDigitSymbolFromEnUsDigit('1');

                string indexName = resourceLoader.GetResourceString((string)(parameter));
                string bitName = resourceLoader.GetResourceString("BitAutomationName");
                string valueName = resourceLoader.GetResourceString("ValueAutomationName");
                string zero = resourceLoader.GetResourceString("BinaryZeroValueAutomationName");
                string one = resourceLoader.GetResourceString("BinaryOneValueAutomationName");
                if ((value != null) && (parameter != null))
                {
                    string binaryDisplay = (string)value;
                    string indexString = (string)parameter;
                    int index = int.Parse(indexString);
                    int binaryLength = 0;

                    // remove all the characters except 0 and 1 from the array.
                    foreach (char bit in binaryDisplay)
                    {
                        if ((bit == ch1) || (bit == ch0))
                        {
                            updatedBinaryDisplay[binaryLength++] = bit;
                        }
                        if (binaryLength == 63)
                        {
                            break;
                        }
                    }

                    // return if binaryDisplay is empty
                    if (binaryLength == 0)
                    {
                        return (indexName + bitName + valueName + zero);
                    }

                    // if index is more than the length of binary display return automation name with zero
                    if (index >= binaryLength)
                    {
                        return (indexName + bitName + valueName + zero);
                    }
                    // if bit is set return automation name with one else return zero
                    if (updatedBinaryDisplay[binaryLength - index - 1] == ch1)
                    {
                        return (indexName + bitName + valueName + one);
                    }
                }
                return (indexName + bitName + valueName + zero);
            }

            public object ConvertBack(object value, Type targetType, object parameter, string language)
            {
                return value;
            }

        }
    }
}
