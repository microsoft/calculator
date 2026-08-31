// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.DataLoaders;

namespace Calculator.Tests
{
    [TestClass]
    public class UnitPickerCatalogTests
    {
        private static Unit U(int id, string name, string abbreviation) =>
            new Unit(id, name, abbreviation, name, false);

        private static Category C(int id, string name) => new Category(id, name, false);

        [TestMethod]
        public void Build_MapsEachCategoryToItsUnitsInOrder()
        {
            var categories = new List<Category> { C(1, "Length"), C(2, "Weight") };
            var units = new List<IReadOnlyList<Unit>>
            {
                new[] { U(10, "Meter", "m"), U(11, "Kilometer", "km") },
                new[] { U(20, "Gram", "g") },
            };

            var catalog = UnitPickerCatalog.Build(categories, units);

            Assert.AreEqual(2, catalog.Count);
            Assert.AreEqual("Length", catalog[0].Category.Name);
            Assert.AreEqual(2, catalog[0].Units.Count);
            Assert.AreEqual("Meter", catalog[0].Units[0].Name);
            Assert.AreEqual("Kilometer", catalog[0].Units[1].Name);
            Assert.AreEqual("Weight", catalog[1].Category.Name);
            Assert.AreEqual(1, catalog[1].Units.Count);
            Assert.AreEqual("Gram", catalog[1].Units[0].Name);
        }

        [TestMethod]
        public void Build_SkipsNullCategoryEntries()
        {
            var categories = new List<Category> { null, C(2, "Weight") };
            var units = new List<IReadOnlyList<Unit>>
            {
                new[] { U(99, "Ghost", "x") },
                new[] { U(20, "Gram", "g") },
            };

            var catalog = UnitPickerCatalog.Build(categories, units);

            Assert.AreEqual(1, catalog.Count);
            Assert.AreEqual("Weight", catalog[0].Category.Name);
            Assert.AreEqual("Gram", catalog[0].Units[0].Name);
        }

        [TestMethod]
        public void Build_ToleratesMissingOrNullUnitLists()
        {
            var categories = new List<Category> { C(1, "Length"), C(2, "Weight"), C(3, "Currency") };
            var units = new List<IReadOnlyList<Unit>>
            {
                new[] { U(10, "Meter", "m") },
                null,
            };

            var catalog = UnitPickerCatalog.Build(categories, units);

            Assert.AreEqual(3, catalog.Count);
            Assert.AreEqual(1, catalog[0].Units.Count);
            Assert.AreEqual(0, catalog[1].Units.Count);
            Assert.AreEqual(0, catalog[2].Units.Count);
        }

        [TestMethod]
        public void CurrencyUnitWrapper_UsesDisambiguatedDisplayName()
        {
            var currency = new CurrencyUnit
            {
                Id = 1,
                Name = "Euro",
                CountryName = "France",
                Abbreviation = "EUR",
                IsRtlLanguage = false,
            };

            var unit = UnitConverterDataLoader.CreateCurrencyUnitWrapper(currency);

            Assert.AreEqual("France - Euro", unit.Name);
            Assert.AreEqual("France Euro", unit.AccessibleName);
            Assert.AreEqual("EUR", unit.Abbreviation);
        }

        [TestMethod]
        public void CurrencyUnitWrapper_ReversesDisplayNameForRtl()
        {
            var currency = new CurrencyUnit
            {
                Id = 1,
                Name = "Currency",
                CountryName = "Country",
                Abbreviation = "CUR",
                IsRtlLanguage = true,
            };

            var unit = UnitConverterDataLoader.CreateCurrencyUnitWrapper(currency);

            Assert.AreEqual("Currency - Country", unit.Name);
            Assert.AreEqual("Currency Country", unit.AccessibleName);
        }
    }
}
