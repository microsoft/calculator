// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Renders the real window offscreen (Avalonia headless + Skia) and checks the
// pixels, not just the layout numbers: the reported distortion was visual, so
// the guard has to be visual too.
//
// Set CALC_SNAPSHOT_DIR to also write the PNGs for human inspection.

using System.Runtime.InteropServices;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Styling;
using Avalonia.Headless.XUnit;
using Avalonia.LogicalTree;
using Avalonia.Headless;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using Avalonia.Threading;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class VisualSnapshotTests
{
    private static string? SnapshotDir =>
        Environment.GetEnvironmentVariable("CALC_SNAPSHOT_DIR");

    private static MainWindow Show(int width, int height)
    {
        var window = new MainWindow { Width = width, Height = height };
        window.Show();
        Dispatcher.UIThread.RunJobs();
        return window;
    }

    private static void SelectMode(MainWindow window, string tag)
    {
        var list = window.GetLogicalDescendants().OfType<ListBox>()
            .First(c => c.Name == "ModeList");
        list.SelectedItem = list.Items.OfType<ListBoxItem>().First(i => i.Tag as string == tag);
        Dispatcher.UIThread.RunJobs();
    }

    private static WriteableBitmap Render(MainWindow window, string name)
    {
        var bitmap = window.CaptureRenderedFrame();
        Assert.NotNull(bitmap);
        Write(bitmap!, name);
        return bitmap!;
    }

    private static void Write(WriteableBitmap bitmap, string name)
    {
        string? dir = SnapshotDir;
        if (string.IsNullOrEmpty(dir))
        {
            return;
        }

        Directory.CreateDirectory(dir!);
        bitmap.Save(Path.Combine(dir!, name + ".png"));
    }

    /// <summary>Fraction of pixels that differ noticeably from the window background.</summary>
    private static double InkedFraction(WriteableBitmap bitmap)
    {
        using var locked = bitmap.Lock();
        var framebuffer = locked;
        int stride = framebuffer.RowBytes;
        byte[] pixels = new byte[stride * bitmap.PixelSize.Height];
        Marshal.Copy(framebuffer.Address, pixels, 0, pixels.Length);

        // Top-left pixel is the app background; anything far from it is UI.
        byte r0 = pixels[0];
        byte g0 = pixels[1];
        byte b0 = pixels[2];

        int inked = 0;
        int total = 0;
        for (int y = 0; y < bitmap.PixelSize.Height; y++)
        {
            int row = y * stride;
            for (int x = 0; x < stride; x += 4)
            {
                int delta = Math.Abs(pixels[row + x] - r0)
                    + Math.Abs(pixels[row + x + 1] - g0)
                    + Math.Abs(pixels[row + x + 2] - b0);
                if (delta > 6)
                {
                    inked++;
                }
                total++;
            }
        }
        return total == 0 ? 0 : inked / (double)total;
    }

    [AvaloniaFact]
    public void StandardKeypad_CoversTheLowerHalfOfTheWindow()
    {
        var window = Show(960, 680);
        try
        {
            WriteableBitmap frame = Render(window, "standard");
            double inked = InkedFraction(frame);
            Assert.True(inked > 0.25,
                $"keypad pixels cover only {inked:P1} of the window — layout is collapsing again");
        }
        finally
        {
            window.Close();
        }
    }

    // Catches panels rendered with no DataContext (blank). Fill quality is
    // asserted geometrically in WindowLayoutTests, which is exact; this is the
    // coarse "did anything actually paint" net.
    [AvaloniaTheory]
    [InlineData("Standard", 0.30)]
    [InlineData("Scientific", 0.25)]
    [InlineData("Programmer", 0.20)]
    [InlineData("Date", 0.05)]
    [InlineData("Volume", 0.10)]
    [InlineData("Graphing", 0.05)]
    public void EachMode_RendersContent(string tag, double minimumInked)
    {
        var window = Show(960, 680);
        try
        {
            SelectMode(window, tag);
            WriteableBitmap frame = Render(window, tag.ToLowerInvariant());
            double inked = InkedFraction(frame);
            Assert.True(inked > minimumInked,
                $"{tag} renders nearly blank ({inked:P1} non-background pixels, expected > {minimumInked:P0})");
        }
        finally
        {
            window.Close();
        }
    }

    [AvaloniaFact]
    public void DarkTheme_RendersTheCalculatorPalette()
    {
        var previous = Application.Current!.RequestedThemeVariant;
        try
        {
            Application.Current.RequestedThemeVariant = ThemeVariant.Dark;
            var window = Show(960, 680);
            try
            {
                WriteableBitmap frame = Render(window, "standard-dark");
                double inked = InkedFraction(frame);
                Assert.True(inked > 0.30, $"dark theme renders nearly blank ({inked:P1})");
            }
            finally
            {
                window.Close();
            }
        }
        finally
        {
            Application.Current.RequestedThemeVariant = previous;
        }
    }

    [AvaloniaFact]
    public void NarrowWindow_StillRendersKeypad()
    {
        var window = Show(340, 520);
        try
        {
            WriteableBitmap frame = Render(window, "narrow-340");
            Assert.True(InkedFraction(frame) > 0.20, "narrow window renders nearly blank");
        }
        finally
        {
            window.Close();
        }
    }

    [AvaloniaFact]
    public void WideWindow_StillRendersKeypad()
    {
        var window = Show(1400, 950);
        try
        {
            WriteableBitmap frame = Render(window, "wide-1400");
            Assert.True(InkedFraction(frame) > 0.20, "wide window renders nearly blank");
        }
        finally
        {
            window.Close();
        }
    }
}