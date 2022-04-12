// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NavCategory.h"
#include "AppResourceProvider.h"
#include "Common/LocalizationStringUtil.h"
#include <initializer_list>

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel;
using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Management::Policies;
using namespace Windows::System;

namespace UCM = UnitConversionManager;

// Calculator categories always support negative and positive.
static constexpr bool SUPPORTS_ALL = true;

// Converter categories usually only support positive.
static constexpr bool SUPPORTS_NEGATIVE = true;
static constexpr bool POSITIVE_ONLY = false;

// vvv THESE CONSTANTS SHOULD NEVER CHANGE vvv
static constexpr int STANDARD_ID = 0;
static constexpr int SCIENTIFIC_ID = 1;
static constexpr int PROGRAMMER_ID = 2;
static constexpr int DATE_ID = 3;
static constexpr int VOLUME_ID = 4;
static constexpr int LENGTH_ID = 5;
static constexpr int WEIGHT_ID = 6;
static constexpr int TEMPERATURE_ID = 7;
static constexpr int ENERGY_ID = 8;
static constexpr int AREA_ID = 9;
static constexpr int SPEED_ID = 10;
static constexpr int TIME_ID = 11;
static constexpr int POWER_ID = 12;
static constexpr int DATA_ID = 13;
static constexpr int PRESSURE_ID = 14;
static constexpr int ANGLE_ID = 15;
static constexpr int CURRENCY_ID = 16;
static constexpr int GRAPHING_ID = 17;
// ^^^ THESE CONSTANTS SHOULD NEVER CHANGE ^^^

wchar_t* towchar_t(int number)
{
    auto wstr = std::to_wstring(number);
    return _wcsdup(wstr.c_str());
}

bool IsGraphingModeAvailable()
{
    static bool supportGraph = Windows::Foundation::Metadata::ApiInformation::IsMethodPresent("Windows.UI.Text.RichEditTextDocument", "GetMath");
    return supportGraph;
}

Box<bool> ^ _isGraphingModeEnabledCached = nullptr;
bool IsGraphingModeEnabled(User ^ currentUser = nullptr)
{
    if (!IsGraphingModeAvailable())
    {
        return false;
    }

    if (_isGraphingModeEnabledCached != nullptr)
    {
        return _isGraphingModeEnabledCached->Value;
    }

    if (!currentUser)
    {
        return true;
    }

    auto namedPolicyData = NamedPolicy::GetPolicyFromPathForUser(currentUser, L"Education", L"AllowGraphingCalculator");
    _isGraphingModeEnabledCached = namedPolicyData->GetBoolean() == true;

    return _isGraphingModeEnabledCached->Value;
}


Platform::Agile<Windows::System::User ^> NavCategoryStates::_currentUser;

// The order of items in this list determines the order of items in the menu.
static std::vector<NavCategoryInitializer> s_categoryManifest = [] {
    auto res = std::vector<NavCategoryInitializer>{ NavCategoryInitializer{ ViewMode::Standard,
                                                                     STANDARD_ID,
                                                                     L"Standard",
                                                                     L"StandardMode",
                                                                     L"\uE8EF",
                                                                     CategoryGroupType::Calculator,
                                                                     MyVirtualKey::Number1,
                                                                     L"1",
                                                                     SUPPORTS_ALL,
                                                                     true },
                                             NavCategoryInitializer{ ViewMode::Scientific,
                                                                     SCIENTIFIC_ID,
                                                                     L"Scientific",
                                                                     L"ScientificMode",
                                                                     L"\uF196",
                                                                     CategoryGroupType::Calculator,
                                                                     MyVirtualKey::Number2,
                                                                     L"2",
                                                                     SUPPORTS_ALL,
                                                                     true } };

    int currentIndex = 3;
    bool supportGraphingCalculator = IsGraphingModeAvailable();
    if (supportGraphingCalculator)
    {
        res.emplace_back(NavCategoryInitializer{ ViewMode::Graphing,
                                              GRAPHING_ID,
                                              L"Graphing",
                                              L"GraphingCalculatorMode",
                                              L"\uF770",
                                              CategoryGroupType::Calculator,
                                              MyVirtualKey::Number3,
                                              L"3",
                                              SUPPORTS_ALL,
                                              false});
        ++currentIndex;
    }
    res.insert(
        res.cend(),
        { NavCategoryInitializer{ ViewMode::Programmer,
                                  PROGRAMMER_ID,
                                  L"Programmer",
                                  L"ProgrammerMode",
                                  L"\uECCE",
                                  CategoryGroupType::Calculator,
                                  supportGraphingCalculator ? MyVirtualKey::Number4 : MyVirtualKey::Number3,
                                  towchar_t(currentIndex++),
                                  SUPPORTS_ALL,
                                  true },
          NavCategoryInitializer{ ViewMode::Date,
                                  DATE_ID,
                                  L"Date",
                                  L"DateCalculationMode",
                                  L"\uE787",
                                  CategoryGroupType::Calculator,
                                  supportGraphingCalculator ? MyVirtualKey::Number5 : MyVirtualKey::Number4,
                                  towchar_t(currentIndex++),
                                  SUPPORTS_ALL,
                                  true },
          NavCategoryInitializer{ ViewMode::Currency,
                                  CURRENCY_ID,
                                  L"Currency",
                                  L"CategoryName_Currency",
                                  L"\uEB0D",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Volume,
                                  VOLUME_ID,
                                  L"Volume",
                                  L"CategoryName_Volume",
                                  L"\uF1AA",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Length,
                                  LENGTH_ID,
                                  L"Length",
                                  L"CategoryName_Length",
                                  L"\uECC6",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Weight,
                                  WEIGHT_ID,
                                  L"Weight and Mass",
                                  L"CategoryName_Weight",
                                  L"\uF4C1",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Temperature,
                                  TEMPERATURE_ID,
                                  L"Temperature",
                                  L"CategoryName_Temperature",
                                  L"\uE7A3",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  SUPPORTS_NEGATIVE,
                                  true },
          NavCategoryInitializer{ ViewMode::Energy,
                                  ENERGY_ID,
                                  L"Energy",
                                  L"CategoryName_Energy",
                                  L"\uECAD",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Area,
                                  AREA_ID,
                                  L"Area",
                                  L"CategoryName_Area",
                                  L"\uE809",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Speed,
                                  SPEED_ID,
                                  L"Speed",
                                  L"CategoryName_Speed",
                                  L"\uEADA",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Time,
                                  TIME_ID,
                                  L"Time",
                                  L"CategoryName_Time",
                                  L"\uE917",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Power,
                                  POWER_ID,
                                  L"Power",
                                  L"CategoryName_Power",
                                  L"\uE945",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  SUPPORTS_NEGATIVE,
                                  true },
          NavCategoryInitializer{ ViewMode::Data,
                                  DATA_ID,
                                  L"Data",
                                  L"CategoryName_Data",
                                  L"\uF20F",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Pressure,
                                  PRESSURE_ID,
                                  L"Pressure",
                                  L"CategoryName_Pressure",
                                  L"\uEC4A",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  POSITIVE_ONLY,
                                  true },
          NavCategoryInitializer{ ViewMode::Angle,
                                  ANGLE_ID,
                                  L"Angle",
                                  L"CategoryName_Angle",
                                  L"\uF515",
                                  CategoryGroupType::Converter,
                                  MyVirtualKey::None,
                                  nullptr,
                                  SUPPORTS_NEGATIVE,
                                  true } });
    return res;
}();

void NavCategory::InitializeCategoryManifest(User ^ user)
{
    //int i = 0;
    //for (NavCategoryInitializer category : s_categoryManifest)
    //{
    //    if (category.viewMode == ViewMode::Graphing)
    //    {
    //        auto navCatInit = s_categoryManifest.begin();
    //        std::advance(navCatInit, i);
    //        (*navCatInit).isEnabled = IsGraphingModeEnabled(user);
    //        break;
    //    }
    //    else
    //    {
    //        i++;
    //    }
    //}
}


void NavCategory::UpdateGraphingModelManifest(Windows::System::User ^ user)
{
    static bool isEnabled = IsGraphingModeEnabled(user);
    std::find_if(
        s_categoryManifest.begin(),
        s_categoryManifest.end(),
        [](const auto& category) { return category.viewMode == ViewMode::Graphing;})
        ->isEnabled = isEnabled;
}

bool NavCategory::IsCalculatorViewMode(ViewMode mode)
{
    // Historically, Calculator modes are Standard, Scientific, and Programmer.
    return !IsDateCalculatorViewMode(mode) && !IsGraphingCalculatorViewMode(mode) && IsModeInCategoryGroup(mode, CategoryGroupType::Calculator);
}

bool NavCategory::IsGraphingCalculatorViewMode(ViewMode mode)
{
    return mode == ViewMode::Graphing;
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
    auto iter = find_if(begin(s_categoryManifest), end(s_categoryManifest), [mode, type](const NavCategoryInitializer& initializer) {
        return initializer.viewMode == mode && initializer.groupType == type;
    });

    return iter != s_categoryManifest.end();
}

NavCategoryGroup::NavCategoryGroup(const NavCategoryGroupInitializer& groupInitializer)
    : m_Categories(ref new Vector<NavCategory ^>())
{
    m_GroupType = groupInitializer.type;

    auto resProvider = AppResourceProvider::GetInstance();
    m_Name = resProvider->GetResourceString(StringReference(groupInitializer.headerResourceKey));
    String ^ groupMode = resProvider->GetResourceString(StringReference(groupInitializer.modeResourceKey));
    String ^ automationName = resProvider->GetResourceString(StringReference(groupInitializer.automationResourceKey));

    String ^ navCategoryHeaderAutomationNameFormat = resProvider->GetResourceString(L"NavCategoryHeader_AutomationNameFormat");
    m_AutomationName = LocalizationStringUtil::GetLocalizedString(navCategoryHeaderAutomationNameFormat, automationName);

    String ^ navCategoryItemAutomationNameFormat = resProvider->GetResourceString(L"NavCategoryItem_AutomationNameFormat");

    for (const NavCategoryInitializer& categoryInitializer : s_categoryManifest)
    {
        if (categoryInitializer.groupType == groupInitializer.type)
        {
            String ^ nameResourceKey = StringReference(categoryInitializer.nameResourceKey);
            String ^ categoryName = resProvider->GetResourceString(nameResourceKey + "Text");
            String ^ categoryAutomationName = LocalizationStringUtil::GetLocalizedString(navCategoryItemAutomationNameFormat, categoryName, m_Name);

            m_Categories->Append(ref new NavCategory(
                categoryName,
                categoryAutomationName,
                StringReference(categoryInitializer.glyph),
                categoryInitializer.accessKey != nullptr ? ref new String(categoryInitializer.accessKey)
                                                         : resProvider->GetResourceString(nameResourceKey + "AccessKey"),
                groupMode,
                categoryInitializer.viewMode,
                categoryInitializer.supportsNegative,
                categoryInitializer.isEnabled));
        }
    }
}

void NavCategoryStates::SetCurrentUser(Windows::System::User^ user)
{
    _currentUser = user;
}

IObservableVector<NavCategoryGroup ^> ^ NavCategoryStates::CreateMenuOptions()
{
    auto menuOptions = ref new Vector<NavCategoryGroup ^>();
    menuOptions->Append(CreateCalculatorCategoryGroup());
    menuOptions->Append(CreateConverterCategoryGroup());
    return menuOptions;
}

NavCategoryGroup ^ NavCategoryStates::CreateCalculatorCategoryGroup()
{
    return ref new NavCategoryGroup(
        NavCategoryGroupInitializer{ CategoryGroupType::Calculator, L"CalculatorModeTextCaps", L"CalculatorModeText", L"CalculatorModePluralText" });
}

NavCategoryGroup ^ NavCategoryStates::CreateConverterCategoryGroup()
{
    return ref new NavCategoryGroup(
        NavCategoryGroupInitializer{ CategoryGroupType::Converter, L"ConverterModeTextCaps", L"ConverterModeText", L"ConverterModePluralText" });
}

// This function should only be used when storing the mode to app data.
int NavCategoryStates::Serialize(ViewMode mode)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode](const auto& initializer) { return initializer.viewMode == mode; });

    return (citer != s_categoryManifest.cend()) ? citer->serializationId : -1;
}

// This function should only be used when restoring the mode from app data.
ViewMode NavCategoryStates::Deserialize(Platform::Object ^ obj)
{
    // If we cast directly to ViewMode we will fail
    // because we technically store an int.
    // Need to cast to int, then ViewMode.
    auto boxed = dynamic_cast<Box<int> ^>(obj);
    if (boxed != nullptr)
    {
        int serializationId = boxed->Value;
        const auto& citer = find_if(
            cbegin(s_categoryManifest),
            cend(s_categoryManifest),
            [serializationId](const auto& initializer) { return initializer.serializationId == serializationId; });

        return citer != s_categoryManifest.cend() ?
                   (citer->viewMode == ViewMode::Graphing ?
                       (IsGraphingModeEnabled() ? citer->viewMode : ViewMode::None)
                       : citer->viewMode)
                   : ViewMode::None;
    }
    else
    {
        return ViewMode::None;
    }
}

ViewMode NavCategoryStates::GetViewModeForFriendlyName(String ^ name)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [name](const auto& initializer) { return wcscmp(initializer.friendlyName, name->Data()) == 0; });

    return (citer != s_categoryManifest.cend()) ? citer->viewMode : ViewMode::None;
}

String ^ NavCategoryStates::GetFriendlyName(ViewMode mode)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode](const auto& initializer) { return initializer.viewMode == mode; });

    return (citer != s_categoryManifest.cend()) ? StringReference(citer->friendlyName) : L"None";
}

String ^ NavCategoryStates::GetNameResourceKey(ViewMode mode)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode](const auto& initializer) { return initializer.viewMode == mode; });

    return (citer != s_categoryManifest.cend()) ? StringReference(citer->nameResourceKey) + "Text" : nullptr;
}

CategoryGroupType NavCategoryStates::GetGroupType(ViewMode mode)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode](const auto& initializer) { return initializer.viewMode == mode; });

    return (citer != s_categoryManifest.cend()) ? citer->groupType : CategoryGroupType::None;
}

// GetIndex is 0-based, GetPosition is 1-based
int NavCategoryStates::GetIndex(ViewMode mode)
{
    int position = GetPosition(mode);
    return std::max(-1, position - 1);
}

int NavCategoryStates::GetFlatIndex(ViewMode mode)
{
    int index = -1;
    CategoryGroupType type = CategoryGroupType::None;
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode, &type, &index](const auto& initializer) {
            ++index;
            if (initializer.groupType != type)
            {
                type = initializer.groupType;
                ++index;
            }
            return initializer.viewMode == mode;
        });

    return (citer != s_categoryManifest.cend()) ? index : -1;
}

// GetIndex is 0-based, GetPosition is 1-based
int NavCategoryStates::GetIndexInGroup(ViewMode mode, CategoryGroupType type)
{
    int index = -1;
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode, type, &index](const auto& initializer) {
            if (initializer.groupType == type)
            {
                ++index;
                return initializer.viewMode == mode;
            }
            return false;
        });

    return (citer != s_categoryManifest.cend()) ? index : -1;
}

// GetIndex is 0-based, GetPosition is 1-based
int NavCategoryStates::GetPosition(ViewMode mode)
{
    int position = 0;
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode, &position](const auto& initializer) {
            ++position;
            return initializer.viewMode == mode;
        });

    return (citer != s_categoryManifest.cend()) ? position : -1;
}

ViewMode NavCategoryStates::GetViewModeForVirtualKey(MyVirtualKey virtualKey)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [virtualKey](const auto& initializer) { return initializer.virtualKey == virtualKey; });

    return (citer != s_categoryManifest.end()) ? citer->viewMode : ViewMode::None;
}

void NavCategoryStates::GetCategoryAcceleratorKeys(IVector<MyVirtualKey> ^ accelerators)
{
    if (accelerators != nullptr)
    {
        accelerators->Clear();
        for (const auto& category : s_categoryManifest)
        {
            if (category.virtualKey != MyVirtualKey::None)
            {
                accelerators->Append(category.virtualKey);
            }
        }
    }
}

bool NavCategoryStates::IsValidViewMode(ViewMode mode)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode](const auto& initializer) { return initializer.viewMode == mode; });

    return citer != s_categoryManifest.cend();
}

bool NavCategoryStates::IsViewModeEnabled(ViewMode mode)
{
    const auto& citer = find_if(
        cbegin(s_categoryManifest),
        cend(s_categoryManifest),
        [mode](const auto& initializer) { return initializer.viewMode == mode && initializer.isEnabled; });

    return citer != s_categoryManifest.cend();
}

