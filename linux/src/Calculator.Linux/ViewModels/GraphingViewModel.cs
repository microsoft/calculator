// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux graphing ViewModel (Task 8). Replaces the GraphControl-bound VMs
// (Equation/GraphingCalculator/GraphingSettings/Variable) with an OxyPlot
// rendering pipeline: equations parse locally (GraphExpression), render as
// LineSeries, and analyze numerically (GraphAnalysis). The proprietary
// symbolic solver (asymptotes, periodicity, monotonicity, inflection points)
// has no open-source equivalent and is a documented delta.
using System.Collections.ObjectModel;
using Avalonia.Media;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Calculator.Linux.Services;
using Calculator.Linux.Services.Graphing;
using LiveChartsCore;
using LiveChartsCore.Defaults;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using LiveChartsCore.SkiaSharpView.Painting.Effects;
using SkiaSharp;

namespace Calculator.Linux.ViewModels;

public enum GraphLineStyle
{
    Solid,
    Dashed,
    Dotted,
}

public sealed record GraphFeatureGroup(string Title, List<string> Items);

public sealed partial class GraphEquation : ObservableObject
{
    [ObservableProperty]
    private string _expression = string.Empty;

    [ObservableProperty]
    private Color _lineColor = Color.FromRgb(0, 120, 215);

    [ObservableProperty]
    private double _lineWidth = 2.0;

    [ObservableProperty]
    private GraphLineStyle _lineStyle = GraphLineStyle.Solid;

    [ObservableProperty]
    private bool _isVisible = true;

    private string? _error;
    private GraphFeatures? _features;

    public string? Error
    {
        get => _error;
        internal set
        {
            if (SetProperty(ref _error, value))
            {
                OnPropertyChanged(nameof(HasError));
            }
        }
    }

    public GraphFeatures? Features
    {
        get => _features;
        internal set => SetProperty(ref _features, value);
    }

    public bool HasError => !string.IsNullOrEmpty(_error);
}

public sealed partial class GraphingViewModel : ObservableObject
{
    public static readonly IReadOnlyList<Color> Palette = new[]
    {
        Color.FromRgb(0, 120, 215),    // blue
        Color.FromRgb(232, 17, 35),    // red
        Color.FromRgb(16, 124, 16),    // green
        Color.FromRgb(255, 140, 0),    // orange
        Color.FromRgb(128, 0, 128),    // purple
        Color.FromRgb(0, 172, 172),    // teal
    };

    public IReadOnlyList<Color> LinePalette => Palette;

    public IReadOnlyList<GraphLineStyle> LineStyles { get; } =
        new[] { GraphLineStyle.Solid, GraphLineStyle.Dashed, GraphLineStyle.Dotted };

    private const int SamplePoints = 800;

    [ObservableProperty]
    private double _xMin = -10;

    [ObservableProperty]
    private double _xMax = 10;

    [ObservableProperty]
    private double _yMin = -10;

    [ObservableProperty]
    private double _yMax = 10;

    [ObservableProperty]
    private bool _isGridVisible = true;

    [ObservableProperty]
    private AngleMode _trigMode = AngleMode.Radians;

    [ObservableProperty]
    private ISeries[] _series = Array.Empty<ISeries>();

    [ObservableProperty]
    private Axis[] _xAxes = Array.Empty<Axis>();

    [ObservableProperty]
    private Axis[] _yAxes = Array.Empty<Axis>();

    [ObservableProperty]
    private GraphEquation? _selectedEquation;

    [ObservableProperty]
    private List<GraphFeatureGroup> _selectedFeatures = new();

    public ObservableCollection<GraphEquation> Equations { get; } = new();

    public Dictionary<string, double> Parameters { get; } = new(StringComparer.OrdinalIgnoreCase);

    public GraphingViewModel()
    {
        Equations.CollectionChanged += OnEquationsChanged;
        Rebuild();
    }

    private void OnEquationsChanged(object? sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
    {
        if (e.NewItems != null)
        {
            foreach (GraphEquation equation in e.NewItems)
            {
                equation.PropertyChanged += OnEquationPropertyChanged;
            }
        }
        if (e.OldItems != null)
        {
            foreach (GraphEquation equation in e.OldItems)
            {
                equation.PropertyChanged -= OnEquationPropertyChanged;
            }
        }
        Rebuild();
    }

    private void OnEquationPropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
    {
        if (e.PropertyName is not (nameof(GraphEquation.Features) or nameof(GraphEquation.Error) or nameof(GraphEquation.HasError)))
        {
            Rebuild();
        }
    }

    [RelayCommand]
    private void OnAddEquation()
    {
        Equations.Add(new GraphEquation { LineColor = Palette[Equations.Count % Palette.Count] });
    }

    public GraphEquation AddEquation(string expression)
    {
        var equation = new GraphEquation
        {
            Expression = expression,
            LineColor = Palette[Equations.Count % Palette.Count],
        };
        Equations.Add(equation);
        Rebuild();
        return equation;
    }

    public void RemoveEquation(GraphEquation equation)
    {
        Equations.Remove(equation);
        Rebuild();
    }

    public void Rebuild()
    {
        var gridPaint = IsGridVisible
            ? new SolidColorPaint(new SKColor(128, 128, 128, 40)) { StrokeThickness = 1 }
            : null;
        XAxes = new[]
        {
            new Axis
            {
                MinLimit = XMin,
                MaxLimit = XMax,
                SeparatorsPaint = gridPaint,
            },
        };
        YAxes = new[]
        {
            new Axis
            {
                MinLimit = YMin,
                MaxLimit = YMax,
                SeparatorsPaint = gridPaint,
            },
        };

        var series = new List<ISeries>();
        foreach (GraphEquation equation in Equations)
        {
            RebuildEquation(equation, series);
        }
        Series = series.ToArray();

        if (SelectedEquation != null)
        {
            SelectedFeatures = KeyFeatures(SelectedEquation);
        }
    }

    private void RebuildEquation(GraphEquation equation, List<ISeries> series)
    {
        GraphExpression parsed = GraphExpression.Parse(equation.Expression);
        if (!parsed.IsValid)
        {
            equation.Error = parsed.Error;
            equation.Features = null;
            return;
        }

        equation.Error = null;
        var parameters = new Dictionary<string, double>(Parameters, StringComparer.OrdinalIgnoreCase);
        AngleMode mode = TrigMode;
        Func<double, double> f = x => parsed.Evaluate(x, parameters, mode);

        if (equation.IsVisible)
        {
            SKColor color = new(equation.LineColor.R, equation.LineColor.G, equation.LineColor.B, equation.LineColor.A);
            var stroke = new SolidColorPaint(color) { StrokeThickness = (float)equation.LineWidth };
            if (equation.LineStyle == GraphLineStyle.Dashed)
            {
                stroke.PathEffect = new DashEffect(new float[] { 10, 5 });
            }
            else if (equation.LineStyle == GraphLineStyle.Dotted)
            {
                stroke.PathEffect = new DashEffect(new float[] { 2, 5 });
            }

            var values = new List<ObservablePoint?>();
            for (int i = 0; i <= SamplePoints; i++)
            {
                double x = XMin + (XMax - XMin) * i / SamplePoints;
                double y;
                try
                {
                    y = f(x);
                }
                catch
                {
                    y = double.NaN;
                }
                // Null breaks the line (discontinuities); far off-viewport
                // points are skipped so neighboring segments stay sane.
                if (double.IsNaN(y) || double.IsInfinity(y)
                    || y < YMin - (YMax - YMin) || y > YMax + (YMax - YMin))
                {
                    values.Add(null);
                    continue;
                }
                values.Add(new ObservablePoint(x, y));
            }
            series.Add(new LineSeries<ObservablePoint?>
            {
                Values = values,
                Stroke = stroke,
                GeometrySize = 0,
                GeometryStroke = null,
                Fill = null,
                LineSmoothness = 0,
            });
        }

        equation.Features = GraphAnalysis.Analyze(f, XMin, XMax);
    }

    public List<GraphFeatureGroup> KeyFeatures(GraphEquation equation)
    {
        var groups = new List<GraphFeatureGroup>();
        if (equation?.Features == null)
        {
            return groups;
        }

        GraphFeatures features = equation.Features;
        string Format(double value) =>
            value.ToString("G6", System.Globalization.CultureInfo.InvariantCulture);

        groups.Add(new GraphFeatureGroup(
            AppResources.GetString("XIntercept"),
            features.XIntercepts.Count == 0
                ? new List<string> { AppResources.GetString("KGFXInterceptNone") }
                : features.XIntercepts.Select(Format).ToList()));
        groups.Add(new GraphFeatureGroup(
            AppResources.GetString("YIntercept"),
            features.YIntercept.HasValue
                ? new List<string> { Format(features.YIntercept.Value) }
                : new List<string> { AppResources.GetString("KGFYInterceptNone") }));
        groups.Add(new GraphFeatureGroup(
            AppResources.GetString("Minima"),
            features.Minima.Count == 0
                ? new List<string> { AppResources.GetString("KGFMinimaNone") }
                : features.Minima.Select(p => $"({Format(p.X)}, {Format(p.Y)})").ToList()));
        groups.Add(new GraphFeatureGroup(
            AppResources.GetString("Maxima"),
            features.Maxima.Count == 0
                ? new List<string> { AppResources.GetString("KGFMaximaNone") }
                : features.Maxima.Select(p => $"({Format(p.X)}, {Format(p.Y)})").ToList()));
        groups.Add(new GraphFeatureGroup(
            AppResources.GetString("Parity"),
            new List<string> { ParityName(features.Parity) }));
        return groups;
    }

    private static string ParityName(string parity) =>
        AppResources.GetString(parity switch
        {
            "Even" => "KGFParityEven",
            "Odd" => "KGFParityOdd",
            "Neither" => "KGFParityNeither",
            _ => "KGFParityUnknown",
        });

    partial void OnXMinChanged(double value) => Rebuild();
    partial void OnXMaxChanged(double value) => Rebuild();
    partial void OnYMinChanged(double value) => Rebuild();
    partial void OnYMaxChanged(double value) => Rebuild();
    partial void OnIsGridVisibleChanged(bool value) => Rebuild();
    partial void OnTrigModeChanged(AngleMode value) => Rebuild();

    partial void OnSelectedEquationChanged(GraphEquation? value)
    {
        SelectedFeatures = value == null ? new List<GraphFeatureGroup>() : KeyFeatures(value);
    }
}
