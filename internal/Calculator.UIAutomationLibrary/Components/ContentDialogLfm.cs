// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Calculator.UIAutomationLibrary.Components
{
    public class ContentDialogLfm
    {
        public ContentDialogLfm(ContentDialogPom objectModel)
        {
            this.ObjectModel = objectModel;
        }

        public ContentDialogPom ObjectModel { get; }

        public void InvokePrimary()
        {
            this.ObjectModel.PrimaryButton.Invoke();
        }
        public void InvokeSecondary()
        {
            this.ObjectModel.SecondaryButton.Invoke();
        }

        public void InvokeClose()
        {
            this.ObjectModel.CloseButton.Invoke();
        }
    }
}
