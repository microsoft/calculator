// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    public class HistoryPom : UIObject
    {
        private const string HistoryButtonId = "HistoryButton";
        private const string HistoryPivotId = "HistoryLabel";
        private const string BodyId = "HistoryListView";

        public HistoryPom(UIObject uiObject) : base(uiObject)
        {
        }

        public Button HistoryButton => new Button(this.Descendants.Find(HistoryButtonId));

        public bool IsHistoryButtonVisible => this.DoesDescendantExist(HistoryButtonId);

        public TabItem HistoryPivot => new TabItem(this.Descendants.Find(HistoryPivotId));

        public UIObject Body => this.Descendants.Find(BodyId);

        public bool IsBodyOpen => this.DoesDescendantExist(BodyId);
    }
}
