// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// The CalcManager project should be able to be compiled with or without a precompiled header
// in - order to support other toolchains besides MSVC. When adding new system headers, make sure
// that the relevant source file includes all headers it needs, but then also add the system headers
// here so that MSVC users see the performance benefit.

#include <algorithm>
#include <array>
#include <cassert>
#include <intsafe.h>
#include <list>
#include <future>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <winerror.h>
#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>
