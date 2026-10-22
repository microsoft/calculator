// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.UI.Xaml.Controls;

namespace GraphControl.DX
{
    public interface IDeviceNotify
    {
        void OnDeviceLost();
        void OnDeviceRestored();
    }

    public class DeviceResources
    {
        private SwapChainPanel _swapChainPanel;
        private Size _outputSize;
        private Size _logicalSize;
        private DisplayOrientations _nativeOrientation;
        private DisplayOrientations _currentOrientation;
        private float _dpi = 96.0f;
        private float _effectiveDpi = 96.0f;
        private float _compositionScaleX = 1.0f;
        private float _compositionScaleY = 1.0f;
        private IDeviceNotify _deviceNotify;

        public Size OutputSize => _outputSize;
        public Size LogicalSize => _logicalSize;
        public float Dpi => _effectiveDpi;

        public DeviceResources(SwapChainPanel panel)
        {
            SetSwapChainPanel(panel);
        }

        public void SetSwapChainPanel(SwapChainPanel panel)
        {
            _swapChainPanel = panel;
            if (panel != null)
            {
                _logicalSize = new Size(panel.ActualWidth, panel.ActualHeight);
                _compositionScaleX = (float)panel.CompositionScaleX;
                _compositionScaleY = (float)panel.CompositionScaleY;
            }
        }

        public void SetLogicalSize(Size logicalSize)
        {
            if (_logicalSize != logicalSize)
            {
                _logicalSize = logicalSize;
            }
        }

        public void SetCurrentOrientation(DisplayOrientations currentOrientation)
        {
            if (_currentOrientation != currentOrientation)
            {
                _currentOrientation = currentOrientation;
            }
        }

        public void SetDpi(float dpi)
        {
            if (dpi != _dpi)
            {
                _dpi = dpi;
                _effectiveDpi = dpi;
            }
        }

        public void SetCompositionScale(float compositionScaleX, float compositionScaleY)
        {
            _compositionScaleX = compositionScaleX;
            _compositionScaleY = compositionScaleY;
        }

        public void ValidateDevice()
        {
        }

        public void HandleDeviceLost()
        {
            _deviceNotify?.OnDeviceLost();
            _deviceNotify?.OnDeviceRestored();
        }

        public void RegisterDeviceNotify(IDeviceNotify deviceNotify)
        {
            _deviceNotify = deviceNotify;
        }

        public void Trim()
        {
        }

        public void Present()
        {
        }
    }
}
