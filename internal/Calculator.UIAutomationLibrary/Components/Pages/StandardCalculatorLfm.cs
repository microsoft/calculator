// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation.Waiters;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Represents the Standard calculator view.
    /// </summary>
    public class StandardCalculatorLfm
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="StandardCalculatorLfm" /> class.
        /// </summary>
        /// <param name="uiObject">The UIObject that is the root of the Standard Calculator.</param>
        public StandardCalculatorLfm(StandardCalculatorPom objectModel)
        {
            this.ObjectModel = objectModel;
        }

        public StandardCalculatorPom ObjectModel { get; }

        public void OnePlusTwoEnter()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Invoking 1");
                this.ObjectModel.NumPad.OneButton.Invoke();
                waiter.TryWait();
            }
            using (UIEventWaiter waiter = this.ObjectModel.GetExpressionChangedWaiter())
            {
                Log.Comment("Pressing +");
                this.ObjectModel.SendKeys("{ADD}");
                // PropertyChangeWaiter is unreliable for the first name changed notification
                // Bug 17624996: PropertyChanged event not fired when Name is updated for the first time for a control with custom automation peer.
                waiter.TryWait();
            }
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Pressing 2");
                this.ObjectModel.SendKeys("2");
                waiter.TryWait();
            }
            // When pressing enter, both the display and the expression change.
            using (UIEventWaiter expressionWaiter = this.ObjectModel.GetExpressionChangedWaiter())
            using (UIEventWaiter displayWaiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Invoking equals");
                this.ObjectModel.EqualButton.Invoke();
                expressionWaiter.TryWait();
                displayWaiter.TryWait();
            }
        }

        public void Clear()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Pressing escape");
                this.ObjectModel.ClearButton.Invoke();
                waiter.TryWait();
            }
        }

        public void ClearFiveMemorySet()
        {
            this.Clear();
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                this.ObjectModel.NumPad.FiveButton.Invoke();
                waiter.TryWait();
            }

            this.ObjectModel.MemoryControls.SetButton.Invoke();
        }

        public MemoryLfm OpenMemory()
        {
            MemoryLfm lfm = new MemoryLfm(this.ObjectModel.MemoryControls);
            lfm.OpenBody();
            return lfm;
        }

        public HistoryLfm OpenHistory()
        {
            HistoryLfm lfm = new HistoryLfm(this.ObjectModel.HistoryControls);
            lfm.OpenBody();
            return lfm;
        }
    }
}
