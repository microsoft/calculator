// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux port of HistoryViewModel. History content is pulled from the engine
// through the C ABI (per-mode lists, newest-first UI order); item removal
// targets the current-mode history exactly like upstream.
using Calculator.Linux.Common;
using Calculator.Linux.Interop;
using Calculator.Linux.Services;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using System.Collections.ObjectModel;

namespace Calculator.Linux.ViewModels;

public delegate void HideHistoryClickedHandler();
public delegate void HistoryItemClickedHandler(HistoryItemViewModel e);

public sealed partial class HistoryViewModel : ObservableObject, IHistoryDisplayTarget
{
    private readonly CalcEngine? _engine;
    private int _currentMode; // 0 = standard, 1 = scientific
    private string? _localizedHistoryCleared;
    private string? _localizedHistorySlotCleared;

    private ObservableCollection<HistoryItemViewModel> _items = new();

    [ObservableProperty]
    private bool _areHistoryShortcutsEnabled;

    private NarratorAnnouncement? _historyAnnouncement;

    public event HideHistoryClickedHandler? HideHistoryClicked;
    public event HistoryItemClickedHandler? HistoryItemClicked;

    internal HistoryViewModel(CalcEngine? engine)
    {
        _engine = engine;
        AreHistoryShortcutsEnabled = true;
        Items = new ObservableCollection<HistoryItemViewModel>();
    }

    public ObservableCollection<HistoryItemViewModel> Items
    {
        get => _items;
        private set => SetProperty(ref _items, value);
    }

    public NarratorAnnouncement? HistoryAnnouncement
    {
        get => _historyAnnouncement;
        private set => SetProperty(ref _historyAnnouncement, value);
    }

    public int ItemsCount => Items?.Count ?? 0;

    public void OnHistoryItemAdded(uint addedItemIndex)
    {
        if (_engine == null)
        {
            return;
        }

        var item = _engine.HistoryItem(_currentMode, addedItemIndex);
        if (item == null)
        {
            return;
        }

        var localizer = LinuxLocalization.Current;
        string expression = item.Value.Expression;
        string result = item.Value.Result;
        localizer.LocalizeDisplayValue(ref expression);
        localizer.LocalizeDisplayValue(ref result);

        var historyItem = new HistoryItemViewModel(
            expression,
            result,
            _engine.HistoryItemTokens(_currentMode, addedItemIndex),
            _engine.HistoryItemCommands(_currentMode, addedItemIndex));

        if (Items.Count >= (int)(_engine?.MaxHistorySize ?? 0))
        {
            Items.RemoveAt(Items.Count - 1);
        }

        Items.Insert(0, historyItem);
        OnPropertyChanged(nameof(ItemsCount));
    }

    [RelayCommand]
    private void OnHide()
    {
        HideHistoryClicked?.Invoke();
    }

    [RelayCommand]
    private void OnClear()
    {
        if (AreHistoryShortcutsEnabled)
        {
            _engine?.ClearHistory();
            ClearItems();

            _localizedHistoryCleared ??= AppResources.Instance.GetResourceString("HistoryList_Cleared");
            HistoryAnnouncement = CalculatorAnnouncement.GetHistoryClearedAnnouncement(_localizedHistoryCleared);
        }
    }

    internal void ClearItems()
    {
        if (Items.Count > 0)
        {
            Items.Clear();
            OnPropertyChanged(nameof(ItemsCount));
        }
    }

    public void ShowItem(HistoryItemViewModel e)
    {
        int index = Items.IndexOf(e);
        if (index >= 0)
        {
            LinuxTraceLogger.Instance.LogHistoryItemLoad(ViewModeForTelemetry(), Items.Count, index);
            HistoryItemClicked?.Invoke(e);
        }
    }

    public void DeleteItem(HistoryItemViewModel e)
    {
        int itemIndex = Items.IndexOf(e);
        if (itemIndex >= 0)
        {
            if ((_engine?.RemoveHistoryItem((uint)itemIndex) ?? false))
            {
                Items.RemoveAt(itemIndex);
                OnPropertyChanged(nameof(ItemsCount));
            }
        }

        string localizedIndex = (itemIndex + 1).ToString();
        LinuxLocalization.Current.LocalizeDisplayValue(ref localizedIndex);

        _localizedHistorySlotCleared = AppResources.Instance.GetResourceString("Format_HistorySlotCleared");
        string announcement = LocalizationStringUtil.GetLocalizedString(_localizedHistorySlotCleared, localizedIndex);
        HistoryAnnouncement = CalculatorAnnouncement.GetHistorySlotClearedAnnouncement(announcement);
    }

    public void ReloadHistory(ViewMode currentMode)
    {
        if (currentMode == ViewMode.Standard)
        {
            _currentMode = 0;
        }
        else if (currentMode == ViewMode.Scientific)
        {
            _currentMode = 1;
        }
        else
        {
            return;
        }

        var historyListVM = new ObservableCollection<HistoryItemViewModel>();
        var localizer = LinuxLocalization.Current;

        uint count = _engine?.HistoryCountForMode(_currentMode) ?? 0;
        for (int i = (int)count - 1; i >= 0; i--)
        {
            var histItem = _engine!.HistoryItem(_currentMode, (uint)i);
            if (histItem == null)
            {
                continue;
            }
            string expression = histItem.Value.Expression;
            string result = histItem.Value.Result;
            localizer.LocalizeDisplayValue(ref expression);
            localizer.LocalizeDisplayValue(ref result);

            historyListVM.Add(new HistoryItemViewModel(
                expression,
                result,
                _engine.HistoryItemTokens(_currentMode, (uint)i),
                _engine.HistoryItemCommands(_currentMode, (uint)i)));
        }

        Items = historyListVM;
        OnPropertyChanged(nameof(ItemsCount));
    }

    internal ulong GetMaxItemSize() => (ulong)(_engine?.MaxHistorySize ?? 0);

    private ViewMode ViewModeForTelemetry() => _currentMode == 1 ? ViewMode.Scientific : ViewMode.Standard;
}
