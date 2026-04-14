// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.ViewModel.Common
{
    /// <summary>
    /// General utility methods. C# port of Utils.h/cpp.
    /// Note: The C++ OBSERVABLE_OBJECT, OBSERVABLE_PROPERTY_RW, and OBSERVABLE_PROPERTY_R macros
    /// are not needed in C# since INotifyPropertyChanged is used directly.
    /// </summary>
    public static class Utils
    {
        public static void IFTPlatformException(int hr)
        {
            if (hr < 0)
            {
                System.Runtime.InteropServices.Marshal.ThrowExceptionForHR(hr);
            }
        }

        public static double GetDoubleFromWstring(string input)
        {
            char[] unwantedChars = { ' ', ',', (char)8234, (char)8235, (char)8236, (char)8237 };
            string cleaned = RemoveUnwantedCharsFromString(input, unwantedChars);
            return double.Parse(cleaned);
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

        public static void RunOnUIThreadNonblocking(Action function, CoreDispatcher currentDispatcher)
        {
            if (currentDispatcher != null)
            {
                var _ = currentDispatcher.RunAsync(CoreDispatcherPriority.Normal, () => function());
            }
        }

        public static bool IsLastCharacterTarget(string input, char target)
        {
            return !string.IsNullOrEmpty(input) && input[input.Length - 1] == target;
        }

        public static DateTimeOffset GetUniversalSystemTime()
        {
            return DateTimeOffset.UtcNow;
        }

        public static bool IsDateTimeOlderThan(DateTimeOffset dateTime, long durationInTicks)
        {
            var now = DateTimeOffset.UtcNow;
            return dateTime.Ticks + durationInTicks < now.Ticks;
        }

        public static async Task WriteFileToFolder(IStorageFolder folder, string fileName, string contents, CreationCollisionOption collisionOption)
        {
            if (folder == null) return;
            StorageFile file = await folder.CreateFileAsync(fileName, collisionOption);
            if (file == null) return;
            await FileIO.WriteTextAsync(file, contents);
        }

        public static async Task<string> ReadFileFromFolder(IStorageFolder folder, string fileName)
        {
            if (folder == null) return null;
            StorageFile file = await folder.GetFileAsync(fileName);
            if (file == null) return null;
            return await FileIO.ReadTextAsync(file);
        }

        public static bool AreColorsEqual(Color color1, Color color2)
        {
            return color1.A == color2.A && color1.R == color2.R && color1.G == color2.G && color1.B == color2.B;
        }

        public static string Trim(string value)
        {
            return value?.Trim();
        }

        public static string RemoveUnwantedCharsFromString(string input, char[] unwantedChars)
        {
            var sb = new StringBuilder(input.Length);
            foreach (char ch in input)
            {
                if (Array.IndexOf(unwantedChars, ch) < 0)
                {
                    sb.Append(ch);
                }
            }
            return sb.ToString();
        }
    }

    /// <summary>
    /// Public WinRT-visible utility class. Port of the C++/CX Utilities ref class.
    /// </summary>
    public sealed class Utilities
    {
        public static string EscapeHtmlSpecialCharacters(string originalString)
        {
            char[] specialCharacters = { '&', '"', '\'', '<', '>' };

            bool replaceCharacters = false;
            int replaceIndex = 0;

            for (int i = 0; i < originalString.Length; i++)
            {
                if (Array.IndexOf(specialCharacters, originalString[i]) >= 0)
                {
                    replaceCharacters = true;
                    replaceIndex = i;
                    break;
                }
            }

            if (!replaceCharacters) return originalString;

            var sb = new StringBuilder();
            sb.Append(originalString, 0, replaceIndex);

            for (int i = replaceIndex; i < originalString.Length; i++)
            {
                switch (originalString[i])
                {
                    case '&': sb.Append("&amp;"); break;
                    case '"': sb.Append("&quot;"); break;
                    case '\'': sb.Append("&apos;"); break;
                    case '<': sb.Append("&lt;"); break;
                    case '>': sb.Append("&gt;"); break;
                    default: sb.Append(originalString[i]); break;
                }
            }

            return sb.ToString();
        }

        public static bool AreColorsEqual(Color color1, Color color2) => Utils.AreColorsEqual(color1, color2);

        public static SolidColorBrush GetContrastColor(Color backgroundColor)
        {
            var luminance = 0.2126 * backgroundColor.R + 0.7152 * backgroundColor.G + 0.0722 * backgroundColor.B;
            if ((255 + 0.05) / (luminance + 0.05) >= 2.5)
            {
                return (SolidColorBrush)Application.Current.Resources["WhiteBrush"];
            }
            return (SolidColorBrush)Application.Current.Resources["BlackBrush"];
        }

        public static int GetWindowId() => Utils.GetWindowId();

        public static long GetConst_WINEVENT_KEYWORD_RESPONSE_TIME()
        {
            return 0x0000001000000000;
        }

        public static bool GetIntegratedDisplaySize(out double sizeInInches)
        {
            sizeInInches = 0.0;
            try
            {
                var displayInfo = Windows.Graphics.Display.DisplayInformation.GetForCurrentView();
                // Compute diagonal size from raw pixels and DPI
                double rawWidth = displayInfo.ScreenWidthInRawPixels;
                double rawHeight = displayInfo.ScreenHeightInRawPixels;
                double dpiX = displayInfo.RawDpiX;
                double dpiY = displayInfo.RawDpiY;
                if (dpiX > 0 && dpiY > 0)
                {
                    double widthInInches = rawWidth / dpiX;
                    double heightInInches = rawHeight / dpiY;
                    sizeInInches = System.Math.Sqrt(widthInInches * widthInInches + heightInInches * heightInInches);
                    return true;
                }
            }
            catch
            {
            }
            return false;
        }
    }
}
