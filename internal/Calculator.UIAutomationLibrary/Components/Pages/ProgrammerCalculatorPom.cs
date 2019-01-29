// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Waiters;

namespace Calculator.UIAutomationLibrary.Components
{
    public class ProgrammerCalculatorPom : UIObject
    {
        private const string FullKeypadButtonId = "fullKeypad";
        private const string BitFlipKeypadButtonId = "bitFlip";
        private const string CalculatorResultsId = "CalculatorResults";
        private const string NumberPadId = "NumberPad";

        private readonly string[] BitLengthButtonIds =
        {
            "qwordButton",
            "dwordButton",
            "wordButton",
            "byteButton"
        };

        public ProgrammerCalculatorPom(UIObject uiObject) : base(uiObject)
        {
        }

        public NumberPadPom NumberPad => new NumberPadPom(this.Descendants.Find(NumberPadId));

        public MemoryPom MemoryControls => new MemoryPom(this);

        public RadioButton FullKeypadButton => new RadioButton(this.Descendants.Find(FullKeypadButtonId));

        public RadioButton BitFlipKeypadButton => new RadioButton(this.Descendants.Find(BitFlipKeypadButtonId));

        public TextBlock Display => new TextBlock(this.Descendants.Find(CalculatorResultsId));

        public UIEventWaiter GetDisplayChangedWaiter() => this.Display.GetNameChangedWaiter();

        public Button GetCurrentBitLengthButton()
        {
            // There are four bit length buttons, with only one visible at a time.
            UIObject button = null;
            foreach (var buttonId in this.BitLengthButtonIds)
            {
                if (this.Descendants.TryFind(buttonId, out button))
                {
                    break;
                }
            }

            return new Button(button);
        }
    }
}
