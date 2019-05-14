using System;
using System.Collections.Generic;
using System.Text;

namespace CalculatorApp
{
    public class ViewState
    {
        public const string Snap = "Snap";
        public const string DockedView = "DockedView";

        public bool IsValidViewState(string viewState) => viewState.Equals(ViewState.Snap) || viewState.Equals(ViewState.DockedView);
    }
}
