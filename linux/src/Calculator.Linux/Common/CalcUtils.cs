// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Pure managed helpers ported from CalculatorApp.ViewModel.Common.Utils.
// UI-bound members (colors, dispatchers, display info, app-titlebar helpers)
// are NOT ported: they belong to the Avalonia shell (Task 9).
namespace Calculator.Linux.Common;

internal static class CalcUtils
{
    public static bool IsLastCharacterTarget(string input, char target) =>
        !string.IsNullOrEmpty(input) && input[input.Length - 1] == target;
}
