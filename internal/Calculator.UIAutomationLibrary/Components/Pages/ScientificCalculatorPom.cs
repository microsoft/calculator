// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Represents the Scientific Calculator
    /// </summary>
    public class ScientificCalculatorPom : CalculatorBasePom
    {
        private const string NumberPadId = "NumberPad";
        private const string StandardOperatorsId = "StandardOperators";
        private const string DisplayControlsId = "DisplayControls";
        private const string ScientificFunctionsId = "ScientificFunctions";
        private const string OneButtonId = "num1Button";
        private const string ThreeButtonId = "num3Button";
        private const string FourButtonId = "num4Button";
        private const string SqrtButtonId = "squareRootButton";
        private const string MinusButtonId = "minusButton";
        private const string PlusButtonId = "plusButton";
        private const string EqualButtonId = "equalButton";
        private const string ClearButtonId = "clearButton";

        /// <summary>
        /// Initializes a new instance of the <see cref="ScientificCalculatorPom" /> class.
        /// </summary>
        /// <param name="uiObject">The UIObject that is the root of the scientific calculator.</param>
        public ScientificCalculatorPom(UIObject uiObject) : base(uiObject)
        {
        }

        public UIObject StandardOperatorsGroup => this.Descendants.Find(StandardOperatorsId);

        public UIObject DisplayControlsGroup => this.Descendants.Find(DisplayControlsId);

        public UIObject ScientificFunctionsGroup => this.Descendants.Find(ScientificFunctionsId);

        public Button OneButton => this.NumberPad.OneButton;

        public Button ThreeButton => this.NumberPad.ThreeButton;

        public Button FourButton => this.NumberPad.FourButton;

        public Button SqrtButton => new Button(this.ScientificFunctionsGroup.Children.Find(SqrtButtonId));

        public Button MinusButton => new Button(this.StandardOperatorsGroup.Children.Find(MinusButtonId));

        public Button PlusButton => new Button(this.StandardOperatorsGroup.Children.Find(PlusButtonId));

        public Button EqualButton => new Button(this.StandardOperatorsGroup.Children.Find(EqualButtonId));

        public Button ClearButton => new Button(this.DisplayControlsGroup.Children.Find(ClearButtonId));

        public NumberPadPom NumberPad => new NumberPadPom(this.Descendants.Find(NumberPadId));

        public HistoryPom HistoryControls => new HistoryPom(this);

        public MemoryPom MemoryControls => new MemoryPom(this);
    }
}
