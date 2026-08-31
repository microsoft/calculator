// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp.ViewModel
{
    public interface IUnitSearchMatcher
    {
        bool IsMatch(string query, Unit unit);
    }
}
