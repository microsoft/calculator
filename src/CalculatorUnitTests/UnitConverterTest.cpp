// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>

using namespace UnitConversionManager;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitConverterUnitTests
{
    void SetUnitParams(Unit* type, int id, wstring name, wstring abbreviation, bool conversionSource, bool conversionTarget, bool isWhimsical)
    {
        type->id = id;
        type->name = name;
        type->abbreviation = abbreviation;
        type->isConversionSource = conversionSource;
        type->isConversionTarget = conversionTarget;
        type->isWhimsical = isWhimsical;
    }

    void SetCategoryParams(Category* type, int id, wstring name, bool supportsNegative)
    {
        type->id = id;
        type->name = name;
        type->supportsNegative = supportsNegative;
    }

    void SetConversionDataParams(ConversionData* type, double ratio, double offset, bool offsetFirst)
    {
        type->ratio = ratio;
        type->offset = offset;
        type->offsetFirst = offsetFirst;
    }

    class TestUnitConverterConfigLoader : public IConverterDataLoader
    {
    public:
        TestUnitConverterConfigLoader()
            : m_loadDataCallCount(0)
        {
            Category c1, c2;
            SetCategoryParams(&c1, 1, L"Length", true);
            SetCategoryParams(&c2, 2, L"Weight", false);
            m_categories.push_back(c1);
            m_categories.push_back(c2);

            Unit u1, u2, u3, u4;
            SetUnitParams(&u1, 1, L"Inches", L"In", true, true, false);
            SetUnitParams(&u2, 2, L"Feet", L"Ft", false, false, false);
            SetUnitParams(&u3, 3, L"Pounds", L"Lb", true, true, false);
            SetUnitParams(&u4, 4, L"Kilograms", L"Kg", false, false, false);

            vector<Unit> c1units = vector<Unit>();
            vector<Unit> c2units = vector<Unit>();
            c1units.push_back(u1);
            c1units.push_back(u2);
            c2units.push_back(u3);
            c2units.push_back(u4);

            m_units[c1.id] = c1units;
            m_units[c2.id] = c2units;

            unordered_map<Unit, ConversionData, UnitHash> unit1Map = unordered_map<Unit, ConversionData, UnitHash>();
            unordered_map<Unit, ConversionData, UnitHash> unit2Map = unordered_map<Unit, ConversionData, UnitHash>();
            unordered_map<Unit, ConversionData, UnitHash> unit3Map = unordered_map<Unit, ConversionData, UnitHash>();
            unordered_map<Unit, ConversionData, UnitHash> unit4Map = unordered_map<Unit, ConversionData, UnitHash>();

            ConversionData conversion1, conversion2, conversion3, conversion4, conversion5;
            SetConversionDataParams(&conversion1, 1.0, 0, false);
            SetConversionDataParams(&conversion2, 0.08333333333333333333333333333333, 0, false);
            SetConversionDataParams(&conversion3, 12.0, 0, false);
            SetConversionDataParams(&conversion4, 0.453592, 0, false);
            SetConversionDataParams(&conversion5, 2.20462, 0, false);

            // Setting the conversion ratios for testing
            unit1Map[u1] = conversion1;
            unit1Map[u2] = conversion2;

            unit2Map[u1] = conversion3;
            unit2Map[u2] = conversion1;

            unit3Map[u3] = conversion1;
            unit3Map[u4] = conversion4;

            unit4Map[u3] = conversion5;
            unit4Map[u4] = conversion1;

            m_ratioMaps[u1] = unit1Map;
            m_ratioMaps[u2] = unit2Map;
            m_ratioMaps[u3] = unit3Map;
            m_ratioMaps[u4] = unit4Map;
        }

        void LoadData()
        {
            m_loadDataCallCount++;
        }

        vector<Category> GetOrderedCategories()
        {
            return m_categories;
        }

        vector<Unit> GetOrderedUnits(const Category& category)
        {
            return m_units[category.id];
        }

        unordered_map<Unit, ConversionData, UnitHash> LoadOrderedRatios(const Unit& u)
        {
            return m_ratioMaps[u];
        }

        bool SupportsCategory(const Category& /*target*/)
        {
            return true;
        }

        UINT m_loadDataCallCount;

    private:
        vector<Category> m_categories;
        CategoryToUnitVectorMap m_units;
        UnitToUnitToConversionDataMap m_ratioMaps;
    };

    class TestUnitConverterVMCallback : public IUnitConverterVMCallback
    {
    public:
        void Reset()
        {
            m_maxDigitsReachedCallCount = 0;
        }

        void DisplayCallback(const wstring& from, const wstring& to) override
        {
            m_lastFrom = from;
            m_lastTo = to;
        }

        void SuggestedValueCallback(const vector<tuple<wstring, Unit>>& suggestedValues) override
        {
            m_lastSuggested = suggestedValues;
        }

        void MaxDigitsReached() override
        {
            m_maxDigitsReachedCallCount++;
        }

        int GetMaxDigitsReachedCallCount()
        {
            return m_maxDigitsReachedCallCount;
        }

        bool CheckDisplayValues(wstring from, wstring to)
        {
            return (from == m_lastFrom && to == m_lastTo);
        }

        bool CheckSuggestedValues(vector<tuple<wstring, Unit>> suggested)
        {
            if (suggested.size() != m_lastSuggested.size())
            {
                return false;
            }
            bool returnValue = true;
            for (unsigned int i = 0; i < suggested.size(); i++)
            {
                if (suggested[i] != m_lastSuggested[i])
                {
                    returnValue = false;
                    break;
                }
            }
            return returnValue;
        }

    private:
        wstring m_lastFrom;
        wstring m_lastTo;
        vector<tuple<wstring, Unit>> m_lastSuggested;
        int m_maxDigitsReachedCallCount = 0;
    };

    TEST_CLASS(UnitConverterTest)
    {
    public:
        // Declare this class as a TestClass, and supply metadata if necessary.
        TEST_CLASS_INITIALIZE(CommonSetup);

        TEST_METHOD_CLEANUP(Cleanup);

        TEST_METHOD(UnitConverterTestInit);
        TEST_METHOD(UnitConverterTestBasic);
        TEST_METHOD(UnitConverterTestGetters);
        TEST_METHOD(UnitConverterTestGetCategory);
        TEST_METHOD(UnitConverterTestUnitTypeSwitching);
        TEST_METHOD(UnitConverterTestQuote);
        TEST_METHOD(UnitConverterTestUnquote);
        TEST_METHOD(UnitConverterTestBackspace);
        TEST_METHOD(UnitConverterTestBackspaceBasic);
        TEST_METHOD(UnitConverterTestClear);
        TEST_METHOD(UnitConverterTestScientificInputs);
        TEST_METHOD(UnitConverterTestSupplementaryResultRounding);
        TEST_METHOD(UnitConverterTestMaxDigitsReached);
        TEST_METHOD(UnitConverterTestMaxDigitsReached_LeadingDecimal);
        TEST_METHOD(UnitConverterTestMaxDigitsReached_TrailingDecimal);
        TEST_METHOD(UnitConverterTestMaxDigitsReached_MultipleTimes);

    private:
        static void ExecuteCommands(vector<Command> commands);

        static shared_ptr<UnitConverter> s_unitConverter;
        static shared_ptr<TestUnitConverterConfigLoader> s_xmlLoader;
        static shared_ptr<TestUnitConverterVMCallback> s_testVMCallback;
        static Category s_testLength;
        static Category s_testWeight;
        static Unit s_testInches;
        static Unit s_testFeet;
        static Unit s_testPounds;
        static Unit s_testKilograms;
    };

    shared_ptr<UnitConverter> UnitConverterTest::s_unitConverter;
    shared_ptr<TestUnitConverterConfigLoader> UnitConverterTest::s_xmlLoader;
    shared_ptr<TestUnitConverterVMCallback> UnitConverterTest::s_testVMCallback;
    Category UnitConverterTest::s_testLength;
    Category UnitConverterTest::s_testWeight;
    Unit UnitConverterTest::s_testInches;
    Unit UnitConverterTest::s_testFeet;
    Unit UnitConverterTest::s_testPounds;
    Unit UnitConverterTest::s_testKilograms;

    // Creates instance of UnitConverter before running tests
    void UnitConverterTest::CommonSetup()
    {
        s_testVMCallback = make_shared<TestUnitConverterVMCallback>();
        s_xmlLoader = make_shared<TestUnitConverterConfigLoader>();
        s_unitConverter = make_shared<UnitConverter>(s_xmlLoader);
        s_unitConverter->SetViewModelCallback(s_testVMCallback);
        SetCategoryParams(&s_testLength, 1, L"Length", true);
        SetCategoryParams(&s_testWeight, 2, L"Weight", false);
        SetUnitParams(&s_testInches, 1, L"Inches", L"In", true, true, false);
        SetUnitParams(&s_testFeet, 2, L"Feet", L"Ft", false, false, false);
        SetUnitParams(&s_testPounds, 3, L"Pounds", L"Lb", true, true, false);
        SetUnitParams(&s_testKilograms, 4, L"Kilograms", L"Kg", false, false, false);
    }

    // Resets calculator state to start state after each test
    void UnitConverterTest::Cleanup()
    {
        s_unitConverter->SendCommand(Command::Reset);
        s_testVMCallback->Reset();
    }

    void UnitConverterTest::ExecuteCommands(vector<Command> commands)
    {
        for (size_t i = 0; i < commands.size() && commands[i] != Command::None; i++)
        {
            s_unitConverter->SendCommand(commands[i]);
        }
    }

    // Test ctor/initialization states
    void UnitConverterTest::UnitConverterTestInit()
    {
        VERIFY_ARE_EQUAL((UINT)0, s_xmlLoader->m_loadDataCallCount); // shouldn't have initialized the loader yet
        s_unitConverter->Initialize();
        VERIFY_ARE_EQUAL((UINT)1, s_xmlLoader->m_loadDataCallCount); // now we should have loaded
    }

    // Verify a basic input command stream.'3', '2', '.', '0'
    void UnitConverterTest::UnitConverterTestBasic()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"0.25"), s_testFeet) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"2.5"), s_testFeet) };

        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"3"), wstring(L"3")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Zero);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Decimal);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30."), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Zero);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.0"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
    }

    // Verify a basic copy paste steam. '20.43' with backspace button pressed
    void UnitConverterTest::UnitConverterTestBackspaceBasic()
    {
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Backspace);

        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20.4"), wstring(L"20.4")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20."), wstring(L"20")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"20"), wstring(L"20")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"2"), wstring(L"2")));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
    }

    // Verify a basic copy paste steam. '20.43' with backspace button pressed
    void UnitConverterTest::UnitConverterTestClear()
    {
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Clear);

        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
    }

    // Check the getter functions
    void UnitConverterTest::UnitConverterTestGetters()
    {
        Category test1[] = { s_testLength, s_testWeight };
        Unit test2[] = { s_testInches, s_testFeet };

        VERIFY_IS_TRUE(s_unitConverter->GetCategories() == vector<Category>(begin(test1), end(test1)));
        VERIFY_IS_TRUE(get<0>(s_unitConverter->SetCurrentCategory(test1[0])) == vector<Unit>(begin(test2), end(test2)));
    }

    // Test getting category after it has been set.
    void UnitConverterTest::UnitConverterTestGetCategory()
    {
        s_unitConverter->SetCurrentCategory(s_testWeight);
        VERIFY_IS_TRUE(s_unitConverter->GetCurrentCategory() == s_testWeight);
    }

    // Test switching of unit types
    void UnitConverterTest::UnitConverterTestUnitTypeSwitching()
    {
        // Enter 57 into the from field, then switch focus to the to field (making it the new from field)
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Seven);
        s_unitConverter->SwitchActive(wstring(L"57"));
        // Now set unit conversion to go from kilograms to pounds
        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testKilograms, s_testPounds);
        s_unitConverter->SendCommand(Command::Five);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"5"), wstring(L"11.0231")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>()));
    }

    // Test input escaping
    void UnitConverterTest::UnitConverterTestQuote()
    {
        constexpr wstring_view input1 = L"Weight";
        constexpr wstring_view output1 = L"Weight";
        constexpr wstring_view input2 = L"{p}Weig;[ht|";
        constexpr wstring_view output2 = L"{lb}p{rb}Weig{sc}{lc}ht{p}";
        constexpr wstring_view input3 = L"{{{t;s}}},:]";
        constexpr wstring_view output3 = L"{lb}{lb}{lb}t{sc}s{rb}{rb}{rb}{cm}{co}{rc}";
        VERIFY_IS_TRUE(UnitConverter::Quote(input1) == output1);
        VERIFY_IS_TRUE(UnitConverter::Quote(input2) == output2);
        VERIFY_IS_TRUE(UnitConverter::Quote(input3) == output3);
    }

    // Test output unescaping
    void UnitConverterTest::UnitConverterTestUnquote()
    {
        constexpr wstring_view input1 = L"Weight";
        constexpr wstring_view input2 = L"{p}Weig;[ht|";
        constexpr wstring_view input3 = L"{{{t;s}}},:]";
        VERIFY_IS_TRUE(UnitConverter::Unquote(input1) == input1);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input1)) == input1);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input2)) == input2);
        VERIFY_IS_TRUE(UnitConverter::Unquote(UnitConverter::Quote(input3)) == input3);
    }

    // Test backspace commands
    void UnitConverterTest::UnitConverterTestBackspace()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"13.66"), s_testKilograms) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"13.65"), s_testKilograms) };
        tuple<wstring, Unit> test3[] = { tuple<wstring, Unit>(wstring(L"13.61"), s_testKilograms) };
        tuple<wstring, Unit> test4[] = { tuple<wstring, Unit>(wstring(L"1.36"), s_testKilograms) };

        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testPounds, s_testPounds);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.12"), wstring(L"30.12")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30.1"), wstring(L"30.1")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30."), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"30"), wstring(L"30")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"3"), wstring(L"3")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test4), end(test4))));
        s_unitConverter->SendCommand(Command::Backspace);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0"), wstring(L"0")));
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>()));
    }

    // Test large values
    void UnitConverterTest::UnitConverterTestScientificInputs()
    {
        s_unitConverter->SetCurrentCategory(s_testWeight);
        s_unitConverter->SetCurrentUnitTypes(s_testPounds, s_testKilograms);
        s_unitConverter->SendCommand(Command::Decimal);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::Zero);
        s_unitConverter->SendCommand(Command::One);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"0.00000000000001"), wstring(L"4.535920e-15")));
        s_unitConverter->SwitchActive(wstring(L"4.535920e-15"));
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        s_unitConverter->SendCommand(Command::Nine);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"999999999999999"), wstring(L"2.204620e+15")));
        s_unitConverter->SwitchActive(wstring(L"2.20463e+15"));
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Six);
        s_unitConverter->SendCommand(Command::Seven);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"1234567"), wstring(L"559989.7")));
        s_unitConverter->SwitchActive(wstring(L"559989.7"));
        s_unitConverter->SendCommand(Command::One);
        s_unitConverter->SendCommand(Command::Two);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Four);
        s_unitConverter->SendCommand(Command::Five);
        s_unitConverter->SendCommand(Command::Six);
        s_unitConverter->SendCommand(Command::Seven);
        s_unitConverter->SendCommand(Command::Eight);
        VERIFY_IS_TRUE(s_testVMCallback->CheckDisplayValues(wstring(L"12345678"), wstring(L"27217529")));
    }

    // Test large values
    void UnitConverterTest::UnitConverterTestSupplementaryResultRounding()
    {
        tuple<wstring, Unit> test1[] = { tuple<wstring, Unit>(wstring(L"27.75"), s_testFeet) };
        tuple<wstring, Unit> test2[] = { tuple<wstring, Unit>(wstring(L"277.8"), s_testFeet) };
        tuple<wstring, Unit> test3[] = { tuple<wstring, Unit>(wstring(L"2778"), s_testFeet) };
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Three);
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test1), end(test1))));
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test2), end(test2))));
        s_unitConverter->SendCommand(Command::Three);
        VERIFY_IS_TRUE(s_testVMCallback->CheckSuggestedValues(vector<tuple<wstring, Unit>>(begin(test3), end(test3))));
    }

    void UnitConverterTest::UnitConverterTestMaxDigitsReached()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }

    void UnitConverterTest::UnitConverterTestMaxDigitsReached_LeadingDecimal()
    {
        ExecuteCommands({ Command::Zero,
                          Command::Decimal,
                          Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::Two });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }

    void UnitConverterTest::UnitConverterTestMaxDigitsReached_TrailingDecimal()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two,
                          Command::Decimal });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        ExecuteCommands({ Command::One });

        VERIFY_ARE_EQUAL(1, s_testVMCallback->GetMaxDigitsReachedCallCount());
    }

    void UnitConverterTest::UnitConverterTestMaxDigitsReached_MultipleTimes()
    {
        ExecuteCommands({ Command::One,
                          Command::Two,
                          Command::Three,
                          Command::Four,
                          Command::Five,
                          Command::Six,
                          Command::Seven,
                          Command::Eight,
                          Command::Nine,
                          Command::One,
                          Command::Zero,
                          Command::One,
                          Command::One,
                          Command::One,
                          Command::Two });

        VERIFY_ARE_EQUAL(0, s_testVMCallback->GetMaxDigitsReachedCallCount());

        for (auto count = 1; count <= 10; count++)
        {
            ExecuteCommands({ Command::Three });

            VERIFY_ARE_EQUAL(count, s_testVMCallback->GetMaxDigitsReachedCallCount(), to_wstring(count).c_str());
        }
    }
}
