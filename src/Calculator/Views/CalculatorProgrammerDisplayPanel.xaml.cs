using CalculatorApp.Utils;
using CalculatorApp.ViewModel.Common;

using System.Diagnostics;
using System.Windows.Input;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class CalculatorProgrammerDisplayPanel : UserControl
    {
        public ICommand BitLengthButtonPressed
        {
            get
            {
                if (donotuse_BitLengthButtonPressed == null)
                {
                    donotuse_BitLengthButtonPressed = DelegateCommandUtils.MakeDelegateCommand(this,
                        (that, param) =>
                        {
                            that.OnBitLengthButtonPressed(param);
                        });
                }
                return donotuse_BitLengthButtonPressed;
            }
        }

        private ICommand donotuse_BitLengthButtonPressed;

        public ViewModel.StandardCalculatorViewModel Model
        {
            get
            {
                Debug.Assert(DataContext as ViewModel.StandardCalculatorViewModel != null, "static_cast result must NOT be null");
                return DataContext as ViewModel.StandardCalculatorViewModel;
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
                    string newState = m_isErrorVisualState ? "ErrorLayout" : "NoErrorLayout";
                    VisualStateManager.GoToState(this, newState, false);
                }
            }
        }

        public CalculatorProgrammerDisplayPanel()
        {
            m_isErrorVisualState = false;
            InitializeComponent();
        }

        private void OnBitLengthButtonPressed(object parameter)
        {
            string buttonId = parameter.ToString();

            QwordButton.Visibility = Visibility.Collapsed;
            DwordButton.Visibility = Visibility.Collapsed;
            WordButton.Visibility = Visibility.Collapsed;
            ByteButton.Visibility = Visibility.Collapsed;
            if (buttonId == "0")
            {
                Model.ValueBitLength = BitLength.BitLengthDWord;
                DwordButton.Visibility = Visibility.Visible;
                DwordButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "1")
            {
                Model.ValueBitLength = BitLength.BitLengthWord;
                WordButton.Visibility = Visibility.Visible;
                WordButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "2")
            {
                Model.ValueBitLength = BitLength.BitLengthByte;
                ByteButton.Visibility = Visibility.Visible;
                ByteButton.Focus(FocusState.Programmatic);
            }
            else if (buttonId == "3")
            {
                Model.ValueBitLength = BitLength.BitLengthQWord;
                QwordButton.Visibility = Visibility.Visible;
                QwordButton.Focus(FocusState.Programmatic);
            }
        }

        private bool m_isErrorVisualState;
    }
}
