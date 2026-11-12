using System;
using System.IO;
using System.IO.Compression;
using System.Text;

namespace CalculatorApp
{
    internal static class DeflateUtils
    {
        internal const int MaxDecompressedBytes = 1024 * 1024;

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
                var buffer = new byte[8192];
                int bytesRead;
                while ((bytesRead = inflater.Read(buffer, 0, buffer.Length)) != 0)
                {
                    if (bytesRead > MaxDecompressedBytes - resultStream.Length)
                    {
                        throw new InvalidDataException("Decompressed snapshot exceeds the size limit.");
                    }

                    resultStream.Write(buffer, 0, bytesRead);
                }
                byte[] decompressed = resultStream.ToArray();
                return Encoding.UTF8.GetString(decompressed);
            }
        }
    }
}
