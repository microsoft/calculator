// AUDIT — replay of the REFERENCE's VM-level golden sequences.
//
// Vectors are extracted by AUDIT/extract_vm_vectors.py from the unmodified
// reference's src/Calculator.Tests/StandardCalculatorViewModelTests.cs @ d125246a
// into AUDIT/evidence/golden_vm_vectors.json.
//
// Each vector replays the reference's own ValidateViewModelByCommands contract:
//
//   if doReset: ButtonPressed(Clear); ButtonPressed(ClearEntry); ClearMemoryCommand
//   for each step: ButtonPressed(command); assert DisplayValue == expected
//
// "{DEC}" is the reference's `_decimalSeparator` concatenation, resolved from the
// active localization here so the vectors stay locale-independent.
//
// These sit a layer ABOVE the 158 engine vectors: they exercise the port's own
// C# view-model (display formatting, expression tokens, error handling, memory),
// which the native engine replay does not cover.

using System.Text.Json;
using Calculator.Linux.Common;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class ReferenceVmGoldenVectorTests
{
    private sealed record Step(string Command, string Expected);
    private sealed record Vector(string Origin, bool DoReset, Step[] Steps);

    private static readonly Lazy<IReadOnlyList<Vector>> s_vectors = new(Load);

    private static IReadOnlyList<Vector> Load()
    {
        string path = Path.Combine(RepoRoot(), "AUDIT", "evidence", "golden_vm_vectors.json");
        Assert.True(File.Exists(path), $"{path} missing (run AUDIT/extract_vm_vectors.py)");
        using var doc = JsonDocument.Parse(File.ReadAllText(path));
        var list = new List<Vector>();
        foreach (var v in doc.RootElement.GetProperty("vectors").EnumerateArray())
        {
            list.Add(new Vector(
                v.GetProperty("origin").GetString()!,
                v.GetProperty("doReset").GetBoolean(),
                v.GetProperty("steps").EnumerateArray()
                    .Select(s => new Step(s.GetProperty("command").GetString()!,
                                          s.GetProperty("expected").GetString()!))
                    .ToArray()));
        }
        return list;
    }

    private static string RepoRoot()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir is not null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, "AUDIT")) &&
                Directory.Exists(Path.Combine(dir.FullName, "src", "CalcManager")))
            {
                return dir.FullName;
            }
            dir = dir.Parent;
        }
        throw new DirectoryNotFoundException("repo root not found");
    }

    public static TheoryData<int> Indexes()
    {
        var data = new TheoryData<int>();
        for (int i = 0; i < s_vectors.Value.Count; i++)
        {
            data.Add(i);
        }
        return data;
    }

    [Theory]
    [MemberData(nameof(Indexes))]
    public void ReferenceVmSequence(int index)
    {
        Vector v = s_vectors.Value[index];
        using var vm = new StandardCalculatorViewModel();
        vm.IsStandard = true;

        if (v.DoReset)
        {
            vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
            vm.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.ClearEntry);
            vm.ClearMemoryCommand.Execute(null);
        }

        string decimalSeparator = LinuxLocalization.Current.DecimalSeparatorString;

        foreach (Step step in v.Steps)
        {
            if (!Enum.TryParse<NumbersAndOperatorsEnum>(step.Command, out var command))
            {
                Assert.Fail($"{v.Origin}: unknown command '{step.Command}'");
            }

            vm.ButtonPressedCommand.Execute(command);

            // The reference stops at NumbersAndOperatorsEnum.None.
            if (command == NumbersAndOperatorsEnum.None)
            {
                break;
            }

            string expected = step.Expected.Replace("{DEC}", decimalSeparator);
            Assert.True(
                vm.DisplayValue == expected,
                $"{v.Origin}\n  after {step.Command}: expected display '{expected}', " +
                $"actual '{vm.DisplayValue}'");
        }
    }

    [Fact]
    public void EveryExtractedVectorHasAnAssertion()
    {
        // Guards against the extractor silently degrading to zero vectors.
        var doc = JsonDocument.Parse(File.ReadAllText(
            Path.Combine(RepoRoot(), "AUDIT", "evidence", "golden_vm_vectors.json")));
        Assert.True(doc.RootElement.GetProperty("count").GetInt32() >= 20,
            "extractor stopped finding sequences");
        Assert.True(doc.RootElement.GetProperty("assertions").GetInt32() >= 100);
    }
}