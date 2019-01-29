// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Etw.Managed;
using MS.Internal.Mita.Foundation;

namespace Calculator.UIAutomationLibrary.Components
{
    public class HistoryLfm : ICanFocusWithClicks
    {
        public HistoryLfm(HistoryPom historyPom)
        {
            this.ObjectModel = historyPom;
        }

        public HistoryPom ObjectModel { get; }

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
            using (EtwWaiter waiter = new EtwWaiter(Constants.CalculatorETWProviderGUID, Constants.HistoryBodyOpenedETWEventName))
            {
                // Only one exists at a given time.
                if (this.ObjectModel.IsHistoryButtonVisible)
                {
                    if (!this.ObjectModel.IsBodyOpen)
                    {
                        this.ObjectModel.HistoryButton.Invoke();
                        waiter.Wait();
                    }
                }
                else if (!this.ObjectModel.HistoryPivot.IsSelected)
                {
                    this.ObjectModel.HistoryPivot.Click();
                    waiter.Wait();
                }
            }
        }
    }
}
