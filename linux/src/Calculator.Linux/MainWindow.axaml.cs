// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.LogicalTree;
using Avalonia.Markup.Xaml;
using Calculator.Linux.Common;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;

namespace Calculator.Linux;

public sealed partial class MainWindow : Window
{
    private readonly StandardCalculatorViewModel _vm;

    public MainWindow()
    {
        InitializeComponent();
        LinuxLocalization.Current = new LinuxLocalization(System.Globalization.CultureInfo.CurrentCulture);
        _vm = new StandardCalculatorViewModel();
        _vm.SetCalculatorType(ViewMode.Standard);
        _vm.HistoryVM.HistoryItemClicked += item => _vm.SelectHistoryItem(item);
        DataContext = _vm;
        Opened += OnOpened;
        SyncAngle();
        SyncRadix();
        SyncBits();
    }

    private void InitializeComponent() => AvaloniaXamlLoader.Load(this);

    private void OnOpened(object? sender, EventArgs e)
    {
        // Install the platform clipboard for copy/paste (Task 3 default is in-memory).
        var topLevel = TopLevel.GetTopLevel(this);
        if (topLevel?.Clipboard != null)
        {
            CopyPasteManager.Clipboard = new AvaloniaClipboardService(topLevel.Clipboard);
        }
    }

    // --- Mode navigation -------------------------------------------------------

    private void ModeList_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (sender is not ListBox list || list.SelectedItem is not ListBoxItem selected)
        {
            return;
        }
        switch (selected.Tag as string)
        {
            case "Scientific":
                _vm.SetCalculatorType(ViewMode.Scientific);
                break;
            case "Programmer":
                _vm.SetCalculatorType(ViewMode.Programmer);
                SyncRadix();
                SyncBits();
                break;
            default:
                _vm.SetCalculatorType(ViewMode.Standard);
                break;
        }
    }

    // --- Memory row --------------------------------------------------------------

    private void MemoryRow_Store(object? sender, RoutedEventArgs e) => _vm.OnMemoryButtonPressed();

    private void MemoryRow_Recall(object? sender, RoutedEventArgs e)
    {
        if (Find<TabControl>("SidePanel") is { } panel)
        {
            panel.SelectedIndex = 1;
        }
    }

    private void MemoryRow_Add(object? sender, RoutedEventArgs e)
    {
        if (_vm.MemorizedNumbers.Count == 0)
        {
            _vm.OnMemoryButtonPressed();
        }
        else
        {
            _vm.MemoryAddCommand.Execute(0);
        }
    }

    private void MemoryRow_Subtract(object? sender, RoutedEventArgs e)
    {
        if (_vm.MemorizedNumbers.Count == 0)
        {
            _vm.OnMemoryButtonPressed();
        }
        else
        {
            _vm.MemorySubtractCommand.Execute(0);
        }
    }

    // --- Memory list items ---------------------------------------------------------

    private static MemoryItemViewModel? ItemOf(object? sender) =>
        (sender as Control)?.DataContext as MemoryItemViewModel;

    private void MemoryItem_Clear(object? sender, RoutedEventArgs e) => ItemOf(sender)?.Clear();
    private void MemoryItem_Add(object? sender, RoutedEventArgs e) => ItemOf(sender)?.MemoryAdd();
    private void MemoryItem_Subtract(object? sender, RoutedEventArgs e) => ItemOf(sender)?.MemorySubtract();

    private void MemoryItem_Recall(object? sender, RoutedEventArgs e)
    {
        var item = ItemOf(sender);
        if (item != null)
        {
            _vm.OnMemoryItemPressed(item.Position);
        }
    }

    // --- History ---------------------------------------------------------------------

    private void HistoryList_DoubleTapped(object? sender, TappedEventArgs e)
    {
        if (sender is ListBox list && list.SelectedItem is HistoryItemViewModel item)
        {
            _vm.SelectHistoryItem(item);
        }
    }

    // --- Scientific angle ---------------------------------------------------------------

    private void Angle_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not RadioButton radio)
        {
            return;
        }
        _vm.SwitchAngleType(radio.Name switch
        {
            "AngleRad" => NumbersAndOperatorsEnum.Radians,
            "AngleGrad" => NumbersAndOperatorsEnum.Grads,
            _ => NumbersAndOperatorsEnum.Degree,
        });
        SyncAngle();
    }

    private void SyncAngle()
    {
        var angle = _vm.GetCurrentAngleType();
        SetChecked("AngleDeg", angle == NumbersAndOperatorsEnum.Degree);
        SetChecked("AngleRad", angle == NumbersAndOperatorsEnum.Radians);
        SetChecked("AngleGrad", angle == NumbersAndOperatorsEnum.Grads);
    }

    // --- Programmer radix / bit length -------------------------------------------------------

    private void Radix_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not RadioButton radio)
        {
            return;
        }
        _vm.SwitchProgrammerModeBase(radio.Name switch
        {
            "RadixHex" => NumberBase.HexBase,
            "RadixOct" => NumberBase.OctBase,
            "RadixBin" => NumberBase.BinBase,
            _ => NumberBase.DecBase,
        });
        SyncRadix();
    }

    private void SyncRadix()
    {
        SetChecked("RadixHex", _vm.CurrentRadixType == NumberBase.HexBase);
        SetChecked("RadixDec", _vm.CurrentRadixType == NumberBase.DecBase);
        SetChecked("RadixOct", _vm.CurrentRadixType == NumberBase.OctBase);
        SetChecked("RadixBin", _vm.CurrentRadixType == NumberBase.BinBase);
    }

    private void Bits_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not RadioButton radio)
        {
            return;
        }
        _vm.ValueBitLength = radio.Name switch
        {
            "BitsDword" => BitLength.BitLengthDWord,
            "BitsWord" => BitLength.BitLengthWord,
            "BitsByte" => BitLength.BitLengthByte,
            _ => BitLength.BitLengthQWord,
        };
        SyncBits();
    }

    private void SyncBits()
    {
        SetChecked("BitsQword", _vm.ValueBitLength == BitLength.BitLengthQWord);
        SetChecked("BitsDword", _vm.ValueBitLength == BitLength.BitLengthDWord);
        SetChecked("BitsWord", _vm.ValueBitLength == BitLength.BitLengthWord);
        SetChecked("BitsByte", _vm.ValueBitLength == BitLength.BitLengthByte);
    }

    private T? Find<T>(string name) where T : Control =>
        this.GetLogicalDescendants().OfType<T>().FirstOrDefault(c => c.Name == name);

    private void SetChecked(string name, bool value)
    {
        if (Find<ToggleButton>(name) is { } toggle)
        {
            toggle.IsChecked = value;
        }
    }

    // --- Bit flip -------------------------------------------------------------------------------

    private void Bit_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not ToggleButton button)
        {
            return;
        }
        int index = IndexOfBit(button);
        if (index >= 0)
        {
            // BinaryDigits is LSB-first; button index == bit position.
            _vm.SendCommandToCalcManager((int)CalcCommand.CommandBINEDITSTART + index);
        }
    }

    private int IndexOfBit(ToggleButton button)
    {
        if (Find<ItemsControl>("BitFlipPanel") is { } panel)
        {
            int index = panel.ItemContainerGenerator?.IndexFromContainer(button) ?? -1;
            if (index >= 0)
            {
                return index;
            }
            // Fallback: ToggleButtons inside a DataTemplate may report via ItemsSource order.
            var items = panel.ItemsSource as System.Collections.IList;
            if (items != null)
            {
                return items.IndexOf(button.DataContext);
            }
        }
        return -1;
    }

    // --- Keyboard: hex digits (mode-gated), copy/paste -----------------------------------------------

    protected override void OnKeyDown(KeyEventArgs e)
    {
        if (e.KeyModifiers.HasFlag(KeyModifiers.Control) && e.Key == Key.C)
        {
            _vm.CopyCommand.Execute(null);
            e.Handled = true;
            return;
        }

        if (e.KeyModifiers.HasFlag(KeyModifiers.Control) && e.Key == Key.V)
        {
            _vm.PasteCommand.Execute(null);
            e.Handled = true;
            return;
        }

        if (_vm.IsProgrammer && e.KeyModifiers == KeyModifiers.None)
        {
            var op = e.Key switch
            {
                Key.A => NumbersAndOperatorsEnum.A,
                Key.B => NumbersAndOperatorsEnum.B,
                Key.C => NumbersAndOperatorsEnum.C,
                Key.D => NumbersAndOperatorsEnum.D,
                Key.E => NumbersAndOperatorsEnum.E,
                Key.F => NumbersAndOperatorsEnum.F,
                _ => (NumbersAndOperatorsEnum?)null,
            };
            if (op != null)
            {
                _vm.ButtonPressedCommand.Execute(op);
                e.Handled = true;
                return;
            }
        }

        base.OnKeyDown(e);
    }
}
