// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Calculator.UIAutomationLibrary.Components
{
    public class DateCalculatorLfm
    {
        public DateCalculatorLfm(DateCalculatorPom dateCalculatorPom)
        {
            this.ObjectModel = dateCalculatorPom;
        }

        public DateCalculatorPom ObjectModel { get; }

        public void EnsureDateDifferenceMode()
        {
            this.OpenModeSelector();
            this.ObjectModel.ModeSelector.AllItems[0].Select();
        }

        public void EnsureAddSubtractMode()
        {
            this.OpenModeSelector();
            this.ObjectModel.ModeSelector.AllItems[1].Select();
        }

        private void OpenModeSelector()
        {
            using (var waiter = this.ObjectModel.ModeSelector.GetExpandedWaiter())
            {
                this.ObjectModel.ModeSelector.Expand();
                waiter.TryWait();
            }
        }
    }
}
