// AUDIT — mode-navigation parity with the reference's CategoryManifest.
//
// Source of truth: src/Calculator.ViewModels/Common/NavCategory.cs @ d125246a,
// the CategoryManifest initializer list, which defines the ORDER of the
// navigation flyout and the VirtualKey Number1..5 / AccessKey "1".."5" bindings
// for the five calculator modes.
//
// Defects guarded:
//  AUD-S3-006  the port listed Graphing LAST (after all twelve converters);
//               the reference orders it THIRD, between Scientific and Programmer.
//  AUD-S3-007  the reference's Ctrl+Alt+1..5 mode shortcuts were absent, so no
//               mode could be reached from the keyboard.

using Avalonia.Controls;
using Avalonia.Headless;
using Avalonia.Headless.XUnit;
using Avalonia.Input;
using Avalonia.LogicalTree;
using Calculator.Linux.Common;
using Xunit;
using Xunit.Abstractions;

namespace Calculator.Linux.Tests.Audit;

public sealed class ModeNavigationParityTests : IDisposable
{
    private readonly ITestOutputHelper _o;
    private readonly MainWindow _window;

    public ModeNavigationParityTests(ITestOutputHelper o)
    {
        _o = o;
        _window = new MainWindow { Width = 1100, Height = 900 };
        _window.Show();
    }

    public void Dispose()
    {
        _window.Close();
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
    }

    private static IReadOnlyList<string> FlyoutOrder(MainWindow w)
    {
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
        var list = w.GetLogicalDescendants().OfType<ListBox>().First(c => c.Name == "ModeList");
        return list.Items.OfType<ListBoxItem>().Select(i => i.Tag as string ?? "").ToList();
    }

    /// <summary>
    /// The reference's CategoryManifest order:
    /// Calculator group - Standard, Scientific, Graphing, Programmer, Date;
    /// Converter group - Currency, Volume, Length, Weight, Temperature, Energy,
    /// Area, Speed, Time, Power, Data, Pressure, Angle.
    /// "About" is the port's addition (upstream has no About nav entry).
    /// </summary>
    [AvaloniaFact]
    public void FlyoutOrder_MatchesReferenceCategoryManifest()
    {
        string[] expected =
        {
            "Standard", "Scientific", "Graphing", "Programmer", "Date",
            "Currency", "Volume", "Length", "Weight", "Temperature", "Energy",
            "Area", "Speed", "Time", "Power", "Data", "Pressure", "Angle",
            "About",
        };

        Assert.Equal(expected, FlyoutOrder(_window));
    }

    [AvaloniaFact]
    public void Graphing_IsThird_NotLast()
    {
        var order = FlyoutOrder(_window);
        int graphing = order.ToList().IndexOf("Graphing");
        Assert.Equal(2, graphing);
        Assert.True(graphing < order.ToList().IndexOf("Programmer"),
            "reference orders Graphing before Programmer");
    }

    [AvaloniaTheory]
    [InlineData(1, "Standard")]
    [InlineData(2, "Scientific")]
    [InlineData(3, "Graphing")]
    [InlineData(4, "Programmer")]
    [InlineData(5, "Date")]
    public void CtrlAltDigit_SwitchesMode(int digit, string expectedMode)
    {
        Key key = Key.D0 + digit;
        _window.KeyPress(key, RawInputModifiers.Control | RawInputModifiers.Alt,
            PhysicalKey.None, string.Empty);
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();

        var selected = _window.GetLogicalDescendants().OfType<ListBox>()
            .First(c => c.Name == "ModeList").SelectedItem as ListBoxItem;
        Assert.Equal(expectedMode, selected?.Tag as string);
    }

    [AvaloniaTheory]
    [InlineData(1, "Standard")]
    [InlineData(2, "Scientific")]
    [InlineData(3, "Graphing")]
    [InlineData(4, "Programmer")]
    [InlineData(5, "Date")]
    public void AltDigit_SwitchesMode_LinuxMnemonic(int digit, string expectedMode)
    {
        // The Linux/desktop convention for a numbered access key is Alt+<digit>,
        // and the app has no Alt menu, so it cannot collide.
        _window.KeyPress(Key.D0 + digit, RawInputModifiers.Alt, PhysicalKey.None, string.Empty);
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();

        var selected = _window.GetLogicalDescendants().OfType<ListBox>()
            .First(c => c.Name == "ModeList").SelectedItem as ListBoxItem;
        Assert.Equal(expectedMode, selected?.Tag as string);
    }

    [AvaloniaTheory]
    [InlineData(6)]
    [InlineData(7)]
    [InlineData(9)]
    public void DigitsBeyondFive_AreNotModeShortcuts(int digit)
    {
        _window.KeyPress(Key.D0 + digit, RawInputModifiers.Control | RawInputModifiers.Alt,
            PhysicalKey.None, string.Empty);
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();

        var selected = _window.GetLogicalDescendants().OfType<ListBox>()
            .First(c => c.Name == "ModeList").SelectedItem as ListBoxItem;
        Assert.NotEqual(digit.ToString(), selected?.Tag as string);
    }
}