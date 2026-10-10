// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for MSVC <winerror.h>.
//
// Only pch.h includes this directly; all real HRESULT-style usage in the
// engine goes through winerror_cross_platform.h (which defines these same
// macros on non-Windows builds). Values below deliberately match
// winerror_cross_platform.h so including both is harmless (guarded).
#pragma once

#include <cstdint>

typedef int32_t HRESULT;

#ifndef S_OK
#define S_OK 0x0
#endif
#ifndef S_FALSE
#define S_FALSE 0x1
#endif
#ifndef E_FAIL
#define E_FAIL 0x80004005
#endif
#ifndef E_INVALIDARG
#define E_INVALIDARG 0x80070057
#endif
#ifndef E_OUTOFMEMORY
#define E_OUTOFMEMORY 0x8007000E
#endif
#ifndef E_POINTER
#define E_POINTER 0x80004003
#endif
#ifndef E_UNEXPECTED
#define E_UNEXPECTED 0x8000FFFF
#endif
#ifndef E_ACCESSDENIED
#define E_ACCESSDENIED 0x80070005
#endif
#ifndef E_BOUNDS
#define E_BOUNDS 0x8000000B
#endif
