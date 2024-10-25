using System;
using System.IO;
using System.IO.Compression;
using System.Text;

namespace CalculatorApp
{
    internal static class DeflateUtils
    {
        public static byte[] Compress(string text)
        {
            var data = Encoding.UTF8.GetBytes(text);
            using (var compressed = new MemoryStream())
            {
                using (var deflater = new DeflateStream(compressed, CompressionLevel.Optimal))
                {
                    deflater.Write(data, 0, data.Length);
                }
                return compressed.ToArray();
            }
        }

        public static bool TryDecompress(byte[] data, out string text)
        {
            text = null;
            try
            {
                using (var srcStream = new MemoryStream(data))
                using (var deflateStream = new DeflateStream(srcStream, CompressionMode.Decompress))
                using (var resultStream = new MemoryStream())
                {
                    deflateStream.CopyTo(resultStream);
                    byte[] decompressed = resultStream.ToArray();
                    text = Encoding.UTF8.GetString(decompressed);
                    return true;
                }
            }
            catch (Exception)
            {
                return false;
            }
        }
    }
}
