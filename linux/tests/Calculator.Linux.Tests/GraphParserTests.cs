// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 8, slice 8a (RED): graphing expression parser + evaluator. Supports
// y-expressions over x with parameters (a..z except x/e), C-style operators,
// common math functions and pi/e constants. Trig functions honor Degrees or
// Radians mode (upstream graphing settings parity).
using Calculator.Linux.Services.Graphing;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class GraphParserTests
{
    private static double Eval(string expression, double x, AngleMode mode = AngleMode.Radians)
    {
        var parsed = GraphExpression.Parse(expression);
        Assert.True(parsed.IsValid, $"expected valid: {expression} ({parsed.Error})");
        return parsed.Evaluate(x, null, mode);
    }

    [Fact]
    public void Linear_Evaluates()
    {
        Assert.Equal(7.0, Eval("2*x+3", 2));
    }

    [Fact]
    public void Quadratic_Evaluates()
    {
        Assert.Equal(0.0, Eval("x^2-4", 2));
    }

    [Fact]
    public void Trig_RadiansByDefault()
    {
        Assert.Equal(1.0, Eval("sin(pi/2)", 0), precision: 9);
    }

    [Fact]
    public void Trig_DegreesMode()
    {
        Assert.Equal(0.5, Eval("sin(30)", 0, AngleMode.Degrees), precision: 9);
    }

    [Fact]
    public void Functions_NestAndCombine()
    {
        Assert.Equal(6.0, Eval("sqrt(9)+abs(0-5)-ln(e^2)", 0), precision: 9);
    }

    [Fact]
    public void Parameters_Substitute()
    {
        var parsed = GraphExpression.Parse("a*x+b");
        var parameters = new Dictionary<string, double> { ["a"] = 2, ["b"] = 1 };
        Assert.Equal(5.0, parsed.Evaluate(2, parameters, AngleMode.Radians));
    }

    [Fact]
    public void ImplicitMultiplication_Works()
    {
        Assert.Equal(6.0, Eval("2x", 3));
        Assert.Equal(6.0, Eval("2(x)", 3));
        Assert.Equal(6.0, Eval("(1+1)(1+2)", 0));
    }

    [Fact]
    public void YEquals_PrefixAccepted()
    {
        Assert.Equal(5.0, Eval("y = 2*x + 1", 2));
    }

    [Fact]
    public void DivisionByZero_YieldsNaN()
    {
        double result = Eval("1/(x-x)", 1);
        Assert.True(double.IsNaN(result));
    }

    [Fact]
    public void Errors_Reported()
    {
        Assert.False(GraphExpression.Parse("").IsValid);
        Assert.False(GraphExpression.Parse("2*(3").IsValid);
        Assert.False(GraphExpression.Parse("2+").IsValid);
        Assert.False(GraphExpression.Parse("foo(2)").IsValid);
        Assert.False(GraphExpression.Parse("2..5").IsValid);
    }
}
