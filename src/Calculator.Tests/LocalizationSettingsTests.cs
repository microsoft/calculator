// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel.Common;
using Windows.Globalization.NumberFormatting;
using System.Collections.Generic;

namespace Calculator.Tests
{
    [TestClass]
    public class LocalizationSettingsTests
    {
        private static DecimalFormatter CreateDecimalFormatter(string language, string geographicRegion)
        {
            var languages = new List<string> { language };
            return new DecimalFormatter(languages, geographicRegion);
        }

        [TestMethod]
        public void TestLocaleName()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("en-US", settings.GetLocaleName());
        }

        [TestMethod]
        public void TestIsDigitEnUsSetting()
        {
            var arabicFormatter = CreateDecimalFormatter("ar-AE", "AE");
            var arabicSettings = new LocalizationSettings(arabicFormatter);
            Assert.IsFalse(arabicSettings.IsDigitEnUsSetting());

            var englishFormatter = CreateDecimalFormatter("en-US", "US");
            var englishSettings = new LocalizationSettings(englishFormatter);
            Assert.IsTrue(englishSettings.IsDigitEnUsSetting());
        }

        [TestMethod]
        public void TestLocalizeDisplayValue()
        {
            var formatter = CreateDecimalFormatter("ar-AE", "AE");
            var settings = new LocalizationSettings(formatter);
            string input = "A123";
            settings.LocalizeDisplayValue(ref input);
            Assert.AreEqual("A\u0661\u0662\u0663", input);
        }

        [TestMethod]
        public void TestGetEnglishValueFromLocalizedDigits()
        {
            var formatter = CreateDecimalFormatter("ar-AE", "AE");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("A123", settings.GetEnglishValueFromLocalizedDigits("A\u0661\u0662\u0663"));
        }

        [TestMethod]
        public void TestIsEnUsDigit()
        {
            var settings = LocalizationSettings.GetInstance();
            Assert.IsFalse(settings.IsEnUsDigit('/'));
            Assert.IsTrue(settings.IsEnUsDigit('0'));
            Assert.IsTrue(settings.IsEnUsDigit('1'));
            Assert.IsTrue(settings.IsEnUsDigit('8'));
            Assert.IsTrue(settings.IsEnUsDigit('9'));
            Assert.IsFalse(settings.IsEnUsDigit(':'));
        }

        [TestMethod]
        public void TestIsLocalizedDigit()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.IsTrue(settings.IsLocalizedDigit('0'));
            Assert.IsFalse(settings.IsLocalizedDigit('A'));
        }

        [TestMethod]
        public void TestIsLocalizedHexDigit()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.IsTrue(settings.IsLocalizedHexDigit('0'));
            Assert.IsTrue(settings.IsLocalizedHexDigit('A'));
            Assert.IsFalse(settings.IsLocalizedHexDigit('G'));
        }

        [TestMethod]
        public void TestRemoveGroupSeparators()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            Assert.AreEqual("1000000", settings.RemoveGroupSeparators("1,000 000"));
        }
    }
}
