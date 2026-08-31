// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalculatorApp.ViewModel
{
    public sealed class SubstringUnitSearchMatcher : IUnitSearchMatcher
    {
        public bool IsMatch(string query, Unit unit)
        {
            if (unit == null)
            {
                return false;
            }

            if (string.IsNullOrEmpty(query))
            {
                return true;
            }

            return Contains(unit.Name, query) || Contains(unit.Abbreviation, query);
        }

        private static bool Contains(string value, string query) =>
            !string.IsNullOrEmpty(value) && value.IndexOf(query, StringComparison.OrdinalIgnoreCase) >= 0;
    }
}
