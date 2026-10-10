// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux port of MemoryItemViewModel (no WinRT Bindable marker needed).
using CommunityToolkit.Mvvm.ComponentModel;

namespace Calculator.Linux.ViewModels;

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
