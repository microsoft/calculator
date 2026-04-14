// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Controls;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using System.Diagnostics;

using Windows.UI.Xaml;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class CalculatorProgrammerOperators
    {
        public static string OrEmpty(string value) => value ?? string.Empty;

        public CalculatorProgrammerOperators()
        {
            InitializeComponent();

            CopyMenuItem.Text = AppResourceProvider.GetInstance().GetResourceString("copyMenuItem");
        }

        public StandardCalculatorViewModel ViewModel => this.DataContext as StandardCalculatorViewModel;

        public Style SymbolButtonStyle
        {
            get => (Style)GetValue(SymbolButtonStyleProperty);
            set => SetValue(SymbolButtonStyleProperty, value);
        }

        // Using a DependencyProperty as the backing store for SymbolButtonStyle.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SymbolButtonStyleProperty =
            DependencyProperty.Register(nameof(SymbolButtonStyle), typeof(Style), typeof(CalculatorProgrammerOperators), new PropertyMetadata(default(Style)));

        internal void SetRadixButton(NumberBase numberBase)
        {
            switch (numberBase)
            {
                case NumberBase.DecBase:
                    DecimalButton.IsChecked = true;
                    break;
                case NumberBase.HexBase:
                    HexButton.IsChecked = true;
                    break;
                case NumberBase.OctBase:
                    OctButton.IsChecked = true;
                    break;
                case NumberBase.BinBase:
                    BinaryButton.IsChecked = true;
                    break;
                default:
                    Debug.Assert(false);
                    break;
            }
        }

        private void DecButtonChecked(object sender, RoutedEventArgs e)
        {
            TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.DecButton, ViewMode.Programmer);
            if (ViewModel != null)
            {
                ViewModel.SwitchProgrammerModeBase(NumberBase.DecBase);
            }
        }

        private void HexButtonChecked(object sender, RoutedEventArgs e)
        {
            TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.HexButton, ViewMode.Programmer);
            if (ViewModel != null)
            {
                ViewModel.SwitchProgrammerModeBase(NumberBase.HexBase);
            }
        }

        private void BinButtonChecked(object sender, RoutedEventArgs e)
        {
            TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.BinButton, ViewMode.Programmer);
            if (ViewModel != null)
            {
                ViewModel.SwitchProgrammerModeBase(NumberBase.BinBase);
            }
        }

        private void OctButtonChecked(object sender, RoutedEventArgs e)
        {
            TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.OctButton, ViewMode.Programmer);
            if (ViewModel != null)
            {
                ViewModel.SwitchProgrammerModeBase(NumberBase.OctBase);
            }
        }

        private void OnCopyMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var source = (RadixButton)ProgrammerOperatorsContextMenu.Target;

            CopyPasteManager.CopyToClipboard(source.GetRawDisplayValue());
        }
    }
}
