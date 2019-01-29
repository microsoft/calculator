// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Waiters;

namespace Calculator.UIAutomationLibrary.Components
{
    public class UnitConverterPom : UIObject
    {
        private const string DisplayId = "Value1";
        private const string NumberPadId = "numberPad";

        public UnitConverterPom(UIObject uiObject) : base(uiObject)
        {
        }

        public NumberPadPom NumberPad => new NumberPadPom(this.Descendants.Find(NumberPadId));

        public TextBlock Display => new TextBlock(this.Descendants.Find(DisplayId));

        public PropertyChangedEventWaiter GetDisplayChangedWaiter() => this.Display.GetNameChangedWaiter();

        public ElementAddedWaiter GetDisplayElementAddedWaiter() => new ElementAddedWaiter(this, Scope.Descendants, DisplayId);
    }
}
