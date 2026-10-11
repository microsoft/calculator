// AUDIT — replay of the REFERENCE's own engine golden vectors against the port.
//
// Vectors are extracted verbatim from the unmodified reference
//   src/CalculatorUnitTests/CalculatorManagerTest.cpp @ d125246a
// by AUDIT/extract_golden.py into AUDIT/evidence/golden_engine_vectors.json,
// and replayed here exactly as the reference's TestDriver does:
//
//   Reset(); if (isScientific) SendCommand(ModeScientific);
//   send commands until NULL;
//   assert primary display, and expression unless it is "N/A".
//
// This is the highest-yield check in the audit: 158 vectors covering standard,
// scientific, programmer, parenthesis, error, memory, max-digits and
// order-of-operations behaviour, all with expectations authored by Microsoft.

using System.Text.Json;
using Calculator.Linux.Interop;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class ReferenceGoldenVectorTests
{
    private sealed record Vector(
        string Origin,
        int[] Commands,
        string ExpectedPrimary,
        string? ExpectedExpression,
        bool Cleanup,
        bool Scientific);

    private static readonly Lazy<IReadOnlyList<Vector>> s_vectors = new(Load);

    private static IReadOnlyList<Vector> Load()
    {
        string path = Path.Combine(FindRepoRoot(), "AUDIT", "evidence", "golden_engine_vectors.json");
        Assert.True(File.Exists(path), $"golden vectors not found: {path} (run AUDIT/extract_golden.py)");

        using var doc = JsonDocument.Parse(File.ReadAllText(path));
        var list = new List<Vector>();
        foreach (var v in doc.RootElement.GetProperty("vectors").EnumerateArray())
        {
            list.Add(new Vector(
                v.GetProperty("origin").GetString()!,
                v.GetProperty("commands").EnumerateArray().Select(c => c.GetInt32()).ToArray(),
                v.GetProperty("expectedPrimary").GetString()!,
                v.GetProperty("expectedExpression").ValueKind == System.Text.Json.JsonValueKind.Null
                    ? null
                    : v.GetProperty("expectedExpression").GetString(),
                v.GetProperty("cleanup").GetBoolean(),
                v.GetProperty("scientific").GetBoolean()));
        }
        return list;
    }

    private static string FindRepoRoot()
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
    public void ReferenceVector(int index)
    {
        Vector v = s_vectors.Value[index];
        using var engine = new CalcEngine();

        if (v.Cleanup)
        {
            engine.Reset();
        }

        if (v.Scientific)
        {
            engine.SetScientificMode();
        }

        // CommandNULL (0) is the reference's array terminator, NOT a command:
        // TestDriver::Test loops `while (*currentCommand != Command::CommandNULL)`.
        // Sending it clears the expression line.
        foreach (int command in v.Commands)
        {
            if (command == 0)
            {
                break;
            }
            engine.Send(command);
        }

        string primary = engine.PrimaryDisplay;
        Assert.True(primary == v.ExpectedPrimary,
            $"{v.Origin}\n  commands: [{string.Join(",", v.Commands)}]\n" +
            $"  expected primary : {v.ExpectedPrimary}\n  actual   primary : {primary}");

        if (v.ExpectedExpression is not null)
        {
            // The engine reports the expression line as a token list; the
            // reference's expected string is those tokens concatenated.
            string expression = string.Concat(engine.ExpressionTokens.Select(t => t.Token));
            Assert.True(expression == v.ExpectedExpression,
                $"{v.Origin}\n  commands: [{string.Join(",", v.Commands)}]\n" +
                $"  expected expression: {v.ExpectedExpression}\n  actual   expression: {expression}");
        }
    }
}