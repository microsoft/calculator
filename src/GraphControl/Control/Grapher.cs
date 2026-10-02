// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using CalculatorApp;
using GraphControl.DX;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.System;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Markup;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Shapes;

namespace GraphControl
{
    public delegate void TracingChangedEventHandler(bool newValue);
    public delegate void TracingValueChangedEventHandler(double xPointValue, double yPointValue);
    public delegate void PointerValueChangedEventHandler(Point value);

    public enum GraphViewChangedReason
    {
        Manipulation,
        Reset
    }

    [ContentProperty(Name = nameof(Equations))]
    public sealed class Grapher : Control, INotifyPropertyChanged
    {
        private const string s_templateKey_SwapChainPanel = "GraphSurface";
        private const string s_templateKey_GraphCanvas = "GraphCanvas";

        public event TracingValueChangedEventHandler TracingValueChangedEvent;
        public event PointerValueChangedEventHandler PointerValueChangedEvent;
        public event TracingChangedEventHandler TracingChangedEvent;
        public event EventHandler<GraphViewChangedReason> GraphViewChangedEvent;
        public event RoutedEventHandler GraphPlottedEvent;
        public event EventHandler<IDictionary<string, Variable>> VariablesUpdated;
        public event PropertyChangedEventHandler PropertyChanged;

        private RenderMain _renderMain;
        private Canvas _graphCanvas;
        private int _trigUnitMode;
        private double _xAxisMin = -10;
        private double _xAxisMax = 10;
        private double _yAxisMin = -10;
        private double _yAxisMax = 10;
        private bool _rangeUpdatedBySettings;
        private bool _resetUsingInitialDisplayRange;
        private double _initialDisplayRangeXMin = -10;
        private double _initialDisplayRangeXMax = 10;
        private double _initialDisplayRangeYMin = -10;
        private double _initialDisplayRangeYMax = 10;
        private bool _trigUnitsChanged;

        private bool _isPointerPanning;
        private Point _lastPointerPosition;
        private Point _currentPointerPosition;

        #region Dependency Properties

        public static readonly DependencyProperty ForceProportionalAxesProperty =
            DependencyProperty.Register(nameof(ForceProportionalAxes), typeof(bool), typeof(Grapher), new PropertyMetadata(true, OnForceProportionalAxesChanged));

        public static readonly DependencyProperty UseCommaDecimalSeperatorProperty =
            DependencyProperty.Register(nameof(UseCommaDecimalSeperator), typeof(bool), typeof(Grapher), new PropertyMetadata(false, OnUseCommaDecimalSeperatorChanged));

        public static readonly DependencyProperty VariablesProperty =
            DependencyProperty.Register(nameof(Variables), typeof(IDictionary<string, Variable>), typeof(Grapher), new PropertyMetadata(new Dictionary<string, Variable>()));

        public static readonly DependencyProperty EquationsProperty =
            DependencyProperty.Register(nameof(Equations), typeof(EquationCollection), typeof(Grapher), new PropertyMetadata(null, OnEquationsChanged));

        public static readonly DependencyProperty AxesColorProperty =
            DependencyProperty.Register(nameof(AxesColor), typeof(Color), typeof(Grapher), new PropertyMetadata(Colors.White, OnAxesColorChanged));

        public static readonly DependencyProperty GraphBackgroundProperty =
            DependencyProperty.Register(nameof(GraphBackground), typeof(Color), typeof(Grapher), new PropertyMetadata(Colors.Transparent, OnGraphBackgroundChanged));

        public static readonly DependencyProperty GridLinesColorProperty =
            DependencyProperty.Register(nameof(GridLinesColor), typeof(Color), typeof(Grapher), new PropertyMetadata(Color.FromArgb(80, 200, 200, 200), OnGridLinesColorChanged));

        public static readonly DependencyProperty LineWidthProperty =
            DependencyProperty.Register(nameof(LineWidth), typeof(double), typeof(Grapher), new PropertyMetadata(2.0, OnLineWidthChanged));

        public static readonly DependencyProperty IsKeepCurrentViewProperty =
            DependencyProperty.Register(nameof(IsKeepCurrentView), typeof(bool), typeof(Grapher), new PropertyMetadata(false));

        public bool ForceProportionalAxes
        {
            get => (bool)GetValue(ForceProportionalAxesProperty);
            set => SetValue(ForceProportionalAxesProperty, value);
        }

        public bool UseCommaDecimalSeperator
        {
            get => (bool)GetValue(UseCommaDecimalSeperatorProperty);
            set => SetValue(UseCommaDecimalSeperatorProperty, value);
        }

        public IDictionary<string, Variable> Variables
        {
            get => (IDictionary<string, Variable>)GetValue(VariablesProperty);
            set => SetValue(VariablesProperty, value);
        }

        public EquationCollection Equations
        {
            get => (EquationCollection)GetValue(EquationsProperty);
            set => SetValue(EquationsProperty, value);
        }

        public Color AxesColor
        {
            get => (Color)GetValue(AxesColorProperty);
            set => SetValue(AxesColorProperty, value);
        }

        public Color GraphBackground
        {
            get => (Color)GetValue(GraphBackgroundProperty);
            set => SetValue(GraphBackgroundProperty, value);
        }

        public Color GridLinesColor
        {
            get => (Color)GetValue(GridLinesColorProperty);
            set => SetValue(GridLinesColorProperty, value);
        }

        public double LineWidth
        {
            get => (double)GetValue(LineWidthProperty);
            set => SetValue(LineWidthProperty, value);
        }

        public bool IsKeepCurrentView
        {
            get => (bool)GetValue(IsKeepCurrentViewProperty);
            set => SetValue(IsKeepCurrentViewProperty, value);
        }

        #endregion

        public bool ActiveTracing
        {
            get => _renderMain != null && _renderMain.ActiveTracing;
            set
            {
                if (_renderMain != null && _renderMain.ActiveTracing != value)
                {
                    _renderMain.ActiveTracing = value;
                    UpdateTracingChanged();
                    RaisePropertyChanged();
                    RenderVisualGraph();
                }
            }
        }

        public Point TraceLocation => _renderMain != null ? _renderMain.TraceLocation : new Point(0, 0);

        public Point ActiveTraceCursorPosition
        {
            get => _renderMain != null ? _renderMain.ActiveTraceCursorPosition : new Point(0, 0);
            set
            {
                if (_renderMain != null && _renderMain.ActiveTraceCursorPosition != value)
                {
                    _renderMain.ActiveTraceCursorPosition = value;
                    UpdateTracingChanged();
                    RenderVisualGraph();
                }
            }
        }

        public int TrigUnitMode
        {
            get => _trigUnitMode;
            set
            {
                if (_trigUnitMode != value)
                {
                    _trigUnitMode = value;
                    _trigUnitsChanged = true;
                    PlotGraph(true);
                }
            }
        }

        public double XAxisMin
        {
            get => _xAxisMin;
            set
            {
                _xAxisMin = value;
                RenderVisualGraph();
            }
        }

        public double XAxisMax
        {
            get => _xAxisMax;
            set
            {
                _xAxisMax = value;
                RenderVisualGraph();
            }
        }

        public double YAxisMin
        {
            get => _yAxisMin;
            set
            {
                _yAxisMin = value;
                RenderVisualGraph();
            }
        }

        public double YAxisMax
        {
            get => _yAxisMax;
            set
            {
                _yAxisMax = value;
                RenderVisualGraph();
            }
        }

        public Grapher()
        {
            DefaultStyleKey = typeof(Grapher);
            Equations = new EquationCollection();
            Variables = new Dictionary<string, Variable>();

            ManipulationMode = ManipulationModes.TranslateX | ManipulationModes.TranslateY | ManipulationModes.Scale | ManipulationModes.All;

            Loaded += OnGrapherLoaded;
            SizeChanged += OnGrapherSizeChanged;
            PointerPressed += OnGrapherPointerPressed;
            PointerMoved += OnGrapherPointerMoved;
            PointerReleased += OnGrapherPointerReleased;
            PointerCanceled += OnGrapherPointerReleased;
            PointerWheelChanged += OnGrapherPointerWheelChanged;
            ManipulationDelta += OnGrapherManipulationDelta;

            var cw = CoreWindow.GetForCurrentThread();
            if (cw != null)
            {
                cw.KeyDown += OnCoreKeyDown;
                cw.KeyUp += OnCoreKeyUp;
            }
        }

        private void OnGrapherLoaded(object sender, RoutedEventArgs e)
        {
            RenderVisualGraph();
        }

        private void OnGrapherSizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (ForceProportionalAxes && e.NewSize.Width > 0 && e.NewSize.Height > 0)
            {
                double currentXRange = _xAxisMax - _xAxisMin;
                double targetYRange = currentXRange * (e.NewSize.Height / e.NewSize.Width);
                double yCenter = (_yAxisMax + _yAxisMin) / 2.0;
                _yAxisMin = yCenter - targetYRange / 2.0;
                _yAxisMax = yCenter + targetYRange / 2.0;
            }

            RenderVisualGraph();
        }

        protected override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
            _graphCanvas = GetTemplateChild(s_templateKey_GraphCanvas) as Canvas;
            var swapChainPanel = GetTemplateChild(s_templateKey_SwapChainPanel) as SwapChainPanel;
            if (swapChainPanel != null)
            {
                _renderMain = new RenderMain(swapChainPanel);
                _renderMain.BackgroundColor = GraphBackground;
            }

            RenderVisualGraph();
            _ = TryUpdateGraph(false);
        }

        private void OnGrapherPointerPressed(object sender, PointerRoutedEventArgs e)
        {
            var point = e.GetCurrentPoint(this);
            if (point.Properties.IsLeftButtonPressed)
            {
                _isPointerPanning = true;
                _lastPointerPosition = point.Position;
                _currentPointerPosition = point.Position;
                CapturePointer(e.Pointer);
            }
        }

        private void OnGrapherPointerMoved(object sender, PointerRoutedEventArgs e)
        {
            var point = e.GetCurrentPoint(this);
            _currentPointerPosition = point.Position;

            if (_isPointerPanning && ActualWidth > 0 && ActualHeight > 0)
            {
                double dxPixels = point.Position.X - _lastPointerPosition.X;
                double dyPixels = point.Position.Y - _lastPointerPosition.Y;

                double xUnitsPerPixel = (_xAxisMax - _xAxisMin) / ActualWidth;
                double yUnitsPerPixel = (_yAxisMax - _yAxisMin) / ActualHeight;

                _xAxisMin -= dxPixels * xUnitsPerPixel;
                _xAxisMax -= dxPixels * xUnitsPerPixel;
                _yAxisMin += dyPixels * yUnitsPerPixel;
                _yAxisMax += dyPixels * yUnitsPerPixel;

                _lastPointerPosition = point.Position;
                RenderVisualGraph();
                GraphViewChangedEvent?.Invoke(this, GraphViewChangedReason.Manipulation);
            }
            else
            {
                if (ActiveTracing)
                {
                    ActiveTraceCursorPosition = point.Position;
                }
            }
        }

        private void OnGrapherPointerReleased(object sender, PointerRoutedEventArgs e)
        {
            if (_isPointerPanning)
            {
                _isPointerPanning = false;
                ReleasePointerCapture(e.Pointer);
            }
        }

        private void OnGrapherPointerWheelChanged(object sender, PointerRoutedEventArgs e)
        {
            var point = e.GetCurrentPoint(this);
            int delta = point.Properties.MouseWheelDelta;
            if (delta == 0) return;

            double scale = delta > 0 ? 0.85 : 1.15;
            ZoomFromPosition(point.Position, scale);
            e.Handled = true;
        }

        private void OnGrapherManipulationDelta(object sender, ManipulationDeltaRoutedEventArgs e)
        {
            if (e.Delta.Scale != 1.0f && e.Delta.Scale > 0)
            {
                ZoomFromPosition(e.Position, 1.0 / e.Delta.Scale);
            }
            else if (ActualWidth > 0 && ActualHeight > 0)
            {
                double dxPixels = e.Delta.Translation.X;
                double dyPixels = e.Delta.Translation.Y;

                double xUnitsPerPixel = (_xAxisMax - _xAxisMin) / ActualWidth;
                double yUnitsPerPixel = (_yAxisMax - _yAxisMin) / ActualHeight;

                _xAxisMin -= dxPixels * xUnitsPerPixel;
                _xAxisMax -= dxPixels * xUnitsPerPixel;
                _yAxisMin += dyPixels * yUnitsPerPixel;
                _yAxisMax += dyPixels * yUnitsPerPixel;

                RenderVisualGraph();
                GraphViewChangedEvent?.Invoke(this, GraphViewChangedReason.Manipulation);
            }
        }

        public void ZoomFromPosition(Point position, double scale)
        {
            if (ActualWidth <= 0 || ActualHeight <= 0) return;

            double mathX = _xAxisMin + (position.X / ActualWidth) * (_xAxisMax - _xAxisMin);
            double mathY = _yAxisMax - (position.Y / ActualHeight) * (_yAxisMax - _yAxisMin);

            ScaleRange(mathX, mathY, scale);
        }

        public void ZoomFromCenter(double scale)
        {
            double centerX = (_xAxisMax + _xAxisMin) / 2.0;
            double centerY = (_yAxisMax + _yAxisMin) / 2.0;
            ScaleRange(centerX, centerY, scale);
        }

        private void ScaleRange(double centerX, double centerY, double scale)
        {
            double xRange = (_xAxisMax - _xAxisMin) * scale;
            double yRange = (_yAxisMax - _yAxisMin) * scale;

            _xAxisMin = centerX - xRange / 2.0;
            _xAxisMax = centerX + xRange / 2.0;
            _yAxisMin = centerY - yRange / 2.0;
            _yAxisMax = centerY + yRange / 2.0;

            RenderVisualGraph();
            GraphViewChangedEvent?.Invoke(this, GraphViewChangedReason.Manipulation);
        }

        public void ResetGrid()
        {
            if (_resetUsingInitialDisplayRange)
            {
                _xAxisMin = _initialDisplayRangeXMin;
                _xAxisMax = _initialDisplayRangeXMax;
                _yAxisMin = _initialDisplayRangeYMin;
                _yAxisMax = _initialDisplayRangeYMax;
                _resetUsingInitialDisplayRange = false;
            }
            else if (_rangeUpdatedBySettings)
            {
                IsKeepCurrentView = false;
                _ = TryPlotGraph(false, false);
                _rangeUpdatedBySettings = false;
                GraphViewChangedEvent?.Invoke(this, GraphViewChangedReason.Reset);
                return;
            }
            else
            {
                _xAxisMin = -10;
                _xAxisMax = 10;
                _yAxisMin = -10;
                _yAxisMax = 10;
            }

            RenderVisualGraph();
            GraphViewChangedEvent?.Invoke(this, GraphViewChangedReason.Reset);
        }

        public void SetVariable(string variableName, double newValue)
        {
            if (!Variables.ContainsKey(variableName))
            {
                Variables[variableName] = new Variable(newValue);
            }
            else
            {
                Variables[variableName].Value = newValue;
            }

            RenderVisualGraph();
        }

        public string ConvertToLinear(string mmlString)
        {
            return MathExpressionEvaluator.ExtractExpressionFromMathML(mmlString);
        }

        public string FormatMathML(string mmlString)
        {
            return mmlString;
        }

        public void PlotGraph(bool keepCurrentView)
        {
            _ = TryPlotGraph(keepCurrentView, false);
        }

        public KeyGraphFeaturesInfo AnalyzeEquation(Equation equation)
        {
            if (equation == null || string.IsNullOrWhiteSpace(equation.Expression))
            {
                return KeyGraphFeaturesInfo.Create(AnalysisErrorType.AnalysisCouldNotBePerformed);
            }

            var varDict = new Dictionary<string, double>();
            if (Variables != null)
            {
                foreach (var kv in Variables)
                {
                    varDict[kv.Key] = kv.Value.Value;
                }
            }

            return MathExpressionEvaluator.AnalyzeFunction(equation.Expression, varDict);
        }

        public void GetDisplayRanges(out double xMin, out double xMax, out double yMin, out double yMax)
        {
            xMin = _xAxisMin;
            xMax = _xAxisMax;
            yMin = _yAxisMin;
            yMax = _yAxisMax;
        }

        public void SetDisplayRanges(double xMin, double xMax, double yMin, double yMax)
        {
            _xAxisMin = xMin;
            _xAxisMax = xMax;
            _yAxisMin = yMin;
            _yAxisMax = yMax;
            _rangeUpdatedBySettings = true;
            RenderVisualGraph();
            GraphViewChangedEvent?.Invoke(this, GraphViewChangedReason.Manipulation);
        }

        public RandomAccessStreamReference GetGraphBitmapStream()
        {
            return null;
        }

        private async Task TryPlotGraph(bool keepCurrentView, bool shouldRetry)
        {
            if (await TryUpdateGraph(keepCurrentView))
            {
                SetEquationsAsValid();
            }
            else
            {
                SetEquationErrors();
                if (shouldRetry)
                {
                    await TryUpdateGraph(keepCurrentView);
                }
            }

            int valid = 0;
            int invalid = 0;
            if (Equations != null)
            {
                foreach (var eq in Equations)
                {
                    if (eq.HasGraphError) invalid++;
                    if (eq.IsValidated) valid++;
                }
            }

            if (!_trigUnitsChanged)
            {
                TraceLogger.GetInstance().LogEquationCountChanged(valid, invalid);
            }

            _trigUnitsChanged = false;
            GraphPlottedEvent?.Invoke(this, new RoutedEventArgs());
        }

        private Task<bool> TryUpdateGraph(bool keepCurrentView)
        {
            RenderVisualGraph();
            UpdateVariables();
            return Task.FromResult(true);
        }

        private void SetEquationsAsValid()
        {
            foreach (var eq in GetGraphableEquations())
            {
                eq.IsValidated = true;
            }
        }

        private void SetEquationErrors()
        {
            foreach (var eq in GetGraphableEquations())
            {
                if (!eq.IsValidated)
                {
                    eq.GraphErrorType = ErrorType.Evaluation;
                    eq.GraphErrorCode = (int)EvaluationErrorCode.GE_GeneralError;
                    eq.HasGraphError = true;
                }
            }
        }

        private List<Equation> GetGraphableEquations()
        {
            var result = new List<Equation>();
            if (Equations != null)
            {
                foreach (var eq in Equations)
                {
                    if (eq.IsGraphableEquation())
                    {
                        result.Add(eq);
                    }
                }
            }
            return result;
        }

        private void UpdateVariables()
        {
            VariablesUpdated?.Invoke(this, Variables);
        }

        private void UpdateTracingChanged()
        {
            TracingChangedEvent?.Invoke(ActiveTracing);
            if (ActiveTracing)
            {
                TracingValueChangedEvent?.Invoke(TraceLocation.X, TraceLocation.Y);
                PointerValueChangedEvent?.Invoke(ActiveTraceCursorPosition);
            }
        }

        #region Visual Graph Drawing (Axes, Grid, Labels, and Equation Curves)

        public void RenderVisualGraph()
        {
            if (_graphCanvas == null) return;

            double width = ActualWidth > 0 ? ActualWidth : _graphCanvas.ActualWidth;
            double height = ActualHeight > 0 ? ActualHeight : _graphCanvas.ActualHeight;

            if (width <= 0 || height <= 0) return;

            _graphCanvas.Children.Clear();
            _graphCanvas.Clip = new RectangleGeometry { Rect = new Rect(0, 0, width, height) };

            // Determine Dark Mode / Light Mode for crisp high contrast text and axes
            bool isDark = (GraphBackground.R * 0.299 + GraphBackground.G * 0.587 + GraphBackground.B * 0.114) < 128;
            if (GraphBackground == Colors.Transparent)
            {
                // Default to dark mode if transparent or not explicitly light
                isDark = true;
            }

            Color axisCol = isDark ? Color.FromArgb(255, 240, 240, 240) : Color.FromArgb(255, 40, 40, 40);
            Color gridCol = isDark ? Color.FromArgb(70, 255, 255, 255) : Color.FromArgb(50, 0, 0, 0);
            Color textCol = isDark ? Color.FromArgb(255, 255, 255, 255) : Color.FromArgb(255, 20, 20, 20);
            Color badgeBgCol = isDark ? Color.FromArgb(180, 28, 28, 28) : Color.FromArgb(180, 250, 250, 250);

            var axisBrush = new SolidColorBrush(axisCol);
            var gridBrush = new SolidColorBrush(gridCol);
            var textBrush = new SolidColorBrush(textCol);
            var badgeBgBrush = new SolidColorBrush(badgeBgCol);

            double xRange = _xAxisMax - _xAxisMin;
            double yRange = _yAxisMax - _yAxisMin;

            if (xRange <= 0) xRange = 1;
            if (yRange <= 0) yRange = 1;

            double gridStepX = CalculateGridStep(xRange, width);
            double gridStepY = CalculateGridStep(yRange, height);

            // Screen conversion helpers
            double ToScreenX(double mathX) => ((mathX - _xAxisMin) / xRange) * width;
            double ToScreenY(double mathY) => (1.0 - (mathY - _yAxisMin) / yRange) * height;

            double originScreenX = ToScreenX(0);
            double originScreenY = ToScreenY(0);

            // 1. Draw Vertical Grid Lines & X Numbers
            double startGridX = Math.Floor(_xAxisMin / gridStepX) * gridStepX;
            for (double gx = startGridX; gx <= _xAxisMax + gridStepX * 0.5; gx += gridStepX)
            {
                double sx = ToScreenX(gx);
                if (sx < -2 || sx > width + 2) continue;

                var vLine = new Line
                {
                    X1 = sx,
                    Y1 = 0,
                    X2 = sx,
                    Y2 = height,
                    Stroke = gridBrush,
                    StrokeThickness = 1
                };
                _graphCanvas.Children.Add(vLine);

                // Add number label
                if (Math.Abs(gx) > gridStepX * 0.1)
                {
                    var tb = new TextBlock
                    {
                        Text = FormatTickNumber(gx),
                        FontSize = 12,
                        FontWeight = Windows.UI.Text.FontWeights.SemiBold,
                        Foreground = textBrush
                    };

                    var badge = new Border
                    {
                        Background = badgeBgBrush,
                        CornerRadius = new CornerRadius(3),
                        Padding = new Thickness(4, 1, 4, 1),
                        Child = tb
                    };
                    badge.Measure(new Size(200, 100));

                    double labelY = Math.Clamp(originScreenY + 4, 4, height - badge.DesiredSize.Height - 4);
                    Canvas.SetLeft(badge, sx - (badge.DesiredSize.Width / 2));
                    Canvas.SetTop(badge, labelY);
                    _graphCanvas.Children.Add(badge);
                }
            }

            // 2. Draw Horizontal Grid Lines & Y Numbers
            double startGridY = Math.Floor(_yAxisMin / gridStepY) * gridStepY;
            for (double gy = startGridY; gy <= _yAxisMax + gridStepY * 0.5; gy += gridStepY)
            {
                double sy = ToScreenY(gy);
                if (sy < -2 || sy > height + 2) continue;

                var hLine = new Line
                {
                    X1 = 0,
                    Y1 = sy,
                    X2 = width,
                    Y2 = sy,
                    Stroke = gridBrush,
                    StrokeThickness = 1
                };
                _graphCanvas.Children.Add(hLine);

                // Add number label
                if (Math.Abs(gy) > gridStepY * 0.1)
                {
                    var tb = new TextBlock
                    {
                        Text = FormatTickNumber(gy),
                        FontSize = 12,
                        FontWeight = Windows.UI.Text.FontWeights.SemiBold,
                        Foreground = textBrush
                    };

                    var badge = new Border
                    {
                        Background = badgeBgBrush,
                        CornerRadius = new CornerRadius(3),
                        Padding = new Thickness(4, 1, 4, 1),
                        Child = tb
                    };
                    badge.Measure(new Size(200, 100));

                    double labelX = Math.Clamp(originScreenX + 6, 6, width - badge.DesiredSize.Width - 6);
                    Canvas.SetLeft(badge, labelX);
                    Canvas.SetTop(badge, sy - (badge.DesiredSize.Height / 2));
                    _graphCanvas.Children.Add(badge);
                }
            }

            // 3. Draw X Axis and Y Axis
            if (originScreenY >= 0 && originScreenY <= height)
            {
                var xAxis = new Line
                {
                    X1 = 0,
                    Y1 = originScreenY,
                    X2 = width,
                    Y2 = originScreenY,
                    Stroke = axisBrush,
                    StrokeThickness = 2.0
                };
                _graphCanvas.Children.Add(xAxis);
            }

            if (originScreenX >= 0 && originScreenX <= width)
            {
                var yAxis = new Line
                {
                    X1 = originScreenX,
                    Y1 = 0,
                    X2 = originScreenX,
                    Y2 = height,
                    Stroke = axisBrush,
                    StrokeThickness = 2.0
                };
                _graphCanvas.Children.Add(yAxis);
            }

            // 4. Draw Equations
            var varDict = new Dictionary<string, double>();
            if (Variables != null)
            {
                foreach (var kv in Variables)
                {
                    varDict[kv.Key] = kv.Value.Value;
                }
            }

            if (Equations != null)
            {
                int sampleCount = Math.Max(200, (int)width);
                double dx = xRange / sampleCount;

                foreach (var eq in Equations)
                {
                    if (!eq.IsLineEnabled || string.IsNullOrWhiteSpace(eq.Expression))
                    {
                        continue;
                    }

                    var evaluator = MathExpressionEvaluator.Compile(eq.Expression, varDict);
                    if (evaluator == null)
                    {
                        continue;
                    }

                    var polyline = new Polyline
                    {
                        Stroke = new SolidColorBrush(eq.LineColor),
                        StrokeThickness = Math.Max(2.0, LineWidth)
                    };

                    ApplyLineStyle(polyline, eq.EquationStyle);

                    double prevY = double.NaN;
                    for (int i = 0; i <= sampleCount; i++)
                    {
                        double mathX = _xAxisMin + (i * dx);
                        double mathY;
                        try
                        {
                            mathY = evaluator(mathX);
                        }
                        catch
                        {
                            mathY = double.NaN;
                        }

                        if (double.IsNaN(mathY) || double.IsInfinity(mathY))
                        {
                            if (polyline.Points.Count > 1)
                            {
                                _graphCanvas.Children.Add(polyline);
                                polyline = new Polyline
                                {
                                    Stroke = new SolidColorBrush(eq.LineColor),
                                    StrokeThickness = Math.Max(2.0, LineWidth)
                                };
                                ApplyLineStyle(polyline, eq.EquationStyle);
                            }
                            prevY = double.NaN;
                            continue;
                        }

                        // Check for vertical asymptotes
                        if (!double.IsNaN(prevY) && Math.Abs(mathY - prevY) > yRange * 3)
                        {
                            if (polyline.Points.Count > 1)
                            {
                                _graphCanvas.Children.Add(polyline);
                                polyline = new Polyline
                                {
                                    Stroke = new SolidColorBrush(eq.LineColor),
                                    StrokeThickness = Math.Max(2.0, LineWidth)
                                };
                                ApplyLineStyle(polyline, eq.EquationStyle);
                            }
                        }

                        double sx = ToScreenX(mathX);
                        double sy = ToScreenY(mathY);

                        polyline.Points.Add(new Point(sx, sy));
                        prevY = mathY;
                    }

                    if (polyline.Points.Count > 1)
                    {
                        _graphCanvas.Children.Add(polyline);
                    }
                }
            }
        }

        private static void ApplyLineStyle(Polyline polyline, EquationLineStyle style)
        {
            switch (style)
            {
                case EquationLineStyle.Dot:
                    polyline.StrokeDashArray = new DoubleCollection { 1, 2 };
                    break;
                case EquationLineStyle.Dash:
                    polyline.StrokeDashArray = new DoubleCollection { 4, 3 };
                    break;
                case EquationLineStyle.DashDot:
                    polyline.StrokeDashArray = new DoubleCollection { 4, 2, 1, 2 };
                    break;
                case EquationLineStyle.DashDotDot:
                    polyline.StrokeDashArray = new DoubleCollection { 4, 2, 1, 2, 1, 2 };
                    break;
            }
        }

        private static double CalculateGridStep(double range, double pixelSize)
        {
            double targetCount = Math.Max(3, pixelSize / 80.0);
            double rawStep = range / targetCount;
            double exponent = Math.Floor(Math.Log10(rawStep));
            double fraction = rawStep / Math.Pow(10, exponent);

            double niceFraction;
            if (fraction < 1.5) niceFraction = 1;
            else if (fraction < 3) niceFraction = 2;
            else if (fraction < 7) niceFraction = 5;
            else niceFraction = 10;

            return niceFraction * Math.Pow(10, exponent);
        }

        private static string FormatTickNumber(double val)
        {
            if (Math.Abs(val) < 1e-10) return "0";
            if (Math.Abs(val) >= 10000 || Math.Abs(val) < 0.01)
            {
                return val.ToString("G4", CultureInfo.InvariantCulture);
            }
            return Math.Round(val, 4).ToString(CultureInfo.InvariantCulture);
        }

        #endregion

        private void OnCoreKeyDown(CoreWindow sender, KeyEventArgs e) { }
        private void OnCoreKeyUp(CoreWindow sender, KeyEventArgs e) { }

        #region Property Changed Handlers

        private static void OnForceProportionalAxesChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is Grapher grapher)
            {
                grapher.RenderVisualGraph();
            }
        }

        private static void OnUseCommaDecimalSeperatorChanged(DependencyObject d, DependencyPropertyChangedEventArgs e) { }

        private static void OnEquationsChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is Grapher grapher)
            {
                if (e.OldValue is EquationCollection oldCol)
                {
                    oldCol.EquationChanged -= grapher.OnEquationChanged;
                    oldCol.EquationStyleChanged -= grapher.OnEquationStyleChanged;
                    oldCol.EquationLineEnabledChanged -= grapher.OnEquationLineEnabledChanged;
                }

                if (e.NewValue is EquationCollection newCol)
                {
                    newCol.EquationChanged += grapher.OnEquationChanged;
                    newCol.EquationStyleChanged += grapher.OnEquationStyleChanged;
                    newCol.EquationLineEnabledChanged += grapher.OnEquationLineEnabledChanged;
                }

                grapher.RenderVisualGraph();
                grapher.PlotGraph(false);
            }
        }

        private void OnEquationChanged(Equation equation)
        {
            if (equation != null)
            {
                equation.HasGraphError = false;
                equation.IsValidated = false;
            }
            RenderVisualGraph();
            _ = TryPlotGraph(false, true);
        }

        private void OnEquationStyleChanged(Equation equation)
        {
            RenderVisualGraph();
        }

        private void OnEquationLineEnabledChanged(Equation equation)
        {
            if (equation == null || equation.HasGraphError || string.IsNullOrEmpty(equation.Expression))
            {
                return;
            }
            bool keepCurrentView = true;
            if (!equation.HasGraphError && !equation.IsValidated && equation.IsLineEnabled)
            {
                keepCurrentView = false;
            }
            RenderVisualGraph();
            PlotGraph(keepCurrentView);
        }

        private static void OnAxesColorChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is Grapher grapher)
            {
                grapher.RenderVisualGraph();
            }
        }

        private static void OnGraphBackgroundChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is Grapher grapher && e.NewValue is Color color)
            {
                if (grapher._renderMain != null)
                {
                    grapher._renderMain.BackgroundColor = color;
                }
                grapher.RenderVisualGraph();
            }
        }

        private static void OnGridLinesColorChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is Grapher grapher)
            {
                grapher.RenderVisualGraph();
            }
        }

        private static void OnLineWidthChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is Grapher grapher)
            {
                TraceLogger.GetInstance().LogLineWidthChanged();
                grapher.RenderVisualGraph();
            }
        }

        #endregion

        private void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
