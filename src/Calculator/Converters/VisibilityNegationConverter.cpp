// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "VisibilityNegationConverter.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;

namespace CalculatorApp
{
    namespace Common
    {
        Object ^ VisibilityNegationConverter::Convert(Object ^ value, TypeName /*targetType*/, Object ^ /*parameter*/, String ^ /*language*/)
        {
            auto boxedVisibility = dynamic_cast<Box<Visibility> ^>(value);
            if (boxedVisibility != nullptr && boxedVisibility->Value == Visibility::Collapsed)
            {
                return Visibility::Visible;
            }
            return Visibility::Collapsed;
        }

        Object ^ VisibilityNegationConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
        {
            return Convert(value, targetType, parameter, language);
        }
    }
}
