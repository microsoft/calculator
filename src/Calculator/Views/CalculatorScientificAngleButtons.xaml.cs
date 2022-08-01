// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificAngleButtons.xaml.h
// Declaration of the CalculatorScientificAngleButtons class
//

using CalculatorApp.Utils;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using Windows.UI.Xaml;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class CalculatorScientificAngleButtons
    {
        public CalculatorScientificAngleButtons()       
        {
            m_isErrorVisualState = false;
            InitializeComponent();                      
        }

        public StandardCalculatorViewModel Model => (StandardCalculatorViewModel)this.DataContext;

        public System.Windows.Input.ICommand ButtonPressed
        {
            get
            {
                if (donotuse_ButtonPressed == null)
                {
                    donotuse_ButtonPressed = DelegateCommandUtils.MakeDelegateCommand(this,
                        (that, param) =>
                        {
                            that.OnAngleButtonPressed(param);
                        });
                }
                return donotuse_ButtonPressed;
            }
        }
        private System.Windows.Input.ICommand donotuse_ButtonPressed;

        public bool IsErrorVisualState
        {
            get => m_isErrorVisualState;
            set
            {
                if (m_isErrorVisualState != value)
                {
                    m_isErrorVisualState = value;
                    string newState = m_isErrorVisualState ? "ErrorFlyout" : "NoErrorFlyout";
                    VisualStateManager.GoToState(this, newState, false);
                }
            }
        }

        private void OnAngleButtonPressed(object commandParameter)
        {
            string buttonId = (string)commandParameter;

            DegreeButton.Visibility = Visibility.Collapsed;
            RadianButton.Visibility = Visibility.Collapsed;
            GradsButton.Visibility = Visibility.Collapsed;

            if (buttonId == "0")
            {
                Model.SwitchAngleType(NumbersAndOperatorsEnum.Radians);
                RadianButton.Visibility = Visibility.Visible;
                RadianButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "1")
            {
                Model.SwitchAngleType(NumbersAndOperatorsEnum.Grads);
                GradsButton.Visibility = Visibility.Visible;
                GradsButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "2")
            {
                Model.SwitchAngleType(NumbersAndOperatorsEnum.Degree);
                DegreeButton.Visibility = Visibility.Visible;
                DegreeButton.Focus(FocusState.Programmatic);
            }
        }

        public System.Windows.Input.ICommand NotationPressed
        {
            get
            {
                if (donotuse_NotationPressed == null)
                {
                    donotuse_NotationPressed = DelegateCommandUtils.MakeDelegateCommand(this,
                        (that, param) =>
                        {
                            that.OnNotationButtonPressed(param);
                        });
                }
                return donotuse_NotationPressed;
            }
        }
        private System.Windows.Input.ICommand donotuse_NotationPressed;

        private void OnNotationButtonPressed(object commandParameter)
        {
            string buttonId = (string)commandParameter;

            AutoButton.Visibility = Visibility.Collapsed;
            SciButton.Visibility = Visibility.Collapsed;
            EngButton.Visibility = Visibility.Collapsed;

            if (buttonId == "0")
            {
                Model.FtoEButtonToggled();
                SciButton.Visibility = Visibility.Visible;
                SciButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "1")
            {
                Model.EngButton();
                EngButton.Visibility = Visibility.Visible;
                EngButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "2")
            {
                Model.FtoEButtonToggled();
                AutoButton.Visibility = Visibility.Visible;
                AutoButton.Focus(FocusState.Programmatic);
            }
        }

        private bool m_isErrorVisualState;
    }
}
