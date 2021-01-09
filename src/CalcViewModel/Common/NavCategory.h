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

#pragma once

#include "Utils.h"
#include "MyVirtualKey.h"

namespace CalculatorApp
{
    namespace Common
    {
        // Don't change the order of these enums
        // and definitely don't use int arithmetic
        // to change modes.
    public
        enum class ViewMode
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
        };

    public
        enum class CategoryGroupType
        {
            None = -1,
            Calculator = 0,
            Converter = 1
        };

    private
        struct NavCategoryInitializer
        {
            NavCategoryInitializer(
                ViewMode mode,
                int id,
                wchar_t * name,
                wchar_t * nameKey,
                wchar_t * glyph,
                CategoryGroupType group,
                MyVirtualKey vKey,
                wchar_t * aKey,
                bool categorySupportsNegative,
                bool enabled)
                : viewMode(mode)
                , serializationId(id)
                , friendlyName(name)
                , nameResourceKey(nameKey)
                , glyph(glyph)
                , groupType(group)
                , virtualKey(vKey)
                , accessKey(aKey)
                , supportsNegative(categorySupportsNegative)
                , isEnabled(enabled)
            {
            }

            ViewMode viewMode;
            int serializationId;
            wchar_t* friendlyName;
            wchar_t* nameResourceKey;
            wchar_t* glyph;
            CategoryGroupType groupType;
            MyVirtualKey virtualKey;
            wchar_t* accessKey;
            bool supportsNegative;
            bool isEnabled;
        };

    private
        struct NavCategoryGroupInitializer
        {
            NavCategoryGroupInitializer(CategoryGroupType t, wchar_t * h, wchar_t * n, wchar_t * a)
                : type(t)
                , headerResourceKey(h)
                , modeResourceKey(n)
                , automationResourceKey(a)
            {
            }

            CategoryGroupType type;
            wchar_t* headerResourceKey;
            wchar_t* modeResourceKey;
            wchar_t* automationResourceKey;
        };

        [Windows::UI::Xaml::Data::Bindable] public ref class NavCategory sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            OBSERVABLE_OBJECT();
            PROPERTY_R(Platform::String ^, Name);
            PROPERTY_R(Platform::String ^, AutomationName);
            PROPERTY_R(Platform::String ^, Glyph);
            PROPERTY_R(ViewMode, Mode);
            PROPERTY_R(Platform::String ^, AccessKey);
            PROPERTY_R(bool, SupportsNegative);
            PROPERTY_R(bool, IsEnabled);

            property Platform::String
                ^ AutomationId { Platform::String ^ get() { return m_Mode.ToString(); } }


            // For saving/restoring last mode used.
            static int Serialize(ViewMode mode);
            static ViewMode Deserialize(Platform::Object ^ obj);
            static ViewMode GetViewModeForFriendlyName(Platform::String ^ name);

            static bool IsValidViewMode(ViewMode mode);
            static bool IsViewModeEnabled(ViewMode mode);
            static bool IsCalculatorViewMode(ViewMode mode);
            static bool IsGraphingCalculatorViewMode(ViewMode mode);
            static bool IsDateCalculatorViewMode(ViewMode mode);
            static bool IsConverterViewMode(ViewMode mode);

            static void CreateCategoryManifest(Windows::System::User ^ user);

            static Platform::String ^ GetFriendlyName(ViewMode mode);
            static Platform::String ^ GetNameResourceKey(ViewMode mode);
            static CategoryGroupType GetGroupType(ViewMode mode);

            // GetIndex is 0-based, GetPosition is 1-based
            static int GetIndex(ViewMode mode);
            static int GetFlatIndex(ViewMode mode);
            static int GetIndexInGroup(ViewMode mode, CategoryGroupType type);
            static int GetPosition(ViewMode mode);

            static ViewMode GetViewModeForVirtualKey(MyVirtualKey virtualKey);

            internal : NavCategory(
                           Platform::String ^ name,
                           Platform::String ^ automationName,
                           Platform::String ^ glyph,
                           Platform::String ^ accessKey,
                           Platform::String ^ mode,
                           ViewMode viewMode,
                           bool supportsNegative,
                           bool isEnabled)
                : m_Name(name)
                , m_AutomationName(automationName)
                , m_Glyph(glyph)
                , m_AccessKey(accessKey)
                , m_modeString(mode)
                , m_Mode(viewMode)
                , m_SupportsNegative(supportsNegative)
                , m_IsEnabled(isEnabled)
            {
            }

            static std::vector<MyVirtualKey> GetCategoryAcceleratorKeys();

        private:
            static bool IsModeInCategoryGroup(ViewMode mode, CategoryGroupType groupType);

            Platform::String ^ m_modeString;
        };

        [Windows::UI::Xaml::Data::Bindable] public ref class NavCategoryGroup sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:
            OBSERVABLE_OBJECT();
            OBSERVABLE_PROPERTY_R(Platform::String ^, Name);
            OBSERVABLE_PROPERTY_R(Platform::String ^, AutomationName);
            OBSERVABLE_PROPERTY_R(CategoryGroupType, GroupType);
            OBSERVABLE_PROPERTY_R(Windows::Foundation::Collections::IObservableVector<NavCategory ^> ^, Categories);

            static Windows::Foundation::Collections::IObservableVector<NavCategoryGroup ^> ^ CreateMenuOptions();

            internal : static NavCategoryGroup ^ CreateCalculatorCategory();
            static NavCategoryGroup ^ CreateConverterCategory();

        private:
            NavCategoryGroup(const NavCategoryGroupInitializer& groupInitializer);
        };
    }
}
