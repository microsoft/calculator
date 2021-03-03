// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

//C4453: A '[WebHostHidden]' type should not be used on the published surface of a public type that is not '[WebHostHidden]' 
// This warning is disabled because the types in this app will not be published for use in javascript environment
#pragma warning(disable:4453)

// Windows headers define a min/max macro.
// Include the below #def to avoid this behavior.
#define NOMINMAX

#include <collection.h>
#include <ppltasks.h>
#include <pplawait.h>
#include <concrt.h>
#include <future>
#include <memory>
#include <cassert>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <type_traits>

// DirectX headers
#include <d2d1_3.h>
#include <d3d11_4.h>
#include <dwrite_3.h>
#include <DirectXColors.h>
#include <wincodec.h>
#include <wrl.h>
#include <windows.ui.xaml.media.dxinterop.h>

// C++/WinRT
#include "winrtHeaders.h"

#include "Control/Grapher.h"
