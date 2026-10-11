// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Numeric key-feature analysis for graphed functions (Task 8). This replaces
// the proprietary symbolic solver with sampling math: exact for the tested
// shapes (roots via bisection, extrema via refine, parity via symmetry
// sampling). Asymptotes, periodicity and monotonicity need symbolic analysis
// and are intentionally out of scope (documented delta).
namespace Calculator.Linux.Services.Graphing;

public sealed record GraphPoint(double X, double Y);

public sealed class GraphFeatures
{
    public List<double> XIntercepts { get; } = new();
    public double? YIntercept { get; set; }
    public List<GraphPoint> Minima { get; } = new();
    public List<GraphPoint> Maxima { get; } = new();
    public string Parity { get; set; } = "Unknown";
    public double ObservedMinY { get; set; } = double.NaN;
    public double ObservedMaxY { get; set; } = double.NaN;
}

public static class GraphAnalysis
{
    private const int DefaultSamples = 2000;
    private const int MaxFeatures = 64;

    public static GraphFeatures Analyze(Func<double, double> f, double xMin, double xMax, int samples = DefaultSamples)
    {
        var features = new GraphFeatures();
        if (samples < 8 || xMax <= xMin)
        {
            return features;
        }

        // Sample once; split into continuous runs at non-finite values so
        // asymptotes never produce false roots or extrema.
        var runs = SampleRuns(f, xMin, xMax, samples);
        if (runs.Count == 0)
        {
            return features;
        }

        double globalMin = double.PositiveInfinity;
        double globalMax = double.NegativeInfinity;
        foreach (var run in runs)
        {
            FindRoots(f, run, features.XIntercepts);
            FindExtrema(f, run, features.Minima, features.Maxima);
            foreach (var point in run)
            {
                globalMin = Math.Min(globalMin, point.Y);
                globalMax = Math.Max(globalMax, point.Y);
            }
        }
        if (!double.IsInfinity(globalMin))
        {
            features.ObservedMinY = globalMin;
            features.ObservedMaxY = globalMax;
        }

        double atZero = f(0);
        if (xMin <= 0 && 0 <= xMax && !double.IsNaN(atZero) && !double.IsInfinity(atZero))
        {
            features.YIntercept = atZero;
        }

        features.Parity = ClassifyParity(f, Math.Max(Math.Abs(xMin), Math.Abs(xMax)));
        return features;
    }

    private static List<List<GraphPoint>> SampleRuns(Func<double, double> f, double xMin, double xMax, int samples)
    {
        var runs = new List<List<GraphPoint>>();
        var current = new List<GraphPoint>();
        for (int i = 0; i <= samples; i++)
        {
            double x = xMin + (xMax - xMin) * i / samples;
            double y;
            try
            {
                y = f(x);
            }
            catch
            {
                y = double.NaN;
            }
            if (double.IsNaN(y) || double.IsInfinity(y))
            {
                if (current.Count > 0)
                {
                    runs.Add(current);
                    current = new List<GraphPoint>();
                }
                continue;
            }
            current.Add(new GraphPoint(x, y));
        }
        if (current.Count > 0)
        {
            runs.Add(current);
        }
        return runs;
    }

    private static void FindRoots(Func<double, double> f, List<GraphPoint> run, List<double> roots)
    {
        for (int i = 1; i < run.Count && roots.Count < MaxFeatures; i++)
        {
            double x0 = run[i - 1].X;
            double y0 = run[i - 1].Y;
            double x1 = run[i].X;
            double y1 = run[i].Y;
            if (y0 == 0)
            {
                AddUnique(roots, x0);
            }
            if ((y0 < 0) == (y1 < 0) || y0 == 0 || y1 == 0)
            {
                if (y1 == 0)
                {
                    AddUnique(roots, x1);
                }
                continue;
            }
            // Sign change: bisect, then verify the candidate is a true zero
            // (bisection across a vertical asymptote converges with |f| huge).
            double lo = x0;
            double hi = x1;
            double flo = y0;
            for (int iter = 0; iter < 60; iter++)
            {
                double mid = 0.5 * (lo + hi);
                double fmid;
                try
                {
                    fmid = f(mid);
                }
                catch
                {
                    break;
                }
                if (double.IsNaN(fmid) || double.IsInfinity(fmid))
                {
                    break;
                }
                if (fmid == 0 || (hi - lo) < 1e-12)
                {
                    lo = mid;
                    flo = fmid;
                    break;
                }
                if ((flo < 0) == (fmid < 0))
                {
                    lo = mid;
                    flo = fmid;
                }
                else
                {
                    hi = mid;
                }
            }
            double tolerance = 1e-6 * Math.Max(1.0, Math.Max(Math.Abs(y0), Math.Abs(y1)));
            if (Math.Abs(flo) <= tolerance)
            {
                AddUnique(roots, lo);
            }
        }
    }

    private static void FindExtrema(
        Func<double, double> f,
        List<GraphPoint> run,
        List<GraphPoint> minima,
        List<GraphPoint> maxima)
    {
        for (int i = 1; i + 1 < run.Count && minima.Count + maxima.Count < MaxFeatures; i++)
        {
            double prev = run[i - 1].Y;
            double cur = run[i].Y;
            double next = run[i + 1].Y;
            bool isMin = cur < prev && cur <= next;
            bool isMax = cur > prev && cur >= next;
            if (!isMin && !isMax)
            {
                continue;
            }
            // Refine with a golden-section search on the bracketing interval.
            double a = run[i - 1].X;
            double b = run[i + 1].X;
            double best = Refine(f, a, b, isMin);
            double bestY;
            try
            {
                bestY = f(best);
            }
            catch
            {
                continue;
            }
            if (double.IsNaN(bestY) || double.IsInfinity(bestY))
            {
                continue;
            }
            (isMin ? minima : maxima).Add(new GraphPoint(best, bestY));
        }
    }

    private static double Refine(Func<double, double> f, double a, double b, bool wantMin)
    {
        const double phi = 1.618033988749895;
        double c = b - (b - a) / phi;
        double d = a + (b - a) / phi;
        for (int i = 0; i < 60; i++)
        {
            double fc;
            double fd;
            try
            {
                fc = f(c);
                fd = f(d);
            }
            catch
            {
                break;
            }
            if (double.IsNaN(fc) || double.IsNaN(fd) || double.IsInfinity(fc) || double.IsInfinity(fd))
            {
                break;
            }
            bool takeLeft = wantMin ? fc < fd : fc > fd;
            if (takeLeft)
            {
                b = d;
            }
            else
            {
                a = c;
            }
            c = b - (b - a) / phi;
            d = a + (b - a) / phi;
        }
        return 0.5 * (a + b);
    }

    private static string ClassifyParity(Func<double, double> f, double radius)
    {
        const int probes = 64;
        bool even = true;
        bool odd = true;
        double scale = 0;
        var samples = new List<(double Neg, double Pos)>();
        for (int i = 1; i <= probes; i++)
        {
            double x = radius * i / probes;
            double pos;
            double neg;
            try
            {
                pos = f(x);
                neg = f(-x);
            }
            catch
            {
                return "Unknown";
            }
            if (double.IsNaN(pos) || double.IsNaN(neg) || double.IsInfinity(pos) || double.IsInfinity(neg))
            {
                return "Unknown";
            }
            samples.Add((neg, pos));
            scale = Math.Max(scale, Math.Max(Math.Abs(pos), Math.Abs(neg)));
        }
        double tol = Math.Max(1e-9, scale * 1e-6);
        foreach (var (neg, pos) in samples)
        {
            if (Math.Abs(neg - pos) > tol)
            {
                even = false;
            }
            if (Math.Abs(neg + pos) > tol)
            {
                odd = false;
            }
        }
        if (even && odd)
        {
            return "Unknown"; // zero function or degenerate range
        }
        if (even)
        {
            return "Even";
        }
        if (odd)
        {
            return "Odd";
        }
        return "Neither";
    }

    private static void AddUnique(List<double> roots, double root)
    {
        foreach (double existing in roots)
        {
            if (Math.Abs(existing - root) < 1e-7)
            {
                return;
            }
        }
        roots.Add(root);
    }
}
