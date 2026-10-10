// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for MSVC <intsafe.h>.
// The engine vendors the one safe-conversion helper it needs
// (Calc_ULongLongToULong in Ratpack/conv.cpp), so this header is
// intentionally empty. It exists only so that `#include <intsafe.h>`
// (via pch.h on MSVC builds) resolves when compiling with GCC.
#pragma once

#include <cstdint>
