// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Converters
    {
        /// <summary>
        /// Value converter that translates true to false and vice versa.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden] public ref class RadixToStringConverter sealed : Windows::UI::Xaml::Data::IValueConverter
        {
        public:
            virtual Platform::Object
                ^ Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language);
            virtual Platform::Object
                ^ ConvertBack(
                    Platform::Object ^ value,
                    Windows::UI::Xaml::Interop::TypeName targetType,
                    Platform::Object ^ parameter,
                    Platform::String ^ language);
        };
    }
}
