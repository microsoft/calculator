// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "RadixToStringConverter.h"
#include "CalcManager/Header Files/RadixType.h"
#include "CalcViewModel/Common/AppResourceProvider.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::ApplicationModel::Resources;

namespace CalculatorApp
{
    namespace Converters
    {
        Object^ RadixToStringConverter::Convert(Object^ value, TypeName targetType, Object^ parameter, String^ language)
        {
            (void)targetType;    // Unused parameter
            (void)parameter;    // Unused parameter
            (void)language;    // Unused parameter

            auto boxedInt = dynamic_cast<Box<int>^>(value);
            Platform::String^ convertedValue;
            auto resourceLoader = AppResourceProvider::GetInstance();
            switch (boxedInt->Value)
            {
            case RADIX_TYPE::BIN_RADIX:
            {
                convertedValue = resourceLoader.GetResourceString("Bin");
                break;
            }
            case RADIX_TYPE::OCT_RADIX:
            {
                convertedValue = resourceLoader.GetResourceString("Oct");
                break;
            }
            case RADIX_TYPE::DEC_RADIX:
            {
                convertedValue = resourceLoader.GetResourceString("Dec");
                break;
            }
            case RADIX_TYPE::HEX_RADIX:
            {
                convertedValue = resourceLoader.GetResourceString("Hex");
                break;
            }
            default:
                break;

            };
            return convertedValue;


        }

        Object^ RadixToStringConverter::ConvertBack(Object^ value, TypeName targetType, Object^ parameter, String^ language)
        {
            throw ref new NotImplementedException();
        }

    }
}
