// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Calculator.UIAutomationLibrary.Components
{
    public class UnitConverterLfm
    {
        public UnitConverterLfm(UnitConverterPom unitConverterPom)
        {
            this.ObjectModel = unitConverterPom;
        }

        public UnitConverterPom ObjectModel { get; }

        public void Eight()
        {
            using (var waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                this.ObjectModel.NumberPad.EightButton.Invoke();
                waiter.TryWait();
            }
        }
    }
}
