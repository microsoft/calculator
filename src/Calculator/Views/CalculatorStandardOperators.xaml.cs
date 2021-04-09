// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorStandardOperators.xaml.h
// Declaration of the CalculatorStandardOperators class
//

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Diagnostics;
using CalculatorApp;

using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

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
            get { return m_isErrorVisualState; }
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
