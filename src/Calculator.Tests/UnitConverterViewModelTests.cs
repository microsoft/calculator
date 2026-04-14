// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

namespace Calculator.Tests
{
    [TestClass]
    public class CategoryViewModelTests
    {
        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetNameReturnsCorrectName() { }

        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetVisibilityReturnsVisible() { }

        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetVisibilityReturnsCollapsed() { }
    }

    [TestClass]
    public class UnitViewModelTests
    {
        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetNameReturnsCorrectName() { }

        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetAbbreviationReturnsCorrectAbbreviation() { }
    }

    [TestClass]
    public class SupplementaryResultsViewModelTests
    {
        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetValueReturnsCorrectValue() { }

        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetUnitNameReturnsCorrectValue() { }

        [TestMethod]
        [Ignore("Requires native UnitConversionManager types")]
        public void TestGetIsWhimsicalReturnsCorrectValue() { }
    }

    [TestClass]
    public class UnitConverterDataLoaderTests
    {
        [TestMethod]
        [Ignore("Integration test requiring UWP ResourceLoader")]
        public void FuncUnitConverterAllUnitCombinations() { }
    }

    [TestClass]
    public class UnitConverterViewModelTests
    {
        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitConverterCtorSetsUpCorrectActiveValue() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitConverterCtorSetsUpVectors() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitConverterLoadSetsUpCallbacks() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitConverterLoadSetsUpCategories() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitConverterLoadSetsUpUnits() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitSelectionChangeUpdatesModel() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategorySelectionChangeUpdatesUnits() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategorySelectionChangeUpdatesModel() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestDisplayCallbackUpdatesDisplayValues() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestButtonCommandFiresModelCommands() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestOnValueGotFocusActivatesControl() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestReselectCurrentlyActiveValueDoesNothing() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestActivatingValueDeactivatesOther() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestSwitchActiveValueUpdatesActiveValueInModel() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestSuggestedVisibilityIsUpdated() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestDisplayValueUpdatesAfterSwitchingActiveUpdateTheRightDisplay() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitChangeAfterSwitchingActiveUpdateUnitsCorrectly() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategorySwitchAndBackKeepsUnitsUnchanged() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategoryAndActiveSwitchAndBack() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategoryChangeAfterSwitchingActiveUpdatesDisplayCorrectly() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestSwitchAndReselectCurrentlyActiveValueDoesNothing() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestSwitchActiveValueTwiceUpdatesActiveValueInModel() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestDisplayValueUpdatesAfterSwitchingActiveTwiceUpdateTheRightDisplay() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitChangeAfterSwitchingActiveTwiceUpdateUnitsCorrectly() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategoryChangeAfterSwitchingActiveTwiceUpdatesDisplayCorrectly() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestSuggestedValuesCallbackUpdatesSupplementaryResults() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategoryChangeImmediatelyUpdatesSupplementaryResults() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCategoryChangeImmediatelyUpdatesSupplementaryResultsWhimsy() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitChangeImmediatelyUpdatesSupplementaryResults() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestSupplementaryResultsWhimsicalUnits() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestOnPaste() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestDecimalFormattingLogic() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestCurrencyFormattingLogic() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestValue1AndValue2AutomationNameChanges() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitsListBuildsFromEmptyModelUnitList() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestUnitsListBuildsFromValidModelUnitList() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestFindInListWhenListIsValid() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestFindInListWhenListIsInvalid() { }

        [TestMethod]
        [Ignore("Requires native UnitConverterMock not available in C# tests")]
        public void TestSetSelectedUnits() { }
    }
}
