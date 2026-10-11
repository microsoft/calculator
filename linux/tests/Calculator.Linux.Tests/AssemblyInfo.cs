// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// The native CalcManager engine initializes shared globals once per load
// (CCalcEngine::InitialOneTimeOnlySetup) and is single-threaded by design
// (upstream drives it from the UI thread only). Parallel xUnit collections
// would construct concurrent engine instances and crash testhost, so all
// tests in this assembly run serially.
using Avalonia;
using Avalonia.Headless;
using Xunit;

[assembly: CollectionBehavior(DisableTestParallelization = true)]

// Boots the real Avalonia app headlessly so layout tests exercise the same
// styles/templates the desktop build uses (no X server required). Skia is
// enabled with the stub drawing backend off so CaptureRenderedFrame() returns
// real pixels.
[assembly: AvaloniaTestApplication(typeof(Calculator.Linux.Tests.TestAppBuilder))]

namespace Calculator.Linux.Tests;

/// <summary>Mirrors Program.BuildAvaloniaApp, minus the X11 backend.</summary>
public static class TestAppBuilder
{
    public static AppBuilder BuildAvaloniaApp() =>
        AppBuilder.Configure<App>()
            .UseSkia()
            .UseHeadless(new AvaloniaHeadlessPlatformOptions { UseHeadlessDrawing = false })
            .WithInterFont();
}
