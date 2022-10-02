// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Runtime.InteropServices;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.ViewModel.Common
{
    public static class Utilities
    {
        private static class NativeMethods
        {
            [DllImport("kernelbase.dll", ExactSpelling = true, PreserveSig = true)]
            public static extern int GetIntegratedDisplaySize(out double sizeInInches);
        }

        public static bool GetIntegratedDisplaySize(out double sizeInInches)
        {
            var hresult = NativeMethods.GetIntegratedDisplaySize(out sizeInInches);
            return hresult == 0;
        }

        // This method calculates the luminance ratio between White and the given background color.
        // The luminance is calculate using the RGB values and does not use the A value.
        // White or Black is returned
        public static SolidColorBrush GetContrastColor(Color backgroundColor)
        {
            var luminance = 0.2126 * backgroundColor.R + 0.7152 * backgroundColor.G + 0.0722 * backgroundColor.B;

            if ((255 + 0.05) / (luminance + 0.05) >= 2.5)
            {
                return (SolidColorBrush)Application.Current.Resources["WhiteBrush"];
            }

            return (SolidColorBrush)Application.Current.Resources["BlackBrush"];
        }

        public static int GetWindowId()
        {
            int windowId = -1;

            var window = CoreWindow.GetForCurrentThread();
            if (window != null)
            {
                windowId = ApplicationView.GetApplicationViewIdForWindow(window);
            }

            return windowId;
        }
    }
}
