// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Pure managed helpers ported from CalculatorApp.ViewModel.Common.Utils.
// UI-bound members (colors, dispatchers, display info) are intentionally
// NOT ported: no Avalonia equivalent is needed (see PORTING-NOTES.md).
namespace Calculator.Linux.Common;

internal static class CalcUtils
{
    public static bool IsLastCharacterTarget(string input, char target) =>
        !string.IsNullOrEmpty(input) && input[input.Length - 1] == target;

    public static DateTimeOffset GetUniversalSystemTime() => DateTimeOffset.UtcNow;

    public static bool IsDateTimeOlderThan(DateTimeOffset dateTime, long durationInTicks) =>
        (GetUniversalSystemTime() - dateTime).Ticks > durationInTicks;
}
