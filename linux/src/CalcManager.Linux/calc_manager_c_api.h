// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// C ABI for the Linux port of Windows Calculator's CalcManager engine.
// Upstream: microsoft/calculator @ d125246a4e19842ce1332e6c7839cf0e110027d8
//
// Why this file exists: the Windows build talks to CalcManager through
// CalcManager.Interop (C++/CX + WinRT .idl), which cannot compile on Linux.
// This header exposes the same engine over a plain C ABI consumed from C#
// via P/Invoke (Task 2). See SPEC-linux-port.md and tasks/plan.md.
//
// String encoding: UTF-8 everywhere (char*). The engine itself uses
// std::wstring (UTF-32 on Linux); conversion happens inside the shim.
// This deliberately avoids wchar_t across the boundary because wchar_t is
// 2 bytes on Windows but 4 bytes on Linux and would corrupt C# CharSet.Unicode
// marshalling.
//
// Threading: all functions are synchronous and NOT thread-safe. Callbacks fire
// synchronously during calc_manager_send_command and friends, on the calling
// thread. Callback pointers are valid ONLY for the duration of the call.
//
// Versioning rule (frozen core): existing signatures never change. New engine
// surface (history items, unit converter) is added as NEW functions.
// Task 1 freezes everything declared here.
#pragma once

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#if defined(_WIN32)
#define CALC_MANAGER_API __declspec(dllexport)
#else
#define CALC_MANAGER_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    // --- Display callbacks (mirror ICalcDisplay, UTF-8) ---------------------

    // Primary display changed. utf8Text is NUL-terminated, valid during call.
    typedef void (*CalcPrimaryDisplayFn)(const char* utf8Text, int isError, void* userData);
    typedef void (*CalcIsInErrorFn)(int isError, void* userData);

    // Expression tokens. tokens[i] is NUL-terminated UTF-8, indices[i] is the
    // command index for tokens[i]; count may be 0 (expression cleared).
    // All pointers valid only during the call.
    typedef void (*CalcExpressionFn)(const char** tokens, const int* indices, int count, void* userData);

    typedef void (*CalcParenthesisFn)(unsigned int count, void* userData);
    typedef void (*CalcSimpleFn)(void* userData); // no-right-paren, max-digits, binary-op, input-changed
    typedef void (*CalcHistoryAddedFn)(unsigned int index, void* userData);
    // Memorized-numbers vector replaced (SetMemorizedNumbers) or one item
    // changed (MemoryItemChanged, index >= 0) / all cleared (index == -1).
    typedef void (*CalcMemoryFn)(int changedIndex, void* userData);

    typedef struct CalcDisplayCallbacks
    {
        void* userData;
        CalcPrimaryDisplayFn onPrimaryDisplay; // required
        CalcIsInErrorFn onIsInError;           // may be NULL
        CalcExpressionFn onExpression;         // may be NULL
        CalcParenthesisFn onParenthesis;       // may be NULL
        CalcSimpleFn onNoRightParenAdded;      // may be NULL
        CalcSimpleFn onMaxDigitsReached;       // may be NULL
        CalcSimpleFn onBinaryOperatorReceived; // may be NULL
        CalcHistoryAddedFn onHistoryItemAdded; // may be NULL
        CalcMemoryFn onMemoryChanged;          // may be NULL
        CalcSimpleFn onInputChanged;           // may be NULL
    } CalcDisplayCallbacks;

    typedef struct CalcManagerOpaque* CalcManagerHandle;

    // Create an engine instance. decimalSep/thousandSep/grouping are UTF-8
    // ("sDecimal"/"sThousand"/"sGrouping"); NULL selects en-US defaults
    // (".", ",", "3;0"). callbacks->onPrimaryDisplay must be non-NULL.
    // Returns NULL on failure.
    CALC_MANAGER_API CalcManagerHandle calc_manager_create(
        const CalcDisplayCallbacks* callbacks,
        const char* decimalSep,
        const char* thousandSep,
        const char* grouping);

    CALC_MANAGER_API void calc_manager_destroy(CalcManagerHandle h);

    CALC_MANAGER_API void calc_manager_reset(CalcManagerHandle h, int clearMemory);
    CALC_MANAGER_API void calc_manager_set_standard_mode(CalcManagerHandle h);
    CALC_MANAGER_API void calc_manager_set_scientific_mode(CalcManagerHandle h);
    CALC_MANAGER_API void calc_manager_set_programmer_mode(CalcManagerHandle h);

    // commandId matches CalculationManager::Command numeric values (Command.h).
    CALC_MANAGER_API void calc_manager_send_command(CalcManagerHandle h, int commandId);

    // --- Memory ---------------------------------------------------------------
    CALC_MANAGER_API void calc_manager_memorize(CalcManagerHandle h);
    CALC_MANAGER_API void calc_manager_memory_load(CalcManagerHandle h, unsigned int index);
    CALC_MANAGER_API void calc_manager_memory_add(CalcManagerHandle h, unsigned int index);
    CALC_MANAGER_API void calc_manager_memory_subtract(CalcManagerHandle h, unsigned int index);
    CALC_MANAGER_API void calc_manager_memory_clear(CalcManagerHandle h, unsigned int index);
    CALC_MANAGER_API void calc_manager_memory_clear_all(CalcManagerHandle h);

    // --- State queries ----------------------------------------------------------
    CALC_MANAGER_API int calc_manager_is_engine_recording(CalcManagerHandle h);
    CALC_MANAGER_API int calc_manager_is_input_empty(CalcManagerHandle h);
    CALC_MANAGER_API int calc_manager_is_in_error(CalcManagerHandle h);

    // Pull accessors (mirror the last pushed values; useful for tests).
    // Returns the bytes needed INCLUDING the NUL terminator. If buffer is
    // NULL or capacity is 0, nothing is written and the needed size is
    // returned. Truncation keeps the output NUL-terminated.
    CALC_MANAGER_API int calc_manager_get_primary_display(CalcManagerHandle h, char* buffer, int capacity);

    CALC_MANAGER_API void calc_manager_set_radix(CalcManagerHandle h, int radixType);
    CALC_MANAGER_API void calc_manager_set_precision(CalcManagerHandle h, int precision);
    CALC_MANAGER_API void calc_manager_update_max_int_digits(CalcManagerHandle h);
    CALC_MANAGER_API int calc_manager_get_result_for_radix(
        CalcManagerHandle h,
        unsigned int radix,
        int precision,
        int groupDigits,
        char* buffer,
        int capacity);
    CALC_MANAGER_API int calc_manager_get_decimal_separator(CalcManagerHandle h, char* buffer, int capacity);

    // --- History (counts only; item marshalling is added in Task 2) ------------
    CALC_MANAGER_API unsigned int calc_manager_history_count(CalcManagerHandle h);
    CALC_MANAGER_API void calc_manager_history_clear(CalcManagerHandle h);
    CALC_MANAGER_API int calc_manager_get_current_degree_mode(CalcManagerHandle h);
    CALC_MANAGER_API void calc_manager_set_in_history_load_mode(CalcManagerHandle h, int enabled);

    // --- Task 4 additive section ------------------------------------------------
    // History/memory/command marshalling. Added additively; everything above
    // is unchanged (frozen since Task 1).
    //
    // Modes for history access: 0 = standard, 1 = scientific.
    // CommandType ids mirror CalculationManager::CommandType
    // (0 Unary, 1 Binary, 2 Operand, 3 Parentheses); tokens without a command
    // report -1 for both index and type.

    // Expression display with per-token command types. Replaces onExpression
    // for new consumers (legacy field kept and still invoked).
    typedef void (*CalcExpressionExFn)(
        const char** tokens,
        const int* indices,
        const int* cmdTypes,
        int count,
        void* userData);

    // NOTE: append new callback fields only at the end of this struct.
    typedef struct CalcDisplayCallbacksEx
    {
        CalcDisplayCallbacks base;
        CalcExpressionExFn onExpressionEx; // may be NULL
    } CalcDisplayCallbacksEx;

    // Create with extended callbacks (base part behaves exactly like
    // calc_manager_create). Returns NULL on failure.
    CALC_MANAGER_API CalcManagerHandle calc_manager_create_ex(
        const CalcDisplayCallbacksEx* callbacks,
        const char* decimalSep,
        const char* thousandSep,
        const char* grouping);

    // History item texts. Returns 0 on success, -1 for bad mode/index.
    // Buffers follow the CopyToBuffer convention (NUL-terminated, truncated).
    CALC_MANAGER_API int calc_manager_history_item(
        CalcManagerHandle h,
        int mode,
        unsigned int index,
        char* expression,
        int expressionCapacity,
        char* result,
        int resultCapacity);

    CALC_MANAGER_API unsigned int calc_manager_history_count_for_mode(CalcManagerHandle h, int mode);
    CALC_MANAGER_API unsigned int calc_manager_max_history_size(CalcManagerHandle h);
    // Remove from the CURRENT mode history (mirrors native RemoveHistoryItem).
    // Returns 1 when an item was removed.
    CALC_MANAGER_API int calc_manager_remove_history_item(CalcManagerHandle h, unsigned int index);

    // History item tokens. Returns token count, or 0 for bad mode/index.
    CALC_MANAGER_API unsigned int calc_manager_history_item_token_count(CalcManagerHandle h, int mode, unsigned int index);
    // Returns 0 on success, -1 for bad index. cmdIndex/cmdType may be NULL.
    CALC_MANAGER_API int calc_manager_history_item_token(
        CalcManagerHandle h,
        int mode,
        unsigned int item,
        unsigned int token,
        char* buffer,
        int capacity,
        int* cmdIndex,
        int* cmdType);

    // Flat engine command ids for replaying an item (see
    // StandardCalculatorViewModel.Recalculate). Returns the needed count;
    // with out==NULL or cap==0 nothing is written. Order matches the
    // upstream GetCommandsFromExpressionCommands flattening.
    CALC_MANAGER_API unsigned int calc_manager_history_item_commands(
        CalcManagerHandle h,
        int mode,
        unsigned int index,
        int* out,
        unsigned int capacity);

    // Flat command ids of the live display (Recalculate parity).
    CALC_MANAGER_API unsigned int calc_manager_display_commands(CalcManagerHandle h, int* out, unsigned int capacity);

    // Memorized numbers (cached from the SetMemorizedNumbers callback).
    CALC_MANAGER_API unsigned int calc_manager_memorized_count(CalcManagerHandle h);
    // Returns needed size incl. NUL, or -1 for bad index.
    CALC_MANAGER_API int calc_manager_memorized_at(CalcManagerHandle h, unsigned int index, char* buffer, int capacity);

#ifdef __cplusplus
} // extern "C"
#endif
