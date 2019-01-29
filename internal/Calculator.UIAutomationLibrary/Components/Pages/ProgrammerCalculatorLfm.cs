// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation.Waiters;

namespace Calculator.UIAutomationLibrary.Components
{
    public class ProgrammerCalculatorLfm
    {
        public ProgrammerCalculatorLfm(ProgrammerCalculatorPom programmerCalculatorPom)
        {
            this.ObjectModel = programmerCalculatorPom;
        }

        public ProgrammerCalculatorPom ObjectModel { get; }

        public void EnsureFullKeypad()
        {
            if (!this.ObjectModel.FullKeypadButton.IsSelected)
            {
                this.ObjectModel.FullKeypadButton.Select();
            }
        }

        public void EnsureBitTogglingKeypad()
        {
            if (!this.ObjectModel.BitFlipKeypadButton.IsSelected)
            {
                this.ObjectModel.BitFlipKeypadButton.Select();
            }
        }

        public void ChangeBitLength()
        {
            this.ObjectModel.GetCurrentBitLengthButton().Invoke();
        }

        public MemoryLfm OpenMemory()
        {
            MemoryLfm lfm = new MemoryLfm(this.ObjectModel.MemoryControls);
            lfm.OpenBody();
            return lfm;
        }

        public void FiveMemorySet()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                this.ObjectModel.NumberPad.FiveButton.Invoke();
                waiter.TryWait();
            }

            this.ObjectModel.MemoryControls.SetButton.Invoke();
        }
    }
}
