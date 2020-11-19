// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

namespace UCM = UnitConversionManager;

#pragma once

namespace CalculatorUnitTests
{
    static constexpr int CURRENCY_ID = 16;

    static UCM::Unit UNIT1 = { 1, L"UNIT1", L"U1", true, false, false };
    static UCM::Unit UNIT2 = { 2, L"UNIT2", L"U2", false, true, false };
    static UCM::Unit UNIT3 = { 3, L"UNIT3", L"U3", false, false, false };
    static UCM::Unit UNIT4 = { 4, L"UNIT4", L"U4", true, false, false };
    static UCM::Unit UNIT5 = { 5, L"UNIT5", L"U5", false, false, false };
    static UCM::Unit UNIT6 = { 6, L"UNIT6", L"U6", false, true, false };
    static UCM::Unit UNIT7 = { 7, L"UNIT7", L"U7", false, true, false };
    static UCM::Unit UNIT8 = { 8, L"UNIT8", L"U8", false, false, false };
    static UCM::Unit UNIT9 = { 9, L"UNIT9", L"U9", true, false, false };
    static UCM::Unit UNITWHIMSY = { 10, L"Whimsy", L"UW", true, false, true };
    static UCM::Unit UNITJPY = { 11, L"Japan - Yen", L"JPY", true, true, false };
    static UCM::Unit UNITJOD = { 12, L"Jordan - Dinar", L"JOD", true, true, false };

    static UCM::Category CAT1 = { 1, L"CAT1", false };                       // contains Unit1 - Unit3
    static UCM::Category CAT2 = { 2, L"CAT2", false };                       // contains Unit4 - Unit6
    static UCM::Category CAT3 = { 3, L"CAT3", false };                       // contains Unit7 - Unit9
    static UCM::Category CAT_CURRENCY = { CURRENCY_ID, L"Currency", false }; // contains UnitJPY and UnitJOD

    class UnitConverterMock : public UnitConversionManager::IUnitConverter
    {
    public:
        UnitConverterMock();
        void Initialize() override;
        std::vector<UCM::Category> GetCategories() override;
        UCM::CategorySelectionInitializer SetCurrentCategory(const UCM::Category& input) override;
        UCM::Category GetCurrentCategory();
        void SetCurrentUnitTypes(const UCM::Unit& fromType, const UCM::Unit& toType) override;
        void SwitchActive(const std::wstring& newValue) override;
        bool IsSwitchedActive() const override;
        std::wstring SaveUserPreferences() override;
        void RestoreUserPreferences(_In_ std::wstring_view userPreferences) override;
        void SendCommand(UCM::Command command) override;
        void SetViewModelCallback(const std::shared_ptr<UCM::IUnitConverterVMCallback>& newCallback) override;
        void SetViewModelCurrencyCallback(_In_ const std::shared_ptr<UCM::IViewModelCurrencyCallback>& /*newCallback*/) override
        {
        }
        void Calculate() override
        {
        }
        void ResetCategoriesAndRatios() override
        {
        }
        std::future<std::pair<bool, std::wstring>> RefreshCurrencyRatios() override
        {
            co_return std::make_pair(true, L"");
        }

        UINT m_initCallCount;
        UINT m_getCategoriesCallCount;
        UINT m_setCurrentCategoryCallCount;
        UINT m_setCurUnitTypesCallCount;
        UINT m_switchActiveCallCount;
        UINT m_sendCommandCallCount;
        UINT m_setVMCallbackCallCount;

        UCM::Category m_curCategory;
        UCM::Unit m_curFrom;
        UCM::Unit m_curTo;
        UCM::Command m_lastCommand;

        std::shared_ptr<UCM::IUnitConverterVMCallback> m_vmCallback;
        std::vector<std::tuple<std::wstring, UCM::Unit>> m_suggestedList;
        std::wstring m_curValue;
    };
}
