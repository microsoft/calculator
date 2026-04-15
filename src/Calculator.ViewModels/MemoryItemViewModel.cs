// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CommunityToolkit.Mvvm.ComponentModel;

namespace CalculatorApp.ViewModel
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class MemoryItemViewModel : ObservableObject
    {
        private readonly StandardCalculatorViewModel _calcVM;

        [ObservableProperty]
        private int _position;

        [ObservableProperty]
        private string _value;

        public MemoryItemViewModel(StandardCalculatorViewModel calcVM)
        {
            _calcVM = calcVM;
            _position = -1;
            _value = string.Empty;
        }

        public void Clear()
        {
            _calcVM.OnMemoryClear(Position);
        }

        public void MemoryAdd()
        {
            _calcVM.OnMemoryAdd(Position);
        }

        public void MemorySubtract()
        {
            _calcVM.OnMemorySubtract(Position);
        }
    }
}
