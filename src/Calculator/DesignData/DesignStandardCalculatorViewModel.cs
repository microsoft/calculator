// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Input;
using CalculatorApp.ViewModel.Common;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace CalculatorApp.DesignData
{
    public sealed class MemorySlot : ObservableObject
    {
        private int _slotPosition;
        private string _slotValue;

        public MemorySlot(int slotPosition, string value)
        {
            _slotPosition = slotPosition;
            _slotValue = value;
        }

        public int SlotPosition
        {
            get => _slotPosition;
            set => SetProperty(ref _slotPosition, value);
        }

        public string SlotValue
        {
            get => _slotValue;
            set => SetProperty(ref _slotValue, value);
        }
    }

    public sealed class StandardCalculatorViewModel : ObservableObject
    {
        private string _displayValue;
        private string _displayStringExpression;
        private string _degreeButtonContent;
        private ObservableCollection<MemorySlot> _memorizedNumbers;
        private bool _isMemoryEmpty;
        private IDictionary<NumbersAndOperatorsEnum, bool> _pressedButtons;

        public StandardCalculatorViewModel()
        {
            _displayValue = "1234569";
            _displayStringExpression = "14560 x 1890";
            _degreeButtonContent = "Deg";
            _isMemoryEmpty = false;

            _memorizedNumbers = new ObservableCollection<MemorySlot>();
            for (int i = 1000; i < 1100; i++)
            {
                _memorizedNumbers.Add(new MemorySlot(i, i.ToString()));
            }

            _pressedButtons = new Dictionary<NumbersAndOperatorsEnum, bool>();
            ButtonPressed = new RelayCommand<object>(OnButtonPressed);
        }

        public string DisplayValue
        {
            get => _displayValue;
            set => SetProperty(ref _displayValue, value);
        }

        public string DisplayStringExpression
        {
            get => _displayStringExpression;
            set => SetProperty(ref _displayStringExpression, value);
        }

        public string DegreeButtonContent
        {
            get => _degreeButtonContent;
            set => SetProperty(ref _degreeButtonContent, value);
        }

        public ObservableCollection<MemorySlot> MemorizedNumbers
        {
            get => _memorizedNumbers;
            set => SetProperty(ref _memorizedNumbers, value);
        }

        public bool IsMemoryEmpty
        {
            get => _isMemoryEmpty;
            set => SetProperty(ref _isMemoryEmpty, value);
        }

        public IDictionary<NumbersAndOperatorsEnum, bool> PressedButtons
        {
            get => _pressedButtons;
            set => SetProperty(ref _pressedButtons, value);
        }

        public ICommand ButtonPressed { get; }

        private void OnButtonPressed(object parameter)
        {
        }
    }
}

namespace Numbers.DesignData
{
    public sealed class MemorySlot : ObservableObject
    {
        private int _slotPosition;
        private string _slotValue;

        public MemorySlot(int slotPosition, string value)
        {
            _slotPosition = slotPosition;
            _slotValue = value;
        }

        public int SlotPosition
        {
            get => _slotPosition;
            set => SetProperty(ref _slotPosition, value);
        }

        public string SlotValue
        {
            get => _slotValue;
            set => SetProperty(ref _slotValue, value);
        }
    }

    public sealed class StandardCalculatorViewModel : ObservableObject
    {
        private string _displayValue;
        private string _displayStringExpression;
        private string _degreeButtonContent;
        private ObservableCollection<MemorySlot> _memorizedNumbers;
        private bool _isMemoryEmpty;
        private IDictionary<NumbersAndOperatorsEnum, bool> _pressedButtons;

        public StandardCalculatorViewModel()
        {
            _displayValue = "1234569";
            _displayStringExpression = "14560 x 1890";
            _degreeButtonContent = "Deg";
            _isMemoryEmpty = false;

            _memorizedNumbers = new ObservableCollection<MemorySlot>();
            for (int i = 1000; i < 1100; i++)
            {
                _memorizedNumbers.Add(new MemorySlot(i, i.ToString()));
            }

            _pressedButtons = new Dictionary<NumbersAndOperatorsEnum, bool>();
            ButtonPressed = new RelayCommand<object>(OnButtonPressed);
        }

        public string DisplayValue
        {
            get => _displayValue;
            set => SetProperty(ref _displayValue, value);
        }

        public string DisplayStringExpression
        {
            get => _displayStringExpression;
            set => SetProperty(ref _displayStringExpression, value);
        }

        public string DegreeButtonContent
        {
            get => _degreeButtonContent;
            set => SetProperty(ref _degreeButtonContent, value);
        }

        public ObservableCollection<MemorySlot> MemorizedNumbers
        {
            get => _memorizedNumbers;
            set => SetProperty(ref _memorizedNumbers, value);
        }

        public bool IsMemoryEmpty
        {
            get => _isMemoryEmpty;
            set => SetProperty(ref _isMemoryEmpty, value);
        }

        public IDictionary<NumbersAndOperatorsEnum, bool> PressedButtons
        {
            get => _pressedButtons;
            set => SetProperty(ref _pressedButtons, value);
        }

        public ICommand ButtonPressed { get; }

        private void OnButtonPressed(object parameter)
        {
        }
    }
}
