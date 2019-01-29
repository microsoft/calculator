// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using MS.Internal.Mita.Foundation;
using MS.Internal.Mita.Foundation.Controls;
using MS.Internal.Mita.Foundation.Waiters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Calculator.UIAutomationLibrary.Components
{
    /// <summary>
    /// Represents the Display section of the calculator modes.
    /// </summary>
    public class CalculatorBasePom : UIObject
    {
        private const string ExpressionContainerId = "CalculatorExpression";
        private const string NormalOutputId = "normalOutput";

        /// <summary>
        /// Initializes a new instance of the <see cref="StandardCalculatorPom" /> class.
        /// </summary>
        /// <param name="uiObject">The UIObject that is the root of the standard calculator.</param>
        public CalculatorBasePom(UIObject uiObject) : base(uiObject)
        {
        }

        public TextBlock Expression
        {
            get
            {
                return new TextBlock(this.Descendants.Find(ExpressionContainerId));
            }
        }

        public UIEventWaiter GetExpressionChangedWaiter()
        {
            return new PropertyChangedEventWaiter(this.Expression, UIProperty.Get("Name"));
        }

        public TextBlock Display
        {
            get
            {
                return new TextBlock(this.Descendants.Find(NormalOutputId));
            }
        }

        public UIEventWaiter GetDisplayChangedWaiter()
        {
            return new PropertyChangedEventWaiter(this.Display, UIProperty.Get("Name"));
        }
    }
}
