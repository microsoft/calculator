// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using CalcManager;
using CalcManager.CalculationManager;
using CalcManager.CEngine;
using CalculatorUnitTests;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorEngineTests
{
    [TestClass]
    public class CalcEngineTests
    {
        private const int MAX_HISTORY_SIZE = 20;
        private CCalcEngine m_calcEngine;
        private IResourceProvider m_resourceProvider;
        private CalculatorHistory m_history;

        [TestInitialize]
        public void CommonSetup()
        {
            m_resourceProvider = new EngineResourceProvider();
            m_history = new CalculatorHistory(MAX_HISTORY_SIZE);
            CCalcEngine.InitialOneTimeOnlySetup(m_resourceProvider);
            m_calcEngine = new CCalcEngine(
                false /* Respect Order of Operations */,
                false /* Set to Integer Mode */,
                m_resourceProvider,
                null,
                m_history);
        }

        [TestCleanup]
        public void Cleanup()
        {
            m_resourceProvider = null;
            m_history = null;
            m_calcEngine = null;
        }

        [TestMethod]
        public void TestGroupDigitsPerRadix()
        {
            // Empty/Error cases
            Assert.IsTrue(string.IsNullOrEmpty(m_calcEngine.GroupDigitsPerRadix("", 10)), "Verify grouping empty string returns empty string.");
            Assert.AreEqual("12345678", m_calcEngine.GroupDigitsPerRadix("12345678", 9), "Verify grouping on invalid base returns original string");

            // Octal
            Assert.AreEqual("1 234 567", m_calcEngine.GroupDigitsPerRadix("1234567", 8), "Verify grouping in octal.");
            Assert.AreEqual("123", m_calcEngine.GroupDigitsPerRadix("123", 8), "Verify minimum grouping in octal.");

            // Binary/Hexadecimal
            Assert.AreEqual("12 3456 7890", m_calcEngine.GroupDigitsPerRadix("1234567890", 2), "Verify grouping in binary.");
            Assert.AreEqual("1234", m_calcEngine.GroupDigitsPerRadix("1234", 2), "Verify minimum grouping in binary.");
            Assert.AreEqual("12 3456 7890", m_calcEngine.GroupDigitsPerRadix("1234567890", 16), "Verify grouping in hexadecimal.");
            Assert.AreEqual("1234", m_calcEngine.GroupDigitsPerRadix("1234", 16), "Verify minimum grouping in hexadecimal.");

            // Decimal
            Assert.AreEqual("1,234,567,890", m_calcEngine.GroupDigitsPerRadix("1234567890", 10), "Verify grouping in base10.");
            Assert.AreEqual("1,234,567.89", m_calcEngine.GroupDigitsPerRadix("1234567.89", 10), "Verify grouping in base10 with decimal.");
            Assert.AreEqual("1,234,567e89", m_calcEngine.GroupDigitsPerRadix("1234567e89", 10), "Verify grouping in base10 with exponent.");
            Assert.AreEqual("1,234,567.89e5", m_calcEngine.GroupDigitsPerRadix("1234567.89e5", 10), "Verify grouping in base10 with decimal and exponent.");
            Assert.AreEqual("-123,456,789", m_calcEngine.GroupDigitsPerRadix("-123456789", 10), "Verify grouping in base10 with negative.");
        }

        [TestMethod]
        public void TestIsNumberInvalid()
        {
            // Binary Number Checks
            var validBinStrs = new List<string> { "0", "1", "0011", "1100" };
            var invalidBinStrs = new List<string> { "2", "A", "0.1" };
            foreach (var str in validBinStrs)
            {
                Assert.AreEqual(0, m_calcEngine.IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }
            foreach (var str in invalidBinStrs)
            {
                Assert.AreEqual(EngineStrings.IDS_ERR_UNK_CH, m_calcEngine.IsNumberInvalid(str, 0, 0, 2 /* Binary */));
            }

            // Octal Number Checks
            var validOctStrs = new List<string> { "0", "7", "01234567", "76543210" };
            var invalidOctStrs = new List<string> { "8", "A", "0.7" };
            foreach (var str in validOctStrs)
            {
                Assert.AreEqual(0, m_calcEngine.IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }
            foreach (var str in invalidOctStrs)
            {
                Assert.AreEqual(EngineStrings.IDS_ERR_UNK_CH, m_calcEngine.IsNumberInvalid(str, 0, 0, 8 /* Octal */));
            }

            // Hexadecimal Number Checks
            var validHexStrs = new List<string> { "0", "F", "0123456789ABCDEF", "FEDCBA9876543210" };
            var invalidHexStrs = new List<string> { "G", "abcdef", "x", "0.1" };
            foreach (var str in validHexStrs)
            {
                Assert.AreEqual(0, m_calcEngine.IsNumberInvalid(str, 0, 0, 16 /* Hex */));
            }
            foreach (var str in invalidHexStrs)
            {
                Assert.AreEqual(EngineStrings.IDS_ERR_UNK_CH, m_calcEngine.IsNumberInvalid(str, 0, 0, 16 /* Hex */));
            }

            // Special case errors: long exponent, long mantissa
            string longExp = "1e12345";
            Assert.AreEqual(0, m_calcEngine.IsNumberInvalid(longExp, 5 /* Max exp length */, 100, 10 /* Decimal */));
            Assert.AreEqual(EngineStrings.IDS_ERR_INPUT_OVERFLOW, m_calcEngine.IsNumberInvalid(longExp, 4 /* Max exp length */, 100, 10 /* Decimal */));

            var longMantStrs = new List<string> { "10000", "10.000", "0000012345", "123.45", "0.00123", "0.12345", "-123.45e678" };
            foreach (var str in longMantStrs)
            {
                Assert.AreEqual(0, m_calcEngine.IsNumberInvalid(str, 100, 5 /* Max mantissa length */, 10 /* Decimal */));
            }
            foreach (var str in longMantStrs)
            {
                Assert.AreEqual(EngineStrings.IDS_ERR_INPUT_OVERFLOW, m_calcEngine.IsNumberInvalid(str, 100, 4 /* Max mantissa length */, 10 /* Decimal */));
            }

            var validDecStrs = new List<string>
            {
                "+1", "-1", "1", "-", "", "1234567890",
                "1.0", "-.", "1.", "0.0", "0.123456",
                "1e", "1.e", "-e", "1e+12345", "1e-12345", "1e123",
                "-123.456e+789"
            };
            var invalidDecStrs = new List<string> { "x123", "123-", "1e1.2", "1-e2" };
            foreach (var str in validDecStrs)
            {
                Assert.AreEqual(0, m_calcEngine.IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
            foreach (var str in invalidDecStrs)
            {
                Assert.AreEqual(EngineStrings.IDS_ERR_UNK_CH, m_calcEngine.IsNumberInvalid(str, 100, 100, 10 /* Dec */));
            }
        }

        [TestMethod]
        public void TestDigitGroupingStringToGroupingVector()
        {
            var groupingVector = new List<uint>();
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector(""), "Verify empty grouping");

            groupingVector = new List<uint> { 1 };
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector("1"), "Verify simple grouping");

            groupingVector = new List<uint> { 3, 0 };
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector("3;0"), "Verify standard grouping");

            groupingVector = new List<uint> { 3, 0, 0 };
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector("3;0;0"), "Verify expanded non-repeating grouping");

            groupingVector = new List<uint> { 5, 3, 2, 4, 6 };
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector("5;3;2;4;6"), "Verify long grouping");

            groupingVector = new List<uint> { 15, 15, 15, 0 };
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector("15;15;15;0"), "Verify large grouping");

            groupingVector = new List<uint> { 4, 7, 0 };
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector("4;16;7;25;0"), "Verify we ignore oversize grouping");

            groupingVector = new List<uint> { 3, 0 };
            string nonRepeatingGrouping = "3;0;0";
            string repeatingGrouping = nonRepeatingGrouping.Substring(0, 3);
            CollectionAssert.AreEqual(groupingVector, CCalcEngine.DigitGroupingStringToGroupingVector(repeatingGrouping), "Verify we don't go past the end of string range");
        }

        [TestMethod]
        public void TestGroupDigits()
        {
            string result = "1234567";
            Assert.AreEqual(result, m_calcEngine.GroupDigits("", new List<uint> { 3, 0 }, "1234567", false), "Verify handling of empty delimiter.");
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint>(), "1234567", false), "Verify handling of empty grouping.");

            result = "1,234,567";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 3, 0 }, "1234567", false), "Verify standard digit grouping.");

            result = "1 234 567";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(" ", new List<uint> { 3, 0 }, "1234567", false), "Verify delimiter change.");

            result = "1|||234|||567";
            Assert.AreEqual(result, m_calcEngine.GroupDigits("|||", new List<uint> { 3, 0 }, "1234567", false), "Verify long delimiter.");

            result = "12,345e67";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 3, 0 }, "12345e67", false), "Verify respect of exponent.");

            result = "12,345.67";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 3, 0 }, "12345.67", false), "Verify respect of decimal.");

            result = "1,234.56e7";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 3, 0 }, "1234.56e7", false), "Verify respect of exponent and decimal.");

            result = "-1,234,567";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 3, 0 }, "-1234567", true), "Verify negative number grouping.");

            // Test various groupings
            result = "1234567890123456";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 0, 0 }, "1234567890123456", false), "Verify no grouping.");

            result = "1234567890123,456";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 3 }, "1234567890123456", false), "Verify non-repeating grouping.");
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 3, 0, 0 }, "1234567890123456", false), "Verify expanded form non-repeating grouping.");

            result = "12,34,56,78,901,23456";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 5, 3, 2, 0 }, "1234567890123456", false), "Verify multigroup with repeating grouping.");

            result = "1234,5678,9012,3456";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 4, 0 }, "1234567890123456", false), "Verify repeating non-standard grouping.");

            result = "123456,78,901,23456";
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 5, 3, 2 }, "1234567890123456", false), "Verify multigroup non-repeating grouping.");
            Assert.AreEqual(result, m_calcEngine.GroupDigits(",", new List<uint> { 5, 3, 2, 0, 0 }, "1234567890123456", false), "Verify expanded form multigroup non-repeating grouping.");
        }
    }
}
