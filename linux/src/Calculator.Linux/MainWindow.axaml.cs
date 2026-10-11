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
using Calculator.Linux.Services.Graphing;
using Calculator.Linux.ViewModels;

namespace Calculator.Linux;

public sealed partial class MainWindow : Window
{
    private readonly StandardCalculatorViewModel _vm;
    private readonly DateCalculatorViewModel _dateVM;
    private UnitConverterViewModel? _converterVM;
    private GraphingViewModel? _graphVM;

    public MainWindow()
    {
        InitializeComponent();
        LinuxLocalization.Current = new LinuxLocalization(System.Globalization.CultureInfo.CurrentCulture);
        _vm = new StandardCalculatorViewModel();
        _vm.SetCalculatorType(ViewMode.Standard);
        _vm.HistoryVM.HistoryItemClicked += item => _vm.SelectHistoryItem(item);
        DataContext = _vm;
        _dateVM = new DateCalculatorViewModel();
        if (Find<StackPanel>("DatePanel") is { } datePanel)
        {
            datePanel.DataContext = _dateVM;
        }
        Opened += OnOpened;
        SyncAngle();
        SyncRadix();
        SyncBits();
        SyncDateMode();
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

    private void About_Click(object? sender, RoutedEventArgs e)
    {
        new AboutWindow().ShowDialog(this);
    }

    private void HistoryToggle_Click(object? sender, RoutedEventArgs e)
    {
        if (Find<TabControl>("SidePanel") is { } panel
            && Find<ToggleButton>("HistoryToggle") is { } toggle)
        {
            panel.IsVisible = toggle.IsChecked == true;
        }
    }

    protected override void OnPropertyChanged(AvaloniaPropertyChangedEventArgs change)
    {
        base.OnPropertyChanged(change);
        // Collapse the docked History/Memory panel on narrow windows so the
        // keypad never clips (upstream uses flyouts at small sizes; delta).
        if (change.Property == BoundsProperty && Bounds.Width < 720)
        {
            if (Find<TabControl>("SidePanel") is { } panel && panel.IsVisible)
            {
                panel.IsVisible = false;
                if (Find<ToggleButton>("HistoryToggle") is { } toggle)
                {
                    toggle.IsChecked = false;
                }
            }
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
                ShowCalculator(ViewMode.Scientific);
                break;
            case "Programmer":
                ShowCalculator(ViewMode.Programmer);
                SyncRadix();
                SyncBits();
                break;
            case "Date":
                ShowDate();
                break;
            case "Graphing":
                ShowGraphing();
                break;
            case string tag when Enum.TryParse<ViewMode>(tag, out var converterMode)
                && converterMode != ViewMode.Currency
                && converterMode != ViewMode.Graphing:
                ShowConverter(converterMode);
                break;
            default:
                ShowCalculator(ViewMode.Standard);
                break;
        }
    }

    private void ShowGraphing()
    {
        HideConverter();
        Title = "Calculator – Graphing";
        _graphVM ??= new GraphingViewModel();
        if (Find<DockPanel>("GraphingPanel") is { } panel)
        {
            panel.DataContext = _graphVM;
            panel.IsVisible = true;
        }
        if (Find<DockPanel>("CalculatorPanel") is { } calc)
        {
            calc.IsVisible = false;
        }
        if (Find<StackPanel>("DatePanel") is { } date)
        {
            date.IsVisible = false;
        }
        if (Find<DockPanel>("ConverterPanel") is { } converter)
        {
            converter.IsVisible = false;
        }
        SyncTrig();
    }

    private void Equation_Delete(object? sender, RoutedEventArgs e)
    {
        if (_graphVM != null && (sender as Control)?.DataContext is GraphEquation equation)
        {
            _graphVM.RemoveEquation(equation);
        }
    }

    private void GraphPad_Insert(object? sender, RoutedEventArgs e)
    {
        if (_graphVM == null || sender is not Button button || button.Tag is not string text)
        {
            return;
        }
        GraphEquation equation = _graphVM.SelectedEquation ?? _graphVM.AddEquation(string.Empty);
        _graphVM.SelectedEquation = equation;
        equation.Expression += text;
    }

    private void Trig_Click(object? sender, RoutedEventArgs e)
    {
        if (_graphVM == null || sender is not RadioButton radio)
        {
            return;
        }
        _graphVM.TrigMode = radio.Name == "TrigDeg" ? AngleMode.Degrees : AngleMode.Radians;
        SyncTrig();
    }

    private void SyncTrig()
    {
        if (_graphVM == null)
        {
            return;
        }
        SetChecked("TrigRad", _graphVM.TrigMode == AngleMode.Radians);
        SetChecked("TrigDeg", _graphVM.TrigMode == AngleMode.Degrees);
    }

    private void Graph_Reset(object? sender, RoutedEventArgs e)
    {
        if (_graphVM == null)
        {
            return;
        }
        _graphVM.XMin = -10;
        _graphVM.XMax = 10;
        _graphVM.YMin = -10;
        _graphVM.YMax = 10;
    }

    private void HideConverter()
    {
        if (Find<DockPanel>("ConverterPanel") is { } converter && converter.IsVisible)
        {
            _converterVM?.SaveUserPreferences();
        }
    }

    private void ShowConverter(ViewMode mode)
    {
        Title = "Calculator – " + mode;
        _converterVM ??= new UnitConverterViewModel();
        _converterVM.Dispatcher = AvaloniaDispatcher.Instance;
        _converterVM.Mode = mode;
        int categoryId = NavCategoryStates.Serialize(mode);
        var category = _converterVM.Categories.FirstOrDefault(c => c.GetModelCategoryId() == categoryId);
        if (category != null)
        {
            _converterVM.CurrentCategory = category;
        }
        if (Find<DockPanel>("ConverterPanel") is { } panel)
        {
            panel.DataContext = _converterVM;
            panel.IsVisible = true;
        }
        if (Find<DockPanel>("CalculatorPanel") is { } calc)
        {
            calc.IsVisible = false;
        }
        if (Find<StackPanel>("DatePanel") is { } date)
        {
            date.IsVisible = false;
        }
    }

    private void ConverterValue_GotFocus(object? sender, GotFocusEventArgs e)
    {
        if (_converterVM == null || sender is not TextBox box)
        {
            return;
        }
        if (box.Name == "ConverterValue1" && !_converterVM.Value1Active
            || box.Name == "ConverterValue2" && !_converterVM.Value2Active)
        {
            _converterVM.SwitchActiveCommand.Execute(null);
        }
    }

    private void ShowCalculator(ViewMode mode)
    {
        HideConverter();
        _vm.SetCalculatorType(mode);
        Title = "Calculator – " + mode;
        if (Find<DockPanel>("CalculatorPanel") is { } calc)
        {
            calc.IsVisible = true;
        }
        if (Find<StackPanel>("DatePanel") is { } date)
        {
            date.IsVisible = false;
        }
        if (Find<DockPanel>("ConverterPanel") is { } converter)
        {
            converter.IsVisible = false;
        }
        if (Find<DockPanel>("GraphingPanel") is { } graphing)
        {
            graphing.IsVisible = false;
        }
    }

    private void ShowDate()
    {
        HideConverter();
        Title = "Calculator – Date calculation";
        if (Find<DockPanel>("CalculatorPanel") is { } calc)
        {
            calc.IsVisible = false;
        }
        if (Find<StackPanel>("DatePanel") is { } date)
        {
            date.IsVisible = true;
        }
        if (Find<DockPanel>("ConverterPanel") is { } converter)
        {
            converter.IsVisible = false;
        }
        if (Find<DockPanel>("GraphingPanel") is { } graphing)
        {
            graphing.IsVisible = false;
        }
    }

    // --- Date calculator ---------------------------------------------------------------

    private void DateMode_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not RadioButton radio)
        {
            return;
        }
        _dateVM.IsDateDiffMode = radio.Name != "DateModeAddSub";
        SyncDateMode();
    }

    private void DateAddSub_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not RadioButton radio)
        {
            return;
        }
        _dateVM.IsAddMode = radio.Name != "DateSub";
        SyncDateMode();
    }

    private void SyncDateMode()
    {
        SetChecked("DateModeDiff", _dateVM.IsDateDiffMode);
        SetChecked("DateModeAddSub", !_dateVM.IsDateDiffMode);
        SetChecked("DateAdd", _dateVM.IsAddMode);
        SetChecked("DateSub", !_dateVM.IsAddMode);
        if (Find<StackPanel>("DateDiffStack") is { } diff)
        {
            diff.IsVisible = _dateVM.IsDateDiffMode;
        }
        if (Find<StackPanel>("DateAddSubStack") is { } addSub)
        {
            addSub.IsVisible = !_dateVM.IsDateDiffMode;
        }
    }

    private void OffsetBox_Changed(object? sender, TextChangedEventArgs e)
    {
        if (sender is not TextBox box)
        {
            return;
        }
        if (!int.TryParse(box.Text, out int value))
        {
            return;
        }
        value = Math.Clamp(value, 0, 999);
        switch (box.Name)
        {
            case "YearsBox":
                _dateVM.YearsOffset = value;
                break;
            case "MonthsBox":
                _dateVM.MonthsOffset = value;
                break;
            case "DaysBox":
                _dateVM.DaysOffset = value;
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
            int index = panel.IndexFromContainer(button);
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

    // --- Keyboard: panel-aware routing ------------------------------------------------------
    // Window-level routing (replaces per-button shortcuts): text-editing controls
    // keep native behavior; otherwise keys drive the visible panel. Matches the
    // upstream map (digits, +-*/%^(), Enter, Esc, Backspace, Delete, F9, E, A-F).

    private bool IsCalculatorActive() =>
        Find<DockPanel>("CalculatorPanel") is { } panel && panel.IsVisible;

    private bool IsConverterActive() =>
        Find<DockPanel>("ConverterPanel") is { } panel && panel.IsVisible;

    protected override void OnKeyDown(KeyEventArgs e)
    {
        bool ctrl = e.KeyModifiers.HasFlag(KeyModifiers.Control);
        if (ctrl && e.Key == Key.C)
        {
            if (IsCalculatorActive())
            {
                _vm.CopyCommand.Execute(null);
            }
            else if (IsConverterActive() && _converterVM != null)
            {
                _converterVM.CopyCommand.Execute(null);
            }
            else
            {
                base.OnKeyDown(e);
                return;
            }
            e.Handled = true;
            return;
        }

        if (ctrl && e.Key == Key.V)
        {
            if (IsCalculatorActive())
            {
                _vm.PasteCommand.Execute(null);
            }
            else if (IsConverterActive() && _converterVM != null)
            {
                _converterVM.PasteCommand.Execute(null);
            }
            else
            {
                base.OnKeyDown(e);
                return;
            }
            e.Handled = true;
            return;
        }

        if (ctrl)
        {
            base.OnKeyDown(e);
            return;
        }

        // Text-editing controls keep native behavior (no calculator side effects).
        var focused = TopLevel.GetTopLevel(this)?.FocusManager?.GetFocusedElement();
        if (focused is TextBox or DatePicker or ComboBox or NumericUpDown)
        {
            base.OnKeyDown(e);
            return;
        }

        if (IsConverterActive() && _converterVM != null)
        {
            if (TryConverterKey(e.Key, e.KeyModifiers, out NumbersAndOperatorsEnum op))
            {
                _converterVM.ButtonPressedCommand.Execute(op);
                e.Handled = true;
                return;
            }
            base.OnKeyDown(e);
            return;
        }

        if (!IsCalculatorActive())
        {
            base.OnKeyDown(e);
            return;
        }

        bool shift = e.KeyModifiers.HasFlag(KeyModifiers.Shift);
        NumbersAndOperatorsEnum? calcOp = e.Key switch
        {
            Key.D0 or Key.NumPad0 => NumbersAndOperatorsEnum.Zero,
            Key.D1 or Key.NumPad1 => NumbersAndOperatorsEnum.One,
            Key.D2 or Key.NumPad2 => NumbersAndOperatorsEnum.Two,
            Key.D3 or Key.NumPad3 => NumbersAndOperatorsEnum.Three,
            Key.D4 or Key.NumPad4 => NumbersAndOperatorsEnum.Four,
            Key.D5 or Key.NumPad5 => shift ? NumbersAndOperatorsEnum.Percent : NumbersAndOperatorsEnum.Five,
            Key.D6 or Key.NumPad6 => shift ? NumbersAndOperatorsEnum.XPowerY : NumbersAndOperatorsEnum.Six,
            Key.D7 or Key.NumPad7 => NumbersAndOperatorsEnum.Seven,
            Key.D8 or Key.NumPad8 => NumbersAndOperatorsEnum.Eight,
            Key.D9 or Key.NumPad9 => NumbersAndOperatorsEnum.Nine,
            Key.Add or Key.OemPlus => NumbersAndOperatorsEnum.Add,
            Key.Subtract or Key.OemMinus => NumbersAndOperatorsEnum.Subtract,
            Key.Multiply => NumbersAndOperatorsEnum.Multiply,
            Key.Divide => NumbersAndOperatorsEnum.Divide,
            Key.OemComma or Key.OemPeriod or Key.Decimal or Key.AbntC1 => NumbersAndOperatorsEnum.Decimal,
            Key.OemOpenBrackets => NumbersAndOperatorsEnum.OpenParenthesis,
            Key.OemCloseBrackets => NumbersAndOperatorsEnum.CloseParenthesis,
            Key.Enter => NumbersAndOperatorsEnum.Equals,
            Key.Escape => NumbersAndOperatorsEnum.Clear,
            Key.Back => NumbersAndOperatorsEnum.Backspace,
            Key.Delete => NumbersAndOperatorsEnum.ClearEntry,
            Key.F9 => NumbersAndOperatorsEnum.Negate,
            _ => null,
        };

        if (calcOp == null && e.KeyModifiers == KeyModifiers.None)
        {
            calcOp = e.Key switch
            {
                Key.E when _vm.IsScientific => NumbersAndOperatorsEnum.Exp,
                Key.E when _vm.IsProgrammer => NumbersAndOperatorsEnum.E,
                Key.A when _vm.IsProgrammer => NumbersAndOperatorsEnum.A,
                Key.B when _vm.IsProgrammer => NumbersAndOperatorsEnum.B,
                Key.C when _vm.IsProgrammer => NumbersAndOperatorsEnum.C,
                Key.D when _vm.IsProgrammer => NumbersAndOperatorsEnum.D,
                Key.F when _vm.IsProgrammer => NumbersAndOperatorsEnum.F,
                _ => null,
            };
        }

        if (calcOp != null)
        {
            _vm.ButtonPressedCommand.Execute(calcOp);
            e.Handled = true;
            return;
        }

        base.OnKeyDown(e);
    }

    private static bool TryConverterKey(Key key, KeyModifiers modifiers, out NumbersAndOperatorsEnum op)
    {
        bool shift = modifiers.HasFlag(KeyModifiers.Shift);
        NumbersAndOperatorsEnum? mapped = key switch
        {
            Key.D0 or Key.NumPad0 => NumbersAndOperatorsEnum.Zero,
            Key.D1 or Key.NumPad1 => NumbersAndOperatorsEnum.One,
            Key.D2 or Key.NumPad2 => NumbersAndOperatorsEnum.Two,
            Key.D3 or Key.NumPad3 => NumbersAndOperatorsEnum.Three,
            Key.D4 or Key.NumPad4 => NumbersAndOperatorsEnum.Four,
            Key.D5 or Key.NumPad5 => NumbersAndOperatorsEnum.Five,
            Key.D6 or Key.NumPad6 => NumbersAndOperatorsEnum.Six,
            Key.D7 or Key.NumPad7 => NumbersAndOperatorsEnum.Seven,
            Key.D8 or Key.NumPad8 => NumbersAndOperatorsEnum.Eight,
            Key.D9 or Key.NumPad9 => NumbersAndOperatorsEnum.Nine,
            Key.OemComma or Key.OemPeriod or Key.Decimal => NumbersAndOperatorsEnum.Decimal,
            Key.Back => NumbersAndOperatorsEnum.Backspace,
            Key.Escape => NumbersAndOperatorsEnum.Clear,
            _ => null,
        };
        op = mapped ?? NumbersAndOperatorsEnum.None;
        return mapped != null && !shift;
    }
}
