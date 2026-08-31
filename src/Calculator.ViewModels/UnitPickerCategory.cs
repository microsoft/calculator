// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class UnitPickerCategory
    {
        public UnitPickerCategory(Category category, IReadOnlyList<Unit> units, string glyph = null)
        {
            Category = category;
            Units = units ?? Array.Empty<Unit>();
            Glyph = glyph;
        }

        public Category Category { get; }
        public IReadOnlyList<Unit> Units { get; }
        public string Glyph { get; }

        public string Name => Category?.Name;
        public int CategoryId => Category?.GetModelCategoryId() ?? -1;
    }
}
