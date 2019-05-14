using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Windows.UI.Core;
using Windows.UI.ViewManagement;

namespace CalculatorApp
{
    public static class Utils
    {
        public const char LRE = (char)0x202a; // Left-to-Right Embedding
        public const char PDF = (char)0x202c; // Pop Directional Formatting
        public const char LRO = (char)0x202d; // Left-to-Right Override

        // Returns windowId for the current view
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

        public static bool IsLastCharacterTarget(string input, char target)
        {
            return !string.IsNullOrEmpty(input) && input.Last() == target;
        }
    }
}
