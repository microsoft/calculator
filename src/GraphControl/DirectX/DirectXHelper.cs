// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.Streams;

namespace GraphControl.DX
{
    public static class DirectXHelper
    {
        public static void ThrowIfFailed(int hr)
        {
            if (hr < 0)
            {
                throw new Exception($"DirectX operation failed with HRESULT: 0x{hr:X8}");
            }
        }

        public static async Task<byte[]> ReadDataAsync(string filename)
        {
            var folder = Windows.ApplicationModel.Package.Current.InstalledLocation;
            var file = await folder.GetFileAsync(filename);
            var buffer = await FileIO.ReadBufferAsync(file);
            using (var reader = DataReader.FromBuffer(buffer))
            {
                byte[] bytes = new byte[buffer.Length];
                reader.ReadBytes(bytes);
                return bytes;
            }
        }

        public static float ConvertDipsToPixels(float dips, float dpi)
        {
            const float dipsPerInch = 96.0f;
            return (float)Math.Floor(dips * dpi / dipsPerInch + 0.5f);
        }
    }
}
