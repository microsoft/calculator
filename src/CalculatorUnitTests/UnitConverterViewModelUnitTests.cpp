// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>
#include "UnitConverterViewModelUnitTests.h"

#include "CalcViewModel/UnitConverterViewModel.h"
#include "CalcViewModel/DataLoaders/UnitConverterDataLoader.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Utils;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;

namespace UCM = UnitConversionManager;
namespace VM = CalculatorApp::ViewModel;
using namespace Windows::Globalization;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    TEST_CLASS(CategoryViewModelTests)
    {
    public:
        // Test all binding values:

        TEST_METHOD(TestGetNameReturnsCorrectName)
        {
            wstring name = L"TestName";
    UCM::Category cat;
    cat.name = name;
    VM::Category ^ vmcat = ref new VM::Category(cat);
    VERIFY_ARE_EQUAL(vmcat->Name->Data(), name);
}

TEST_METHOD(TestGetVisibilityReturnsVisible)
{
    UCM::Category cat;
    cat.supportsNegative = true;
    VM::Category ^ vmcat = ref new VM::Category(cat);
    VERIFY_IS_TRUE(Visibility::Visible == vmcat->NegateVisibility);
}

TEST_METHOD(TestGetVisibilityReturnsCollapsed)
{
    UCM::Category cat;
    cat.supportsNegative = false;
    VM::Category ^ vmcat = ref new VM::Category(cat);
    VERIFY_IS_TRUE(Visibility::Collapsed == vmcat->NegateVisibility);
}
}
;

TEST_CLASS(UnitViewModelTests){ public:
                                    // Test all binding values:

                                    TEST_METHOD(TestGetNameReturnsCorrectName){ wstring name = L"TestName";
UCM::Unit unit;
unit.name = name;
VM::Unit ^ vmunit = ref new VM::Unit(unit);
VERIFY_ARE_EQUAL(vmunit->Name->Data(), name);
}

TEST_METHOD(TestGetAbbreviationReturnsCorrectAbbreviation)
{
    wstring abbr = L"TestName";
    UCM::Unit unit;
    unit.abbreviation = abbr;
    VM::Unit ^ vmunit = ref new VM::Unit(unit);
    VERIFY_ARE_EQUAL(vmunit->Abbreviation->Data(), abbr);
}
}
;

TEST_CLASS(SupplementaryResultsViewModelTests){ public:
                                                    // Test all binding values:

                                                    TEST_METHOD(TestGetValueReturnsCorrectValue){ wstring value = L"TestName";
UCM::Unit unit = { 1, L"", L"", false, false, false };
VM::Unit ^ vmunit = ref new VM::Unit(unit);
VM::SupplementaryResult ^ vmsupp = ref new VM::SupplementaryResult(ref new String(value.c_str()), vmunit);
VERIFY_ARE_EQUAL(vmsupp->Value->Data(), value);
}

TEST_METHOD(TestGetUnitNameReturnsCorrectValue)
{
    wstring name = L"TestName";
    UCM::Unit unit = { 1, name, L"", false, false, false };
    unit.name = name;
    VM::Unit ^ vmunit = ref new VM::Unit(unit);
    VM::SupplementaryResult ^ vmsupp = ref new VM::SupplementaryResult(L"", vmunit);
    VERIFY_ARE_EQUAL(vmsupp->Unit->Name->Data(), name);
}

TEST_METHOD(TestGetIsWhimsicalReturnsCorrectValue)
{
    UCM::Unit unit = { 1, L"", L"", false, false, false };
    UCM::Unit unitW = { 2, L"", L"", false, false, true };
    VM::Unit ^ vmUnit = ref new VM::Unit(unit);
    VM::Unit ^ vmUnitW = ref new VM::Unit(unitW);
    VM::SupplementaryResult ^ vmsupp = ref new VM::SupplementaryResult(L"", vmUnit);
    VM::SupplementaryResult ^ vmsuppW = ref new VM::SupplementaryResult(L"", vmUnitW);
    VERIFY_IS_FALSE(vmsupp->IsWhimsical());
    VERIFY_IS_TRUE(vmsuppW->IsWhimsical());
}
}
;

UnitConverterMock::UnitConverterMock()
    : m_initCallCount(0)
    , m_getCategoriesCallCount(0)
    , m_setCurrentCategoryCallCount(0)
    , m_setCurUnitTypesCallCount(0)
    , m_switchActiveCallCount(0)
    , m_sendCommandCallCount(0)
    , m_setVMCallbackCallCount(0)
{
}

// IUnitConverter

void UnitConverterMock::Initialize()
{
    m_initCallCount++;
}

vector<UCM::Category> UnitConverterMock::GetCategories()
{
    m_getCategoriesCallCount++;

    vector<UCM::Category> cats;
    cats.push_back(CAT1);
    cats.push_back(CAT2);
    cats.push_back(CAT3);
    cats.push_back(CAT_CURRENCY);

    m_curCategory = CAT2;

    return cats;
}

UCM::CategorySelectionInitializer UnitConverterMock::SetCurrentCategory(const UCM::Category& input)
{
    m_setCurrentCategoryCallCount++;
    m_curCategory = input;
    vector<UCM::Unit> units;
    switch (input.id)
    {
    case 1:
    {
        units.push_back(UNIT1);
        units.push_back(UNIT2);
        units.push_back(UNIT3);
        break;
    }
    case 2:
    {
        units.push_back(UNIT4);
        units.push_back(UNIT5);
        units.push_back(UNIT6);
        break;
    }
    case 3:
    {
        units.push_back(UNIT7);
        units.push_back(UNIT8);
        units.push_back(UNIT9);
        break;
    }
    case CURRENCY_ID:
        units.push_back(UNITJPY);
        units.push_back(UNITJOD);
        break;
    default:
        throw;
    }

    for (const UCM::Unit& unit : units)
    {
        if (unit.isConversionSource)
        {
            m_curFrom = unit;
        }

        if (unit.isConversionTarget)
        {
            m_curTo = unit;
        }
    }

    units.push_back(UNITWHIMSY); // needs to be filtered out

    return make_tuple(units, m_curFrom, m_curTo);
}

UCM::Category UnitConverterMock::GetCurrentCategory()
{
    return m_curCategory;
}

void UnitConverterMock::SetCurrentUnitTypes(const UCM::Unit& fromType, const UCM::Unit& toType)
{
    m_setCurUnitTypesCallCount++;
    m_curFrom = fromType;
    m_curTo = toType;
    m_vmCallback->SuggestedValueCallback(m_suggestedList);
}

void UnitConverterMock::SwitchActive(const std::wstring& newValue)
{
    m_switchActiveCallCount++;
    m_curValue = newValue;
}

bool UnitConverterMock::IsSwitchedActive() const
{
    return false;
}

std::wstring UnitConverterMock::SaveUserPreferences()
{
    return L"TEST";
};

void UnitConverterMock::RestoreUserPreferences(_In_ std::wstring_view /*userPreferences*/){};

void UnitConverterMock::SendCommand(UCM::Command command)
{
    m_sendCommandCallCount++;
    m_lastCommand = command;

    m_vmCallback->SuggestedValueCallback(m_suggestedList);
}

void UnitConverterMock::SetViewModelCallback(const shared_ptr<UCM::IUnitConverterVMCallback>& newCallback)
{
    m_setVMCallbackCallCount++;
    m_vmCallback = newCallback;
}

ref class MockActivatable sealed : public VM::IActivatable
{
private:
    bool m_active;

public:
    MockActivatable(bool active)
        : m_active(active)
    {
    }

    virtual property bool IsActive
    {
        bool get()
        {
            return m_active;
        }
        void set(bool value)
        {
            m_active = value;
        }
    }
};

TEST_CLASS(UnitConverterDataLoaderTests){ public: TEST_METHOD(FuncUnitConverterAllUnitCombinations){
    VM::UnitConverterViewModel ^ vm = ref new UnitConverterViewModel(
        make_shared<UnitConversionManager::UnitConverter>(make_shared<UnitConverterDataLoader>(ref new GeographicRegion()), nullptr));
IObservableVector<Category ^> ^ categoryList = vm->Categories;
ResourceLoader ^ m_resLoader = ResourceLoader::GetForViewIndependentUse("Test");
double epsilon = 0.1; // Could be more precise like 0.001 except atm to pascal conversion

for (unsigned int k = 0; k < categoryList->Size; k++)
{
    vm->CurrentCategory = categoryList->GetAt(k);
    IObservableVector<Unit ^> ^ unitList = vm->Units;
    for (unsigned int i = 0; i < unitList->Size; i++)
    {
        vm->Unit1 = unitList->GetAt(i);
        wstring unit1Name = vm->Unit1->Name->Data();
        for (unsigned int j = 0; j < unitList->Size; j++)
        {
            vm->Value2Active = true;
            vm->Value1Active = false;
            vm->Unit2 = unitList->GetAt(j);
            wstring unit2Name = vm->Unit2->Name->Data();
            // Change value2 as 1.
            vm->ButtonPressed->Execute(NumbersAndOperatorsEnum::One);
            String ^ expectedResult = m_resLoader->GetString(ref new String((unit1Name + L"-" + unit2Name).c_str()));

            // if the corresponding conversion ratio is present in Test.resw file
            if (expectedResult != nullptr)
            {
                wstring expResult = expectedResult->Data();

                double expectedConversion = GetDoubleFromWstring(expResult);
                double actualConversion = GetDoubleFromWstring(vm->Value1->Data());
                double diff = abs(expectedConversion - actualConversion);

                // Assert for diff less than epsilonth fraction of expected conversion result
                VERIFY_IS_LESS_THAN_OR_EQUAL(diff, epsilon * expectedConversion);
            }
            // Clearing the value1
            vm->ButtonPressed->Execute(NumbersAndOperatorsEnum::Clear);
        }
    }
}
}
}
;

TEST_CLASS(UnitConverterViewModelTests){
    public:
        // Test that the ctor makes value1 active and value2 passive
        TEST_METHOD(TestUnitConverterCtorSetsUpCorrectActiveValue){ shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
VM::UnitConverterViewModel vm(mock);
VERIFY_IS_TRUE(vm.Value1Active);
VERIFY_IS_FALSE(vm.Value2Active);
}

// Test that we've created all the vectors on init.
TEST_METHOD(TestUnitConverterCtorSetsUpVectors)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    VERIFY_IS_NOT_NULL(vm.Categories);
    VERIFY_IS_NOT_NULL(vm.Units);
    VERIFY_IS_NOT_NULL(vm.SupplementaryResults);
}

// Test that we've set up all the display callbacks on init.
TEST_METHOD(TestUnitConverterLoadSetsUpCallbacks)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_setVMCallbackCallCount);
}

// Test that we've set up all categories on load and that the first
// category is selected.
TEST_METHOD(TestUnitConverterLoadSetsUpCategories)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    IObservableVector<VM::Category ^> ^ cats = vm.Categories;
    VERIFY_ARE_EQUAL((UINT)1, mock->m_getCategoriesCallCount);
    VERIFY_ARE_EQUAL((UINT)4, cats->Size);
    // Verify that we match current category
    VERIFY_IS_TRUE(CAT2 == vm.CurrentCategory->GetModelCategory());
}

// Test that we've set up all units on load and that the default
// units are selected.
TEST_METHOD(TestUnitConverterLoadSetsUpUnits)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();

    VM::UnitConverterViewModel vm(mock);
    IObservableVector<VM::Unit ^> ^ units = vm.Units;
    VERIFY_ARE_EQUAL((UINT)1, mock->m_setCurrentCategoryCallCount);
    VERIFY_ARE_EQUAL((UINT)3, units->Size);
    VERIFY_IS_TRUE(UNIT4 == vm.Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT6 == vm.Unit2->GetModelUnit());
    VERIFY_ARE_EQUAL((UINT)1, mock->m_setCurUnitTypesCallCount);
    VERIFY_IS_TRUE(mock->m_curFrom == UNIT4);
    VERIFY_IS_TRUE(mock->m_curTo == UNIT6);
}

// Test that changing units updates the current unit types
// in the model
TEST_METHOD(TestUnitSelectionChangeUpdatesModel)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    vm.Unit1 = vm.Units->GetAt(1); // Change from u4 to u5
                                   // Count will be 2 here since it was already called once at init
    VERIFY_ARE_EQUAL((UINT)2, mock->m_setCurUnitTypesCallCount);
    VERIFY_IS_TRUE(UNIT5 == mock->m_curFrom);
    VERIFY_IS_TRUE(UNIT6 == mock->m_curTo);
    vm.Unit2 = vm.Units->GetAt(0); // Change from u3 to u1
    VERIFY_ARE_EQUAL((UINT)3, mock->m_setCurUnitTypesCallCount);
    VERIFY_IS_TRUE(UNIT5 == mock->m_curFrom);
    VERIFY_IS_TRUE(UNIT4 == mock->m_curTo);
}

// Test that changing categories updates the unit list
TEST_METHOD(TestCategorySelectionChangeUpdatesUnits)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    vm.CurrentCategory = vm.Categories->GetAt(2); // Change from cat1 to cat3
                                                  // Counts will be 2 here since the first call should have happened during init
    VERIFY_IS_GREATER_THAN_OR_EQUAL(2u, mock->m_setCurrentCategoryCallCount);
    VERIFY_ARE_EQUAL((UINT)3, vm.Units->Size);
    VERIFY_IS_TRUE(UNIT7 == vm.Units->GetAt(0)->GetModelUnit());
    VERIFY_IS_TRUE(UNIT8 == vm.Units->GetAt(1)->GetModelUnit());
    VERIFY_IS_TRUE(UNIT9 == vm.Units->GetAt(2)->GetModelUnit());
}

// Test that changing categories updates the current unit types
// in the model
TEST_METHOD(TestCategorySelectionChangeUpdatesModel)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    vm.CurrentCategory = vm.Categories->GetAt(2); // Change from cat1 to cat3
                                                  // Counts will be 2 here since the first call should have happened during init
    VERIFY_IS_GREATER_THAN_OR_EQUAL(2u, mock->m_setCurrentCategoryCallCount);
    VERIFY_IS_TRUE(UNIT9 == vm.Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT7 == vm.Unit2->GetModelUnit());
    VERIFY_IS_GREATER_THAN_OR_EQUAL(2u, mock->m_setCurUnitTypesCallCount);
    VERIFY_IS_TRUE(UNIT9 == mock->m_curFrom);
    VERIFY_IS_TRUE(UNIT7 == mock->m_curTo);
}

// Test that the displaycallback updates the display values
TEST_METHOD(TestDisplayCallbackUpdatesDisplayValues)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    vm.UpdateDisplay(L"1234", L"56.78");
    VERIFY_IS_TRUE(vm.Value1 == L"1,234");
    VERIFY_IS_TRUE(vm.Value2 == L"56.78");
}

// Test that the calculator button command correctly fires
// commands to the model.
TEST_METHOD(TestButtonCommandFiresModelCommands)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    // Call count is being set to 1 because we send 'CE' command as the first call
    UINT callCount = 1;

    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Zero);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Zero == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::One);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::One == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Two);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Two == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Three);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Three == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Four);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Four == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Five);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Five == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Six);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Six == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Seven);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Seven == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Eight);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Eight == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Nine);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Nine == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Decimal);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Decimal == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Negate);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Negate == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Backspace);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Backspace == mock->m_lastCommand);
    vm.ButtonPressed->Execute(NumbersAndOperatorsEnum::Clear);
    VERIFY_ARE_EQUAL(++callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Clear == mock->m_lastCommand);
}

// Tests that when we fire the OnGotFocus, it activates the given control
TEST_METHOD(TestOnValueGotFocusActivatesControl)
{
    // shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    // VM::UnitConverterViewModel vm(mock);
    // MockActivatable^ activatable = ref new MockActivatable(false);
    // vm.OnValueGotFocus(AsActivatable(activatable));
    // VERIFY_IS_TRUE(activatable->IsActive);
    // vm.OnValueGotFocus(AsActivatable(activatable)); // try again, starting with true
    // VERIFY_IS_TRUE(activatable->IsActive);
}

// Tests that when we select the currently active value, nothing
// happens
TEST_METHOD(TestReselectCurrentlyActiveValueDoesNothing)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    // Establish base condition
    VERIFY_ARE_EQUAL((UINT)0, mock->m_switchActiveCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_sendCommandCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_setCurUnitTypesCallCount);
    vm.Value1Active = true;
    VERIFY_ARE_EQUAL((UINT)0, mock->m_switchActiveCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_sendCommandCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_setCurUnitTypesCallCount);
}

// Tests that when we set switch the active value, it sets the oppsite value
// to inactive
TEST_METHOD(TestActivatingValueDeactivatesOther)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    vm.Value1Active = true; // base
    VERIFY_IS_TRUE(vm.Value1Active);
    VERIFY_IS_FALSE(vm.Value2Active);
    vm.Value2Active = true; // change
    VERIFY_IS_TRUE(vm.Value2Active);
    VERIFY_IS_FALSE(vm.Value1Active);
    vm.Value2Active = true; // setting it to true again doesnt change anything
    VERIFY_IS_TRUE(vm.Value2Active);
    VERIFY_IS_FALSE(vm.Value1Active);
    vm.Value1Active = true; // back to 1
    VERIFY_IS_TRUE(vm.Value1Active);
    VERIFY_IS_FALSE(vm.Value2Active);
}

// Tests that when we switch the active value, the active value
// gets updated in the model
TEST_METHOD(TestSwitchActiveValueUpdatesActiveValueInModel)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    VERIFY_ARE_EQUAL((UINT)1, mock->m_switchActiveCallCount);
    VERIFY_ARE_EQUAL(0, mock->m_curValue.compare(vTo));
}

// Tests that the suggested visibility value gets updated correctly
TEST_METHOD(TestSuggestedVisibilityIsUpdated)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    VERIFY_IS_TRUE(Visibility::Collapsed == vm.SupplementaryVisibility);
    vector<tuple<wstring, UCM::Unit>> supp;
    supp.push_back(tuple<wstring, UCM::Unit>(L"1", UNIT1));
    vm.UpdateSupplementaryResults(supp);
    WaitForSingleObjectEx(GetCurrentThread(), 1100, FALSE);
    VERIFY_IS_TRUE(Visibility::Visible == vm.SupplementaryVisibility);
    vm.UpdateSupplementaryResults(vector<tuple<wstring, UCM::Unit>>());
    WaitForSingleObjectEx(GetCurrentThread(), 1100, FALSE);
    VERIFY_IS_TRUE(Visibility::Collapsed == vm.SupplementaryVisibility);
}

// Tests that when we switch the active field and get display
// updates, the correct values are being updated.
TEST_METHOD(TestDisplayValueUpdatesAfterSwitchingActiveUpdateTheRightDisplay)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    vm.UpdateDisplay(L"3", L"57");
    VERIFY_IS_TRUE(vm.Value2 == L"3");
    VERIFY_IS_TRUE(vm.Value1 == L"57");
}

// Tests that when we switch the active field and get change units,
// the correct unit values are being passed.
TEST_METHOD(TestUnitChangeAfterSwitchingActiveUpdateUnitsCorrectly)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    vm.Unit2 = vm.Units->GetAt(0);
    VERIFY_IS_TRUE(UNIT4 == mock->m_curFrom);
    vm.Unit1 = vm.Units->GetAt(2);
    VERIFY_IS_TRUE(UNIT6 == mock->m_curTo);
}

// Test that if we switch categories and come back, our first
// and second units are still the same
TEST_METHOD(TestCategorySwitchAndBackKeepsUnitsUnchanged)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.CurrentCategory = vm.Categories->GetAt(2);
    vm.CurrentCategory = vm.Categories->GetAt(0);
    VERIFY_IS_TRUE(UNIT1 == vm.Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT2 == vm.Unit2->GetModelUnit());
}

// Test that if we switch categories and active, and then
// come back, our first and second units are swapped
// and second unit is active
TEST_METHOD(TestCategoryAndActiveSwitchAndBack)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.CurrentCategory = vm.Categories->GetAt(2);
    vm.Value2Active = true;
    vm.CurrentCategory = vm.Categories->GetAt(0);
    VERIFY_IS_TRUE(UNIT2 == vm.Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT1 == vm.Unit2->GetModelUnit());
    VERIFY_IS_TRUE(vm.Value2Active);
}

// Tests that when we switch the active field and then change
// category, the correct units are displayed.
TEST_METHOD(TestCategoryChangeAfterSwitchingActiveUpdatesDisplayCorrectly)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    vm.CurrentCategory = vm.Categories->GetAt(2);
    VERIFY_IS_TRUE(UNIT7 == vm.Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT9 == vm.Unit2->GetModelUnit());
    VERIFY_IS_TRUE(UNIT9 == mock->m_curFrom);
    VERIFY_IS_TRUE(UNIT7 == mock->m_curTo);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_switchActiveCallCount);
    vm.UpdateDisplay(L"5", L"7");
    VERIFY_IS_TRUE(vm.Value2 == L"5");
    VERIFY_IS_TRUE(vm.Value1 == L"7");
}

// Repeat above active switch tests but with a second switch to ensure
// transitions work both ways.
TEST_METHOD(TestSwitchAndReselectCurrentlyActiveValueDoesNothing)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    // Establish base condition
    VERIFY_ARE_EQUAL((UINT)1, mock->m_switchActiveCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_sendCommandCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_setCurUnitTypesCallCount);
    vm.Value2Active = true;
    VERIFY_ARE_EQUAL((UINT)1, mock->m_switchActiveCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_sendCommandCallCount);
    VERIFY_ARE_EQUAL((UINT)1, mock->m_setCurUnitTypesCallCount);
}

TEST_METHOD(TestSwitchActiveValueTwiceUpdatesActiveValueInModel)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    vm.Value1Active = true;
    VERIFY_ARE_EQUAL((UINT)2, mock->m_switchActiveCallCount);
    VERIFY_ARE_EQUAL(0, mock->m_curValue.compare(vFrom));
}

TEST_METHOD(TestDisplayValueUpdatesAfterSwitchingActiveTwiceUpdateTheRightDisplay)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    vm.Value1Active = true;
    vm.UpdateDisplay(L"3", L"57");
    VERIFY_IS_TRUE(vm.Value1 == L"3");
    VERIFY_IS_TRUE(vm.Value2 == L"57");
}

TEST_METHOD(TestUnitChangeAfterSwitchingActiveTwiceUpdateUnitsCorrectly)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    vm.Value1Active = true;
    vm.Unit2 = vm.Units->GetAt(0);
    VERIFY_IS_TRUE(UNIT4 == mock->m_curTo);
    vm.Unit1 = vm.Units->GetAt(2);
    VERIFY_IS_TRUE(UNIT6 == mock->m_curFrom);
}

TEST_METHOD(TestCategoryChangeAfterSwitchingActiveTwiceUpdatesDisplayCorrectly)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"1", *vTo = L"234";
    vm.UpdateDisplay(vFrom, vTo);
    vm.Value2Active = true;
    vm.Value1Active = true;
    vm.CurrentCategory = vm.Categories->GetAt(2);
    VERIFY_IS_TRUE(UNIT9 == vm.Unit1->GetModelUnit());
    VERIFY_IS_TRUE(UNIT7 == vm.Unit2->GetModelUnit());
    VERIFY_IS_TRUE(UNIT9 == mock->m_curFrom);
    VERIFY_IS_TRUE(UNIT7 == mock->m_curTo);
    VERIFY_ARE_EQUAL((UINT)2, mock->m_switchActiveCallCount);
    vm.UpdateDisplay(L"5", L"7");
    VERIFY_IS_TRUE(vm.Value1 == L"5");
    VERIFY_IS_TRUE(vm.Value2 == L"7");
}

// There is a 100 ms fudge time for the time based tests below

// Test that UpdateSupplementaryResults updates the supplementary results
// after a delay
TEST_METHOD(TestSuggestedValuesCallbackUpdatesSupplementaryResults)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    vector<tuple<wstring, UCM::Unit>> supp;
    supp.push_back(tuple<wstring, UCM::Unit>(L"1", UNIT1));
    supp.push_back(tuple<wstring, UCM::Unit>(L"2", UNIT2));
    supp.push_back(tuple<wstring, UCM::Unit>(L"3", UNIT3));
    vm.UpdateSupplementaryResults(supp);
    VERIFY_ARE_EQUAL((UINT)0, vm.SupplementaryResults->Size);
    WaitForSingleObjectEx(GetCurrentThread(), 200, FALSE);
    // Now we should see it
    VERIFY_ARE_EQUAL((UINT)3, vm.SupplementaryResults->Size);
    VERIFY_IS_TRUE(L"1" == vm.SupplementaryResults->GetAt(0)->Value);
    VERIFY_IS_TRUE(UNIT1 == vm.SupplementaryResults->GetAt(0)->Unit->GetModelUnit());
    VERIFY_IS_TRUE(L"2" == vm.SupplementaryResults->GetAt(1)->Value);
    VERIFY_IS_TRUE(UNIT2 == vm.SupplementaryResults->GetAt(1)->Unit->GetModelUnit());
    VERIFY_IS_TRUE(L"3" == vm.SupplementaryResults->GetAt(2)->Value);
    VERIFY_IS_TRUE(UNIT3 == vm.SupplementaryResults->GetAt(2)->Unit->GetModelUnit());
}

// Test that changing category immediately updates supplementary results
TEST_METHOD(TestCategoryChangeImmediatelyUpdatesSupplementaryResults)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vector<tuple<wstring, UCM::Unit>> supp;
    supp.push_back(tuple<wstring, UCM::Unit>(L"1", UNIT1));
    supp.push_back(tuple<wstring, UCM::Unit>(L"2", UNIT2));
    supp.push_back(tuple<wstring, UCM::Unit>(L"3", UNIT3));
    vm.UpdateSupplementaryResults(supp);
    WaitForSingleObjectEx(GetCurrentThread(), 1100, FALSE);
    VERIFY_ARE_EQUAL((UINT)3, vm.SupplementaryResults->Size); // Verify we're in the state we expect as a pre condition

    vm.CurrentCategory = vm.Categories->GetAt(2);
    VERIFY_ARE_EQUAL((UINT)0, vm.SupplementaryResults->Size);
}

// Test that changing category immediately updates supplementary results
TEST_METHOD(TestCategoryChangeImmediatelyUpdatesSupplementaryResultsWhimsy)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vector<tuple<wstring, UCM::Unit>> supp;
    supp.push_back(tuple<wstring, UCM::Unit>(L"1", UNIT1));
    supp.push_back(tuple<wstring, UCM::Unit>(L"2", UNIT2));
    supp.push_back(tuple<wstring, UCM::Unit>(L"3", UNIT3));
    supp.push_back(tuple<wstring, UCM::Unit>(L"4", UNITWHIMSY));
    vm.UpdateSupplementaryResults(supp);
    WaitForSingleObjectEx(GetCurrentThread(), 1100, FALSE);
    VERIFY_ARE_EQUAL((UINT)4, vm.SupplementaryResults->Size); // Verify we're in the state we expect as a pre condition

    VERIFY_IS_TRUE(vm.SupplementaryResults->GetAt(3)->Unit->GetModelUnit().isWhimsical);
    VERIFY_IS_FALSE(vm.SupplementaryResults->GetAt(0)->Unit->GetModelUnit().isWhimsical);
}

// Test that changing units immediately updates supplementary results
TEST_METHOD(TestUnitChangeImmediatelyUpdatesSupplementaryResults)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vector<tuple<wstring, UCM::Unit>> supp;
    supp.push_back(tuple<wstring, UCM::Unit>(L"1", UNIT1));
    supp.push_back(tuple<wstring, UCM::Unit>(L"2", UNIT2));
    supp.push_back(tuple<wstring, UCM::Unit>(L"3", UNIT3));
    vm.UpdateSupplementaryResults(supp);
    WaitForSingleObjectEx(GetCurrentThread(), 1100, FALSE);
    VERIFY_ARE_EQUAL((UINT)3, vm.SupplementaryResults->Size); // Verify we're in the state we expect as a pre condition

    vm.Unit1 = vm.Units->GetAt(2);
    VERIFY_ARE_EQUAL((UINT)0, vm.SupplementaryResults->Size);

    // Reset and try with other unit
    vm.UpdateSupplementaryResults(supp);
    WaitForSingleObjectEx(GetCurrentThread(), 1100, FALSE);
    VERIFY_ARE_EQUAL((UINT)3, vm.SupplementaryResults->Size); // Verify we're in the state we expect as a pre condition

    vm.Unit2 = vm.Units->GetAt(0);
    VERIFY_ARE_EQUAL((UINT)0, vm.SupplementaryResults->Size);
}

// Test that only the first whimsical unit is selected and is appended
// to end of supplementary results.
TEST_METHOD(TestSupplementaryResultsWhimsicalUnits)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    UCM::Unit unit;
    unit.isWhimsical = false;

    UCM::Unit unitWhimSubsequent;
    unitWhimSubsequent.isWhimsical = true;

    vector<tuple<std::wstring, UCM::Unit>> suggestedList;
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", unit));
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", unit));
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", UNITWHIMSY));
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", unitWhimSubsequent));
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", unit));
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", unit));
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", unitWhimSubsequent));
    suggestedList.push_back(tuple<wstring, UCM::Unit>(L"", unit));

    vm.UpdateSupplementaryResults(suggestedList);
    WaitForSingleObjectEx(GetCurrentThread(), 1100, FALSE);
    VERIFY_ARE_EQUAL((UINT)6, vm.SupplementaryResults->Size);
    while (vm.SupplementaryResults->Size > 1)
    {
        VERIFY_IS_FALSE(vm.SupplementaryResults->GetAt(0)->IsWhimsical());
        vm.SupplementaryResults->RemoveAt(0);
    }
    // Last item
    VERIFY_IS_TRUE(vm.SupplementaryResults->GetAt(0)->Unit->GetModelUnit() == UNITWHIMSY);
}

TEST_METHOD(TestOnPaste)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    // Call count is being set to 1 because we send 'CE' command as the first call in OnPaste method of the ViewModel
    UINT callCount = 1;

    // Paste an invalid character - verify that call count doesn't increment
    vm.OnPaste("z");
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);

    // Test all valid characters. Verify that two commands are sent for each character
    vm.OnPaste("0");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Zero == mock->m_lastCommand);

    vm.OnPaste("1");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::One == mock->m_lastCommand);

    vm.OnPaste("2");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Two == mock->m_lastCommand);

    vm.OnPaste("3");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Three == mock->m_lastCommand);

    vm.OnPaste("4");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Four == mock->m_lastCommand);

    vm.OnPaste("5");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Five == mock->m_lastCommand);

    vm.OnPaste("6");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Six == mock->m_lastCommand);

    vm.OnPaste("7");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Seven == mock->m_lastCommand);

    vm.OnPaste("8");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Eight == mock->m_lastCommand);

    vm.OnPaste("9");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Nine == mock->m_lastCommand);

    vm.OnPaste(".");
    callCount += 2;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    VERIFY_IS_TRUE(UCM::Command::Decimal == mock->m_lastCommand);

    vm.OnPaste("-");
    // Call count should increment by one (the Clear command) since negate isn't
    // sent by itself, only after another legal character
    ++callCount;
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);

    // Send an invalid character

    vm.OnPaste("a");
    // Count should remain the same
    VERIFY_ARE_EQUAL(callCount, mock->m_sendCommandCallCount);
    // Last command should remain the same
    VERIFY_IS_TRUE(UCM::Command::Clear == mock->m_lastCommand);
}

TEST_METHOD(TestDecimalFormattingLogic)
{
    // verify that a dangling decimal is left alone until the user switches active fields, and that trailing zeroes
    // entered by the user remain in the display even after a switch.

    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    const WCHAR *vFrom = L"3.", *vTo = L"2.50";
    vm.UpdateDisplay(vFrom, vTo);
    // Establish base condition
    VERIFY_IS_TRUE(vm.Value1 == L"3.");
    VERIFY_IS_TRUE(vm.Value2 == L"2.50");
    vm.SwitchActive->Execute(nullptr);
    VERIFY_IS_TRUE(vm.Value1 == L"3"); // dangling decimal now removed
    VERIFY_IS_TRUE(vm.Value2 == L"2.50");
    vm.SwitchActive->Execute(nullptr);
    VERIFY_IS_TRUE(vm.Value1 == L"3");
    VERIFY_IS_TRUE(vm.Value2 == L"2.50");
}

TEST_METHOD(TestCurrencyFormattingLogic)
{
    // verify that currency fraction digits is formatted per currency type

    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    // Establish base condition
    vm.CurrentCategory = vm.Categories->GetAt(3); // Currency
    vm.Unit1 = vm.Units->GetAt(0);                // JPY
    vm.Unit2 = vm.Units->GetAt(1);                // JOD
    vm.UnitChanged->Execute(nullptr);

    const WCHAR *vFrom = L"1.2340", *vTo = L"0.0070";
    vm.UpdateDisplay(vFrom, vTo);

    VERIFY_IS_TRUE(vm.Value1 == L"1");
    VERIFY_IS_TRUE(vm.Value2 == L"0.007");
    vm.SwitchActive->Execute(nullptr);
    VERIFY_IS_TRUE(vm.Value1 == L"1");
    VERIFY_IS_TRUE(vm.Value2 == L"0.007");
    vm.SwitchActive->Execute(nullptr);
    VERIFY_IS_TRUE(vm.Value1 == L"1");
    VERIFY_IS_TRUE(vm.Value2 == L"0.007");
}

// Tests that when we switch the active field and get display
// updates, the correct automation names are are being updated.
TEST_METHOD(TestValue1AndValue2AutomationNameChanges)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);
    vm.Value1 = L"1";
    vm.Unit1 = vm.Units->GetAt(1);
    VERIFY_IS_TRUE(vm.Value1AutomationName == L"Convert from 1 UNIT5");
    VERIFY_IS_TRUE(vm.Value2AutomationName == L"Converts into 0 UNIT6");
    vm.Value2 = L"234";
    vm.Value2Active = true;
    VERIFY_IS_TRUE(vm.Value2AutomationName == L"Convert from 234 UNIT6");
    VERIFY_IS_TRUE(vm.Value1AutomationName == L"Converts into 1 UNIT5");
    vm.Value2 = L"3";
    vm.Unit2 = vm.Units->GetAt(0);
    VERIFY_IS_TRUE(vm.Value2AutomationName == L"Convert from 3 UNIT4");
    VERIFY_IS_TRUE(vm.Value1AutomationName == L"Converts into 1 UNIT5");
    vm.Value1Active = true;
    VERIFY_IS_TRUE(vm.Value1AutomationName == L"Convert from 1 UNIT5");
    VERIFY_IS_TRUE(vm.Value2AutomationName == L"Converts into 3 UNIT4");
}

// Test that an invalid model unit list results in a Units list of size 1
// containing EMPTY_UNIT.
TEST_METHOD(TestUnitsListBuildsFromEmptyModelUnitList)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vm.BuildUnitList({});

    VERIFY_ARE_EQUAL(1u, vm.Units->Size);
    VERIFY_ARE_EQUAL((UCM::EMPTY_UNIT).id, (vm.Units->GetAt(0)->GetModelUnit()).id);
}

// Test that a valid model unit list vuilds
TEST_METHOD(TestUnitsListBuildsFromValidModelUnitList)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vm.BuildUnitList({ UNIT1, UNIT2, UNIT3 });

    VERIFY_ARE_EQUAL(3u, vm.Units->Size);
    VERIFY_ARE_EQUAL(UNIT1.id, (vm.Units->GetAt(0)->GetModelUnit()).id);
    VERIFY_ARE_EQUAL(UNIT2.id, (vm.Units->GetAt(1)->GetModelUnit()).id);
    VERIFY_ARE_EQUAL(UNIT3.id, (vm.Units->GetAt(2)->GetModelUnit()).id);
}

TEST_METHOD(TestFindInListWhenListIsValid)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vm.BuildUnitList({ UNIT1, UNIT2, UNIT3 });

    Unit ^ foundUnit = vm.FindUnitInList(UNIT1);
    VERIFY_ARE_EQUAL(UNIT1.id, foundUnit->GetModelUnit().id);
}

TEST_METHOD(TestFindInListWhenListIsInvalid)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vm.BuildUnitList({});

    Unit ^ foundUnit = vm.FindUnitInList(UNIT1);

    VERIFY_ARE_EQUAL(UCM::EMPTY_UNIT.id, foundUnit->GetModelUnit().id);
}

TEST_METHOD(TestSetSelectedUnits)
{
    shared_ptr<UnitConverterMock> mock = make_shared<UnitConverterMock>();
    VM::UnitConverterViewModel vm(mock);

    vm.CurrentCategory = vm.Categories->GetAt(0);

    vm.SetSelectedUnits();
    VERIFY_ARE_NOT_EQUAL(UCM::EMPTY_UNIT.id, vm.Unit1->GetModelUnit().id);
    VERIFY_ARE_NOT_EQUAL(UCM::EMPTY_UNIT.id, vm.Unit2->GetModelUnit().id);
}
}
;
}
