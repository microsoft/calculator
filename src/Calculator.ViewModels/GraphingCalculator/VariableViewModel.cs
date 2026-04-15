// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;
using CalculatorApp.ViewModel.Common;
using CommunityToolkit.Mvvm.ComponentModel;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class VariableViewModel : ObservableObject
    {
        private const int DefaultMinMaxRange = 10;

        private readonly string _name;
        private readonly GraphControl.Variable _variable;

        [ObservableProperty]
        private bool _sliderSettingsVisible;

        // Internal state for variable values (used when _variable is an opaque interop object)
        private double _min;
        private double _max;
        private double _step;
        private double _value;

        public event System.EventHandler<VariableChangedEventArgs> VariableUpdated;

        public VariableViewModel(string name, GraphControl.Variable variable)
        {
            _name = name;
            _variable = variable;
            _sliderSettingsVisible = false;

            // Initialize defaults from variable if available
            _min = -DefaultMinMaxRange;
            _max = DefaultMinMaxRange;
            _step = 1.0;
            _value = 0.0;
        }

        public string Name
        {
            get => _name;
        }

        public double Min
        {
            get => _min;
            set
            {
                if (_min != value)
                {
                    if (value >= _max)
                    {
                        _max = value + DefaultMinMaxRange;
                        OnPropertyChanged(nameof(Max));
                    }
                    _min = value;
                    OnPropertyChanged(nameof(Min));
                }
            }
        }

        public double Step
        {
            get => _step;
            set => SetProperty(ref _step, value);
        }

        public double Max
        {
            get => _max;
            set
            {
                if (_max != value)
                {
                    if (value <= _min)
                    {
                        _min = value - DefaultMinMaxRange;
                        OnPropertyChanged(nameof(Min));
                    }
                    _max = value;
                    OnPropertyChanged(nameof(Max));
                }
            }
        }

        public double Value
        {
            get => _value;
            set
            {
                if (value < _min)
                {
                    _min = value;
                    OnPropertyChanged(nameof(Min));
                }
                else if (value > _max)
                {
                    _max = value;
                    OnPropertyChanged(nameof(Max));
                }

                if (_value != value)
                {
                    _value = value;
                    VariableUpdated?.Invoke(this, new VariableChangedEventArgs { VariableName = Name, NewValue = value });
                    OnPropertyChanged(nameof(Value));
                }
            }
        }

        public string VariableAutomationName
        {
            get
            {
                return LocalizationStringUtil.GetLocalizedString(
                    AppResourceProvider.GetInstance().GetResourceString("VariableListViewItem"), Name);
            }
        }

    }
}
