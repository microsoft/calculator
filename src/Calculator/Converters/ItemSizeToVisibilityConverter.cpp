// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ItemSizeToVisibilityConverter.h"
#include "BooleanToVisibilityConverter.h"

using namespace CalculatorApp::Converters;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;

Object ^ ItemSizeToVisibilityConverter::Convert(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
{
    (void)targetType; // Unused parameter
    (void)parameter;  // Unused parameter
    (void)language;   // Unused parameter

    auto items = dynamic_cast<Box<int> ^>(value);
    auto boolValue = (items != nullptr && (items->Value == 0));
    return BooleanToVisibilityConverter::Convert(boolValue);
}

Object ^ ItemSizeToVisibilityConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
{
    throw ref new NotImplementedException();
}

Object ^ ItemSizeToVisibilityNegationConverter::Convert(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
{
    (void)targetType; // Unused parameter
    (void)parameter;  // Unused parameter
    (void)language;   // Unused parameter

    auto items = dynamic_cast<Box<int> ^>(value);
    auto boolValue = (items != nullptr && (items->Value > 0));
    return BooleanToVisibilityConverter::Convert(boolValue);
}

Object ^ ItemSizeToVisibilityNegationConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
{
    throw ref new NotImplementedException();
}
