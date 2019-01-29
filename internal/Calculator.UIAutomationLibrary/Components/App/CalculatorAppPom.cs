// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Calculator.UIAutomationLibrary.Components
{
    public class CalculatorAppPom : UIObject
    {
        /// <summary>
        /// Creates a new instance of the <see cref="CalculatorAppPom"/> class.
        /// </summary>
        /// <param name="uiObject">UIObject for the calculator app window.</param>
        public CalculatorAppPom(UIObject uiObject)
            : base(uiObject)
        {
        }
        public MainPagePom MainPagePom
        {
            get
            {
                return new MainPagePom(this);
            }
        }
    }
}
