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

        public static string Decompress(byte[] data)
        {
            using (var srcStream = new MemoryStream(data))
            using (var inflater = new DeflateStream(srcStream, CompressionMode.Decompress))
            using (var resultStream = new MemoryStream())
            {
                inflater.CopyTo(resultStream);
                byte[] decompressed = resultStream.ToArray();
                return Encoding.UTF8.GetString(decompressed);
            }
        }
    }
}
