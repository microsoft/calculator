using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using CalculatorApp.ViewModel;
using System.Windows.Input;
using CalculatorApp.Common;

namespace CalculatorApp
{
    public sealed partial class CalculatorScientificAngleButtons : UserControl
    {
        bool m_isErrorVisualState;

        public StandardCalculatorViewModel Model => (StandardCalculatorViewModel)(DataContext);

        public ICommand ButtonPressed;


        public CalculatorScientificAngleButtons()
        {
            this.InitializeComponent();
            ButtonPressed = new DelegateCommand(OnAngleButtonPressed);
            m_isErrorVisualState = false;
        }

        void HypButton_Toggled(object sender, RoutedEventArgs e)
        {
            // TraceLogger.GetInstance().LogHypButtonUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
        }

        void FToEButton_Toggled(object sender, RoutedEventArgs e)
        {
            var viewModel = (StandardCalculatorViewModel)(this.DataContext);
            viewModel.FtoEButtonToggled();
        }

        void OnAngleButtonPressed(object commandParameter)
        {
            // TraceLogger.GetInstance().LogAngleButtonUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            String buttonId = (String)(commandParameter);

            degreeButton.Visibility = Visibility.Collapsed;
            radianButton.Visibility = Visibility.Collapsed;
            gradsButton.Visibility = Visibility.Collapsed;

            if (buttonId == "0")
            {
                Model.SwitchAngleType(NumbersAndOperatorsEnum.Radians);
                radianButton.Visibility = Visibility.Visible;
                radianButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "1")
            {
                Model.SwitchAngleType(NumbersAndOperatorsEnum.Grads);
                gradsButton.Visibility = Visibility.Visible;
                gradsButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "2")
            {
                Model.SwitchAngleType(NumbersAndOperatorsEnum.Degree);
                degreeButton.Visibility = Visibility.Visible;
                degreeButton.Focus(FocusState.Programmatic);
            }
        }

        public bool IsErrorVisualState
        {
            get => m_isErrorVisualState;

            set
            {
                if (m_isErrorVisualState != value)
                {
                    m_isErrorVisualState = value;
                    String newState = m_isErrorVisualState ? "ErrorLayout" : "NoErrorLayout";
                    VisualStateManager.GoToState(this, newState, false);
                }
            }
        }
    }
}
