// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows headers define min/max macros.
// Disable it for project code.
#define NOMINMAX

#include <assert.h>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
#include <memory>
#include <vector>
#include <limits>
#include <list>
#include <regex>
#include <unordered_map>
#include <array>
#include <string_view>

#if defined(_WIN32) && defined(_MSC_VER)

#include <windows.h>
#include <winerror.h>
#include <intsafe.h>
#include <ppltasks.h>

#else

// #include <intsafe.h>
// #include <ppltasks.h>

#include "winerror_cross_platform.h"
#include "win_data_types_cross_platform.h"
#include "sal_cross_platform.h"

#ifdef __GNUC__
#define DECLSPEC_SELECTANY __attribute__((selectany))
#else
#define DECLSPEC_SELECTANY __declspec(selectany)
#endif

#endif
