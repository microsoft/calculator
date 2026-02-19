// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// pch.h
// Header for standard system include files.
//

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows headers define min/max macros.
// Disable it for project code.
#define NOMINMAX

#define UNIT_TESTS

#include <windows.h>
#include <collection.h>
#include <ppltasks.h>
#include <cassert>
#include <concrt.h>
#include <string>
#include <bitset>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <deque>
#include <regex>
#include <concurrent_vector.h>
#include <experimental/resumable>
#include <pplawait.h>
#include <unordered_map>
#include <mutex>
#include <locale>
#include <sal.h>
#include <sstream>

// C++\WinRT Headers
#include "winrt/base.h"
#include "winrt/Windows.Foundation.Diagnostics.h"
#include "winrt/Windows.Globalization.h"
#include "winrt/Windows.Globalization.DateTimeFormatting.h"
#include "winrt/Windows.System.UserProfile.h"

// CalcManager Headers
#include "CalcManager/ExpressionCommand.h"
#include "CalcManager/CalculatorResource.h"
#include "CalcManager/CalculatorManager.h"
#include "CalcManager/UnitConverter.h"

// CalcViewModel Headers
#include "CalcViewModel/Common/DelegateCommand.h"
#include "CalcViewModel/Common/Utils.h"
#include "CalcViewModel/Common/MyVirtualKey.h"
#include "CalcViewModel/Common/NavCategory.h"
#include "CalcViewModel/Common/CalculatorButtonUser.h"
#include "CalcViewModel/Common/NetworkManager.h"

#include "Helpers.h"

#include "UnitTestApp.xaml.h"
