// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "BooleanToDoubleNegationConverter.h"

using namespace CalculatorApp::Common;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;

namespace CalculatorApp
{
    namespace Converters
    {
        Object ^ BooleanToDoubleNegationConverter::Convert(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
        {
            (void)targetType; // Unused parameter
            (void)parameter;  // Unused parameter
            (void)language;   // Unused parameter

            auto boxedBool = dynamic_cast<Box<bool> ^>(value);
            auto boolValue = (boxedBool != nullptr && boxedBool->Value);
            return boolValue ? 0.0 : 1.0;
        }

        Object ^ BooleanToDoubleNegationConverter::ConvertBack(Object ^ value, TypeName targetType, Object ^ parameter, String ^ language)
        {
            throw ref new NotImplementedException();
        }
    }
}
