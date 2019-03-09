// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <CppUnitTest.h>

#include "CalcViewModel/StandardCalculatorViewModel.h"
#include "CalcViewModel/Common/CopyPasteManager.h"

using namespace CalculationManager;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace CalculatorApp::ViewModel;
using namespace Platform;
using namespace std;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Globalization;
using namespace Windows::Storage;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorUnitTests
{
    extern void ChangeMode(StandardCalculatorViewModel^ viewModel, int mode);

#define ASSERT_POSITIVE_TESTCASES(func, dataSet) \
{\
    int size = sizeof(dataSet)/sizeof(*dataSet);\
    while(--size)\
    {\
        VERIFY_ARE_EQUAL(func(dataSet[size]), dataSet[size]);\
    }\
}

#define ASSERT_NEGATIVE_TESTCASES(func, dataSet) \
{\
    int size = sizeof(dataSet)/sizeof(*dataSet);\
    while(--size)\
    {\
        VERIFY_ARE_EQUAL(func(dataSet[size]), StringReference(L"NoOp"));\
    }\
}

// returns a iterator from end
#define START_LOOP(dataSet)\
{\
    int size = sizeof(dataSet)/sizeof(*dataSet);\
    while(--size)\
    {

#define END_LOOP\
    }\
}

    TEST_CLASS(CopyPasteManagerTest)
    {
    public:
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
            for (int i = 0; i < m_CopyPasteManager.MaxPasteableLength / 8; i++)
            {
                exp_TooLong += L"-1234567";
            }
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ValidatePasteExpression(StringReference(exp_TooLong.c_str()), ViewMode::Standard, CategoryGroupType::Calculator, -1, -1), StringReference(exp_TooLong.c_str()), L"Verify ValidatePasteExpression handles expressions up to max length");
            exp_TooLong += L"1";
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ValidatePasteExpression(StringReference(exp_TooLong.c_str()), ViewMode::Standard, CategoryGroupType::Calculator, -1, -1), StringReference(L"NoOp"), L"Verify ValidatePasteExpression returns NoOp for strings over max length");

            VERIFY_ARE_EQUAL(m_CopyPasteManager.ValidatePasteExpression(StringReference(L""), ViewMode::Standard, CategoryGroupType::Calculator, -1, -1), StringReference(L"NoOp"), L"Verify empty string is invalid");

            VERIFY_ARE_EQUAL(m_CopyPasteManager.ValidatePasteExpression(StringReference(L"123e456"), ViewMode::Standard, CategoryGroupType::Calculator, -1, -1), StringReference(L"NoOp"), L"Verify pasting unsupported strings for the current mode is invalid");

            VERIFY_ARE_EQUAL(m_CopyPasteManager.ValidatePasteExpression(StringReference(L"123"), ViewMode::None, CategoryGroupType::None, -1, -1), StringReference(L"NoOp"), L"Verify pasting without a ViewMode or Category is invalid");
        };

        TEST_METHOD(ValidateExtractOperands)
        {
            vector<wstring> results = {};

            vector<wstring> oneOperand = { L"123456" };
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123456", ViewMode::Standard), oneOperand);
            oneOperand = { L"123^456" };
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123^456", ViewMode::Standard), oneOperand);

            vector<wstring> twoOperands = { L"123", L"456" };
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123+456", ViewMode::Standard), twoOperands);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123-456", ViewMode::Standard), twoOperands);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123*456", ViewMode::Standard), twoOperands);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123/456", ViewMode::Standard), twoOperands);

            vector<wstring> expOperand = { L"123e456" };
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123e456", ViewMode::Standard), expOperand);
            expOperand = { L"123e4567" };
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"123e4567", ViewMode::Standard), expOperand);

            vector<wstring> twoOperandsParens = { L"((45)", L"(-30))" };
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"((45)+(-30))", ViewMode::Scientific), twoOperandsParens);
        };

        TEST_METHOD(ValidateExtractOperandsErrors)
        {
            wstring exp_OperandLimit = L"";
            for (int i = 0; i < m_CopyPasteManager.MaxOperandCount; i++)
            {
                exp_OperandLimit += L"+1";
            }
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode::Standard).size(), 100, L"Verify ExtractOperands handles up to MaxOperandCount operands");

            exp_OperandLimit += L"+1";
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(exp_OperandLimit, ViewMode::Standard).size(), 0, L"Verify ExtractOperands returns empty vector on too many operands");

            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"12e9999", ViewMode::Standard).size(), 1, L"Verify ExtractOperands handles up to 4 digit exponents");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ExtractOperands(L"12e10000", ViewMode::Standard).size(), 0, L"Verify ExtractOperands returns empty vector when the exponent is too long");
        };

        TEST_METHOD(ValidateExpressionRegExMatch)
        {
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{}, ViewMode::Standard, CategoryGroupType::Calculator, -1, -1), L"Verify empty list of operands returns false.");
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"123" }, ViewMode::None, CategoryGroupType::Calculator, -1, -1), L"Verify invalid ViewMode/CategoryGroups return false.");
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"123" }, ViewMode::Currency, CategoryGroupType::None, -1, -1), L"Verify invalid ViewMode/CategoryGroups return false.");

            Logger::WriteMessage(L"Verify operand lengths > max return false.");
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"12345678901234567" }, ViewMode::Standard, CategoryGroupType::Calculator, -1, -1));
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"123456789012345678901234567890123" }, ViewMode::Scientific, CategoryGroupType::Calculator, -1, -1));
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"12345678901234567" }, ViewMode::None, CategoryGroupType::Converter, -1, -1));
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"11111111111111111" }, ViewMode::Programmer, CategoryGroupType::Calculator, HexBase, QwordType));
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"12345678901234567890" }, ViewMode::Programmer, CategoryGroupType::Calculator, DecBase, QwordType));
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"11111111111111111111111" }, ViewMode::Programmer, CategoryGroupType::Calculator, OctBase, QwordType));
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"10000000000000000000000000000000000000000000000000000000000000000" }, ViewMode::Programmer, CategoryGroupType::Calculator, BinBase, QwordType));

            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"9223372036854775808" }, ViewMode::Programmer, CategoryGroupType::Calculator, DecBase, QwordType), L"Verify operand values > max return false.");

            VERIFY_IS_TRUE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"((((((((((((((((((((123))))))))))))))))))))" }, ViewMode::Scientific, CategoryGroupType::Calculator, -1, -1), L"Verify sanitized operand is detected as within max length.");
            VERIFY_IS_TRUE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"9223372036854775807" }, ViewMode::Programmer, CategoryGroupType::Calculator, DecBase, QwordType), L"Verify operand values == max return true.");

            Logger::WriteMessage(L"Verify all operands must match patterns.");
            VERIFY_IS_TRUE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"123", L"456" }, ViewMode::Standard, CategoryGroupType::Calculator, -1, -1));
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"123", L"1e23" }, ViewMode::Standard, CategoryGroupType::Calculator, -1, -1));

            VERIFY_IS_TRUE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"1.23e+456" }, ViewMode::Scientific, CategoryGroupType::Calculator, -1, -1), L"Verify operand only needs to match one pattern.");

            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"123", L"12345678901234567" }, ViewMode::Standard, CategoryGroupType::Calculator, -1, -1), L"Verify all operands must be within maxlength");
            VERIFY_IS_FALSE(m_CopyPasteManager.ExpressionRegExMatch(vector<wstring>{ L"123", L"9223372036854775808" }, ViewMode::Programmer, CategoryGroupType::Calculator, DecBase, QwordType), L"Verify all operand must be within max value.");
        };

        TEST_METHOD(ValidateGetMaxOperandLengthAndValue)
        {
            pair<size_t, unsigned long long int> standardModeMaximums = make_pair(m_CopyPasteManager.MaxStandardOperandLength, 0);
            pair<size_t, unsigned long long int> scientificModeMaximums = make_pair(m_CopyPasteManager.MaxScientificOperandLength, 0);
            pair<size_t, unsigned long long int> converterModeMaximums = make_pair(m_CopyPasteManager.MaxConverterInputLength, 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Standard, CategoryGroupType::None, -1, -1), standardModeMaximums, L"Verify Standard mode maximum values");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Scientific, CategoryGroupType::None, -1, -1), scientificModeMaximums, L"Verify Scientific mode maximum values");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::None, CategoryGroupType::Converter, -1, -1), converterModeMaximums, L"Verify Converter mode maximum values");

            unsigned long long int ullQwordMax = UINT64_MAX;
            unsigned long long int ullDwordMax = UINT32_MAX;
            unsigned long long int ullWordMax = UINT16_MAX;
            unsigned long long int ullByteMax = UINT8_MAX;
            Logger::WriteMessage(L"Verify Programmer Mode HexBase maximum values");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, HexBase, QwordType), make_pair((size_t)16u, ullQwordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, HexBase, DwordType), make_pair((size_t)8u, ullDwordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, HexBase, WordType), make_pair((size_t)4u, ullWordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, HexBase, ByteType), make_pair((size_t)2u, ullByteMax));

            Logger::WriteMessage(L"Verify Programmer Mode DecBase maximum values");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, DecBase, QwordType), make_pair((size_t)19u, ullQwordMax >> 1));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, DecBase, DwordType), make_pair((size_t)10u, ullDwordMax >> 1));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, DecBase, WordType), make_pair((size_t)5u, ullWordMax >> 1));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, DecBase, ByteType), make_pair((size_t)3u, ullByteMax >> 1));

            Logger::WriteMessage(L"Verify Programmer Mode OctBase maximum values");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, OctBase, QwordType), make_pair((size_t)22u, ullQwordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, OctBase, DwordType), make_pair((size_t)11u, ullDwordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, OctBase, WordType), make_pair((size_t)6u, ullWordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, OctBase, ByteType), make_pair((size_t)3u, ullByteMax));

            Logger::WriteMessage(L"Verify Programmer Mode BinBase maximum values");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, BinBase, QwordType), make_pair((size_t)64u, ullQwordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, BinBase, DwordType), make_pair((size_t)32u, ullDwordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, BinBase, WordType), make_pair((size_t)16u, ullWordMax));
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::Programmer, CategoryGroupType::None, BinBase, ByteType), make_pair((size_t)8u, ullByteMax));

            Logger::WriteMessage(L"Verify invalid ViewModes/Categories return 0 for max values");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.GetMaxOperandLengthAndValue(ViewMode::None, CategoryGroupType::None, -1, -1), make_pair((size_t)0u, 0ull));
        };

        TEST_METHOD(ValidateSanitizeOperand)
        {
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"((1234"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"1234))"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"-1234"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"12-34"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"((((1234))))"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"1'2'3'4"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"'''''1234''''"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"1_2_3_4"), L"1234");
            VERIFY_ARE_EQUAL(m_CopyPasteManager.SanitizeOperand(L"______1234___"), L"1234");
        };

        TEST_METHOD(ValidateTryOperandToULL)
        {
            unsigned long long int result = 0;

            Logger::WriteMessage(L"Verify TryOperandToULL HexBase conversion");
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"1234", HexBase, result));
            VERIFY_ARE_EQUAL(result, 0x1234ull);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"FF", HexBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFull);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"FFFFFFFFFFFFFFFF", HexBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"0xFFFFFFFFFFFFFFFF", HexBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"0XFFFFFFFFFFFFFFFF", HexBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"0X0FFFFFFFFFFFFFFFF", HexBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL DecBase conversion");
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"1234", DecBase, result));
            VERIFY_ARE_EQUAL(result, 1234ull);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"18446744073709551615", DecBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"018446744073709551615", DecBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL OctBase conversion");
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"777", OctBase, result));
            VERIFY_ARE_EQUAL(result, 0777ull);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"0777", OctBase, result));
            VERIFY_ARE_EQUAL(result, 0777ull);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"1777777777777777777777", OctBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"01777777777777777777777", OctBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL BinBase conversion");
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"1111", BinBase, result));
            VERIFY_ARE_EQUAL(result, 0b1111ull);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"0010", BinBase, result));
            VERIFY_ARE_EQUAL(result, 0b10ull);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"1111111111111111111111111111111111111111111111111111111111111111", BinBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"01111111111111111111111111111111111111111111111111111111111111111", BinBase, result));
            VERIFY_ARE_EQUAL(result, 0xFFFF'FFFF'FFFF'FFFF);

            Logger::WriteMessage(L"Verify TryOperandToULL invalid numberBase defaults to DecBase");
            VERIFY_IS_TRUE(m_CopyPasteManager.TryOperandToULL(L"1234", 128, result));
            VERIFY_ARE_EQUAL(result, 1234ull);

            Logger::WriteMessage(L"Verify TryOperandToULL returns false when input is invalid or strtoull throws exceptions");
            // Max values + 1
            VERIFY_IS_FALSE(m_CopyPasteManager.TryOperandToULL(L"0xFFFFFFFFFFFFFFFFF1", HexBase, result));
            VERIFY_IS_FALSE(m_CopyPasteManager.TryOperandToULL(L"18446744073709551616", DecBase, result));
            VERIFY_IS_FALSE(m_CopyPasteManager.TryOperandToULL(L"2000000000000000000000", OctBase, result));
            VERIFY_IS_FALSE(m_CopyPasteManager.TryOperandToULL(L"11111111111111111111111111111111111111111111111111111111111111111", BinBase, result));
            // Invalid values/characters
            VERIFY_IS_FALSE(m_CopyPasteManager.TryOperandToULL(L"-1", DecBase, result));
            VERIFY_IS_FALSE(m_CopyPasteManager.TryOperandToULL(L"5555", BinBase, result));
            VERIFY_IS_FALSE(m_CopyPasteManager.TryOperandToULL(L"xyz", BinBase, result));

        };

        TEST_METHOD(ValidateStandardScientificOperandLength)
        {
            VERIFY_ARE_EQUAL(m_CopyPasteManager.StandardScientificOperandLength(L""), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.StandardScientificOperandLength(L"0.2"), 1);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.StandardScientificOperandLength(L"1.2"), 2);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.StandardScientificOperandLength(L"0."), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.StandardScientificOperandLength(L"12345"), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.StandardScientificOperandLength(L"-12345"), 6);

        };

        TEST_METHOD(ValidateProgrammerOperandLength)
        {
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"1001", BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"1001b", BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"1001B", BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0b1001", BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0B1001", BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0y1001", BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0Y1001", BinBase), 4);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0b", BinBase), 1);

            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"123456", OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0t123456", OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0T123456", OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0o123456", OctBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0O123456", OctBase), 6);

            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"", DecBase), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"-", DecBase), 0);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"12345", DecBase), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"-12345", DecBase), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0n12345", DecBase), 5);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0N12345", DecBase), 5);

            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"123ABC", HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0x123ABC", HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"0X123ABC", HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"123ABCh", HexBase), 6);
            VERIFY_ARE_EQUAL(m_CopyPasteManager.ProgrammerOperandLength(L"123ABCH", HexBase), 6);
        };

    private:
        CopyPasteManager m_CopyPasteManager;
        String^ ValidateStandardPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Standard, -1/*number base*/, -1/*bitlength Type*/);
        }

        String^ ValidateScientificPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Scientific, -1/*number base*/, -1/*bitlength Type*/);
        }

        String^ ValidateConverterPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::None, CategoryGroupType::Converter, -1/*number base*/, -1/*bitlength Type*/);
        }

        String^ ValidateProgrammerHexQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, HexBase/*number base*/, QwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerHexDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, HexBase/*number base*/, DwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerHexWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, HexBase/*number base*/, WordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerHexBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, HexBase/*number base*/, ByteType/*bitlength Type*/);
        }

        String^ ValidateProgrammerDecQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, DecBase/*number base*/, QwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerDecDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, DecBase/*number base*/, DwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerDecWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, DecBase/*number base*/, WordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerDecBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, DecBase/*number base*/, ByteType/*bitlength Type*/);
        }

        String^ ValidateProgrammerOctQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, OctBase/*number base*/, QwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerOctDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, OctBase/*number base*/, DwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerOctWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, OctBase/*number base*/, WordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerOctBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, OctBase/*number base*/, ByteType/*bitlength Type*/);
        }

        String^ ValidateProgrammerBinQwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, BinBase/*number base*/, QwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerBinDwordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, BinBase/*number base*/, DwordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerBinWordPasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, BinBase/*number base*/, WordType/*bitlength Type*/);
        }

        String^ ValidateProgrammerBinBytePasteExpression(_In_ String^ pastedText)
        {
            return m_CopyPasteManager.ValidatePasteExpression(pastedText, ViewMode::Programmer, BinBase/*number base*/, ByteType/*bitlength Type*/);
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
        StandardCalculatorViewModel^ scvm = ref new StandardCalculatorViewModel();
        scvm->IsStandard = true;
        String^ input[] = { L"123", L"12345", L"123+456", L"1,234", L"1 2 3", L"\n\r1,234\n", L"\n 1+\n2 ", L"1\"2" };

        START_LOOP(input)
            // paste number in standard mode and then validate the pastability of displayed number for other modes
            scvm->OnPaste(input[size], ViewMode::Standard);
            VERIFY_ARE_EQUAL(ValidateStandardPasteExpression(scvm->DisplayValue), scvm->DisplayValue);
            VERIFY_ARE_EQUAL(ValidateScientificPasteExpression(scvm->DisplayValue), scvm->DisplayValue);
            VERIFY_ARE_EQUAL(ValidateProgrammerHexQwordPasteExpression(scvm->DisplayValue), scvm->DisplayValue);
        END_LOOP
    }


    void CopyPasteManagerTest::ValidateStandardPasteExpressionTest()
    {
        String^ positiveInput[] = { L"123", L"+123", L"-133", L"12345.", L"+12.34", L"12.345", L"012.034", L"-23.032", L"-.123", L".1234", L"012.012", L"123+456", L"123+-234", L"123*-345", L"123*4*-3", L"123*+4*-3", L"1,234", L"1 2 3", L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"1\"2", L"1234567891234567"/*boundary condition <=16 digits*/, L"2+2=", L"2+2=   " };
        String^ negativeInput[] = { L"(123)+(456)", L"1.2e23"/*unsigned exponent*/, L"12345e-23", L"abcdef", L"xyz", L"ABab", L"e+234", L"12345678912345678"/*boundary condition: greater than 16 digits*/, L"SIN(2)", L"2+2==", L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateStandardPasteExpression, positiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateStandardPasteExpression, negativeInput);
    }

    void CopyPasteManagerTest::ValidateScientificPasteExpressionTest()
    {
        String^ positiveInput[] = { L"123", L"+123", L"-133", L"123+456", L"12345e+023", L"1,234", L"1.23", L"-.123", L".1234", L"012.012", L"123+-234", L"123*-345", L"123*4*-3", L"123*+4*-3", L"1 2 3", L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"1\"2", L"1.2e+023", L"12345e-23", L"(123)+(456)", L"12345678912345678123456789012345", L"(123)+(456)=", L"2+2=   " };
        String^ negativeInput[] = { L"1.2e23"/*unsigned exponent*/, L"abcdef", L"xyz", L"ABab", L"e+234", L"123456789123456781234567890123456"/*boundary condition: greater than 32 digits*/, L"SIN(2)", L"2+2==", L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateScientificPasteExpression, positiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateScientificPasteExpression, negativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerHexPasteExpressionTest()
    {
        String^ qwordPositiveInput[] = { L"123", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"12345e-23"/*note: here is considered as E of hex*/, L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"e+234", L"1\"2", L"(123)+(456)", L"abcdef", L"ABab", L"ABCDF21abc41a"/*within boundary*/, L"0x1234", L"0xab12", L"0X1234", L"AB12h", L"BC34H", L"1234u", L"1234ul", L"1234ULL", L"2+2=", L"2+2=   " };
        String^ qwordNegativeInput[] = { L"+123", L"1.23"/*floating number*/, L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"-133", L"1.2e+023", L"1.2e23"/*unsigned exponent*/, L"xyz", L"ABCDEF21abc41abc7"/*outside boundary of 16 digits*/, L"SIN(2)", L"123+-234", L"1234x", L"A0x1234", L"0xx1234", L"1234uu", L"1234ulll", L"2+2==", L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexQwordPasteExpression, qwordNegativeInput);

        String^ dwordPositiveInput[] = { L"123", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"12345e-23"/*note: here is considered as E of hex*/, L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"e+234", L"1\"2", L"(123)+(456)", L"abcdef", L"ABab", L"ABCD123a"/*within boundary*/, L"0x1234", L"0xab12", L"0X1234", L"AB12h", L"BC34H", L"1234u", L"1234ul", L"1234ULL" };
        String^ dwordNegativeInput[] = { L"+123", L"1.23"/*floating number*/, L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"-133", L"1.2e+023", L"1.2e23"/*unsigned exponent*/, L"xyz", L"ABCD123ab"/*outside boundary of 8 digits*/, L"SIN(2)", L"123+-234", L"1234x", L"A0x1234", L"0xx1234", L"1234uu", L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexDwordPasteExpression, dwordNegativeInput);

        String^ wordPositiveInput[] = { L"123", L"13+456", L"1,34", L"12 3", L"1'2'3'4", L"1_2_3_4", L"15e-23"/*note: here is considered as E of hex*/, L"\r1", L"\n\r1,4", L"\n1,4\n", L"\f\n1+2\t\r\v", L"\n 1+\n2 ", L"e+24", L"1\"2", L"(23)+(4)", L"aef", L"ABab", L"A1a3"/*within boundary*/, L"0x1234", L"0xab12", L"0X1234", L"AB12h", L"BC34H", L"1234u", L"1234ul", L"1234ULL" };
        String^ wordNegativeInput[] = { L"+123", L"1.23"/*floating number*/, L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"-133", L"1.2e+023", L"1.2e23"/*unsigned exponent*/, L"xyz", L"A1a3b"/*outside boundary of 4 digits*/, L"SIN(2)", L"123+-234", L"1234x", L"A0x1234", L"0xx1234", L"1234uu", L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexWordPasteExpression, wordNegativeInput);

        String^ bytePositiveInput[] = { L"13", L"13+6", L"1,4", L"2 3", L"1'2", L"1_2", L"5e-3"/*note: here is considered as E of hex*/, L"\r1", L"a", L"ab", L"A1"/*within boundary*/, L"0x12", L"0xab", L"0X12", L"A9h", L"B8H", L"12u", L"12ul", L"12ULL" };
        String^ byteNegativeInput[] = { L"+3", L"1.2"/*floating number*/, L"1''2", L"'12", L"12'", L"1__2", L"_12", L"12_", L"-3", L"1.1e+02", L"1.2e3"/*unsigned exponent*/, L"xz", L"A3a"/*outside boundary of 2 digits*/, L"SIN(2)", L"13+-23", L"12x", L"A0x1", L"0xx12", L"12uu", L"12ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerHexBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerHexBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerDecPasteExpressionTest()
    {
        String^ qwordPositiveInput[] = { L"123", L"+123", L"-133", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"1\"2", L"(123)+(456)", L"123+-234", L"123*-345", L"123*4*-3", L"123*+4*-3", L"9223372036854775807", L"-9223372036854775807"/*boundary condition: max/min allowed number*/, L"0n1234", L"0N1234", L"1234u", L"1234ul", L"1234ULL", L"2+2=", L"2+2=   " };
        String^ qwordNegativeInput[] = { L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"12345e-23", L"abcdef", L"xyz", L"ABab", L"e+234", L"9223372036854775809"/*boundary condition: greater than max allowed number 9223372036854775807*/, L"SIN(2)", L"-0n123", L"0nn1234", L"1234uu", L"1234ulll", L"2+2==", L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecQwordPasteExpression, qwordNegativeInput);

        String^ dwordPositiveInput[] = { L"123", L"+123", L"-133", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"1\"2", L"(123)+(456)", L"123+-234", L"123*-345", L"123*4*-3", L"123*+4*-3", L"2147483647", L"-2147483647"/*boundary condition: max/min allowed number*/, L"0n1234", L"0N1234", L"1234u", L"1234ul", L"1234ULL" };
        String^ dwordNegativeInput[] = { L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"12345e-23", L"abcdef", L"xyz", L"ABab", L"e+234", L"2147483649"/*boundary condition: greater than max allowed number 2147483647*/, L"SIN(2)", L"-0n123", L"0nn1234", L"1234uu", L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecDwordPasteExpression, dwordNegativeInput);

        String^ wordPositiveInput[] = { L"123", L"+123", L"-133", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"\f\n1+2\t\r\v\x85", L"1\"2", L"(123)+(456)", L"123+-234", L"123*-345", L"123*4*-3", L"123*+4*-3", L"32767", L"-32767"/*boundary condition: max/min allowed number*/, L"0n1234", L"0N1234", L"1234u", L"1234ul", L"1234ULL" };
        String^ wordNegativeInput[] = { L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"12345e-23", L"abcdef", L"xyz", L"ABab", L"e+234", L"32769"/*boundary condition: greater than max allowed number 32769*/, L"SIN(2)", L"-0n123", L"0nn1234", L"1234uu", L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecWordPasteExpression, wordNegativeInput);

        String^ bytePositiveInput[] = { L"13", L"+13", L"-13", L"13+46", L"13+-34", L"13*-3", L"3*4*-3", L"3*+4*-3", L"1,3", L"1 3", L"1'2'3", L"1_2_3", L"1\"2", L"127", L"-127"/*boundary condition: max/min allowed number*/, L"0n123", L"0N123", L"123u", L"123ul", L"123ULL" };
        String^ byteNegativeInput[] = { L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"15e-23", L"abcdef", L"xyz", L"ABab", L"e+24", L"129"/*boundary condition: greater than max allowed number 127*/, L"SIN(2)", L"-0n123", L"0nn1234", L"123uu", L"123ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerDecBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerDecBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerOctPasteExpressionTest()
    {
        String^ qwordPositiveInput[] = { L"123", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"1\"2", L"(123)+(456)", L"0t1234", L"0T1234", L"0o1234", L"0O1234", L"1234u", L"1234ul", L"1234ULL", L"2+2=", L"2+2=   " };
        String^ qwordNegativeInput[] = { L"+123", L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"-133", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"12345e-23", L"abcdef", L"xyz", L"ABab", L"e+234", L"12345678901234567890123"/*boundary condition: greater than max allowed digits 22*/, L"SIN(2)", L"123+-234", L"0ot1234", L"1234uu", L"1234ulll", L"2+2==", L"2=+2" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctQwordPasteExpression, qwordNegativeInput);

        String^ dwordPositiveInput[] = { L"123", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"\n\r1,234\n", L"\f\n1+2\t\r\v\x85", L"\n 1+\n2 ", L"1\"2", L"(123)+(456)", L"37777777777"/*boundary condition: max allowed number*/, L"0t1234", L"0T1234", L"0o1234", L"0O1234", L"1234u", L"1234ul", L"1234ULL" };
        String^ dwordNegativeInput[] = { L"+123", L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"-133", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"12345e-23", L"abcdef", L"xyz", L"ABab", L"e+234", L"377777777771"/*boundary condition: greater than max allowed number 37777777777*/, L"SIN(2)", L"123+-234", L"0ot1234", L"1234uu", L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctDwordPasteExpression, dwordNegativeInput);

        String^ wordPositiveInput[] = { L"123", L"123+456", L"1,234", L"1 2 3", L"1'2'3'4", L"1_2_3_4", L"\f\n1+2\t\r\v\x85", L"1\"2", L"(123)+(456)", L"177777"/*boundary condition: max allowed number*/, L"0t1234", L"0T1234", L"0o1234", L"0O1234", L"1234u", L"1234ul", L"1234ULL" };
        String^ wordNegativeInput[] = { L"+123", L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"-133", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"12345e-23", L"abcdef", L"xyz", L"ABab", L"e+234", L"1777771"/*boundary condition: greater than max allowed number 177777*/, L"SIN(2)", L"123+-234", L"0ot1234", L"1234uu", L"1234ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctWordPasteExpression, wordNegativeInput);

        String^ bytePositiveInput[] = { L"13", L"13+46", L"1,3", L"1 3", L"1'2'3", L"1_2_3", L"1\"2", L"377"/*boundary condition: max allowed number*/, L"0t123", L"0T123", L"0o123", L"0O123", L"123u", L"123ul", L"123ULL" };
        String^ byteNegativeInput[] = { L"+123", L"1.23", L"1''2", L"'123", L"123'", L"1__2", L"_123", L"123_", L"-13", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"15e-23", L"abcdef", L"xyz", L"ABab", L"e+24", L"477"/*boundary condition: greater than max allowed number 377*/, L"SIN(2)", L"123+-34", L"0ot123", L"123uu", L"123ulll" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerOctBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerOctBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateProgrammerBinPasteExpressionTest()
    {
        String^ qwordPositiveInput[] = { L"100", L"100+101", L"1,001", L"1 0 1", L"1'0'0'1", L"1_0_0_1", L"\n\r1,010\n", L"\f\n1+11\t\r\v\x85", L"\n 1+\n1 ", L"1\"1", L"(101)+(10)", L"0b1001", L"0B1111", L"0y1001", L"0Y1001", L"1100b", L"1101B", L"1111u", L"1111ul", L"1111ULL", L"1010101010101010101010101011110110100100101010101001010101001010"/*boundary condition: max allowed digits 64*/, L"1+10=", L"1+10=   " };
        String^ qwordNegativeInput[] = { L"+10101", L"1.01", L"1''0", L"'101", L"101'", L"1__0", L"_101", L"101_", L"-10101001", L"123", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"101010e-1010", L"abcdef", L"xyz", L"ABab", L"e+10101", L"b1001", L"10b01", L"0x10", L"1001x", L"1001h", L"0bb1111", L"1111uu", L"1111ulll", L"10101010101010101010101010111101101001001010101010010101010010100"/*boundary condition: greater than max allowed digits 64*/, L"SIN(01010)", L"10+-10101010101", L"1+10==", L"1=+10" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinQwordPasteExpression, qwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinQwordPasteExpression, qwordNegativeInput);

        String^ dwordPositiveInput[] = { L"100", L"100+101", L"1,001", L"1 0 1", L"1'0'0'1", L"1_0_0_1", L"\n\r1,010\n", L"\f\n1+11\t\r\v\x85", L"\n 1+\n1 ", L"1\"1", L"(101)+(10)", L"0b1001", L"0B1111", L"0y1001", L"0Y1001", L"1100b", L"1101B", L"1111u", L"1111ul", L"1111ULL", L"10101001001010101101010111111100"/*boundary condition: max allowed number*/ };
        String^ dwordNegativeInput[] = { L"+10101", L"1.01", L"1''0", L"'101", L"101'", L"1__0", L"_101", L"101_", L"-10101001", L"123", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"101010e-1010", L"abcdef", L"xyz", L"ABab", L"e+10101", L"b1001", L"10b01", L"0x10", L"1001x", L"1001h", L"0bb1111", L"1111uu", L"1111ulll", L"101010010010101011010101111111001"/*boundary condition: greater than max allowed digits 32*/, L"SIN(01010)", L"10+-10101010101" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinDwordPasteExpression, dwordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinDwordPasteExpression, dwordNegativeInput);

        String^ wordPositiveInput[] = { L"100", L"100+101", L"1,001", L"1 0 1", L"1'0'0'1", L"1_0_0_1", L"\n\r1,010\n", L"\f\n1+11\t\r\v\x85", L"\n 1+\n1 ", L"1\"1", L"(101)+(10)", L"0b1001", L"0B1111", L"0y1001", L"0Y1001", L"1100b", L"1101B", L"1111u", L"1111ul", L"1111ULL", L"1010101010010010"/*boundary condition: max allowed number*/ };
        String^ wordNegativeInput[] = { L"+10101", L"1.01", L"1''0", L"'101", L"101'", L"1__0", L"_101", L"101_", L"-10101001", L"123", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"101010e-1010", L"abcdef", L"xyz", L"ABab", L"e+10101", L"b1001", L"10b01", L"0x10", L"1001x", L"1001h", L"0bb1111", L"1111uu", L"1111ulll", L"10101010100100101"/*boundary condition: greater than max allowed digits 16*/, L"SIN(01010)", L"10+-10101010101" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinWordPasteExpression, wordPositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinWordPasteExpression, wordNegativeInput);

        String^ bytePositiveInput[] = { L"100", L"100+101", L"1,001", L"1 0 1", L"1'0'0'1", L"1_0_0_1", L"\n\r1,010\n", L"\n 1+\n1 ", L"1\"1", L"(101)+(10)", L"0b1001", L"0B1111", L"0y1001", L"0Y1001", L"1100b", L"1101B", L"1111u", L"1111ul", L"1111ULL", L"10100010"/*boundary condition: max allowed number*/ };
        String^ byteNegativeInput[] = { L"+10101", L"1.01", L"1''0", L"'101", L"101'", L"1__0", L"_101", L"101_", L"-10101001", L"123", L"1.2e23"/*unsigned exponent*/, L"1.2e+023", L"101010e-1010", L"abcdef", L"xyz", L"ABab", L"e+10101", L"b1001", L"10b01", L"0x10", L"1001x", L"1001h", L"0bb1111", L"1111uu", L"1111ulll", L"101000101"/*boundary condition: greater than max allowed digits 8*/, L"SIN(01010)", L"10+-1010101" };

        ASSERT_POSITIVE_TESTCASES(ValidateProgrammerBinBytePasteExpression, bytePositiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateProgrammerBinBytePasteExpression, byteNegativeInput);
    }

    void CopyPasteManagerTest::ValidateConverterPasteExpressionTest()
    {
        String^ positiveInput[] = { L"123", L"+123", L"-133", L"12345.", L"012.012", L"1,234", L"1 2 3", L"\n\r1,234\n", L"\f\n12\t\r\v\x85", L"1\"2", L"100=", L"100=   " };
        String^ negativeInput[] = { L"(123)+(456)", L"1.2e23"/*unsigned exponent*/, L"12345e-23", L"\n 1+\n2 ", L"123+456", L"abcdef", L"\n 1+\n2 ", L"xyz", L"ABab", L"e+234", L"12345678912345678"/*boundary condition: greater than 16 bits*/, L"SIN(2)", L"123+-234", L"100==", L"=100" };

        ASSERT_POSITIVE_TESTCASES(ValidateConverterPasteExpression, positiveInput);
        ASSERT_NEGATIVE_TESTCASES(ValidateConverterPasteExpression, negativeInput);
    }
}
