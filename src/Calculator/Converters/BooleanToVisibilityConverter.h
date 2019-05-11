// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    namespace Converters
    {
        /// <summary>
        /// Value converter that translates true to <see cref="Visibility::Visible"/> and false
        /// to <see cref="Visibility::Collapsed"/>.
        /// </summary>
    public
        ref class BooleanToVisibilityConverter sealed : Windows::UI::Xaml::Data::IValueConverter
        {
        public:
            static constexpr Windows::UI::Xaml::Visibility Convert(bool visibility)
            {
                return visibility ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
            }

            virtual Platform::Object
                ^ Convert(Platform::Object ^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^ parameter, Platform::String ^ language);
            virtual Platform::Object
                ^ ConvertBack(
                    Platform::Object ^ value,
                    Windows::UI::Xaml::Interop::TypeName targetType,
                    Platform::Object ^ parameter,
                    Platform::String ^ language);
        };

        /// <summary>
        /// Value converter that translates false to <see cref="Visibility::Visible"/> and true
        /// to <see cref="Visibility::Collapsed"/>.
        /// </summary>
    public
        ref class BooleanToVisibilityNegationConverter sealed : Windows::UI::Xaml::Data::IValueConverter
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
