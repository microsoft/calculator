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
#include <windows.h>
#include <winerror.h>
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
#include <intsafe.h>
#include <array>
#include <ppltasks.h>
