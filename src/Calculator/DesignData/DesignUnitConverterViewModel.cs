// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;

#if WINDOWS_UWP || NETFX_CORE
using Windows.UI.Xaml;
#endif

namespace CalculatorApp.DesignData
{
    public sealed class CategoryViewModel : ObservableObject
    {
        private string _name;
#if WINDOWS_UWP || NETFX_CORE
        private Visibility _negateVisibility;
#else
        private int _negateVisibility;
#endif

        public CategoryViewModel(string name)
        {
            _name = name;
#if WINDOWS_UWP || NETFX_CORE
            _negateVisibility = Visibility.Collapsed;
#else
            _negateVisibility = 1;
#endif
        }

#if WINDOWS_UWP || NETFX_CORE
        public CategoryViewModel(string name, Visibility negateVisibility)
        {
            _name = name;
            _negateVisibility = negateVisibility;
        }

        public Visibility NegateVisibility
        {
            get => _negateVisibility;
            set => SetProperty(ref _negateVisibility, value);
        }
#endif

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }
    }

    public sealed class UnitViewModel : ObservableObject
    {
        private string _name;
        private string _abbreviation;

        public UnitViewModel(string unit, string abbr)
        {
            _name = unit;
            _abbreviation = abbr;
        }

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public string Abbreviation
        {
            get => _abbreviation;
            set => SetProperty(ref _abbreviation, value);
        }
    }

    public sealed class UnitConverterSupplementaryResultViewModel : ObservableObject
    {
        private string _value;
        private UnitViewModel _unit;

        public UnitConverterSupplementaryResultViewModel(string value, string unit, string abbr)
        {
            _value = value;
            _unit = new UnitViewModel(unit, abbr);
        }

        public string Value
        {
            get => _value;
            set => SetProperty(ref _value, value);
        }

        public UnitViewModel Unit
        {
            get => _unit;
            set => SetProperty(ref _unit, value);
        }
    }

    public sealed class UnitConverterViewModel : ObservableObject
    {
        private string _value1;
        private string _value2;
        private ObservableCollection<CategoryViewModel> _categories;
        private CategoryViewModel _currentCategory;
        private ObservableCollection<UnitViewModel> _units;
        private UnitViewModel _unit1;
        private UnitViewModel _unit2;
        private bool _value1Active;
        private bool _value2Active;
        private ObservableCollection<UnitConverterSupplementaryResultViewModel> _supplementaryResults;

        public UnitConverterViewModel()
        {
            _value1 = "Åy24";
            _value2 = "Åy183";
            _value1Active = true;
            _value2Active = false;

            _supplementaryResults = new ObservableCollection<UnitConverterSupplementaryResultViewModel>
            {
                new UnitConverterSupplementaryResultViewModel("128", "Kilograms", "Kgs"),
                new UnitConverterSupplementaryResultViewModel("42.55", "Liters", "ÅyL"),
                new UnitConverterSupplementaryResultViewModel("1.5e3", "Gallons", "G"),
                new UnitConverterSupplementaryResultViewModel("1929", "Gigabyte", "GB")
            };

            _categories = new ObservableCollection<CategoryViewModel>();
            _categories.Add(new CategoryViewModel("Volume"));
#if WINDOWS_UWP || NETFX_CORE
            _categories.Add(new CategoryViewModel("Temperature", Visibility.Visible));
#else
            _categories.Add(new CategoryViewModel("Temperature"));
#endif
            _currentCategory = new CategoryViewModel("ÅyTime");
            _categories.Add(_currentCategory);
            _categories.Add(new CategoryViewModel("Speed"));

            _units = new ObservableCollection<UnitViewModel>();
            _unit1 = new UnitViewModel("ÅySeconds", "S");
            _unit2 = new UnitViewModel("ÅyMinutes", "M");
            _units.Add(new UnitViewModel("Miliseconds", "MS"));
            _units.Add(_unit1);
            _units.Add(_unit2);
            _units.Add(new UnitViewModel("Hours", "HRs"));
        }

        public string Value1
        {
            get => _value1;
            set => SetProperty(ref _value1, value);
        }

        public string Value2
        {
            get => _value2;
            set => SetProperty(ref _value2, value);
        }

        public ObservableCollection<CategoryViewModel> Categories
        {
            get => _categories;
            set => SetProperty(ref _categories, value);
        }

        public CategoryViewModel CurrentCategory
        {
            get => _currentCategory;
            set => SetProperty(ref _currentCategory, value);
        }

        public ObservableCollection<UnitViewModel> Units
        {
            get => _units;
            set => SetProperty(ref _units, value);
        }

        public UnitViewModel Unit1
        {
            get => _unit1;
            set => SetProperty(ref _unit1, value);
        }

        public UnitViewModel Unit2
        {
            get => _unit2;
            set => SetProperty(ref _unit2, value);
        }

        public bool Value1Active
        {
            get => _value1Active;
            set => SetProperty(ref _value1Active, value);
        }

        public bool Value2Active
        {
            get => _value2Active;
            set => SetProperty(ref _value2Active, value);
        }

        public ObservableCollection<UnitConverterSupplementaryResultViewModel> SupplementaryResults
        {
            get => _supplementaryResults;
            set => SetProperty(ref _supplementaryResults, value);
        }
    }
}

namespace Numbers.DesignData
{
    public sealed class CategoryViewModel : ObservableObject
    {
        private string _name;
#if WINDOWS_UWP || NETFX_CORE
        private Visibility _negateVisibility;
#else
        private int _negateVisibility;
#endif

        public CategoryViewModel(string name)
        {
            _name = name;
#if WINDOWS_UWP || NETFX_CORE
            _negateVisibility = Visibility.Collapsed;
#else
            _negateVisibility = 1;
#endif
        }

#if WINDOWS_UWP || NETFX_CORE
        public CategoryViewModel(string name, Visibility negateVisibility)
        {
            _name = name;
            _negateVisibility = negateVisibility;
        }

        public Visibility NegateVisibility
        {
            get => _negateVisibility;
            set => SetProperty(ref _negateVisibility, value);
        }
#endif

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }
    }

    public sealed class UnitViewModel : ObservableObject
    {
        private string _name;
        private string _abbreviation;

        public UnitViewModel(string unit, string abbr)
        {
            _name = unit;
            _abbreviation = abbr;
        }

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public string Abbreviation
        {
            get => _abbreviation;
            set => SetProperty(ref _abbreviation, value);
        }
    }

    public sealed class UnitConverterSupplementaryResultViewModel : ObservableObject
    {
        private string _value;
        private UnitViewModel _unit;

        public UnitConverterSupplementaryResultViewModel(string value, string unit, string abbr)
        {
            _value = value;
            _unit = new UnitViewModel(unit, abbr);
        }

        public string Value
        {
            get => _value;
            set => SetProperty(ref _value, value);
        }

        public UnitViewModel Unit
        {
            get => _unit;
            set => SetProperty(ref _unit, value);
        }
    }

    public sealed class UnitConverterViewModel : ObservableObject
    {
        private string _value1;
        private string _value2;
        private ObservableCollection<CategoryViewModel> _categories;
        private CategoryViewModel _currentCategory;
        private ObservableCollection<UnitViewModel> _units;
        private UnitViewModel _unit1;
        private UnitViewModel _unit2;
        private bool _value1Active;
        private bool _value2Active;
        private ObservableCollection<UnitConverterSupplementaryResultViewModel> _supplementaryResults;

        public UnitConverterViewModel()
        {
            _value1 = "Åy24";
            _value2 = "Åy183";
            _value1Active = true;
            _value2Active = false;

            _supplementaryResults = new ObservableCollection<UnitConverterSupplementaryResultViewModel>
            {
                new UnitConverterSupplementaryResultViewModel("128", "Kilograms", "Kgs"),
                new UnitConverterSupplementaryResultViewModel("42.55", "Liters", "ÅyL"),
                new UnitConverterSupplementaryResultViewModel("1.5e3", "Gallons", "G"),
                new UnitConverterSupplementaryResultViewModel("1929", "Gigabyte", "GB")
            };

            _categories = new ObservableCollection<CategoryViewModel>();
            _categories.Add(new CategoryViewModel("Volume"));
#if WINDOWS_UWP || NETFX_CORE
            _categories.Add(new CategoryViewModel("Temperature", Visibility.Visible));
#else
            _categories.Add(new CategoryViewModel("Temperature"));
#endif
            _currentCategory = new CategoryViewModel("ÅyTime");
            _categories.Add(_currentCategory);
            _categories.Add(new CategoryViewModel("Speed"));

            _units = new ObservableCollection<UnitViewModel>();
            _unit1 = new UnitViewModel("ÅySeconds", "S");
            _unit2 = new UnitViewModel("ÅyMinutes", "M");
            _units.Add(new UnitViewModel("Miliseconds", "MS"));
            _units.Add(_unit1);
            _units.Add(_unit2);
            _units.Add(new UnitViewModel("Hours", "HRs"));
        }

        public string Value1
        {
            get => _value1;
            set => SetProperty(ref _value1, value);
        }

        public string Value2
        {
            get => _value2;
            set => SetProperty(ref _value2, value);
        }

        public ObservableCollection<CategoryViewModel> Categories
        {
            get => _categories;
            set => SetProperty(ref _categories, value);
        }

        public CategoryViewModel CurrentCategory
        {
            get => _currentCategory;
            set => SetProperty(ref _currentCategory, value);
        }

        public ObservableCollection<UnitViewModel> Units
        {
            get => _units;
            set => SetProperty(ref _units, value);
        }

        public UnitViewModel Unit1
        {
            get => _unit1;
            set => SetProperty(ref _unit1, value);
        }

        public UnitViewModel Unit2
        {
            get => _unit2;
            set => SetProperty(ref _unit2, value);
        }

        public bool Value1Active
        {
            get => _value1Active;
            set => SetProperty(ref _value1Active, value);
        }

        public bool Value2Active
        {
            get => _value2Active;
            set => SetProperty(ref _value2Active, value);
        }

        public ObservableCollection<UnitConverterSupplementaryResultViewModel> SupplementaryResults
        {
            get => _supplementaryResults;
            set => SetProperty(ref _supplementaryResults, value);
        }
    }
}
