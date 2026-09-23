// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
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
