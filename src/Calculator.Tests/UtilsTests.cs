// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class UtilsTests
    {
        [TestMethod]
        public void IsLastCharacterSuccess()
        {
            Assert.IsTrue(Utils.IsLastCharacterTarget("Test.", '.'));
        }

        [TestMethod]
        public void IsLastCharacterSuccessMultipleSuffices()
        {
            Assert.IsTrue(Utils.IsLastCharacterTarget("Test..", '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailure()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget("Test", '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailureAllButLastMatch()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(".....T", '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailureEmptyInput()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(string.Empty, '.'));
        }

        [TestMethod]
        public void IsLastCharacterFailureNullTarget()
        {
            Assert.IsFalse(Utils.IsLastCharacterTarget(string.Empty, '\0'));
        }
    }
}
