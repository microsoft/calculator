// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.OneCoreUap.Test.AppModel;
using MS.Internal.Mita.Foundation;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Calculator.UIAutomationLibrary.Components
{
    public class CalculatorAppLfm
    {
        private readonly IViewDescriptor viewDescriptor;

        public CalculatorAppLfm(CalculatorAppPom objectModel, IViewDescriptor viewDescriptor)
        {
            this.ObjectModel = objectModel;
            this.viewDescriptor = viewDescriptor;
        }

        public CalculatorAppPom ObjectModel { get; }

        public  MainPageLfm MainPageLfm
        {
            get
            {
                return new MainPageLfm(this.ObjectModel.MainPagePom);
            }
        }

        public void Close()
        {
            // ObjectModel is essentially the window ui object.
            if (this.viewDescriptor != null)
            {
                NavigationHelper.CloseApplication(this.viewDescriptor.AUMID);
            }
        }
    }
}
