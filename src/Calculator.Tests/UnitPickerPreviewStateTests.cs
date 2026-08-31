// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using CalculatorApp.ViewModel;

namespace Calculator.Tests
{
    [TestClass]
    public class UnitPickerPreviewStateTests
    {
        [TestMethod]
        public void PreviewCategory_PersistsIndependentlyForEachPicker()
        {
            var state = new UnitPickerPreviewState();

            Assert.AreEqual(5, state.GetCategoryId(isFromUnit: true, currentCategoryId: 5));
            Assert.AreEqual(5, state.GetCategoryId(isFromUnit: false, currentCategoryId: 5));

            state.PreviewCategory(isFromUnit: true, categoryId: 4);

            Assert.AreEqual(4, state.GetCategoryId(isFromUnit: true, currentCategoryId: 5));
            Assert.AreEqual(5, state.GetCategoryId(isFromUnit: false, currentCategoryId: 5));

            state.PreviewCategory(isFromUnit: false, categoryId: 6);

            Assert.AreEqual(4, state.GetCategoryId(isFromUnit: true, currentCategoryId: 5));
            Assert.AreEqual(6, state.GetCategoryId(isFromUnit: false, currentCategoryId: 5));
        }

        [TestMethod]
        public void SynchronizeCategory_ResetsBothPickerPreviews()
        {
            var state = new UnitPickerPreviewState();
            state.PreviewCategory(isFromUnit: true, categoryId: 4);
            state.PreviewCategory(isFromUnit: false, categoryId: 6);

            state.SynchronizeCategory(categoryId: 9);

            Assert.AreEqual(9, state.GetCategoryId(isFromUnit: true, currentCategoryId: 5));
            Assert.AreEqual(9, state.GetCategoryId(isFromUnit: false, currentCategoryId: 5));
        }
    }
}
