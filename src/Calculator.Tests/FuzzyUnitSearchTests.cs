// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;

namespace Calculator.Tests
{
    [TestClass]
    public class FuzzyUnitSearchTests
    {
        private static Unit U(int id, string name, string abbreviation) =>
            new Unit(id, name, abbreviation, name, false);

        private static UnitPickerCategory Cat(int id, string name, params Unit[] units) =>
            new UnitPickerCategory(new Category(id, name, false), units);

        private static UnitPickerViewModel Picker() => new UnitPickerViewModel(new[]
        {
            Cat(1, "Length", U(10, "Centimeters", "cm"), U(11, "Kilometers", "km"), U(12, "Nautical miles", "nmi")),
            Cat(2, "Temperature", U(20, "Celsius", "\u00b0C"), U(21, "Fahrenheit", "\u00b0F")),
            Cat(3, "Energy", U(30, "Kilowatt-hours", "kWh"), U(31, "Joules", "J")),
            Cat(4, "Volume", U(40, "Cubic meters", "m\u00b3")),
        });

        private static string[] Names(UnitPickerViewModel vm) =>
            vm.FilteredUnits.Select(item => item.Unit.Name).ToArray();

        [TestMethod]
        public void MisspelledUnitIsStillFound()
        {
            var vm = Picker();

            vm.SearchText = "celcius";

            CollectionAssert.Contains(Names(vm), "Celsius", "A common misspelling should not lose the unit.");
        }

        [TestMethod]
        public void MissingLetterIsStillFound()
        {
            var vm = Picker();

            vm.SearchText = "farenheit";

            CollectionAssert.Contains(Names(vm), "Fahrenheit");
        }

        [TestMethod]
        public void MisspelledUnitPrefixIsFoundBelowTruePrefix()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(
                    1,
                    "Length",
                    U(10, "Centima units", "cu"),
                    U(11, "Centimeters", "cm")),
            });

            vm.SearchText = "centima";

            CollectionAssert.AreEqual(
                new[] { "Centima units", "Centimeters" },
                Names(vm));
        }

        [TestMethod]
        public void MisspelledUnitPrefixDoesNotBroadenOtherFields()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Unrelated", U(10, "Meters", "centimeters")),
                Cat(2, "Centimeters", U(20, "Feet", "ft")),
            });

            vm.SearchText = "centima";

            Assert.AreEqual(0, vm.FilteredUnits.Count);
        }

        [TestMethod]
        public void InitialsOfAMultiWordUnitAreFound()
        {
            var vm = Picker();

            vm.SearchText = "kwh";

            CollectionAssert.Contains(Names(vm), "Kilowatt-hours");
        }

        [TestMethod]
        public void SkippedLettersStillFindTheUnit()
        {
            var vm = Picker();

            vm.SearchText = "cbcmtr";

            CollectionAssert.Contains(Names(vm), "Cubic meters");
        }

        [TestMethod]
        public void OrdinallyDistinctCharactersDoNotMatchAsSubsequence()
        {
            var matcher = new FuzzyUnitSearchMatcher();
            var item = new UnitPickerItem(U(50, "k-x-y", string.Empty), "Unrelated", 1);

            Assert.AreEqual(0, matcher.Rank("\u212Axy", item));
        }

        [TestMethod]
        public void OrdinallyDistinctCharactersCountAsEditDistanceChanges()
        {
            var matcher = new FuzzyUnitSearchMatcher();
            var item = new UnitPickerItem(U(50, "kxbc", string.Empty), "Unrelated", 1);

            Assert.AreEqual(0, matcher.Rank("\u212Aabc", item));
        }

        [TestMethod]
        public void SubsequenceDoesNotCombineSurrogateHalves()
        {
            var matcher = new FuzzyUnitSearchMatcher();
            string query = char.ConvertFromUtf32(0x10000) + "xy";
            string value =
                char.ConvertFromUtf32(0x10001) + char.ConvertFromUtf32(0x10400) + "x-y";
            var item = new UnitPickerItem(U(50, value, string.Empty), "Unrelated", 1);

            Assert.AreEqual(0, matcher.Rank(query, item));
        }

        [TestMethod]
        public void SupplementaryCasePairMatchesAsSubsequence()
        {
            var matcher = new FuzzyUnitSearchMatcher();
            string query = char.ConvertFromUtf32(0x10428) + "xy";
            string value = char.ConvertFromUtf32(0x10400) + "-x-y";
            var item = new UnitPickerItem(U(50, value, string.Empty), "Unrelated", 1);

            Assert.IsTrue(matcher.Rank(query, item) > 0);
        }

        [TestMethod]
        public void SearchingACategoryNameSurfacesItsUnits()
        {
            var vm = Picker();

            vm.SearchText = "temperature";

            var names = Names(vm);
            CollectionAssert.Contains(names, "Celsius");
            CollectionAssert.Contains(names, "Fahrenheit");
        }

        [TestMethod]
        public void BestMatchIsRankedFirst()
        {
            var vm = Picker();

            // "Kilometers" starts with the query; "Kilowatt-hours" only shares a prefix of it.
            vm.SearchText = "kilom";

            Assert.AreEqual("Kilometers", Names(vm).First());
        }

        [TestMethod]
        public void ExactAbbreviationOutranksAPartialNameMatch()
        {
            var vm = Picker();

            vm.SearchText = "cm";

            Assert.AreEqual("Centimeters", Names(vm).First());
        }

        [TestMethod]
        public void NamePrefixOutranksSelectedCategoryAbbreviationPrefix()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Area", U(10, "Square centimeters", "cm\u00b2")),
                Cat(2, "Length", U(20, "Centimeters", "cm")),
                Cat(3, "Volume", U(30, "Cubic centimeters", "cm\u00b3")),
            }, matcher: null, selectedCategoryId: 1);

            vm.SearchText = "c";

            CollectionAssert.AreEqual(
                new[] { "Centimeters", "Cubic centimeters", "Square centimeters" },
                Names(vm));
        }

        [TestMethod]
        public void NonsenseStillMatchesNothing()
        {
            var vm = Picker();

            vm.SearchText = "zzzqqq";

            Assert.AreEqual(0, vm.FilteredUnits.Count);
            Assert.IsTrue(vm.HasNoSearchResults);
        }

        [TestMethod]
        public void ShortQueryDoesNotDragInLooseMatches()
        {
            var vm = Picker();

            // Two characters are almost always an abbreviation. Allowing gaps at this length would
            // match most of the catalogue and make the list useless.
            vm.SearchText = "km";

            CollectionAssert.AreEqual(new[] { "Kilometers" }, Names(vm));
        }

        // Building this walks every category and holds a native engine, so catalogue-only tests
        // share one rather than each standing up their own.
        internal static IReadOnlyList<UnitPickerCategory> RealCatalog => s_realCatalog.Value;

        private static readonly Lazy<IReadOnlyList<UnitPickerCategory>> s_realCatalog =
            new Lazy<IReadOnlyList<UnitPickerCategory>>(() =>
            {
                var converter = new UnitConverterViewModel();
                var categories = converter.Categories.ToList();
                var unitsByCategory = new List<IReadOnlyList<Unit>>();
                foreach (var category in categories)
                {
                    converter.CurrentCategory = category;
                    unitsByCategory.Add(converter.Units.ToList());
                }

                return UnitPickerCatalog.Build(categories, unitsByCategory);
            });

        // The cases above use a small hand-built catalogue for readability; this one runs the same
        // search over the app's real unit list, where the loose strategies have far more to go wrong on.
        [TestMethod]
        public void RealCatalogSearchFindsTheExpectedUnit()
        {
            var picker = new UnitPickerViewModel(RealCatalog);

            // A misspelling, a truncation, and a category name. Each must put the obvious unit first.
            AssertTopResultContains(picker, "celcius", "Celsius");
            AssertTopResultContains(picker, "farenheit", "Fahrenheit");
            AssertTopResultContains(picker, "centimet", "Centimet");
            AssertTopResultContains(picker, "centima", "Centimet");

            picker.SearchText = "zzzqqq";
            Assert.AreEqual(0, picker.FilteredUnits.Count, "Nonsense matched something in the real catalogue.");
        }

        private static void AssertTopResultContains(UnitPickerViewModel picker, string query, string expected)
        {
            picker.SearchText = query;

            Assert.IsTrue(
                picker.FilteredUnits.Count > 0,
                $"'{query}' returned nothing.");
            Assert.IsTrue(
                picker.FilteredUnits[0].Unit.Name.IndexOf(expected, StringComparison.OrdinalIgnoreCase) >= 0,
                $"'{query}' ranked '{picker.FilteredUnits[0].Unit.Name}' first, expected a '{expected}' unit.");
        }
    }
}
