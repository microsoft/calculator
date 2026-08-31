// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class UnitPickerItem
    {
        public UnitPickerItem(Unit unit, string categoryName, int categoryId)
        {
            Unit = unit;
            CategoryName = categoryName;
            CategoryId = categoryId;
        }

        public Unit Unit { get; }
        public string CategoryName { get; }
        public int CategoryId { get; }
    }
}
