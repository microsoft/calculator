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

        public event System.EventHandler<VariableChangedEventArgs> VariableUpdated;

        public VariableViewModel(string name, GraphControl.Variable variable)
        {
            _name = name;
            _variable = variable;
            _sliderSettingsVisible = false;
        }

        public string Name
        {
            get => _name;
        }

        public double Min
        {
            get => _variable.Min;
            set
            {
                if (_variable.Min != value)
                {
                    if (value >= _variable.Max)
                    {
                        _variable.Max = value + DefaultMinMaxRange;
                        OnPropertyChanged(nameof(Max));
                    }
                    _variable.Min = value;
                    OnPropertyChanged(nameof(Min));
                }
            }
        }

        public double Step
        {
            get => _variable.Step;
            set
            {
                if (_variable.Step != value)
                {
                    _variable.Step = value;
                    OnPropertyChanged(nameof(Step));
                }
            }
        }

        public double Max
        {
            get => _variable.Max;
            set
            {
                if (_variable.Max != value)
                {
                    if (value <= _variable.Min)
                    {
                        _variable.Min = value - DefaultMinMaxRange;
                        OnPropertyChanged(nameof(Min));
                    }
                    _variable.Max = value;
                    OnPropertyChanged(nameof(Max));
                }
            }
        }

        public double Value
        {
            get => _variable.Value;
            set
            {
                if (value < _variable.Min)
                {
                    _variable.Min = value;
                    OnPropertyChanged(nameof(Min));
                }
                else if (value > _variable.Max)
                {
                    _variable.Max = value;
                    OnPropertyChanged(nameof(Max));
                }

                if (_variable.Value != value)
                {
                    _variable.Value = value;
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
