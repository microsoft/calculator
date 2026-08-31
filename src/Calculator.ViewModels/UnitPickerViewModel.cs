// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;

namespace CalculatorApp.ViewModel
{
    /// <summary>
    /// Category-first unit picker (mirrors the converter redesign mockup, Design1Page):
    /// the selected category drives the unit list; typing a query switches to a flat
    /// cross-category search and dims the category list. Selecting a category only
    /// previews its units; committing a unit is the caller's job (UnitConverterViewModel).
    /// </summary>
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class UnitPickerViewModel : ObservableObject
    {
        private readonly IReadOnlyList<UnitPickerCategory> _catalog;
        private readonly List<UnitPickerItem> _allItems;
        private readonly IUnitSearchMatcher _matcher;
        private readonly int _selectedUnitId;
        private readonly Dictionary<int, UnitPickerCategoryLoadState> _categoryLoadStates;
        private UnitPickerItem _selectedUnit;

        public UnitPickerViewModel(
            IEnumerable<UnitPickerCategory> catalog,
            IUnitSearchMatcher matcher = null,
            int selectedCategoryId = -1,
            int selectedUnitId = -1,
            IReadOnlyDictionary<int, UnitPickerCategoryLoadState> categoryLoadStates = null)
        {
            _matcher = matcher ?? new SubstringUnitSearchMatcher();
            _selectedUnitId = selectedUnitId;
            _categoryLoadStates = categoryLoadStates == null
                ? new Dictionary<int, UnitPickerCategoryLoadState>()
                : new Dictionary<int, UnitPickerCategoryLoadState>(categoryLoadStates);

            var entries = new List<UnitPickerCategory>();
            foreach (var entry in catalog ?? Array.Empty<UnitPickerCategory>())
            {
                if (entry?.Category != null)
                {
                    entries.Add(entry);
                }
            }
            _catalog = entries;
            Categories = entries;

            // Flatten every unit once so cross-category search does not rebuild it each keystroke.
            _allItems = new List<UnitPickerItem>();
            RebuildAllItems();

            FilteredUnits = new ObservableCollection<UnitPickerItem>();
            _selectedCategory = FindCategory(selectedCategoryId) ?? (entries.Count > 0 ? entries[0] : null);
            RebuildFilteredUnits();
        }

        public IReadOnlyList<UnitPickerCategory> Categories { get; }

        public ObservableCollection<UnitPickerItem> FilteredUnits { get; }

        public UnitPickerItem SelectedUnit
        {
            get => _selectedUnit;
            private set => SetProperty(ref _selectedUnit, value);
        }

        [ObservableProperty]
        private UnitPickerCategory _selectedCategory;

        partial void OnSelectedCategoryChanged(UnitPickerCategory value)
        {
            if (value != null && string.IsNullOrEmpty(SearchText))
            {
                RebuildFilteredUnits();
            }

            NotifyDisplayStateChanged();
        }

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(CategoryListEnabled))]
        private string _searchText = string.Empty;

        partial void OnSearchTextChanged(string value)
        {
            RebuildFilteredUnits();
            NotifyDisplayStateChanged();
        }

        // The category list is a browse affordance only while there is no query; once the
        // user searches, results span every category, so the list is disabled and dimmed.
        public bool CategoryListEnabled => string.IsNullOrEmpty(SearchText);

        public bool HasResults => FilteredUnits.Count > 0;

        public bool HasNoSearchResults => !string.IsNullOrEmpty(SearchText) && !HasResults;

        public bool IsSelectedCategoryLoading =>
            string.IsNullOrEmpty(SearchText)
            && GetSelectedCategoryLoadState() == UnitPickerCategoryLoadState.Loading;

        public bool HasSelectedCategoryLoadFailed =>
            string.IsNullOrEmpty(SearchText)
            && GetSelectedCategoryLoadState() == UnitPickerCategoryLoadState.Failed;

        public bool AreUnitsVisible => !IsSelectedCategoryLoading && !HasSelectedCategoryLoadFailed;

        internal void UpdateCategory(
            int categoryId,
            IReadOnlyList<Unit> units,
            UnitPickerCategoryLoadState loadState)
        {
            var category = FindCategory(categoryId);
            if (category == null)
            {
                return;
            }

            category.UpdateUnits(units);
            _categoryLoadStates[categoryId] = loadState;
            RebuildAllItems();
            RebuildFilteredUnits();
            NotifyDisplayStateChanged();
        }

        private void RebuildFilteredUnits()
        {
            FilteredUnits.Clear();

            if (string.IsNullOrEmpty(SearchText))
            {
                if (SelectedCategory != null)
                {
                    int categoryId = SelectedCategory.CategoryId;
                    foreach (var item in _allItems)
                    {
                        if (item.CategoryId == categoryId)
                        {
                            FilteredUnits.Add(item);
                        }
                    }
                }
            }
            else
            {
                foreach (var item in _allItems)
                {
                    if (_matcher.IsMatch(SearchText, item.Unit))
                    {
                        FilteredUnits.Add(item);
                    }
                }
            }

            SelectedUnit = string.IsNullOrEmpty(SearchText) && SelectedCategory != null
                ? FindUnit(_selectedUnitId, SelectedCategory.CategoryId)
                : null;
            OnPropertyChanged(nameof(HasResults));
            OnPropertyChanged(nameof(HasNoSearchResults));
        }

        private UnitPickerCategoryLoadState GetSelectedCategoryLoadState()
        {
            if (SelectedCategory != null
                && _categoryLoadStates.TryGetValue(SelectedCategory.CategoryId, out var state))
            {
                return state;
            }

            return UnitPickerCategoryLoadState.Loaded;
        }

        private void RebuildAllItems()
        {
            _allItems.Clear();
            foreach (var entry in _catalog)
            {
                foreach (var unit in entry.Units)
                {
                    if (unit != null)
                    {
                        _allItems.Add(new UnitPickerItem(unit, entry.Name, entry.CategoryId));
                    }
                }
            }
        }

        private void NotifyDisplayStateChanged()
        {
            OnPropertyChanged(nameof(IsSelectedCategoryLoading));
            OnPropertyChanged(nameof(HasSelectedCategoryLoadFailed));
            OnPropertyChanged(nameof(AreUnitsVisible));
        }

        private UnitPickerCategory FindCategory(int categoryId)
        {
            if (categoryId < 0)
            {
                return null;
            }

            foreach (var entry in _catalog)
            {
                if (entry.CategoryId == categoryId)
                {
                    return entry;
                }
            }

            return null;
        }

        private UnitPickerItem FindUnit(int unitId, int categoryId)
        {
            if (unitId < 0)
            {
                return null;
            }

            foreach (var item in _allItems)
            {
                if (item.CategoryId == categoryId && item.Unit.ModelUnitID() == unitId)
                {
                    return item;
                }
            }

            return null;
        }
    }
}
