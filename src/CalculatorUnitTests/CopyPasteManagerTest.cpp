// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/Common/CopyPasteManager.h"
#include "Helpers.h"

using namespace CalculationManager;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Globalization;
using namespace Windows::Storage;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    extern void ChangeMode(StandardCalculatorViewModel ^ viewModel, int mode);

#define ASSERT_POSITIVE_TESTCASES(func, dataSet)                                                                                                               \
    {                                                                                                                                                          \
        for (auto data : dataSet)                                                                                                                              \
        {                                                                                                                                                      \
            VERIFY_ARE_EQUAL(func(data), data);                                                                                                                \
        }                                                                                                                                                      \
    }

#define ASSERT_NEGATIVE_TESTCASES(func, dataSet)                                                                                                               \
    {                                                                                                                                                          \
        for (auto data : dataSet)                                                                                                                              \
        {                                                                                                                                                      \
            VERIFY_ARE_EQUAL(func(data), StringReference(L"NoOp"));                                                                                            \
        }                                                                                                                                                      \
    }

#define VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(ref, length, value, ...)                                                                                    \
    {                                                                                                                                                          \
        auto result = ref;                                                                                                                                     \
        Assert::IsTrue(ref.maxLength == length, __VA_ARGS__);                                                                                                  \
        Assert::IsTrue(ref.maxValue == value, __VA_ARGS__);                                                                                                    \
    }

    TEST_CLASS(CopyPasteManagerTest)
    {
    public:
        CopyPasteManagerTest()
        {
            m_CopyPasteManager = ref new CopyPasteManager();
        }
        TEST_METHOD(FunctionalCopyPasteTest);
        TEST_METHOD(ValidateStandardPasteExpressionTest);
        TEST_METHOD(ValidateScientificPasteExpressionTest);
        TEST_METHOD(ValidateProgrammerDecPasteExpressionTest);
        TEST_METHOD(ValidateProgrammerOctPasteExpressionTest);
        TEST_METHOD(ValidateProgrammerHexPasteExpressionTest);
        TEST_METHOD(ValidateProgrammerBinPasteExpressionTest);
        TEST_METHOD(ValidateConverterPasteExpressionTest);

        TEST_METHOD(ValidatePasteExpressionErrorStates)
        {
            wstring exp_TooLong = L"";
            for (unsigned int i = 0; i < CopyPasteManager::MaxPasteableLength / 8; i++)
            {
                exp_TooLong += L"-1234567";
            }
            VERIFY_ARE_EQUAL(
                m_CopyPasteManager->ValidatePasteExpression(
                    StringReference(exp_TooLong.c_str()), ViewMode::Standard, CategoryGroupType::Calculator, NumberBase::Unknown, BitLength::BitLengthUnknown),
                StringReference(exp_TooLong.c_str()),
                L"Verify ValidatePasteExpression handles expressions up to max length");
            exp_TooLong += L"1";
            VERIFY_ARE_EQUAL(
                m_CopyPasteManager->ValidatePasteExpression(
                    StringReference(exp_TooLong.c_str()), ViewMode::Standard, CategoryGroupType::Calculator, NumberBase::Unknown, BitLength::BitLengthUnknown),
                StringReference(L"NoOp"),
                L"Verify ValidatePasteExpression returns NoOp for strings over max length");

            VERIFY_ARE_EQUAL(
                m_CopyPasteManager->ValidatePasteExpression(
                    StringReference(L""), ViewMode::Standard, CategoryGroupType::Calculator, NumberBase::Unknown, BitLength::BitLengthUnknown),
                StringReference(L"NoOp"),
                L"Verify empty string is invalid");

            VERIFY_ARE_EQUAL(
                m_CopyPasteManager->ValidatePasteExpression(
                    StringReference(L"1a23f456"), ViewMode::Standard, CategoryGroupType::Calculator, NumberBase::Unknown, BitLength::BitLengthUnknown),
                StringReference(L"NoOp"),
                L"Verify pasting unsupported strings for the current mode is invalid");

            VERIFY_ARE_EQUAL(
                m_CopyPasteManager->ValidatePasteExpression(
                    StringReference(L"123"), ViewMode::None, CategoryGroupType::None, NumberBase::Unknown, BitLength::BitLengthUnknown),
                StringReference(L"NoOp"),
                L"Verify pasting without a ViewMode or Category is invalid");
        };

        TEST_METHOD(ValidateExtractOperands)
        {
            vector<wstring> results = {};

            auto oneOperand = ref new Vector<String ^>();
            oneOperand->Append(L"123456");

            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123456", ViewMode::Standard), oneOperand);
            oneOperand->Clear();
            oneOperand->Append(L"123^456");
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123^456", ViewMode::Standard), oneOperand);

            auto twoOperands = ref new Vector<String ^>();
            twoOperands->Append(L"123");
            twoOperands->Append(L"456");
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123+456", ViewMode::Standard), twoOperands);
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123-456", ViewMode::Standard), twoOperands);
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123*456", ViewMode::Standard), twoOperands);
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123/456", ViewMode::Standard), twoOperands);

            auto expOperand = ref new Vector<String ^>();
            expOperand->Append(L"123e456");
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123e456", ViewMode::Standard), expOperand);
            expOperand->Clear();
            expOperand->Append(L"123e4567");
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"123e4567", ViewMode::Standard), expOperand);

            auto twoOperandsParens = ref new Vector<String ^>();
            twoOperandsParens->Append(L"((45)");
            twoOperandsParens->Append(L"(-30))");
            VERIFY_VECTORS_ARE_EQUAL(CopyPasteManager::ExtractOperands(L"((45)+(-30))", ViewMode::Scientific), twoOperandsParens);
        };

        TEST_METHOD(ValidateExtractOperandsErrors)
        {
            auto exp_OperandLimit = ref new String(L"");
            for (unsigned int i = 0; i < m_CopyPasteManager->MaxOperandCount; i++)
            {
                exp_OperandLimit += L"+1";
            }
            VERIFY_ARE_EQUAL(
                CopyPasteManager::ExtractOperands(exp_OperandLimit, ViewMode::Standard)->Size,
                100,
                L"Verify ExtractOperands handles up to MaxOperandCount operands");

            exp_OperandLimit += L"+1";
            VERIFY_ARE_EQUAL(
                CopyPasteManager::ExtractOperands(exp_OperandLimit, ViewMode::Standard)->Size,
                0,
                L"Verify ExtractOperands returns empty vector on too many operands");

            VERIFY_ARE_EQUAL(
                CopyPasteManager::ExtractOperands(L"12e9999", ViewMode::Standard)->Size, 1, L"Verify ExtractOperands handles up to 4 digit exponents");
            VERIFY_ARE_EQUAL(
                CopyPasteManager::ExtractOperands(L"12e10000", ViewMode::Standard)->Size,
                0,
                L"Verify ExtractOperands returns empty vector when the exponent is too long");
        };

        TEST_METHOD(ValidateExpressionRegExMatch)
        {
            VERIFY_IS_FALSE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>(), ViewMode::Standard, CategoryGroupType::Calculator, NumberBase::Unknown, BitLength::BitLengthUnknown),
                L"Verify empty list of operands returns false.");
            VERIFY_IS_FALSE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"123" }),
                    ViewMode::None,
                    CategoryGroupType::Calculator,
                    NumberBase::Unknown,
                    BitLength::BitLengthUnknown),
                L"Verify invalid ViewMode/CategoryGroups return false.");
            VERIFY_IS_FALSE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"123" }),
                    ViewMode::Currency,
                    CategoryGroupType::None,
                    NumberBase::Unknown,
                    BitLength::BitLengthUnknown),
                L"Verify invalid ViewMode/CategoryGroups return false.");

            Logger::WriteMessage(L"Verify operand lengths > max return false.");
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"12345678901234567" }),
                ViewMode::Standard,
                CategoryGroupType::Calculator,
                NumberBase::Unknown,
                BitLength::BitLengthUnknown));
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"123456789012345678901234567890123" }),
                ViewMode::Scientific,
                CategoryGroupType::Calculator,
                NumberBase::Unknown,
                BitLength::BitLengthUnknown));
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"12345678901234567" }),
                ViewMode::None,
                CategoryGroupType::Converter,
                NumberBase::Unknown,
                BitLength::BitLengthUnknown));
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"11111111111111111" }),
                ViewMode::Programmer,
                CategoryGroupType::Calculator,
                NumberBase::HexBase,
                BitLength::BitLengthQWord));
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"12345678901234567890" }),
                ViewMode::Programmer,
                CategoryGroupType::Calculator,
                NumberBase::DecBase,
                BitLength::BitLengthQWord));
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"11111111111111111111111" }),
                ViewMode::Programmer,
                CategoryGroupType::Calculator,
                NumberBase::OctBase,
                BitLength::BitLengthQWord));
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"10000000000000000000000000000000000000000000000000000000000000000" }),
                ViewMode::Programmer,
                CategoryGroupType::Calculator,
                NumberBase::BinBase,
                BitLength::BitLengthQWord));

            VERIFY_IS_FALSE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"9223372036854775808" }),
                    ViewMode::Programmer,
                    CategoryGroupType::Calculator,
                    NumberBase::DecBase,
                    BitLength::BitLengthQWord),
                L"Verify operand values > max return false.");

            VERIFY_IS_TRUE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"((((((((((((((((((((123))))))))))))))))))))" }),
                    ViewMode::Scientific,
                    CategoryGroupType::Calculator,
                    NumberBase::Unknown,
                    BitLength::BitLengthUnknown),
                L"Verify sanitized operand is detected as within max length.");
            VERIFY_IS_TRUE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"9223372036854775807" }),
                    ViewMode::Programmer,
                    CategoryGroupType::Calculator,
                    NumberBase::DecBase,
                    BitLength::BitLengthQWord),
                L"Verify operand values == max return true.");
            VERIFY_IS_TRUE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"-9223372036854775808" }),
                    ViewMode::Programmer,
                    CategoryGroupType::Calculator,
                    NumberBase::DecBase,
                    BitLength::BitLengthQWord),
                L"Verify operand values == max negative return true.");

            Logger::WriteMessage(L"Verify all operands must match patterns.");
            VERIFY_IS_TRUE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"123", L"456" }),
                ViewMode::Standard,
                CategoryGroupType::Calculator,
                NumberBase::Unknown,
                BitLength::BitLengthUnknown));
            VERIFY_IS_TRUE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"123", L"1e23" }),
                ViewMode::Standard,
                CategoryGroupType::Calculator,
                NumberBase::Unknown,
                BitLength::BitLengthUnknown));
            VERIFY_IS_FALSE(m_CopyPasteManager->ExpressionRegExMatch(
                ref new Vector<String ^>({ L"123", L"fab10" }),
                ViewMode::Standard,
                CategoryGroupType::Calculator,
                NumberBase::Unknown,
                BitLength::BitLengthUnknown));

            VERIFY_IS_TRUE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>(
                        { L"1.23e+456", L"1.23e456", L".23e+456", L"123e-456", L"12e2", L"12e+2", L"12e-2", L"-12e2", L"-12e+2", L"-12e-2" }),
                    ViewMode::Scientific,
                    CategoryGroupType::Calculator,
                    NumberBase::Unknown,
                    BitLength::BitLengthUnknown),
                L"Verify exponents are accepted in scientific mode.");

            VERIFY_IS_FALSE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"123", L"12345678901234567" }),
                    ViewMode::Standard,
                    CategoryGroupType::Calculator,
                    NumberBase::Unknown,
                    BitLength::BitLengthUnknown),
                L"Verify all operands must be within maxlength");
            VERIFY_IS_FALSE(
                m_CopyPasteManager->ExpressionRegExMatch(
                    ref new Vector<String ^>({ L"123", L"9223372036854775808" }),
                    ViewMode::Programmer,
                    CategoryGroupType::Calculator,
                    NumberBase::DecBase,
                    BitLength::BitLengthQWord),
                L"Verify all operand must be within max value.");
        };

        TEST_METHOD(ValidateGetMaxOperandLengthAndValue)
        {
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(
                    ViewMode::Standard, CategoryGroupType::None, NumberBase::Unknown, BitLength::BitLengthUnknown),
                m_CopyPasteManager->MaxStandardOperandLength,
                0,
                L"Verify Standard mode maximum values");
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(
                    ViewMode::Scientific, CategoryGroupType::None, NumberBase::Unknown, BitLength::BitLengthUnknown),
                m_CopyPasteManager->MaxScientificOperandLength,
                0,
                L"Verify Scientific mode maximum values");
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::None, CategoryGroupType::Converter, NumberBase::Unknown, BitLength::BitLengthUnknown),
                m_CopyPasteManager->MaxConverterInputLength,
                0,
                L"Verify Converter mode maximum values");

            unsigned long long int ullQwordMax = UINT64_MAX;
            unsigned long long int ullDwordMax = UINT32_MAX;
            unsigned long long int ullWordMax = UINT16_MAX;
            unsigned long long int ullByteMax = UINT8_MAX;
            Logger::WriteMessage(L"Verify Programmer Mode NumberBase::HexBase maximum values");
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::HexBase, BitLength::BitLengthQWord),
                16u,
                ullQwordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::HexBase, BitLength::BitLengthDWord),
                8u,
                ullDwordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::HexBase, BitLength::BitLengthWord),
                4u,
                ullWordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::HexBase, BitLength::BitLengthByte),
                2u,
                ullByteMax);

            Logger::WriteMessage(L"Verify Programmer Mode NumberBase::DecBase maximum values");
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::DecBase, BitLength::BitLengthQWord),
                19u,
                ullQwordMax >> 1);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::DecBase, BitLength::BitLengthDWord),
                10u,
                ullDwordMax >> 1);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::DecBase, BitLength::BitLengthWord),
                5u,
                ullWordMax >> 1);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::DecBase, BitLength::BitLengthByte),
                3u,
                ullByteMax >> 1);

            Logger::WriteMessage(L"Verify Programmer Mode NumberBase::OctBase maximum values");
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::OctBase, BitLength::BitLengthQWord),
                22u,
                ullQwordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::OctBase, BitLength::BitLengthDWord),
                11u,
                ullDwordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::OctBase, BitLength::BitLengthWord),
                6u,
                ullWordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::OctBase, BitLength::BitLengthByte),
                3u,
                ullByteMax);

            Logger::WriteMessage(L"Verify Programmer Mode NumberBase::BinBase maximum values");
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::BinBase, BitLength::BitLengthQWord),
                64u,
                ullQwordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::BinBase, BitLength::BitLengthDWord),
                32u,
                ullDwordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::BinBase, BitLength::BitLengthWord),
                16u,
                ullWordMax);
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, NumberBase::BinBase, BitLength::BitLengthByte),
                8u,
                ullByteMax);

            Logger::WriteMessage(L"Verify invalid ViewModes/Categories return 0 for max values");
            VERIFY_MAXOPERANDLENGTHANDVALUE_ARE_EQUALS(
                m_CopyPasteManager->GetMaxOperandLengthAndValue(ViewMode::None, CategoryGroupType::None, NumberBase::Unknown, BitLength::BitLengthUnknown),
                0u,
                0ull);
        };

        TEST_METHOD(ValidateSanitizeOperand)
        {
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"((1234"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"1234))"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"1234))"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"-1234"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"+1234"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"-(1234)"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"+(1234)"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"12-34"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"((((1234))))"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"1'2'3'4"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"'''''1234''''"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"1_2_3_4"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager->SanitizeOperand(L"______1234___"), L"1234");
        };

        // Using unicode literals here until the encoding issues get figured out
        TEST_METHOD(ValidatePrefixCurrencySymbols)
        {
            // ¥5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u00A5\u0035"), L"5");
            // ¤5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u00A4\u0035"), L"5");
            // ₵5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u20B5\u0035"), L"5");
            // $5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u0024\u0035"), L"5");
            // ₡5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u20A1\u0035"), L"5");
            // ₩5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u20A9\u0035"), L"5");
            // ₪5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u20AA\u0035"), L"5");
            // ₦5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u20A6\u0035"), L"5");
            // ₹5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u20B9\u0035"), L"5");
            // £5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u00A3\u0035"), L"5");
            // €5
            VERIFY_ARE_EQUAL(m_CopyPasteManager->RemoveUnwantedCharsFromString(L"\u20AC\u0035"), L"5");
        };

        TEST_METHOD(ValidateTryOperandToULL)
        {
            IBox<unsigned long long int> ^ result = nullptr;

            Logger::WriteMessage(L"Verify TryOperandToULL NumberBase::HexBase conversion");
            result = CopyPasteManager::TryOperandToULL(L"1234", NumberBase::HexBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0x1234ull);
            result = CopyPasteManager::TryOperandToULL(L"FF", NumberBase::HexBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFull);
            result = CopyPasteManager::TryOperandToULL(L"FFFFFFFFFFFFFFFF", NumberBase::HexBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);
            result = CopyPasteManager::TryOperandToULL(L"0xFFFFFFFFFFFFFFFF", NumberBase::HexBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);
            result = CopyPasteManager::TryOperandToULL(L"0XFFFFFFFFFFFFFFFF", NumberBase::HexBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);
            result = CopyPasteManager::TryOperandToULL(L"0X0FFFFFFFFFFFFFFFF", NumberBase::HexBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL NumberBase::DecBase conversion");
            result = CopyPasteManager::TryOperandToULL(L"1234", NumberBase::DecBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 1234ull);
            result = CopyPasteManager::TryOperandToULL(L"18446744073709551615", NumberBase::DecBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);
            result = CopyPasteManager::TryOperandToULL(L"018446744073709551615", NumberBase::DecBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL NumberBase::OctBase conversion");
            result = CopyPasteManager::TryOperandToULL(L"777", NumberBase::OctBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0777ull);
            result = CopyPasteManager::TryOperandToULL(L"0777", NumberBase::OctBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0777ull);
            result = CopyPasteManager::TryOperandToULL(L"1777777777777777777777", NumberBase::OctBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);
            result = CopyPasteManager::TryOperandToULL(L"01777777777777777777777", NumberBase::OctBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL NumberBase::BinBase conversion");
            result = CopyPasteManager::TryOperandToULL(L"1111", NumberBase::BinBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0b1111ull);
            result = CopyPasteManager::TryOperandToULL(L"0010", NumberBase::BinBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0b10ull);
            result = CopyPasteManager::TryOperandToULL(L"1111111111111111111111111111111111111111111111111111111111111111", NumberBase::BinBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);
            result = CopyPasteManager::TryOperandToULL(L"01111111111111111111111111111111111111111111111111111111111111111", NumberBase::BinBase);
            VERIFY_IS_NOT_NULL(result);
            VERIFY_ARE_EQUAL(result->Value, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL returns false when input is invalid or strtoull throws exceptions");
            // Max values + 1
            VERIFY_IS_NULL(CopyPasteManager::TryOperandToULL(L"0xFFFFFFFFFFFFFFFFF1", NumberBase::HexBase));
            VERIFY_IS_NULL(CopyPasteManager::TryOperandToULL(L"18446744073709551616", NumberBase::DecBase));
            VERIFY_IS_NULL(CopyPasteManager::TryOperandToULL(L"2000000000000000000000", NumberBase::OctBase));
            VERIFY_IS_NULL(CopyPasteManager::TryOperandToULL(L"11111111111111111111111111111111111111111111111111111111111111111", NumberBase::BinBase));
            // Invalid values/characters
            VERIFY_IS_NULL(CopyPasteManager::TryOperandToULL(L"-1", NumberBase::DecBase));
            VERIFY_IS_NULL(CopyPasteManager::TryOperandToULL(L"5555", NumberBase::BinBase));
            VERIFY_IS_NULL(CopyPasteManager::TryOperandToULL(L"xyz", NumberBase::BinBase));
        };

        TEST_METHOD(ValidateStandardScientificOperandLength)
        {
            VERIFY_ARE_EQUAL(m_CopyPasteManager->StandardScientificOperandLength(L""), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->StandardScientificOperandLength(L"0.2"), 1);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->StandardScientificOperandLength(L"1.2"), 2);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->StandardScientificOperandLength(L"0."), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->StandardScientificOperandLength(L"12345"), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->StandardScientificOperandLength(L"-12345"), 6);
        };

        TEST_METHOD(ValidateProgrammerOperandLength)
        {
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"1001", NumberBase::BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"1001b", NumberBase::BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"1001B", NumberBase::BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0b1001", NumberBase::BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0B1001", NumberBase::BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0y1001", NumberBase::BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0Y1001", NumberBase::BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0b", NumberBase::BinBase), 1);

            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"123456", NumberBase::OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0t123456", NumberBase::OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0T123456", NumberBase::OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0o123456", NumberBase::OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0O123456", NumberBase::OctBase), 6);

            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"", NumberBase::DecBase), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"-", NumberBase::DecBase), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"12345", NumberBase::DecBase), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"-12345", NumberBase::DecBase), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0n12345", NumberBase::DecBase), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0N12345", NumberBase::DecBase), 5);

            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"123ABC", NumberBase::HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0x123ABC", NumberBase::HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"0X123ABC", NumberBase::HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"123ABCh", NumberBase::HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager->ProgrammerOperandLength(L"123ABCH", NumberBase::HexBase), 6);
        };

    private:
        CopyPasteManager ^ m_CopyPasteManager;
        String^ ValidateStandardPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Standard, NumberBase::Unknown, BitLength::BitLengthUnknown);
        }

        String^ ValidateScientificPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Scientific, NumberBase::Unknown, BitLength::BitLengthUnknown);
        }

        String^ ValidateConverterPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::None, CategoryGroupType::Converter, NumberBase::Unknown, BitLength::BitLengthUnknown);
        }

        String^ ValidateProgrammerHexQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::HexBase, BitLength::BitLengthQWord);
        }

        String^ ValidateProgrammerHexDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::HexBase, BitLength::BitLengthDWord);
        }

        String^ ValidateProgrammerHexWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::HexBase, BitLength::BitLengthWord);
        }

        String^ ValidateProgrammerHexBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::HexBase, BitLength::BitLengthByte);
        }

        String^ ValidateProgrammerDecQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::DecBase, BitLength::BitLengthQWord);
        }

        String^ ValidateProgrammerDecDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::DecBase, BitLength::BitLengthDWord);
        }

        String^ ValidateProgrammerDecWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::DecBase, BitLength::BitLengthWord);
        }

        String^ ValidateProgrammerDecBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::DecBase, BitLength::BitLengthByte);
        }

        String^ ValidateProgrammerOctQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::OctBase, BitLength::BitLengthQWord);
        }

        String^ ValidateProgrammerOctDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::OctBase, BitLength::BitLengthDWord);
        }

        String^ ValidateProgrammerOctWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::OctBase, BitLength::BitLengthWord);
        }

        String^ ValidateProgrammerOctBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::OctBase, BitLength::BitLengthByte);
        }

        String^ ValidateProgrammerBinQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::BinBase, BitLength::BitLengthQWord);
        }

        String^ ValidateProgrammerBinDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::BinBase, BitLength::BitLengthDWord);
        }

        String^ ValidateProgrammerBinWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::BinBase, BitLength::BitLengthWord);
        }

        String^ ValidateProgrammerBinBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager->ValidatePasteExpression(pastedText, ViewMode::Programmer, NumberBase::BinBase, BitLength::BitLengthByte);
        }
    };

    /*************************************
    standard:
    can paste simple numbers / expressions not exponential numbers / expressions

    scientific :
    can paste exponential numbers / expressions too

    programmer :
    can paste specific numbers / expressions based on radixes.Hex numbers such 13abe is allowed when radix is set to hex, but not allowed otherwise.

    converter :
    can paste simple numbers not expressions

    List of test cases:
    1. simple unsigned number
    2. negative number
    3. exponential number with positive exp
    4. exponential number with negative exp
    5. exponential number with unsigned exp
    6. exponential number with very large(larger than 4 digit) exp
    7. expression involving simple numbers
    8. expression involving exponential numbers
    9. number with random text like xyz
    10. hex numbers
    11. binary numbers
    12. octal numbers
    13. very large number
    14. number with some escape characters in b/w like ",/. \n\r ", '"
    15. expression involving sin, cos or other mathematical functions
    16. expression having more than one operator in b/w operands
    17. expression involving open and close parenthesis (, )

    ****************************************/

    void CopyPasteManagerTest::FunctionalCopyPasteTest()
    {
        // Doesn't have test where converter is involved. Will add such a test later.
        StandardCalculatorViewModel ^ scvm = nullptr;
        scvm = ref new StandardCalculatorViewModel();
        scvm->IsStandard = true;
        String ^ inputs[] = { L"123", L"12345", L"123+456", L"1,234", L"1 2 3", L"\n\r1,234\n", L"\n 1+\n2 ", L"1\"2" };

        for (String ^ &input : inputs)
        {
            // paste number in standard mode and then validate the pastability of displayed number for other modes
            scvm->OnPaste(input);
            VERIFY_ARE_EQUAL(ValidateStandardPasteExpression(scvm->DisplayValue), scvm->DisplayValue);
            VERIFY_ARE_EQUAL(ValidateScientificPasteExpression(scvm->DisplayValue), scvm->DisplayValue);
            VERIFY_ARE_EQUAL(ValidateProgrammerHexQwordPasteExpression(scvm->DisplayValue), scvm->DisplayValue);
        }
    }

    void CopyPasteManagerTest::ValidateStandardPasteExpressionTest()
    {
        String ^ positiveInput[] = {
            L"123",
            L"+123",
            L"-133",
            L"12345.",
            L"+12.34",
            L"12.345",
            L"012.034",
            L"-23.032",
            L"-.123",
            L".1234",
            L"012.012",
            L"123+456",
            L"123+-234",
            L"123*-345",
            L"123*4*-3",
            L"123*+4*-3",
            L"1,234",
            L"1 2 3",
            L"\n\r1,234\n",
            L"\f\n1+2\t\r\v\x85",
            L"\n 1+\n2 ",
            L"1\"2",
            L"1234567891234567" /*boundary condition <=16 digits*/,
            L"2+2=",
            L"2+2=   ",
            L"1.2e23",
            L"12345e-23",

        };
        String ^ negativeInput[] = { L"(123)+(456)", L"abcdef", L"xyz",  L"ABab", L"e+234", L"12345678912345678" /*boundary condition: greater than 16 digits*/,
                                     L"SIN(2)",      L"2+2==",  L"2=+2", L"2%2",  L"10^2" };

        ASSERT_POSITIVE_TESTCASES(ValidateStandardPasteExpression, positiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateStandardPasteExpression, negativeInput);
    }

    void CopyPasteManagerTest::ValidateScientificPasteExpressionTest()
    {
        String ^ positiveInput[] = { L"123",
                                     L"+123",
                                     L"-133",
                                     L"123+456",
                                     L"12345e+023",
                                     L"1,234",
                                     L"1.23",
                                     L"-.123",
                                     L".1234",
                                     L"012.012",
                                     L"123+-234",
                                     L"123*-345",
                                     L"123*4*-3",
                                     L"123*+4*-3",
                                     L"1 2 3",
                                     L"\n\r1,234\n",
                                     L"\f\n1+2\t\r\v\x85",
                                     L"\n 1+\n2 ",
                                     L"1\"2",
                                     L"1.2e+023",
                                     L"12345e-23",
                                     L"(123)+(456)",
                                     L"12345678912345678123456789012345",
                                     L"(123)+(456)=",
                                     L"2+2=   ",
                                     "-(43)",
                                     "+(41213)",
                                     "-(432+3232)",
                                     "-(+(-3213)+(-2312))",
                                     "-(-(432+3232))",
                                     L"1.2e23" /*unsigned exponent*/,
                                     L"12^2",
                                     L"-12.12^-2",
                                     L"61%99"
                                     L"-6.1%99",
                                     L"1.1111111111111111111111111111111e+1142" };
        String ^ negativeInput[] = { L"abcdef",
                                     L"xyz",
                                     L"ABab",
                                     L"e+234",
                                     L"123456789123456781234567890123456" /*boundary condition: greater than 32 digits*/,
                                     L"11.1111111111111111111111111111111e+1142",
                                     L"1.1e+10001",                                   /*boundary condition: exponent greater than 5 digits*/
                                     L"0.11111111111111111111111111111111111e+111111" /*boundary condition: both exponent and non exponent exceed limits*/
                                     L"SIN(2)",
                                     L"2+2==",
                                     L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateScientificPasteExpression, positiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateScientificPasteExpression, negativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerHexPasteExpressionTest()
    {
        String ^ qwordPositiveInput[] = { L"123",
                                          L"123+456",
                                          L"1,234",
                                          L"1 2 3",
                                          L"1'2'3'4",
                                          L"1_2_3_4",
                                          L"12345e-23" /*note: here is considered as E of hex*/,
                                          L"\n\r1,234\n",
                                          L"\f\n1+2\t\r\v\x85",
                                          L"\f\n1+2\t\r\v\x85",
                                          L"\n 1+\n2 ",
                                          L"e+234",
                                          L"1\"2",
                                          L"(123)+(456)",
                                          L"abcdef",
                                          L"ABab",
                                          L"ABCDF21abc41a" /*within boundary*/,
                                          L"0x1234",
                                          L"0xab12",
                                          L"0X1234",
                                          L"AB12h",
                                          L"BC34H",
                                          L"1234u",
                                          L"1234ul",
                                          L"1234ULL",
                                          L"2+2=",
                                          L"2+2=   ",
                                          L"A4C3%12",
                                          L"1233%AB",
                                          L"FFC1%F2" };
        String ^ qwordNegativeInput[] = { L"+123",
                                          L"1.23" /*floating number*/,
                                          L"1''2",
                                          L"'123",
                                          L"123'",
                                          L"1__2",
                                          L"_123",
                                          L"123_",
                                          L"-133",
                                          L"1.2e+023",
                                          L"1.2e23" /*unsigned exponent*/,
                                          L"xyz",
                                          L"ABCDEF21abc41abc7" /*outside boundary of 16 digits*/,
                                          L"SIN(2)",
                                          L"123+-234",
                                          L"1234x",
                                          L"A0x1234",
                                          L"0xx1234",
                                          L"1234uu",
                                          L"1234ulll",
                                          L"2+2==",
                                          L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexQwordPasteExpression, qwordNegativeInput);

        String ^ dwordPositiveInput[] = { L"123",
                                          L"123+456",
                                          L"1,234",
                                          L"1 2 3",
                                          L"1'2'3'4",
                                          L"1_2_3_4",
                                          L"12345e-23" /*note: here is considered as E of hex*/,
                                          L"\n\r1,234\n",
                                          L"\f\n1+2\t\r\v\x85",
                                          L"\n 1+\n2 ",
                                          L"e+234",
                                          L"1\"2",
                                          L"(123)+(456)",
                                          L"abcdef",
                                          L"ABab",
                                          L"ABCD123a" /*within boundary*/,
                                          L"0x1234",
                                          L"0xab12",
                                          L"0X1234",
                                          L"AB12h",
                                          L"BC34H",
                                          L"1234u",
                                          L"1234ul",
                                          L"1234ULL" };
        String ^ dwordNegativeInput[] = { L"+123",
                                          L"1.23" /*floating number*/,
                                          L"1''2",
                                          L"'123",
                                          L"123'",
                                          L"1__2",
                                          L"_123",
                                          L"123_",
                                          L"-133",
                                          L"1.2e+023",
                                          L"1.2e23" /*unsigned exponent*/,
                                          L"xyz",
                                          L"ABCD123ab" /*outside boundary of 8 digits*/,
                                          L"SIN(2)",
                                          L"123+-234",
                                          L"1234x",
                                          L"A0x1234",
                                          L"0xx1234",
                                          L"1234uu",
                                          L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexDwordPasteExpression, dwordNegativeInput);

        String ^ wordPositiveInput[] = { L"123",
                                         L"13+456",
                                         L"1,34",
                                         L"12 3",
                                         L"1'2'3'4",
                                         L"1_2_3_4",
                                         L"15e-23" /*note: here is considered as E of hex*/,
                                         L"\r1",
                                         L"\n\r1,4",
                                         L"\n1,4\n",
                                         L"\f\n1+2\t\r\v",
                                         L"\n 1+\n2 ",
                                         L"e+24",
                                         L"1\"2",
                                         L"(23)+(4)",
                                         L"aef",
                                         L"ABab",
                                         L"A1a3" /*within boundary*/,
                                         L"FFFF" /*boundary condition: max allowed number*/,
                                         L"0x1234",
                                         L"0xab12",
                                         L"0X1234",
                                         L"AB12h",
                                         L"BC34H",
                                         L"1234u",
                                         L"1234ul",
                                         L"1234ULL" };
        String ^ wordNegativeInput[] = { L"+123",
                                         L"1.23" /*floating number*/,
                                         L"1''2",
                                         L"'123",
                                         L"123'",
                                         L"1__2",
                                         L"_123",
                                         L"123_",
                                         L"-133",
                                         L"1.2e+023",
                                         L"1.2e23" /*unsigned exponent*/,
                                         L"xyz",
                                         L"A1a3b" /*outside boundary of 4 digits*/,
                                         L"SIN(2)",
                                         L"123+-234",
                                         L"1234x",
                                         L"A0x1234",
                                         L"0xx1234",
                                         L"1234uu",
                                         L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexWordPasteExpression, wordNegativeInput);

        String ^ bytePositiveInput[] = { L"13",
                                         L"13+6",
                                         L"1,4",
                                         L"2 3",
                                         L"1'2",
                                         L"1_2",
                                         L"5e-3" /*note: here is considered as E of hex*/,
                                         L"\r1",
                                         L"a",
                                         L"ab",
                                         L"A1" /*within boundary*/,
                                         L"0x12",
                                         L"0xab",
                                         L"0X12",
                                         L"A9h",
                                         L"B8H",
                                         L"12u",
                                         L"12ul",
                                         L"12ULL" };
        String ^ byteNegativeInput[] = { L"+3",
                                         L"1.2" /*floating number*/,
                                         L"1''2",
                                         L"'12",
                                         L"12'",
                                         L"1__2",
                                         L"_12",
                                         L"12_",
                                         L"-3",
                                         L"1.1e+02",
                                         L"1.2e3" /*unsigned exponent*/,
                                         L"xz",
                                         L"A3a" /*outside boundary of 2 digits*/,
                                         L"SIN(2)",
                                         L"13+-23",
                                         L"12x",
                                         L"A0x1",
                                         L"0xx12",
                                         L"12uu",
                                         L"12ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerDecPasteExpressionTest()
    {
        String ^ qwordPositiveInput[] = { L"123",
                                          L"+123",
                                          L"-133",
                                          L"123+456",
                                          L"1,234",
                                          L"1 2 3",
                                          L"1'2'3'4",
                                          L"1_2_3_4",
                                          L"\n\r1,234\n",
                                          L"\f\n1+2\t\r\v\x85",
                                          L"\n 1+\n2 ",
                                          L"1\"2",
                                          L"(123)+(456)",
                                          L"123+-234",
                                          L"123*-345",
                                          L"123*4*-3",
                                          L"123*+4*-3",
                                          L"9223372036854775807",
                                          L"-9223372036854775808" /*boundary condition: max/min allowed number*/,
                                          L"0n1234",
                                          L"0N1234",
                                          L"1234u",
                                          L"1234ul",
                                          L"1234ULL",
                                          L"2+2=",
                                          L"2+2=   ",
                                          L"823%21" };
        String ^ qwordNegativeInput[] = { L"1.23",
                                          L"1''2",
                                          L"'123",
                                          L"123'",
                                          L"1__2",
                                          L"_123",
                                          L"123_",
                                          L"1.2e23" /*unsigned exponent*/,
                                          L"1.2e+023",
                                          L"12345e-23",
                                          L"abcdef",
                                          L"xyz",
                                          L"ABab",
                                          L"e+234",
                                          L"9223372036854775808" /*boundary condition: greater than max allowed number 9223372036854775807*/,
                                          L"9223372036854775809" /*boundary condition: greater than max allowed number 9223372036854775807*/,
                                          L"SIN(2)",
                                          L"-0n123",
                                          L"0nn1234",
                                          L"1234uu",
                                          L"1234ulll",
                                          L"2+2==",
                                          L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecQwordPasteExpression, qwordNegativeInput);

        String ^ dwordPositiveInput[] = { L"123",
                                          L"+123",
                                          L"-133",
                                          L"123+456",
                                          L"1,234",
                                          L"1 2 3",
                                          L"1'2'3'4",
                                          L"1_2_3_4",
                                          L"\n\r1,234\n",
                                          L"\f\n1+2\t\r\v\x85",
                                          L"\n 1+\n2 ",
                                          L"1\"2",
                                          L"(123)+(456)",
                                          L"123+-234",
                                          L"123*-345",
                                          L"123*4*-3",
                                          L"123*+4*-3",
                                          L"2147483647",
                                          L"-2147483647" /*boundary condition: max/min allowed number*/,
                                          L"0n1234",
                                          L"0N1234",
                                          L"1234u",
                                          L"1234ul",
                                          L"1234ULL" };
        String ^ dwordNegativeInput[] = { L"1.23",
                                          L"1''2",
                                          L"'123",
                                          L"123'",
                                          L"1__2",
                                          L"_123",
                                          L"123_",
                                          L"1.2e23" /*unsigned exponent*/,
                                          L"1.2e+023",
                                          L"12345e-23",
                                          L"abcdef",
                                          L"xyz",
                                          L"ABab",
                                          L"e+234",
                                          L"2147483649" /*boundary condition: greater than max allowed number 2147483647*/,
                                          L"SIN(2)",
                                          L"-0n123",
                                          L"0nn1234",
                                          L"1234uu",
                                          L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecDwordPasteExpression, dwordNegativeInput);

        String ^ wordPositiveInput[] = { L"123",
                                         L"+123",
                                         L"-133",
                                         L"123+456",
                                         L"1,234",
                                         L"1 2 3",
                                         L"1'2'3'4",
                                         L"1_2_3_4",
                                         L"\f\n1+2\t\r\v\x85",
                                         L"1\"2",
                                         L"(123)+(456)",
                                         L"123+-234",
                                         L"123*-345",
                                         L"123*4*-3",
                                         L"123*+4*-3",
                                         L"32767",
                                         L"-32767",
                                         L"-32768" /*boundary condition: max/min allowed number*/,
                                         L"0n1234",
                                         L"0N1234",
                                         L"1234u",
                                         L"1234ul",
                                         L"1234ULL" };
        String ^ wordNegativeInput[] = { L"1.23",
                                         L"1''2",
                                         L"'123",
                                         L"123'",
                                         L"1__2",
                                         L"_123",
                                         L"123_",
                                         L"1.2e23" /*unsigned exponent*/,
                                         L"1.2e+023",
                                         L"12345e-23",
                                         L"abcdef",
                                         L"xyz",
                                         L"ABab",
                                         L"e+234",
                                         L"32769" /*boundary condition: greater than max allowed number 32769*/,
                                         L"SIN(2)",
                                         L"-0n123",
                                         L"0nn1234",
                                         L"1234uu",
                                         L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecWordPasteExpression, wordNegativeInput);

        String ^ bytePositiveInput[] = { L"13",    L"+13",    L"-13",     L"13+46", L"13+-34",
                                         L"13*-3", L"3*4*-3", L"3*+4*-3", L"1,3",   L"1 3",
                                         L"1'2'3", L"1_2_3",  L"1\"2",    L"127",   L"-127" /*boundary condition: max/min allowed number*/,
                                         L"0n123", L"0N123",  L"123u",    L"123ul", L"123ULL" };
        String ^ byteNegativeInput[] = { L"1.23",
                                         L"1''2",
                                         L"'123",
                                         L"123'",
                                         L"1__2",
                                         L"_123",
                                         L"123_",
                                         L"1.2e23" /*unsigned exponent*/,
                                         L"1.2e+023",
                                         L"15e-23",
                                         L"abcdef",
                                         L"xyz",
                                         L"ABab",
                                         L"e+24",
                                         L"129" /*boundary condition: greater than max allowed number 127*/,
                                         L"SIN(2)",
                                         L"-0n123",
                                         L"0nn1234",
                                         L"123uu",
                                         L"123ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerOctPasteExpressionTest()
    {
        String ^ qwordPositiveInput[] = { L"123",         L"123+456",
                                          L"1,234",       L"1 2 3",
                                          L"1'2'3'4",     L"1_2_3_4",
                                          L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85",
                                          L"\n 1+\n2 ",   L"1\"2",
                                          L"(123)+(456)", L"0t1234",
                                          L"0T1234",      L"0o1234",
                                          L"0O1234",      L"1234u",
                                          L"1234ul",      L"1234ULL",
                                          L"2+2=",        L"2+2=   ",
                                          L"127%71",      L"1777777777777777777777" /*boundary condition: the max allowed number*/ };
        String ^ qwordNegativeInput[] = { L"+123",
                                          L"1.23",
                                          L"1''2",
                                          L"'123",
                                          L"123'",
                                          L"1__2",
                                          L"_123",
                                          L"123_",
                                          L"-133",
                                          L"1.2e23" /*unsigned exponent*/,
                                          L"1.2e+023",
                                          L"12345e-23",
                                          L"abcdef",
                                          L"xyz",
                                          L"ABab",
                                          L"e+234",
                                          L"12345678901234567890123" /*boundary condition: greater than max allowed digits 22*/,
                                          L"2000000000000000000000" /*boundary condition: greater than max allowed number*/,
                                          L"SIN(2)",
                                          L"123+-234",
                                          L"0ot1234",
                                          L"1234uu",
                                          L"1234ulll",
                                          L"2+2==",
                                          L"2=+2",
                                          L"89%12" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctQwordPasteExpression, qwordNegativeInput);

        String ^ dwordPositiveInput[] = { L"123",       L"123+456", L"1,234",       L"1 2 3",
                                          L"1'2'3'4",   L"1_2_3_4", L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85",
                                          L"\n 1+\n2 ", L"1\"2",    L"(123)+(456)", L"37777777777" /*boundary condition: max allowed number*/,
                                          L"0t1234",    L"0T1234",  L"0o1234",      L"0O1234",
                                          L"1234u",     L"1234ul",  L"1234ULL" };
        String ^ dwordNegativeInput[] = { L"+123",
                                          L"1.23",
                                          L"1''2",
                                          L"'123",
                                          L"123'",
                                          L"1__2",
                                          L"_123",
                                          L"123_",
                                          L"-133",
                                          L"1.2e23" /*unsigned exponent*/,
                                          L"1.2e+023",
                                          L"12345e-23",
                                          L"abcdef",
                                          L"xyz",
                                          L"ABab",
                                          L"e+234",
                                          L"377777777771" /*boundary condition: greater than max allowed number 37777777777*/,
                                          L"40000000000" /*boundary condition: greater than max allowed number 37777777777*/,
                                          L"SIN(2)",
                                          L"123+-234",
                                          L"0ot1234",
                                          L"1234uu",
                                          L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctDwordPasteExpression, dwordNegativeInput);

        String ^ wordPositiveInput[] = { L"123",
                                         L"123+456",
                                         L"1,234",
                                         L"1 2 3",
                                         L"1'2'3'4",
                                         L"1_2_3_4",
                                         L"\f\n1+2\t\r\v\x85",
                                         L"1\"2",
                                         L"(123)+(456)",
                                         L"177777" /*boundary condition: max allowed number*/,
                                         L"0t1234",
                                         L"0T1234",
                                         L"0o1234",
                                         L"0O1234",
                                         L"1234u",
                                         L"1234ul",
                                         L"1234ULL" };
        String ^ wordNegativeInput[] = { L"+123",
                                         L"1.23",
                                         L"1''2",
                                         L"'123",
                                         L"123'",
                                         L"1__2",
                                         L"_123",
                                         L"123_",
                                         L"-133",
                                         L"1.2e23" /*unsigned exponent*/,
                                         L"1.2e+023",
                                         L"12345e-23",
                                         L"abcdef",
                                         L"xyz",
                                         L"ABab",
                                         L"e+234",
                                         L"1777771" /*boundary condition: greater than max allowed number 177777*/,
                                         L"200000" /*boundary condition: greater than max allowed number 177777*/,
                                         L"SIN(2)",
                                         L"123+-234",
                                         L"0ot1234",
                                         L"1234uu",
                                         L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctWordPasteExpression, wordNegativeInput);

        String
            ^ bytePositiveInput[] = { L"13",    L"13+46", L"1,3",   L"1 3",   L"1'2'3", L"1_2_3", L"1\"2",  L"377" /*boundary condition: max allowed number*/,
                                      L"0t123", L"0T123", L"0o123", L"0O123", L"123u",  L"123ul", L"123ULL" };
        String ^ byteNegativeInput[] = { L"+123",
                                         L"1.23",
                                         L"1''2",
                                         L"'123",
                                         L"123'",
                                         L"1__2",
                                         L"_123",
                                         L"123_",
                                         L"-13",
                                         L"1.2e23" /*unsigned exponent*/,
                                         L"1.2e+023",
                                         L"15e-23",
                                         L"abcdef",
                                         L"xyz",
                                         L"ABab",
                                         L"e+24",
                                         L"477" /*boundary condition: greater than max allowed number 377*/,
                                         L"400" /*boundary condition: greater than max allowed number 377*/,
                                         L"SIN(2)",
                                         L"123+-34",
                                         L"0ot123",
                                         L"123uu",
                                         L"123ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerBinPasteExpressionTest()
    {
        String ^ qwordPositiveInput[] = { L"100",
                                          L"100+101",
                                          L"1,001",
                                          L"1 0 1",
                                          L"1'0'0'1",
                                          L"1_0_0_1",
                                          L"\n\r1,010\n",
                                          L"\f\n1+11\t\r\v\x85",
                                          L"\n 1+\n1 ",
                                          L"1\"1",
                                          L"(101)+(10)",
                                          L"0b1001",
                                          L"0B1111",
                                          L"0y1001",
                                          L"0Y1001",
                                          L"1100b",
                                          L"1101B",
                                          L"1111u",
                                          L"1111ul",
                                          L"1111ULL",
                                          L"1010101010101010101010101011110110100100101010101001010101001010" /*boundary condition: max allowed digits 64*/,
                                          L"1+10=",
                                          L"1+10=   ",
                                          L"1001%10" };
        String ^ qwordNegativeInput[] = {
            L"+10101",
            L"1.01",
            L"1''0",
            L"'101",
            L"101'",
            L"1__0",
            L"_101",
            L"101_",
            L"-10101001",
            L"123",
            L"1.2e23" /*unsigned exponent*/,
            L"1.2e+023",
            L"101010e-1010",
            L"abcdef",
            L"xyz",
            L"ABab",
            L"e+10101",
            L"b1001",
            L"10b01",
            L"0x10",
            L"1001x",
            L"1001h",
            L"0bb1111",
            L"1111uu",
            L"1111ulll",
            L"10101010101010101010101010111101101001001010101010010101010010100" /*boundary condition: greater than max allowed digits 64*/,
            L"SIN(01010)",
            L"10+-10101010101",
            L"1+10==",
            L"1=+10"
        };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinQwordPasteExpression, qwordNegativeInput);

        String ^ dwordPositiveInput[] = { L"100",
                                          L"100+101",
                                          L"1,001",
                                          L"1 0 1",
                                          L"1'0'0'1",
                                          L"1_0_0_1",
                                          L"\n\r1,010\n",
                                          L"\f\n1+11\t\r\v\x85",
                                          L"\n 1+\n1 ",
                                          L"1\"1",
                                          L"(101)+(10)",
                                          L"0b1001",
                                          L"0B1111",
                                          L"0y1001",
                                          L"0Y1001",
                                          L"1100b",
                                          L"1101B",
                                          L"1111u",
                                          L"1111ul",
                                          L"1111ULL",
                                          L"10101001001010101101010111111100" /*boundary condition: max allowed number*/ };
        String ^ dwordNegativeInput[] = { L"+10101",
                                          L"1.01",
                                          L"1''0",
                                          L"'101",
                                          L"101'",
                                          L"1__0",
                                          L"_101",
                                          L"101_",
                                          L"-10101001",
                                          L"123",
                                          L"1.2e23" /*unsigned exponent*/,
                                          L"1.2e+023",
                                          L"101010e-1010",
                                          L"abcdef",
                                          L"xyz",
                                          L"ABab",
                                          L"e+10101",
                                          L"b1001",
                                          L"10b01",
                                          L"0x10",
                                          L"1001x",
                                          L"1001h",
                                          L"0bb1111",
                                          L"1111uu",
                                          L"1111ulll",
                                          L"101010010010101011010101111111001" /*boundary condition: greater than max allowed digits 32*/,
                                          L"SIN(01010)",
                                          L"10+-10101010101" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinDwordPasteExpression, dwordNegativeInput);

        String
            ^ wordPositiveInput
                [] = { L"100",       L"100+101", L"1,001",      L"1 0 1",   L"1'0'0'1",         L"1_0_0_1", L"\n\r1,010\n", L"\f\n1+11\t\r\v\x85",
                       L"\n 1+\n1 ", L"1\"1",    L"(101)+(10)", L"0b1001",  L"0B1111",          L"0y1001",  L"0Y1001",      L"1100b",
                       L"1101B",     L"1111u",   L"1111ul",     L"1111ULL", L"1010101010010010" /*boundary condition: max allowed number*/ };
        String ^ wordNegativeInput[] = { L"+10101",
                                         L"1.01",
                                         L"1''0",
                                         L"'101",
                                         L"101'",
                                         L"1__0",
                                         L"_101",
                                         L"101_",
                                         L"-10101001",
                                         L"123",
                                         L"1.2e23" /*unsigned exponent*/,
                                         L"1.2e+023",
                                         L"101010e-1010",
                                         L"abcdef",
                                         L"xyz",
                                         L"ABab",
                                         L"e+10101",
                                         L"b1001",
                                         L"10b01",
                                         L"0x10",
                                         L"1001x",
                                         L"1001h",
                                         L"0bb1111",
                                         L"1111uu",
                                         L"1111ulll",
                                         L"10101010100100101" /*boundary condition: greater than max allowed digits 16*/,
                                         L"SIN(01010)",
                                         L"10+-10101010101" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinWordPasteExpression, wordNegativeInput);

        String ^ bytePositiveInput[] = { L"100",   L"100+101",    L"1,001",   L"1 0 1",    L"1'0'0'1", L"1_0_0_1", L"\n\r1,010\n", L"\n 1+\n1 ",
                                         L"1\"1",  L"(101)+(10)", L"0b1001",  L"0B1111",   L"0y1001",  L"0Y1001",  L"1100b",       L"1101B",
                                         L"1111u", L"1111ul",     L"1111ULL", L"10100010", L"11111111" /*boundary condition: max allowed number*/ };
        String ^ byteNegativeInput[] = { L"+10101",
                                         L"1.01",
                                         L"1''0",
                                         L"'101",
                                         L"101'",
                                         L"1__0",
                                         L"_101",
                                         L"101_",
                                         L"-10101001",
                                         L"123",
                                         L"1.2e23" /*unsigned exponent*/,
                                         L"1.2e+023",
                                         L"101010e-1010",
                                         L"abcdef",
                                         L"xyz",
                                         L"ABab",
                                         L"e+10101",
                                         L"b1001",
                                         L"10b01",
                                         L"0x10",
                                         L"1001x",
                                         L"1001h",
                                         L"0bb1111",
                                         L"1111uu",
                                         L"1111ulll",
                                         L"101000101" /*boundary condition: greater than max allowed digits 8*/,
                                         L"100000000" /*boundary condition: greater than max allowed value*/,
                                         L"SIN(01010)",
                                         L"10+-1010101" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateConverterPasteExpressionTest()
    {
        String ^ positiveInput[] = { L"123",  L"+123", L"-133",   L"12345.", L"012.012", L"1,234", L"1 2 3", L"\n\r1,234\n", L"\f\n12\t\r\v\x85",
                                     L"1\"2", L"100=", L"100=   " };
        String ^ negativeInput[] = { L"(123)+(456)",
                                     L"1.2e23" /*unsigned exponent*/,
                                     L"12345e-23",
                                     L"\n 1+\n2 ",
                                     L"123+456",
                                     L"abcdef",
                                     L"\n 1+\n2 ",
                                     L"xyz",
                                     L"ABab",
                                     L"e+234",
                                     L"12345678912345678" /*boundary condition: greater than 16 bits*/,
                                     L"SIN(2)",
                                     L"123+-234",
                                     L"100==",
                                     L"=100" };

        ASSERT_POSITIVE_TESTCASES(ValidateConverterPasteExpression, positiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateConverterPasteExpression, negativeInput);
    }
}
