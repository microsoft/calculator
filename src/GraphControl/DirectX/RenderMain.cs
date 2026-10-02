// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace GraphControl.DX
{
    public sealed class RenderMain : IDeviceNotify
    {
        private readonly DeviceResources _deviceResources;
        private readonly NearestPointRenderer _nearestPointRenderer;
        private readonly SwapChainPanel _swapChainPanel;
        private object _graph;
        private bool _drawNearestPoint;
        private Point _pointerLocation;
        private bool _drawActiveTracing;
        private Point _activeTracingPointerLocation;
        private Color _backgroundColor = Colors.Transparent;
        private double _xTraceValue;
        private double _yTraceValue;
        private Point _traceLocation;
        private bool _tracing;
        private bool _activeTracing;
        private int _hResult;

        public object Graph
        {
            get => _graph;
            set => _graph = value;
        }

        public Color BackgroundColor
        {
            get => _backgroundColor;
            set
            {
                _backgroundColor = value;
                RunRenderPass();
            }
        }

        public bool DrawNearestPoint
        {
            get => _drawNearestPoint;
            set
            {
                if (_drawNearestPoint != value)
                {
                    _drawNearestPoint = value;
                    if (!_drawNearestPoint)
                    {
                        _tracing = false;
                    }
                }
            }
        }

        public Point PointerLocation
        {
            get => _pointerLocation;
            set
            {
                if (_pointerLocation != value)
                {
                    _pointerLocation = value;
                    bool wasPointRendered = _tracing;
                    if (CanRenderPoint() || wasPointRendered)
                    {
                        _ = RunRenderPassAsync();
                    }
                }
            }
        }

        public bool ActiveTracing
        {
            get => _activeTracing;
            set
            {
                if (_activeTracing != value)
                {
                    _activeTracing = value;
                    if (_activeTracing)
                    {
                        _drawActiveTracing = true;
                    }
                    else
                    {
                        _drawActiveTracing = false;
                        _tracing = false;
                    }
                    RunRenderPass();
                }
            }
        }

        public Point ActiveTraceCursorPosition
        {
            get => _activeTracingPointerLocation;
            set
            {
                if (_activeTracingPointerLocation != value)
                {
                    _activeTracingPointerLocation = value;
                    bool wasPointRendered = _tracing;
                    if (CanRenderPoint() || wasPointRendered)
                    {
                        _ = RunRenderPassAsync();
                    }
                }
            }
        }

        public double XTraceValue => _xTraceValue;
        public double YTraceValue => _yTraceValue;
        public Point TraceLocation => _traceLocation;
        public bool Tracing => _tracing;

        public RenderMain(SwapChainPanel panel)
        {
            _swapChainPanel = panel;
            _deviceResources = new DeviceResources(panel);
            _nearestPointRenderer = new NearestPointRenderer(_deviceResources);
            _deviceResources.RegisterDeviceNotify(this);
            RegisterEventHandlers();
        }

        public void OnDeviceLost()
        {
            _nearestPointRenderer.ReleaseDeviceDependentResources();
        }

        public void OnDeviceRestored()
        {
            _nearestPointRenderer.CreateDeviceDependentResources();
            CreateWindowSizeDependentResources();
        }

        public void CreateWindowSizeDependentResources()
        {
        }

        public bool CanRenderPoint()
        {
            return _drawNearestPoint || _drawActiveTracing;
        }

        public void SetPointRadius(float radius)
        {
            _nearestPointRenderer.SetRadius(radius);
        }

        public bool RunRenderPass()
        {
            return RunRenderPassInternal();
        }

        public Task<bool> RunRenderPassAsync(bool allowCancel = true)
        {
            return Task.FromResult(RunRenderPassInternal());
        }

        public int GetRenderError() => _hResult;

        private bool RunRenderPassInternal()
        {
            return Render();
        }

        private bool Render()
        {
            return true;
        }

        private void RegisterEventHandlers()
        {
            if (_swapChainPanel != null)
            {
                _swapChainPanel.Loaded += OnLoaded;
                _swapChainPanel.CompositionScaleChanged += OnCompositionScaleChanged;
                _swapChainPanel.SizeChanged += OnSizeChanged;
            }
        }

        private void UnregisterEventHandlers()
        {
            if (_swapChainPanel != null)
            {
                _swapChainPanel.Loaded -= OnLoaded;
                _swapChainPanel.CompositionScaleChanged -= OnCompositionScaleChanged;
                _swapChainPanel.SizeChanged -= OnSizeChanged;
            }
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
        }

        private void OnCompositionScaleChanged(SwapChainPanel sender, object args)
        {
            if (sender != null)
            {
                _deviceResources.SetCompositionScale((float)sender.CompositionScaleX, (float)sender.CompositionScaleY);
            }
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (e != null)
            {
                _deviceResources.SetLogicalSize(e.NewSize);
            }
        }
    }
}
