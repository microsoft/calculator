// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Regression guards for two defects found by visual review:
//
//  1. Mode routing: "Standard" is a ViewMode member, so routing nav tags
//     through Enum.TryParse<ViewMode> sent the calculator modes into the unit
//     converter — the window said "Calculator - Standard" while the Volume
//     keypad was on screen.
//  2. Layout fill: DockPanel only gives the *last declared* child the fill
//     slot. With four stacked mode panels as siblings, the visible one was
//     measured without the fill area, so the keypad collapsed to content-sized
//     buttons floating in empty space.

using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Threading;
using Avalonia.Headless;
using Avalonia.Input;
using Avalonia.Headless.XUnit;
using Avalonia.LogicalTree;
using Avalonia.VisualTree;
using Avalonia;
using Avalonia.Media;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class WindowLayoutTests : IDisposable
{
    private readonly MainWindow _window = OpenWindow();

    private static MainWindow OpenWindow(int width = 960, int height = 680)
    {
        var window = new MainWindow { Width = width, Height = height };
        window.Show();
        Dispatcher.UIThread.RunJobs();
        return window;
    }

    public void Dispose()
    {
        _window.Close();
        Dispatcher.UIThread.RunJobs();
    }

    // Logical, not visual: the mode list lives inside a Popup and is only in
    // the visual tree while the flyout is open.
    private static T? Descendant<T>(Control root, string name)
        where T : Control =>
        root.GetLogicalDescendants().OfType<T>().FirstOrDefault(c => c.Name == name);

    private Control Panel(string name) => Descendant<Control>(Descendant<Grid>(_window, "ContentHost")!, name)!;

    private static readonly string[] PanelNames =
        { "CalculatorPanel", "DatePanel", "ConverterPanel", "GraphingPanel" };

    private void SelectMode(string tag)
    {
        var list = Descendant<ListBox>(_window, "ModeList")!;
        list.SelectedItem = list.Items.OfType<ListBoxItem>().First(i => i.Tag as string == tag);
        Dispatcher.UIThread.RunJobs();
    }

    private List<string> VisiblePanels() =>
        PanelNames.Where(n => Panel(n).IsVisible).ToList();

    // --- 1. mode routing -------------------------------------------------------

    [AvaloniaTheory]
    [InlineData("Standard", "CalculatorPanel")]
    [InlineData("Scientific", "CalculatorPanel")]
    [InlineData("Programmer", "CalculatorPanel")]
    [InlineData("Date", "DatePanel")]
    [InlineData("Graphing", "GraphingPanel")]
    [InlineData("Volume", "ConverterPanel")]
    [InlineData("Length", "ConverterPanel")]
    [InlineData("Temperature", "ConverterPanel")]
    [InlineData("Currency", "ConverterPanel")]
    public void SelectingNavItem_ShowsExactlyOneMatchingPanel(string tag, string expectedPanel)
    {
        SelectMode(tag);
        Assert.Equal(new[] { expectedPanel }, VisiblePanels());
    }

    [AvaloniaFact]
    public void SelectingStandard_KeepsConverterHidden()
    {
        SelectMode("Volume");
        SelectMode("Standard");

        Assert.Equal(new[] { "CalculatorPanel" }, VisiblePanels());
        Assert.Equal("Standard", Descendant<TextBlock>(_window, "CategoryNameText")!.Text);
    }

    [AvaloniaFact]
    public void EveryNavTag_RoutesSomewhere()
    {
        foreach (var item in Descendant<ListBox>(_window, "ModeList")!.Items.OfType<ListBoxItem>())
        {
            if (item.Tag as string == "About")
            {
                continue;
            }

            SelectMode((string)item.Tag!);
            Assert.True(VisiblePanels().Count == 1,
                $"tag '{item.Tag}' left {VisiblePanels().Count} panels visible");
        }
    }

    [AvaloniaFact]
    public void EveryPanel_GetsItsViewModelBound()
    {
        // Regression: the code-behind looked panels up by element type
        // (Find<DockPanel>("ConverterPanel")). Restyling switched those to
        // Grid, the lookup silently returned null and the panels rendered with
        // no DataContext — blank date/converter/graphing views.
        Assert.NotNull(Descendant<Control>(_window, "CalculatorPanel")!.DataContext);
        Assert.NotNull(Descendant<Control>(_window, "DatePanel")!.DataContext);

        foreach (string tag in new[] { "Volume", "Currency", "Graphing" })
        {
            SelectMode(tag);
            string panel = tag == "Graphing" ? "GraphingPanel" : "ConverterPanel";
            Assert.NotNull(Panel(panel).DataContext);
        }
    }

    // --- 2. layout fill --------------------------------------------------------

    private Grid StandardKeypad()
    {
        Grid keypad = Panel("CalculatorPanel").GetVisualDescendants().OfType<Grid>()
            .First(g => g.IsVisible && g.RowDefinitions.Count == 6 && g.ColumnDefinitions.Count == 4);
        return keypad;
    }

    [AvaloniaFact]
    public void KeypadButtons_FillTheirGridCells()
    {
        Grid keypad = StandardKeypad();
        var buttons = keypad.Children.OfType<Button>().ToList();
        Assert.Equal(24, buttons.Count);

        foreach (var button in buttons)
        {
            // A stretched button covers its whole cell; the distortion was
            // content-sized buttons sitting inside full-size cells.
            Rect cell = CellBounds(keypad, button);
            Assert.True(cell.Width > 100 && cell.Height > 30,
                $"cell too small for '{button.Content}': {cell}");
            Assert.True(button.Bounds.Width >= cell.Width - Inset(button),
                $"button '{button.Content}' width {button.Bounds.Width} < cell {cell.Width}");
            Assert.True(button.Bounds.Height >= cell.Height - Inset(button),
                $"button '{button.Content}' height {button.Bounds.Height} < cell {cell.Height}");
        }
    }

    [AvaloniaFact]
    public void Keypad_SpansTheContentArea()
    {
        var panel = Panel("CalculatorPanel");
        Grid keypad = StandardKeypad();

        // The keypad must own the panel, not sit in a corner of it.
        Assert.True(keypad.Bounds.Width >= panel.Bounds.Width - 2,
            $"keypad {keypad.Bounds.Width} vs panel {panel.Bounds.Width}");
        Assert.True(keypad.Bounds.Height >= panel.Bounds.Height * 0.5,
            $"keypad {keypad.Bounds.Height} vs panel {panel.Bounds.Height}");
    }

    [AvaloniaFact]
    public void ConverterKeypad_FillsWhenConverterIsSelected()
    {
        SelectMode("Volume");

        var panel = Panel("ConverterPanel");
        Grid keypad = panel.GetVisualDescendants().OfType<Grid>()
            .First(g => g.IsVisible && g.RowDefinitions.Count == 5 && g.ColumnDefinitions.Count == 3);

        Assert.True(keypad.Bounds.Width >= panel.Bounds.Width - 2,
            $"converter keypad {keypad.Bounds.Width} vs panel {panel.Bounds.Width}");
        foreach (var button in keypad.Children.OfType<Button>())
        {
            Rect cell = CellBounds(keypad, button);
            Assert.True(button.Bounds.Width >= cell.Width - Inset(button),
                $"converter button '{button.Content}' width {button.Bounds.Width} < cell {cell.Width}");
        }
    }

    [AvaloniaFact]
    public void ScientificAndProgrammerKeypads_FillToo()
    {
        foreach (string tag in new[] { "Scientific", "Programmer" })
        {
            SelectMode(tag);
            var panel = Panel("CalculatorPanel");
            Grid keypad = panel.GetVisualDescendants().OfType<Grid>()
                .First(g => g.IsVisible && g.RowDefinitions.Count > 2 && g.RowDefinitions.All(r => r.Height.IsStar));
            Assert.True(keypad.Bounds.Width >= panel.Bounds.Width - 2,
                $"{tag}: keypad {keypad.Bounds.Width} vs panel {panel.Bounds.Width}");
            Assert.NotEmpty(keypad.Children.OfType<Button>());
        }
    }

    // Opening the popup is not asserted: Avalonia's headless OverlayLayer
    // mutates its collection mid-measure ("Collection was modified"), which is
    // a test-harness limitation rather than an app defect. The flyout content,
    // its closed-by-default state and mode switching are all covered.
    [AvaloniaFact]
    public void ModeFlyout_IsClosedByDefault_AndListsEveryMode()
    {
        var popup = Descendant<Popup>(_window, "NavPopup")!;
        Assert.False(popup.IsOpen);

        var list = Descendant<ListBox>(popup, "ModeList")!;
        var tags = list.Items.OfType<ListBoxItem>().Select(i => i.Tag as string).ToList();

        // Order and membership are the reference's CategoryManifest
        // (src/Calculator.ViewModels/Common/NavCategory.cs): the five calculator
        // modes first - Standard, Scientific, Graphing, Programmer, Date - then
        // the thirteen converters, then About (a port addition).
        Assert.Equal(
            new[]
            {
                "Standard", "Scientific", "Graphing", "Programmer", "Date",
                "Currency", "Volume", "Length", "Weight", "Temperature", "Energy",
                "Area", "Speed", "Time", "Power", "Data", "Pressure", "Angle",
                "About",
            },
            tags);
    }

    [AvaloniaFact]
    public void HistoryDock_HiddenByDefault_AndToggles()
    {
        var dock = Descendant<TabControl>(_window, "SidePanel")!;
        var toggle = Descendant<ToggleButton>(_window, "HistoryToggle")!;

        Assert.False(dock.IsVisible);

        var center = new Point(toggle.Bounds.Width / 2, toggle.Bounds.Height / 2);
        Point onScreen = toggle.TranslatePoint(center, _window) ?? center;
        _window.MouseDown(onScreen, MouseButton.Left);
        _window.MouseUp(onScreen, MouseButton.Left);
        Dispatcher.UIThread.RunJobs();

        Assert.True(toggle.IsChecked == true);
        Assert.True(dock.IsVisible);
    }

    /// <summary>Margin is excluded from Bounds but included in the cell slot.</summary>
    private static double Inset(Control control) =>
        control.Margin.Left + control.Margin.Right;

    private static Rect CellBounds(Grid grid, Control child)
    {
        int row = Grid.GetRow(child);
        int column = Grid.GetColumn(child);
        double x = 0;
        for (int i = 0; i < column; i++)
        {
            x += grid.ColumnDefinitions[i].ActualWidth;
        }
        double y = 0;
        for (int i = 0; i < row; i++)
        {
            y += grid.RowDefinitions[i].ActualHeight;
        }
        return new Rect(
            x,
            y,
            grid.ColumnDefinitions[column].ActualWidth,
            grid.RowDefinitions[row].ActualHeight);
    }
}