// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cstdint>

// CalcErr.h
//
// Defines the error codes thrown by ratpak and caught by Calculator
//
//
//  Ratpak errors are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-------+---------------------+-------------------------------+
//  |S|   R   |    Facility         |               Code            |
//  +-+-------+---------------------+-------------------------------+
//
//  where
//
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail
//
//      R - Reserved - not currently used for anything
//
//      r - reserved portion of the facility code. Reserved for internal
//              use. Used to indicate int32_t values that are not status
//              values, but are instead message ids for display strings.
//
//      Facility - is the facility code
//
//      Code - is the actual error code
//
// This format is based loosely on an OLE HRESULT and is compatible with the
// SUCCEEDED and FAILED macros as well as the HRESULT_CODE macro

using ResultCode = int32_t;

// CALC_E_DIVIDEBYZERO
//
// The current operation would require a divide by zero to complete
static constexpr uint32_t CALC_E_DIVIDEBYZERO = (uint32_t)0x80000000;

// CALC_E_DOMAIN
//
// The given input is not within the domain of this function
static constexpr uint32_t CALC_E_DOMAIN = (uint32_t)0x80000001;

// CALC_E_INDEFINITE
//
// The result of this function is undefined
static constexpr uint32_t CALC_E_INDEFINITE = (uint32_t)0x80000002;

// CALC_E_POSINFINITY
//
// The result of this function is Positive Infinity.
static constexpr uint32_t CALC_E_POSINFINITY = (uint32_t)0x80000003;

// CALC_E_NEGINFINITY
//
// The result of this function is Negative Infinity
static constexpr uint32_t CALC_E_NEGINFINITY = (uint32_t)0x80000004;

// CALC_E_INVALIDRANGE
//
// The given input is within the domain of the function but is beyond
// the range for which calc can successfully compute the answer
static constexpr uint32_t CALC_E_INVALIDRANGE = (uint32_t)0x80000006;

// CALC_E_OUTOFMEMORY
//
// There is not enough free memory to complete the requested function
static constexpr uint32_t CALC_E_OUTOFMEMORY = (uint32_t)0x80000007;

// CALC_E_OVERFLOW
//
// The result of this operation is an overflow
static constexpr uint32_t CALC_E_OVERFLOW = (uint32_t)0x80000008;

// CALC_E_NORESULT
//
// The result of this operation is undefined
static constexpr uint32_t CALC_E_NORESULT = (uint32_t)0x80000009;
