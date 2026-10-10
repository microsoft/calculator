// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for the C# EngineResourceProvider (WinRT CEngineStrings
// ResourceLoader). The native engine serves its own copy of this table
// (see linux/src/CalcManager.Linux); this helper covers the few strings the
// managed layer needs directly (e.g. paste/domain errors).
using Calculator.Linux.Services;

namespace Calculator.Linux.Common;

internal static class EngineStrings
{
    public static string GetCEngineString(string id) => AppResources.GetCEngineString(id);
}
