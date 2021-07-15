// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp
{
    static class ViewState
    {
        public static readonly string Snap = "Snap";
        public static readonly string DockedView = "DockedView";

        public static bool IsValidViewState(string viewState)
        {
            return (viewState == Snap) || (viewState == DockedView);
        }
    }
}

