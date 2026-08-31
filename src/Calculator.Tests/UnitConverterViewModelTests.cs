// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.VisualStudio.TestTools.UnitTesting;
using Windows.UI.Xaml;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class CategoryViewModelTests
    {
        [TestMethod]
        public void TestGetNameReturnsCorrectName()
        {
            var category = new Category(3, "Length", supportsNegative: false);

            Assert.AreEqual("Length", category.Name);
            Assert.AreEqual(3, category.GetModelCategoryId());
        }

        [TestMethod]
        public void TestGetVisibilityReturnsVisible()
        {
            var category = new Category(7, "Temperature", supportsNegative: true);

            Assert.AreEqual(Visibility.Visible, category.NegateVisibility);
        }

        [TestMethod]
        public void TestGetVisibilityReturnsCollapsed()
        {
            var category = new Category(3, "Length", supportsNegative: false);

            Assert.AreEqual(Visibility.Collapsed, category.NegateVisibility);
        }
    }

    [TestClass]
    public class UnitViewModelTests
    {
        [TestMethod]
        public void TestGetNameReturnsCorrectName()
        {
            var unit = new Unit(11, "Centimeters", "cm", "Centimeters");

            Assert.AreEqual("Centimeters", unit.Name);
            Assert.AreEqual(11, unit.ModelUnitID());
        }

        [TestMethod]
        public void TestGetAbbreviationReturnsCorrectAbbreviation()
        {
            var unit = new Unit(11, "Centimeters", "cm", "centimeters");

            Assert.AreEqual("cm", unit.Abbreviation);
            Assert.AreEqual("centimeters", unit.AccessibleName);
            Assert.AreEqual("centimeters", unit.ToString());
        }
    }

    [TestClass]
    public class SupplementaryResultsViewModelTests
    {
        [TestMethod]
        public void TestGetValueReturnsCorrectValue()
        {
            var result = new SupplementaryResult(
                "3.5", new Unit(11, "Centimeters", "cm", "centimeters"));

            Assert.AreEqual("3.5", result.Value);
        }

        [TestMethod]
        public void TestGetUnitNameReturnsCorrectValue()
        {
            var unit = new Unit(11, "Centimeters", "cm", "centimeters");
            var result = new SupplementaryResult("3.5", unit);

            Assert.AreSame(unit, result.Unit);
            Assert.AreEqual("3.5 Centimeters", result.GetLocalizedAutomationName());
        }

        [TestMethod]
        public void TestGetIsWhimsicalReturnsCorrectValue()
        {
            var plain = new SupplementaryResult(
                "3.5", new Unit(11, "Centimeters", "cm", "centimeters"));
            var whimsical = new SupplementaryResult(
                "2", new Unit(90, "Jumbo Jets", "jj", "jumbo jets", isWhimsical: true));

            Assert.IsFalse(plain.IsWhimsical());
            Assert.IsTrue(whimsical.IsWhimsical());
        }
    }

    [TestClass]
    public class UnitConverterDataLoaderTests
    {
        [TestMethod]
        public void AllStaticUnitsProduceFiniteConversionsInBothDirections()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            int unitsChecked = 0;

            foreach (var category in viewModel.Categories.Where(
                category => category.GetModelCategoryId() != currencyId))
            {
                viewModel.CurrentCategory = category;
                var units = viewModel.Units.ToList();
                Assert.IsTrue(units.Count > 0, $"Category '{category.Name}' exposed no units.");

                var reference = units[0];
                foreach (var unit in units)
                {
                    viewModel.Unit1 = unit;
                    viewModel.Unit2 = reference;
                    viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
                    viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);

                    AssertIsRealNumber(viewModel.Value2, category.Name, unit.Name, reference.Name);

                    viewModel.Unit1 = reference;
                    viewModel.Unit2 = unit;
                    AssertIsRealNumber(viewModel.Value2, category.Name, reference.Name, unit.Name);
                    unitsChecked++;
                }
            }

            Assert.IsTrue(unitsChecked > 100, $"Only {unitsChecked} units were checked.");
        }

        private static void AssertIsRealNumber(string displayed, string category, string from, string to)
        {
            string location = $"{category}: {from} -> {to} displayed '{displayed}'";
            Assert.IsFalse(string.IsNullOrWhiteSpace(displayed), $"{location} (empty)");

            var settings = LocalizationSettings.GetInstance();
            string bare = displayed
                .Replace(settings.GetNumberGroupingSeparatorStr(), string.Empty)
                .Replace("\u00A0", string.Empty)
                .Replace(settings.GetDecimalSeparatorStr(), ".");

            Assert.IsTrue(
                double.TryParse(
                    bare,
                    NumberStyles.Float,
                    CultureInfo.InvariantCulture,
                    out double value),
                $"{location} (not a number)");
            Assert.IsFalse(double.IsNaN(value) || double.IsInfinity(value), $"{location} (not finite)");
        }
    }

    [TestClass]
    public class UnitConverterViewModelTests
    {
        [TestMethod]
        public void ChangingCategoryPreservesEditedFromValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);

            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            int volumeId = NavCategoryStates.Serialize(ViewMode.Volume);
            int targetCategoryId = viewModel.CurrentCategory.GetModelCategoryId() == lengthId ? volumeId : lengthId;
            Category targetCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == targetCategoryId);

            viewModel.CurrentCategory = targetCategory;

            Assert.AreEqual("5", viewModel.Value1);
        }

        [TestMethod]
        public void EnteringValueAfterSwitchingActiveUpdatesSecondValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.SwitchActiveCommand.Execute(null);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual("7", viewModel.Value2);
        }

        [TestMethod]
        public void MaxDigitsAnnouncementIncludesTheConversionResult()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            viewModel.OnMaxDigitsReached();

            var announcement = viewModel.Announcement?.Announcement;

            Assert.IsFalse(
                string.IsNullOrEmpty(announcement),
                "Expected a max-digits announcement.");
            Assert.IsFalse(
                announcement.Contains("%1"),
                $"The format placeholder was never substituted: '{announcement}'.");
        }

        [TestMethod]
        public void SwitchingActiveValueSwapsTheFromAndToAutomationFormats()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);

            var value1NameBefore = viewModel.Value1AutomationName;
            var value2NameBefore = viewModel.Value2AutomationName;

            viewModel.SwitchActiveCommand.Execute(null);
            viewModel.UpdateValue1AutomationName();
            viewModel.UpdateValue2AutomationName();

            // Value1 is the conversion target after the switch, so its automation name has to stop
            // describing itself as the source.
            Assert.AreNotEqual(
                StripDigits(value1NameBefore),
                StripDigits(viewModel.Value1AutomationName),
                "Value1's automation name still uses the 'from' format after switching.");
            Assert.AreNotEqual(
                StripDigits(value2NameBefore),
                StripDigits(viewModel.Value2AutomationName),
                "Value2's automation name still uses the 'to' format after switching.");
        }

        private static string StripDigits(string value)
        {
            return value == null ? null : new string(value.Where(c => !char.IsDigit(c)).ToArray());
        }

        [TestMethod]
        public void PastingAMinusAfterDigitsDoesNotNegateTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            // A minus is only a sign when it leads the value. Anywhere else it is not a legal
            // character and the digits around it are simply concatenated.
            viewModel.OnPaste("5-3");

            Assert.AreEqual("53", viewModel.Value1);
        }

        [TestMethod]
        public void PastingALeadingMinusNegatesTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            viewModel.OnPaste("-53");

            Assert.AreEqual("-53", viewModel.Value1);
        }

        private static void SelectNegatableCategory(UnitConverterViewModel viewModel)
        {
            var negatable = viewModel.Categories.First(
                category => category.NegateVisibility == Visibility.Visible);
            viewModel.CurrentCategory = negatable;
        }

        [TestMethod]
        public void TextWithNoUsableNumberIsRejectedBeforeItReachesTheConverter()
        {
            foreach (string candidate in new[] { "-", "-abc", ".", "abc" })
            {
                Assert.AreEqual(
                    "NoOp",
                    CopyPasteManager.ValidatePasteExpression(
                        candidate,
                        ViewMode.Length,
                        CategoryGroupType.Converter,
                        NumberBase.Unknown,
                        BitLength.BitLengthUnknown),
                    $"'{candidate}' should be rejected as a paste for a converter.");
            }
        }

        [TestMethod]
        public void PartialDisplayValuesDoNotThrowDuringFormatting()
        {
            var viewModel = new UnitConverterViewModel();

            viewModel.UpdateDisplay("-", ".");

            Assert.AreEqual("-", viewModel.Value1);
            Assert.AreEqual(".", viewModel.Value2);
        }

        [TestMethod]
        public void RejectedPasteSaysWhyInsteadOfBlankingTheDisplay()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);
            viewModel.OnPaste("53");
            Assert.AreEqual("53", viewModel.Value1);

            viewModel.OnPaste("NoOp");

            Assert.IsFalse(string.IsNullOrEmpty(viewModel.Value1), "A rejected paste must report something.");
            Assert.AreEqual(viewModel.Value1, viewModel.Value2);
            Assert.AreNotEqual("53", viewModel.Value1);
        }

        [TestMethod]
        public void LargeValuesAreDisplayedWithGroupSeparators()
        {
            var viewModel = new UnitConverterViewModel();
            foreach (var digit in new[]
            {
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Five,
                NumbersAndOperatorsEnum.Six,
                NumbersAndOperatorsEnum.Seven
            })
            {
                viewModel.ButtonPressedCommand.Execute(digit);
            }

            var separator = LocalizationSettings.GetInstance().GetNumberGroupingSeparatorStr();

            Assert.IsTrue(
                viewModel.Value1.Contains(separator),
                $"Expected a group separator '{separator}' in the entered value but got '{viewModel.Value1}'.");
        }

        [TestMethod]
        public void ChangingCategoryPreservesEditedToValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.SwitchActiveCommand.Execute(null);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            int volumeId = NavCategoryStates.Serialize(ViewMode.Volume);
            int targetCategoryId = viewModel.CurrentCategory.GetModelCategoryId() == lengthId ? volumeId : lengthId;
            Category targetCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == targetCategoryId);

            viewModel.CurrentCategory = targetCategory;

            Assert.AreEqual("7", viewModel.Value2);
        }

        [TestMethod]
        public void ChangingFromUnitPreservesEditedValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            Unit replacement = viewModel.Units.First(unit => unit.ModelUnitID() != viewModel.Unit1.ModelUnitID());
            var item = new UnitPickerItem(
                replacement,
                viewModel.CurrentCategory.Name,
                viewModel.CurrentCategory.GetModelCategoryId());

            viewModel.SelectPickerUnit(item, isFromUnit: true);

            Assert.AreEqual("5", viewModel.Value1);
        }

        [TestMethod]
        public void ChangingActiveToUnitKeepsItAsConversionSource()
        {
            var viewModel = CreateLengthViewModel();
            Unit topUnit = viewModel.Units[0];
            Unit replacementBottomUnit = viewModel.Units[2];
            SelectUnit(viewModel, topUnit, isFromUnit: true);
            SelectUnit(viewModel, viewModel.Units[1], isFromUnit: false);
            viewModel.SwitchActiveCommand.Execute(null);
            SelectUnit(viewModel, replacementBottomUnit, isFromUnit: false);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            var expectedViewModel = CreateLengthViewModel();
            SelectUnit(expectedViewModel, replacementBottomUnit, isFromUnit: true);
            SelectUnit(expectedViewModel, topUnit, isFromUnit: false);
            expectedViewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
            expectedViewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual(expectedViewModel.Value2, viewModel.Value1);
        }

        [TestMethod]
        public void SwappingUnitsPreservesTopValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            int originalUnit1Id = viewModel.Unit1.ModelUnitID();
            int originalUnit2Id = viewModel.Unit2.ModelUnitID();

            viewModel.SwapUnitsCommand.Execute(null);

            Assert.AreEqual("5", viewModel.Value1);
            Assert.AreEqual(originalUnit2Id, viewModel.Unit1.ModelUnitID());
            Assert.AreEqual(originalUnit1Id, viewModel.Unit2.ModelUnitID());
        }

        [TestMethod]
        public void SelectingCrossCategoryFromUnitPreservesEditedValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            UnitPickerViewModel picker = viewModel.CreateUnitPicker();
            int currentCategoryId = viewModel.CurrentCategory.GetModelCategoryId();
            int currencyCategoryId = NavCategoryStates.Serialize(ViewMode.Currency);
            picker.SelectedCategory = picker.Categories.First(
                category => category.CategoryId != currentCategoryId
                    && category.CategoryId != currencyCategoryId
                    && category.Units.Count > 0);
            UnitPickerItem item = picker.FilteredUnits[0];

            viewModel.SelectPickerUnit(item, isFromUnit: true);

            Assert.AreEqual("5", viewModel.Value1);
            Assert.AreEqual(item.CategoryId, viewModel.CurrentCategory.GetModelCategoryId());
            Assert.AreEqual(item.Unit.ModelUnitID(), viewModel.Unit1.ModelUnitID());
        }

        [TestMethod]
        public void SelectingCrossCategoryToUnitPreservesEditedValue()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.SwitchActiveCommand.Execute(null);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);
            UnitPickerViewModel picker = viewModel.CreateUnitPicker();
            int currentCategoryId = viewModel.CurrentCategory.GetModelCategoryId();
            int currencyCategoryId = NavCategoryStates.Serialize(ViewMode.Currency);
            picker.SelectedCategory = picker.Categories.First(
                category => category.CategoryId != currentCategoryId
                    && category.CategoryId != currencyCategoryId
                    && category.Units.Count > 0);
            UnitPickerItem item = picker.FilteredUnits[0];

            viewModel.SelectPickerUnit(item, isFromUnit: false);

            Assert.AreEqual("7", viewModel.Value2);
            Assert.AreEqual(item.CategoryId, viewModel.CurrentCategory.GetModelCategoryId());
            Assert.AreEqual(item.Unit.ModelUnitID(), viewModel.Unit2.ModelUnitID());
        }

        [TestMethod]
        public void ScientificCurrencyValueIsNotRepasted()
        {
            bool shouldRepaste = UnitConverterViewModel.TryPrepareCurrencyInputForPaste(
                "1.000000e+16",
                fractionDigits: 2,
                out string preparedValue);

            Assert.IsFalse(shouldRepaste);
            Assert.AreEqual("1.000000e+16", preparedValue);
        }

        [TestMethod]
        public void DecimalCurrencyValueIsTruncatedBeforeRepaste()
        {
            bool shouldRepaste = UnitConverterViewModel.TryPrepareCurrencyInputForPaste(
                "1.2345",
                fractionDigits: 2,
                out string preparedValue);

            Assert.IsTrue(shouldRepaste);
            Assert.AreEqual("1.23", preparedValue);
        }

        [TestMethod]
        public void DecimalCurrencyValueUsesLocalizedSeparatorForRepaste()
        {
            bool shouldRepaste = UnitConverterViewModel.TryPrepareCurrencyInputForPaste(
                "1.2345",
                fractionDigits: 2,
                decimalSeparator: ',',
                out string preparedValue);

            Assert.IsTrue(shouldRepaste);
            Assert.AreEqual("1,23", preparedValue);
        }

        [TestMethod]
        public async Task EnteringDigitAfterCurrencyUnitChangeReplacesValue()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            viewModel.OnPaste("1.23");
            Unit replacement = viewModel.Units.First(
                unit => unit.ModelUnitID() != viewModel.Unit1.ModelUnitID());
            SelectUnit(viewModel, replacement, isFromUnit: true);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual("7", viewModel.Value1);
        }

        [TestMethod]
        public void PickingFromUnitThatMatchesNewCategoryDefaultKeepsUnitsDistinct()
        {
            // Switching category resets both sides to the new category's defaults, then only the
            // picked side is overwritten. Picking the unit that the other side just defaulted to
            // must not leave both chips on the same unit.
            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);

            var probe = new UnitConverterViewModel();
            Category target = probe.Categories.First(
                category => category.GetModelCategoryId() != lengthId
                    && category.GetModelCategoryId() != currencyId);
            probe.CurrentCategory = target;
            int defaultToUnitId = probe.Unit2.ModelUnitID();
            Unit collidingUnit = probe.Units.Single(unit => unit.ModelUnitID() == defaultToUnitId);

            // Start from Length so selecting the probed unit genuinely switches category. The view
            // model restores the last category from LocalSettings, so pin it explicitly.
            var viewModel = CreateLengthViewModel();
            viewModel.SelectPickerUnit(
                new UnitPickerItem(collidingUnit, target.Name, target.GetModelCategoryId()),
                isFromUnit: true);

            Assert.AreEqual(defaultToUnitId, viewModel.Unit1.ModelUnitID());
            Assert.AreNotEqual(
                viewModel.Unit1.ModelUnitID(),
                viewModel.Unit2.ModelUnitID(),
                "Both chips landed on the same unit after picking the new category's default to-unit.");
        }

        [TestMethod]
        public async Task ChangingCurrencyUnitPreservesBottomActiveValue()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            viewModel.SwitchActiveCommand.Execute(null);
            // Clear first so the assertion cannot inherit a display value from earlier activity.
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Eight);
            Assert.AreEqual("8", viewModel.Value2, "Precondition: the bottom row holds the typed value.");

            Unit replacement = viewModel.Units.First(
                unit => unit.ModelUnitID() != viewModel.Unit2.ModelUnitID());
            SelectUnit(viewModel, replacement, isFromUnit: false);

            Assert.AreEqual("8", viewModel.Value2);
            Assert.AreEqual(replacement.ModelUnitID(), viewModel.Unit2.ModelUnitID());
        }

        [TestMethod]
        public void PickerLifecycleForBothChipsKeepsSelectionAndCategoryIndependent()
        {
            var viewModel = CreateLengthViewModel();
            int startingCategoryId = viewModel.CurrentCategory.GetModelCategoryId();

            UnitPickerViewModel fromPicker = viewModel.CreateUnitPicker(isFromUnit: true);
            UnitPickerViewModel toPicker = viewModel.CreateUnitPicker(isFromUnit: false);

            Assert.AreEqual(viewModel.Unit1.ModelUnitID(), fromPicker.SelectedUnit?.Unit.ModelUnitID());
            Assert.AreEqual(viewModel.Unit2.ModelUnitID(), toPicker.SelectedUnit?.Unit.ModelUnitID());
            Assert.AreEqual(
                startingCategoryId,
                viewModel.CurrentCategory.GetModelCategoryId(),
                "Creating pickers must not change the converter's category.");

            int untouchedUnit2Id = viewModel.Unit2.ModelUnitID();
            UnitPickerItem replacement = fromPicker.FilteredUnits.First(
                item => item.Unit.ModelUnitID() != viewModel.Unit1.ModelUnitID()
                    && item.Unit.ModelUnitID() != untouchedUnit2Id);

            viewModel.SelectPickerUnit(replacement, isFromUnit: true);

            Assert.AreEqual(replacement.Unit.ModelUnitID(), viewModel.Unit1.ModelUnitID());
            Assert.AreEqual(untouchedUnit2Id, viewModel.Unit2.ModelUnitID());
        }

        [TestMethod]
        public void PickingTheSameUnitAsTheOtherChipSwapsInsteadOfDuplicating()
        {
            // Converting a unit into itself is not useful, so the two chips must never show the
            // same unit. Picking the unit the other chip already holds hands that chip the picked
            // side's previous unit, which reads as a swap.
            var viewModel = CreateLengthViewModel();
            Unit unitA = viewModel.Units[0];
            Unit unitB = viewModel.Units[1];

            // Establish a known starting pair; saved user preferences make the initial units
            // unpredictable.
            SelectUnit(viewModel, unitA, isFromUnit: true);
            SelectUnit(viewModel, unitB, isFromUnit: false);
            Assert.AreEqual(unitA.ModelUnitID(), viewModel.Unit1.ModelUnitID());
            Assert.AreEqual(unitB.ModelUnitID(), viewModel.Unit2.ModelUnitID());

            SelectUnit(viewModel, unitB, isFromUnit: true);

            Assert.AreEqual(unitB.ModelUnitID(), viewModel.Unit1.ModelUnitID());
            Assert.AreEqual(unitA.ModelUnitID(), viewModel.Unit2.ModelUnitID());
            Assert.AreNotEqual(viewModel.Unit1.ModelUnitID(), viewModel.Unit2.ModelUnitID());
        }

        [TestMethod]
        public async Task EnteringCurrencyAfterBackgroundLoadUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            Assert.IsFalse(
                viewModel.IsCurrencyCurrentCategory,
                "Precondition: the view model starts outside Currency.");

            await WaitForCurrencyLoadAsync(viewModel);

            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            // The mock currency data rates MAR at 1.00 and MON at 0.50.
            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }

        [TestMethod]
        public async Task CurrencyLoadFinishingInsideCurrencyUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            Assert.IsFalse(
                viewModel.IsCurrencyDataLoaded,
                "Precondition: Currency is entered before the background load reports back.");

            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            // The mock currency data rates MAR at 1.00 and MON at 0.50.
            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }

        [TestMethod]
        public async Task LeavingCurrencyClearsTheCurrencySymbolsAndRatio()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Assert.IsFalse(
                string.IsNullOrEmpty(viewModel.CurrencySymbol1),
                "Precondition: Currency shows a symbol.");

            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == lengthId);

            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol1, "Currency symbol leaked into Length.");
            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol2, "Currency symbol leaked into Length.");
            Assert.AreEqual(
                Windows.UI.Xaml.Visibility.Collapsed,
                viewModel.CurrencySymbolVisibility,
                "The currency symbol block must be collapsed outside Currency.");
            Assert.AreEqual(string.Empty, viewModel.CurrencyRatioEquality, "Currency ratio leaked into Length.");
        }

        private static async Task WaitForCurrencyLoadAsync(UnitConverterViewModel viewModel)
        {
            for (int attempt = 0; attempt < 250; attempt++)
            {
                if (viewModel.IsCurrencyDataLoaded)
                {
                    return;
                }

                await Task.Delay(20);
            }

            Assert.Fail("The background currency load did not finish.");
        }

        private static async Task WaitForCurrencyUnitsAsync(UnitConverterViewModel viewModel)
        {
            // Units appear as soon as the loader hands them over, but the load's completion path
            // also resets the converter, so waiting on the units alone can return mid-load.
            await WaitForCurrencyLoadAsync(viewModel);

            for (int attempt = 0; attempt < 100; attempt++)
            {
                if (viewModel.Units.Count > 1 && viewModel.Units[0].ModelUnitID() != -1)
                {
                    return;
                }

                await Task.Delay(20);
            }

            Assert.Fail("Currency units did not load.");
        }

        private static UnitConverterViewModel CreateLengthViewModel()
        {
            var viewModel = new UnitConverterViewModel();
            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == lengthId);
            return viewModel;
        }

        private static void SelectUnit(UnitConverterViewModel viewModel, Unit unit, bool isFromUnit)
        {
            viewModel.SelectPickerUnit(
                new UnitPickerItem(
                    unit,
                    viewModel.CurrentCategory.Name,
                    viewModel.CurrentCategory.GetModelCategoryId()),
                isFromUnit);
        }

        [TestMethod]
        public void CreateUnitPickerPreselectsOpeningChipUnit()
        {
            var viewModel = new UnitConverterViewModel();
            int unit1Id = viewModel.Unit1.ModelUnitID();
            int unit2Id = viewModel.Unit2.ModelUnitID();
            int categoryId = viewModel.CurrentCategory.GetModelCategoryId();

            UnitPickerViewModel fromPicker = viewModel.CreateUnitPicker(isFromUnit: true);
            UnitPickerViewModel toPicker = viewModel.CreateUnitPicker(isFromUnit: false);

            Assert.AreEqual(unit1Id, fromPicker.SelectedUnit.Unit.ModelUnitID());
            Assert.AreEqual(unit2Id, toPicker.SelectedUnit.Unit.ModelUnitID());
            Assert.AreEqual(categoryId, viewModel.CurrentCategory.GetModelCategoryId());
            Assert.AreEqual(unit1Id, viewModel.Unit1.ModelUnitID());
            Assert.AreEqual(unit2Id, viewModel.Unit2.ModelUnitID());
        }

        [TestMethod]
        public async Task RefreshCurrencyRatiosCompletesAfterInitialLoad()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);
            viewModel.OnCurrencyTimestampUpdated("stale timestamp", isWeekOld: true);

            await viewModel.RefreshCurrencyRatiosAsync();

            Assert.IsFalse(viewModel.IsCurrencyLoadingVisible);
            Assert.AreNotEqual("stale timestamp", viewModel.CurrencyTimestamp);
            Assert.IsFalse(viewModel.CurrencyDataIsWeekOld);
        }

        [TestMethod]
        public async Task CurrencyLoadFailureUpdatesExistingPicker()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            UnitPickerViewModel picker = viewModel.CreateUnitPicker();
            picker.SelectedCategory = picker.Categories.Single(
                category => category.CategoryId == currencyId);

            viewModel.OnCurrencyDataLoadFinished(didLoad: false);

            Assert.IsTrue(picker.HasSelectedCategoryLoadFailed);
            Assert.IsFalse(picker.AreUnitsVisible);
            Assert.IsFalse(picker.HasNoSearchResults);
        }

        [TestMethod]
        public async Task CurrencyFailureRemainsFailedAfterNetworkChange()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            UnitPickerViewModel openPicker = viewModel.CreateUnitPicker();
            openPicker.SelectedCategory = openPicker.Categories.Single(
                category => category.CategoryId == currencyId);
            viewModel.OnCurrencyDataLoadFinished(didLoad: false);

            viewModel.HandleNetworkBehaviorChanged(NetworkAccessBehavior.Normal);
            UnitPickerViewModel newPicker = viewModel.CreateUnitPicker();
            newPicker.SelectedCategory = newPicker.Categories.Single(
                category => category.CategoryId == currencyId);

            Assert.IsTrue(openPicker.HasSelectedCategoryLoadFailed);
            Assert.IsTrue(newPicker.HasSelectedCategoryLoadFailed);
        }

        [TestMethod]
        public async Task RefreshCurrencyRatiosSurvivesTimestampFailure()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);
            viewModel.OnCurrencyTimestampUpdated("stale timestamp", isWeekOld: true);

            PropertyChangedEventHandler failTimestamp = (sender, args) =>
            {
                if (args.PropertyName == nameof(UnitConverterViewModel.CurrencyTimestamp))
                {
                    throw new InvalidOperationException("timestamp display failed");
                }
            };

            viewModel.PropertyChanged += failTimestamp;
            try
            {
                await viewModel.RefreshCurrencyRatiosAsync();
            }
            finally
            {
                viewModel.PropertyChanged -= failTimestamp;
            }

            Assert.IsFalse(
                viewModel.IsCurrencyLoadingVisible,
                "A timestamp failure left the loading spinner up, which disables the refresh button.");
            Assert.IsTrue(
                viewModel.IsCurrencyDataLoaded,
                "A timestamp failure left the converter without ratios and with no way to reload.");
        }

        [TestMethod]
        public async Task RefreshCurrencyRatiosPreservesSelectedCurrencies()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            await WaitForCurrencyCatalogAsync(viewModel);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);

            Unit[] replacements = viewModel.Units.Where(
                unit => unit.Abbreviation != viewModel.Unit1.Abbreviation
                    && unit.Abbreviation != viewModel.Unit2.Abbreviation)
                .Take(2)
                .ToArray();
            Assert.AreEqual(2, replacements.Length);
            Unit newFrom = replacements[0];
            Unit newTo = replacements[1];
            SelectUnit(viewModel, newFrom, isFromUnit: true);
            SelectUnit(viewModel, newTo, isFromUnit: false);

            await viewModel.RefreshCurrencyRatiosAsync();

            Assert.AreEqual(newFrom.Abbreviation, viewModel.Unit1.Abbreviation);
            Assert.AreEqual(newTo.Abbreviation, viewModel.Unit2.Abbreviation);
        }

        [TestMethod]
        public void ConverterCommandsKeepTheirIdentityAcrossReads()
        {
            var viewModel = new UnitConverterViewModel();

            Assert.AreSame(viewModel.CategoryChangedCommand, viewModel.CategoryChangedCommand);
            Assert.AreSame(viewModel.UnitChangedCommand, viewModel.UnitChangedCommand);
            Assert.AreSame(viewModel.SwitchActiveCommand, viewModel.SwitchActiveCommand);
            Assert.AreSame(viewModel.SwapUnitsCommand, viewModel.SwapUnitsCommand);
            Assert.AreSame(viewModel.ButtonPressedCommand, viewModel.ButtonPressedCommand);
            Assert.AreSame(viewModel.CopyCommand, viewModel.CopyCommand);
            Assert.AreSame(viewModel.PasteCommand, viewModel.PasteCommand);
            Assert.AreSame(
                viewModel.ButtonPressedCommand,
                viewModel.ButtonPressed,
                "The ButtonPressed alias must be the same command object it aliases.");
        }

        [TestMethod]
        public void OtherViewModelCommandsKeepTheirIdentityAcrossReads()
        {
            var standard = new StandardCalculatorViewModel();
            HistoryViewModel history = standard.HistoryVM;
            Assert.AreSame(history.ClearCommand, history.ClearCommand);
            Assert.AreSame(history.HideCommand, history.HideCommand);

            var dateCalculator = new DateCalculatorViewModel();
            Assert.AreSame(dateCalculator.CopyCommand, dateCalculator.CopyCommand);

            var application = new ApplicationViewModel();
            Assert.AreSame(application.CopyCommand, application.CopyCommand);
            Assert.AreSame(application.PasteCommand, application.PasteCommand);
        }

        [TestMethod]
        public async Task RefreshLeavesTheSpinnerClearWhenTheTailOfLoadFinishedThrows()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);

            PropertyChangedEventHandler failAnnouncement = (sender, args) =>
            {
                if (args.PropertyName == nameof(UnitConverterViewModel.Announcement))
                {
                    throw new InvalidOperationException("announcement failed");
                }
            };

            viewModel.PropertyChanged += failAnnouncement;
            try
            {
                await viewModel.RefreshCurrencyRatiosAsync();
            }
            catch (InvalidOperationException)
            {
                // The throw is the point; what matters is the state it leaves behind.
            }
            finally
            {
                viewModel.PropertyChanged -= failAnnouncement;
            }

            Assert.IsFalse(
                viewModel.IsCurrencyLoadingVisible,
                "The spinner must be cleared before anything that can throw, or the refresh button stays disabled.");
        }

        [TestMethod]
        public async Task RefreshNotificationsCannotStrandLoadingState()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            UnitPickerViewModel picker = viewModel.CreateUnitPicker();
            picker.SelectedCategory = picker.Categories.Single(
                category => category.CategoryId == currencyId);

            PropertyChangedEventHandler failNotifications = (sender, args) =>
            {
                if ((args.PropertyName == nameof(UnitConverterViewModel.IsCurrencyLoadingVisible)
                        && viewModel.IsCurrencyLoadingVisible)
                    || args.PropertyName == nameof(UnitConverterViewModel.CurrencyDataLoadFailed))
                {
                    throw new InvalidOperationException("notification failed");
                }
            };

            viewModel.PropertyChanged += failNotifications;
            InvalidOperationException failure = null;
            try
            {
                await viewModel.RefreshCurrencyRatiosAsync();
            }
            catch (InvalidOperationException exception)
            {
                failure = exception;
            }
            finally
            {
                viewModel.PropertyChanged -= failNotifications;
            }

            Assert.IsNotNull(failure);
            Assert.IsFalse(viewModel.IsCurrencyLoadingVisible);
            Assert.IsTrue(viewModel.IsCurrencyDataLoaded);
            Assert.IsTrue(viewModel.CurrencyDataLoadFailed);
            Assert.IsFalse(picker.IsSelectedCategoryLoading);
            Assert.IsTrue(picker.HasSelectedCategoryLoadFailed);
        }

        [TestMethod]
        public async Task RefreshUpdatesOpenPickerBeforeNativeCallbackFailure()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            UnitPickerViewModel picker = viewModel.CreateUnitPicker();
            picker.SelectedCategory = picker.Categories.Single(
                category => category.CategoryId == currencyId);

            PropertyChangedEventHandler failUnitUpdate = (sender, args) =>
            {
                if (args.PropertyName == nameof(UnitConverterViewModel.Unit1))
                {
                    throw new InvalidOperationException("unit update failed");
                }
            };

            viewModel.PropertyChanged += failUnitUpdate;
            InvalidOperationException failure = null;
            try
            {
                await viewModel.RefreshCurrencyRatiosAsync();
            }
            catch (InvalidOperationException exception)
            {
                failure = exception;
            }
            finally
            {
                viewModel.PropertyChanged -= failUnitUpdate;
            }

            Assert.IsNotNull(failure);
            Assert.IsFalse(picker.IsSelectedCategoryLoading);
        }

        [TestMethod]
        public async Task PickerNotificationFailureDoesNotSkipCurrencyReset()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyCatalogAsync(viewModel);
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            UnitPickerViewModel picker = viewModel.CreateUnitPicker();
            picker.SelectedCategory = picker.Categories.Single(
                category => category.CategoryId == currencyId);
            bool unitUpdated = false;

            PropertyChangedEventHandler recordUnitUpdate = (sender, args) =>
            {
                if (args.PropertyName == nameof(UnitConverterViewModel.Unit1))
                {
                    unitUpdated = true;
                }
            };
            PropertyChangedEventHandler failPickerCompletion = (sender, args) =>
            {
                if (args.PropertyName == nameof(UnitPickerViewModel.IsSelectedCategoryLoading)
                    && !picker.IsSelectedCategoryLoading)
                {
                    throw new InvalidOperationException("picker update failed");
                }
            };

            viewModel.PropertyChanged += recordUnitUpdate;
            picker.PropertyChanged += failPickerCompletion;
            InvalidOperationException failure = null;
            try
            {
                await viewModel.RefreshCurrencyRatiosAsync();
            }
            catch (InvalidOperationException exception)
            {
                failure = exception;
            }
            finally
            {
                picker.PropertyChanged -= failPickerCompletion;
                viewModel.PropertyChanged -= recordUnitUpdate;
            }

            Assert.IsNotNull(failure);
            Assert.IsTrue(unitUpdated);
        }

        private static async Task WaitForCurrencyCatalogAsync(UnitConverterViewModel viewModel)
        {
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            for (int attempt = 0; attempt < 100; attempt++)
            {
                UnitPickerCategory currency = viewModel.CreateUnitPicker()
                    .Categories.Single(category => category.CategoryId == currencyId);
                if (currency.Units.Count > 0)
                {
                    return;
                }

                await Task.Delay(20);
            }

            Assert.Fail("Currency units did not load.");
        }


        [TestMethod]
        public void MaxDigitsAnnouncementIncludesTheConversionResult()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            viewModel.OnMaxDigitsReached();

            var announcement = viewModel.Announcement?.Announcement;

            Assert.IsFalse(string.IsNullOrEmpty(announcement));
            Assert.IsFalse(
                announcement.Contains("%1"),
                $"The format placeholder was never substituted: '{announcement}'.");
        }

        [TestMethod]
        public void ConversionResultNarrationSubstitutesAllPlaceholders()
        {
            var viewModel = new UnitConverterViewModel();

            string result = viewModel.GetLocalizedConversionResultStringFormat(
                "1", "meter", "3.28", "feet");

            Assert.IsFalse(result.Contains("%1"));
            Assert.IsFalse(result.Contains("%2"));
            Assert.IsFalse(result.Contains("%3"));
            Assert.IsFalse(result.Contains("%4"));
            StringAssert.Contains(result, "meter");
            StringAssert.Contains(result, "feet");
        }

        [TestMethod]
        public void SwitchingActiveValueSwapsTheFromAndToAutomationFormats()
        {
            var viewModel = new UnitConverterViewModel();
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);

            var value1NameBefore = viewModel.Value1AutomationName;
            var value2NameBefore = viewModel.Value2AutomationName;

            viewModel.SwitchActiveCommand.Execute(null);
            viewModel.UpdateValue1AutomationName();
            viewModel.UpdateValue2AutomationName();

            Assert.AreNotEqual(
                StripDigits(value1NameBefore),
                StripDigits(viewModel.Value1AutomationName));
            Assert.AreNotEqual(
                StripDigits(value2NameBefore),
                StripDigits(viewModel.Value2AutomationName));
        }

        private static string StripDigits(string value)
        {
            return value == null ? null : new string(value.Where(c => !char.IsDigit(c)).ToArray());
        }

        [TestMethod]
        public void PastingAMinusAfterDigitsDoesNotNegateTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            viewModel.OnPaste("5-3");

            Assert.AreEqual("53", viewModel.Value1);
        }

        [TestMethod]
        public void PastingALeadingMinusNegatesTheValue()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);

            viewModel.OnPaste("-53");

            Assert.AreEqual("-53", viewModel.Value1);
        }

        private static void SelectNegatableCategory(UnitConverterViewModel viewModel)
        {
            viewModel.CurrentCategory = viewModel.Categories.First(
                category => category.NegateVisibility == Visibility.Visible);
        }

        [TestMethod]
        public void TextWithNoUsableNumberIsRejectedBeforeItReachesTheConverter()
        {
            foreach (string candidate in new[] { "-", "-abc", ".", "abc" })
            {
                Assert.AreEqual(
                    "NoOp",
                    CopyPasteManager.ValidatePasteExpression(
                        candidate,
                        ViewMode.Length,
                        CategoryGroupType.Converter,
                        NumberBase.Unknown,
                        BitLength.BitLengthUnknown),
                    $"'{candidate}' should be rejected as a paste for a converter.");
            }
        }

        [TestMethod]
        public void PartialDisplayValuesDoNotThrowDuringFormatting()
        {
            var viewModel = new UnitConverterViewModel();

            viewModel.UpdateDisplay("-", ".");

            Assert.AreEqual("-", viewModel.Value1);
            Assert.AreEqual(".", viewModel.Value2);
        }

        [TestMethod]
        public void RejectedPasteSaysWhyInsteadOfBlankingTheDisplay()
        {
            var viewModel = new UnitConverterViewModel();
            SelectNegatableCategory(viewModel);
            viewModel.OnPaste("53");
            Assert.AreEqual("53", viewModel.Value1);

            viewModel.OnPaste("NoOp");

            Assert.IsFalse(string.IsNullOrEmpty(viewModel.Value1));
            Assert.AreEqual(viewModel.Value1, viewModel.Value2);
            Assert.AreNotEqual("53", viewModel.Value1);
        }

        [TestMethod]
        public void LargeValuesAreDisplayedWithGroupSeparators()
        {
            var viewModel = new UnitConverterViewModel();
            foreach (var digit in new[]
            {
                NumbersAndOperatorsEnum.One,
                NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Three,
                NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Five,
                NumbersAndOperatorsEnum.Six,
                NumbersAndOperatorsEnum.Seven
            })
            {
                viewModel.ButtonPressedCommand.Execute(digit);
            }

            var separator = LocalizationSettings.GetInstance().GetNumberGroupingSeparatorStr();

            StringAssert.Contains(viewModel.Value1, separator);
        }

        [TestMethod]
        public void LengthSuggestionsPreserveWhimsicalUnitMetadata()
        {
            var viewModel = CreateLengthViewModel();
            var resources = AppResourceProvider.GetInstance();
            Unit centimeters = viewModel.Units.Single(
                unit => unit.Name == resources.GetResourceString("UnitName_Centimeter"));
            Unit inches = viewModel.Units.Single(
                unit => unit.Name == resources.GetResourceString("UnitName_Inch"));
            SelectUnit(viewModel, centimeters, isFromUnit: true);
            SelectUnit(viewModel, inches, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Clear);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Four);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            SupplementaryResult result = viewModel.SupplementaryResults.Last();
            Assert.AreEqual(resources.GetResourceString("UnitName_Hand"), result.Unit.Name);
            Assert.AreEqual(
                resources.GetResourceString("UnitAbbreviation_Hand"),
                result.Unit.Abbreviation);
            Assert.IsTrue(result.IsWhimsical());
        }

        [TestMethod]
        public async Task EnteringDigitAfterCurrencyUnitChangeReplacesValue()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            viewModel.OnPaste("1.23");
            Unit replacement = viewModel.Units.First(
                unit => unit.ModelUnitID() != viewModel.Unit1.ModelUnitID());
            SelectUnit(viewModel, replacement, isFromUnit: true);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Seven);

            Assert.AreEqual("7", viewModel.Value1);
        }

        [TestMethod]
        public async Task EnteringCurrencyAfterBackgroundLoadUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            Assert.IsFalse(viewModel.IsCurrencyCurrentCategory);

            await WaitForCurrencyLoadAsync(viewModel);

            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }

        [TestMethod]
        public async Task CurrencyLoadFinishingInsideCurrencyUsesLoadedRatios()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            Assert.IsFalse(viewModel.IsCurrencyDataLoaded);

            await WaitForCurrencyUnitsAsync(viewModel);

            Unit mars = viewModel.Units.First(unit => unit.Abbreviation == "MAR");
            Unit moon = viewModel.Units.First(unit => unit.Abbreviation == "MON");
            SelectUnit(viewModel, mars, isFromUnit: true);
            SelectUnit(viewModel, moon, isFromUnit: false);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Zero);

            Assert.AreEqual("100", viewModel.Value1);
            Assert.AreEqual("50", viewModel.Value2);
        }

        [TestMethod]
        public async Task LeavingCurrencyClearsTheCurrencySymbolsAndRatio()
        {
            var viewModel = new UnitConverterViewModel();
            int currencyId = NavCategoryStates.Serialize(ViewMode.Currency);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == currencyId);
            await WaitForCurrencyUnitsAsync(viewModel);

            Assert.IsFalse(string.IsNullOrEmpty(viewModel.CurrencySymbol1));

            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == lengthId);

            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol1);
            Assert.AreEqual(string.Empty, viewModel.CurrencySymbol2);
            Assert.AreEqual(Visibility.Collapsed, viewModel.CurrencySymbolVisibility);
            Assert.AreEqual(string.Empty, viewModel.CurrencyRatioEquality);
        }

        [TestMethod]
        public async Task CurrencyRefreshCompletesAfterInitialLoad()
        {
            var viewModel = new UnitConverterViewModel();
            await WaitForCurrencyLoadAsync(viewModel);
            viewModel.OnCurrencyTimestampUpdated("stale timestamp", isWeekOld: true);

            await viewModel.RefreshCurrencyRatiosAsync();

            Assert.IsTrue(viewModel.IsCurrencyDataLoaded);
            Assert.IsFalse(viewModel.IsCurrencyLoadingVisible);
            Assert.IsFalse(viewModel.CurrencyDataLoadFailed);
            Assert.AreNotEqual("stale timestamp", viewModel.CurrencyTimestamp);
        }

        [TestMethod]
        public async Task LocaleDefaultCurrencyMapIsPackaged()
        {
            var file = await Windows.Storage.StorageFile.GetFileFromApplicationUriAsync(
                new Uri("ms-appx:///DataLoaders/DefaultFromToCurrency.json"));
            string json = await Windows.Storage.FileIO.ReadTextAsync(file);

            StringAssert.Contains(json, "\"en-GB\"");
            StringAssert.Contains(json, "\"GBP\"");
            StringAssert.Contains(json, "\"en-CA\"");
            StringAssert.Contains(json, "\"CAD\"");
        }

        [TestMethod]
        public void ConverterCommandsKeepTheirIdentityAcrossReads()
        {
            var viewModel = new UnitConverterViewModel();

            Assert.AreSame(viewModel.CategoryChangedCommand, viewModel.CategoryChangedCommand);
            Assert.AreSame(viewModel.UnitChangedCommand, viewModel.UnitChangedCommand);
            Assert.AreSame(viewModel.SwitchActiveCommand, viewModel.SwitchActiveCommand);
            Assert.AreSame(viewModel.ButtonPressedCommand, viewModel.ButtonPressedCommand);
            Assert.AreSame(viewModel.CopyCommand, viewModel.CopyCommand);
            Assert.AreSame(viewModel.PasteCommand, viewModel.PasteCommand);
            Assert.AreSame(viewModel.ButtonPressedCommand, viewModel.ButtonPressed);
        }

        [TestMethod]
        public void OtherViewModelCommandsKeepTheirIdentityAcrossReads()
        {
            var standard = new StandardCalculatorViewModel();
            HistoryViewModel history = standard.HistoryVM;
            Assert.AreSame(history.ClearCommand, history.ClearCommand);
            Assert.AreSame(history.HideCommand, history.HideCommand);

            var dateCalculator = new DateCalculatorViewModel();
            Assert.AreSame(dateCalculator.CopyCommand, dateCalculator.CopyCommand);

            var application = new ApplicationViewModel();
            Assert.AreSame(application.CopyCommand, application.CopyCommand);
            Assert.AreSame(application.PasteCommand, application.PasteCommand);
        }

        [TestMethod]
        public void ConstructionAndCategorySwitchingKeepTheConverterConsistent()
        {
            var viewModel = new UnitConverterViewModel();

            Assert.IsTrue(viewModel.Categories.Count > 0);
            Assert.IsTrue(viewModel.Units.Count > 0);
            Assert.IsNotNull(viewModel.CurrentCategory);
            Assert.IsNotNull(viewModel.Unit1);
            Assert.IsNotNull(viewModel.Unit2);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            var original = viewModel.CurrentCategory;
            var originalUnits = viewModel.Units.Select(unit => unit.ModelUnitID()).ToList();
            var other = viewModel.Categories.First(
                category => category.GetModelCategoryId() != original.GetModelCategoryId());

            viewModel.CurrentCategory = other;
            CollectionAssert.AreNotEqual(
                originalUnits,
                viewModel.Units.Select(unit => unit.ModelUnitID()).ToList());
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit1));
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit2));

            viewModel.CurrentCategory = original;
            CollectionAssert.AreEqual(
                originalUnits,
                viewModel.Units.Select(unit => unit.ModelUnitID()).ToList());
        }

        [TestMethod]
        public void CategorySwitchPublishesNewUnitsBeforeSelectedUnits()
        {
            var viewModel = new UnitConverterViewModel();
            var originalCollection = viewModel.Units;
            var originalUnits = originalCollection.Select(unit => unit.ModelUnitID()).ToList();
            int sequence = 0;
            int unitsChanged = -1;
            int unit1Changed = -1;
            int unit2Changed = -1;

            viewModel.PropertyChanged += (sender, args) =>
            {
                sequence++;
                if (args.PropertyName == nameof(UnitConverterViewModel.Units))
                {
                    unitsChanged = sequence;
                }
                else if (args.PropertyName == nameof(UnitConverterViewModel.Unit1))
                {
                    unit1Changed = sequence;
                }
                else if (args.PropertyName == nameof(UnitConverterViewModel.Unit2))
                {
                    unit2Changed = sequence;
                }
            };

            var other = viewModel.Categories.First(
                category => category.GetModelCategoryId() != viewModel.CurrentCategory.GetModelCategoryId()
                    && category.GetModelCategoryId() != NavCategoryStates.Serialize(ViewMode.Currency));
            viewModel.CurrentCategory = other;

            Assert.AreNotSame(originalCollection, viewModel.Units);
            CollectionAssert.AreEqual(
                originalUnits,
                originalCollection.Select(unit => unit.ModelUnitID()).ToList());
            Assert.IsTrue(unitsChanged > 0);
            Assert.IsTrue(unit1Changed > unitsChanged);
            Assert.IsTrue(unit2Changed > unitsChanged);
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit1));
            Assert.IsTrue(viewModel.Units.Contains(viewModel.Unit2));
        }

        [TestMethod]
        public void InputFollowsTheActiveValueAndIsFormattedForDisplay()
        {
            var viewModel = new UnitConverterViewModel();
            var separator = LocalizationSettings.GetInstance().GetDecimalSeparatorStr();
            bool firstWasActive = viewModel.Value1Active;

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.One);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Decimal);
            StringAssert.EndsWith(viewModel.Value1, separator);
            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Five);
            Assert.AreEqual($"1{separator}5", viewModel.Value1);

            viewModel.SwitchActiveCommand.Execute(null);
            Assert.AreNotEqual(firstWasActive, viewModel.Value1Active);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            viewModel.ButtonPressedCommand.Execute(NumbersAndOperatorsEnum.Eight);
            Assert.AreEqual("8", viewModel.Value2);

            viewModel.SwitchActiveCommand.Execute(null);
            Assert.AreEqual(firstWasActive, viewModel.Value1Active);
            Assert.IsTrue(viewModel.Value1Active ^ viewModel.Value2Active);

            viewModel.UpdateValue1AutomationName();
            Assert.IsFalse(string.IsNullOrEmpty(viewModel.Value1AutomationName));
            StringAssert.Contains(viewModel.Value1AutomationName, viewModel.Unit1.AccessibleName);
        }

        private static async Task WaitForCurrencyLoadAsync(UnitConverterViewModel viewModel)
        {
            for (int attempt = 0; attempt < 250; attempt++)
            {
                if (viewModel.IsCurrencyDataLoaded)
                {
                    return;
                }

                await Task.Delay(20);
            }

            Assert.Fail("The background currency load did not finish.");
        }

        private static async Task WaitForCurrencyUnitsAsync(UnitConverterViewModel viewModel)
        {
            await WaitForCurrencyLoadAsync(viewModel);

            for (int attempt = 0; attempt < 100; attempt++)
            {
                if (viewModel.Units.Count > 1
                    && viewModel.Units[0].ModelUnitID() != -1
                    && viewModel.Unit1 != null
                    && viewModel.Unit2 != null
                    && viewModel.Units.Contains(viewModel.Unit1)
                    && viewModel.Units.Contains(viewModel.Unit2))
                {
                    return;
                }

                await Task.Delay(20);
            }

            Assert.Fail("Currency units did not load.");
        }

        private static UnitConverterViewModel CreateLengthViewModel()
        {
            var viewModel = new UnitConverterViewModel();
            int lengthId = NavCategoryStates.Serialize(ViewMode.Length);
            viewModel.CurrentCategory = viewModel.Categories.Single(
                category => category.GetModelCategoryId() == lengthId);
            return viewModel;
        }

        private static void SelectUnit(UnitConverterViewModel viewModel, Unit unit, bool isFromUnit)
        {
            if (isFromUnit)
            {
                viewModel.Unit1 = unit;
            }
            else
            {
                viewModel.Unit2 = unit;
            }
        }
    }
}
