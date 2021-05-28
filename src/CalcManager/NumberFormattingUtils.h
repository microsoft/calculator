// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "sal_cross_platform.h"

namespace CalcManager::NumberFormattingUtils
{
    void TrimTrailingZeros(_Inout_ std::wstring& input);
    unsigned int GetNumberDigits(std::wstring value);
    unsigned int GetNumberDigitsWholeNumberPart(double value);
    std::wstring RoundSignificantDigits(double value, unsigned int numberSignificantDigits);
    std::wstring ToScientificNumber(double number);
}
