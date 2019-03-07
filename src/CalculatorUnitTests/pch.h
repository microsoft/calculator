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

#define UNIT_TESTS

#include <windows.h>
#include <collection.h>
#include <ppltasks.h>
#include <assert.h>
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

namespace CalculatorApp
{
    namespace WF = Windows::Foundation;
    namespace WUC = Windows::UI::Core;
    namespace WX = Windows::UI::Xaml;
    namespace WXC = Windows::UI::Xaml::Controls;
    namespace WXCP = Windows::UI::Xaml::Controls::Primitives;
    namespace P = Platform;
    namespace PC = Platform::Collections;
    namespace WXI = Windows::UI::Xaml::Input;
    namespace WFC = Windows::Foundation::Collections;
    namespace WS = Windows::System;
    namespace WAR = Windows::ApplicationModel::Resources;
    namespace WXMA = Windows::UI::Xaml::Media::Animation;
    namespace WXD = Windows::UI::Xaml::Data;
    namespace WXInt = Windows::UI::Xaml::Interop;
    namespace WXM = Windows::UI::Xaml::Markup;
    namespace WXA = Windows::UI::Xaml::Automation;
}

// The following namespaces exist as a convenience to resolve
// ambiguity for Windows types in the Windows::UI::Xaml::Automation::Peers
// namespace that only exist on RS3.
// Once the app switches to min version RS3, the namespaces can be removed.
// TODO - MSFT 12735088
namespace StandardPeers = Windows::UI::Xaml::Automation::Peers;
namespace CalculatorApp::Common::Automation {}
namespace CustomPeers = CalculatorApp::Common::Automation;

//CalcManager Headers
#include "CalcManager/CalculatorVector.h"
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

#include "Mocks/CurrencyHttpClient.h"
#include "Helpers.h"

#include "UnitTestApp.xaml.h"
