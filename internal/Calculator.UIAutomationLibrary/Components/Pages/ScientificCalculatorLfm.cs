// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation.Waiters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Represents the Standard calculator view.
    /// </summary>
    public class ScientificCalculatorLfm
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ScientificCalculatorLfm" /> class.
        /// </summary>
        /// <param name="uiObject">The UIObject that is the root of the scientific Calculator.</param>
        public ScientificCalculatorLfm(ScientificCalculatorPom objectModel)
        {
            this.ObjectModel = objectModel;
        }

        public ScientificCalculatorPom ObjectModel { get; }

        public void Press1()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Invoking 1");
                this.ObjectModel.OneButton.Invoke();
                waiter.TryWait();
            }
        }

        public void Press2()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Invoking 2");
                this.ObjectModel.NumberPad.TwoButton.Invoke();
                waiter.TryWait();
            }
        }

        public void Press3()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Invoking 3");
                this.ObjectModel.ThreeButton.Invoke();
                waiter.TryWait();
            }
        }

        public void Press4()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Invoking 4");
                this.ObjectModel.FourButton.Invoke();
                waiter.TryWait();
            }
        }

        public void PressSqrt()
        {
            // When invoking sqrt, both the expression changes.
            using (UIEventWaiter waiter = this.ObjectModel.GetExpressionChangedWaiter())
            {
                Log.Comment("Invoking sqrt");
                this.ObjectModel.SqrtButton.Invoke();
                waiter.TryWait();
            }
        }

        public void PressMinus()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetExpressionChangedWaiter())
            {
                Log.Comment("Invoking minus");
                this.ObjectModel.MinusButton.Invoke();
                waiter.TryWait();
            }
        }

        public void PressPlus()
        {
            using (UIEventWaiter waiter = this.ObjectModel.GetExpressionChangedWaiter())
            {
                Log.Comment("Invoking plus");
                this.ObjectModel.PlusButton.Invoke();
                waiter.TryWait();
            }
        }

        public void PressEquals()
        {
            // When invoking equals, both the display and the expression change.
            using (UIEventWaiter expressionWaiter = this.ObjectModel.GetExpressionChangedWaiter())
            using (UIEventWaiter displayWaiter = this.ObjectModel.GetDisplayChangedWaiter())
            {
                Log.Comment("Invoking equals");
                this.ObjectModel.EqualButton.Invoke();
                expressionWaiter.TryWait();
                displayWaiter.TryWait();
            }
        }

        public void OnePlusTwoEnter()
        {
            Press1();
            PressPlus();
            Press2();
            PressEquals();
        }

        public void MemorySet() => this.ObjectModel.MemoryControls.SetButton.Invoke();

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
