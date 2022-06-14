// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorStandardOperators.xaml.h
// Declaration of the CalculatorStandardOperators class
//

using Windows.UI.Xaml;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class CalculatorStandardOperators
    {
        public CalculatorStandardOperators()
        {
            m_isErrorVisualState = false;
            InitializeComponent();
        }

        public bool IsErrorVisualState
        {
            get => m_isErrorVisualState;
            set
            {
                if (m_isErrorVisualState != value)
                {
                    m_isErrorVisualState = value;
                    string newState = m_isErrorVisualState ? "ErrorLayout" : "NoErrorLayout";
                    VisualStateManager.GoToState(this, newState, false);
                    NumberPad.IsErrorVisualState = m_isErrorVisualState;
                }
            }
        }
        private bool m_isErrorVisualState;
    }
}
