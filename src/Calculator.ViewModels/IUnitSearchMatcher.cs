// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp.ViewModel
{
    public interface IUnitSearchMatcher
    {
        /// <summary>
        /// Scores how well a unit answers a search query. Zero means it does not match and is left
        /// out; anything higher is a match, and results are listed best first. The scale itself is
        /// private to the implementation, since only the relative order is used.
        /// </summary>
        int Rank(string query, UnitPickerItem item);
    }
}
