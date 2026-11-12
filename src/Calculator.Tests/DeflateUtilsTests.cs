// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.IO;

using Microsoft.VisualStudio.TestTools.UnitTesting;

using CalculatorApp;

namespace Calculator.Tests
{
    [TestClass]
    public class DeflateUtilsTests
    {
        [DataTestMethod]
        [DataRow(0)]
        [DataRow(DeflateUtils.MaxDecompressedBytes - 1)]
        [DataRow(DeflateUtils.MaxDecompressedBytes)]
        public void DecompressionWithinByteLimitRoundTrips(int length)
        {
            var text = new string('9', length);

            Assert.AreEqual(text, DeflateUtils.Decompress(DeflateUtils.Compress(text)));
        }

        [DataTestMethod]
        [DataRow(1)]
        [DataRow(8192)]
        public void DecompressionBeyondByteLimitIsRejected(int excessBytes)
        {
            var compressed = DeflateUtils.Compress(
                new string('9', DeflateUtils.MaxDecompressedBytes + excessBytes));

            Assert.ThrowsException<InvalidDataException>(() => DeflateUtils.Decompress(compressed));
        }

        [TestMethod]
        public void DecompressionLimitCountsUtf8BytesRatherThanCharacters()
        {
            var text = new string('\u00e9', DeflateUtils.MaxDecompressedBytes / 2);

            Assert.AreEqual(text, DeflateUtils.Decompress(DeflateUtils.Compress(text)));
            Assert.ThrowsException<InvalidDataException>(
                () => DeflateUtils.Decompress(DeflateUtils.Compress(text + "9")));
        }
    }
}
