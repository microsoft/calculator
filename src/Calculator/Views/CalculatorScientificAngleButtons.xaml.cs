// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificAngleButtons.xaml.h
// Declaration of the CalculatorScientificAngleButtons class
//

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Diagnostics;
using CalculatorApp;
using CalculatorApp.Utils;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.ViewManagement;
using Windows.UI.Core;

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

        public StandardCalculatorViewModel Model
        {
            get { return (StandardCalculatorViewModel)this.DataContext; }
        }

        public System.Windows.Input.ICommand ButtonPressed
        {
            get
            {
                if (donotuse_ButtonPressed == null)
                {
                    donotuse_ButtonPressed = DelegateCommandUtils.MakeDelegateCommand(this,
                        (that, param)=>
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
                if(m_isErrorVisualState != value)
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

        private void FToEButton_Toggled(object sender, RoutedEventArgs e)
        {
            var viewModel = (StandardCalculatorViewModel)this.DataContext;
            viewModel.FtoEButtonToggled();
        }

        private bool m_isErrorVisualState;
    }
}
