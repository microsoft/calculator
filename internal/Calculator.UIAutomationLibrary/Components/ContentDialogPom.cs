// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    public class ContentDialogPom : UIObject
    {
        private static readonly UICondition textScrollViewerCondition = UICondition.CreateFromId("ContentScrollViewer");
        private static readonly UICondition titleTextBlockCondition = UICondition.CreateFromClassName("TextBlock");

        private static readonly UICondition primaryButtonCondition =
            UICondition.CreateFromClassName("Button")
                .AndWith(UICondition.CreateFromId("PrimaryButton"));
        private static readonly UICondition secondaryButtonCondition =
            UICondition.CreateFromClassName("Button")
                .AndWith(UICondition.CreateFromId("SecondaryButton"));
        private static readonly UICondition closeButtonCondition =
            UICondition.CreateFromClassName("Button")
                .AndWith(UICondition.CreateFromId("CloseButton"));

        public ContentDialogPom(UIObject uiObject) : base(uiObject)
        {
        }

        public Button PrimaryButton
        {
            get
            {
                return new Button(this.Children.Find(primaryButtonCondition));
            }
        }

        public Button SecondaryButton
        {
            get
            {
                return new Button(this.Children.Find(secondaryButtonCondition));
            }
        }

        public Button CloseButton
        {
            get
            {
                return new Button(this.Children.Find(closeButtonCondition));
            }
        }

        public string Title
        {
            get
            {
                var scrollViewer = this.Children.Find(textScrollViewerCondition);
                var textBlock = scrollViewer.Children.Find(titleTextBlockCondition);
                return textBlock.Name;
            }
        }
    }
}
