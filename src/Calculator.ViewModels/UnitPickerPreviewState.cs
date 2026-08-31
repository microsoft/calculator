// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp.ViewModel
{
    internal sealed class UnitPickerPreviewState
    {
        private int? _fromCategoryId;
        private int? _toCategoryId;

        internal int GetCategoryId(bool isFromUnit, int currentCategoryId) =>
            (isFromUnit ? _fromCategoryId : _toCategoryId) ?? currentCategoryId;

        internal void PreviewCategory(bool isFromUnit, int categoryId)
        {
            if (isFromUnit)
            {
                _fromCategoryId = categoryId;
            }
            else
            {
                _toCategoryId = categoryId;
            }
        }

        internal void SynchronizeCategory(int categoryId)
        {
            _fromCategoryId = categoryId;
            _toCategoryId = categoryId;
        }
    }
}
