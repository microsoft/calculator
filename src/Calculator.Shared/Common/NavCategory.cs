// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* The NavCategory group of classes and enumerations is intended to serve
 * as a single location for storing metadata about a navigation mode.
 *
 * These .h and .cpp files:
 *     - Define the ViewMode enumeration which is used for setting the mode of the app.
 *     - Define a list of metadata associated with each ViewMode.
 *     - Define the order of groups and items in the navigation menu.
 *     - Provide a static helper function for creating the navigation menu options.
 *     - Provide static helper functions for querying information about a given ViewMode.
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using Windows.Foundation.Collections;

namespace CalculatorApp
{
    namespace Common
    {
        // Don't change the order of these enums
        // and definitely don't use int arithmetic
        // to change modes.
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
            Currency = 16
        };

        public enum CategoryGroupType
        {
            None = -1,
            Calculator = 0,
            Converter = 1
        };


        struct NavCategoryInitializer
        {
            public NavCategoryInitializer(
                ViewMode mode,
                int id,
                string name,
                string nameKey,
                string glyph,
                CategoryGroupType group,
                MyVirtualKey vKey,
                bool categorySupportsNegative)
            {
                this.viewMode = mode;
                this.serializationId = id;
                this.friendlyName = name;
                this.nameResourceKey = nameKey;
                this.glyph = glyph;
                this.groupType = group;
                this.virtualKey = vKey;
                this.supportsNegative = categorySupportsNegative;
            }

            public readonly ViewMode viewMode;
            public readonly int serializationId;
            public readonly string friendlyName;
            public readonly string nameResourceKey;
            public readonly string glyph;
            public readonly CategoryGroupType groupType;
            public readonly MyVirtualKey virtualKey;
            public readonly bool supportsNegative;
        };

        internal struct NavCategoryGroupInitializer
        {
            public NavCategoryGroupInitializer(CategoryGroupType t, string h, string n, string a)
            {
                type = t;
                headerResourceKey = h;
                modeResourceKey = n;
                automationResourceKey = a;
            }

            public readonly CategoryGroupType type;
            public readonly string headerResourceKey;
            public readonly string modeResourceKey;
            public readonly string automationResourceKey;
        };

        [Windows.UI.Xaml.Data.Bindable]
        public sealed class NavCategory : INotifyPropertyChanged
        {




            public event PropertyChangedEventHandler PropertyChanged;
            private void RaisePropertyChanged([System.Runtime.CompilerServices.CallerMemberName]string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));

            public string Name => m_name;

            public string AutomationName => m_varmationName;

            public string Glyph => m_glyph;

            int Position
            {
                get
                {
                    return m_position;
                }
            }

            public ViewMode Mode
            {
                get
                {
                    return m_viewMode;
                }
            }

            public string AutomationId => m_viewMode.ToString();

            public string AccessKey => m_accessKey;

            bool SupportsNegative
            {
                get
                {
                    return m_supportsNegative;
                }
            }

            internal NavCategory(
                           string name,
                           string varmationName,
                           string glyph,
                           string accessKey,
                           string mode,
                           ViewMode viewMode,
                           bool supportsNegative)

            {
                m_name = name;
                m_varmationName = varmationName;
                m_glyph = glyph;
                m_accessKey = accessKey;
                m_mode = mode;
                m_viewMode = viewMode;
                m_supportsNegative = supportsNegative;
                m_position = GetPosition(m_viewMode);
            }

            ViewMode m_viewMode;
            string m_name;
            string m_varmationName;
            string m_glyph;
            string m_accessKey;
            string m_mode;
            int m_position;
            bool m_supportsNegative;


            // Calculator categories always support negative and positive.
            const bool SUPPORTS_ALL = true;

            // Converter categories usually only support positive.
            const bool SUPPORTS_NEGATIVE = true;
            const bool POSITIVE_ONLY = false;

            // The order of items in this list determines the order of groups in the menu.
            internal static NavCategoryGroupInitializer?[] s_categoryGroupManifest = {
                new NavCategoryGroupInitializer( CategoryGroupType.Calculator, "CalculatorModeTextCaps", "CalculatorModeText", "CalculatorModePluralText" ),
                new NavCategoryGroupInitializer( CategoryGroupType.Converter, "ConverterModeTextCaps", "ConverterModeText", "ConverterModePluralText" )
            };

            // vvv THESE CONSTANTS SHOULD NEVER CHANGE vvv
            const int STANDARD_ID = 0;
            const int SCIENTIFIC_ID = 1;
            const int PROGRAMMER_ID = 2;
            const int DATE_ID = 3;
            const int VOLUME_ID = 4;
            const int LENGTH_ID = 5;
            const int WEIGHT_ID = 6;
            const int TEMPERATURE_ID = 7;
            const int ENERGY_ID = 8;
            const int AREA_ID = 9;
            const int SPEED_ID = 10;
            const int TIME_ID = 11;
            const int POWER_ID = 12;
            const int DATA_ID = 13;
            const int PRESSURE_ID = 14;
            const int ANGLE_ID = 15;
            const int CURRENCY_ID = 16;
            //^^ THESE CONSTANTS SHOULD NEVER CHANGE^^


            internal static NavCategoryInitializer?[] s_categoryManifest = { new NavCategoryInitializer( ViewMode.Standard,
                                                                                                        STANDARD_ID,
                                                                                                        "Standard",
                                                                                                        "StandardMode",
                                                                                                        "\uE8EF",
                                                                                                        CategoryGroupType.Calculator,
                                                                                                        MyVirtualKey.Number1,
                                                                                                        SUPPORTS_ALL ),
                                                                               new  NavCategoryInitializer( ViewMode.Scientific,
                                                                                                        SCIENTIFIC_ID,
                                                                                                        "Scientific",
                                                                                                        "ScientificMode",
                                                                                                        "\uF196",
                                                                                                        CategoryGroupType.Calculator,
                                                                                                        MyVirtualKey.Number2,
                                                                                                        SUPPORTS_ALL ),
                                                                                new NavCategoryInitializer( ViewMode.Programmer,
                                                                                                        PROGRAMMER_ID,
                                                                                                        "Programmer",
                                                                                                        "ProgrammerMode",
                                                                                                        "\uECCE",
                                                                                                        CategoryGroupType.Calculator,
                                                                                                        MyVirtualKey.Number3,
                                                                                                        SUPPORTS_ALL ),
                                                                                new NavCategoryInitializer( ViewMode.Date,
                                                                                                        DATE_ID,
                                                                                                        "Date",
                                                                                                        "DateCalculationMode",
                                                                                                        "\uE787",
                                                                                                        CategoryGroupType.Calculator,
                                                                                                        MyVirtualKey.Number4,
                                                                                                        SUPPORTS_ALL ),
                                                                                new NavCategoryInitializer( ViewMode.Currency,
                                                                                                        CURRENCY_ID,
                                                                                                        "Currency",
                                                                                                        "CategoryName_Currency",
                                                                                                        "\uEB0D",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Volume,
                                                                                                        VOLUME_ID,
                                                                                                        "Volume",
                                                                                                        "CategoryName_Volume",
                                                                                                        "\uF1AA",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Length,
                                                                                                        LENGTH_ID,
                                                                                                        "Length",
                                                                                                        "CategoryName_Length",
                                                                                                        "\uECC6",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Weight,
                                                                                                        WEIGHT_ID,
                                                                                                        "Weight and Mass",
                                                                                                        "CategoryName_Weight",
                                                                                                        "\uF4C1",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Temperature,
                                                                                                        TEMPERATURE_ID,
                                                                                                        "Temperature",
                                                                                                        "CategoryName_Temperature",
                                                                                                        "\uE7A3",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        SUPPORTS_NEGATIVE ),
                                                                                new NavCategoryInitializer( ViewMode.Energy,
                                                                                                        ENERGY_ID,
                                                                                                        "Energy",
                                                                                                        "CategoryName_Energy",
                                                                                                        "\uECAD",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Area,
                                                                                                        AREA_ID,
                                                                                                        "Area",
                                                                                                        "CategoryName_Area",
                                                                                                        "\uE809",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Speed,
                                                                                                        SPEED_ID,
                                                                                                        "Speed",
                                                                                                        "CategoryName_Speed",
                                                                                                        "\uEADA",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Time,
                                                                                                        TIME_ID,
                                                                                                        "Time",
                                                                                                        "CategoryName_Time",
                                                                                                        "\uE917",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Power,
                                                                                                        POWER_ID,
                                                                                                        "Power",
                                                                                                        "CategoryName_Power",
                                                                                                        "\uE945",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Data,
                                                                                                        DATA_ID,
                                                                                                        "Data",
                                                                                                        "CategoryName_Data",
                                                                                                        "\uF20F",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Pressure,
                                                                                                        PRESSURE_ID,
                                                                                                        "Pressure",
                                                                                                        "CategoryName_Pressure",
                                                                                                        "\uEC4A",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ),
                                                                                new NavCategoryInitializer( ViewMode.Angle,
                                                                                                        ANGLE_ID,
                                                                                                        "Angle",
                                                                                                        "CategoryName_Angle",
                                                                                                        "\uF515",
                                                                                                        CategoryGroupType.Converter,
                                                                                                        MyVirtualKey.None,
                                                                                                        POSITIVE_ONLY ) };

            // This function should only be used when storing the mode to app data.
            public static  int Serialize(ViewMode mode)
            {
                var iter = s_categoryManifest.FirstOrDefault(c => c.Value.viewMode == mode);

                return iter != null ? iter.Value.serializationId : -1;
            }

            // This function should only be used when restoring the mode from app data.
            static public ViewMode Deserialize(object obj)
            {
                // If we cast directly to ViewMode we will fail
                // because we technically store an int.
                // Need to cast to int, then ViewMode.
                var boxed = (int)(obj);
                if (obj is int serializationId)
                {
                    var iter = s_categoryManifest.FirstOrDefault(c => c.Value.serializationId == serializationId);

                    if (iter != null)
                    {
                        return iter.Value.viewMode;
                    }
                }

                return ViewMode.None;
            }

            public static bool IsValidViewMode(ViewMode mode)
            {
                var iter = s_categoryManifest.FirstOrDefault(c => c.Value.viewMode == mode);

                return iter != null;
            }

            public static bool IsCalculatorViewMode(ViewMode mode)
            {
                // Historically, Date Calculator is not a Calculator mode
                // even though it is in the Calculator category.
                return !IsDateCalculatorViewMode(mode) && IsModeInCategoryGroup(mode, CategoryGroupType.Calculator);
            }

            public static bool IsDateCalculatorViewMode(ViewMode mode)
            {
                return mode == ViewMode.Date;
            }

            public static bool IsConverterViewMode(ViewMode mode)
            {
                return IsModeInCategoryGroup(mode, CategoryGroupType.Converter);
            }

            public static bool IsModeInCategoryGroup(ViewMode mode, CategoryGroupType type)
            {
                var iter = s_categoryManifest.FirstOrDefault(c => c.Value.viewMode == mode && c.Value.groupType == type);

                return iter != null;
            }

            string GetFriendlyName(ViewMode mode)
            {
                var iter = s_categoryManifest.FirstOrDefault(c => c.Value.viewMode == mode);

                return (iter != null) ? iter.Value.friendlyName : "None";
            }

            ViewMode GetViewModeForFriendlyName(string name)
            {
                var iter = s_categoryManifest.FirstOrDefault(c => c.Value.friendlyName == name);

                return (iter != null) ? iter.Value.viewMode : ViewMode.None;
            }

            public static string GetNameResourceKey(ViewMode mode)
            {
                var iter = s_categoryManifest.FirstOrDefault(c => c.Value.viewMode == mode);

                return (iter != null) ? iter.Value.nameResourceKey + "Text" : null;
            }

            public static CategoryGroupType GetGroupType(ViewMode mode)
            {
                var iter = s_categoryManifest.FirstOrDefault(c => c.Value.viewMode == mode);

                return (iter != null) ? iter.Value.groupType : CategoryGroupType.None;
            }

            // GetIndex is 0-based, GetPosition is 1-based
            int GetIndex(ViewMode mode)
            {
                int position = GetPosition(mode);
                return Math.Max(-1, position - 1);
            }

            public static int GetFlatIndex(ViewMode mode)
            {
                int index = -1;
                CategoryGroupType type = CategoryGroupType.None;
                var iter = s_categoryManifest.FirstOrDefault(initializer => {
                    index++;
                    if (initializer.Value.groupType != type)
                    {
                        type = initializer.Value.groupType;
                        index++;
                    }

                    return initializer.Value.viewMode == mode;
                });

                return (iter != null) ? index : -1;
            }

            // GetIndex is 0-based, GetPosition is 1-based
            public static int GetIndexInGroup(ViewMode mode, CategoryGroupType type)
            {
                int index = -1;
                var iter = s_categoryManifest.FirstOrDefault(initializer =>
                {
                    if (initializer.Value.groupType == type)
                    {
                        index++;
                        return initializer.Value.viewMode == mode;
                    }

                    return false;
                });

                return (iter != null) ? index : -1;
            }

            // GetIndex is 0-based, GetPosition is 1-based
            int GetPosition(ViewMode mode)
            {
                int position = 0;
                var iter = s_categoryManifest.FirstOrDefault(initializer =>
                {
                    position++;
                    return initializer.Value.viewMode == mode;
                });

                return (iter != null) ? position : -1;
            }

            ViewMode GetViewModeForVirtualKey(MyVirtualKey virtualKey)
            {
                var iter = s_categoryManifest.FirstOrDefault(initializer => initializer.Value.virtualKey == virtualKey);

                return (iter != null) ? iter.Value.viewMode : ViewMode.None;
            }

            public List<MyVirtualKey> GetCategoryAcceleratorKeys()
            {
                List<MyVirtualKey> accelerators = new List<MyVirtualKey>();
                foreach (var category in s_categoryManifest)
                {
                    if (category.Value.virtualKey != MyVirtualKey.None)
                    {
                        accelerators.Add(category.Value.virtualKey);
                    }
                }

                return accelerators;
            }
        };

        [Windows.UI.Xaml.Data.Bindable]
        public sealed class NavCategoryGroup : INotifyPropertyChanged
        {
            public event PropertyChangedEventHandler PropertyChanged;
            private void RaisePropertyChanged([System.Runtime.CompilerServices.CallerMemberName]string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));
            private string m_Name;
            public string Name { get => m_Name; private set { m_Name = value; RaisePropertyChanged("Name"); } }


            private string m_AutomationName;
            public string AutomationName { get => m_AutomationName; private set { m_AutomationName = value; RaisePropertyChanged("AutomationName"); } }


            private CategoryGroupType m_GroupType;
            public CategoryGroupType GroupType { get => m_GroupType; private set { m_GroupType = value; RaisePropertyChanged("GroupType"); } }


            private ObservableCollection<NavCategory> m_Categories;
            public ObservableCollection<NavCategory> Categories { get => m_Categories; private set { m_Categories = value; RaisePropertyChanged("Categories"); } }


            internal NavCategoryGroup(NavCategoryGroupInitializer groupInitializer)
            {
                m_Categories = new ObservableCollection<NavCategory>();
                m_GroupType = groupInitializer.type;

                var resProvider = AppResourceProvider.GetInstance();
                string headerResourceKey = (groupInitializer.headerResourceKey);
                string modeResourceKey = (groupInitializer.modeResourceKey);
                string automationResourceKey = (groupInitializer.automationResourceKey);
                m_Name = resProvider.GetResourceString(headerResourceKey);
                string groupMode = resProvider.GetResourceString(modeResourceKey);
                string automationName = resProvider.GetResourceString(automationResourceKey);

                string navCategoryHeaderAutomationNameFormat = resProvider.GetResourceString("NavCategoryHeader_AutomationNameFormat");
                m_AutomationName = LocalizationStringUtil.GetLocalizedString(navCategoryHeaderAutomationNameFormat, automationName);

                string navCategoryItemAutomationNameFormat = resProvider.GetResourceString("NavCategoryItem_AutomationNameFormat");

                foreach (NavCategoryInitializer? categoryInitializer in NavCategory.s_categoryManifest)
                {
                    if (categoryInitializer.Value.groupType == groupInitializer.type)
                    {
                        string nameResourceKey = (categoryInitializer.Value.nameResourceKey);
                        string categoryName = resProvider.GetResourceString(nameResourceKey + "Text");
                        string categoryAutomationName = LocalizationStringUtil.GetLocalizedString(navCategoryItemAutomationNameFormat, categoryName, m_Name);

                        m_Categories.Add( new NavCategory(
                            categoryName,
                            categoryAutomationName,
                            (categoryInitializer.Value.glyph),
                            resProvider.GetResourceString(nameResourceKey + "AccessKey"),
                            groupMode,
                            categoryInitializer.Value.viewMode,
                            categoryInitializer.Value.supportsNegative));
                    }
                }
            }

            public static ObservableCollection<NavCategoryGroup> CreateMenuOptions()
            {
                var menuOptions =  new ObservableCollection<NavCategoryGroup>();
                menuOptions.Add(CreateCalculatorCategory());
                menuOptions.Add(CreateConverterCategory());
                return menuOptions;
            }

            static NavCategoryGroup CreateCalculatorCategory()
            {
                return  new NavCategoryGroup(NavCategory.s_categoryGroupManifest[0].Value);
            }

            static NavCategoryGroup CreateConverterCategory()
            {
                return  new NavCategoryGroup(NavCategory.s_categoryGroupManifest[1].Value);
            }

            List<NavCategoryInitializer> GetInitializerCategoryGroup(CategoryGroupType groupType)
            {
                return new List<NavCategoryInitializer>(
                    NavCategory.s_categoryManifest.Where(initializer => initializer.Value.groupType == groupType).Select(i => i.Value));
            }

            string GetHeaderResourceKey(CategoryGroupType type)
            {
                var iter = NavCategory.s_categoryGroupManifest.FirstOrDefault(initializer => initializer.Value.type == type);

                return (iter != null) ? (iter.Value.headerResourceKey) : null;
            }
        }
    }
}
