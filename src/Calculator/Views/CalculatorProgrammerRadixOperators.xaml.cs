// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class CalculatorProgrammerRadixOperators
    {
        public CalculatorProgrammerRadixOperators()
        {
            m_isErrorVisualState = false;
            InitializeComponent();
            LoadResourceStrings();
        }

        public StandardCalculatorViewModel Model => (StandardCalculatorViewModel)this.DataContext;

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

        public string ParenthesisCountToString(uint count)
        {
            return (count == 0) ? "" : count.ToString();
        }

        public void checkDefaultBitShift()
        {
            LoadDeferredLoadButtons();

            if (IsButtonLoaded())
            {
                return;
            }

            CollapseBitshiftButtons();

            m_selectedShiftButtonMode = BitShiftMode.Arithmetic;
            LshButton.Visibility = Visibility.Visible;
            RshButton.Visibility = Visibility.Visible;
            LshButton.IsEnabled = true;
            RshButton.IsEnabled = true;
        }

        private enum BitShiftMode
        {
            Arithmetic,
            LogicalShift,
            RotateCircular,
            RotateCarry
        };

        private void BitshiftFlyout_Checked(object sender, RoutedEventArgs e)
        {
            // Load deferred load buttons
            LoadDeferredLoadButtons();

            if (IsButtonLoaded())
            {
                return;
            }

            CollapseBitshiftButtons();

            var radioButton = (RadioButton)sender;
            string announcementString = "";
            BitShiftMode selectedButtonMode = m_selectedShiftButtonMode;

            if (radioButton == ArithmeticShiftButton)
            {
                LshButton.Visibility = Visibility.Visible;
                RshButton.Visibility = Visibility.Visible;
                LshButton.IsEnabled = true;
                RshButton.IsEnabled = true;
                announcementString = m_arithmeticShiftButtonContent;
                selectedButtonMode = BitShiftMode.Arithmetic;
            }
            else if (radioButton == LogicalShiftButton)
            {
                LshLogicalButton.Visibility = Visibility.Visible;
                RshLogicalButton.Visibility = Visibility.Visible;
                LshLogicalButton.IsEnabled = true;
                RshLogicalButton.IsEnabled = true;
                announcementString = m_logicalShiftButtonContent;
                selectedButtonMode = BitShiftMode.LogicalShift;
            }
            else if (radioButton == RotateCircularButton)
            {
                RolButton.Visibility = Visibility.Visible;
                RorButton.Visibility = Visibility.Visible;
                RolButton.IsEnabled = true;
                RorButton.IsEnabled = true;
                announcementString = m_rotateCircularButtonContent;
                selectedButtonMode = BitShiftMode.RotateCircular;
            }
            else if (radioButton == RotateCarryShiftButton)
            {
                RolCarryButton.Visibility = Visibility.Visible;
                RorCarryButton.Visibility = Visibility.Visible;
                RolCarryButton.IsEnabled = true;
                RorCarryButton.IsEnabled = true;
                announcementString = m_rotateCarryShiftButtonContent;
                selectedButtonMode = BitShiftMode.RotateCarry;
            }

            if (selectedButtonMode != m_selectedShiftButtonMode)
            {
                this.BitShiftFlyout.Hide();
                m_selectedShiftButtonMode = selectedButtonMode;
            }

            Model.SetBitshiftRadioButtonCheckedAnnouncement(announcementString);
        }

        private void FlyoutButton_Clicked(object sender, RoutedEventArgs e)
        {
            this.BitwiseFlyout.Hide();
        }

        private void CollapseBitshiftButtons()
        {
            RolButton.Visibility = Visibility.Collapsed;
            RorButton.Visibility = Visibility.Collapsed;
            RolCarryButton.Visibility = Visibility.Collapsed;
            RorCarryButton.Visibility = Visibility.Collapsed;
            LshButton.Visibility = Visibility.Collapsed;
            RshButton.Visibility = Visibility.Collapsed;
            LshLogicalButton.Visibility = Visibility.Collapsed;
            RshLogicalButton.Visibility = Visibility.Collapsed;

            // We need to set the collapsed buttons to disabled so that the KeyboardShortcutManager can skip the keybinds for the disabled buttons
            RolButton.IsEnabled = false;
            RorButton.IsEnabled = false;
            RolCarryButton.IsEnabled = false;
            RorCarryButton.IsEnabled = false;
            LshButton.IsEnabled = false;
            RshButton.IsEnabled = false;
            LshLogicalButton.IsEnabled = false;
            RshLogicalButton.IsEnabled = false;
        }

        private void LoadResourceStrings()
        {
            var resProvider = AppResourceProvider.GetInstance();
            m_arithmeticShiftButtonContent = resProvider.GetResourceString("arithmeticShiftButtonSelected");
            m_logicalShiftButtonContent = resProvider.GetResourceString("logicalShiftButtonSelected");
            m_rotateCircularButtonContent = resProvider.GetResourceString("rotateCircularButtonSelected");
            m_rotateCarryShiftButtonContent = resProvider.GetResourceString("rotateCarryShiftButtonSelected");
        }

        private void LoadDeferredLoadButtons()
        {
            // Load deferred load buttons
            if (RolButton == null)
            {
                FindName("RolButton");
                FindName("RorButton");
                FindName("RolCarryButton");
                FindName("RorCarryButton");
                FindName("LshLogicalButton");
                FindName("RshLogicalButton");
            }
        }

        private bool IsButtonLoaded()
        {
            // Since arithmeticShiftButton defaults to IsChecked = true, this event an fire before we can load the deferred loaded controls. If that is the case, just
            // return and do nothing.
            return RolButton == null || RorButton == null || RolCarryButton == null || RorCarryButton == null || LshLogicalButton == null
                   || RshLogicalButton == null;
        }

        private bool m_isErrorVisualState;

        private void OpenParenthesisButton_GotFocus(object sender, RoutedEventArgs e)
        {
            Model.SetOpenParenthesisCountNarratorAnnouncement();
        }

        private void ClearEntryButton_LostFocus(object sender, RoutedEventArgs e)
        {
            if (ClearEntryButton.Visibility == Visibility.Collapsed && ClearButton.Visibility == Visibility.Visible)
            {
                ClearButton.Focus(FocusState.Programmatic);
            }
        }

        private void ClearButton_LostFocus(object sender, RoutedEventArgs e)
        {
            if (ClearEntryButton.Visibility == Visibility.Visible && ClearButton.Visibility == Visibility.Collapsed)
            {
                ClearEntryButton.Focus(FocusState.Programmatic);
            }
        }

        private void BitShiftFlyout_Opened(object sender, object e)
        {
            if (m_selectedShiftButtonMode == BitShiftMode.Arithmetic)
            {
                ArithmeticShiftButton.IsChecked = true;
            }
            else if (m_selectedShiftButtonMode == BitShiftMode.LogicalShift)
            {
                LogicalShiftButton.IsChecked = true;
            }
            else if (m_selectedShiftButtonMode == BitShiftMode.RotateCircular)
            {
                RotateCircularButton.IsChecked = true;
            }
            else if (m_selectedShiftButtonMode == BitShiftMode.RotateCarry)
            {
                RotateCarryShiftButton.IsChecked = true;
            }
        }

        private BitShiftMode m_selectedShiftButtonMode;
        private string m_arithmeticShiftButtonContent;
        private string m_logicalShiftButtonContent;
        private string m_rotateCircularButtonContent;
        private string m_rotateCarryShiftButtonContent;
    }
}
