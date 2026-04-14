// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class LocalizationServiceUnitTests
    {
        [TestMethod]
        [Ignore("LocalizationService.Sort not available in C#")]
        public void TestSortStrings()
        {
            // Ported from C++ LocalizationServiceUnitTests
            // Original test sorts strings using LocalizationService::GetInstance()->Sort()
            // and verifies locale-aware sort order:
            //   Input:    "Zebra", "Alpha", "beta1", "Gamma", "Beta", "alpha1", "États-Unis", "Epsilon", "Etude"
            //   Expected: "Alpha", "alpha1", "Beta", "beta1", "Epsilon", "États-Unis", "Etude", "Gamma", "Zebra"
        }

        [TestMethod]
        [Ignore("LocalizationService.Sort not available in C#")]
        public void TestSortEmptyStrings()
        {
            // Ported from C++ LocalizationServiceUnitTests
            // Verifies LocalizationService::Sort doesn't crash when the vector is empty or null:
            //   - Empty list
            //   - List with single empty string
            //   - List with multiple empty strings
            //   - List with null and empty strings
        }

        [TestMethod]
        [Ignore("LocalizationService.Sort not available in C#")]
        public void TestSortGeneric()
        {
            // Ported from C++ LocalizationServiceUnitTests
            // Tests sorting with a key selector function using LocalizationService::Sort<T>()
            // First sort uses a function that wraps strings with "CAL:" prefix and "TEST" suffix
            // Second sort uses a function that takes substring from index 1
            //   Input:    "fermentum", "fringilla", "Curabitur", "rhoncus", "Aenean", "Fusce",
            //             "sollicitudin", "empor", "edapibus", "édapibas", "édapîbos", "édapîbÉs"
            //   Expected (first sort):
            //             "Aenean", "Curabitur", "édapibas", "édapîbÉs", "édapîbos", "edapibus",
            //             "empor", "fermentum", "fringilla", "Fusce", "rhoncus", "sollicitudin"
            //   Expected (second sort):
            //             "édapibas", "édapîbÉs", "édapîbos", "edapibus", "Aenean", "fermentum",
            //             "rhoncus", "empor", "sollicitudin", "fringilla", "Curabitur", "Fusce"
        }
    }
}
