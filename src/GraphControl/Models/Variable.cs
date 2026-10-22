// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace GraphControl
{
    public sealed class Variable : INotifyPropertyChanged
    {
        private double _value;
        private double _step;
        private double _min;
        private double _max;

        public event PropertyChangedEventHandler PropertyChanged;

        public double Value
        {
            get => _value;
            set
            {
                if (_value != value)
                {
                    _value = value;
                    RaisePropertyChanged();
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
                    RaisePropertyChanged();
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
                    _min = value;
                    RaisePropertyChanged();
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
                    _max = value;
                    RaisePropertyChanged();
                }
            }
        }

        public Variable(double value)
        {
            _value = value;
            _step = 0.1;
            _min = -5.0;
            _max = 5.0;
        }

        private void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
