// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Waiters;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Represents the Standard Calculator
    /// </summary>
    public class StandardCalculatorPom : CalculatorBasePom
    {
        private const string CalculatorResultsId = "CalculatorResults";
        private const string ExpressionContainerId = "CalculatorExpression";
        private const string NumberPadId = "NumberPad";
        private const string StandardOperatorsId = "StandardOperators";
        private const string DisplayControlsId = "DisplayControls";
        private const string EqualButtonId = "equalButton";
        private const string ClearButtonId = "clearButton";

        /// <summary>
        /// Initializes a new instance of the <see cref="StandardCalculatorPom" /> class.
        /// </summary>
        /// <param name="uiObject">The UIObject that is the root of the standard calculator.</param>
        public StandardCalculatorPom(UIObject uiObject) : base(uiObject)
        {
        }        

        public NumberPadPom NumPad =>  new NumberPadPom(this.Descendants.Find(NumberPadId));

        public MemoryPom MemoryControls => new MemoryPom(this);

        public HistoryPom HistoryControls => new HistoryPom(this);

        public UIObject StandardOperatorsGroup => this.Descendants.Find(StandardOperatorsId);

        public UIObject DisplayControlsGroup => this.Descendants.Find(DisplayControlsId);

        public Button EqualButton => new Button(this.StandardOperatorsGroup.Children.Find(EqualButtonId));

        public Button ClearButton => new Button(this.DisplayControlsGroup.Children.Find(ClearButtonId));
    }
}
