// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Avalonia;

namespace Calculator.Linux;

internal static class Program
{
    // Avalonia desktop entry point. The engine must only be touched from the
    // UI thread (native globals are single-threaded); all ViewModel calls in
    // this app originate here.
    public static void Main(string[] args)
    {
        BuildAvaloniaApp().StartWithClassicDesktopLifetime(args);
    }

    public static AppBuilder BuildAvaloniaApp() =>
        AppBuilder.Configure<App>()
            .UsePlatformDetect()
            .WithInterFont()
            .LogToTrace();
}
