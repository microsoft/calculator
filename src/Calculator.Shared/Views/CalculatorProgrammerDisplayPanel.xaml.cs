using CalculatorApp;
using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Windows.Input;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed partial class CalculatorProgrammerDisplayPanel : UserControl
    {
        bool m_isErrorVisualState;

        public ICommand BitLengthButtonPressed { get; }

        StandardCalculatorViewModel Model => (CalculatorApp.ViewModel.StandardCalculatorViewModel)(this.DataContext);

        public CalculatorProgrammerDisplayPanel()
        {
            this.InitializeComponent();
            m_isErrorVisualState = false;
            BitLengthButtonPressed = new DelegateCommand(OnBitLengthButtonPressed);
        }

        void ShowBitFlip(object sender, RoutedEventArgs e)
        {
            // TraceLogger.GetInstance().LogBitFlipPaneClicked();
        }

        void OnBitLengthButtonPressed(object parameter)
        {
            // TraceLogger.GetInstance().LogBitLengthButtonUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            String buttonId = parameter.ToString();

            qwordButton.Visibility = Visibility.Collapsed;
            dwordButton.Visibility = Visibility.Collapsed;
            wordButton.Visibility = Visibility.Collapsed;
            byteButton.Visibility = Visibility.Collapsed;
            if (buttonId == "0")
            {
                Model.ButtonPressed.Execute(NumbersAndOperatorsEnum.Dword);
                dwordButton.Visibility = Visibility.Visible;
                dwordButton.Focus(FocusState.Programmatic);
                Model.IsQwordEnabled = false;
                Model.IsDwordEnabled = true;
                Model.IsWordEnabled = true;
            }
            else if (buttonId == "1")
            {
                Model.ButtonPressed.Execute(NumbersAndOperatorsEnum.Word);
                wordButton.Visibility = Visibility.Visible;
                wordButton.Focus(FocusState.Programmatic);
                Model.IsQwordEnabled = false;
                Model.IsDwordEnabled = false;
                Model.IsWordEnabled = true;
            }
            else if (buttonId == "2")
            {
                Model.ButtonPressed.Execute(NumbersAndOperatorsEnum.Byte);
                byteButton.Visibility = Visibility.Visible;
                byteButton.Focus(FocusState.Programmatic);
                Model.IsQwordEnabled = false;
                Model.IsDwordEnabled = false;
                Model.IsWordEnabled = false;
            }
            else if (buttonId == "3")
            {
                Model.ButtonPressed.Execute(NumbersAndOperatorsEnum.Qword);
                qwordButton.Visibility = Visibility.Visible;
                qwordButton.Focus(FocusState.Programmatic);
                Model.IsQwordEnabled = true;
                Model.IsDwordEnabled = true;
                Model.IsWordEnabled = true;
            }
            // update memory list according to bit length
            Model.SetMemorizedNumbersString();
        }

        bool IsErrorVisualState {
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
