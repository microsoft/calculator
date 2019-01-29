// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    public class MemoryPom : UIObject
    {
        private const string ClearMemoryButtonId = "ClearMemoryButton";
        private const string RecallButtonId = "MemRecall";
        private const string PlusButtonId = "MemPlus";
        private const string MinusButtonId = "MemMinus";
        private const string SetButtonId = "memButton";
        private const string MemoryButtonId = "MemoryButton";
        private const string MemoryPivotId = "MemoryLabel";
        private const string BodyId = "MemoryListView";

        public MemoryPom(UIObject uiObject) : base(uiObject)
        {
        }

        public Button ClearButton => new Button(this.Descendants.Find(ClearMemoryButtonId));

        public Button RecallButton => new Button(this.Descendants.Find(RecallButtonId));

        public Button PlusButton => new Button(this.Descendants.Find(PlusButtonId));

        public Button MinusButton => new Button(this.Descendants.Find(MinusButtonId));

        public Button SetButton => new Button(this.Descendants.Find(SetButtonId));

        public Button MemoryButton => new Button(this.Descendants.Find(MemoryButtonId));

        public bool IsMemoryButtonVisible => this.DoesDescendantExist(MemoryButtonId);

        public TabItem MemoryPivot => new TabItem(this.Descendants.Find(MemoryPivotId));

        public UIObject Body => this.Descendants.Find(BodyId);

        public bool IsBodyOpen => this.DoesDescendantExist(BodyId);
    }
}
