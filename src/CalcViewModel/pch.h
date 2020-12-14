// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows headers define min/max macros.
// Disable it for project code.
#define NOMINMAX

#include <windows.h>

#include <collection.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <ppltasks.h>
#include <pplawait.h>
#include <memory>
#include <mutex>
#include <cassert>
#include <locale>
#include <sal.h>
#include <sstream>
#include <concrt.h>
#include <regex>
#include <iterator>
#include <intsafe.h>
#include <ppltasks.h>

// C++\WinRT Headers
#include "winrt/base.h"
#include "winrt/Windows.Foundation.Diagnostics.h"
#include "winrt/Windows.Globalization.h"
#include "winrt/Windows.Globalization.DateTimeFormatting.h"
#include "winrt/Windows.System.UserProfile.h"
#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.Foundation.Metadata.h"
#include "winrt/Windows.Management.Policies.h"

// The following namespaces exist as a convenience to resolve
// ambiguity for Windows types in the Windows::UI::Xaml::Automation::Peers
// namespace that only exist on RS3.
// Once the app switches to min version RS3, the namespaces can be removed.
// TODO - MSFT 12735088
namespace StandardPeers = Windows::UI::Xaml::Automation::Peers;
namespace CalculatorApp::Common::Automation
{
}
namespace CustomPeers = CalculatorApp::Common::Automation;
