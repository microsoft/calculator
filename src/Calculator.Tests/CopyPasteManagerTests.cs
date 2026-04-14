// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class CopyPasteManagerTests
    {
        [TestMethod]
        public void FunctionalCopyPasteTest()
        {
            var scvm = new StandardCalculatorViewModel();
            scvm.IsStandard = true;
            string[] inputs = { "123", "12345", "123+456", "1,234", "1 2 3", "\n\r1,234\n", "\n 1+\n2 ", "1\"2" };

            foreach (var input in inputs)
            {
                scvm.OnPaste(input);
                Assert.AreEqual(ValidateStandardPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
                Assert.AreEqual(ValidateScientificPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(scvm.DisplayValue), scvm.DisplayValue);
            }
        }

        [TestMethod]
        public void ValidateStandardPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123",
                "+123",
                "-133",
                "12345.",
                "+12.34",
                "12.345",
                "012.034",
                "-23.032",
                "-.123",
                ".1234",
                "012.012",
                "123+456",
                "123+-234",
                "123*-345",
                "123*4*-3",
                "123*+4*-3",
                "1,234",
                "1 2 3",
                "\n\r1,234\n",
                "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ",
                "1\"2",
                "1234567891234567",
                "2+2=",
                "2+2=   ",
                "1.2e23",
                "12345e-23",
            };
            string[] negativeInput =
            {
                "(123)+(456)",
                "abcdef",
                "xyz",
                "ABab",
                "e+234",
                "12345678912345678",
                "SIN(2)",
                "2+2==",
                "2=+2",
                "2%2",
                "10^2",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateStandardPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateStandardPasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateScientificPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123",
                "+123",
                "-133",
                "123+456",
                "12345e+023",
                "1,234",
                "1.23",
                "-.123",
                ".1234",
                "012.012",
                "123+-234",
                "123*-345",
                "123*4*-3",
                "123*+4*-3",
                "1 2 3",
                "\n\r1,234\n",
                "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ",
                "1\"2",
                "1.2e+023",
                "12345e-23",
                "(123)+(456)",
                "12345678912345678123456789012345",
                "(123)+(456)=",
                "2+2=   ",
                "-(43)",
                "+(41213)",
                "-(432+3232)",
                "-(+(-3213)+(-2312))",
                "-(-(432+3232))",
                "1.2e23",
                "12^2",
                "-12.12^-2",
                "61%99" + "-6.1%99",
                "1.1111111111111111111111111111111e+1142",
            };
            string[] negativeInput =
            {
                "abcdef",
                "xyz",
                "ABab",
                "e+234",
                "123456789123456781234567890123456",
                "11.1111111111111111111111111111111e+1142",
                "1.1e+10001",
                "0.11111111111111111111111111111111111e+111111" + "SIN(2)",
                "2+2==",
                "2=+2",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateScientificPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateScientificPasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateProgrammerDecPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "9223372036854775807", "-9223372036854775808",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ", "823%21",
            };
            string[] qwordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "9223372036854775808", "9223372036854775809",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
                "2+2==", "2=+2",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "2147483647", "-2147483647",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "2147483649",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "+123", "-133", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\f\n1+2\t\r\v\x85", "1\"2",
                "(123)+(456)", "123+-234", "123*-345", "123*4*-3", "123*+4*-3",
                "32767", "-32767", "-32768",
                "0n1234", "0N1234", "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "32769",
                "SIN(2)", "-0n123", "0nn1234", "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerDecWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "+13", "-13", "13+46", "13+-34",
                "13*-3", "3*4*-3", "3*+4*-3", "1,3", "1 3",
                "1'2'3", "1_2_3", "1\"2", "127", "-127",
                "0n123", "0N123", "123u", "123ul", "123ULL",
            };
            string[] byteNegativeInput =
            {
                "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "1.2e23", "1.2e+023", "15e-23",
                "abcdef", "xyz", "ABab", "e+24",
                "129",
                "SIN(2)", "-0n123", "0nn1234", "123uu", "123ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerDecBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerDecBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateProgrammerOctPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ", "127%71",
                "1777777777777777777777",
            };
            string[] qwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "12345678901234567890123",
                "2000000000000000000000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
                "2+2==", "2=+2", "89%12",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ", "1\"2",
                "(123)+(456)", "37777777777",
                "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "377777777771",
                "40000000000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "\f\n1+2\t\r\v\x85", "1\"2",
                "(123)+(456)", "177777",
                "0t1234", "0T1234", "0o1234", "0O1234",
                "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e23", "1.2e+023", "12345e-23",
                "abcdef", "xyz", "ABab", "e+234",
                "1777771",
                "200000",
                "SIN(2)", "123+-234", "0ot1234", "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerOctWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "13+46", "1,3", "1 3", "1'2'3", "1_2_3", "1\"2",
                "377",
                "0t123", "0T123", "0o123", "0O123", "123u", "123ul", "123ULL",
            };
            string[] byteNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-13", "1.2e23", "1.2e+023", "15e-23",
                "abcdef", "xyz", "ABab", "e+24",
                "477",
                "400",
                "SIN(2)", "123+-34", "0ot123", "123uu", "123ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerOctBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerOctBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateProgrammerHexPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "12345e-23",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\f\n1+2\t\r\v\x85",
                "\n 1+\n2 ", "e+234", "1\"2",
                "(123)+(456)", "abcdef", "ABab",
                "ABCDF21abc41a",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
                "2+2=", "2+2=   ",
                "A4C3%12", "1233%AB", "FFC1%F2",
            };
            string[] qwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "ABCDEF21abc41abc7",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
                "2+2==", "2=+2",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "123", "123+456", "1,234", "1 2 3", "1'2'3'4", "1_2_3_4",
                "12345e-23",
                "\n\r1,234\n", "\f\n1+2\t\r\v\x85", "\n 1+\n2 ",
                "e+234", "1\"2",
                "(123)+(456)", "abcdef", "ABab",
                "ABCD123a",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
            };
            string[] dwordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "ABCD123ab",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "123", "13+456", "1,34", "12 3", "1'2'3'4", "1_2_3_4",
                "15e-23",
                "\r1", "\n\r1,4", "\n1,4\n",
                "\f\n1+2\t\r\v", "\n 1+\n2 ",
                "e+24", "1\"2",
                "(23)+(4)", "aef", "ABab",
                "A1a3",
                "FFFF",
                "0x1234", "0xab12", "0X1234",
                "AB12h", "BC34H",
                "1234u", "1234ul", "1234ULL",
            };
            string[] wordNegativeInput =
            {
                "+123", "1.23", "1''2", "'123", "123'", "1__2", "_123", "123_",
                "-133", "1.2e+023", "1.2e23",
                "xyz",
                "A1a3b",
                "SIN(2)", "123+-234", "1234x", "A0x1234", "0xx1234",
                "1234uu", "1234ulll",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerHexWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "13", "13+6", "1,4", "2 3", "1'2", "1_2",
                "5e-3",
                "\r1", "a", "ab", "A1",
                "0x12", "0xab", "0X12",
                "A9h", "B8H",
                "12u", "12ul", "12ULL",
            };
            string[] byteNegativeInput =
            {
                "+3", "1.2", "1''2", "'12", "12'", "1__2", "_12", "12_",
                "-3", "1.1e+02", "1.2e3",
                "xz",
                "A3a",
                "SIN(2)", "13+-23", "12x", "A0x1", "0xx12",
                "12uu", "12ulll",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerHexBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerHexBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateProgrammerBinPasteExpressionTest()
        {
            // QWord
            string[] qwordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "1010101010101010101010101011110110100100101010101001010101001010",
                "1+10=", "1+10=   ", "1001%10",
            };
            string[] qwordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "10101010101010101010101010111101101001001010101010010101010010100",
                "SIN(01010)", "10+-10101010101",
                "1+10==", "1=+10",
            };

            foreach (var data in qwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinQwordPasteExpression(data), data);
            foreach (var data in qwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinQwordPasteExpression(data), "NoOp");

            // DWord
            string[] dwordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "10101001001010101101010111111100",
            };
            string[] dwordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "101010010010101011010101111111001",
                "SIN(01010)", "10+-10101010101",
            };

            foreach (var data in dwordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinDwordPasteExpression(data), data);
            foreach (var data in dwordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinDwordPasteExpression(data), "NoOp");

            // Word
            string[] wordPositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\f\n1+11\t\r\v\x85", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "1010101010010010",
            };
            string[] wordNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "10101010100100101",
                "SIN(01010)", "10+-10101010101",
            };

            foreach (var data in wordPositiveInput)
                Assert.AreEqual(ValidateProgrammerBinWordPasteExpression(data), data);
            foreach (var data in wordNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinWordPasteExpression(data), "NoOp");

            // Byte
            string[] bytePositiveInput =
            {
                "100", "100+101", "1,001", "1 0 1", "1'0'0'1", "1_0_0_1",
                "\n\r1,010\n", "\n 1+\n1 ", "1\"1",
                "(101)+(10)",
                "0b1001", "0B1111", "0y1001", "0Y1001",
                "1100b", "1101B",
                "1111u", "1111ul", "1111ULL",
                "10100010", "11111111",
            };
            string[] byteNegativeInput =
            {
                "+10101", "1.01", "1''0", "'101", "101'", "1__0", "_101", "101_",
                "-10101001", "123", "1.2e23", "1.2e+023", "101010e-1010",
                "abcdef", "xyz", "ABab", "e+10101",
                "b1001", "10b01", "0x10", "1001x", "1001h",
                "0bb1111", "1111uu", "1111ulll",
                "101000101",
                "100000000",
                "SIN(01010)", "10+-1010101",
            };

            foreach (var data in bytePositiveInput)
                Assert.AreEqual(ValidateProgrammerBinBytePasteExpression(data), data);
            foreach (var data in byteNegativeInput)
                Assert.AreEqual(ValidateProgrammerBinBytePasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidateConverterPasteExpressionTest()
        {
            string[] positiveInput =
            {
                "123", "+123", "-133", "12345.", "012.012", "1,234", "1 2 3",
                "\n\r1,234\n", "\f\n12\t\r\v\x85", "1\"2", "100=", "100=   ",
            };
            string[] negativeInput =
            {
                "(123)+(456)", "1.2e23", "12345e-23",
                "\n 1+\n2 ", "123+456", "abcdef", "\n 1+\n2 ",
                "xyz", "ABab", "e+234",
                "12345678912345678",
                "SIN(2)", "123+-234", "100==", "=100",
            };

            foreach (var data in positiveInput)
                Assert.AreEqual(ValidateConverterPasteExpression(data), data);
            foreach (var data in negativeInput)
                Assert.AreEqual(ValidateConverterPasteExpression(data), "NoOp");
        }

        [TestMethod]
        public void ValidatePasteExpressionErrorStates()
        {
            string exp_TooLong = "";
            for (uint i = 0; i < CopyPasteManager.MaxPasteableLength / 8; i++)
            {
                exp_TooLong += "-1234567";
            }
            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    exp_TooLong, ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                exp_TooLong);
            exp_TooLong += "1";
            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    exp_TooLong, ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "", ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "1a23f456", ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");

            Assert.AreEqual(
                CopyPasteManager.ValidatePasteExpression(
                    "123", ViewMode.None, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown),
                "NoOp");
        }

        [TestMethod]
        public void ValidateExtractOperands()
        {
            var oneOperand = new List<string> { "123456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123456", ViewMode.Standard), oneOperand);

            oneOperand.Clear();
            oneOperand.Add("123^456");
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123^456", ViewMode.Standard), oneOperand);

            var twoOperands = new List<string> { "123", "456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123+456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123-456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123*456", ViewMode.Standard), twoOperands);
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123/456", ViewMode.Standard), twoOperands);

            var expOperand = new List<string> { "123e456" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123e456", ViewMode.Standard), expOperand);
            expOperand.Clear();
            expOperand.Add("123e4567");
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("123e4567", ViewMode.Standard), expOperand);

            var twoOperandsParens = new List<string> { "((45)", "(-30))" };
            AssertListsAreEqual(CopyPasteManager.ExtractOperands("((45)+(-30))", ViewMode.Scientific), twoOperandsParens);
        }

        [TestMethod]
        public void ValidateExtractOperandsErrors()
        {
            string exp_OperandLimit = "";
            for (uint i = 0; i < CopyPasteManager.MaxOperandCount; i++)
            {
                exp_OperandLimit += "+1";
            }
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode.Standard).Count,
                (uint)100);

            exp_OperandLimit += "+1";
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode.Standard).Count,
                (uint)0);

            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands("12e9999", ViewMode.Standard).Count,
                (uint)1);
            Assert.AreEqual(
                (uint)CopyPasteManager.ExtractOperands("12e10000", ViewMode.Standard).Count,
                (uint)0);
        }

        [TestMethod]
        public void ValidateExpressionRegExMatch()
        {
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string>(), ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123" },
                    ViewMode.None, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123" },
                    ViewMode.Currency, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown));

            // Verify operand lengths > max return false
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123456789012345678901234567890123" },
                ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567" },
                ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "11111111111111111" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.HexBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "12345678901234567890" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "11111111111111111111111" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.OctBase, BitLength.BitLengthQWord));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "10000000000000000000000000000000000000000000000000000000000000000" },
                ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.BinBase, BitLength.BitLengthQWord));

            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));

            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "((((((((((((((((((((123))))))))))))))))))))" },
                    ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "9223372036854775807" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "-9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));

            // Verify all operands must match patterns
            Assert.IsTrue(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "456" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsTrue(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "1e23" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(CopyPasteManager.ExpressionRegExMatch(
                new List<string> { "123", "fab10" },
                ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));

            Assert.IsTrue(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "1.23e+456", "1.23e456", ".23e+456", "123e-456", "12e2", "12e+2", "12e-2", "-12e2", "-12e+2", "-12e-2" },
                    ViewMode.Scientific, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));

            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123", "12345678901234567" },
                    ViewMode.Standard, CategoryGroupType.Calculator, NumberBase.Unknown, BitLength.BitLengthUnknown));
            Assert.IsFalse(
                CopyPasteManager.ExpressionRegExMatch(
                    new List<string> { "123", "9223372036854775808" },
                    ViewMode.Programmer, CategoryGroupType.Calculator, NumberBase.DecBase, BitLength.BitLengthQWord));
        }

        [TestMethod]
        public void ValidateGetMaxOperandLengthAndValue()
        {
            var result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.Standard, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxStandardOperandLength);
            Assert.IsTrue(result.MaxValue == 0);

            result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.Scientific, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxScientificOperandLength);
            Assert.IsTrue(result.MaxValue == 0);

            result = CopyPasteManager.GetMaxOperandLengthAndValue(
                ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == CopyPasteManager.MaxConverterInputLength);
            Assert.IsTrue(result.MaxValue == 0);

            ulong ullQwordMax = ulong.MaxValue;
            ulong ullDwordMax = uint.MaxValue;
            ulong ullWordMax = ushort.MaxValue;
            ulong ullByteMax = byte.MaxValue;

            // Hex
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 16u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 8u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 4u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.HexBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 2u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Dec
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 19u);
            Assert.IsTrue(result.MaxValue == ullQwordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 10u);
            Assert.IsTrue(result.MaxValue == ullDwordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 5u);
            Assert.IsTrue(result.MaxValue == ullWordMax >> 1);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.DecBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 3u);
            Assert.IsTrue(result.MaxValue == ullByteMax >> 1);

            // Oct
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 22u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 11u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 6u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.OctBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 3u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Bin
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthQWord);
            Assert.IsTrue(result.MaxLength == 64u);
            Assert.IsTrue(result.MaxValue == ullQwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthDWord);
            Assert.IsTrue(result.MaxLength == 32u);
            Assert.IsTrue(result.MaxValue == ullDwordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthWord);
            Assert.IsTrue(result.MaxLength == 16u);
            Assert.IsTrue(result.MaxValue == ullWordMax);
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.Programmer, CategoryGroupType.None, NumberBase.BinBase, BitLength.BitLengthByte);
            Assert.IsTrue(result.MaxLength == 8u);
            Assert.IsTrue(result.MaxValue == ullByteMax);

            // Invalid
            result = CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode.None, CategoryGroupType.None, NumberBase.Unknown, BitLength.BitLengthUnknown);
            Assert.IsTrue(result.MaxLength == 0u);
            Assert.IsTrue(result.MaxValue == 0UL);
        }

        [TestMethod]
        public void ValidateSanitizeOperand()
        {
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("((1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1234))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1234))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("-1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("+1234"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("-(1234)"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("+(1234)"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("12-34"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("((((1234))))"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1'2'3'4"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("'''''1234''''"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("1_2_3_4"), "1234");
            Assert.AreEqual(CopyPasteManager.SanitizeOperand("______1234___"), "1234");
        }

        [TestMethod]
        public void ValidatePrefixCurrencySymbols()
        {
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A5\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A4\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20B5\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u0024\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A1\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A9\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20AA\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20A6\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20B9\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u00A3\u0035"), "5");
            Assert.AreEqual(CopyPasteManager.RemoveUnwantedCharsFromString("\u20AC\u0035"), "5");
        }

        [TestMethod]
        public void ValidateTryOperandToULL()
        {
            ulong? result = null;

            // Hex
            result = CopyPasteManager.TryOperandToULL("1234", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0x1234UL);
            result = CopyPasteManager.TryOperandToULL("FF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFUL);
            result = CopyPasteManager.TryOperandToULL("FFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0xFFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0XFFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("0X0FFFFFFFFFFFFFFFF", NumberBase.HexBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Dec
            result = CopyPasteManager.TryOperandToULL("1234", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 1234UL);
            result = CopyPasteManager.TryOperandToULL("18446744073709551615", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("018446744073709551615", NumberBase.DecBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Oct
            result = CopyPasteManager.TryOperandToULL("777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 511UL); // 0777 octal
            result = CopyPasteManager.TryOperandToULL("0777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 511UL);
            result = CopyPasteManager.TryOperandToULL("1777777777777777777777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("01777777777777777777777", NumberBase.OctBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Bin
            result = CopyPasteManager.TryOperandToULL("1111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0b1111UL);
            result = CopyPasteManager.TryOperandToULL("0010", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0b10UL);
            result = CopyPasteManager.TryOperandToULL("1111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);
            result = CopyPasteManager.TryOperandToULL("01111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase);
            Assert.IsNotNull(result);
            Assert.AreEqual(result.Value, 0xFFFF_FFFF_FFFF_FFFF);

            // Invalid / overflow
            Assert.IsNull(CopyPasteManager.TryOperandToULL("0xFFFFFFFFFFFFFFFFF1", NumberBase.HexBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("18446744073709551616", NumberBase.DecBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("2000000000000000000000", NumberBase.OctBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("11111111111111111111111111111111111111111111111111111111111111111", NumberBase.BinBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("-1", NumberBase.DecBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("5555", NumberBase.BinBase));
            Assert.IsNull(CopyPasteManager.TryOperandToULL("xyz", NumberBase.BinBase));
        }

        [TestMethod]
        public void ValidateStandardScientificOperandLength()
        {
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength(""), (uint)0);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("0.2"), (uint)1);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("1.2"), (uint)2);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("0."), (uint)0);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("12345"), (uint)5);
            Assert.AreEqual(CopyPasteManager.StandardScientificOperandLength("-12345"), (uint)6);
        }

        [TestMethod]
        public void ValidateProgrammerOperandLength()
        {
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001b", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("1001B", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0b1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0B1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0y1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0Y1001", NumberBase.BinBase), (uint)4);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0b", NumberBase.BinBase), (uint)1);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0t123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0T123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0o123456", NumberBase.OctBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0O123456", NumberBase.OctBase), (uint)6);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("", NumberBase.DecBase), (uint)0);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("-", NumberBase.DecBase), (uint)0);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("-12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0n12345", NumberBase.DecBase), (uint)5);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0N12345", NumberBase.DecBase), (uint)5);

            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0x123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("0X123ABC", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABCh", NumberBase.HexBase), (uint)6);
            Assert.AreEqual(CopyPasteManager.ProgrammerOperandLength("123ABCH", NumberBase.HexBase), (uint)6);
        }

        #region Helper Methods

        private static void AssertListsAreEqual(IList<string> actual, IList<string> expected)
        {
            Assert.AreEqual(expected.Count, actual.Count);
            for (int i = 0; i < expected.Count; i++)
            {
                Assert.AreEqual(expected[i], actual[i]);
            }
        }

        private static string ValidateStandardPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Standard, NumberBase.Unknown, BitLength.BitLengthUnknown);
        }

        private static string ValidateScientificPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Scientific, NumberBase.Unknown, BitLength.BitLengthUnknown);
        }

        private static string ValidateConverterPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.None, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown);
        }

        private static string ValidateProgrammerHexQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerHexDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerHexWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerHexBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthByte);
        }

        private static string ValidateProgrammerDecQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerDecDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerDecWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerDecBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.DecBase, BitLength.BitLengthByte);
        }

        private static string ValidateProgrammerOctQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerOctDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerOctWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerOctBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.OctBase, BitLength.BitLengthByte);
        }

        private static string ValidateProgrammerBinQwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthQWord);
        }

        private static string ValidateProgrammerBinDwordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthDWord);
        }

        private static string ValidateProgrammerBinWordPasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthWord);
        }

        private static string ValidateProgrammerBinBytePasteExpression(string pastedText)
        {
            return CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode.Programmer, NumberBase.BinBase, BitLength.BitLengthByte);
        }

        #endregion
    }
}
