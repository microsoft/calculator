// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Portable prelude force-included (-include) for every CalcManager source.
//
// Why: on MSVC the engine relies on pch.h (precompiled header) for standard
// library headers — several headers use std::wstring/vector/memory without
// including them. MSVC never notices because the PCH is always present.
// GCC has no forced PCH here, so this prelude provides the same baseline
// WITHOUT pulling in any Windows-only header. Upstream sources stay untouched.
#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
