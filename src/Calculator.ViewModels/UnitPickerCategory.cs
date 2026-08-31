// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace CalculatorApp.ViewModel
{
    public enum UnitPickerCategoryLoadState
    {
        Loaded,
        Loading,
        Failed,
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class UnitPickerCategory
    {
        private IReadOnlyList<Unit> _units;

        public UnitPickerCategory(Category category, IReadOnlyList<Unit> units, string glyph = null)
        {
            Category = category;
            _units = units ?? Array.Empty<Unit>();
            Glyph = glyph;
        }

        public Category Category { get; }
        public IReadOnlyList<Unit> Units => _units;
        public string Glyph { get; }

        public string Name => Category?.Name;
        public int CategoryId => Category?.GetModelCategoryId() ?? -1;

        internal void UpdateUnits(IReadOnlyList<Unit> units)
        {
            _units = units ?? Array.Empty<Unit>();
        }
    }
}
