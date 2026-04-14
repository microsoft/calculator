// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;
using CalculatorApp.ViewModel.Common;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class VariableViewModel : INotifyPropertyChanged
    {
        private const int DefaultMinMaxRange = 10;

        private readonly string _name;
        private readonly GraphControl.Variable _variable;
        private bool _sliderSettingsVisible;

        // Internal state for variable values (used when _variable is an opaque interop object)
        private double _min;
        private double _max;
        private double _step;
        private double _value;

        public event PropertyChangedEventHandler PropertyChanged;
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

        public bool SliderSettingsVisible
        {
            get => _sliderSettingsVisible;
            set
            {
                if (_sliderSettingsVisible != value)
                {
                    _sliderSettingsVisible = value;
                    RaisePropertyChanged(nameof(SliderSettingsVisible));
                }
            }
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
                        RaisePropertyChanged(nameof(Max));
                    }
                    _min = value;
                    RaisePropertyChanged(nameof(Min));
                }
            }
        }

        public double Step
        {
            get => _step;
            set
            {
                if (_step != value)
                {
                    _step = value;
                    RaisePropertyChanged(nameof(Step));
                }
            }
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
                        RaisePropertyChanged(nameof(Min));
                    }
                    _max = value;
                    RaisePropertyChanged(nameof(Max));
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
                    RaisePropertyChanged(nameof(Min));
                }
                else if (value > _max)
                {
                    _max = value;
                    RaisePropertyChanged(nameof(Max));
                }

                if (_value != value)
                {
                    _value = value;
                    VariableUpdated?.Invoke(this, new VariableChangedEventArgs { VariableName = Name, NewValue = value });
                    RaisePropertyChanged(nameof(Value));
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

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
