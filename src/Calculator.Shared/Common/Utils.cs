using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
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

            //var window = CoreWindow.GetForCurrentThread();
            //if (window != null)
            //{
            //    windowId = ApplicationView.GetApplicationViewIdForWindow(window);
            //}

            return windowId;
        }

        public static bool IsLastCharacterTarget(string input, char target)
        {
            return !string.IsNullOrEmpty(input) && input.Last() == target;
        }

		public static void Swap<T>(ref T field1, ref T field2)
		{
			var tmp = field1;
			field1 = field2;
			field2 = tmp;
		}

		public static bool IsDateTimeOlderThan(DateTime dateTime, long duration)
		{
			return dateTime + TimeSpan.FromTicks(duration) < DateTime.Now;
		}

		public static async Task<string> ReadFileFromFolder(StorageFolder folder, string filename)
		{
			if (folder == null)
			{
				return null;
			}

			var filePath = Path.Combine(folder.Path, filename);
			using (var reader = new StreamReader(filePath))
			{
				return await reader.ReadToEndAsync();
			}
		}

		public static async Task WriteFileToFolder(StorageFolder folder, string filename, string contents, CreationCollisionOption colisionOption)
		{
			if (folder== null)
			{
				return;
			}

			var filePath = Path.Combine(folder.Path, filename);
			using (var writer = new StreamWriter(filePath, append: false))
			{
				await writer.WriteAsync(contents);
			}
		}

		public static DateTime GetUniversalSystemTime() => DateTime.UtcNow;
	}
}
