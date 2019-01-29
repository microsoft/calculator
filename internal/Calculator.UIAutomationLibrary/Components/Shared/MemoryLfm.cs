// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Etw.Managed;
using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Waiters;
using WEX.Logging.Interop;

namespace Calculator.UIAutomationLibrary.Components
{
    public class MemoryLfm : ICanFocusWithClicks
    {
        public MemoryLfm(MemoryPom memoryPom)
        {
            this.ObjectModel = memoryPom;
        }

        public MemoryPom ObjectModel { get; }

        public void FocusWithClicks()
        {
            // On the Programming calc, the default click location can land on the first memory item, dismissing the flyout.
            // Instead, click just below, in the gutter to the left of the trash can.
            var body = this.ObjectModel.Body;
            int height = body.BoundingRectangle.Height;
            body.DoubleClick(PointerButtons.Primary, Constants.ClickMargin, height + Constants.ClickMargin);
        }

        public void OpenBody()
        {
            using (EtwWaiter waiter = new EtwWaiter(Constants.CalculatorETWProviderGUID, Constants.MemoryBodyOpenedETWEventName))
            {
                // Only one exists at a given time
                if (this.ObjectModel.IsMemoryButtonVisible)
                {
                    if (!this.ObjectModel.IsBodyOpen)
                    {
                        this.ObjectModel.MemoryButton.Invoke();
                        waiter.Wait();
                    }
                }
                else if (!this.ObjectModel.MemoryPivot.IsSelected)
                {
                    this.ObjectModel.MemoryPivot.Click();
                    waiter.Wait();
                }
            }
        }
    }
}
