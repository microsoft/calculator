// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <CppUnitTest.h>
#include <cmath>

#include "CalcManager/CalculatorHistory.h"
#include "CalcManager/EpsilonEngine/EpsilonEngine.h"
#include "CalcManager/Header Files/EngineStrings.h"

using namespace CalculationManager;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace EpsilonEngineTests
{
    class ResourceProvider final : public IResourceProvider
    {
    public:
        ResourceProvider(wstring decimal = L".", wstring thousands = L",", wstring grouping = L"3;0")
            : m_decimal(std::move(decimal)), m_thousands(std::move(thousands)), m_grouping(std::move(grouping))
        {
        }

        wstring GetCEngineString(wstring_view id) override
        {
            if (id == L"sDecimal")
                return m_decimal;
            if (id == L"sThousand")
                return m_thousands;
            if (id == L"sGrouping")
                return m_grouping;
            if (id == to_wstring(IDS_DIVBYZERO))
                return L"divide by zero";
            if (id == to_wstring(IDS_OVERFLOW))
                return L"overflow";
            if (id == to_wstring(IDS_DOMAIN))
                return L"domain";
            if (id == to_wstring(IDS_UNDEFINED))
                return L"undefined";
            return wstring(id);
        }

    private:
        wstring m_decimal;
        wstring m_thousands;
        wstring m_grouping;
    };

    class Display final : public ICalcDisplay
    {
    public:
        void SetPrimaryDisplay(const wstring& text, bool error) override
        {
            primary = text;
            isError = error;
            ++primaryChanges;
        }

        void SetIsInError(bool error) override
        {
            isError = error;
        }

        void SetExpressionDisplay(
            shared_ptr<vector<pair<wstring, int>>> const& newTokens,
            shared_ptr<vector<shared_ptr<IExpressionCommand>>> const& newCommands) override
        {
            tokens = *newTokens;
            commandCount = newCommands->size();
            ++expressionChanges;
        }

        void SetParenthesisNumber(unsigned int count) override
        {
            parentheses = count;
        }

        void OnNoRightParenAdded() override
        {
            ++rejectedRightParentheses;
        }

        void MaxDigitsReached() override
        {
            ++maxDigits;
        }

        void BinaryOperatorReceived() override
        {
            ++binaryOperators;
        }

        void OnHistoryItemAdded(unsigned int index) override
        {
            historyIndexes.push_back(index);
        }

        void SetMemorizedNumbers(const vector<wstring>&) override {}
        void MemoryItemChanged(unsigned int) override {}

        void InputChanged() override
        {
            ++inputChanges;
        }

        wstring primary;
        vector<pair<wstring, int>> tokens;
        vector<unsigned int> historyIndexes;
        size_t commandCount = 0;
        unsigned int parentheses = 0;
        int primaryChanges = 0;
        int expressionChanges = 0;
        int rejectedRightParentheses = 0;
        int maxDigits = 0;
        int binaryOperators = 0;
        int inputChanges = 0;
        bool isError = false;
    };

    TEST_CLASS(EpsilonEngineTest)
    {
        TEST_METHOD_INITIALIZE(Initialize)
        {
            m_display = Display{};
            m_history = make_shared<CalculatorHistory>(20);
            m_engine = make_unique<EpsilonEngine>(&m_resources, &m_display, m_history);
            m_engine->SetPrecision(32);
        }

        void Send(initializer_list<Command> commands)
        {
            for (Command command : commands)
            {
                m_engine->ProcessCommand(command);
            }
        }

        void VerifyResult(const wchar_t* expected)
        {
            Assert::AreEqual(wstring(expected), m_engine->GetResult());
            Assert::IsFalse(m_display.isError, L"The engine unexpectedly entered an error state.");
        }

        void EnterDigits(const char* digits)
        {
            for (; *digits; ++digits)
            {
                m_engine->ProcessCommand(static_cast<Command>(static_cast<int>(Command::Command0) + *digits - '0'));
            }
        }

        void VerifyExpression(const wchar_t* expected)
        {
            wstring actual;
            for (const auto& token : m_display.tokens)
            {
                actual += token.first;
                VERIFY_ARE_EQUAL(-1, token.second);
            }
            VERIFY_ARE_EQUAL(wstring(expected), actual);
            VERIFY_ARE_EQUAL(static_cast<size_t>(0), m_display.commandCount);
        }

        static string ScaledPositiveDecimal(const wstring& value, size_t fractionalPlaces)
        {
            string integer;
            string fraction;
            bool afterDecimal = false;
            for (wchar_t character : value)
            {
                if (character == L'.')
                {
                    afterDecimal = true;
                }
                else if (character >= L'0' && character <= L'9')
                {
                    (afterDecimal ? fraction : integer).push_back(static_cast<char>(character));
                }
            }
            if (integer.empty())
            {
                integer = "0";
            }
            fraction.resize(fractionalPlaces, '0');
            string scaled = integer + fraction;
            size_t firstNonZero = scaled.find_first_not_of('0');
            return firstNonZero == string::npos ? "0" : scaled.substr(firstNonZero);
        }

        static bool IsWithinDecimalTolerance(const wstring& actual, const wchar_t* expected, size_t tolerancePlaces)
        {
            // Compare positive fixed-point decimal integers at two guard
            // places beyond the requested tolerance. No native floating
            // point participates in numerical regression checks.
            if (actual.empty() || actual.front() == L'-' || actual.find_first_of(L"eE") != wstring::npos)
            {
                return false;
            }
            size_t scale = tolerancePlaces + 2;
            string left = ScaledPositiveDecimal(actual, scale);
            string right = ScaledPositiveDecimal(expected, scale);
            size_t width = max(left.size(), right.size());
            left.insert(0, width - left.size(), '0');
            right.insert(0, width - right.size(), '0');
            if (left < right)
            {
                swap(left, right);
            }

            string difference(width, '0');
            int borrow = 0;
            for (size_t position = width; position-- > 0;)
            {
                int digit = (left[position] - '0') - (right[position] - '0') - borrow;
                if (digit < 0)
                {
                    digit += 10;
                    borrow = 1;
                }
                else
                {
                    borrow = 0;
                }
                difference[position] = static_cast<char>('0' + digit);
            }
            size_t firstNonZero = difference.find_first_not_of('0');
            difference = firstNonZero == string::npos ? "0" : difference.substr(firstNonZero);
            return difference.size() < 3 || (difference.size() == 3 && difference <= "100");
        }

        void VerifyIndependentReference(const wchar_t* expected)
        {
            wstring actual = m_engine->GetResult();
            wstring message = L"Actual: " + actual + L"\nExpected reference: " + expected;
            wstring reference(expected);
            const bool negative = !reference.empty() && reference.front() == L'-';
            Assert::AreEqual(negative, !actual.empty() && actual.front() == L'-', message.c_str());
            if (negative)
            {
                actual.erase(0, 1);
                reference.erase(0, 1);
            }
            Assert::IsTrue(IsWithinDecimalTolerance(actual, reference.c_str(), 30), message.c_str());
            VERIFY_IS_FALSE(m_display.isError);
        }

        TEST_METHOD(ConstructorHasNoDisplayReentrancy)
        {
            VERIFY_IS_TRUE(m_engine->IsInputEmpty());
            VERIFY_IS_FALSE(m_engine->IsEngineRecording());
            VERIFY_ARE_EQUAL(0, m_display.primaryChanges);
            VERIFY_ARE_EQUAL(0, m_display.expressionChanges);
            VERIFY_ARE_EQUAL(0, m_display.inputChanges);

            m_engine->Reset();
            VERIFY_IS_TRUE(m_display.primaryChanges > 0);
            VerifyResult(L"0");
        }

        TEST_METHOD(PrattPrecedenceAndLeftAssociativity)
        {
            Send({Command::Command2, Command::CommandADD, Command::Command3, Command::CommandMUL, Command::Command4, Command::CommandEQU});
            VerifyResult(L"14");

            m_engine->Reset();
            Send({Command::CommandOPENP, Command::Command2, Command::CommandADD, Command::Command3, Command::CommandCLOSEP,
                  Command::CommandMUL, Command::Command4, Command::CommandEQU});
            VerifyResult(L"20");

            m_engine->Reset();
            Send({Command::Command8, Command::CommandDIV, Command::Command4, Command::CommandDIV, Command::Command2, Command::CommandEQU});
            VerifyResult(L"1");

            m_engine->Reset();
            Send({Command::Command8, Command::CommandSUB, Command::Command3, Command::CommandSUB, Command::Command2, Command::CommandEQU});
            VerifyResult(L"3");
        }

        TEST_METHOD(NestedAndIncompleteInput)
        {
            Send({Command::CommandOPENP, Command::CommandOPENP, Command::Command2, Command::CommandADD, Command::Command3,
                  Command::CommandCLOSEP, Command::CommandMUL, Command::Command4, Command::CommandCLOSEP, Command::CommandEQU});
            VerifyResult(L"20");

            m_engine->Reset();
            Send({Command::CommandOPENP, Command::Command2, Command::CommandADD, Command::Command3, Command::CommandEQU});
            VerifyResult(L"5");
            VERIFY_ARE_EQUAL(0u, m_display.parentheses);

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::Command4, Command::CommandADD, Command::CommandEQU});
            VerifyResult(L"8");

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::Command2, Command::CommandADD, Command::Command3, Command::CommandMUL, Command::CommandEQU});
            VerifyResult(L"11");

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::CommandOPENP, Command::CommandEQU});
            VERIFY_IS_TRUE(m_display.isError);
        }

        TEST_METHOD(ImplicitMultiplicationAndOperatorReplacement)
        {
            Send(
                {Command::Command2, Command::CommandOPENP, Command::Command3, Command::CommandADD, Command::Command4,
                 Command::CommandCLOSEP, Command::CommandEQU});
            VerifyResult(L"14");

            m_engine->Reset();
            Send(
                {Command::CommandOPENP, Command::Command2, Command::CommandADD, Command::Command3, Command::CommandCLOSEP,
                 Command::Command4, Command::CommandEQU});
            VerifyResult(L"20");

            m_engine->Reset();
            Send({Command::Command8, Command::CommandADD, Command::CommandMUL, Command::Command2, Command::CommandEQU});
            VerifyResult(L"16");
        }

        TEST_METHOD(PrefixPlusAndCommittedPrefixResults)
        {
            Send({Command::Command1, Command::CommandADD, Command::CommandOPENP, Command::CommandADD,
                  Command::Command3, Command::CommandCLOSEP});
            VerifyResult(L"3");
            VerifyExpression(L"1 + (0 + 3)");

            m_engine->Reset();
            Send({Command::Command5, Command::Command0, Command::CommandADD, Command::Command2,
                  Command::Command0, Command::CommandREC, Command::CommandSUB});
            VerifyResult(L"50.05");
            VerifyExpression(L"50 + 1/(20) - ");

            m_engine->Reset();
            m_engine->ProcessCommand(Command::CommandSQRT);
            VerifyResult(L"0");
            VerifyExpression(L"\x221A(0)");
        }

        TEST_METHOD(PendingOperatorsAndEqualsUseTheDisplayedRetainedValue)
        {
            Send({Command::Command1, Command::CommandADD, Command::Command2, Command::CommandMUL});
            VerifyResult(L"2");
            Send({Command::Command3, Command::CommandEQU});
            VerifyResult(L"7");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandOPENP, Command::Command2, Command::CommandCLOSEP, Command::CommandADD});
            VerifyResult(L"4");
            Send({Command::CommandEQU});
            VerifyResult(L"8");
            VerifyExpression(L"2 \x00D7 (2) + 4=");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandADD, Command::Command3, Command::CommandMUL, Command::CommandEQU});
            VerifyResult(L"11");

            m_engine->Reset();
            Send({Command::Command1, Command::CommandADD, Command::CommandOPENP, Command::Command2,
                  Command::CommandADD, Command::Command3, Command::CommandCLOSEP});
            VerifyResult(L"5");
            Send({Command::CommandMUL, Command::Command2, Command::CommandEQU});
            VerifyResult(L"11");
        }

        TEST_METHOD(ExactAngleZerosAndPolesAreCertifiedWithoutTolerance)
        {
            Send({Command::CommandRAD, Command::CommandPI, Command::CommandTAN});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::CommandGRAD, Command::Command4, Command::Command0, Command::Command0, Command::CommandSIN});
            VerifyResult(L"0");
            Send({Command::CommandCLEAR, Command::Command4, Command::Command0, Command::Command0, Command::CommandCOS});
            VerifyResult(L"1");
            Send({Command::CommandCLEAR, Command::Command4, Command::Command0, Command::Command0, Command::CommandTAN});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command9, Command::Command0, Command::CommandTAN});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"divide by zero"), m_display.primary);
        }

        TEST_METHOD(UnaryGroupDisplayHasOnlyOneOuterPair)
        {
            Send({Command::CommandOPENP, Command::Command2, Command::CommandPNT, Command::Command2,
                  Command::Command5, Command::CommandSQRT, Command::CommandDIV, Command::Command1,
                  Command::CommandPNT, Command::Command5, Command::CommandCLOSEP, Command::CommandLOG});
            VerifyResult(L"0");
            VerifyExpression(L"log(\x221A(2.25) \x00F7 1.5)");
        }

        TEST_METHOD(DecimalScientificAndExactArithmetic)
        {
            Send({Command::Command0, Command::CommandPNT, Command::Command1, Command::CommandADD,
                  Command::Command0, Command::CommandPNT, Command::Command2, Command::CommandEQU});
            VerifyResult(L"0.3");

            m_engine->Reset();
            Send({Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command3, Command::CommandEQU});
            VerifyResult(L"0.001");

            m_engine->Reset();
            Send({Command::Command1, Command::CommandDIV, Command::Command3, Command::CommandEQU,
                  Command::CommandMUL, Command::Command3, Command::CommandEQU});
            VerifyResult(L"1");
        }

        TEST_METHOD(ExponentEntryRetainsEvaluatedValues)
        {
            Send({Command::Command9, Command::CommandSQRT, Command::CommandEXP});
            VerifyResult(L"3.e+");
            VerifyExpression(L"\x221A(9)");
            VERIFY_IS_FALSE(m_engine->IsInputEmpty());

            Send({Command::CommandADD, Command::Command2, Command::CommandEQU});
            VerifyResult(L"5");

            m_engine->Reset();
            Send({Command::Command9, Command::CommandSQRT, Command::CommandEXP, Command::Command2, Command::CommandEQU});
            VerifyResult(L"300");

            m_engine->Reset();
            Send({Command::Command1, Command::CommandPNT, Command::Command2, Command::Command3, Command::CommandEXP,
                  Command::Command1, Command::Command0});
            VerifyResult(L"1.23e+10");
            m_engine->ProcessCommand(Command::CommandSIGN);
            VerifyResult(L"1.23e-10");
            m_engine->ProcessCommand(Command::CommandBACK);
            VerifyResult(L"1.23e-1");
        }

        TEST_METHOD(ImmediateUnaryAndClosedGroup)
        {
            Send({Command::Command9, Command::CommandSQRT});
            VerifyResult(L"3");

            Send({Command::CommandSQR});
            VerifyResult(L"9");

            m_engine->Reset();
            Send({Command::CommandOPENP, Command::Command2, Command::CommandADD, Command::Command7, Command::CommandCLOSEP, Command::CommandSQRT});
            VerifyResult(L"3");

            m_engine->Reset();
            Send({Command::CommandSIGN, Command::Command3, Command::CommandSQR});
            VerifyResult(L"9");

            m_engine->Reset();
            Send({Command::Command4, Command::CommandREC, Command::CommandMUL, Command::Command8, Command::CommandEQU});
            VerifyResult(L"2");

            m_engine->Reset();
            Send({Command::Command9, Command::CommandSQRT, Command::Command4, Command::CommandEQU});
            VerifyResult(L"4");
        }

        TEST_METHOD(SquareRootRegressionValues)
        {
            Send({Command::Command0, Command::CommandSQRT});
            VerifyResult(L"0");
            m_engine->Reset();
            Send({Command::Command4, Command::CommandSQRT});
            VerifyResult(L"2");
            m_engine->Reset();
            Send({Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5, Command::CommandSQRT});
            VerifyResult(L"1.5");
            m_engine->Reset();
            Send({Command::Command2, Command::CommandSQRT});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"1.4142135623730950488016887242097", 0) == 0);
        }

        TEST_METHOD(IndependentHighPrecisionNumericalReferences)
        {
            // Independently generated with .NET BigInteger fixed-point
            // Newton/Taylor/atanh/Machin algorithms at 90 working digits.
            m_engine->SetPrecision(65);

            Send({Command::Command2, Command::CommandSQRT});
            VerifyIndependentReference(L"1.41421356237309504880168872420969807856967187537694807317667973799");
            m_engine->ProcessCommand(Command::CommandSQR);
            VerifyResult(L"2");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send(
                {Command::Command2, Command::Command5, Command::Command7, Command::CommandSQRT, Command::CommandSQRT, Command::CommandSQRT});
            VerifyIndependentReference(L"2.00097489763307733742202773513848814958553525561573435555265729634");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send({Command::CommandEuler});
            VerifyIndependentReference(L"2.71828182845904523536028747135266249775724709369995957496696762772");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send({Command::Command2, Command::CommandPOWE});
            VerifyIndependentReference(L"7.38905609893065022723042746057500781318031557055184732408712782252");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send({Command::Command1, Command::Command0, Command::CommandLN});
            VerifyIndependentReference(L"2.30258509299404568401799145468436420760110148862877297603332790096");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send({Command::CommandPI});
            VerifyIndependentReference(L"3.14159265358979323846264338327950288419716939937510582097494459230");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send({Command::CommandRAD, Command::Command1, Command::CommandSIN});
            VerifyIndependentReference(L"0.84147098480789650665250232163029899962256306079837106567275170999");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send({Command::CommandRAD, Command::Command1, Command::CommandCOS});
            VerifyIndependentReference(L"0.54030230586813971740093660744297660373231042061792222767009725538");

            m_engine->Reset();
            m_engine->SetPrecision(65);
            Send({Command::CommandRAD, Command::Command1, Command::CommandTAN});
            VerifyIndependentReference(L"1.55740772465490223050697480745836017308725077238152003838394660569");
        }

        TEST_METHOD(ConstantsLogsAndExponential)
        {
            Send({Command::CommandPI});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"3.141592653589793238462643383279", 0) == 0);

            m_engine->Reset();
            Send({Command::CommandEuler});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"2.718281828459045235360287471352", 0) == 0);

            m_engine->Reset();
            Send({Command::Command1, Command::CommandLN});
            VerifyResult(L"0");
            m_engine->Reset();
            Send({Command::Command1, Command::Command0, Command::Command0, Command::CommandLOG});
            VerifyResult(L"2");
            m_engine->Reset();
            Send({Command::Command0, Command::CommandPOWE});
            VerifyResult(L"1");
        }

        TEST_METHOD(BoundedClassificationNeverCertifiesApproximateZero)
        {
            Send({Command::Command2, Command::CommandSUB, Command::Command2, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command4, Command::CommandSUB, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command1, Command::CommandADD, Command::CommandSIGN, Command::Command1, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command4, Command::CommandSQRT, Command::CommandSUB, Command::Command2, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandPNT, Command::Command2, Command::Command5, Command::CommandSQRT,
                  Command::CommandSUB, Command::Command1, Command::CommandPNT, Command::Command5, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command1, Command::Command0, Command::Command0, Command::Command0,
                  Command::CommandSIGN, Command::CommandPOWE});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"overflow"), m_engine->GetResult());
        }

        TEST_METHOD(SoundExactFactPropagation)
        {
            Send({Command::Command0, Command::CommandPNT, Command::Command1, Command::CommandADD,
                  Command::Command0, Command::CommandPNT, Command::Command2, Command::CommandSUB,
                  Command::Command0, Command::CommandPNT, Command::Command3, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandADD, Command::Command3,
                  Command::CommandSUB, Command::Command5, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command1, Command::CommandDIV, Command::Command3, Command::CommandMUL,
                  Command::Command3, Command::CommandSUB, Command::Command1, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandSQRT, Command::CommandSQR,
                  Command::CommandSUB, Command::Command2, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command1, Command::Command0, Command::Command0, Command::CommandLOG,
                  Command::CommandSUB, Command::Command2, Command::CommandEQU});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::CommandRAD, Command::CommandPI, Command::CommandSIN});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command2,
                  Command::Command5, Command::Command6, Command::CommandEQU, Command::CommandLN});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_IS_FALSE(m_engine->GetResult().empty());
            VERIFY_ARE_EQUAL(L'-', m_engine->GetResult().front());
        }

        TEST_METHOD(AngleModeIsPartOfExactRelationMetadata)
        {
            Send({Command::CommandDEG, Command::Command1, Command::CommandSIN, Command::CommandSUB,
                  Command::CommandRAD, Command::Command1, Command::CommandSIN, Command::CommandEQU});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_ARE_NOT_EQUAL(wstring(L"0"), m_engine->GetResult());
        }

        TEST_METHOD(AngleModesAndTrigonometry)
        {
            Send({Command::Command1, Command::CommandSIN});
            VerifyResult(L"0.017452406437283512819418978516316");

            m_engine->Reset();
            Send({Command::Command3, Command::Command0, Command::CommandSIN});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"0.5", 0) == 0);

            m_engine->Reset();
            Send({Command::CommandRAD, Command::CommandPI, Command::CommandDIV, Command::Command2, Command::CommandEQU, Command::CommandSIN});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"1", 0) == 0);

            m_engine->Reset();
            Send({Command::CommandGRAD, Command::Command1, Command::Command0, Command::Command0, Command::CommandSIN});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"1", 0) == 0);

            m_engine->Reset();
            Send({Command::Command6, Command::Command0, Command::CommandCOS});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"0.5", 0) == 0);

            m_engine->Reset();
            Send({Command::Command4, Command::Command5, Command::CommandTAN});
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"1", 0) == 0);
        }

        TEST_METHOD(LargePiMultiplesHaveExactTrigonometricResults)
        {
            for (const char* multiplier : {"2", "10000000000000000000000", "10000000000000000000001"})
            {
                for (Command function : {Command::CommandSIN, Command::CommandCOS, Command::CommandTAN})
                {
                    m_engine->Reset();
                    EnterDigits(multiplier);
                    Send({Command::CommandMUL, Command::CommandPI, Command::CommandEQU});
                    const wstring product = m_engine->GetResult();
                    Send({Command::CommandRAD});
                    VerifyResult(product.c_str());
                    Send({function});
                    const bool odd = string(multiplier).back() == '1';
                    VerifyResult(function == Command::CommandCOS ? (odd ? L"-1" : L"1") : L"0");
                }
            }
        }

        TEST_METHOD(LargeHalfPiMultiplesPreserveQuadrantsAndPoles)
        {
            for (bool negative : {false, true})
            {
                for (Command function : {Command::CommandSIN, Command::CommandCOS, Command::CommandTAN})
                {
                    m_engine->Reset();
                    EnterDigits("10000000000000000000001");
                    Send({Command::CommandMUL, Command::CommandPI, Command::CommandDIV, Command::Command2, Command::CommandEQU});
                    if (negative)
                        Send({Command::CommandSIGN});
                    Send({Command::CommandRAD, function});
                    if (function == Command::CommandTAN)
                    {
                        VERIFY_IS_TRUE(m_display.isError);
                        VERIFY_ARE_EQUAL(wstring(L"divide by zero"), m_engine->GetResult());
                    }
                    else
                    {
                        VerifyResult(function == Command::CommandCOS ? L"0" : negative ? L"-1" : L"1");
                    }
                }
            }
        }

        TEST_METHOD(ExactAngleCertificatesSurviveArithmeticAndExponentEntry)
        {
            Send({Command::Command2, Command::CommandMUL, Command::Command3, Command::CommandEQU,
                  Command::CommandMUL, Command::CommandPI, Command::CommandEQU, Command::CommandRAD, Command::CommandSIN});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandADD, Command::Command3, Command::CommandEQU,
                  Command::CommandMUL, Command::CommandPI, Command::CommandDIV, Command::Command2,
                  Command::CommandEQU, Command::CommandRAD, Command::CommandSIN});
            VerifyResult(L"1");

            m_engine->Reset();
            Send({Command::CommandPI, Command::CommandADD, Command::CommandPI, Command::CommandEQU,
                  Command::CommandRAD, Command::CommandSIN});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandREC, Command::CommandMUL, Command::CommandPI,
                  Command::CommandEQU, Command::CommandRAD, Command::CommandCOS});
            VerifyResult(L"0");

            m_engine->Reset();
            Send({Command::CommandPI, Command::CommandEXP, Command::Command2, Command::Command2,
                  Command::CommandEQU, Command::CommandRAD, Command::CommandSIN});
            VerifyResult(L"0");
        }

        TEST_METHOD(LargeDegreeAndGradianQuadrantsRemainExact)
        {
            EnterDigits("900000000000000000000000");
            Send({Command::CommandSIN});
            VerifyResult(L"0");

            m_engine->Reset();
            EnterDigits("900000000000000000000090");
            Send({Command::CommandSIN});
            VerifyResult(L"1");

            m_engine->Reset();
            EnterDigits("1000000000000000000000100");
            Send({Command::CommandGRAD, Command::CommandCOS});
            VerifyResult(L"0");
        }

        TEST_METHOD(LargeNonQuadrantArgumentsAreNotRejected)
        {
            for (Command function : {Command::CommandSIN, Command::CommandCOS, Command::CommandTAN})
            {
                for (bool negative : {false, true})
                {
                    m_engine->Reset();
                    EnterDigits("1000001");
                    if (negative)
                        Send({Command::CommandSIGN});
                    Send({Command::CommandRAD, function});
                    VERIFY_IS_FALSE(m_display.isError);
                    const double argument = negative ? -1000001.0 : 1000001.0;
                    const double expected = function == Command::CommandSIN ? std::sin(argument)
                        : function == Command::CommandCOS ? std::cos(argument) : std::tan(argument);
                    VERIFY_IS_TRUE(std::abs(std::stod(m_engine->GetResult()) - expected) < 1e-12);
                }
            }
        }

        TEST_METHOD(NearbyPiMultiplesAndTinyAnglesAreNotRoundedToZero)
        {
            EnterDigits("10000000000000000000000");
            Send({Command::CommandMUL, Command::CommandPI, Command::CommandADD,
                  Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command2, Command::Command0,
                  Command::CommandEQU, Command::CommandRAD, Command::CommandSIN});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_IS_TRUE(std::abs(std::stod(m_engine->GetResult()) / 1e-20 - 1) < 1e-12);

            m_engine->Reset();
            Send({Command::Command1, Command::CommandEXP, Command::CommandSIGN, Command::Command2, Command::Command0,
                  Command::CommandRAD, Command::CommandSIN});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_IS_TRUE(std::abs(std::stod(m_engine->GetResult()) / 1e-20 - 1) < 1e-12);
        }

        TEST_METHOD(VeryLargeAnglesMatchIndependentDecimalReferences)
        {
            // Decimal arithmetic at 350 digits, Machin's pi formula, and Taylor
            // series after reduction modulo 2*pi; references are independent of Epsilon.
            struct Reference
            {
                const char* exponent;
                const wchar_t* sine;
                const wchar_t* cosine;
                const wchar_t* tangent;
            };
            for (const auto& reference : {
                Reference{"22", L"-0.852200849767188801772705893753029368261762150",
                    L"0.523214785395138945497594473384709492140919972",
                    L"-1.628778225606898878549375936939548513545151168"},
                Reference{"256", L"0.564062222598159253602928076382852673247697975",
                    L"-0.825732286541845619413233527791646781657280101",
                    L"-0.683105446876061183364961721424452529120886419"}})
            {
                for (Command function : {Command::CommandSIN, Command::CommandCOS, Command::CommandTAN})
                {
                    m_engine->Reset();
                    Send({Command::Command1, Command::CommandEXP});
                    const bool squareInput = string(reference.exponent) == "256";
                    EnterDigits(squareInput ? "128" : reference.exponent);
                    if (squareInput)
                        Send({Command::CommandSQR});
                    Send({Command::CommandRAD, function});
                    VerifyIndependentReference(function == Command::CommandSIN ? reference.sine
                        : function == Command::CommandCOS ? reference.cosine : reference.tangent);
                }
            }
        }

        TEST_METHOD(AngleCertificatesAreNotReusedForDifferentValuesOrUnits)
        {
            Send({Command::CommandPI, Command::CommandSIN});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_IS_TRUE(std::abs(std::stod(m_engine->GetResult()) - std::sin(std::acos(-1.0) * std::acos(-1.0) / 180)) < 1e-12);

            m_engine->Reset();
            Send({Command::CommandPI, Command::CommandSQR, Command::CommandRAD, Command::CommandSIN});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_IS_TRUE(std::abs(std::stod(m_engine->GetResult()) - std::sin(std::acos(-1.0) * std::acos(-1.0))) < 1e-12);

            m_engine->Reset();
            Send({Command::CommandPI, Command::CommandREC, Command::CommandRAD, Command::CommandSIN});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_IS_TRUE(std::abs(std::stod(m_engine->GetResult()) - std::sin(1 / std::acos(-1.0))) < 1e-12);

            m_engine->Reset();
            Send({Command::CommandPI, Command::CommandADD, Command::Command1, Command::CommandEQU,
                  Command::CommandRAD, Command::CommandSIN});
            VERIFY_IS_FALSE(m_display.isError);
            VERIFY_IS_TRUE(std::abs(std::stod(m_engine->GetResult()) + std::sin(1.0)) < 1e-12);
        }

        TEST_METHOD(ExponentialArgumentStillHasAResourceLimit)
        {
            EnterDigits("1001");
            Send({Command::CommandPOWE});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"overflow"), m_engine->GetResult());
        }

        TEST_METHOD(ErrorsAndRecovery)
        {
            Send({Command::Command1, Command::CommandDIV, Command::Command0, Command::CommandEQU});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"divide by zero"), m_engine->GetResult());
            VerifyExpression(L"1 \x00f7 ");

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::Command0, Command::CommandDIV, Command::Command0, Command::CommandEQU});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"undefined"), m_engine->GetResult());
            VerifyExpression(L"0 \x00f7 ");

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::CommandSIGN, Command::Command1, Command::CommandSQRT});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"domain"), m_engine->GetResult());
            VerifyExpression(L"\x221A(-1)");

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::Command0, Command::CommandLN});
            VERIFY_IS_TRUE(m_display.isError);

            m_engine->ProcessCommand(Command::CommandCLEAR);
            for (int i = 0; i < 33; ++i)
                m_engine->ProcessCommand(Command::CommandOPENP);
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"overflow"), m_engine->GetResult());

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::Command2, Command::CommandADD, Command::Command2, Command::CommandEQU});
            VerifyResult(L"4");
        }

        TEST_METHOD(InputMagnitudeAndOperationBudgets)
        {
            Send({Command::Command1, Command::CommandEXP, Command::Command9, Command::Command9, Command::Command9, Command::CommandEQU});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"overflow"), m_engine->GetResult());

            m_engine->ProcessCommand(Command::CommandCLEAR);
            Send({Command::Command1, Command::CommandEXP, Command::Command2, Command::Command5, Command::Command6,
                  Command::CommandMUL, Command::Command1, Command::CommandEXP, Command::Command2, Command::Command5, Command::Command6,
                  Command::CommandMUL, Command::Command1, Command::CommandEXP, Command::Command2, Command::Command5, Command::Command6,
                  Command::CommandEQU});
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"overflow"), m_engine->GetResult());

            m_engine->ProcessCommand(Command::CommandCLEAR);
            for (int operation = 0; operation < 130 && !m_display.isError; ++operation)
            {
                m_engine->ProcessCommand(Command::Command1);
                m_engine->ProcessCommand(Command::CommandADD);
            }
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"overflow"), m_engine->GetResult());

            m_engine->ProcessCommand(Command::CommandCENTR);
            m_engine->ProcessCommand(Command::Command1);
            for (int operation = 0; operation < 40 && !m_display.isError; ++operation)
            {
                m_engine->ProcessCommand(Command::CommandSQR);
            }
            VERIFY_IS_TRUE(m_display.isError);
            VERIFY_ARE_EQUAL(wstring(L"overflow"), m_engine->GetResult());

            m_engine->ProcessCommand(Command::CommandCENTR);
            Send({Command::Command3, Command::CommandEQU});
            VerifyResult(L"3");
        }

        TEST_METHOD(BackspaceClearEntryAndClear)
        {
            Send({Command::Command1, Command::Command2, Command::CommandBACK, Command::Command3});
            VerifyResult(L"13");
            m_engine->ProcessCommand(Command::CommandADD);
            Send({Command::Command9, Command::CommandCENTR, Command::Command4, Command::CommandEQU});
            VerifyResult(L"17");
            m_engine->ProcessCommand(Command::CommandCLEAR);
            VerifyResult(L"0");
            VERIFY_ARE_EQUAL(0u, m_display.parentheses);
        }

        TEST_METHOD(ClearEntryExposesFullClearAndRemovesCurrentOperand)
        {
            VERIFY_IS_TRUE(m_engine->IsInputEmpty());

            Send({Command::Command1, Command::Command2, Command::CommandADD, Command::Command3,
                  Command::Command4, Command::CommandCENTR});
            VerifyResult(L"0");
            VerifyExpression(L"12 + ");
            VERIFY_IS_TRUE(m_engine->IsInputEmpty());
            Send({Command::Command5, Command::CommandEQU});
            VerifyResult(L"17");

            m_engine->ProcessCommand(Command::CommandCLEAR);
            m_engine->ProcessCommand(Command::CommandPI);
            VERIFY_IS_FALSE(m_engine->IsInputEmpty());

            m_engine->ProcessCommand(Command::CommandCENTR);
            VerifyResult(L"0");
            VerifyExpression(L"");
            VERIFY_IS_TRUE(m_engine->IsInputEmpty());

            m_engine->ProcessCommand(Command::CommandEuler);
            VERIFY_IS_TRUE(m_engine->GetResult().rfind(L"2.718281828459045235360287471352", 0) == 0);
            VERIFY_IS_FALSE(m_engine->IsInputEmpty());

            m_engine->Reset();
            Send({Command::CommandOPENP, Command::Command1, Command::Command2,
                  Command::CommandADD, Command::Command3});
            VERIFY_IS_FALSE(m_engine->IsInputEmpty());
            m_engine->ProcessCommand(Command::CommandCENTR);
            VerifyResult(L"0");
            VerifyExpression(L"(12 + ");
            VERIFY_IS_TRUE(m_engine->IsInputEmpty());

            m_engine->ProcessCommand(Command::CommandCLEAR);
            VERIFY_IS_TRUE(m_engine->IsInputEmpty());
            VERIFY_ARE_EQUAL(0u, m_display.parentheses);
            Send({Command::Command2, Command::CommandADD, Command::Command3, Command::CommandEQU});
            VerifyResult(L"5");
        }

        TEST_METHOD(ContinuedAndRepeatedEquals)
        {
            Send({Command::Command2, Command::CommandADD, Command::Command3, Command::CommandEQU});
            VerifyResult(L"5");
            size_t historyCount = m_history->GetHistory().size();
            m_engine->ProcessCommand(Command::CommandEQU);
            VerifyResult(L"5");
            VERIFY_ARE_EQUAL(historyCount, m_history->GetHistory().size());

            Send({Command::CommandMUL, Command::Command4, Command::CommandEQU});
            VerifyResult(L"20");
        }

        TEST_METHOD(UnsupportedCommandPreservesInput)
        {
            Send({Command::Command1, Command::Command2});
            wstring before = m_engine->GetResult();
            Assert::ExpectException<invalid_argument>([&]() { m_engine->ProcessCommand(Command::CommandFAC); });
            VERIFY_ARE_EQUAL(before, m_engine->GetResult());
            VERIFY_IS_TRUE(EpsilonEngine::IsCommandSupported(Command::CommandSQRT));
            VERIFY_IS_FALSE(EpsilonEngine::IsCommandSupported(Command::CommandFAC));
            VERIFY_ARE_EQUAL(0, m_display.maxDigits);
        }

        TEST_METHOD(AppendOnlyHistoryAndCallbacks)
        {
            Send({Command::Command2, Command::CommandADD, Command::Command3, Command::CommandEQU});
            Send({Command::CommandMUL, Command::Command4, Command::CommandEQU});
            VERIFY_ARE_EQUAL(static_cast<size_t>(2), m_history->GetHistory().size());
            VERIFY_ARE_EQUAL(static_cast<size_t>(2), m_display.historyIndexes.size());
            VERIFY_ARE_EQUAL(0u, m_display.historyIndexes[0]);
            VERIFY_ARE_EQUAL(1u, m_display.historyIndexes[1]);
            for (const auto& item : m_history->GetHistory())
            {
                VERIFY_IS_TRUE(item->historyItemVector.spCommands->empty());
                VERIFY_ARE_EQUAL(-1, item->historyItemVector.spTokens->front().second);
            }
            VERIFY_IS_TRUE(m_display.primaryChanges > 0);
            VERIFY_IS_TRUE(m_display.expressionChanges > 0);
            VERIFY_IS_TRUE(m_display.inputChanges > 0);
            VERIFY_IS_TRUE(m_display.binaryOperators > 0);
            VERIFY_ARE_EQUAL(static_cast<size_t>(0), m_display.commandCount);
        }

        TEST_METHOD(ExpressionCallbacksHideEditableOperands)
        {
            Send({Command::Command1, Command::Command2, Command::Command3, Command::CommandPNT,
                  Command::Command4, Command::Command5, Command::Command6});
            VerifyResult(L"123.456");
            VerifyExpression(L"");

            m_engine->Reset();
            Send({Command::Command2, Command::CommandADD, Command::Command3});
            VerifyResult(L"3");
            VerifyExpression(L"2 + ");
            m_engine->ProcessCommand(Command::CommandEQU);
            VerifyResult(L"5");
            VerifyExpression(L"2 + 3=");
        }

        TEST_METHOD(SignificantDigitsAreRoundedOnce)
        {
            m_engine->SetPrecision(2);
            Send({Command::Command9, Command::Command9, Command::Command4, Command::CommandPNT,
                  Command::Command9, Command::Command9, Command::CommandEQU});
            VerifyResult(L"9.9e+2");

            m_engine->Reset();
            m_engine->SetPrecision(32);
            for (int digit = 0; digit < 32; ++digit)
            {
                m_engine->ProcessCommand(Command::Command9);
            }
            Send({Command::Command4, Command::CommandPNT, Command::Command9, Command::Command9, Command::CommandEQU});
            VerifyResult(L"9.9999999999999999999999999999999e+32");
        }

        TEST_METHOD(FormatToggleAndPrecisionBound)
        {
            m_engine->ProcessCommand(Command::CommandFE);
            VerifyResult(L"0.e+0");
            m_engine->ProcessCommand(Command::CommandFE);
            Send({Command::Command1, Command::Command0, Command::Command0, Command::Command0, Command::CommandFE});
            VerifyResult(L"1.e+3");
            m_engine->SetPrecision(10000);
            m_engine->ProcessCommand(Command::CommandFE);
            VerifyResult(L"1,000");

            m_engine->Reset();
            Send({Command::CommandRAD, Command::CommandFE, Command::CommandPI, Command::CommandCOS});
            VerifyResult(L"-1.e+0");
        }

        TEST_METHOD(NegativeScientificRoundingNormalizesTheMantissa)
        {
            m_engine->SetPrecision(2);
            Send({Command::Command9, Command::Command9, Command::Command9, Command::CommandPNT,
                  Command::Command9, Command::Command9, Command::CommandSIGN, Command::CommandEQU});
            VerifyResult(L"-1.e+3");
        }

        TEST_METHOD(LiveGroupingNeverChangesEditableOrRetainedNumbers)
        {
            Send({Command::Command1, Command::Command2, Command::Command3, Command::Command4,
                  Command::Command5, Command::Command6, Command::Command7});
            VerifyResult(L"1,234,567");
            Send({Command::CommandBACK, Command::CommandPNT, Command::Command9});
            VerifyResult(L"123,456.9");
            Send({Command::CommandADD, Command::Command1, Command::CommandEQU});
            VerifyResult(L"123,457.9");

            m_engine->Reset();
            Send({Command::Command1, Command::Command0, Command::Command0, Command::Command0, Command::CommandEQU,
                  Command::CommandEXP, Command::Command2});
            VerifyResult(L"1000.e+2");
            Send({Command::CommandEQU});
            VerifyResult(L"100,000");
        }

        TEST_METHOD(LocaleGroupingAndRetainedExactValue)
        {
            ResourceProvider indianLocale(L",", L".", L"3;2;0");
            Display localizedDisplay;
            auto localizedHistory = make_shared<CalculatorHistory>(20);
            EpsilonEngine localizedEngine(&indianLocale, &localizedDisplay, localizedHistory);

            auto sendLocalized = [&](initializer_list<Command> commands) {
                for (Command command : commands)
                {
                    localizedEngine.ProcessCommand(command);
                }
            };

            sendLocalized(
                {Command::Command1, Command::Command2, Command::Command3, Command::Command4, Command::Command5, Command::Command6,
                 Command::Command7, Command::CommandPNT, Command::Command8, Command::Command9});
            VERIFY_ARE_EQUAL(wstring(L"12.34.567,89"), localizedEngine.GetResult());
            sendLocalized({Command::CommandEQU});
            VERIFY_ARE_EQUAL(wstring(L"12.34.567,89"), localizedEngine.GetResult());
            VERIFY_ARE_EQUAL(L',', localizedEngine.DecimalSeparator());

            localizedEngine.Reset();
            localizedEngine.SetPrecision(5);
            sendLocalized({Command::Command1, Command::CommandDIV, Command::Command3, Command::CommandEQU});
            VERIFY_ARE_EQUAL(wstring(L"0,33333"), localizedEngine.GetResult());

            sendLocalized({Command::CommandMUL, Command::Command3, Command::CommandEQU});
            VERIFY_ARE_EQUAL(wstring(L"1"), localizedEngine.GetResult());
            VERIFY_IS_FALSE(localizedDisplay.isError);
        }

        TEST_METHOD(TerminalLocaleGroupingRepeats)
        {
            Send({Command::Command1, Command::Command2, Command::Command3, Command::Command4, Command::Command5,
                  Command::Command6, Command::Command7, Command::Command8, Command::Command9, Command::Command0,
                  Command::Command1, Command::Command2, Command::Command3, Command::CommandEQU});
            VerifyResult(L"1,234,567,890,123");

            ResourceProvider indianLocale(L".", L",", L"3;2;0");
            Display localizedDisplay;
            auto localizedHistory = make_shared<CalculatorHistory>(20);
            EpsilonEngine localizedEngine(&indianLocale, &localizedDisplay, localizedHistory);
            for (Command command : {Command::Command1, Command::Command2, Command::Command3, Command::Command4,
                                    Command::Command5, Command::Command6, Command::Command7, Command::Command8,
                                    Command::Command9, Command::Command0, Command::Command1, Command::Command2,
                                    Command::Command3, Command::CommandEQU})
            {
                localizedEngine.ProcessCommand(command);
            }
            VERIFY_ARE_EQUAL(wstring(L"12,34,56,78,90,123"), localizedEngine.GetResult());
            VERIFY_IS_FALSE(localizedDisplay.isError);
        }

    private:
        ResourceProvider m_resources;
        Display m_display;
        shared_ptr<CalculatorHistory> m_history;
        unique_ptr<EpsilonEngine> m_engine;
    };
}
