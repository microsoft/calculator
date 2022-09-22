// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Core;
using Windows.UI.ViewManagement;

namespace CalculatorApp.ViewModel.Common
{
    public static class Utilities
    {
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
