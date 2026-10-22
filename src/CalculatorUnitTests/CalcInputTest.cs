// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Linq;
using CalcManager.CEngine;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace CalculatorEngineTests
{
    [TestClass]
    public class CalcInputTest
    {
        private CalcInput m_calcInput;

        [TestInitialize]
        public void CommonSetup()
        {
            m_calcInput = new CalcInput('.');
        }

        [TestCleanup]
        public void Cleanup()
        {
            m_calcInput.Clear();
            m_calcInput.SetDecimalSymbol('.');
        }

        [TestMethod]
        public void Clear()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryToggleSign(false, "999");
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(3, 10, false, "999", 64, 32);

            Assert.AreEqual("-1.2e+3", m_calcInput.ToString(10), "Verify input is correct.");

            m_calcInput.Clear();

            Assert.AreEqual("0", m_calcInput.ToString(10), "Verify input is 0 after clear.");
        }

        [TestMethod]
        public void TryToggleSignZero()
        {
            Assert.IsTrue(m_calcInput.TryToggleSign(false, "999"), "Verify toggling 0 succeeds.");
            Assert.AreEqual("0", m_calcInput.ToString(10), "Verify toggling 0 does not create -0.");
        }

        [TestMethod]
        public void TryToggleSignExponent()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            Assert.IsTrue(m_calcInput.TryToggleSign(false, "999"), "Verify toggling exponent sign succeeds.");
            Assert.AreEqual("1.e-2", m_calcInput.ToString(10), "Verify toggling exponent sign does not toggle base sign.");
            Assert.IsTrue(m_calcInput.TryToggleSign(false, "999"), "Verify toggling exponent sign succeeds.");
            Assert.AreEqual("1.e+2", m_calcInput.ToString(10), "Verify toggling negative exponent sign does not toggle base sign.");
        }

        [TestMethod]
        public void TryToggleSignBase()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            Assert.IsTrue(m_calcInput.TryToggleSign(false, "999"), "Verify toggling base sign succeeds.");
            Assert.AreEqual("-1", m_calcInput.ToString(10), "Verify toggling base sign creates negative base.");
            Assert.IsTrue(m_calcInput.TryToggleSign(false, "999"), "Verify toggling base sign succeeds.");
            Assert.AreEqual("1", m_calcInput.ToString(10), "Verify toggling negative base sign creates positive base.");
        }

        [TestMethod]
        public void TryToggleSignBaseIntegerMode()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            Assert.IsTrue(m_calcInput.TryToggleSign(true, "999"), "Verify toggling base sign in integer mode succeeds.");
            Assert.AreEqual("-1", m_calcInput.ToString(10), "Verify toggling base sign creates negative base.");
        }

        [TestMethod]
        public void TryToggleSignRollover()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            Assert.IsTrue(m_calcInput.TryToggleSign(true, "127"), "Verify toggling base sign in integer mode succeeds.");
            m_calcInput.TryAddDigit(8, 10, false, "999", 64, 32);
            Assert.IsFalse(m_calcInput.TryToggleSign(true, "127"), "Verify toggling base sign in integer mode fails on rollover.");
            Assert.AreEqual("-128", m_calcInput.ToString(10), "Verify toggling base sign on rollover does not change value.");
        }

        [TestMethod]
        public void TryAddDigitLeadingZeroes()
        {
            Assert.IsTrue(m_calcInput.TryAddDigit(0, 10, false, "999", 64, 32), "Verify TryAddDigit succeeds.");
            Assert.IsTrue(m_calcInput.TryAddDigit(0, 10, false, "999", 64, 32), "Verify TryAddDigit succeeds.");
            Assert.IsTrue(m_calcInput.TryAddDigit(0, 10, false, "999", 64, 32), "Verify TryAddDigit succeeds.");
            Assert.AreEqual("0", m_calcInput.ToString(10), "Verify leading zeros are ignored.");
        }

        [TestMethod]
        public void TryAddDigitMaxCount()
        {
            Assert.IsTrue(m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32), "Verify TryAddDigit for base with length < maxDigits succeeds.");
            Assert.AreEqual("1", m_calcInput.ToString(10), "Verify adding digit for base with length < maxDigits succeeded.");
            Assert.IsFalse(m_calcInput.TryAddDigit(2, 10, false, "999", 64, 1), "Verify TryAddDigit for base with length > maxDigits fails.");
            Assert.AreEqual("1", m_calcInput.ToString(10), "Verify digit for base was not added.");
            m_calcInput.TryBeginExponent();
            Assert.IsTrue(m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32), "Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            Assert.IsTrue(m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32), "Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            Assert.IsTrue(m_calcInput.TryAddDigit(3, 10, false, "999", 64, 32), "Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            Assert.IsTrue(m_calcInput.TryAddDigit(4, 10, false, "999", 64, 32), "Verify TryAddDigit for exponent with length < maxDigits succeeds.");
            Assert.IsFalse(m_calcInput.TryAddDigit(5, 10, false, "999", 64, 32), "Verify TryAddDigit for exponent with length > maxDigits fails.");
            Assert.AreEqual("1.e+1234", m_calcInput.ToString(10), "Verify adding digits for exponent with length < maxDigits succeeded.");

            m_calcInput.Clear();
            m_calcInput.TryAddDecimalPt();
            Assert.IsTrue(m_calcInput.TryAddDigit(1, 10, false, "999", 64, 1), "Verify decimal point and leading zero does not count toward maxDigits.");
            Assert.AreEqual("0.1", m_calcInput.ToString(10), "Verify input value checking dec pt and leading zero impact on maxDigits.");
        }

        [TestMethod]
        public void TryAddDigitValues()
        {
            for (uint i = 0; i < 25; i++)
            {
                Assert.IsTrue(m_calcInput.TryAddDigit(i, 10, false, "999", 64, 32), $"Verify TryAddDigit succeeds for {i}");
                m_calcInput.Clear();
            }
        }

        [TestMethod]
        public void TryAddDigitRolloverBaseCheck()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(2, 16, true, "999", 64, 1), "Verify TryAddDigit rollover fails for bases other than 8,10.");
            Assert.IsFalse(m_calcInput.TryAddDigit(1, 2, true, "999", 64, 1), "Verify TryAddDigit rollover fails for bases other than 8,10.");
        }

        [TestMethod]
        public void TryAddDigitRolloverOctalByte()
        {
            m_calcInput.TryAddDigit(1, 8, true, "777", 64, 32);
            Assert.IsTrue(m_calcInput.TryAddDigit(2, 8, true, "377", 8, 1), "Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, "777", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(2, 8, true, "377", 8, 1), "Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }

        [TestMethod]
        public void TryAddDigitRolloverOctalWord()
        {
            m_calcInput.TryAddDigit(1, 8, true, "777", 64, 32);
            Assert.IsTrue(m_calcInput.TryAddDigit(2, 8, true, "377", 16, 1), "Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, "777", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(2, 8, true, "377", 16, 1), "Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }

        [TestMethod]
        public void TryAddDigitRolloverOctalDword()
        {
            m_calcInput.TryAddDigit(1, 8, true, "777", 64, 32);
            Assert.IsTrue(m_calcInput.TryAddDigit(2, 8, true, "377", 32, 1), "Verify we can add an extra digit in OctalByte if first digit <= 3.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(4, 8, true, "777", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(2, 8, true, "377", 32, 1), "Verify we cannot add an extra digit in OctalByte if first digit > 3.");
        }

        [TestMethod]
        public void TryAddDigitRolloverOctalQword()
        {
            m_calcInput.TryAddDigit(1, 8, true, "777", 64, 32);
            Assert.IsTrue(m_calcInput.TryAddDigit(2, 8, true, "377", 64, 1), "Verify we can add an extra digit in OctalByte if first digit == 1.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(2, 8, true, "777", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(2, 8, true, "377", 64, 1), "Verify we cannot add an extra digit in OctalByte if first digit > 1.");
        }

        [TestMethod]
        public void TryAddDigitRolloverDecimal()
        {
            m_calcInput.TryAddDigit(1, 10, true, "127", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(0, 10, true, "1", 8, 1), "Verify we cannot add a digit if input size matches maxStr size.");
            m_calcInput.TryAddDigit(2, 10, true, "127", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(2, 10, true, "110", 8, 2), "Verify we cannot add a digit if n char comparison > 0.");
            Assert.IsTrue(m_calcInput.TryAddDigit(7, 10, true, "130", 8, 2), "Verify we can add a digit if n char comparison < 0.");

            m_calcInput.Clear();
            m_calcInput.TryAddDigit(1, 10, true, "127", 64, 32);
            m_calcInput.TryAddDigit(2, 10, true, "127", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDigit(8, 10, true, "127", 8, 2), "Verify we cannot add a digit if digit exceeds max value.");
            Assert.IsTrue(m_calcInput.TryAddDigit(7, 10, true, "127", 8, 2), "Verify we can add a digit if digit does not exceed max value.");

            m_calcInput.Backspace();
            m_calcInput.TryToggleSign(true, "127");
            Assert.IsFalse(m_calcInput.TryAddDigit(9, 10, true, "127", 8, 2), "Negative value: verify we cannot add a digit if digit exceeds max value.");
            Assert.IsTrue(m_calcInput.TryAddDigit(8, 10, true, "127", 8, 2), "Negative value: verify we can add a digit if digit does not exceed max value.");
        }

        [TestMethod]
        public void TryAddDecimalPtEmpty()
        {
            Assert.IsFalse(m_calcInput.HasDecimalPt(), "Verify input has no decimal point.");
            Assert.IsTrue(m_calcInput.TryAddDecimalPt(), "Verify adding decimal to empty input.");
            Assert.IsTrue(m_calcInput.HasDecimalPt(), "Verify input has decimal point.");
            Assert.AreEqual("0.", m_calcInput.ToString(10), "Verify decimal on empty input.");
        }

        [TestMethod]
        public void TryAddDecimalPointTwice()
        {
            Assert.IsFalse(m_calcInput.HasDecimalPt(), "Verify input has no decimal point.");
            Assert.IsTrue(m_calcInput.TryAddDecimalPt(), "Verify adding decimal to empty input.");
            Assert.IsTrue(m_calcInput.HasDecimalPt(), "Verify input has decimal point.");
            Assert.IsFalse(m_calcInput.TryAddDecimalPt(), "Verify adding decimal point fails if input has decimal point.");
        }

        [TestMethod]
        public void TryAddDecimalPointExponent()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            Assert.IsFalse(m_calcInput.TryAddDecimalPt(), "Verify adding decimal point fails if input has exponent.");
        }

        [TestMethod]
        public void TryBeginExponentNoExponent()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            Assert.IsTrue(m_calcInput.TryBeginExponent(), "Verify adding exponent succeeds on input without exponent.");
            Assert.AreEqual("1.e+0", m_calcInput.ToString(10), "Verify exponent present.");
        }

        [TestMethod]
        public void TryBeginExponentWithExponent()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            Assert.IsTrue(m_calcInput.TryBeginExponent(), "Verify adding exponent succeeds on input without exponent.");
            Assert.IsFalse(m_calcInput.TryBeginExponent(), "Verify cannot add exponent if input already has exponent.");
        }

        [TestMethod]
        public void BackspaceZero()
        {
            m_calcInput.Backspace();
            Assert.AreEqual("0", m_calcInput.ToString(10), "Verify backspace on 0 is still 0.");
        }

        [TestMethod]
        public void BackspaceSingleChar()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            Assert.AreEqual("1", m_calcInput.ToString(10), "Verify input before backspace.");
            m_calcInput.Backspace();
            Assert.AreEqual("0", m_calcInput.ToString(10), "Verify input after backspace.");
        }

        [TestMethod]
        public void BackspaceMultiChar()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            Assert.AreEqual("12", m_calcInput.ToString(10), "Verify input before backspace.");
            m_calcInput.Backspace();
            Assert.AreEqual("1", m_calcInput.ToString(10), "Verify input after backspace.");
        }

        [TestMethod]
        public void BackspaceDecimal()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            Assert.AreEqual("1.", m_calcInput.ToString(10), "Verify input before backspace.");
            Assert.IsTrue(m_calcInput.HasDecimalPt(), "Verify input has decimal point.");
            m_calcInput.Backspace();
            Assert.AreEqual("1", m_calcInput.ToString(10), "Verify input after backspace.");
            Assert.IsFalse(m_calcInput.HasDecimalPt(), "Verify decimal point was removed.");
        }

        [TestMethod]
        public void BackspaceMultiCharDecimal()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, "999", 64, 32);
            Assert.AreEqual("1.23", m_calcInput.ToString(10), "Verify input before backspace.");
            m_calcInput.Backspace();
            Assert.AreEqual("1.2", m_calcInput.ToString(10), "Verify input after backspace.");
        }

        [TestMethod]
        public void BackspaceZeroDecimalWithoutPrefixZeros()
        {
            m_calcInput.TryAddDigit(0, 10, false, "999", 64, 32);
            m_calcInput.TryAddDecimalPt();
            Assert.AreEqual("0.", m_calcInput.ToString(10), "Verify input before backspace.");
            m_calcInput.Backspace();
            m_calcInput.TryAddDigit(0, 10, false, "999", 64, 32);
            Assert.AreEqual("0", m_calcInput.ToString(10), "Verify input after backspace.");
        }

        [TestMethod]
        public void SetDecimalSymbol()
        {
            m_calcInput.TryAddDecimalPt();
            Assert.AreEqual("0.", m_calcInput.ToString(10), "Verify default decimal point.");
            m_calcInput.SetDecimalSymbol(',');
            Assert.AreEqual("0,", m_calcInput.ToString(10), "Verify new decimal point.");
        }

        [TestMethod]
        public void ToStringEmpty()
        {
            Assert.AreEqual("0", m_calcInput.ToString(10), "Verify ToString of empty value.");
        }

        [TestMethod]
        public void ToStringNegative()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryToggleSign(false, "999");
            Assert.AreEqual("-1", m_calcInput.ToString(10), "Verify ToString of negative value.");
        }

        [TestMethod]
        public void ToStringExponentBase10()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryBeginExponent();
            Assert.AreEqual("1.e+0", m_calcInput.ToString(10), "Verify ToString of empty base10 exponent.");
        }

        [TestMethod]
        public void ToStringExponentBase8()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryBeginExponent();
            Assert.AreEqual("1.^+0", m_calcInput.ToString(8), "Verify ToString of empty base8 exponent.");
        }

        [TestMethod]
        public void ToStringExponentNegative()
        {
            m_calcInput.TryAddDigit(1, 8, false, "999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryToggleSign(false, "999");
            Assert.AreEqual("1.e-0", m_calcInput.ToString(10), "Verify ToString of empty negative exponent.");
        }

        [TestMethod]
        public void ToStringExponentPositive()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryBeginExponent();
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, "999", 64, 32);
            m_calcInput.TryAddDigit(4, 10, false, "999", 64, 32);
            Assert.AreEqual("1.e+234", m_calcInput.ToString(10), "Verify ToString of exponent with value.");
        }

        [TestMethod]
        public void ToStringInteger()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            Assert.AreEqual("1", m_calcInput.ToString(10), "Verify ToString of integer value hides decimal.");
        }

        [TestMethod]
        public void ToRational()
        {
            m_calcInput.TryAddDigit(1, 10, false, "999", 64, 32);
            m_calcInput.TryAddDigit(2, 10, false, "999", 64, 32);
            m_calcInput.TryAddDigit(3, 10, false, "999", 64, 32);
            Assert.AreEqual("123", m_calcInput.ToString(10), "Verify input before conversion to rational.");

            var rat = m_calcInput.ToRational(10, 32);
            Assert.AreEqual(1, rat.P.Mantissa.Count, "Verify digit count of rational.");
            Assert.AreEqual((uint)123, rat.P.Mantissa.First(), "Verify first digit of mantissa.");
        }
    }
}
