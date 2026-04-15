// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using CalculatorApp.ViewModel.Common;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace CalculatorApp.ViewModel
{
    public struct VariableChangedEventArgs
    {
        public string VariableName;
        public double NewValue;
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class GraphingCalculatorViewModel : ObservableObject
    {
        private bool _isDecimalEnabled;
        private ObservableCollection<EquationViewModel> _equations;
        private ObservableCollection<VariableViewModel> _variables;
        private EquationViewModel _selectedEquation;

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
            private set => SetProperty(ref _isDecimalEnabled, value);
        }

        public ObservableCollection<EquationViewModel> Equations
        {
            get => _equations;
            private set => SetProperty(ref _equations, value ?? new ObservableCollection<EquationViewModel>());
        }

        public ObservableCollection<VariableViewModel> Variables
        {
            get => _variables;
            private set => SetProperty(ref _variables, value ?? new ObservableCollection<VariableViewModel>());
        }

        public EquationViewModel SelectedEquation
        {
            get => _selectedEquation;
            private set => SetProperty(ref _selectedEquation, value);
        }

        private RelayCommand<object> _buttonPressedCommand;
        public RelayCommand<object> ButtonPressedCommand => _buttonPressedCommand ?? (_buttonPressedCommand = new RelayCommand<object>(OnButtonPressed));
        public RelayCommand<object> ButtonPressed => ButtonPressedCommand;

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

    }
}
