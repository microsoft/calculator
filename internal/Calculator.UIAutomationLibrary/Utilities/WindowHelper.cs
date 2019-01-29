// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation.Controls;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary
{
    public class WindowHelper
    {
        public static void SetAsForeground(Window window)
        {
            Log.Comment($"Set window {window.NativeWindowHandle} as the foreground window.");

            NativeMethods.SetForegroundWindow(window.NativeWindowHandle);
        }
    }
}
