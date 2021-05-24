// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Diagnostics;
using CalculatorApp;
using CalculatorApp.Controls;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using Windows.Devices.Input;

using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.ViewManagement;
using Windows.UI.Core;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class CalculatorProgrammerOperators
    {
        public CalculatorProgrammerOperators()
        {
            InitializeComponent();

            CopyMenuItem.Text = AppResourceProvider.GetInstance().GetResourceString("copyMenuItem");
        }

        public StandardCalculatorViewModel Model
        {
            get { return (StandardCalculatorViewModel)this.DataContext; }
        }

        public Style SymbolButtonStyle
        {
            get { return (Style)GetValue(SymbolButtonStyleProperty); }
            set { SetValue(SymbolButtonStyleProperty, value); }
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
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(NumberBase.DecBase);
            }
        }

        private void HexButtonChecked(object sender, RoutedEventArgs e)
        {
            TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.HexButton, ViewMode.Programmer);
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(NumberBase.HexBase);
            }
        }

        private void BinButtonChecked(object sender, RoutedEventArgs e)
        {
            TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.BinButton, ViewMode.Programmer);
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(NumberBase.BinBase);
            }
        }

        private void OctButtonChecked(object sender, RoutedEventArgs e)
        {
            TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.OctButton, ViewMode.Programmer);
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(NumberBase.OctBase);
            }
        }

        private void OnCopyMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var source = (RadixButton)ProgrammerOperatorsContextMenu.Target;

            CopyPasteManager.CopyToClipboard(source.GetRawDisplayValue());
        }
    }
}
