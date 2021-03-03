// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>

#include "CalcViewModel/Common/LocalizationSettings.h"

using namespace CalculatorApp::Common;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static Windows::Globalization::NumberFormatting::DecimalFormatter ^ CreateDecimalFormatter(const wchar_t* language, const wchar_t* geographicRegion)
{
    auto languages = ref new Platform::Collections::Vector<Platform::String ^>({ ref new Platform::String(language) });
    return ref new Windows::Globalization::NumberFormatting::DecimalFormatter(languages, ref new Platform::String(geographicRegion));
}

namespace CalculatorUnitTests
{
    TEST_CLASS(LocalizationSettingsUnitTests){ public: TEST_METHOD(TestLocaleName){ auto formatter = CreateDecimalFormatter(L"en-US", L"US");
    LocalizationSettings settings(formatter);
    VERIFY_ARE_EQUAL(L"en-US", settings.GetLocaleName());
}

TEST_METHOD(TestIsDigitEnUsSetting)
{
    auto arabicFormatter = CreateDecimalFormatter(L"ar-AE", L"AE");
    LocalizationSettings arabicSettings(arabicFormatter);
    VERIFY_IS_FALSE(arabicSettings.IsDigitEnUsSetting());

    auto englishFormatter = CreateDecimalFormatter(L"en-US", L"US");
    LocalizationSettings englishSettings(englishFormatter);
    VERIFY_IS_TRUE(englishSettings.IsDigitEnUsSetting());
}

TEST_METHOD(TestLocalizeDisplayValue)
{
    auto formatter = CreateDecimalFormatter(L"ar-AE", L"AE");
    LocalizationSettings settings(formatter);
    std::wstring input(L"A123");
    settings.LocalizeDisplayValue(&input);
    VERIFY_ARE_EQUAL(L"A١٢٣", input);
}

TEST_METHOD(TestGetEnglishValueFromLocalizedDigits)
{
    auto formatter = CreateDecimalFormatter(L"ar-AE", L"AE");
    LocalizationSettings settings(formatter);
    VERIFY_ARE_EQUAL(L"A123", settings.GetEnglishValueFromLocalizedDigits(L"A١٢٣"));
}

TEST_METHOD(TestIsEnUsDigit)
{
    auto& settings = LocalizationSettings::GetInstance();
    VERIFY_IS_FALSE(settings.IsEnUsDigit(L'/'));
    VERIFY_IS_TRUE(settings.IsEnUsDigit(L'0'));
    VERIFY_IS_TRUE(settings.IsEnUsDigit(L'1'));
    VERIFY_IS_TRUE(settings.IsEnUsDigit(L'8'));
    VERIFY_IS_TRUE(settings.IsEnUsDigit(L'9'));
    VERIFY_IS_FALSE(settings.IsEnUsDigit(L':'));
}

TEST_METHOD(TestIsLocalizedDigit)
{
    auto formatter = CreateDecimalFormatter(L"en-US", L"US");
    LocalizationSettings settings(formatter);
    VERIFY_IS_TRUE(settings.IsLocalizedDigit(L'0'));
    VERIFY_IS_FALSE(settings.IsLocalizedDigit(L'A'));
}

TEST_METHOD(TestIsLocalizedHexDigit)
{
    auto formatter = CreateDecimalFormatter(L"en-US", L"US");
    LocalizationSettings settings(formatter);
    VERIFY_IS_TRUE(settings.IsLocalizedHexDigit(L'0'));
    VERIFY_IS_TRUE(settings.IsLocalizedHexDigit(L'A'));
    VERIFY_IS_FALSE(settings.IsLocalizedHexDigit(L'G'));
}

TEST_METHOD(TestRemoveGroupSeparators)
{
    auto formatter = CreateDecimalFormatter(L"en-US", L"US");
    LocalizationSettings settings(formatter);
    VERIFY_ARE_EQUAL(L"1000000", settings.RemoveGroupSeparators(L"1,000 000"));
}
}
;
}
