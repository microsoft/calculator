// AUDIT (independent QA) — UI completeness guard.
//
// Regression: the port's keypads omitted 32 of the reference's 59 Scientific
// buttons (Sec/Csc/Cot and every inverse and hyperbolic family, Floor, Ceil,
// x^3, rand, Euler, log_b, 2^x, y-th root) and 5 Programmer operations
// (NAND, NOR, LSH, rotate-through-carry left/right). The ViewModel and the
// native engine already computed all of them correctly — the functions were
// simply unreachable, which a ViewModel-only test suite cannot detect.
//
// This test walks the REAL window's visual tree and requires that every
// ButtonId the reference declares is bound to a button that exists on screen.

using System.Text.RegularExpressions;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Headless;
using Avalonia.Headless.XUnit;
using Avalonia.LogicalTree;
using Avalonia.Threading;
using Avalonia.VisualTree;
using Xunit;
using Xunit.Abstractions;

namespace Calculator.Linux.Tests.Audit;

public sealed class KeypadCompletenessTests
{
    private readonly ITestOutputHelper _o;

    // The reference's own XAML, which is vendored unmodified in this repo at
    // src/Calculator/Views/. It is the contract for what must be reachable.
    private static string ReferenceRoot = ReferenceRootProbe();

    public KeypadCompletenessTests(ITestOutputHelper o) => _o = o;

    private static HashSet<string> ReferenceButtonIds(string fileName)
    {
        string path = Path.Combine(ReferenceRoot, fileName);
        Assert.True(File.Exists(path), $"reference view not found: {path}");
        string text = File.ReadAllText(path);
        return Regex.Matches(text, @"ButtonId=""(\w+)""")
            .Select(m => m.Groups[1].Value)
            .ToHashSet(StringComparer.Ordinal);
    }

    private static string ReferenceRootProbe()
    {
        // Walk up from the test binary until we find src/Calculator/Views.
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir is not null)
        {
            string candidate = Path.Combine(dir.FullName, "src", "Calculator", "Views");
            if (Directory.Exists(candidate))
            {
                return candidate;
            }
            dir = dir.Parent;
        }
        throw new DirectoryNotFoundException("src/Calculator/Views not found above " + AppContext.BaseDirectory);
    }

    private static HashSet<string> WiredEnums(string mode)
    {
        var window = new MainWindow { Width = 1100, Height = 900 };
        window.Show();
        try
        {
            var list = window.GetLogicalDescendants().OfType<ListBox>().First(c => c.Name == "ModeList");
            list.SelectedItem = list.Items.OfType<ListBoxItem>().First(i => i.Tag as string == mode);
            Dispatcher.UIThread.RunJobs();

            // Buttons that are currently realized in the visual tree for this mode.
            var host = window.GetLogicalDescendants().OfType<Grid>().First(c => c.Name == "ContentHost");
            var panel = host.GetLogicalDescendants().OfType<Control>()
                .First(c => c.Name == (mode is "Scientific" or "Programmer" or "Standard" ? "CalculatorPanel" : mode + "Panel"));
            var wired = new HashSet<string>(StringComparer.Ordinal);
            foreach (var b in panel.GetVisualDescendants().OfType<Button>())
            {
                var m = Regex.Match(b.CommandParameter?.ToString() ?? string.Empty, @"(\w+)$");
                if (m.Success)
                {
                    wired.Add(m.Groups[1].Value);
                }
            }
            return wired;
        }
        finally
        {
            window.Close();
            Dispatcher.UIThread.RunJobs();
        }
    }

    [AvaloniaFact]
    public void ReferenceViewFiles_AreReachable()
    {
        _o.WriteLine("reference views: " + ReferenceRoot);
        Assert.True(File.Exists(Path.Combine(ReferenceRoot, "CalculatorScientificOperators.xaml")));
    }

    [AvaloniaFact]
    public void Scientific_ExposesEveryReferenceButton()
    {
        HashSet<string> reference = ReferenceButtonIds("CalculatorScientificOperators.xaml");
        HashSet<string> wired = WiredEnums("Scientific");

        // The reference models digits via <NumberPad>, which the port expresses
        // as individual digit buttons; those are legitimately not ButtonIds.
        var missing = reference.Where(id => !wired.Contains(id)).OrderBy(x => x).ToList();
        _o.WriteLine($"reference scientific ButtonIds={reference.Count} wired={wired.Count} missing={missing.Count}");
        _o.WriteLine("missing: " + string.Join(", ", missing));
        Assert.Empty(missing);
    }

    [AvaloniaFact]
    public void Programmer_ExposesEveryReferenceButton()
    {
        HashSet<string> reference = ReferenceButtonIds("CalculatorProgrammerRadixOperators.xaml");
        HashSet<string> wired = WiredEnums("Programmer");

        var missing = reference.Where(id => !wired.Contains(id)).OrderBy(x => x).ToList();
        _o.WriteLine($"reference programmer ButtonIds={reference.Count} wired={wired.Count} missing={missing.Count}");
        _o.WriteLine("missing: " + string.Join(", ", missing));
        Assert.Empty(missing);
    }

    [AvaloniaFact]
    public void Standard_ExposesEveryReferenceButton()
    {
        HashSet<string> reference = ReferenceButtonIds("CalculatorStandardOperators.xaml");
        HashSet<string> wired = WiredEnums("Standard");

        var missing = reference.Where(id => !wired.Contains(id)).OrderBy(x => x).ToList();
        _o.WriteLine($"reference standard ButtonIds={reference.Count} wired={wired.Count} missing={missing.Count}");
        _o.WriteLine("missing: " + string.Join(", ", missing));
        Assert.Empty(missing);
    }
}