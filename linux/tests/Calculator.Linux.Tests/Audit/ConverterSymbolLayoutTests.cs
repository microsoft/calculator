// AUDIT — converter display layout parity.
//
// Defect guarded:
//  AUD-S3-008  Currency mode rendered the two currency symbols in a row below
//               the display instead of beside their amounts. The leading symbol
//               sat flush against the window edge with no padding, so its glyph
//               was clipped; the trailing symbol inherited the default left
//               alignment inside its `*` grid column and so appeared at the
//               start of the right third - visually the middle of the window -
//               instead of beside the right-aligned Value2.
//
// Reference: src/Calculator/Views/UnitConverter.xaml places CurrencySymbol1Block
// and CurrencySymbol2Block as Grid.Column siblings of Value1/Value2 inside
// Value1Container/Value2Container, with Padding="16,0,0,0" on the leading block.
// The CurrencySymbolRightState visual state moves both to Grid.Column="2" so
// they trail the amount when the second field is active.
//
// Found by driving the real app (AUDIT/ui_drive.sh) - no headless test had
// caught it because both symbols rendered, just in the wrong place.

using Avalonia.Controls;
using Avalonia.Headless;
using Avalonia.Headless.XUnit;
using Avalonia.LogicalTree;
using Avalonia.Media;
using Calculator.Linux.Common;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class ConverterSymbolLayoutTests : IDisposable
{
    private readonly MainWindow _window;

    public ConverterSymbolLayoutTests()
    {
        _window = new MainWindow { Width = 1100, Height = 900 };
        _window.Show();
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
    }

    public void Dispose()
    {
        _window.Close();
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
    }

    private void ShowCurrency()
    {
        var list = _window.GetLogicalDescendants().OfType<ListBox>().First(c => c.Name == "ModeList");
        list.SelectedItem = list.Items.OfType<ListBoxItem>()
            .First(i => i.Tag as string == "Currency");
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
    }

    /// <summary>
    /// The two currency-symbol TextBlocks, located STRUCTURALLY: they are the
    /// only TextBlocks on the converter's Row 2 (the symbol row), at Column 0
    /// and Column 2 of the three-column display grid.
    ///
    /// Deliberately not located by their text. The symbol characters come from
    /// the live rate feed, so a text-based lookup makes the test depend on the
    /// network - it passed when the feed answered and failed when it did not.
    /// </summary>
    private (TextBlock Leading, TextBlock Trailing) SymbolBlocks()
    {
        var converter = _window.GetLogicalDescendants().OfType<Grid>()
            .First(g => g.Name == "ConverterPanel");

        var display = converter.GetLogicalDescendants().OfType<Grid>()
            .First(g => g.RowDefinitions.Count == 3 && g.ColumnDefinitions.Count == 3);

        var row = display.GetLogicalDescendants().OfType<TextBlock>()
            .Where(t => Grid.GetRow(t) == 2)
            .OrderBy(t => Grid.GetColumn(t))
            .ToList();

        Assert.True(row.Count == 2, $"expected 2 symbol blocks on row 2, found {row.Count}");
        Assert.Equal(0, Grid.GetColumn(row[0]));
        Assert.Equal(2, Grid.GetColumn(row[1]));
        return (row[0], row[1]);
    }

    [AvaloniaFact]
    public void CurrencySymbolsSitBesideTheirAmounts()
    {
        ShowCurrency();
        var (leading, trailing) = SymbolBlocks();

        // Leading symbol: the reference pads it 16px so the glyph is not clipped
        // by the window edge (AUD-S3-008).
        Assert.Equal(16, leading.Padding.Left);
        Assert.Equal(Avalonia.Layout.HorizontalAlignment.Left, leading.HorizontalAlignment);

        // Trailing symbol: right-aligned so it trails Value2, which the port
        // renders with TextAlignment="Right".
        Assert.Equal(Avalonia.Layout.HorizontalAlignment.Right, trailing.HorizontalAlignment);
    }

    [AvaloniaFact]
    public void ConverterAmountsAlignToTheOuterEdges_WhereTheSymbolsAre()
    {
        ShowCurrency();

        // Alignment comes from TextAlignment on a stretched TextBlock: Value1
        // hugs the left edge, Value2 the right - which is what makes the symbol
        // alignment asserted above correct.
        TextBlock? value1 = _window.GetLogicalDescendants().OfType<TextBlock>()
            .FirstOrDefault(t => t.Text == "0" && t.FontSize >= 28
                              && t.TextAlignment == TextAlignment.Left);
        Assert.NotNull(value1);
        Assert.Equal(TextAlignment.Left, value1!.TextAlignment);
        Assert.Equal(Avalonia.Layout.HorizontalAlignment.Stretch, value1.HorizontalAlignment);

        TextBlock? value2 = _window.GetLogicalDescendants().OfType<TextBlock>()
            .FirstOrDefault(t => t.Text == "0" && t.FontSize >= 28
                              && t.TextAlignment == TextAlignment.Right);
        Assert.NotNull(value2);
        Assert.Equal(TextAlignment.Right, value2!.TextAlignment);
    }

    private void SelectConverter(string tag)
    {
        var list = _window.GetLogicalDescendants().OfType<ListBox>().First(c => c.Name == "ModeList");
        list.SelectedItem = list.Items.OfType<ListBoxItem>().First(i => i.Tag as string == tag);
        Avalonia.Threading.Dispatcher.UIThread.RunJobs();
    }

    [AvaloniaFact]
    public void NonCurrencyConverters_HideTheSymbolRow()
    {
        // Switching away from Currency must hide the symbol row, or Volume et al.
        // would keep showing a stray glyph (the Currency -> Volume transition is
        // exactly where stale state would show up).
        //
        // Again structural: CurrencySymbolVisibility is false for every
        // non-currency converter, so the blocks are hidden whatever text (if any)
        // they happen to hold.
        foreach (string tag in new[]
                 {
                     "Volume", "Length", "Weight", "Temperature", "Energy",
                     "Area", "Speed", "Time", "Power", "Data", "Pressure", "Angle",
                 })
        {
            ShowCurrency();
            SelectConverter(tag);

            var (leading, trailing) = SymbolBlocks();
            Assert.False(leading.IsVisible, $"{tag}: leading symbol still visible");
            Assert.False(trailing.IsVisible, $"{tag}: trailing symbol still visible");
        }
    }
}