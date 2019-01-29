// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// pch.h
// Header for standard system include files.
//

#pragma once

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
#include <concrt.h>
#include <regex>

// C++\WinRT Headers
#include "winrt\base.h"
#include "winrt\Windows.ApplicationModel.h"
#include "winrt\Windows.ApplicationModel.Core.h"
#include "winrt\Windows.Foundation.Diagnostics.h"
#include "winrt\Windows.Globalization.h"
#include "winrt\Windows.Globalization.DateTimeFormatting.h"
#include "winrt\Windows.System.UserProfile.h"
#include "winrt\Windows.UI.ViewManagement.h"
#include "winrt\Windows.UI.Xaml.h"

// The following namespaces exist as a convenience to resolve
// ambiguity for Windows types in the Windows::UI::Xaml::Automation::Peers
// namespace that only exist on RS3.
// Once the app switches to min version RS3, the namespaces can be removed.
// TODO - MSFT 12735088
namespace StandardPeers = Windows::UI::Xaml::Automation::Peers;
namespace CalculatorApp::Common::Automation {}
namespace CustomPeers = CalculatorApp::Common::Automation;

// CalcManager Headers
#include "CalcManager\CalculatorVector.h"
#include "CalcManager\ExpressionCommand.h"
#include "CalcManager\CalculatorResource.h"
#include "CalcManager\CalculatorManager.h"
#include "CalcManager\UnitConverter.h"

//CalcViewModel Headers
#include "CalcViewModel\Common\DelegateCommand.h"
#include "CalcViewModel\Common\Utils.h"
#include "CalcViewModel\Common\CalculatorButtonUser.h"
#include "CalcViewModel\Common\MyVirtualKey.h"
#include "CalcViewModel\Common\NavCategory.h"
#include "CalcViewModel\Common\TraceLogger.h"

// Project Headers

#include "App.xaml.h"
