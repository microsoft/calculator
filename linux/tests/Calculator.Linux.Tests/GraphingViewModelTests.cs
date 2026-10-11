// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 8, slice 8b: GraphingViewModel (series building, errors, features,
// settings rebuild, remove).
using Calculator.Linux.Services.Graphing;
using Calculator.Linux.ViewModels;
using LiveChartsCore.SkiaSharpView;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class GraphingViewModelTests : IDisposable
{
    private readonly GraphingViewModel _vm = new();

    public void Dispose()
    {
    }

    [Fact]
    public void AddLine_BuildsSeries()
    {
        var equation = _vm.AddEquation("2*x+1");
        Assert.Null(equation.Error);
        Assert.Single(_vm.Series);
        var series = Assert.IsType<LineSeries<LiveChartsCore.Defaults.ObservablePoint?>>(_vm.Series[0]);
        Assert.True(series.Values!.Count() > 100);
    }

    [Fact]
    public void InvalidExpression_SetsErrorAndSkipsSeries()
    {
        var equation = _vm.AddEquation("2*(");
        Assert.False(string.IsNullOrEmpty(equation.Error));
        Assert.Empty(_vm.Series);
    }

    [Fact]
    public void HiddenEquation_SkipsSeriesButKeepsFeatures()
    {
        var equation = _vm.AddEquation("x^2");
        equation.IsVisible = false;
        Assert.Empty(_vm.Series);
        Assert.NotNull(equation.Features);
    }

    [Fact]
    public void KeyFeatures_ListsRootsAndParity()
    {
        var equation = _vm.AddEquation("x^2-4");
        var groups = _vm.KeyFeatures(equation);
        Assert.Equal(5, groups.Count);
        var roots = groups.First(g => g.Title.Contains("X-Intercept"));
        Assert.Contains("2", string.Join(";", roots.Items));
        var parity = groups.First(g => g.Title.Contains("Parity"));
        Assert.Contains("even", string.Join(";", parity.Items), StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void SettingsChange_RebuildsAxes()
    {
        _vm.AddEquation("x");
        _vm.XMin = -5;
        _vm.XMax = 5;
        Assert.Equal(-5, _vm.XAxes[0].MinLimit);
        Assert.Equal(5, _vm.XAxes[0].MaxLimit);
    }

    [Fact]
    public void RemoveEquation_ClearsSeries()
    {
        var equation = _vm.AddEquation("x");
        Assert.Single(_vm.Series);
        _vm.RemoveEquation(equation);
        Assert.Empty(_vm.Series);
        Assert.Empty(_vm.Equations);
    }

    [Fact]
    public void ExpressionEdit_TriggersRebuild()
    {
        var equation = _vm.AddEquation("x");
        Assert.Single(_vm.Series);
        equation.Expression = "2*(";
        Assert.False(string.IsNullOrEmpty(equation.Error));
        Assert.Empty(_vm.Series);
    }

    [Fact]
    public void Parameters_SubstituteInPlot()
    {
        _vm.Parameters["a"] = 2;
        _vm.Parameters["b"] = 1;
        var equation = _vm.AddEquation("a*x+b");
        Assert.Null(equation.Error);
        var series = Assert.IsType<LineSeries<LiveChartsCore.Defaults.ObservablePoint?>>(_vm.Series[0]);
        var points = series.Values!.Where(p => p != null).Select(p => p!).ToList();
        var atTwo = points.Where(p => Math.Abs((p.X ?? double.NaN) - 2) < 0.05).OrderBy(p => Math.Abs((p.X ?? 0) - 2)).FirstOrDefault();
        Assert.NotNull(atTwo);
        Assert.True(Math.Abs((atTwo!.Y ?? double.NaN) - 5.0) < 0.06);
    }

    [Fact]
    public void DegreesMode_Sin90PeaksAtOne()
    {
        _vm.TrigMode = AngleMode.Degrees;
        _vm.XMin = 0;
        _vm.XMax = 100;
        var equation = _vm.AddEquation("sin(x)");
        Assert.Null(equation.Error);
        var series = Assert.IsType<LineSeries<LiveChartsCore.Defaults.ObservablePoint?>>(_vm.Series[0]);
        double maxY = series.Values!.Where(p => p != null).Max(p => p!.Y) ?? double.NaN;
        Assert.Equal(1.0, maxY, precision: 3);
    }
}
