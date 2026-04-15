// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using CalcManager.Interop;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace CalculatorApp.ViewModel
{
    public delegate void HideHistoryClickedHandler();
    public delegate void HistoryItemClickedHandler(HistoryItemViewModel e);

    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class HistoryViewModel : ObservableObject, IHistoryDisplayTarget
    {
        private readonly CalculatorManagerWrapper _calculatorManager;
        private int _currentMode; // CalculatorMode enum value
        private string _localizedHistoryCleared;
        private string _localizedHistorySlotCleared;

        private ObservableCollection<HistoryItemViewModel> _items;

        [ObservableProperty]
        private bool _areHistoryShortcutsEnabled;

        private NarratorAnnouncement _historyAnnouncement;

        public event HideHistoryClickedHandler HideHistoryClicked;
        public event HistoryItemClickedHandler HistoryItemClicked;

        internal HistoryViewModel(CalculatorManagerWrapper calculatorManager)
        {
            _calculatorManager = calculatorManager; // may be null if engine failed to init
            AreHistoryShortcutsEnabled = true;
            Items = new ObservableCollection<HistoryItemViewModel>();
        }

        public ObservableCollection<HistoryItemViewModel> Items
        {
            get => _items;
            private set => SetProperty(ref _items, value);
        }

        public NarratorAnnouncement HistoryAnnouncement
        {
            get => _historyAnnouncement;
            private set => SetProperty(ref _historyAnnouncement, value);
        }

        public int ItemsCount => Items?.Count ?? 0;

        public void OnHistoryItemAdded(uint addedItemIndex)
        {
            var newItem = _calculatorManager.GetHistoryItem(addedItemIndex);
            if (newItem == null)
            {
                return;
            }

            var localizer = LocalizationSettings.GetInstance();
            string expression = newItem.Expression;
            string result = newItem.Result;
            localizer.LocalizeDisplayValue(ref expression);
            localizer.LocalizeDisplayValue(ref result);

            var item = new HistoryItemViewModel(expression, result, newItem.Tokens, newItem.Commands);

            // Check if we have hit the max items
            if (Items.Count >= (int)(_calculatorManager?.MaxHistorySize ?? 0))
            {
                Items.RemoveAt(Items.Count - 1);
            }

            Items.Insert(0, item);
            OnPropertyChanged(nameof(ItemsCount));
        }

        public RelayCommand<object> ClearCommand => new RelayCommand<object>(OnClearCommand);
        public RelayCommand<object> HideCommand => new RelayCommand<object>(OnHideCommand);

        public void OnHideCommand(object e)
        {
            HideHistoryClicked?.Invoke();
        }

        public void OnClearCommand(object e)
        {
            if (AreHistoryShortcutsEnabled)
            {
                _calculatorManager?.ClearHistory();

                if (Items.Count > 0)
                {
                    Items.Clear();
                    OnPropertyChanged(nameof(ItemsCount));
                }

                if (_localizedHistoryCleared == null)
                {
                    _localizedHistoryCleared = AppResourceProvider.GetInstance().GetResourceString("HistoryList_Cleared");
                }
                HistoryAnnouncement = CalculatorAnnouncement.GetHistoryClearedAnnouncement(_localizedHistoryCleared);
            }
        }

        public void ShowItem(HistoryItemViewModel e)
        {
            int index = Items.IndexOf(e);
            if (index >= 0)
            {
                TraceLogger.GetInstance().LogHistoryItemLoad((ViewMode)_currentMode, Items.Count, index);
                HistoryItemClicked?.Invoke(e);
            }
        }

        public void DeleteItem(HistoryItemViewModel e)
        {
            int itemIndex = Items.IndexOf(e);
            if (itemIndex >= 0)
            {
                if ((_calculatorManager?.RemoveHistoryItem((uint)itemIndex) ?? false))
                {
                    Items.RemoveAt(itemIndex);
                    OnPropertyChanged(nameof(ItemsCount));
                }
            }

            string localizedIndex = (itemIndex + 1).ToString();
            LocalizationSettings.GetInstance().LocalizeDisplayValue(ref localizedIndex);

            _localizedHistorySlotCleared = AppResourceProvider.GetInstance().GetResourceString("Format_HistorySlotCleared");
            string announcement = LocalizationStringUtil.GetLocalizedString(_localizedHistorySlotCleared, localizedIndex);
            HistoryAnnouncement = CalculatorAnnouncement.GetHistorySlotClearedAnnouncement(announcement);
        }

        public void ReloadHistory(ViewMode currentMode)
        {
            if (currentMode == ViewMode.Standard)
            {
                _currentMode = 0; // CalculatorMode.Standard
            }
            else if (currentMode == ViewMode.Scientific)
            {
                _currentMode = 1; // CalculatorMode.Scientific
            }
            else
            {
                return;
            }

            var historyListModel = _calculatorManager?.GetHistoryItems();
            var historyListVM = new ObservableCollection<HistoryItemViewModel>();
            var localizer = LocalizationSettings.GetInstance();

            if (historyListModel != null && historyListModel.Length > 0)
            {
                // Iterate in reverse order
                for (int i = historyListModel.Length - 1; i >= 0; i--)
                {
                    var histItem = historyListModel[i];
                    string expression = histItem.Expression;
                    string result = histItem.Result;
                    localizer.LocalizeDisplayValue(ref expression);
                    localizer.LocalizeDisplayValue(ref result);

                    var item = new HistoryItemViewModel(expression, result, histItem.Tokens, histItem.Commands);
                    historyListVM.Add(item);
                }
            }

            Items = historyListVM;
            OnPropertyChanged(nameof(ItemsCount));
        }

        internal void SetCalculatorDisplay(CalculatorDisplay calculatorDisplay)
        {
            calculatorDisplay.SetHistoryCallback(this);
        }

        internal ulong GetMaxItemSize()
        {
            return (ulong)(_calculatorManager?.MaxHistorySize ?? 0);
        }

    }
}
