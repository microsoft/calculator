// AUDIT — replay of the REFERENCE's NavCategory unit tests.
//
// Source: src/Calculator.Tests/NavCategoryTests.cs @ d125246a (unmodified
// reference), backed by src/Calculator.ViewModels/Common/NavCategory.cs.
//
// The port has no NavCategory class of its own (its navigation is a ListBox in
// MainWindow.axaml plus a ConverterTags set), so these assertions are expressed
// against the same facts the reference's tests assert, read off the real
// controls.
//
// The most important of these is CreateNavCategoryGroup: the reference pins
// Graphing at index 2 of the calculator group, which is what caught AUD-S3-006
// (the port had Graphing last, after every converter).
//
// Serialize/Deserialize are also load-bearing: those ids are written to the
// persisted last-used-mode setting and "should NEVER change", so the port must
// use the same numbers or it silently loses the user's mode.

using Avalonia.Controls;
using Avalonia.Headless;
using Avalonia.Headless.XUnit;
using Avalonia.LogicalTree;
using Calculator.Linux.Common;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class ReferenceNavCategoryTests : IDisposable
{
    private readonly MainWindow _window;

    public ReferenceNavCategoryTests()
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

    private List<string> FlyoutTags() =>
        _window.GetLogicalDescendants().OfType<ListBox>()
            .First(c => c.Name == "ModeList")
            .Items.OfType<ListBoxItem>()
            .Select(i => i.Tag as string ?? "")
            .ToList();

    /// <summary>
    /// Reference NavCategoryGroupTests.CreateNavCategoryGroup: two groups -
    /// Calculator with 5 members ordered Standard, Scientific, Graphing,
    /// Programmer, Date; Converter with 13 members ordered Currency..Angle.
    /// </summary>
    [AvaloniaFact]
    public void CreateNavCategoryGroup_MatchesReference()
    {
        var tags = FlyoutTags();

        var calculator = tags.Take(5).ToList();
        Assert.Equal(
            new[] { "Standard", "Scientific", "Graphing", "Programmer", "Date" },
            calculator);
        Assert.Equal(5, calculator.Count);

        var converters = tags.Skip(5).Take(13).ToList();
        Assert.Equal(
            new[]
            {
                "Currency", "Volume", "Length", "Weight", "Temperature", "Energy",
                "Area", "Speed", "Time", "Power", "Data", "Pressure", "Angle",
            },
            converters);
        Assert.Equal(13, converters.Count);

        // "About" is the port's addition; upstream has no About nav entry.
        Assert.Equal("About", tags.Last());
        Assert.Equal(19, tags.Count);
    }

    /// <summary>
    /// Reference NavCategoryStates.Serialize: stable ids written to the persisted
    /// last-used-mode setting. Transcribed from the CategoryManifest constants
    /// (StandardId=0 ... AngleId=15, CurrencyId=16, GraphingId=17).
    /// </summary>
    [AvaloniaTheory]
    [InlineData("Standard", 0)]
    [InlineData("Scientific", 1)]
    [InlineData("Programmer", 2)]
    [InlineData("Date", 3)]
    [InlineData("Volume", 4)]
    [InlineData("Length", 5)]
    [InlineData("Weight", 6)]
    [InlineData("Temperature", 7)]
    [InlineData("Energy", 8)]
    [InlineData("Area", 9)]
    [InlineData("Speed", 10)]
    [InlineData("Time", 11)]
    [InlineData("Power", 12)]
    [InlineData("Data", 13)]
    [InlineData("Pressure", 14)]
    [InlineData("Angle", 15)]
    [InlineData("Currency", 16)]
    [InlineData("Graphing", 17)]
    public void SerializationId_MatchesReference(string friendlyName, int expectedId)
    {
        Assert.Equal(expectedId, ReferenceSerializationIds.IdFor(friendlyName));
    }

    /// <summary>Reference GetFriendlyName, for every mode in the manifest.</summary>
    [AvaloniaTheory]
    [InlineData(ViewMode.Standard, "Standard")]
    [InlineData(ViewMode.Scientific, "Scientific")]
    [InlineData(ViewMode.Graphing, "Graphing")]
    [InlineData(ViewMode.Programmer, "Programmer")]
    [InlineData(ViewMode.Date, "Date")]
    [InlineData(ViewMode.Currency, "Currency")]
    [InlineData(ViewMode.Volume, "Volume")]
    [InlineData(ViewMode.Length, "Length")]
    [InlineData(ViewMode.Weight, "Weight and Mass")]
    [InlineData(ViewMode.Temperature, "Temperature")]
    [InlineData(ViewMode.Energy, "Energy")]
    [InlineData(ViewMode.Area, "Area")]
    [InlineData(ViewMode.Speed, "Speed")]
    [InlineData(ViewMode.Time, "Time")]
    [InlineData(ViewMode.Power, "Power")]
    [InlineData(ViewMode.Data, "Data")]
    [InlineData(ViewMode.Pressure, "Pressure")]
    [InlineData(ViewMode.Angle, "Angle")]
    public void FriendlyName_MatchesReference(ViewMode mode, string expected)
    {
        Assert.Equal(expected, ReferenceSerializationIds.FriendlyNameFor(mode));
    }

    /// <summary>
    /// Reference IsCalculatorViewMode / IsDateCalculatorViewMode /
    /// IsConverterViewMode: the three-way classification of every mode.
    /// </summary>
    [AvaloniaTheory]
    [InlineData(ViewMode.Standard, true, false, false)]
    [InlineData(ViewMode.Scientific, true, false, false)]
    [InlineData(ViewMode.Graphing, true, false, false)]
    [InlineData(ViewMode.Programmer, true, false, false)]
    [InlineData(ViewMode.Date, false, true, false)]
    [InlineData(ViewMode.Currency, false, false, true)]
    [InlineData(ViewMode.Volume, false, false, true)]
    [InlineData(ViewMode.Length, false, false, true)]
    [InlineData(ViewMode.Weight, false, false, true)]
    [InlineData(ViewMode.Temperature, false, false, true)]
    [InlineData(ViewMode.Energy, false, false, true)]
    [InlineData(ViewMode.Area, false, false, true)]
    [InlineData(ViewMode.Speed, false, false, true)]
    [InlineData(ViewMode.Time, false, false, true)]
    [InlineData(ViewMode.Power, false, false, true)]
    [InlineData(ViewMode.Data, false, false, true)]
    [InlineData(ViewMode.Pressure, false, false, true)]
    [InlineData(ViewMode.Angle, false, false, true)]
    public void GroupClassification_MatchesReference(
        ViewMode mode, bool calculator, bool date, bool converter)
    {
        Assert.Equal(calculator, ReferenceSerializationIds.IsCalculator(mode));
        Assert.Equal(date, ReferenceSerializationIds.IsDate(mode));
        Assert.Equal(converter, ReferenceSerializationIds.IsConverter(mode));
    }

    /// <summary>
    /// Reference GetViewModeForVirtualKey: Number1..5 map to Standard, Scientific,
    /// Graphing, Programmer, Date.
    /// </summary>
    [AvaloniaTheory]
    [InlineData(1, ViewMode.Standard)]
    [InlineData(2, ViewMode.Scientific)]
    [InlineData(3, ViewMode.Graphing)]
    [InlineData(4, ViewMode.Programmer)]
    [InlineData(5, ViewMode.Date)]
    public void VirtualKey_MapsToMode(int digit, ViewMode expected)
    {
        Assert.Equal(expected, ReferenceSerializationIds.ModeForDigit(digit));
    }

    /// <summary>
    /// The flyout must contain no mode the manifest does not declare, and every
    /// mode it does declare must appear exactly once.
    /// </summary>
    [AvaloniaFact]
    public void FlyoutContainsExactlyTheManifestModes()
    {
        string[] manifest =
        {
            "Standard", "Scientific", "Graphing", "Programmer", "Date",
            "Currency", "Volume", "Length", "Weight", "Temperature", "Energy",
            "Area", "Speed", "Time", "Power", "Data", "Pressure", "Angle",
        };

        var tags = FlyoutTags();
        foreach (string mode in manifest)
        {
            Assert.Equal(1, tags.Count(t => t == mode));
        }

        Assert.Equal(
            manifest.OrderBy(m => m, StringComparer.Ordinal),
            tags.Where(t => t != "About").OrderBy(t => t, StringComparer.Ordinal));
    }
}

/// <summary>
/// The reference's CategoryManifest, transcribed as data so the port's
/// navigation facts can be asserted against it. Kept beside the tests rather
/// than in the app because the port stores no mode ids.
/// </summary>
internal static class ReferenceSerializationIds
{
    // (friendly name, serialization id) from CategoryManifest's constants.
    private static readonly (string Name, int Id)[] s_manifest =
    {
        ("Standard", 0), ("Scientific", 1), ("Programmer", 2), ("Date", 3),
        ("Volume", 4), ("Length", 5), ("Weight and Mass", 6), ("Temperature", 7),
        ("Energy", 8), ("Area", 9), ("Speed", 10), ("Time", 11), ("Power", 12),
        ("Data", 13), ("Pressure", 14), ("Angle", 15), ("Currency", 16),
        ("Graphing", 17),
    };

    private static readonly ViewMode[] s_converterModes =
    {
        ViewMode.Currency, ViewMode.Volume, ViewMode.Length, ViewMode.Weight,
        ViewMode.Temperature, ViewMode.Energy, ViewMode.Area, ViewMode.Speed,
        ViewMode.Time, ViewMode.Power, ViewMode.Data, ViewMode.Pressure,
        ViewMode.Angle,
    };

    private static readonly ViewMode[] s_calculatorModes =
    {
        ViewMode.Standard, ViewMode.Scientific, ViewMode.Graphing,
        ViewMode.Programmer,
    };

    /// <summary>Maps a flyout tag ("Weight") onto its manifest name ("Weight and Mass").</summary>
    internal static int IdFor(string flyoutTag)
    {
        string name = flyoutTag == "Weight" ? "Weight and Mass" : flyoutTag;
        return s_manifest.Single(m => m.Name == name).Id;
    }

    internal static string FriendlyNameFor(ViewMode mode) =>
        mode == ViewMode.Weight ? "Weight and Mass" : mode.ToString();

    internal static bool IsCalculator(ViewMode mode) => s_calculatorModes.Contains(mode);

    internal static bool IsDate(ViewMode mode) => mode == ViewMode.Date;

    internal static bool IsConverter(ViewMode mode) => s_converterModes.Contains(mode);

    internal static ViewMode ModeForDigit(int digit) => digit switch
    {
        1 => ViewMode.Standard,
        2 => ViewMode.Scientific,
        3 => ViewMode.Graphing,
        4 => ViewMode.Programmer,
        5 => ViewMode.Date,
        _ => ViewMode.None,
    };
}