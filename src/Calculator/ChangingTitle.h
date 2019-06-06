// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// ChangingTitle.h
// To differentiate the titles of the Developer and Retail build windows.
//

#pragma once

// In the retail version change the TitleTag macro definition to 1.
#define TitleTag 0

#include "../CalcViewModel/Common/AppResourceProvider.h"

using namespace Platform;

namespace CalculatorApp{
    
    #if TitleTag
       Platform::String ^ AppNameLocal = AppResourceProvider::GetInstance().GetResourceString(L"AppName");
    #else  
       Platform::String ^ AppNameLocal =  AppResourceProvider::GetInstance().GetResourceString(L"DevAppName");
    #endif

}
