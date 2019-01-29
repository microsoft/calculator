// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Converters
    {
        /// <summary>
        /// Value converter that translates binary value to automation name for a bit.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class BitFlipAutomationNameConverter sealed : Windows::UI::Xaml::Data::IValueConverter
        {
        public:
            virtual Platform::Object^ Convert(_In_ Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, _In_ Platform::Object^ parameter, _In_ Platform::String^ language);
            virtual Platform::Object^ ConvertBack(_In_ Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, _In_ Platform::Object^ parameter, _In_ Platform::String^ language);

        };
    }
}
