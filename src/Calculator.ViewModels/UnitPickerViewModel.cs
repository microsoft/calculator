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
        private readonly IReadOnlyList<UnitPickerItem> _allItems;
        private readonly IUnitSearchMatcher _matcher;

        public UnitPickerViewModel(
            IEnumerable<UnitPickerCategory> catalog,
            IUnitSearchMatcher matcher = null,
            int selectedCategoryId = -1)
        {
            _matcher = matcher ?? new SubstringUnitSearchMatcher();

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
            var allItems = new List<UnitPickerItem>();
            foreach (var entry in entries)
            {
                foreach (var unit in entry.Units)
                {
                    if (unit != null)
                    {
                        allItems.Add(new UnitPickerItem(unit, entry.Name, entry.CategoryId));
                    }
                }
            }
            _allItems = allItems;

            FilteredUnits = new ObservableCollection<UnitPickerItem>();
            _selectedCategory = FindCategory(selectedCategoryId) ?? (entries.Count > 0 ? entries[0] : null);
            RebuildFilteredUnits();
        }

        public IReadOnlyList<UnitPickerCategory> Categories { get; }

        public ObservableCollection<UnitPickerItem> FilteredUnits { get; }

        [ObservableProperty]
        private UnitPickerCategory _selectedCategory;

        partial void OnSelectedCategoryChanged(UnitPickerCategory value)
        {
            if (value != null && string.IsNullOrEmpty(SearchText))
            {
                RebuildFilteredUnits();
            }
        }

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(CategoryListEnabled))]
        private string _searchText = string.Empty;

        partial void OnSearchTextChanged(string value)
        {
            RebuildFilteredUnits();
        }

        // The category list is a browse affordance only while there is no query; once the
        // user searches, results span every category, so the list is disabled and dimmed.
        public bool CategoryListEnabled => string.IsNullOrEmpty(SearchText);

        public bool HasResults => FilteredUnits.Count > 0;

        private void RebuildFilteredUnits()
        {
            FilteredUnits.Clear();

            if (string.IsNullOrEmpty(SearchText))
            {
                if (SelectedCategory != null)
                {
                    string categoryName = SelectedCategory.Name;
                    int categoryId = SelectedCategory.CategoryId;
                    foreach (var unit in SelectedCategory.Units)
                    {
                        if (unit != null)
                        {
                            FilteredUnits.Add(new UnitPickerItem(unit, categoryName, categoryId));
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

            OnPropertyChanged(nameof(HasResults));
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
    }
}
