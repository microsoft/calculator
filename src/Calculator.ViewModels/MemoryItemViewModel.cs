// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class MemoryItemViewModel : INotifyPropertyChanged
    {
        private readonly StandardCalculatorViewModel _calcVM;
        private int _position;
        private string _value;

        public event PropertyChangedEventHandler PropertyChanged;

        public MemoryItemViewModel(StandardCalculatorViewModel calcVM)
        {
            _calcVM = calcVM;
            _position = -1;
            _value = string.Empty;
        }

        public int Position
        {
            get => _position;
            set
            {
                if (_position != value)
                {
                    _position = value;
                    RaisePropertyChanged(nameof(Position));
                }
            }
        }

        public string Value
        {
            get => _value;
            set
            {
                if (_value != value)
                {
                    _value = value;
                    RaisePropertyChanged(nameof(Value));
                }
            }
        }

        public void Clear()
        {
            _calcVM.OnMemoryClear(_position);
        }

        public void MemoryAdd()
        {
            _calcVM.OnMemoryAdd(_position);
        }

        public void MemorySubtract()
        {
            _calcVM.OnMemorySubtract(_position);
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
