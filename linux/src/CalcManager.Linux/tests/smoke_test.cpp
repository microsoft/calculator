// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Smoke test for the Linux C ABI: drives the REAL CalcManager engine
// (no mocks) through calc_manager_c_api.h only, the same way the C#
// P/Invoke layer will in Task 2.
//
// Vectors mirror src/CalculatorUnitTests/CalculatorManagerTest.cpp behavior:
//   2 + 2 =  -> "4"      |  C -> "0"  |  1 / 0 = -> error  |  sin(30 deg) -> 0.5
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "calc_manager_c_api.h"

namespace
{
    // Command ids from src/CalcManager/Command.h (CalculationManager::Command).
    constexpr int kClear = 81;
    constexpr int kDiv = 91;
    constexpr int kAdd = 93;
    constexpr int kSin = 102;
    constexpr int kEqu = 121;
    constexpr int k0 = 130;
    constexpr int k1 = 131;
    constexpr int k2 = 132;
    constexpr int k3 = 133;

    struct Capture
    {
        std::string primary = "<unset>";
        bool isError = false;
        unsigned historyAdded = 0;
    };

    void OnPrimary(const char* text, int isError, void* userData)
    {
        auto* cap = static_cast<Capture*>(userData);
        cap->primary = text != nullptr ? text : "<null>";
        cap->isError = isError != 0;
    }

    void OnHistory(unsigned /*index*/, void* userData)
    {
        ++static_cast<Capture*>(userData)->historyAdded;
    }

    std::string Pull(CalcManagerHandle h)
    {
        char buf[256];
        calc_manager_get_primary_display(h, buf, sizeof(buf));
        return std::string(buf);
    }

    int Fail(const char* what, const std::string& got, const std::string& want)
    {
        std::printf("FAIL: %s: got '%s', want '%s'\n", what, got.c_str(), want.c_str());
        return 1;
    }
} // namespace

int main()
{
    // RED-proof validation: no callback table -> no engine.
    if (calc_manager_create(nullptr, nullptr, nullptr, nullptr) != nullptr)
    {
        std::printf("FAIL: create with NULL callbacks must return NULL\n");
        return 1;
    }
    CalcDisplayCallbacks empty{};
    if (calc_manager_create(&empty, nullptr, nullptr, nullptr) != nullptr)
    {
        std::printf("FAIL: create without onPrimaryDisplay must return NULL\n");
        return 1;
    }

    Capture cap;
    CalcDisplayCallbacks cb{};
    cb.userData = &cap;
    cb.onPrimaryDisplay = &OnPrimary;
    cb.onHistoryItemAdded = &OnHistory;

    CalcManagerHandle h = calc_manager_create(&cb, ".", ",", "3;0");
    if (h == nullptr)
    {
        std::printf("FAIL: create with valid callbacks returned NULL\n");
        return 1;
    }

    // 2 + 2 =
    calc_manager_set_standard_mode(h);
    calc_manager_send_command(h, k2);
    calc_manager_send_command(h, kAdd);
    calc_manager_send_command(h, k2);
    calc_manager_send_command(h, kEqu);
    if (cap.primary != "4" || Pull(h) != "4")
    {
        return Fail("2+2=", Pull(h), "4");
    }
    if (calc_manager_history_count(h) < 1 || cap.historyAdded < 1)
    {
        std::printf("FAIL: history not recorded (count=%u added=%u)\n", calc_manager_history_count(h), cap.historyAdded);
        return 1;
    }

    // C -> 0
    calc_manager_send_command(h, kClear);
    if (Pull(h) != "0")
    {
        return Fail("clear", Pull(h), "0");
    }

    // 1 / 0 = -> error state (divide by zero), engine stays usable after C.
    calc_manager_send_command(h, k1);
    calc_manager_send_command(h, kDiv);
    calc_manager_send_command(h, k0);
    calc_manager_send_command(h, kEqu);
    if (!cap.isError && calc_manager_is_in_error(h) == 0)
    {
        std::printf("FAIL: 1/0 did not enter error state (display='%s')\n", Pull(h).c_str());
        return 1;
    }
    calc_manager_send_command(h, kClear);
    if (calc_manager_is_in_error(h) != 0)
    {
        std::printf("FAIL: clear did not leave error state\n");
        return 1;
    }

    // Scientific: sin(30) in degrees -> 0.5
    calc_manager_set_scientific_mode(h);
    calc_manager_send_command(h, k3);
    calc_manager_send_command(h, k0);
    calc_manager_send_command(h, kSin);
    if (Pull(h) != "0.5")
    {
        return Fail("sin(30)", Pull(h), "0.5");
    }

    // Task 4 ABI: history item texts + replay commands + memorized numbers.
    if (calc_manager_history_count_for_mode(h, 0) < 1)
    {
        std::printf("FAIL: standard history empty\n");
        return 1;
    }
    char expr[256] = {};
    char result[256] = {};
    if (calc_manager_history_item(h, 0, 0, expr, sizeof(expr), result, sizeof(result)) != 0)
    {
        std::printf("FAIL: history_item(0) failed\n");
        return 1;
    }
    if (std::string(result) != "4")
    {
        return Fail("history result", result, "4");
    }
    unsigned cmdCount = calc_manager_history_item_commands(h, 0, 0, nullptr, 0);
    // "2 + 2" replays as exactly [2, +, 2] (= is not a stored command).
    int cmds[16] = {};
    if (cmdCount != 3 || calc_manager_history_item_commands(h, 0, 0, cmds, 16) != 3 || cmds[0] != 132 || cmds[1] != 93 || cmds[2] != 132)
    {
        std::printf("FAIL: history commands != [132,93,132] (count=%u)\n", cmdCount);
        return 1;
    }
    if (calc_manager_history_item(h, 0, 99, expr, sizeof(expr), result, sizeof(result)) == 0)
    {
        std::printf("FAIL: history_item(99) must fail\n");
        return 1;
    }

    calc_manager_memorize(h);
    if (calc_manager_memorized_count(h) != 1)
    {
        std::printf("FAIL: memorized count != 1\n");
        return 1;
    }
    char mem[256] = {};
    if (calc_manager_memorized_at(h, 0, mem, sizeof(mem)) < 0)
    {
        std::printf("FAIL: memorized_at(0) failed\n");
        return 1;
    }

    calc_manager_destroy(h);
    std::printf("PASS: calc_manager_smoke (2+2=4, clear, div0-error, sin30=0.5, history, history-items, memory)\n");
    return 0;
}
