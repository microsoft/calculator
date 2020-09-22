// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// pch.h
// Header for standard system include files.
//

#pragma once

// Windows headers define min/max macros.
// Disable it for project code.
#define NOMINMAX

#include <collection.h>
#include <unordered_map>
#include <map>
#include <ppltasks.h>
#include <pplawait.h>
#include <memory>
#include <mutex>
#include <cassert>
#include <locale>
#include <sal.h>
#include <sstream>
#include <string>
#include <concrt.h>
#include <regex>
#include <string>
#include <tuple>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <WindowsNumerics.h>

// C++\WinRT Headers
#include "winrt/base.h"
#include "winrt/Windows.ApplicationModel.h"
#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.Foundation.Diagnostics.h"
#include "winrt/Windows.Globalization.h"
#include "winrt/Windows.Globalization.DateTimeFormatting.h"
#include "winrt/Windows.System.UserProfile.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.Foundation.h"

// Project Headers
#include "App.xaml.h"
