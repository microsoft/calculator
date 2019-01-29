// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    public class AboutFlyoutPom : UIObject
    {
        private const string TitleId = "Header";

        /// <summary>
        /// Initializes a new instance of the <see cref="AboutFlyoutPom" /> class.
        /// </summary>
        /// <param name="uiObject">The UIObject that is the root of the navigation menu.</param>
        public AboutFlyoutPom(UIObject uiObject) : base(uiObject)
        {
        }

        public UIObject Title
        {
            get
            {
                return new UIObject(this.Descendants.Find(TitleId));
            }
        }
    }
}
