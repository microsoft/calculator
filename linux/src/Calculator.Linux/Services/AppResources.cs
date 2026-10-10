// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for AppResourceProvider (WinRT ResourceLoader).
// Strings come from the generated en-US tables; v1 is English-only and the
// lookup falls back exactly like upstream (resource key / empty string).
namespace Calculator.Linux.Services;

public sealed class AppResources
{
    private static readonly Lazy<AppResources> s_instance = new(() => new AppResources());
    public static AppResources Instance => s_instance.Value;

    private AppResources()
    {
    }

    public static string GetString(string key) =>
        GeneratedResources.Strings.TryGetValue(key, out string? value) ? value : key;

    public static string GetCEngineString(string key) =>
        GeneratedResources.CEngineStrings.TryGetValue(key, out string? value) ? value : string.Empty;

    // Instance shape mirrors upstream AppResourceProvider so ported ViewModels
    // keep their `GetInstance().GetResourceString(..)` call sites (Task 4+).
    public string GetResourceString(string key) => GetString(key);
    public string GetCEngineStringInstance(string key) => GetCEngineString(key);
}
