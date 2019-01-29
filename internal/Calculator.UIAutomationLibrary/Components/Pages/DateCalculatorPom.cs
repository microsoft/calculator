// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    public class DateCalculatorPom : UIObject
    {
        private const string ModeSelectorId = "DateCalculationOption";

        public DateCalculatorPom(UIObject uiObject) : base(uiObject)
        {
        }

        public ComboBox ModeSelector
        {
            get
            {
                return new ComboBox(this.Descendants.Find(ModeSelectorId));
            }
        }
    }
}
