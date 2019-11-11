// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IsLineEnabledToOpacityConverter.h"

using namespace CalculatorApp::Converters;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Interop;

Object ^ IsLineEnabledToOpacityConverter::Convert(Object ^ value, TypeName /*targetType*/, Object ^ /*parameter*/, String ^ /*language*/)
{
    auto boxedBool = dynamic_cast<Box<bool> ^>(value);
    auto boolValue = (boxedBool != nullptr && boxedBool->Value);

    return boolValue ? 1.0 : 0.7;
}

Object ^ IsLineEnabledToOpacityConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
{
    (void)targetType; // Unused parameter
    (void)parameter;  // Unused parameter
    (void)language;   // Unused parameter

    auto boxedBool = dynamic_cast<Box<bool> ^>(value);
    auto boolValue = (boxedBool != nullptr && boxedBool->Value);
    return Convert(!boolValue, targetType, parameter, language);
}
