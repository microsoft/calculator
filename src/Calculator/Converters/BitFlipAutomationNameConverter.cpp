// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/Common/LocalizationSettings.h"
#include "BitFlipAutomationNameConverter.h"

using namespace CalculatorApp::Common;
using namespace CalculatorApp::Converters;
using namespace Platform;
using namespace std;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Interop;

// returns automationName in form "Nth bit Value zero/one" depending if bit is set/unset at corresponding index of binary display
Object^ BitFlipAutomationNameConverter::Convert(_In_ Object^ value, TypeName targetType, _In_ Object^ parameter, _In_ String^ language)
{
    auto resourceLoader = AppResourceProvider::GetInstance();

    // initialising the updated display with 64 bits of zeros
    wstring updatedBinaryDisplay(64, L'0');

    const auto& localizationSettings = LocalizationSettings::GetInstance();
    wchar_t ch0 = localizationSettings.GetDigitSymbolFromEnUsDigit('0');
    wchar_t ch1 = localizationSettings.GetDigitSymbolFromEnUsDigit('1');

    String^ indexName = resourceLoader.GetResourceString(static_cast<String^>(parameter));
    String^ bitName = resourceLoader.GetResourceString(L"BitAutomationName");
    String^ valueName = resourceLoader.GetResourceString(L"ValueAutomationName");
    String^ zero = resourceLoader.GetResourceString(L"BinaryZeroValueAutomationName");
    String^ one = resourceLoader.GetResourceString(L"BinaryOneValueAutomationName");
    if ((value != nullptr) && (parameter != nullptr))
    {
        wstring binaryDisplay = (static_cast<String^>(value))->Data();
        wstring indexString = (static_cast<String^>(parameter))->Data();
        wstringstream converter;
        converter << indexString;
        unsigned int index;
        converter >> index;
        unsigned int binaryLength = 0;

        // remove all the characters except 0 and 1 from the array.
        for each (wchar_t bit in binaryDisplay)
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

Object^ BitFlipAutomationNameConverter::ConvertBack(_In_ Object^ value, TypeName targetType, _In_ Object^ parameter, _In_ String^ language)
{
    return value;
}
