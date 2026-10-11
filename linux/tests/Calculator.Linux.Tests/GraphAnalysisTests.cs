// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 8, slice 8b (RED): numeric key-feature analysis over sampled
// functions (roots, intercepts, extrema, parity). Symbolic analysis
// (asymptotes, periodicity, monotonicity) needs the proprietary solver and
// is a documented delta.
using Calculator.Linux.Services.Graphing;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class GraphAnalysisTests
{
    private static Func<double, double> Fn(string expression)
    {
        var parsed = GraphExpression.Parse(expression);
        Assert.True(parsed.IsValid);
        return x => parsed.Evaluate(x, null, AngleMode.Radians);
    }

    [Fact]
    public void Quadratic_RootsMinEven()
    {
        var features = GraphAnalysis.Analyze(Fn("x^2-4"), -10, 10);
        Assert.Equal(2, features.XIntercepts.Count);
        Assert.Contains(-2.0, features.XIntercepts, new DoubleComparer(1e-6));
        Assert.Contains(2.0, features.XIntercepts, new DoubleComparer(1e-6));
        Assert.Equal(-4.0, features.YIntercept);
        Assert.Single(features.Minima);
        Assert.Equal(0.0, features.Minima[0].X, precision: 3);
        Assert.Equal(-4.0, features.Minima[0].Y, precision: 3);
        Assert.Empty(features.Maxima);
        Assert.Equal("Even", features.Parity);
    }

    [Fact]
    public void Cubic_RootAtZeroOdd()
    {
        var features = GraphAnalysis.Analyze(Fn("x^3"), -10, 10);
        Assert.Single(features.XIntercepts);
        Assert.Equal(0.0, features.XIntercepts[0], precision: 6);
        Assert.Equal("Odd", features.Parity);
    }

    [Fact]
    public void Sine_RootsButNoFalseExtremaCount()
    {
        var features = GraphAnalysis.Analyze(Fn("sin(x)"), -10, 10);
        // Roots near -3pi..3pi within range: -9.42, -6.28, -3.14, 0, 3.14, 6.28, 9.42
        Assert.Equal(7, features.XIntercepts.Count);
        Assert.NotEmpty(features.Minima);
        Assert.NotEmpty(features.Maxima);
        Assert.Equal("Odd", features.Parity);
    }

    [Fact]
    public void Reciprocal_NoFalseRootAcrossAsymptote()
    {
        var features = GraphAnalysis.Analyze(Fn("1/x"), -10, 10);
        Assert.Empty(features.XIntercepts);
        Assert.Null(features.YIntercept);
    }

    [Fact]
    public void Line_NeitherParity()
    {
        var features = GraphAnalysis.Analyze(Fn("2*x+3"), -10, 10);
        Assert.Equal("Neither", features.Parity);
        Assert.Single(features.XIntercepts);
        Assert.Equal(-1.5, features.XIntercepts[0], precision: 6);
    }

    private sealed class DoubleComparer(double tolerance) : IEqualityComparer<double>
    {
        public bool Equals(double a, double b) => Math.Abs(a - b) <= tolerance;
        public int GetHashCode(double value) => 0;
    }
}
