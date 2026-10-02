// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorUnitTests
{
    public static class Helpers
    {
        public const int StandardModePrecision = 16;
        public const int ScientificModePrecision = 32;
        public const int ProgrammerModePrecision = 64;

        public static class UtfUtils
        {
            public const char MUL = '\u00d7'; // Multiplication Symbol
        }

        public static void VerifyVectorsAreEqual<T>(IList<T> vecA, IList<T> vecB, string message = "")
        {
            if (vecA == null && vecB == null) return;
            Assert.IsNotNull(vecA, message);
            Assert.IsNotNull(vecB, message);
            Assert.AreEqual(vecA.Count, vecB.Count, message);

            for (int i = 0; i < vecA.Count; ++i)
            {
                Assert.AreEqual(vecA[i], vecB[i], $"{message} at index {i}");
            }
        }
    }
}
