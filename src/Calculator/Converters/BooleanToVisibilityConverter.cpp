// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BooleanToVisibilityConverter.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;

namespace CalculatorApp
{
    namespace Converters
    {
        Object ^ BooleanToVisibilityConverter::Convert(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
        {
            (void)targetType; // Unused parameter
            (void)parameter;  // Unused parameter
            (void)language;   // Unused parameter

            auto boxedBool = dynamic_cast<Box<bool> ^>(value);
            auto boolValue = (boxedBool != nullptr && boxedBool->Value);
            return BooleanToVisibilityConverter::Convert(boolValue);
        }

        Object ^ BooleanToVisibilityConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
        {
            (void)targetType; // Unused parameter
            (void)parameter;  // Unused parameter
            (void)language;   // Unused parameter

            auto visibility = dynamic_cast<Box<Visibility> ^>(value);
            return (visibility != nullptr && visibility->Value == Visibility::Visible);
        }

        Object ^ BooleanToVisibilityNegationConverter::Convert(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
        {
            (void)targetType; // Unused parameter
            (void)parameter;  // Unused parameter
            (void)language;   // Unused parameter

            auto boxedBool = dynamic_cast<Box<bool> ^>(value);
            auto boolValue = (boxedBool != nullptr && boxedBool->Value);
            return BooleanToVisibilityConverter::Convert(!boolValue);
        }

        Object ^ BooleanToVisibilityNegationConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
        {
            (void)targetType; // Unused parameter
            (void)parameter;  // Unused parameter
            (void)language;   // Unused parameter

            auto visibility = dynamic_cast<Box<Visibility> ^>(value);
            return (visibility != nullptr && visibility->Value != Visibility::Visible);
        }

    }
}
