// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    public class NumberPadPom : UIObject
    {
        private const string OneButtonId = "num1Button";
        private const string TwoButtonId = "num2Button";
        private const string ThreeButtonId = "num3Button";
        private const string FourButtonId = "num4Button";
        private const string FiveButtonId = "num5Button";
        private const string SixButtonId = "num6Button";
        private const string SevenButtonId = "num7Button";
        private const string EightButtonId = "num8Button";
        private const string NineButtonId = "num9Button";
        private const string ZeroButtonId = "num0Button";
        private const string DecimalButtonId = "decimalSeparatorButton";

        public NumberPadPom(UIObject uiObject) : base(uiObject)
        {
        }

        public Button ZeroButton => new Button(this.Children.Find(ZeroButtonId));

        public Button OneButton => new Button(this.Children.Find(OneButtonId));

        public Button TwoButton => new Button(this.Children.Find(TwoButtonId));

        public Button ThreeButton => new Button(this.Children.Find(ThreeButtonId));

        public Button FourButton => new Button(this.Children.Find(FourButtonId));

        public Button FiveButton => new Button(this.Children.Find(FiveButtonId));

        public Button SixButton => new Button(this.Children.Find(SixButtonId));

        public Button SevenButton => new Button(this.Children.Find(SevenButtonId));

        public Button EightButton => new Button(this.Children.Find(EightButtonId));

        public Button NineButton => new Button(this.Children.Find(NineButtonId));

        public Button DecimalButton => new Button(this.Children.Find(DecimalButtonId));
    }
}
