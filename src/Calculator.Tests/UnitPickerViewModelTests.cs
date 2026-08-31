// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;

namespace Calculator.Tests
{
    // Mirrors the category-first unit picker (Design1Page mockup): the selected category drives
    // the unit list; a query switches to a flat cross-category search and dims the category list.
    [TestClass]
    public class UnitPickerViewModelTests
    {
        private static Unit U(int id, string name, string abbreviation) =>
            new Unit(id, name, abbreviation, name, false);

        private static UnitPickerCategory Cat(int id, string name, params Unit[] units) =>
            new UnitPickerCategory(new Category(id, name, false), units);

        private static UnitPickerCategory Cat(int id, string name, string glyph, params Unit[] units) =>
            new UnitPickerCategory(new Category(id, name, false), units, glyph);

        private sealed class ExactNameMatcher : IUnitSearchMatcher
        {
            public bool IsMatch(string query, Unit unit) =>
                string.Equals(unit?.Name, query, StringComparison.OrdinalIgnoreCase);
        }

        [TestMethod]
        public void Default_ShowsSelectedCategoryUnitsOnly()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
                Cat(2, "Weight", U(20, "Gram", "g")),
            });

            Assert.AreEqual(2, vm.FilteredUnits.Count);
            Assert.AreEqual("Meter", vm.FilteredUnits[0].Unit.Name);
            Assert.AreEqual("Length", vm.FilteredUnits[0].CategoryName);
            Assert.AreEqual(1, vm.FilteredUnits[0].CategoryId);
            Assert.AreEqual("Kilometer", vm.FilteredUnits[1].Unit.Name);
        }

        [TestMethod]
        public void PreselectsCategoryById()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m")),
                Cat(2, "Weight", U(20, "Gram", "g")),
            }, matcher: null, selectedCategoryId: 2);

            Assert.AreEqual("Weight", vm.SelectedCategory.Name);
            Assert.AreEqual(1, vm.FilteredUnits.Count);
            Assert.AreEqual("Gram", vm.FilteredUnits[0].Unit.Name);
        }

        [TestMethod]
        public void PreselectsCurrentUnitById()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
            }, matcher: null, selectedCategoryId: 1, selectedUnitId: 11);

            Assert.IsNotNull(vm.SelectedUnit);
            Assert.AreEqual(11, vm.SelectedUnit.Unit.ModelUnitID());
        }

        [TestMethod]
        public void CategoryPreviewHidesAndRestoresCurrentUnitSelection()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
                Cat(2, "Weight", U(20, "Gram", "g")),
            }, matcher: null, selectedCategoryId: 1, selectedUnitId: 11);

            vm.SelectedCategory = vm.Categories[1];
            Assert.IsNull(vm.SelectedUnit);

            vm.SelectedCategory = vm.Categories[0];
            Assert.AreEqual(11, vm.SelectedUnit.Unit.ModelUnitID());
        }

        [TestMethod]
        public void SelectCategory_RebuildsUnits()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
                Cat(2, "Weight", U(20, "Gram", "g")),
            });

            Assert.AreEqual(2, vm.FilteredUnits.Count);

            vm.SelectedCategory = vm.Categories[1];

            Assert.AreEqual(1, vm.FilteredUnits.Count);
            Assert.AreEqual("Gram", vm.FilteredUnits[0].Unit.Name);
        }

        [TestMethod]
        public void SelectCategoryDuringSearch_KeepsSearchResults()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Foot", "ft")),
                Cat(2, "Weight", U(20, "Gram", "g")),
            });

            vm.SearchText = "foot";
            Assert.AreEqual(1, vm.FilteredUnits.Count);

            // The category list is disabled while searching; even if a selection slips through it
            // must not clobber the active cross-category search results.
            vm.SelectedCategory = vm.Categories[1];

            Assert.AreEqual(1, vm.FilteredUnits.Count);
            Assert.AreEqual("Foot", vm.FilteredUnits[0].Unit.Name);
            Assert.AreEqual("Weight", vm.SelectedCategory.Name);
        }

        [TestMethod]
        public void Search_IsFlatAcrossCategories()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
                Cat(2, "Weight", U(20, "Gram", "g")),
            });

            vm.SearchText = "GRAM";

            Assert.AreEqual(1, vm.FilteredUnits.Count);
            Assert.AreEqual("Gram", vm.FilteredUnits[0].Unit.Name);
            Assert.AreEqual("Weight", vm.FilteredUnits[0].CategoryName);
        }

        [TestMethod]
        public void SearchByAbbreviation_Matches()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
            });

            vm.SearchText = "km";

            Assert.AreEqual(1, vm.FilteredUnits.Count);
            Assert.AreEqual("Kilometer", vm.FilteredUnits[0].Unit.Name);
        }

        [TestMethod]
        public void Search_DimsAndDisablesCategoryList()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m")),
            });

            Assert.IsTrue(vm.CategoryListEnabled);

            vm.SearchText = "m";
            Assert.IsFalse(vm.CategoryListEnabled);

            vm.SearchText = string.Empty;
            Assert.IsTrue(vm.CategoryListEnabled);
        }

        [TestMethod]
        public void SearchNoMatch_ClearsResults_ThenClearRestores()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
            });

            Assert.IsTrue(vm.HasResults);

            vm.SearchText = "zzz";
            Assert.AreEqual(0, vm.FilteredUnits.Count);
            Assert.IsFalse(vm.HasResults);
            Assert.IsTrue(vm.HasNoSearchResults);

            vm.SearchText = string.Empty;
            Assert.AreEqual(2, vm.FilteredUnits.Count);
            Assert.IsFalse(vm.HasNoSearchResults);
        }

        [TestMethod]
        public void EmptyCategoryWithoutSearchIsNotNoResults()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Currency"),
            });

            Assert.IsFalse(vm.HasResults);
            Assert.IsFalse(vm.HasNoSearchResults);
        }

        [TestMethod]
        public void LoadingCategory_ShowsLoadingInsteadOfNoResults()
        {
            var vm = new UnitPickerViewModel(
                new[] { Cat(1, "Currency") },
                matcher: null,
                selectedCategoryId: 1,
                selectedUnitId: -1,
                categoryLoadStates: new Dictionary<int, UnitPickerCategoryLoadState>
                {
                    [1] = UnitPickerCategoryLoadState.Loading,
                });

            Assert.IsTrue(vm.IsSelectedCategoryLoading);
            Assert.IsFalse(vm.HasSelectedCategoryLoadFailed);
            Assert.IsFalse(vm.AreUnitsVisible);
            Assert.IsFalse(vm.HasNoSearchResults);
        }

        [TestMethod]
        public void LoadingCategory_RefreshesUnitsWhenLoadCompletes()
        {
            var vm = new UnitPickerViewModel(
                new[] { Cat(1, "Currency") },
                matcher: null,
                selectedCategoryId: 1,
                selectedUnitId: 11,
                categoryLoadStates: new Dictionary<int, UnitPickerCategoryLoadState>
                {
                    [1] = UnitPickerCategoryLoadState.Loading,
                });

            vm.UpdateCategory(
                1,
                new[] { U(10, "Euro", "EUR"), U(11, "Dollar", "USD") },
                UnitPickerCategoryLoadState.Loaded);

            Assert.IsFalse(vm.IsSelectedCategoryLoading);
            Assert.IsTrue(vm.AreUnitsVisible);
            Assert.AreEqual(2, vm.FilteredUnits.Count);
            Assert.AreEqual("Euro", vm.FilteredUnits[0].Unit.Name);
            Assert.AreEqual(11, vm.SelectedUnit.Unit.ModelUnitID());
        }

        [TestMethod]
        public void FailedCategory_ShowsFailureInsteadOfNoResults()
        {
            var vm = new UnitPickerViewModel(
                new[] { Cat(1, "Currency") },
                matcher: null,
                selectedCategoryId: 1,
                selectedUnitId: -1,
                categoryLoadStates: new Dictionary<int, UnitPickerCategoryLoadState>
                {
                    [1] = UnitPickerCategoryLoadState.Failed,
                });

            Assert.IsFalse(vm.IsSelectedCategoryLoading);
            Assert.IsTrue(vm.HasSelectedCategoryLoadFailed);
            Assert.IsFalse(vm.AreUnitsVisible);
            Assert.IsFalse(vm.HasNoSearchResults);
        }

        [TestMethod]
        public void FailedCategory_SearchStillShowsCrossCategoryResults()
        {
            var vm = new UnitPickerViewModel(
                new[]
                {
                    Cat(1, "Currency"),
                    Cat(2, "Length", U(10, "Meter", "m")),
                },
                matcher: null,
                selectedCategoryId: 1,
                selectedUnitId: -1,
                categoryLoadStates: new Dictionary<int, UnitPickerCategoryLoadState>
                {
                    [1] = UnitPickerCategoryLoadState.Failed,
                });

            vm.SearchText = "meter";

            Assert.IsFalse(vm.HasSelectedCategoryLoadFailed);
            Assert.IsTrue(vm.AreUnitsVisible);
            Assert.AreEqual(1, vm.FilteredUnits.Count);
            Assert.AreEqual("Meter", vm.FilteredUnits[0].Unit.Name);
        }

        [TestMethod]
        public void Categories_ExposeNameAndGlyph()
        {
            var vm = new UnitPickerViewModel(new[]
            {
                Cat(1, "Length", "\uECC6", U(10, "Meter", "m")),
                Cat(2, "Weight", "\uF4C1", U(20, "Gram", "g")),
            });

            Assert.AreEqual(2, vm.Categories.Count);
            Assert.AreEqual("Length", vm.Categories[0].Name);
            Assert.AreEqual("\uECC6", vm.Categories[0].Glyph);
            Assert.AreEqual("\uF4C1", vm.Categories[1].Glyph);
        }

        [TestMethod]
        public void CustomMatcher_OverridesDefaultSearch()
        {
            var catalog = new[]
            {
                Cat(1, "Length", U(10, "Meter", "m"), U(11, "Kilometer", "km")),
            };

            var defaultVm = new UnitPickerViewModel(catalog);
            defaultVm.SearchText = "meter";
            Assert.AreEqual(2, defaultVm.FilteredUnits.Count);

            var customVm = new UnitPickerViewModel(catalog, new ExactNameMatcher());
            customVm.SearchText = "meter";
            Assert.AreEqual(1, customVm.FilteredUnits.Count);
            Assert.AreEqual("Meter", customVm.FilteredUnits[0].Unit.Name);
        }

        [TestMethod]
        public void AbbreviationSearch_IsCultureInvariant()
        {
            var prev = System.Globalization.CultureInfo.CurrentCulture;
            try
            {
                System.Globalization.CultureInfo.CurrentCulture = new System.Globalization.CultureInfo("tr-TR");

                // "Nautical Mile" has no "nmi" substring, so only the abbreviation can match.
                // Under Turkish casing 'i' != 'I'; an ordinal matcher still finds it.
                var vm = new UnitPickerViewModel(new[]
                {
                    Cat(1, "Length", U(30, "Nautical Mile", "nmi")),
                });

                vm.SearchText = "NMI";

                Assert.AreEqual(1, vm.FilteredUnits.Count);
                Assert.AreEqual("Nautical Mile", vm.FilteredUnits[0].Unit.Name);
            }
            finally
            {
                System.Globalization.CultureInfo.CurrentCulture = prev;
            }
        }

        [TestMethod]
        public void NullUnit_IsFilteredOut()
        {
            var category = new UnitPickerCategory(
                new Category(1, "Length", false),
                new Unit[] { U(10, "Meter", "m"), null });
            var vm = new UnitPickerViewModel(new[] { category });

            Assert.AreEqual(1, vm.FilteredUnits.Count);

            vm.SearchText = "meter";
            Assert.AreEqual(1, vm.FilteredUnits.Count);
        }

        [TestMethod]
        public void NullCatalogEntry_IsSkipped()
        {
            var vm = new UnitPickerViewModel(new UnitPickerCategory[]
            {
                null,
                Cat(1, "Length", U(10, "Meter", "m")),
            });

            Assert.AreEqual(1, vm.FilteredUnits.Count);
            Assert.AreEqual(1, vm.Categories.Count);
        }
    }
}
