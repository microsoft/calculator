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

        [TestMethod]
        public void TestNumberGroupingSystemCanBeChanged()
        {
            var formatter = CreateDecimalFormatter("en-US", "US");
            var settings = new LocalizationSettings(formatter);
            string groupingSeparator = settings.GetNumberGroupingSeparatorStr();

            Assert.AreEqual("3;0", settings.GetNumberGroupingStr());
            Assert.AreEqual("123,456,789", settings.ApplyNumberGrouping("123,456,789"));
            Assert.IsTrue(settings.SetNumberGroupingSystem(NumberGroupingSystem.Indian));
            Assert.AreEqual("3;2;0", settings.GetNumberGroupingStr());
            Assert.AreEqual(groupingSeparator, settings.GetNumberGroupingSeparatorStr());
            Assert.AreEqual("12,34,56,789.01", settings.ApplyNumberGrouping("123,456,789.01"));
            Assert.AreEqual("1,23,45,678", settings.ApplyNumberGrouping("12,345,678"));

            Assert.IsFalse(settings.SetNumberGroupingSystem(NumberGroupingSystem.Indian));
            Assert.IsTrue(settings.SetNumberGroupingSystem(NumberGroupingSystem.International));
            Assert.AreEqual("3;0", settings.GetNumberGroupingStr());
            Assert.AreEqual("123,456,789.01", settings.ApplyNumberGrouping("12,34,56,789.01"));
            Assert.AreEqual("12,345,678", settings.ApplyNumberGrouping("1,23,45,678"));
        }
    }
}
