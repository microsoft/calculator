// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel
{
    public struct VariableChangedEventArgs
    {
        public string VariableName;
        public double NewValue;
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class GraphingCalculatorViewModel : INotifyPropertyChanged
    {
        private bool _isDecimalEnabled;
        private ObservableCollection<EquationViewModel> _equations;
        private ObservableCollection<VariableViewModel> _variables;
        private EquationViewModel _selectedEquation;

        public event PropertyChangedEventHandler PropertyChanged;
        public event System.EventHandler<VariableChangedEventArgs> VariableUpdated;

        public GraphingCalculatorViewModel()
        {
            _equations = new ObservableCollection<EquationViewModel>();
            _variables = new ObservableCollection<VariableViewModel>();
            _isDecimalEnabled = true;
        }

        public bool IsDecimalEnabled
        {
            get => _isDecimalEnabled;
            private set
            {
                if (_isDecimalEnabled != value)
                {
                    _isDecimalEnabled = value;
                    RaisePropertyChanged(nameof(IsDecimalEnabled));
                }
            }
        }

        public ObservableCollection<EquationViewModel> Equations
        {
            get => _equations;
            private set
            {
                if (_equations != value)
                {
                    _equations = value ?? new ObservableCollection<EquationViewModel>();
                    RaisePropertyChanged(nameof(Equations));
                }
            }
        }

        public ObservableCollection<VariableViewModel> Variables
        {
            get => _variables;
            private set
            {
                if (_variables != value)
                {
                    _variables = value ?? new ObservableCollection<VariableViewModel>();
                    RaisePropertyChanged(nameof(Variables));
                }
            }
        }

        public EquationViewModel SelectedEquation
        {
            get => _selectedEquation;
            private set
            {
                if (_selectedEquation != value)
                {
                    _selectedEquation = value;
                    RaisePropertyChanged(nameof(SelectedEquation));
                }
            }
        }

        private DelegateCommand _buttonPressedCommand;
        public DelegateCommand ButtonPressedCommand => _buttonPressedCommand ?? (_buttonPressedCommand = new DelegateCommand(OnButtonPressed));
        public DelegateCommand ButtonPressed => ButtonPressedCommand;

        public void UpdateVariables(IEnumerable<KeyValuePair<string, GraphControl.Variable>> variables)
        {
            Variables.Clear();
            if (variables == null) return;

            foreach (var kvp in variables)
            {
                var vm = new VariableViewModel(kvp.Key, kvp.Value);
                vm.VariableUpdated += (sender, args) =>
                {
                    VariableUpdated?.Invoke(sender, args);
                };
                Variables.Add(vm);
            }
        }

        public void SetSelectedEquation(EquationViewModel equation)
        {
            SelectedEquation = equation;
        }

        private void OnButtonPressed(object parameter)
        {
            if (parameter is NumbersAndOperatorsEnum numOp)
            {
                // Handle graphing calculator button press
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
