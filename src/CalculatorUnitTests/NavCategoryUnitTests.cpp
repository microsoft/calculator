// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

using namespace CalculatorApp::ViewModel::Common;
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
        TEST_METHOD(IsCalculatorViewMode);
        TEST_METHOD(IsDateCalculatorViewMode);
        TEST_METHOD(IsConverterViewMode);
    };

    void NavCategoryUnitTests::IsCalculatorViewMode()
    {
        VERIFY_IS_TRUE(NavCategory::IsCalculatorViewMode(ViewMode::Standard));
        VERIFY_IS_TRUE(NavCategory::IsCalculatorViewMode(ViewMode::Scientific));
        VERIFY_IS_TRUE(NavCategory::IsCalculatorViewMode(ViewMode::Programmer));

        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Date));
        VERIFY_IS_FALSE(NavCategory::IsCalculatorViewMode(ViewMode::Graphing));

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
        VERIFY_IS_FALSE(NavCategory::IsDateCalculatorViewMode(ViewMode::Graphing));

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
        VERIFY_IS_FALSE(NavCategory::IsConverterViewMode(ViewMode::Graphing));

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

    TEST_CLASS(NavCategoryGroupUnitTests)
    {
    public:
        TEST_METHOD(CreateNavCategoryGroup)
        {
            IVector<NavCategoryGroup ^> ^ menuOptions = NavCategoryStates::CreateMenuOptions();

            VERIFY_ARE_EQUAL(2, menuOptions->Size);

            NavCategoryGroup ^ calculatorGroup = menuOptions->GetAt(0);
            VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, calculatorGroup->GroupType);

            IVector<NavCategory ^> ^ calculatorCategories = calculatorGroup->Categories;
            ValidateNavCategory(calculatorCategories, 0u, ViewMode::Standard);
            ValidateNavCategory(calculatorCategories, 1u, ViewMode::Scientific);
            ValidateNavCategory(calculatorCategories, 2u, ViewMode::Graphing);
            ValidateNavCategory(calculatorCategories, 3u, ViewMode::Programmer);
            ValidateNavCategory(calculatorCategories, 4u, ViewMode::Date);
            VERIFY_ARE_EQUAL(5, calculatorCategories->Size);

            NavCategoryGroup ^ converterGroup = menuOptions->GetAt(1);
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, converterGroup->GroupType);

            IVector<NavCategory ^> ^ converterCategories = converterGroup->Categories;
            VERIFY_ARE_EQUAL(13, converterCategories->Size);
            ValidateNavCategory(converterCategories, 0u, ViewMode::Currency);
            ValidateNavCategory(converterCategories, 1u, ViewMode::Volume);
            ValidateNavCategory(converterCategories, 2u, ViewMode::Length);
            ValidateNavCategory(converterCategories, 3u, ViewMode::Weight);
            ValidateNavCategory(converterCategories, 4u, ViewMode::Temperature);
            ValidateNavCategory(converterCategories, 5u, ViewMode::Energy);
            ValidateNavCategory(converterCategories, 6u, ViewMode::Area);
            ValidateNavCategory(converterCategories, 7u, ViewMode::Speed);
            ValidateNavCategory(converterCategories, 8u, ViewMode::Time);
            ValidateNavCategory(converterCategories, 9u, ViewMode::Power);
            ValidateNavCategory(converterCategories, 10u, ViewMode::Data);
            ValidateNavCategory(converterCategories, 11u, ViewMode::Pressure);
            ValidateNavCategory(converterCategories, 12u, ViewMode::Angle);
        }

    private:
        void ValidateNavCategory(IVector<NavCategory ^> ^ categories, unsigned int index, ViewMode expectedMode)
        {
            VERIFY_IS_LESS_THAN(0u, categories->Size);
            VERIFY_IS_GREATER_THAN(categories->Size, index);

            NavCategory ^ category = categories->GetAt(index);
            VERIFY_ARE_EQUAL(expectedMode, category->ViewMode);
            VERIFY_ARE_EQUAL(category->IsEnabled, (expectedMode != ViewMode::Graphing) ? true : false);
        }
    };

    TEST_CLASS(NavCategoryStatesUnitTests)
    {
    public:
        TEST_METHOD(Serialize)
        {
            // While values in other tests may change (for example, the order
            // of a navigation item might change), these values should NEVER
            // change.  We are validating the unique ID for each mode, not
            // it's position or index.
            VERIFY_ARE_EQUAL(0, NavCategoryStates::Serialize(ViewMode::Standard));
            VERIFY_ARE_EQUAL(1, NavCategoryStates::Serialize(ViewMode::Scientific));
            VERIFY_ARE_EQUAL(2, NavCategoryStates::Serialize(ViewMode::Programmer));
            VERIFY_ARE_EQUAL(3, NavCategoryStates::Serialize(ViewMode::Date));
            VERIFY_ARE_EQUAL(16, NavCategoryStates::Serialize(ViewMode::Currency));
            VERIFY_ARE_EQUAL(4, NavCategoryStates::Serialize(ViewMode::Volume));
            VERIFY_ARE_EQUAL(5, NavCategoryStates::Serialize(ViewMode::Length));
            VERIFY_ARE_EQUAL(6, NavCategoryStates::Serialize(ViewMode::Weight));
            VERIFY_ARE_EQUAL(7, NavCategoryStates::Serialize(ViewMode::Temperature));
            VERIFY_ARE_EQUAL(8, NavCategoryStates::Serialize(ViewMode::Energy));
            VERIFY_ARE_EQUAL(9, NavCategoryStates::Serialize(ViewMode::Area));
            VERIFY_ARE_EQUAL(10, NavCategoryStates::Serialize(ViewMode::Speed));
            VERIFY_ARE_EQUAL(11, NavCategoryStates::Serialize(ViewMode::Time));
            VERIFY_ARE_EQUAL(12, NavCategoryStates::Serialize(ViewMode::Power));
            VERIFY_ARE_EQUAL(13, NavCategoryStates::Serialize(ViewMode::Data));
            VERIFY_ARE_EQUAL(14, NavCategoryStates::Serialize(ViewMode::Pressure));
            VERIFY_ARE_EQUAL(15, NavCategoryStates::Serialize(ViewMode::Angle));

            VERIFY_ARE_EQUAL(-1, NavCategoryStates::Serialize(ViewMode::None));
        }

        TEST_METHOD(Deserialize_AllValid)
        {
            // While values in other tests may change (for example, the order
            // of a navigation item might change), these values should NEVER
            // change.  We are validating the unique ID for each mode, not
            // it's position or index.
            VERIFY_ARE_EQUAL(ViewMode::Standard, NavCategoryStates::Deserialize(ref new Box<int>(0)));
            VERIFY_ARE_EQUAL(ViewMode::Scientific, NavCategoryStates::Deserialize(ref new Box<int>(1)));
            VERIFY_ARE_EQUAL(ViewMode::Programmer, NavCategoryStates::Deserialize(ref new Box<int>(2)));
            VERIFY_ARE_EQUAL(ViewMode::Date, NavCategoryStates::Deserialize(ref new Box<int>(3)));
            VERIFY_ARE_EQUAL(ViewMode::Currency, NavCategoryStates::Deserialize(ref new Box<int>(16)));
            VERIFY_ARE_EQUAL(ViewMode::Volume, NavCategoryStates::Deserialize(ref new Box<int>(4)));
            VERIFY_ARE_EQUAL(ViewMode::Length, NavCategoryStates::Deserialize(ref new Box<int>(5)));
            VERIFY_ARE_EQUAL(ViewMode::Weight, NavCategoryStates::Deserialize(ref new Box<int>(6)));
            VERIFY_ARE_EQUAL(ViewMode::Temperature, NavCategoryStates::Deserialize(ref new Box<int>(7)));
            VERIFY_ARE_EQUAL(ViewMode::Energy, NavCategoryStates::Deserialize(ref new Box<int>(8)));
            VERIFY_ARE_EQUAL(ViewMode::Area, NavCategoryStates::Deserialize(ref new Box<int>(9)));
            VERIFY_ARE_EQUAL(ViewMode::Speed, NavCategoryStates::Deserialize(ref new Box<int>(10)));
            VERIFY_ARE_EQUAL(ViewMode::Time, NavCategoryStates::Deserialize(ref new Box<int>(11)));
            VERIFY_ARE_EQUAL(ViewMode::Power, NavCategoryStates::Deserialize(ref new Box<int>(12)));
            VERIFY_ARE_EQUAL(ViewMode::Data, NavCategoryStates::Deserialize(ref new Box<int>(13)));
            VERIFY_ARE_EQUAL(ViewMode::Pressure, NavCategoryStates::Deserialize(ref new Box<int>(14)));
            VERIFY_ARE_EQUAL(ViewMode::Angle, NavCategoryStates::Deserialize(ref new Box<int>(15)));
        }

        TEST_METHOD(Deserialize_AllInvalid)
        {
            VERIFY_ARE_EQUAL(ViewMode::None, NavCategoryStates::Deserialize(nullptr));
            VERIFY_ARE_EQUAL(ViewMode::None, NavCategoryStates::Deserialize(ref new String(L"fail")));

            // Boundary testing
            VERIFY_ARE_EQUAL(ViewMode::None, NavCategoryStates::Deserialize(ref new Box<int>(-1)));
            VERIFY_ARE_EQUAL(ViewMode::None, NavCategoryStates::Deserialize(ref new Box<int>(18)));
        }

        TEST_METHOD(IsValidViewMode_AllValid)
        {
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Standard));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Scientific));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Programmer));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Date));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Graphing));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Currency));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Volume));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Length));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Weight));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Temperature));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Energy));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Area));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Speed));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Time));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Power));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Data));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Pressure));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(ViewMode::Angle));
        }

        TEST_METHOD(IsValidViewMode_AllInvalid)
        {
            VERIFY_IS_FALSE(NavCategoryStates::IsValidViewMode(ViewMode::None));
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(static_cast<ViewMode>(17)));
            VERIFY_IS_FALSE(NavCategoryStates::IsValidViewMode(static_cast<ViewMode>(18)));

            // Also verify the lower bound
            VERIFY_IS_TRUE(NavCategoryStates::IsValidViewMode(static_cast<ViewMode>(0)));
            VERIFY_IS_FALSE(NavCategoryStates::IsValidViewMode(static_cast<ViewMode>(-1)));
        }

        TEST_METHOD(GetFriendlyName)
        {
            VERIFY_ARE_EQUAL(StringReference(L"Standard"), NavCategoryStates::GetFriendlyName(ViewMode::Standard));
            VERIFY_ARE_EQUAL(StringReference(L"Scientific"), NavCategoryStates::GetFriendlyName(ViewMode::Scientific));
            VERIFY_ARE_EQUAL(StringReference(L"Programmer"), NavCategoryStates::GetFriendlyName(ViewMode::Programmer));
            VERIFY_ARE_EQUAL(StringReference(L"Date"), NavCategoryStates::GetFriendlyName(ViewMode::Date));
            VERIFY_ARE_EQUAL(StringReference(L"Graphing"), NavCategoryStates::GetFriendlyName(ViewMode::Graphing));
            VERIFY_ARE_EQUAL(StringReference(L"Currency"), NavCategoryStates::GetFriendlyName(ViewMode::Currency));
            VERIFY_ARE_EQUAL(StringReference(L"Volume"), NavCategoryStates::GetFriendlyName(ViewMode::Volume));
            VERIFY_ARE_EQUAL(StringReference(L"Length"), NavCategoryStates::GetFriendlyName(ViewMode::Length));
            VERIFY_ARE_EQUAL(StringReference(L"Weight and Mass"), NavCategoryStates::GetFriendlyName(ViewMode::Weight));
            VERIFY_ARE_EQUAL(StringReference(L"Temperature"), NavCategoryStates::GetFriendlyName(ViewMode::Temperature));
            VERIFY_ARE_EQUAL(StringReference(L"Energy"), NavCategoryStates::GetFriendlyName(ViewMode::Energy));
            VERIFY_ARE_EQUAL(StringReference(L"Area"), NavCategoryStates::GetFriendlyName(ViewMode::Area));
            VERIFY_ARE_EQUAL(StringReference(L"Speed"), NavCategoryStates::GetFriendlyName(ViewMode::Speed));
            VERIFY_ARE_EQUAL(StringReference(L"Time"), NavCategoryStates::GetFriendlyName(ViewMode::Time));
            VERIFY_ARE_EQUAL(StringReference(L"Power"), NavCategoryStates::GetFriendlyName(ViewMode::Power));
            VERIFY_ARE_EQUAL(StringReference(L"Data"), NavCategoryStates::GetFriendlyName(ViewMode::Data));
            VERIFY_ARE_EQUAL(StringReference(L"Pressure"), NavCategoryStates::GetFriendlyName(ViewMode::Pressure));
            VERIFY_ARE_EQUAL(StringReference(L"Angle"), NavCategoryStates::GetFriendlyName(ViewMode::Angle));

            VERIFY_ARE_EQUAL(StringReference(L"None"), NavCategoryStates::GetFriendlyName(ViewMode::None));
        }

        TEST_METHOD(GetGroupType)
        {
            VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategoryStates::GetGroupType(ViewMode::Standard));
            VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategoryStates::GetGroupType(ViewMode::Scientific));
            VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategoryStates::GetGroupType(ViewMode::Programmer));
            VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategoryStates::GetGroupType(ViewMode::Date));
            VERIFY_ARE_EQUAL(CategoryGroupType::Calculator, NavCategoryStates::GetGroupType(ViewMode::Graphing));

            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Currency));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Volume));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Length));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Weight));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Temperature));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Energy));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Area));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Speed));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Time));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Power));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Data));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Pressure));
            VERIFY_ARE_EQUAL(CategoryGroupType::Converter, NavCategoryStates::GetGroupType(ViewMode::Angle));
        }

        TEST_METHOD(GetIndex)
        {
            // Index is the 0-based ordering of modes
            const auto orderedModesSize = _orderedModes.size();
            for (size_t index = 0; index < orderedModesSize; index++)
            {
                ViewMode mode = _orderedModes[index];
                VERIFY_ARE_EQUAL(index, (size_t)NavCategoryStates::GetIndex(mode));
            }

            VERIFY_ARE_EQUAL(-1, NavCategoryStates::GetIndex(ViewMode::None));
        }

        TEST_METHOD(GetPosition)
        {
            // Position is the 1-based ordering of modes
            const auto orderedModesSize = _orderedModes.size();
            for (size_t pos = 1; pos <= orderedModesSize; pos++)
            {
                ViewMode mode = _orderedModes[pos - 1];
                VERIFY_ARE_EQUAL(pos, (size_t)NavCategoryStates::GetPosition(mode));
            }

            VERIFY_ARE_EQUAL(-1, NavCategoryStates::GetPosition(ViewMode::None));
        }

        TEST_METHOD(GetIndex_GetPosition_Relationship)
        {
            // Index should be 1 less than Position.
            // The other checks verify the order of Index and Position.
            // Just verify the relationship here.
            VERIFY_ARE_EQUAL(NavCategoryStates::GetIndex(ViewMode::Standard) + 1, NavCategoryStates::GetPosition(ViewMode::Standard));
            VERIFY_ARE_EQUAL(NavCategoryStates::GetPosition(ViewMode::Volume) - 1, NavCategoryStates::GetIndex(ViewMode::Volume));
        }

        TEST_METHOD(GetIndexInGroup)
        {
            VERIFY_ARE_EQUAL(0, NavCategoryStates::GetIndexInGroup(ViewMode::Standard, CategoryGroupType::Calculator));
            VERIFY_ARE_EQUAL(1, NavCategoryStates::GetIndexInGroup(ViewMode::Scientific, CategoryGroupType::Calculator));
            VERIFY_ARE_EQUAL(2, NavCategoryStates::GetIndexInGroup(ViewMode::Graphing, CategoryGroupType::Calculator));
            VERIFY_ARE_EQUAL(3, NavCategoryStates::GetIndexInGroup(ViewMode::Programmer, CategoryGroupType::Calculator));
            VERIFY_ARE_EQUAL(4, NavCategoryStates::GetIndexInGroup(ViewMode::Date, CategoryGroupType::Calculator));
            VERIFY_ARE_EQUAL(0, NavCategoryStates::GetIndexInGroup(ViewMode::Currency, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(1, NavCategoryStates::GetIndexInGroup(ViewMode::Volume, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(2, NavCategoryStates::GetIndexInGroup(ViewMode::Length, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(3, NavCategoryStates::GetIndexInGroup(ViewMode::Weight, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(4, NavCategoryStates::GetIndexInGroup(ViewMode::Temperature, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(5, NavCategoryStates::GetIndexInGroup(ViewMode::Energy, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(6, NavCategoryStates::GetIndexInGroup(ViewMode::Area, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(7, NavCategoryStates::GetIndexInGroup(ViewMode::Speed, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(8, NavCategoryStates::GetIndexInGroup(ViewMode::Time, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(9, NavCategoryStates::GetIndexInGroup(ViewMode::Power, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(10, NavCategoryStates::GetIndexInGroup(ViewMode::Data, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(11, NavCategoryStates::GetIndexInGroup(ViewMode::Pressure, CategoryGroupType::Converter));
            VERIFY_ARE_EQUAL(12, NavCategoryStates::GetIndexInGroup(ViewMode::Angle, CategoryGroupType::Converter));

            VERIFY_ARE_EQUAL(-1, NavCategoryStates::GetIndexInGroup(ViewMode::None, CategoryGroupType::Calculator));
            VERIFY_ARE_EQUAL(-1, NavCategoryStates::GetIndexInGroup(ViewMode::None, CategoryGroupType::Converter));
        }

        TEST_METHOD(GetViewModeForVirtualKey)
        {
            VERIFY_ARE_EQUAL(ViewMode::Standard, NavCategoryStates::GetViewModeForVirtualKey(MyVirtualKey::Number1));
            VERIFY_ARE_EQUAL(ViewMode::Scientific, NavCategoryStates::GetViewModeForVirtualKey(MyVirtualKey::Number2));
            VERIFY_ARE_EQUAL(ViewMode::Graphing, NavCategoryStates::GetViewModeForVirtualKey(MyVirtualKey::Number3));
            VERIFY_ARE_EQUAL(ViewMode::Programmer, NavCategoryStates::GetViewModeForVirtualKey(MyVirtualKey::Number4));
            VERIFY_ARE_EQUAL(ViewMode::Date, NavCategoryStates::GetViewModeForVirtualKey(MyVirtualKey::Number5));
        }

        TEST_METHOD(IsViewModeEnabled_ShouldBeTrue_ExceptGraphingMode)
        {
            for (const auto& mode : _orderedModes)
            {
                if (mode != ViewMode::Graphing)
                {
                    VERIFY_IS_TRUE(NavCategoryStates::IsViewModeEnabled(mode));
                }
            }
        }

        TEST_METHOD(GraphingModeIsEnabled_ShouldBeTrue_WhenNullUserAssigned)
        {
            NavCategoryStates::SetCurrentUser("null-user"); // make sure User::GetFromId() returns nullptr
            VERIFY_IS_TRUE(NavCategoryStates::IsViewModeEnabled(ViewMode::Graphing));
        }

    private:
        const static inline std::vector<ViewMode> _orderedModes {
            ViewMode::Standard, ViewMode::Scientific, ViewMode::Graphing, ViewMode::Programmer,  ViewMode::Date,     ViewMode::Currency,
            ViewMode::Volume,   ViewMode::Length,     ViewMode::Weight,   ViewMode::Temperature, ViewMode::Energy,   ViewMode::Area,
            ViewMode::Speed,    ViewMode::Time,       ViewMode::Power,    ViewMode::Data,        ViewMode::Pressure, ViewMode::Angle };
    };
}
