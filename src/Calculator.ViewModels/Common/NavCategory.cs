// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// The NavCategory group of classes and enumerations is intended to serve
// as a single location for storing metadata about a navigation mode.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using CommunityToolkit.Mvvm.ComponentModel;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.ViewModel.Common
{
    // Don't change the order of these enums
    // and definitely don't use int arithmetic to change modes.
    public enum ViewMode
    {
        None = -1,
        Standard = 0,
        Scientific = 1,
        Programmer = 2,
        Date = 3,
        Volume = 4,
        Length = 5,
        Weight = 6,
        Temperature = 7,
        Energy = 8,
        Area = 9,
        Speed = 10,
        Time = 11,
        Power = 12,
        Data = 13,
        Pressure = 14,
        Angle = 15,
        Currency = 16,
        Graphing = 17
    }

    public enum CategoryGroupType
    {
        None = -1,
        Calculator = 0,
        Converter = 1
    }

    internal struct NavCategoryInitializer
    {
        public ViewMode ViewMode;
        public int SerializationId;
        public string FriendlyName;
        public string NameResourceKey;
        public string Glyph;
        public CategoryGroupType GroupType;
        public MyVirtualKey VirtualKey;
        public string AccessKey; // null means use resource
        public bool SupportsNegative;
    }

    internal struct NavCategoryGroupInitializer
    {
        public CategoryGroupType Type;
        public string HeaderResourceKey;
        public string ModeResourceKey;
        public string AutomationResourceKey;
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class NavCategory : ObservableObject
    {
        private string _name;
        private string _automationName;
        private string _glyph;
        private ViewMode _viewMode;
        private string _accessKey;
        private bool _supportsNegative;

        [ObservableProperty]
        private bool _isEnabled;

        private string _modeString;

        internal NavCategory(
            string name,
            string automationName,
            string glyph,
            string accessKey,
            string mode,
            ViewMode viewMode,
            bool supportsNegative,
            bool isEnabled)
        {
            _name = name;
            _automationName = automationName;
            _glyph = glyph;
            _accessKey = accessKey;
            _modeString = mode;
            _viewMode = viewMode;
            _supportsNegative = supportsNegative;
            _isEnabled = isEnabled;
        }

        public string Name => _name;
        public string AutomationName => _automationName;
        public string Glyph => _glyph;
        public ViewMode ViewMode => _viewMode;
        public string AccessKey => _accessKey;
        public bool SupportsNegative => _supportsNegative;

        public string AutomationId => _viewMode.ToString();

        public static bool IsCalculatorViewMode(ViewMode mode)
        {
            // Historically, Calculator modes are Standard, Scientific, and Programmer.
            return !IsDateCalculatorViewMode(mode) && !IsGraphingCalculatorViewMode(mode) && IsModeInCategoryGroup(mode, CategoryGroupType.Calculator);
        }

        public static bool IsGraphingCalculatorViewMode(ViewMode mode)
        {
            return mode == ViewMode.Graphing;
        }

        public static bool IsDateCalculatorViewMode(ViewMode mode)
        {
            return mode == ViewMode.Date;
        }

        public static bool IsConverterViewMode(ViewMode mode)
        {
            return IsModeInCategoryGroup(mode, CategoryGroupType.Converter);
        }

        private static bool IsModeInCategoryGroup(ViewMode mode, CategoryGroupType type)
        {
            return NavCategoryStates.CategoryManifest.Any(init => init.ViewMode == mode && init.GroupType == type);
        }
    }

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class NavCategoryGroup : ObservableObject
    {
        private string _name;
        private string _automationName;
        private CategoryGroupType _groupType;
        private ObservableCollection<NavCategory> _categories;

        internal NavCategoryGroup(NavCategoryGroupInitializer groupInitializer)
        {
            _categories = new ObservableCollection<NavCategory>();
            _groupType = groupInitializer.Type;

            var resProvider = AppResourceProvider.GetInstance();
            _name = resProvider.GetResourceString(groupInitializer.HeaderResourceKey);
            string groupMode = resProvider.GetResourceString(groupInitializer.ModeResourceKey);
            string automationName = resProvider.GetResourceString(groupInitializer.AutomationResourceKey);

            string navCategoryHeaderAutomationNameFormat = resProvider.GetResourceString("NavCategoryHeader_AutomationNameFormat");
            _automationName = LocalizationStringUtil.GetLocalizedString(navCategoryHeaderAutomationNameFormat, automationName);

            string navCategoryItemAutomationNameFormat = resProvider.GetResourceString("NavCategoryItem_AutomationNameFormat");

            foreach (var categoryInitializer in NavCategoryStates.CategoryManifest)
            {
                if (categoryInitializer.GroupType == groupInitializer.Type)
                {
                    string nameResourceKey = categoryInitializer.NameResourceKey;
                    string categoryName = resProvider.GetResourceString(nameResourceKey + "Text");
                    string categoryAutomationName = LocalizationStringUtil.GetLocalizedString(
                        navCategoryItemAutomationNameFormat, categoryName, _name);

                    string accessKey = categoryInitializer.AccessKey ?? resProvider.GetResourceString(nameResourceKey + "AccessKey");

                    _categories.Add(new NavCategory(
                        categoryName,
                        categoryAutomationName,
                        categoryInitializer.Glyph,
                        accessKey,
                        groupMode,
                        categoryInitializer.ViewMode,
                        categoryInitializer.SupportsNegative,
                        categoryInitializer.ViewMode != ViewMode.Graphing));
                }
            }
        }

        public string Name
        {
            get => _name;
            private set => SetProperty(ref _name, value);
        }

        public string AutomationName
        {
            get => _automationName;
            private set => SetProperty(ref _automationName, value);
        }

        public CategoryGroupType GroupType
        {
            get => _groupType;
            private set => SetProperty(ref _groupType, value);
        }

        public ObservableCollection<NavCategory> Categories
        {
            get => _categories;
            private set => SetProperty(ref _categories, value);
        }
    }

    public sealed class NavCategoryStates
    {
        // Calculator categories always support negative and positive.
        private const bool SupportsAll = true;

        // Converter categories usually only support positive.
        private const bool SupportsNegative = true;
        private const bool PositiveOnly = false;

        // vvv THESE CONSTANTS SHOULD NEVER CHANGE vvv
        private const int StandardId = 0;
        private const int ScientificId = 1;
        private const int ProgrammerId = 2;
        private const int DateId = 3;
        private const int VolumeId = 4;
        private const int LengthId = 5;
        private const int WeightId = 6;
        private const int TemperatureId = 7;
        private const int EnergyId = 8;
        private const int AreaId = 9;
        private const int SpeedId = 10;
        private const int TimeId = 11;
        private const int PowerId = 12;
        private const int DataId = 13;
        private const int PressureId = 14;
        private const int AngleId = 15;
        private const int CurrencyId = 16;
        private const int GraphingId = 17;
        // ^^^ THESE CONSTANTS SHOULD NEVER CHANGE ^^^

        private static string _currentUserId;

        internal static readonly NavCategoryInitializer[] CategoryManifest = new[]
        {
            new NavCategoryInitializer { ViewMode = ViewMode.Standard, SerializationId = StandardId, FriendlyName = "Standard", NameResourceKey = "StandardMode", Glyph = "\uE8EF", GroupType = CategoryGroupType.Calculator, VirtualKey = MyVirtualKey.Number1, AccessKey = "1", SupportsNegative = SupportsAll },
            new NavCategoryInitializer { ViewMode = ViewMode.Scientific, SerializationId = ScientificId, FriendlyName = "Scientific", NameResourceKey = "ScientificMode", Glyph = "\uF196", GroupType = CategoryGroupType.Calculator, VirtualKey = MyVirtualKey.Number2, AccessKey = "2", SupportsNegative = SupportsAll },
            new NavCategoryInitializer { ViewMode = ViewMode.Graphing, SerializationId = GraphingId, FriendlyName = "Graphing", NameResourceKey = "GraphingCalculatorMode", Glyph = "\uF770", GroupType = CategoryGroupType.Calculator, VirtualKey = MyVirtualKey.Number3, AccessKey = "3", SupportsNegative = SupportsAll },
            new NavCategoryInitializer { ViewMode = ViewMode.Programmer, SerializationId = ProgrammerId, FriendlyName = "Programmer", NameResourceKey = "ProgrammerMode", Glyph = "\uECCE", GroupType = CategoryGroupType.Calculator, VirtualKey = MyVirtualKey.Number4, AccessKey = "4", SupportsNegative = SupportsAll },
            new NavCategoryInitializer { ViewMode = ViewMode.Date, SerializationId = DateId, FriendlyName = "Date", NameResourceKey = "DateCalculationMode", Glyph = "\uE787", GroupType = CategoryGroupType.Calculator, VirtualKey = MyVirtualKey.Number5, AccessKey = "5", SupportsNegative = SupportsAll },
            new NavCategoryInitializer { ViewMode = ViewMode.Currency, SerializationId = CurrencyId, FriendlyName = "Currency", NameResourceKey = "CategoryName_Currency", Glyph = "\uEB0D", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Volume, SerializationId = VolumeId, FriendlyName = "Volume", NameResourceKey = "CategoryName_Volume", Glyph = "\uF1AA", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Length, SerializationId = LengthId, FriendlyName = "Length", NameResourceKey = "CategoryName_Length", Glyph = "\uECC6", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Weight, SerializationId = WeightId, FriendlyName = "Weight and Mass", NameResourceKey = "CategoryName_Weight", Glyph = "\uF4C1", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Temperature, SerializationId = TemperatureId, FriendlyName = "Temperature", NameResourceKey = "CategoryName_Temperature", Glyph = "\uE7A3", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = SupportsNegative },
            new NavCategoryInitializer { ViewMode = ViewMode.Energy, SerializationId = EnergyId, FriendlyName = "Energy", NameResourceKey = "CategoryName_Energy", Glyph = "\uECAD", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Area, SerializationId = AreaId, FriendlyName = "Area", NameResourceKey = "CategoryName_Area", Glyph = "\uE809", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Speed, SerializationId = SpeedId, FriendlyName = "Speed", NameResourceKey = "CategoryName_Speed", Glyph = "\uEADA", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Time, SerializationId = TimeId, FriendlyName = "Time", NameResourceKey = "CategoryName_Time", Glyph = "\uE917", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Power, SerializationId = PowerId, FriendlyName = "Power", NameResourceKey = "CategoryName_Power", Glyph = "\uE945", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = SupportsNegative },
            new NavCategoryInitializer { ViewMode = ViewMode.Data, SerializationId = DataId, FriendlyName = "Data", NameResourceKey = "CategoryName_Data", Glyph = "\uF20F", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Pressure, SerializationId = PressureId, FriendlyName = "Pressure", NameResourceKey = "CategoryName_Pressure", Glyph = "\uEC4A", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = PositiveOnly },
            new NavCategoryInitializer { ViewMode = ViewMode.Angle, SerializationId = AngleId, FriendlyName = "Angle", NameResourceKey = "CategoryName_Angle", Glyph = "\uF515", GroupType = CategoryGroupType.Converter, VirtualKey = MyVirtualKey.None, AccessKey = null, SupportsNegative = SupportsNegative },
        };

        public static void SetCurrentUser(string userId)
        {
            _currentUserId = userId;
        }

        public static IList<NavCategoryGroup> CreateMenuOptions()
        {
            var menuOptions = new List<NavCategoryGroup>();
            menuOptions.Add(CreateCalculatorCategoryGroup());
            menuOptions.Add(CreateConverterCategoryGroup());
            return menuOptions;
        }

        public static NavCategoryGroup CreateCalculatorCategoryGroup()
        {
            return new NavCategoryGroup(new NavCategoryGroupInitializer
            {
                Type = CategoryGroupType.Calculator,
                HeaderResourceKey = "CalculatorModeTextCaps",
                ModeResourceKey = "CalculatorModeText",
                AutomationResourceKey = "CalculatorModePluralText"
            });
        }

        public static NavCategoryGroup CreateConverterCategoryGroup()
        {
            return new NavCategoryGroup(new NavCategoryGroupInitializer
            {
                Type = CategoryGroupType.Converter,
                HeaderResourceKey = "ConverterModeTextCaps",
                ModeResourceKey = "ConverterModeText",
                AutomationResourceKey = "ConverterModePluralText"
            });
        }

        public static bool IsValidViewMode(ViewMode mode)
        {
            return CategoryManifest.Any(init => init.ViewMode == mode);
        }

        public static bool IsViewModeEnabled(ViewMode mode)
        {
            // Graphing mode may be disabled by policy; other modes are always enabled
            return mode != ViewMode.Graphing || true; // Simplified: always enabled in managed
        }

        // This function should only be used when storing the mode to app data.
        public static int Serialize(ViewMode mode)
        {
            var match = CategoryManifest.FirstOrDefault(init => init.ViewMode == mode);
            return match.FriendlyName != null ? match.SerializationId : -1;
        }

        // This function should only be used when restoring the mode from app data.
        public static ViewMode Deserialize(object obj)
        {
            if (obj is int serializationId)
            {
                var match = CategoryManifest.FirstOrDefault(init => init.SerializationId == serializationId);
                if (match.FriendlyName != null)
                {
                    if (match.ViewMode == ViewMode.Graphing)
                    {
                        return IsViewModeEnabled(ViewMode.Graphing) ? match.ViewMode : ViewMode.None;
                    }
                    return match.ViewMode;
                }
            }
            return ViewMode.None;
        }

        public static ViewMode GetViewModeForFriendlyName(string name)
        {
            var match = CategoryManifest.FirstOrDefault(init => init.FriendlyName == name);
            return match.FriendlyName != null ? match.ViewMode : ViewMode.None;
        }

        public static string GetFriendlyName(ViewMode mode)
        {
            var match = CategoryManifest.FirstOrDefault(init => init.ViewMode == mode);
            return match.FriendlyName ?? "None";
        }

        public static string GetNameResourceKey(ViewMode mode)
        {
            var match = CategoryManifest.FirstOrDefault(init => init.ViewMode == mode);
            return match.NameResourceKey != null ? match.NameResourceKey + "Text" : null;
        }

        public static CategoryGroupType GetGroupType(ViewMode mode)
        {
            var match = CategoryManifest.FirstOrDefault(init => init.ViewMode == mode);
            return match.FriendlyName != null ? match.GroupType : CategoryGroupType.None;
        }

        // GetIndex is 0-based, GetPosition is 1-based
        public static int GetIndex(ViewMode mode)
        {
            int position = GetPosition(mode);
            return Math.Max(-1, position - 1);
        }

        public static int GetFlatIndex(ViewMode mode)
        {
            int index = -1;
            CategoryGroupType type = CategoryGroupType.None;

            foreach (var init in CategoryManifest)
            {
                ++index;
                if (init.GroupType != type)
                {
                    type = init.GroupType;
                    ++index;
                }
                if (init.ViewMode == mode)
                {
                    return index;
                }
            }

            return -1;
        }

        public static int GetIndexInGroup(ViewMode mode, CategoryGroupType type)
        {
            int index = -1;
            foreach (var init in CategoryManifest)
            {
                if (init.GroupType == type)
                {
                    ++index;
                    if (init.ViewMode == mode)
                    {
                        return index;
                    }
                }
            }
            return -1;
        }

        // GetIndex is 0-based, GetPosition is 1-based
        public static int GetPosition(ViewMode mode)
        {
            int position = 0;
            foreach (var init in CategoryManifest)
            {
                ++position;
                if (init.ViewMode == mode)
                {
                    return position;
                }
            }
            return -1;
        }

        public static ViewMode GetViewModeForVirtualKey(MyVirtualKey virtualKey)
        {
            var match = CategoryManifest.FirstOrDefault(init => init.VirtualKey == virtualKey);
            return match.FriendlyName != null ? match.ViewMode : ViewMode.None;
        }

        public static void GetCategoryAcceleratorKeys(IList<MyVirtualKey> results)
        {
            if (results != null)
            {
                results.Clear();
                foreach (var category in CategoryManifest)
                {
                    if (category.VirtualKey != MyVirtualKey.None)
                    {
                        results.Add(category.VirtualKey);
                    }
                }
            }
        }
    }
}
