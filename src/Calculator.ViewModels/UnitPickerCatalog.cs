// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;

namespace CalculatorApp.ViewModel
{
    public static class UnitPickerCatalog
    {
        public static IReadOnlyList<UnitPickerCategory> Build(
            IReadOnlyList<Category> categories,
            IReadOnlyList<IReadOnlyList<Unit>> unitsByCategory,
            IReadOnlyList<string> glyphs = null)
        {
            var catalog = new List<UnitPickerCategory>();
            for (int i = 0; i < categories.Count; i++)
            {
                var category = categories[i];
                if (category == null)
                {
                    continue;
                }

                IReadOnlyList<Unit> units = (unitsByCategory != null && i < unitsByCategory.Count)
                    ? unitsByCategory[i]
                    : null;
                string glyph = (glyphs != null && i < glyphs.Count) ? glyphs[i] : null;
                catalog.Add(new UnitPickerCategory(category, units, glyph));
            }

            return catalog;
        }
    }
}
