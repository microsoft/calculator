// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NavCategory.h"
#include "AppResourceProvider.h"
#include "Common/LocalizationStringUtil.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::Foundation::Collections;

namespace UCM = UnitConversionManager;

// Calculator categories always support negative and positive.
static constexpr bool SUPPORTS_ALL = true;

// Converter categories usually only support positive.
static constexpr bool SUPPORTS_NEGATIVE = true;
static constexpr bool POSITIVE_ONLY = false;

// The order of items in this list determines the order of groups in the menu.
static constexpr array<const NavCategoryGroupInitializer, 2> s_categoryGroupManifest = {
    NavCategoryGroupInitializer { CategoryGroupType::Calculator, L"CalculatorModeTextCaps", L"CalculatorModeText", L"CalculatorModePluralText"},
    NavCategoryGroupInitializer { CategoryGroupType::Converter,  L"ConverterModeTextCaps",  L"ConverterModeText", L"ConverterModePluralText" }
};

// vvv THESE CONSTANTS SHOULD NEVER CHANGE vvv
static constexpr int STANDARD_ID    = 0;
static constexpr int SCIENTIFIC_ID  = 1;
static constexpr int PROGRAMMER_ID  = 2;
static constexpr int DATE_ID        = 3;
static constexpr int VOLUME_ID      = 4;
static constexpr int LENGTH_ID      = 5;
static constexpr int WEIGHT_ID      = 6;
static constexpr int TEMPERATURE_ID = 7;
static constexpr int ENERGY_ID      = 8;
static constexpr int AREA_ID        = 9;
static constexpr int SPEED_ID       = 10;
static constexpr int TIME_ID        = 11;
static constexpr int POWER_ID       = 12;
static constexpr int DATA_ID        = 13;
static constexpr int PRESSURE_ID    = 14;
static constexpr int ANGLE_ID       = 15;
static constexpr int CURRENCY_ID    = 16;
// ^^^ THESE CONSTANTS SHOULD NEVER CHANGE ^^^

// The order of items in this list determines the order of items in the menu.
static constexpr array<const NavCategoryInitializer, 17> s_categoryManifest = {
    NavCategoryInitializer { ViewMode::Standard,    STANDARD_ID,    L"Standard",        L"StandardMode",             L"\uE8EF", CategoryGroupType::Calculator, MyVirtualKey::Number1, SUPPORTS_ALL      },
    NavCategoryInitializer { ViewMode::Scientific,  SCIENTIFIC_ID,  L"Scientific",      L"ScientificMode",           L"\uF196", CategoryGroupType::Calculator, MyVirtualKey::Number2, SUPPORTS_ALL      },
    NavCategoryInitializer { ViewMode::Programmer,  PROGRAMMER_ID,  L"Programmer",      L"ProgrammerMode",           L"\uECCE", CategoryGroupType::Calculator, MyVirtualKey::Number3, SUPPORTS_ALL      },
    NavCategoryInitializer { ViewMode::Date,        DATE_ID,        L"Date",            L"DateCalculationMode",      L"\uE787", CategoryGroupType::Calculator, MyVirtualKey::Number4, SUPPORTS_ALL      },
    NavCategoryInitializer { ViewMode::Currency,    CURRENCY_ID,    L"Currency",        L"CategoryName_Currency",    L"\uEB0D", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Volume,      VOLUME_ID,      L"Volume",          L"CategoryName_Volume",      L"\uF1AA", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Length,      LENGTH_ID,      L"Length",          L"CategoryName_Length",      L"\uECC6", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Weight,      WEIGHT_ID,      L"Weight and Mass", L"CategoryName_Weight",      L"\uF4C1", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Temperature, TEMPERATURE_ID, L"Temperature",     L"CategoryName_Temperature", L"\uE7A3", CategoryGroupType::Converter,  MyVirtualKey::None,    SUPPORTS_NEGATIVE },
    NavCategoryInitializer { ViewMode::Energy,      ENERGY_ID,      L"Energy",          L"CategoryName_Energy",      L"\uECAD", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Area,        AREA_ID,        L"Area",            L"CategoryName_Area",        L"\uE809", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Speed,       SPEED_ID,       L"Speed",           L"CategoryName_Speed",       L"\uEADA", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Time,        TIME_ID,        L"Time",            L"CategoryName_Time",        L"\uE917", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Power,       POWER_ID,       L"Power",           L"CategoryName_Power",       L"\uE945", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Data,        DATA_ID,        L"Data",            L"CategoryName_Data",        L"\uF20F", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Pressure,    PRESSURE_ID,    L"Pressure",        L"CategoryName_Pressure",    L"\uEC4A", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     },
    NavCategoryInitializer { ViewMode::Angle,       ANGLE_ID,       L"Angle",           L"CategoryName_Angle",       L"\uF515", CategoryGroupType::Converter,  MyVirtualKey::None,    POSITIVE_ONLY     }
};

// This function should only be used when storing the mode to app data.
int NavCategory::Serialize(ViewMode mode)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode](const NavCategoryInitializer& initializer)
    {
        return initializer.viewMode == mode;
    });

    return (iter != s_categoryManifest.end())
        ? iter->serializationId
        : -1;
}

// This function should only be used when restoring the mode from app data.
ViewMode NavCategory::Deserialize(Platform::Object^ obj)
{
    // If we cast directly to ViewMode we will fail
    // because we technically store an int.
    // Need to cast to int, then ViewMode.
    auto boxed = dynamic_cast<Box<int>^>(obj);
    if (boxed != nullptr)
    {
        int serializationId = boxed->Value;
        auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
            [serializationId](const NavCategoryInitializer& initializer)
        {
            return initializer.serializationId == serializationId;
        });

        if (iter != s_categoryManifest.end())
        {
            return iter->viewMode;
        }
    }

    return ViewMode::None;
}

bool NavCategory::IsValidViewMode(ViewMode mode)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode](const NavCategoryInitializer& initializer)
    {
        return initializer.viewMode == mode;
    });

    return iter != s_categoryManifest.end();
}

bool NavCategory::IsCalculatorViewMode(ViewMode mode)
{
    // Historically, Date Calculator is not a Calculator mode
    // even though it is in the Calculator category.
    return !IsDateCalculatorViewMode(mode) && IsModeInCategoryGroup(mode, CategoryGroupType::Calculator);
}

bool NavCategory::IsDateCalculatorViewMode(ViewMode mode)
{
    return mode == ViewMode::Date;
}

bool NavCategory::IsConverterViewMode(ViewMode mode)
{
    return IsModeInCategoryGroup(mode, CategoryGroupType::Converter);
}

bool NavCategory::IsModeInCategoryGroup(ViewMode mode, CategoryGroupType type)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode, type](const NavCategoryInitializer& initializer)
    {
        return initializer.viewMode == mode && initializer.groupType == type;
    });

    return iter != s_categoryManifest.end();
}

String^ NavCategory::GetFriendlyName(ViewMode mode)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode](const NavCategoryInitializer& initializer)
    {
        return initializer.viewMode == mode;
    });

    return (iter != s_categoryManifest.end())
        ? StringReference(iter->friendlyName)
        : L"None";
}

ViewMode NavCategory::GetViewModeForFriendlyName(String^ name)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [name](const NavCategoryInitializer& initializer)
    {
        return wcscmp(initializer.friendlyName, name->Data()) == 0;
    });

    return (iter != s_categoryManifest.end())
        ? iter->viewMode
        : ViewMode::None;
}

String^ NavCategory::GetNameResourceKey(ViewMode mode)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode](const NavCategoryInitializer& initializer)
    {
        return initializer.viewMode == mode;
    });

    return (iter != s_categoryManifest.end())
        ? StringReference(iter->nameResourceKey) + "Text"
        : nullptr;
}

CategoryGroupType NavCategory::GetGroupType(ViewMode mode)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode](const NavCategoryInitializer& initializer)
    {
        return initializer.viewMode == mode;
    });

    return (iter != s_categoryManifest.end())
        ? iter->groupType
        : CategoryGroupType::None;
}

// GetIndex is 0-based, GetPosition is 1-based
int NavCategory::GetIndex(ViewMode mode)
{
    int position = NavCategory::GetPosition(mode);
    return max(-1, position - 1);
}

int NavCategory::GetFlatIndex(ViewMode mode)
{
    int index = -1;
    CategoryGroupType type = CategoryGroupType::None;
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode, &type, &index](const NavCategoryInitializer& initializer)
    {
        index++;
        if (initializer.groupType != type)
        {
            type = initializer.groupType;
            index++;
        }

        return initializer.viewMode == mode;
    });

    return (iter != s_categoryManifest.end())
        ? index
        : -1;
}

// GetIndex is 0-based, GetPosition is 1-based
int NavCategory::GetIndexInGroup(ViewMode mode, CategoryGroupType type)
{
    int index = -1;
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode, type, &index](const NavCategoryInitializer& initializer)
    {
        if (initializer.groupType == type)
        {
            index++;
            return initializer.viewMode == mode;
        }

        return false;
    });

    return (iter != s_categoryManifest.end())
        ? index
        : -1;
}

// GetIndex is 0-based, GetPosition is 1-based
int NavCategory::GetPosition(ViewMode mode)
{
    int position = 0;
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [mode, &position](const NavCategoryInitializer& initializer)
    {
        position++;
        return initializer.viewMode == mode;
    });

    return (iter != s_categoryManifest.end())
        ? position
        : -1;
}

ViewMode NavCategory::GetViewModeForVirtualKey(MyVirtualKey virtualKey)
{
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest),
        [virtualKey](const NavCategoryInitializer& initializer)
    {
        return initializer.virtualKey == virtualKey;
    });

    return (iter != s_categoryManifest.end())
        ? iter->viewMode
        : ViewMode::None;
}

vector<MyVirtualKey> NavCategory::GetCategoryAcceleratorKeys()
{
    vector<MyVirtualKey> accelerators{};
    for (auto category : s_categoryManifest)
    {
        if (category.virtualKey != MyVirtualKey::None)
        {
            accelerators.push_back(category.virtualKey);
        }
    }

    return accelerators;
}

NavCategoryGroup::NavCategoryGroup(const NavCategoryGroupInitializer& groupInitializer) :
    m_Categories(ref new Vector<NavCategory^>())
{
    m_GroupType = groupInitializer.type;

    auto resProvider = AppResourceProvider::GetInstance();
    String^ headerResourceKey = StringReference(groupInitializer.headerResourceKey);
    String^ modeResourceKey = StringReference(groupInitializer.modeResourceKey);
    String^ automationResourceKey = StringReference(groupInitializer.automationResourceKey);
    m_Name = resProvider.GetResourceString(headerResourceKey);
    String^ groupMode = resProvider.GetResourceString(modeResourceKey);
    String^ automationName = resProvider.GetResourceString(automationResourceKey);

    String^ navCategoryHeaderAutomationNameFormat = resProvider.GetResourceString(L"NavCategoryHeader_AutomationNameFormat");
    m_AutomationName = ref new String(LocalizationStringUtil::GetLocalizedString(
        navCategoryHeaderAutomationNameFormat->Data(),
        automationName->Data()).c_str());

    String^ navCategoryItemAutomationNameFormat = resProvider.GetResourceString(L"NavCategoryItem_AutomationNameFormat");

    for (const NavCategoryInitializer& categoryInitializer : s_categoryManifest)
    {
        if (categoryInitializer.groupType == groupInitializer.type)
        {
            String^ nameResourceKey = StringReference(categoryInitializer.nameResourceKey);
            String^ categoryName = resProvider.GetResourceString(nameResourceKey + "Text");
            String^ categoryAutomationName = ref new String(LocalizationStringUtil::GetLocalizedString(
                navCategoryItemAutomationNameFormat->Data(),
                categoryName->Data(),
                m_Name->Data()).c_str());

            m_Categories->Append(ref new NavCategory(
                categoryName,
                categoryAutomationName,
                StringReference(categoryInitializer.glyph),
                resProvider.GetResourceString(nameResourceKey + "AccessKey"),
                groupMode,
                categoryInitializer.viewMode,
                categoryInitializer.supportsNegative));
        }
    }
}

IObservableVector<NavCategoryGroup^>^ NavCategoryGroup::CreateMenuOptions()
{
    auto menuOptions = ref new Vector<NavCategoryGroup^>();
    menuOptions->Append(CreateCalculatorCategory());
    menuOptions->Append(CreateConverterCategory());
    return menuOptions;
}

NavCategoryGroup^ NavCategoryGroup::CreateCalculatorCategory()
{
    return ref new NavCategoryGroup(s_categoryGroupManifest.at(0));
}

NavCategoryGroup^ NavCategoryGroup::CreateConverterCategory()
{
    return ref new NavCategoryGroup(s_categoryGroupManifest.at(1));
}

vector<NavCategoryInitializer> NavCategoryGroup::GetInitializerCategoryGroup(CategoryGroupType groupType)
{
    vector<NavCategoryInitializer> initializers{};
    copy_if(begin(s_categoryManifest), end(s_categoryManifest), back_inserter(initializers),
        [groupType](const NavCategoryInitializer& initializer)
    {
        return initializer.groupType == groupType;
    });

    return initializers;
}

String^ NavCategoryGroup::GetHeaderResourceKey(CategoryGroupType type)
{
    auto iter = find_if(begin(s_categoryGroupManifest), end(s_categoryGroupManifest),
        [type](const NavCategoryGroupInitializer& initializer)
    {
        return initializer.type == type;
    });

    return (iter != s_categoryGroupManifest.end())
        ? StringReference(iter->headerResourceKey)
        : nullptr;
}
