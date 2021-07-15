// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Diagnostics;
using CalculatorApp;
using CalculatorApp.ViewModel;

using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using System.ComponentModel;

namespace CalculatorApp
{
    public sealed partial class KeyGraphFeaturesPanel : System.ComponentModel.INotifyPropertyChanged
    {
        public KeyGraphFeaturesPanel()
        {
            InitializeComponent();
            this.Loaded += KeyGraphFeaturesPanel_Loaded;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        internal void RaisePropertyChanged(string p)
        {
#if !UNIT_TESTS
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));
#endif
        }

        public CalculatorApp.ViewModel.EquationViewModel ViewModel
        {
            get { return m_viewModel; }
            set
            {
                if (m_viewModel != value)
                {
                    m_viewModel = value;
                    RaisePropertyChanged(nameof(ViewModel));
                }
            }
        }

        public event Windows.UI.Xaml.RoutedEventHandler KeyGraphFeaturesClosed;

        public static Windows.UI.Xaml.Media.SolidColorBrush
                     ToSolidColorBrush(Windows.UI.Color color)
        {
            return new Windows.UI.Xaml.Media.SolidColorBrush(color);
        }

        private void KeyGraphFeaturesPanel_Loaded(object sender, RoutedEventArgs e)
        {
            BackButton.Focus(FocusState.Programmatic);
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            KeyGraphFeaturesClosed?.Invoke(this, new RoutedEventArgs());
        }

        private CalculatorApp.ViewModel.EquationViewModel m_viewModel;
    }
}
