// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Calculator.UIAutomationLibrary.Components
{
    public class AboutFlyoutLfm : ICanFocusWithClicks
    {
        private const string FlyoutId = "FlyoutNav";

        /// <summary>
        /// Initializes a new instance of the <see cref="AboutFlyoutLfm" /> class.
        /// </summary>
        /// <param name="objectModel">The AboutFlyoutPom that represents the About flyout panel.</param>
        public AboutFlyoutLfm(AboutFlyoutPom objectModel)
        {
            this.ObjectModel = objectModel;
        }

        public AboutFlyoutPom ObjectModel { get; }

        public void FocusWithClicks()
        {
            this.ObjectModel.Title.DoubleClick();
        }

        public void Close()
        {
            this.ObjectModel.SendKeys("{ESC}");
        }
    }
}
