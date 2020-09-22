// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "RadixButton.h"
#include "CalcViewModel/Common/LocalizationSettings.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::Controls;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Data;

String ^ RadixButton::GetRawDisplayValue()
{
    String ^ radixContent = Content->ToString();
    return LocalizationSettings::GetInstance().RemoveGroupSeparators(radixContent);
}
