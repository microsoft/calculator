// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

using namespace CalculatorApp::Common;
using namespace CalculatorUnitTests;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation::Collections;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    TEST_CLASS(NavCategoryUnitTests)
    {
    public:
        TEST_METHOD(Serialize);
        TEST_METHOD(Deserialize_AllValid);
        TEST_METHOD(Deserialize_AllInvalid);

        TEST_METHOD(IsValidViewMode_AllValid);
        TEST_METHOD(IsValidViewMode_AllInvalid);

        TEST_METHOD(IsCalculatorViewMode);
        TEST_METHOD(IsDateCalculatorViewMode);
        TEST_METHOD(IsConverterViewMode);

        TEST_METHOD(GetFriendlyName);
        TEST_METHOD(GetGroupType);
        TEST_METHOD(GetIndex);
        TEST_METHOD(GetPosition);
        TEST_METHOD(GetIndex_GetPosition_Relationship);
        TEST_METHOD(GetIndexInGroup);

        TEST_METHOD(GetViewModeForVirtualKey);
    };

    void NavCategoryUnitTests::Serialize()
    {
        // While values in other tests may change (for example, the order
        // of a navigation item might change), these values should NEVER
        // change.  We are validating the unique ID for each mode, not
        // it's position or index.
        VERIFY_ARE_EQUAL(0, NavCategory::Serialize(ViewMode::Standard));
        VERIFY_ARE_EQUAL(1, NavCategory::Serialize(ViewMode::Scientific));
        VERIFY_ARE_EQUAL(2, NavCategory::Serialize(ViewMode::Programmer));
        VERIFY_ARE_EQUAL(3, NavCategory::Serialize(ViewMode::Date));
        VERIFY_ARE_EQUAL(16, NavCategory::Serialize(ViewMode::Currency));
        VERIFY_ARE_EQUAL(4, NavCategory::Serialize(ViewMode::Volume));
        VERIFY_ARE_EQUAL(5, NavCategory::Serialize(ViewMode::Length));
        VERIFY_ARE_EQUAL(6, NavCategory::Serialize(ViewMode::Weight));
        VERIFY_ARE_EQUAL(7, NavCategory::Serialize(ViewMode::Temperature));
        VERIFY_ARE_EQUAL(8, NavCategory::Serialize(ViewMode::Energy));
        VERIFY_ARE_EQUAL(9, NavCategory::Serialize(ViewMode::Area));
        VERIFY_ARE_EQUAL(10, NavCategory::Serialize(ViewMode::Speed));
        VERIFY_ARE_EQUAL(11, NavCategory::Serialize(ViewMode::Time));
        VERIFY_ARE_EQUAL(12, NavCategory::Serialize(ViewMode::Power));
        VERIFY_ARE_EQUAL(13, NavCategory::Serialize(ViewMode::Data));
        VERIFY_ARE_EQUAL(14, NavCategory::Serialize(ViewMode::Pressure));
        VERIFY_ARE_EQUAL(15, NavCategory::Serialize(ViewMode::Angle));

        VERIFY_ARE_EQUAL(-1, NavCategory::Serialize(ViewMode::None));
    }

    void NavCategoryUnitTests::Deserialize_AllValid()
    {
        // While values in other tests may change (for example, the order
        // of a navigation item might change), these values should NEVER
        // change.  We are validating the unique ID for each mode, not
        // it's position or index.
        VERIFY_ARE_EQUAL(ViewMode::Standard, NavCategory::Deserialize(ref new Box<int>(0)));
        VERIFY_ARE_EQUAL(ViewMode::Scientific, NavCategory::Deserialize(ref new Box<int>(1)));
        VERIFY_ARE_EQUAL(ViewMode::Programmer, NavCategory::Deserialize(ref new Box<int>(2)));
        VERIFY_ARE_EQUAL(ViewMode::Date, NavCategory::Deserialize(ref new Box<int>(3)));
        VERIFY_ARE_EQUAL(ViewMode::Currency, NavCategory::Deserialize(ref new Box<int>(16)));
        VERIFY_ARE_EQUAL(ViewMode::Volume, NavCategory::Deserialize(ref new Box<int>(4)));
        VERIFY_ARE_EQUAL(ViewMode::Length, NavCategory::Deserialize(ref new Box<int>(5)));
        VERIFY_ARE_EQUAL(ViewMode::Weight, NavCategory::Deserialize(ref new Box<int>(6)));
        VERIFY_ARE_EQUAL(ViewMode::Temperature, NavCategory::Deserialize(ref new Box<int>(7)));
        VERIFY_ARE_EQUAL(ViewMode::Energy, NavCategory::Deserialize(ref new Box<int>(8)));
        VERIFY_ARE_EQUAL(ViewMode::Area, NavCategory::Deserialize(ref new Box<int>(9)));
        VERIFY_ARE_EQUAL(ViewMode::Speed, NavCategory::Deserialize(ref new Box<int>(10)));
        VERIFY_ARE_EQUAL(ViewMode::Time, NavCategory::Deserialize(ref new Box<int>(11)));
        VERIFY_ARE_EQUAL(ViewMode::Power, NavCategory::Deserialize(ref new Box<int>(12)));
        VERIFY_ARE_EQUAL(ViewMode::Data, NavCategory::Deserialize(ref new Box<int>(13)));
        VERIFY_ARE_EQUAL(ViewMode::Pressure, NavCategory::Deserialize(ref new Box<int>(14)));
        VERIFY_ARE_EQUAL(ViewMode::Angle, NavCategory::Deserialize(ref new Box<int>(15)));
    }

    void NavCategoryUnitTests::Deserialize_AllInvalid()
    {
        VERIFY_ARE_EQUAL(ViewMode::None, NavCategory::Deserialize(nullptr));
        VERIFY_ARE_EQUAL(ViewMode::None, NavCategory::Deserialize(ref new String(L"fail")));

        // Boundary testing
        VERIFY_ARE_EQUAL(ViewMode::None, NavCategory::Deserialize(ref new Box<int>(-1)));
        VERIFY_ARE_EQUAL(ViewMode::None, NavCategory::Deserialize(ref new Box<int>(17)));
    }

    void NavCategoryUnitTests::IsValidViewMode_AllValid()
    {
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Standard));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Scientific));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Programmer));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Date));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Currency));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Volume));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Length));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Weight));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Temperature));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Energy));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Area));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Speed));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Time));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Power));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Data));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Pressure));
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(ViewMode::Angle));
    }

    void NavCategoryUnitTests::IsValidViewMode_AllInvalid()
    {
        VERIFY_IS_FALSE(NavCategory::IsValidViewMode(ViewMode::None));

        // There are 17 total options so int 17 should be the first invalid
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(static_cast<ViewMode>(16)));
        VERIFY_IS_FALSE(NavCategory::IsValidViewMode(static_cast<ViewMode>(17)));

        // Also verify the lower bound
        VERIFY_IS_TRUE(NavCategory::IsValidViewMode(static_cast<ViewMode>(0)));
        VERIFY_IS_FALSE(NavCategory::IsValidViewMode(static_cast<ViewMode>(-1)));
    }

    void NavCategoryUnitTests::IsCalculatorViewMode()
    {
        VERIFY_IS_TRUE(NavCategory::IsCalculatorViewMode(ViewMode::Standard));
        VERIFY_IS_TRUE(NavCategory::IsCalculatorViewMode(ViewMode::Scientific));
        VERIFY_IS_TRUE(NavCategory::IsCalculatorViewMode(ViewMode::Programmer));

        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Date));

        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Currency));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Volume));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Length));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Weight));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Temperature));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Energy));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Area));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Speed));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Time));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Power));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Data));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Pressure));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Angle));
    }

    void NavCategoryUnitTests::IsDateCalculatorViewMode()
    {
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Standard));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Scientific));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Programmer));

        VERIFY_IS_TRUE(NavCategory::IsDateCalculatorViewMode(ViewMode::Date));

        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Currency));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Volume));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Length));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Weight));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Temperature));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Energy));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Area));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Speed));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Time));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Power));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Data));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Pressure));
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Angle));
    }

    void NavCategoryUnitTests::IsConverterViewMode()
    {
        VERIFY_IS_FALSE(NavCategory::IsConverterViewMode(ViewMode::Standard));
        VERIFY_IS_FALSE(NavCategory::IsConverterViewMode(ViewMode::Scientific));
        VERIFY_IS_FALSE(NavCategory::IsConverterViewMode(ViewMode::Programmer));

        VERIFY_IS_FALSE(NavCategory::IsConverterViewMode(ViewMode::Date));

        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Currency));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Volume));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Length));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Weight));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Temperature));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Energy));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Area));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Speed));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Time));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Power));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Data));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Pressure));
        VERIFY_IS_TRUE(NavCategory::IsConverterViewMode(ViewMode::Angle));
    }

    void NavCategoryUnitTests::GetFriendlyName()
    {
        VERIFY_ARE_EQUAL(StringReference(L"Standard"), NavCategory::GetFriendlyName(ViewMode::Standard));
        VERIFY_ARE_EQUAL(StringReference(L"Scientific"), NavCategory::GetFriendlyName(ViewMode::Scientific));
        VERIFY_ARE_EQUAL(StringReference(L"Programmer"), NavCategory::GetFriendlyName(ViewMode::Programmer));
        VERIFY_ARE_EQUAL(StringReference(L"Date"), NavCategory::GetFriendlyName(ViewMode::Date));
        VERIFY_ARE_EQUAL(StringReference(L"Currency"), NavCategory::GetFriendlyName(ViewMode::Currency));
        VERIFY_ARE_EQUAL(StringReference(L"Volume"), NavCategory::GetFriendlyName(ViewMode::Volume));
        VERIFY_ARE_EQUAL(StringReference(L"Length"), NavCategory::GetFriendlyName(ViewMode::Length));
        VERIFY_ARE_EQUAL(StringReference(L"Weight and Mass"), NavCategory::GetFriendlyName(ViewMode::Weight));
        VERIFY_ARE_EQUAL(StringReference(L"Temperature"), NavCategory::GetFriendlyName(ViewMode::Temperature));
        VERIFY_ARE_EQUAL(StringReference(L"Energy"), NavCategory::GetFriendlyName(ViewMode::Energy));
        VERIFY_ARE_EQUAL(StringReference(L"Area"), NavCategory::GetFriendlyName(ViewMode::Area));
        VERIFY_ARE_EQUAL(StringReference(L"Speed"), NavCategory::GetFriendlyName(ViewMode::Speed));
        VERIFY_ARE_EQUAL(StringReference(L"Time"), NavCategory::GetFriendlyName(ViewMode::Time));
        VERIFY_ARE_EQUAL(StringReference(L"Power"), NavCategory::GetFriendlyName(ViewMode::Power));
        VERIFY_ARE_EQUAL(StringReference(L"Data"), NavCategory::GetFriendlyName(ViewMode::Data));
        VERIFY_ARE_EQUAL(StringReference(L"Pressure"), NavCategory::GetFriendlyName(ViewMode::Pressure));
        VERIFY_ARE_EQUAL(StringReference(L"Angle"), NavCategory::GetFriendlyName(ViewMode::Angle));

        VERIFY_ARE_EQUAL(StringReference(L"None"), NavCategory::GetFriendlyName(ViewMode::None));
    }

    void NavCategoryUnitTests::GetGroupType()
    {
        VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategory::GetGroupType(ViewMode::Standard));
        VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategory::GetGroupType(ViewMode::Scientific));
        VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategory::GetGroupType(ViewMode::Programmer));
        VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategory::GetGroupType(ViewMode::Date));

        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Currency));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Volume));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Length));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Weight));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Temperature));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Energy));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Area));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Speed));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Time));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Power));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Data));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Pressure));
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategory::GetGroupType(ViewMode::Angle));
    }

    void NavCategoryUnitTests::GetIndex()
    {
        // Index is the 0-based ordering of modes
        vector<ViewMode> orderedModes = {
            ViewMode::Standard,
            ViewMode::Scientific,
            ViewMode::Programmer,
            ViewMode::Date,
            ViewMode::Currency,
            ViewMode::Volume,
            ViewMode::Length,
            ViewMode::Weight,
            ViewMode::Temperature,
            ViewMode::Energy,
            ViewMode::Area,
            ViewMode::Speed,
            ViewMode::Time,
            ViewMode::Power,
            ViewMode::Data,
            ViewMode::Pressure,
            ViewMode::Angle
        };

        for (size_t index = 0; index < orderedModes.size(); index++)
        {
            ViewMode mode = orderedModes[index];
            VERIFY_ARE_EQUAL(index, (size_t)NavCategory::GetIndex(mode));
        }

        VERIFY_ARE_EQUAL(-1, NavCategory::GetIndex(ViewMode::None));
    }

    void NavCategoryUnitTests::GetPosition()
    {
        // Position is the 1-based ordering of modes
        vector<ViewMode> orderedModes = {
            ViewMode::Standard,
            ViewMode::Scientific,
            ViewMode::Programmer,
            ViewMode::Date,
            ViewMode::Currency,
            ViewMode::Volume,
            ViewMode::Length,
            ViewMode::Weight,
            ViewMode::Temperature,
            ViewMode::Energy,
            ViewMode::Area,
            ViewMode::Speed,
            ViewMode::Time,
            ViewMode::Power,
            ViewMode::Data,
            ViewMode::Pressure,
            ViewMode::Angle
        };

        for (size_t pos = 1; pos <= orderedModes.size(); pos++)
        {
            ViewMode mode = orderedModes[pos - 1];
            VERIFY_ARE_EQUAL(pos, (size_t)NavCategory::GetPosition(mode));
        }

        VERIFY_ARE_EQUAL(-1, NavCategory::GetPosition(ViewMode::None));
    }

    void NavCategoryUnitTests::GetIndex_GetPosition_Relationship()
    {
        // Index should be 1 less than Position.
        // The other checks verify the order of Index and Position.
        // Just verify the relationship here.
        VERIFY_ARE_EQUAL(NavCategory::GetIndex(ViewMode::Standard) + 1, NavCategory::GetPosition(ViewMode::Standard));
        VERIFY_ARE_EQUAL(NavCategory::GetPosition(ViewMode::Volume) - 1, NavCategory::GetIndex(ViewMode::Volume));
    }

    void NavCategoryUnitTests::GetIndexInGroup()
    {
        VERIFY_ARE_EQUAL(0, NavCategory::GetIndexInGroup(ViewMode::Standard, CategoryGroupType::Calculator));
        VERIFY_ARE_EQUAL(1, NavCategory::GetIndexInGroup(ViewMode::Scientific, CategoryGroupType::Calculator));
        VERIFY_ARE_EQUAL(2, NavCategory::GetIndexInGroup(ViewMode::Programmer, CategoryGroupType::Calculator));
        VERIFY_ARE_EQUAL(3, NavCategory::GetIndexInGroup(ViewMode::Date, CategoryGroupType::Calculator));

        VERIFY_ARE_EQUAL(0, NavCategory::GetIndexInGroup(ViewMode::Currency, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(1, NavCategory::GetIndexInGroup(ViewMode::Volume, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(2, NavCategory::GetIndexInGroup(ViewMode::Length, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(3, NavCategory::GetIndexInGroup(ViewMode::Weight, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(4, NavCategory::GetIndexInGroup(ViewMode::Temperature, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(5, NavCategory::GetIndexInGroup(ViewMode::Energy, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(6, NavCategory::GetIndexInGroup(ViewMode::Area, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(7, NavCategory::GetIndexInGroup(ViewMode::Speed, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(8, NavCategory::GetIndexInGroup(ViewMode::Time, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(9, NavCategory::GetIndexInGroup(ViewMode::Power, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(10, NavCategory::GetIndexInGroup(ViewMode::Data, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(11, NavCategory::GetIndexInGroup(ViewMode::Pressure, CategoryGroupType::Converter));
        VERIFY_ARE_EQUAL(12, NavCategory::GetIndexInGroup(ViewMode::Angle, CategoryGroupType::Converter));

        VERIFY_ARE_EQUAL(-1, NavCategory::GetIndexInGroup(ViewMode::None, CategoryGroupType::Calculator));
        VERIFY_ARE_EQUAL(-1, NavCategory::GetIndexInGroup(ViewMode::None, CategoryGroupType::Converter));
    }

    void NavCategoryUnitTests::GetViewModeForVirtualKey()
    {
        VERIFY_ARE_EQUAL(ViewMode::Standard, NavCategory::GetViewModeForVirtualKey(MyVirtualKey::Number1));
        VERIFY_ARE_EQUAL(ViewMode::Scientific, NavCategory::GetViewModeForVirtualKey(MyVirtualKey::Number2));
        VERIFY_ARE_EQUAL(ViewMode::Programmer, NavCategory::GetViewModeForVirtualKey(MyVirtualKey::Number3));
    }

    TEST_CLASS(NavCategoryGroupUnitTests)
    {
    public:
        TEST_METHOD(CreateNavCategoryGroup);

    private:
        void ValidateNavCategory(IObservableVector<NavCategory^>^ categories, unsigned int index, ViewMode expectedMode, int expectedPosition)
        {
            VERIFY_IS_LESS_THAN(0u, categories->Size);
            VERIFY_IS_GREATER_THAN(categories->Size, index);

            NavCategory^ category = categories->GetAt(index);
            VERIFY_ARE_EQUAL(expectedMode, category->Mode);
            VERIFY_ARE_EQUAL(expectedPosition, category->Position);
        }
    };

    void NavCategoryGroupUnitTests::CreateNavCategoryGroup()
    {
        IObservableVector<NavCategoryGroup^>^ menuOptions = NavCategoryGroup::CreateMenuOptions();

        VERIFY_ARE_EQUAL(2, menuOptions->Size);

        NavCategoryGroup^ calculatorGroup = menuOptions->GetAt(0);
        VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, calculatorGroup->GroupType);

        IObservableVector<NavCategory^>^ calculatorCategories = calculatorGroup->Categories;
        VERIFY_ARE_EQUAL(4, calculatorCategories->Size);
        ValidateNavCategory(calculatorCategories, 0u, ViewMode::Standard, 1);
        ValidateNavCategory(calculatorCategories, 1u, ViewMode::Scientific, 2);
        ValidateNavCategory(calculatorCategories, 2u, ViewMode::Programmer, 3);
        ValidateNavCategory(calculatorCategories, 3u, ViewMode::Date, 4);

        NavCategoryGroup^ converterGroup = menuOptions->GetAt(1);
        VERIFY_ARE_EQUAL(CategoryGroupType::Converter, converterGroup->GroupType);

        IObservableVector<NavCategory^>^ converterCategories = converterGroup->Categories;
        VERIFY_ARE_EQUAL(13, converterCategories->Size);
        ValidateNavCategory(converterCategories, 0u, ViewMode::Currency, 5);
        ValidateNavCategory(converterCategories, 1u, ViewMode::Volume, 6);
        ValidateNavCategory(converterCategories, 2u, ViewMode::Length, 7);
        ValidateNavCategory(converterCategories, 3u, ViewMode::Weight, 8);
        ValidateNavCategory(converterCategories, 4u, ViewMode::Temperature, 9);
        ValidateNavCategory(converterCategories, 5u, ViewMode::Energy, 10);
        ValidateNavCategory(converterCategories, 6u, ViewMode::Area, 11);
        ValidateNavCategory(converterCategories, 7u, ViewMode::Speed, 12);
        ValidateNavCategory(converterCategories, 8u, ViewMode::Time, 13);
        ValidateNavCategory(converterCategories, 9u, ViewMode::Power, 14);
        ValidateNavCategory(converterCategories, 10u, ViewMode::Data, 15);
        ValidateNavCategory(converterCategories, 11u, ViewMode::Pressure, 16);
        ValidateNavCategory(converterCategories, 12u, ViewMode::Angle, 17);
    }
}
