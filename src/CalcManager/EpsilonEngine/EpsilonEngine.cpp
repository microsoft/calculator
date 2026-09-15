// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "../pch.h"
#include "EpsilonEngine.h"

#include "../CalculatorHistory.h"
#include "../CalculatorResource.h"
#include "../Header Files/EngineStrings.h"
#include "../Header Files/ICalcDisplay.h"
#include "../Ratpack/CalcErr.h"
#include "../Epsilon/chars.hpp"
#include "../Epsilon/r.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <variant>

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::vector;
using std::wstring;

namespace CalculationManager
{
    namespace
    {
        using Container = epx::default_container_type;
        using Real = epx::r<Container>;
        using Integer = epx::z<Container>;

        constexpr size_t MaxInputCharacters = 256;
        constexpr size_t MaxTokens = 256;
        constexpr unsigned MaxParseDepth = 32;
        constexpr unsigned MaxOperations = 128;
        constexpr int MaxInputExponent = 256;
        constexpr int MaxDisplayPrecision = 100;
        constexpr int ClassificationPrecision = 640;
        constexpr unsigned MaxFractionalPlaces = 370;
        constexpr size_t MaxIntegerLimbs = 54; // Roughly 520 decimal digits.

        class ParseError : public std::runtime_error
        {
        public:
            explicit ParseError(const char* message) : runtime_error(message) {}
        };

        class ResourceLimitError : public std::runtime_error
        {
        public:
            explicit ResourceLimitError(const char* message) : runtime_error(message) {}
        };

        class IndefiniteError : public std::runtime_error
        {
        public:
            IndefiniteError() : runtime_error("indefinite result") {}
        };

        struct SourceSpan
        {
            size_t begin;
            size_t end;
        };

        enum class ZeroProof
        {
            ProvenZero,
            ProvenNonZero,
            Unknown
        };

        struct ExactMultiple
        {
            Integer numerator;
            Integer denominator;
            bool timesPi;
        };

        struct ExactValueProof
        {
            ZeroProof zero = ZeroProof::Unknown;
            bool one = false;
            string identity;
            bool positive = false;
            std::optional<ExactMultiple> multiple;
        };

        struct FactorCancellationProof
        {
            string factorIdentity;
            ExactValueProof result;
        };

        struct ValueFacts
        {
            ZeroProof zero = ZeroProof::Unknown;
            bool one = false;
            string identity;
            bool positive = false;
            std::optional<FactorCancellationProof> multiplyCancellation;
            std::optional<ExactValueProof> squareResult;
            std::optional<ExactMultiple> multiple;
        };

        struct NumberToken
        {
            shared_ptr<Real> value;
            string lexeme;
            wstring display;
            SourceSpan span;
            unsigned lazyDepth;
            unsigned operationCount;
            ValueFacts facts;
        };
        struct PlusToken
        {
            SourceSpan span;
        };
        struct MinusToken
        {
            SourceSpan span;
        };
        struct MultiplyToken
        {
            SourceSpan span;
        };
        struct DivideToken
        {
            SourceSpan span;
        };
        struct LeftParenToken
        {
            SourceSpan span;
        };
        struct RightParenToken
        {
            SourceSpan span;
        };

        using Token = std::variant<NumberToken, PlusToken, MinusToken, MultiplyToken, DivideToken, LeftParenToken, RightParenToken>;

        struct ParseResult
        {
            Real value;
            unsigned lazyDepth;
            unsigned operationCount;
            ValueFacts facts;
        };

        int parseBoundedExponent(string_view text);

        string negateIdentity(string identity)
        {
            if (identity.empty())
            {
                return {};
            }
            if (identity.front() == '+')
            {
                identity.front() = '-';
                return identity;
            }
            if (identity.front() == '-')
            {
                identity.front() = '+';
                return identity;
            }
            constexpr string_view prefix = "neg(";
            if (identity.starts_with(prefix) && identity.back() == ')')
            {
                return identity.substr(prefix.size(), identity.size() - prefix.size() - 1);
            }
            return "neg(" + identity + ")";
        }

        bool areOppositeIdentities(const string& left, const string& right)
        {
            return !left.empty() && !right.empty() && left == negateIdentity(right);
        }

        ExactValueProof exactValueProof(const ValueFacts& facts)
        {
            return {facts.zero, facts.one, facts.identity, facts.positive, facts.multiple};
        }

        ValueFacts valueFacts(const ExactValueProof& proof)
        {
            return {proof.zero, proof.one, proof.identity, proof.positive, {}, {}, proof.multiple};
        }

        std::optional<int> powerOfTenExponent(string_view identity)
        {
            if (!identity.starts_with("+1e") || identity.size() == 3)
            {
                return std::nullopt;
            }
            bool negative = false;
            size_t position = 3;
            if (identity[position] == '+' || identity[position] == '-')
            {
                negative = identity[position] == '-';
                ++position;
            }
            if (position == identity.size())
            {
                return std::nullopt;
            }
            int exponent = 0;
            for (; position < identity.size(); ++position)
            {
                char character = identity[position];
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                exponent = exponent * 10 + (character - '0');
                if (exponent > MaxInputExponent)
                {
                    return std::nullopt;
                }
            }
            return negative ? -exponent : exponent;
        }

        ValueFacts integerFacts(int value)
        {
            if (value == 0)
            {
                return {ZeroProof::ProvenZero, false, "0", false};
            }
            string identity = value < 0 ? "-" : "+";
            identity += std::to_string(std::abs(value));
            identity += "e0";
            return {ZeroProof::ProvenNonZero, value == 1, std::move(identity), value > 0};
        }

        struct ExactDecimalAtom
        {
            int64_t coefficient;
            int exponent;
        };

        std::optional<ExactDecimalAtom> exactDecimalAtom(string_view identity)
        {
            if (identity.size() < 4 || (identity.front() != '+' && identity.front() != '-'))
            {
                return std::nullopt;
            }
            size_t exponentMarker = identity.find('e', 1);
            if (exponentMarker == string_view::npos || exponentMarker == 1)
            {
                return std::nullopt;
            }

            int64_t coefficient = 0;
            for (char character : identity.substr(1, exponentMarker - 1))
            {
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                int digit = character - '0';
                if (coefficient > (std::numeric_limits<int64_t>::max() - digit) / 10)
                {
                    return std::nullopt;
                }
                coefficient = coefficient * 10 + digit;
            }
            if (identity.front() == '-')
            {
                coefficient = -coefficient;
            }

            int exponent = 0;
            bool negativeExponent = false;
            size_t position = exponentMarker + 1;
            if (position < identity.size() && (identity[position] == '+' || identity[position] == '-'))
            {
                negativeExponent = identity[position] == '-';
                ++position;
            }
            if (position == identity.size())
            {
                return std::nullopt;
            }
            for (; position < identity.size(); ++position)
            {
                char character = identity[position];
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                exponent = exponent * 10 + (character - '0');
                if (exponent > MaxInputExponent)
                {
                    return std::nullopt;
                }
            }
            return ExactDecimalAtom{coefficient, negativeExponent ? -exponent : exponent};
        }

        bool scaleExactDecimal(int64_t& coefficient, int places)
        {
            for (int place = 0; place < places; ++place)
            {
                if (coefficient > std::numeric_limits<int64_t>::max() / 10
                    || coefficient < std::numeric_limits<int64_t>::min() / 10)
                {
                    return false;
                }
                coefficient *= 10;
            }
            return true;
        }

        std::optional<string> exactDecimalAddIdentity(
            string_view leftIdentity, string_view rightIdentity, bool subtractRight)
        {
            auto left = exactDecimalAtom(leftIdentity);
            auto right = exactDecimalAtom(rightIdentity);
            if (!left || !right)
            {
                return std::nullopt;
            }
            int exponent = std::min(left->exponent, right->exponent);
            if (!scaleExactDecimal(left->coefficient, left->exponent - exponent)
                || !scaleExactDecimal(right->coefficient, right->exponent - exponent))
            {
                return std::nullopt;
            }
            if (subtractRight)
            {
                if (right->coefficient == std::numeric_limits<int64_t>::min())
                {
                    return std::nullopt;
                }
                right->coefficient = -right->coefficient;
            }
            if ((right->coefficient > 0
                 && left->coefficient > std::numeric_limits<int64_t>::max() - right->coefficient)
                || (right->coefficient < 0
                    && left->coefficient < std::numeric_limits<int64_t>::min() - right->coefficient))
            {
                return std::nullopt;
            }
            int64_t coefficient = left->coefficient + right->coefficient;
            if (coefficient == 0)
            {
                return string{"0"};
            }
            while (coefficient % 10 == 0)
            {
                coefficient /= 10;
                ++exponent;
            }
            uint64_t magnitude = coefficient < 0
                ? static_cast<uint64_t>(-(coefficient + 1)) + 1
                : static_cast<uint64_t>(coefficient);
            return string(coefficient < 0 ? "-" : "+") + std::to_string(magnitude) + "e" + std::to_string(exponent);
        }

        ValueFacts exactDecimalFacts(const string& identity)
        {
            if (identity == "0")
            {
                return {ZeroProof::ProvenZero, false, "0", false};
            }
            return {
                ZeroProof::ProvenNonZero,
                identity == "+1e0",
                identity,
                !identity.empty() && identity.front() == '+'};
        }

        std::optional<string> exactSquareRootIdentity(string_view identity)
        {
            if (identity.size() < 4 || identity.front() != '+')
            {
                return std::nullopt;
            }
            size_t exponentMarker = identity.find('e', 1);
            if (exponentMarker == string_view::npos || exponentMarker == 1)
            {
                return std::nullopt;
            }

            uint64_t significand = 0;
            for (char character : identity.substr(1, exponentMarker - 1))
            {
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                unsigned digit = static_cast<unsigned>(character - '0');
                if (significand > (std::numeric_limits<uint64_t>::max() - digit) / 10)
                {
                    return std::nullopt;
                }
                significand = significand * 10 + digit;
            }

            int exponent = 0;
            bool negativeExponent = false;
            size_t position = exponentMarker + 1;
            if (position < identity.size() && (identity[position] == '+' || identity[position] == '-'))
            {
                negativeExponent = identity[position] == '-';
                ++position;
            }
            if (position == identity.size())
            {
                return std::nullopt;
            }
            for (; position < identity.size(); ++position)
            {
                char character = identity[position];
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                exponent = exponent * 10 + (character - '0');
                if (exponent > MaxInputExponent)
                {
                    return std::nullopt;
                }
            }
            exponent = negativeExponent ? -exponent : exponent;
            if (exponent % 2 != 0)
            {
                if (significand > std::numeric_limits<uint64_t>::max() / 10)
                {
                    return std::nullopt;
                }
                significand *= 10;
                --exponent;
            }

            uint64_t low = 1;
            uint64_t high = std::min(significand, uint64_t{0xffffffff});
            uint64_t root = 0;
            while (low <= high)
            {
                uint64_t middle = low + (high - low) / 2;
                if (middle <= significand / middle)
                {
                    root = middle;
                    low = middle + 1;
                }
                else
                {
                    high = middle - 1;
                }
            }
            if (root == 0 || root * root != significand)
            {
                return std::nullopt;
            }

            int rootExponent = exponent / 2;
            while (root > 1 && root % 10 == 0)
            {
                root /= 10;
                ++rootExponent;
            }
            return "+" + std::to_string(root) + "e" + std::to_string(rootExponent);
        }

        ValueFacts literalFacts(string_view lexeme)
        {
            bool negative = !lexeme.empty() && lexeme.front() == '-';
            size_t position = (!lexeme.empty() && (lexeme.front() == '+' || lexeme.front() == '-')) ? 1 : 0;
            string digits;
            int fractionalDigits = 0;
            bool afterDecimal = false;
            while (position < lexeme.size() && lexeme[position] != 'e' && lexeme[position] != 'E')
            {
                char character = lexeme[position++];
                if (character == '.')
                {
                    afterDecimal = true;
                }
                else
                {
                    digits.push_back(character);
                    fractionalDigits += afterDecimal ? 1 : 0;
                }
            }
            int exponent = position < lexeme.size() ? parseBoundedExponent(lexeme.substr(position + 1)) : 0;
            size_t firstNonZero = digits.find_first_not_of('0');
            if (firstNonZero == string::npos)
            {
                return {ZeroProof::ProvenZero, false, "0", false};
            }
            digits.erase(0, firstNonZero);
            int decimalExponent = exponent - fractionalDigits;
            while (digits.size() > 1 && digits.back() == '0')
            {
                digits.pop_back();
                ++decimalExponent;
            }
            string identity = negative ? "-" : "+";
            identity += digits;
            identity += "e";
            identity += std::to_string(decimalExponent);
            bool one = !negative && digits == "1" && decimalExponent == 0;
            return {ZeroProof::ProvenNonZero, one, std::move(identity), !negative};
        }

        std::optional<ExactMultiple> boundedMultiple(Integer numerator, Integer denominator, bool timesPi)
        {
            if (epx::is_zero(denominator))
                return std::nullopt;
            if (epx::is_negative(denominator))
            {
                epx::negate(numerator);
                epx::negate(denominator);
            }
            Integer divisor = numerator;
            divisor.sgn = epx::sign::positive;
            Integer remainder = denominator;
            while (!epx::is_zero(remainder))
            {
                auto division = epx::div_n(divisor, remainder);
                divisor = std::move(remainder);
                remainder = std::move(division.r);
            }
            numerator = epx::floor_div(numerator, divisor).q;
            denominator = epx::floor_div(denominator, divisor).q;
            if (numerator.digits.size() > MaxIntegerLimbs || denominator.digits.size() > MaxIntegerLimbs)
                return std::nullopt;
            return ExactMultiple{std::move(numerator), std::move(denominator), timesPi};
        }

        std::optional<ExactMultiple> exactMultiple(const ValueFacts& facts)
        {
            if (facts.multiple)
                return facts.multiple;
            const auto one = epx::create<Container>(1);
            if (facts.zero == ZeroProof::ProvenZero)
                return ExactMultiple{Integer{}, one, false};
            if (facts.identity == "pi" || facts.identity == "neg(pi)")
                return ExactMultiple{epx::create<Container>(facts.identity == "pi" ? 1 : -1), one, true};

            string_view identity = facts.identity;
            if (identity.empty() || (identity.front() != '+' && identity.front() != '-'))
                return std::nullopt;
            size_t marker = identity.find('e');
            if (marker == string_view::npos)
                return std::nullopt;
            auto coefficient = epx::try_from_chars<Container>(identity.substr(0, marker));
            if (!coefficient || coefficient->digits.size() > MaxIntegerLimbs)
                return std::nullopt;
            int exponent = 0;
            auto exponentText = identity.substr(marker + 1);
            auto parsed = std::from_chars(exponentText.data(), exponentText.data() + exponentText.size(), exponent);
            constexpr int maxScale = MaxInputExponent + static_cast<int>(MaxInputCharacters);
            if (parsed.ec != std::errc{} || parsed.ptr != exponentText.data() + exponentText.size()
                || exponent < -maxScale || exponent > maxScale)
                return std::nullopt;
            auto scale = epx::details::pow10<Container>(static_cast<unsigned>(std::abs(exponent)));
            return exponent < 0
                ? boundedMultiple(std::move(*coefficient), std::move(scale), false)
                : boundedMultiple(epx::mul(*coefficient, scale), one, false);
        }

        std::optional<ExactMultiple> combineMultiples(
            const Token& operation, const ValueFacts& left, const ValueFacts& right)
        {
            auto lhs = exactMultiple(left);
            auto rhs = exactMultiple(right);
            if (!lhs || !rhs)
                return std::nullopt;
            if (std::holds_alternative<PlusToken>(operation) || std::holds_alternative<MinusToken>(operation))
            {
                if (epx::is_zero(lhs->numerator))
                    lhs->timesPi = rhs->timesPi;
                if (epx::is_zero(rhs->numerator))
                    rhs->timesPi = lhs->timesPi;
                if (lhs->timesPi != rhs->timesPi)
                    return std::nullopt;
                auto numerator = epx::mul(lhs->numerator, rhs->denominator);
                auto other = epx::mul(rhs->numerator, lhs->denominator);
                numerator = std::holds_alternative<PlusToken>(operation)
                    ? epx::add(numerator, other) : epx::sub(numerator, other);
                return boundedMultiple(
                    std::move(numerator), epx::mul(lhs->denominator, rhs->denominator), lhs->timesPi);
            }
            if (std::holds_alternative<MultiplyToken>(operation))
            {
                if (lhs->timesPi && rhs->timesPi)
                    return std::nullopt;
                return boundedMultiple(epx::mul(lhs->numerator, rhs->numerator),
                    epx::mul(lhs->denominator, rhs->denominator), lhs->timesPi || rhs->timesPi);
            }
            if (!lhs->timesPi && rhs->timesPi)
                return std::nullopt;
            return boundedMultiple(epx::mul(lhs->numerator, rhs->denominator),
                epx::mul(lhs->denominator, rhs->numerator), lhs->timesPi && !rhs->timesPi);
        }

        ValueFacts basicBinaryFacts(const Token& operation, const ValueFacts& left, const ValueFacts& right)
        {
            ValueFacts result;
            if (std::holds_alternative<PlusToken>(operation))
            {
                if (auto identity = exactDecimalAddIdentity(left.identity, right.identity, false))
                    return exactDecimalFacts(*identity);
                if (areOppositeIdentities(left.identity, right.identity))
                    return {ZeroProof::ProvenZero, false, "0", false};
                if (left.zero == ZeroProof::ProvenZero)
                    return right;
                if (right.zero == ZeroProof::ProvenZero)
                    return left;
            }
            else if (std::holds_alternative<MinusToken>(operation))
            {
                if (auto identity = exactDecimalAddIdentity(left.identity, right.identity, true))
                    return exactDecimalFacts(*identity);
                if (!left.identity.empty() && left.identity == right.identity)
                    return {ZeroProof::ProvenZero, false, "0", false};
                if (right.zero == ZeroProof::ProvenZero)
                    return left;
                if (left.zero == ZeroProof::ProvenZero && right.zero == ZeroProof::ProvenNonZero)
                {
                    result.zero = ZeroProof::ProvenNonZero;
                    result.identity = negateIdentity(right.identity);
                }
            }
            else if (std::holds_alternative<MultiplyToken>(operation))
            {
                if (left.zero == ZeroProof::ProvenZero || right.zero == ZeroProof::ProvenZero)
                    return {ZeroProof::ProvenZero, false, "0", false};
                if (left.multiplyCancellation && left.multiplyCancellation->factorIdentity == right.identity)
                    return valueFacts(left.multiplyCancellation->result);
                if (right.multiplyCancellation && right.multiplyCancellation->factorIdentity == left.identity)
                    return valueFacts(right.multiplyCancellation->result);
                if (left.one)
                    return right;
                if (right.one)
                    return left;
                if (left.zero == ZeroProof::ProvenNonZero && right.zero == ZeroProof::ProvenNonZero)
                    result.zero = ZeroProof::ProvenNonZero;
                result.one = left.one && right.one;
                result.positive = left.positive && right.positive;
            }
            else
            {
                if (left.zero == ZeroProof::ProvenZero)
                    return {ZeroProof::ProvenZero, false, "0", false};
                if (right.one)
                    return left;
                if (left.zero == ZeroProof::ProvenNonZero && right.zero == ZeroProof::ProvenNonZero)
                    result.zero = ZeroProof::ProvenNonZero;
                result.one = right.zero == ZeroProof::ProvenNonZero && !left.identity.empty() && left.identity == right.identity;
                if (result.one)
                    result.identity = "+1e0";
                result.positive = left.positive && right.positive;
                if (left.zero == ZeroProof::ProvenNonZero && right.zero == ZeroProof::ProvenNonZero
                    && !right.identity.empty())
                {
                    result.multiplyCancellation = FactorCancellationProof{right.identity, exactValueProof(left)};
                }
            }
            return result;
        }

        ValueFacts binaryFacts(const Token& operation, const ValueFacts& left, const ValueFacts& right)
        {
            ValueFacts result = basicBinaryFacts(operation, left, right);
            if (auto multiple = combineMultiples(operation, left, right))
                result.multiple = std::move(multiple);
            return result;
        }

        Integer integer(int value)
        {
            return epx::create<Container>(value);
        }

        Real rational(Integer numerator, Integer denominator = integer(1))
        {
            return epx::make_q(std::move(numerator), std::move(denominator));
        }

        Real negate(Real value)
        {
            return epx::opp(std::move(value));
        }

        Real subtract(Real left, Real right)
        {
            return epx::add(std::move(left), negate(std::move(right)));
        }

        int compareSigned(const Integer& left, const Integer& right)
        {
            if (epx::is_negative(left) != epx::is_negative(right))
            {
                return epx::is_negative(left) ? -1 : 1;
            }
            int result = epx::cmp_n(left, right);
            return epx::is_negative(left) ? -result : result;
        }

        Integer boundedApproximation(const Real& value, int precision)
        {
            if (precision < 0 || precision > ClassificationPrecision)
            {
                throw ResourceLimitError("realization precision limit");
            }
            auto result = value.approx(precision).get();
            if (result.digits.size() > MaxIntegerLimbs + static_cast<size_t>((precision * 2 + 31) / 32))
            {
                throw ResourceLimitError("numeric magnitude limit");
            }
            return result;
        }

        void ensureMagnitude(const Real& value)
        {
            auto approximation = boundedApproximation(value, 0);
            if (approximation.digits.size() > MaxIntegerLimbs)
            {
                throw ResourceLimitError("numeric magnitude limit");
            }
        }

        bool approximationIsZero(const Real& value)
        {
            return epx::is_zero(boundedApproximation(value, ClassificationPrecision));
        }

        bool approximationIsNegative(const Real& value)
        {
            return epx::is_negative(boundedApproximation(value, ClassificationPrecision));
        }

        void requireNonZero(const Real& value, const ValueFacts& facts)
        {
            if (facts.zero == ZeroProof::ProvenZero)
            {
                throw epx::divide_by_zero_error{};
            }
            if (facts.zero == ZeroProof::Unknown && approximationIsZero(value))
            {
                throw ResourceLimitError("zero classification limit");
            }
        }

        void ensureExponentialArgument(const Real& value)
        {
            auto approximation = boundedApproximation(value, 8);
            Integer bound = epx::mul_4exp(integer(1000), 8);
            Integer negativeBound = bound;
            epx::negate(negativeBound);
            if (compareSigned(approximation, negativeBound) < 0 || compareSigned(approximation, bound) > 0)
            {
                throw ResourceLimitError("function argument limit");
            }
        }

        int parseBoundedExponent(string_view text)
        {
            if (text.empty())
            {
                throw ParseError("missing exponent");
            }
            bool negative = false;
            size_t position = 0;
            if (text.front() == '+' || text.front() == '-')
            {
                negative = text.front() == '-';
                position = 1;
            }
            if (position == text.size())
            {
                throw ParseError("missing exponent digits");
            }
            int value = 0;
            for (; position < text.size(); ++position)
            {
                unsigned char character = static_cast<unsigned char>(text[position]);
                if (!std::isdigit(character))
                {
                    throw ParseError("invalid exponent");
                }
                int digit = character - '0';
                if (value > (MaxInputExponent - digit) / 10)
                {
                    throw ResourceLimitError("input exponent limit");
                }
                value = value * 10 + digit;
            }
            return negative ? -value : value;
        }

        Real parseNumber(string_view lexeme)
        {
            if (lexeme.empty() || lexeme == "+" || lexeme == "-" || lexeme == "." || lexeme == "+." || lexeme == "-.")
            {
                throw ParseError("incomplete number");
            }

            bool negative = false;
            size_t position = 0;
            if (lexeme[position] == '+' || lexeme[position] == '-')
            {
                negative = lexeme[position] == '-';
                ++position;
            }

            string digits;
            int fractionalDigits = 0;
            bool sawDigit = false;
            bool sawDecimal = false;
            while (position < lexeme.size() && lexeme[position] != 'e' && lexeme[position] != 'E')
            {
                char character = lexeme[position++];
                if (character == '.')
                {
                    if (sawDecimal)
                    {
                        throw ParseError("multiple decimal separators");
                    }
                    sawDecimal = true;
                    continue;
                }
                if (!std::isdigit(static_cast<unsigned char>(character)))
                {
                    throw ParseError("invalid number");
                }
                sawDigit = true;
                digits.push_back(character);
                if (sawDecimal)
                {
                    ++fractionalDigits;
                }
            }
            if (!sawDigit)
            {
                throw ParseError("missing digits");
            }

            int exponent = 0;
            if (position < lexeme.size())
            {
                ++position;
                exponent = parseBoundedExponent(lexeme.substr(position));
            }
            int scale = fractionalDigits - exponent;
            if (scale > MaxInputExponent || scale < -MaxInputExponent)
            {
                throw ResourceLimitError("decimal scale limit");
            }

            size_t firstNonZero = digits.find_first_not_of('0');
            if (firstNonZero == string::npos)
            {
                return rational(integer(0));
            }
            digits.erase(0, firstNonZero);
            if (digits.size() + static_cast<size_t>(std::max(0, -scale)) > MaxInputCharacters)
            {
                throw ResourceLimitError("numeric magnitude limit");
            }

            auto numerator = epx::try_from_chars<Container>(digits);
            if (!numerator)
            {
                throw ParseError("invalid number");
            }
            if (negative)
            {
                epx::negate(*numerator);
            }
            if (scale <= 0)
            {
                *numerator = epx::mul(*numerator, epx::details::pow10<Container>(static_cast<unsigned>(-scale)));
                return rational(std::move(*numerator));
            }
            return rational(std::move(*numerator), epx::details::pow10<Container>(static_cast<unsigned>(scale)));
        }

        class PrattParser
        {
        public:
            explicit PrattParser(const vector<Token>& tokens) : m_tokens(tokens) {}

            ParseResult Parse()
            {
                if (m_tokens.empty())
                {
                    throw ParseError("empty expression");
                }
                ParseResult result = ParseExpression(0, 0);
                if (m_position != m_tokens.size())
                {
                    throw ParseError("unexpected token");
                }
                return result;
            }

        private:
            static std::optional<int> InfixPrecedence(const Token& token)
            {
                if (std::holds_alternative<PlusToken>(token) || std::holds_alternative<MinusToken>(token))
                {
                    return 10;
                }
                if (std::holds_alternative<MultiplyToken>(token) || std::holds_alternative<DivideToken>(token))
                {
                    return 20;
                }
                return std::nullopt;
            }

            static unsigned CheckedIncrement(unsigned value, const char* message)
            {
                if (value >= MaxOperations)
                {
                    throw ResourceLimitError(message);
                }
                return value + 1;
            }

            static unsigned CheckedCombine(unsigned left, unsigned right, unsigned operationCost)
            {
                if (left > MaxOperations || right > MaxOperations || left > MaxOperations - right
                    || operationCost > MaxOperations - left - right)
                {
                    throw ResourceLimitError("operation limit");
                }
                return left + right + operationCost;
            }

            ParseResult ParseExpression(int minimumPrecedence, unsigned depth)
            {
                if (depth > MaxParseDepth)
                {
                    throw ResourceLimitError("parenthesis depth limit");
                }
                ParseResult left = ParsePrefix(depth);
                while (m_position < m_tokens.size())
                {
                    auto precedence = InfixPrecedence(m_tokens[m_position]);
                    if (!precedence || *precedence < minimumPrecedence)
                    {
                        break;
                    }
                    Token operation = m_tokens[m_position++];
                    ParseResult right = ParseExpression(*precedence + 1, depth);
                    unsigned operationCost = std::holds_alternative<MinusToken>(operation)
                            || std::holds_alternative<DivideToken>(operation)
                        ? 2u
                        : 1u;
                    unsigned operationCount = CheckedCombine(left.operationCount, right.operationCount, operationCost);
                    unsigned lazyDepth = std::max(left.lazyDepth, right.lazyDepth);
                    ValueFacts facts = binaryFacts(operation, left.facts, right.facts);
                    if (lazyDepth > MaxParseDepth - operationCost)
                    {
                        throw ResourceLimitError("lazy expression depth limit");
                    }
                    if (std::holds_alternative<PlusToken>(operation))
                    {
                        left.value = epx::add(std::move(left.value), std::move(right.value));
                    }
                    else if (std::holds_alternative<MinusToken>(operation))
                    {
                        left.value = subtract(std::move(left.value), std::move(right.value));
                    }
                    else if (std::holds_alternative<MultiplyToken>(operation))
                    {
                        left.value = epx::mul(std::move(left.value), std::move(right.value));
                    }
                    else
                    {
                        if (right.facts.zero == ZeroProof::ProvenZero)
                        {
                            if (left.facts.zero == ZeroProof::ProvenZero)
                            {
                                throw IndefiniteError{};
                            }
                            throw epx::divide_by_zero_error{};
                        }
                        requireNonZero(right.value, right.facts);
                        left.value = epx::mul(std::move(left.value), epx::inv(std::move(right.value)));
                    }
                    left.lazyDepth = lazyDepth + operationCost;
                    left.operationCount = operationCount;
                    left.facts = std::move(facts);
                    ensureMagnitude(left.value);
                }
                return left;
            }

            ParseResult ParsePrefix(unsigned depth)
            {
                if (m_position >= m_tokens.size())
                {
                    throw ParseError("unexpected end of expression");
                }
                const Token& token = m_tokens[m_position++];
                if (const auto* number = std::get_if<NumberToken>(&token))
                {
                    if (number->lazyDepth > MaxParseDepth || number->operationCount > MaxOperations)
                    {
                        throw ResourceLimitError("retained expression limit");
                    }
                    return {*number->value, number->lazyDepth, number->operationCount, number->facts};
                }
                if (std::holds_alternative<PlusToken>(token))
                {
                    ParseResult result = ParseExpression(30, depth);
                    result.operationCount = CheckedIncrement(result.operationCount, "operation limit");
                    if (result.lazyDepth >= MaxParseDepth)
                    {
                        throw ResourceLimitError("lazy expression depth limit");
                    }
                    ++result.lazyDepth;
                    return result;
                }
                if (std::holds_alternative<MinusToken>(token))
                {
                    ParseResult result = ParseExpression(30, depth);
                    result.operationCount = CheckedIncrement(result.operationCount, "operation limit");
                    if (result.lazyDepth >= MaxParseDepth)
                    {
                        throw ResourceLimitError("lazy expression depth limit");
                    }
                    result.value = negate(std::move(result.value));
                    ++result.lazyDepth;
                    if (result.facts.one)
                    {
                        result.facts.one = false;
                    }
                    result.facts.positive = false;
                    result.facts.identity = negateIdentity(std::move(result.facts.identity));
                    if (result.facts.multiple)
                        epx::negate(result.facts.multiple->numerator);
                    return result;
                }
                if (std::holds_alternative<LeftParenToken>(token))
                {
                    ParseResult result = ParseExpression(0, depth + 1);
                    if (m_position >= m_tokens.size() || !std::holds_alternative<RightParenToken>(m_tokens[m_position]))
                    {
                        throw ParseError("missing closing parenthesis");
                    }
                    ++m_position;
                    return result;
                }
                throw ParseError("expected operand");
            }

            const vector<Token>& m_tokens;
            size_t m_position = 0;
        };

        wstring widen(string_view value)
        {
            return wstring(value.begin(), value.end());
        }

        string narrowNumber(const wstring& input, wchar_t decimalSeparator)
        {
            string result;
            result.reserve(input.size());
            for (wchar_t character : input)
            {
                if (character == decimalSeparator)
                {
                    result.push_back('.');
                }
                else if (character >= L'0' && character <= L'9')
                {
                    result.push_back(static_cast<char>(character));
                }
                else if (character == L'+' || character == L'-' || character == L'e' || character == L'E' || character == L'.')
                {
                    result.push_back(static_cast<char>(character));
                }
                else
                {
                    throw ParseError("invalid localized number");
                }
            }
            return result;
        }

        bool endsWithOperand(const vector<Token>& tokens)
        {
            return !tokens.empty()
                && (std::holds_alternative<NumberToken>(tokens.back()) || std::holds_alternative<RightParenToken>(tokens.back()));
        }

        bool isOperator(const Token& token)
        {
            return std::holds_alternative<PlusToken>(token) || std::holds_alternative<MinusToken>(token)
                || std::holds_alternative<MultiplyToken>(token) || std::holds_alternative<DivideToken>(token);
        }
    }

    class EpsilonEngine::Impl
    {
    public:
        Impl(IResourceProvider* resourceProvider, ICalcDisplay* displayCallback, shared_ptr<CalculatorHistory> history)
            : m_resourceProvider(resourceProvider), m_display(displayCallback), m_history(std::move(history))
        {
            if (m_resourceProvider == nullptr)
            {
                throw std::invalid_argument("EpsilonEngine requires a resource provider");
            }
        }

        void Reset()
        {
            m_tokens.clear();
            m_input.clear();
            m_result.reset();
            m_resultLazyDepth = 0;
            m_resultOperationCount = 0;
            m_resultFacts = {};
            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_errorExpressionCandidate.clear();
            m_nextIdentity = 0;
            m_resultText = L"0";
            m_error = false;
            m_completed = false;
            m_replaceOperandOnInput = false;
            m_scientificFormat = false;
            m_angle = Command::CommandDEG;
            m_openParentheses = 0;
            PublishAll();
        }

        bool IsInputEmpty() const
        {
            return !m_error && m_input.empty() && !m_replaceOperandOnInput && !m_completed && !endsWithOperand(m_tokens);
        }

        bool IsRecording() const
        {
            return !m_completed && (!m_input.empty() || !m_tokens.empty());
        }

        void SetPrecision(int32_t precision)
        {
            m_precision = std::clamp(precision, int32_t{1}, int32_t{MaxDisplayPrecision});
            if (m_result)
            {
                m_resultText = Format(*m_result, m_resultFacts);
                PublishPrimary();
            }
        }

        wchar_t DecimalSeparator() const
        {
            wstring separator = m_resourceProvider->GetCEngineString(L"sDecimal");
            return separator.empty() ? L'.' : separator.front();
        }

        void Process(Command command)
        {
            if (m_error && command != Command::CommandCLEAR && command != Command::CommandCENTR)
            {
                return;
            }

            try
            {
                if (command >= Command::Command0 && command <= Command::Command9)
                {
                    Digit(static_cast<int>(command) - static_cast<int>(Command::Command0));
                    return;
                }
                switch (command)
                {
                case Command::CommandPNT:
                    Decimal();
                    break;
                case Command::CommandEXP:
                    Exponent();
                    break;
                case Command::CommandSIGN:
                    Sign();
                    break;
                case Command::CommandBACK:
                    Backspace();
                    break;
                case Command::CommandCENTR:
                    ClearEntry();
                    break;
                case Command::CommandCLEAR:
                    ClearCalculation();
                    break;
                case Command::CommandOPENP:
                    OpenParenthesis();
                    break;
                case Command::CommandCLOSEP:
                    CloseParenthesis();
                    break;
                case Command::CommandADD:
                case Command::CommandSUB:
                case Command::CommandMUL:
                case Command::CommandDIV:
                    Binary(command);
                    break;
                case Command::CommandSQR:
                case Command::CommandSQRT:
                case Command::CommandREC:
                case Command::CommandSIN:
                case Command::CommandCOS:
                case Command::CommandTAN:
                case Command::CommandLN:
                case Command::CommandLOG:
                case Command::CommandPOWE:
                    Unary(command);
                    break;
                case Command::CommandPI:
                case Command::CommandEuler:
                    Constant(command);
                    break;
                case Command::CommandDEG:
                case Command::CommandRAD:
                case Command::CommandGRAD:
                    m_angle = command;
                    PublishExpression();
                    break;
                case Command::CommandFE:
                    ToggleFormat();
                    break;
                case Command::CommandEQU:
                    Equals();
                    break;
                default:
                    throw std::invalid_argument("unsupported EpsilonEngine command");
                }
            }
            catch (const ParseError&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_DOMAIN));
            }
            catch (const epx::divide_by_zero_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_DIVIDEBYZERO));
            }
            catch (const IndefiniteError&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_INDEFINITE));
            }
            catch (const epx::negative_radicand_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_DOMAIN));
            }
            catch (const epx::non_positive_log_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_DOMAIN));
            }
            catch (const epx::kthroot_too_small_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_DOMAIN));
            }
            catch (const epx::negative_zpow_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_DOMAIN));
            }
            catch (const ResourceLimitError&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_OVERFLOW));
            }
            catch (const epx::msd_overflow_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_OVERFLOW));
            }
            catch (const epx::precision_overflow_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_OVERFLOW));
            }
            catch (const std::bad_alloc&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_OVERFLOW));
            }
            catch (const std::length_error&)
            {
                DisplayError(static_cast<int32_t>(CALC_E_OVERFLOW));
            }
        }

        void DisplayError(int32_t errorCode)
        {
            uint32_t code = static_cast<uint32_t>(errorCode);
            int resourceId;
            switch (code)
            {
            case CALC_E_DIVIDEBYZERO:
                resourceId = IDS_DIVBYZERO;
                break;
            case CALC_E_INDEFINITE:
                resourceId = IDS_UNDEFINED;
                break;
            case CALC_E_OVERFLOW:
                resourceId = IDS_OVERFLOW;
                break;
            case CALC_E_DOMAIN:
            default:
                resourceId = IDS_DOMAIN;
                break;
            }
            wstring message = m_resourceProvider->GetCEngineString(std::to_wstring(resourceId));
            if (message.empty())
            {
                message = resourceId == IDS_DIVBYZERO ? L"Cannot divide by zero"
                    : resourceId == IDS_UNDEFINED     ? L"Result is undefined"
                    : resourceId == IDS_OVERFLOW       ? L"Overflow"
                                                     : L"Invalid input";
            }
            m_resultText = std::move(message);
            m_error = true;
            if (m_display)
            {
                m_display->SetPrimaryDisplay(m_resultText, true);
                m_display->SetIsInError(true);
            }
            if (!m_errorExpressionCandidate.empty())
            {
                PublishExpressionText(m_errorExpressionCandidate);
                m_errorExpressionCandidate.clear();
            }
        }

        const wstring& Result() const
        {
            return m_resultText;
        }

    private:
        void StartFreshInputIfCompleted()
        {
            if (m_completed)
            {
                m_tokens.clear();
                m_result.reset();
                m_resultLazyDepth = 0;
                m_resultOperationCount = 0;
                m_resultFacts = {};
                m_exponentBase.reset();
                m_exponentBaseDisplay.clear();
                m_errorExpressionCandidate.clear();
                m_nextIdentity = 0;
                m_input.clear();
                m_completed = false;
                m_replaceOperandOnInput = false;
                m_openParentheses = 0;
            }
        }

        void ReplaceEvaluatedOperandForInput()
        {
            if (!m_replaceOperandOnInput || !m_input.empty())
            {
                return;
            }
            size_t begin = CurrentOperandStart();
            m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
            m_result.reset();
            m_resultLazyDepth = 0;
            m_resultOperationCount = 0;
            m_resultFacts = {};
            m_replaceOperandOnInput = false;
        }

        void CheckInputLimit() const
        {
            if (m_input.size() >= MaxInputCharacters)
            {
                throw ResourceLimitError("input length limit");
            }
        }

        void CheckTokenLimit() const
        {
            if (m_tokens.size() >= MaxTokens)
            {
                throw ResourceLimitError("token limit");
            }
        }

        void BeginNumericInput()
        {
            StartFreshInputIfCompleted();
            if (m_exponentBase)
            {
                return;
            }
            ReplaceEvaluatedOperandForInput();
            if (m_input.empty() && !m_tokens.empty() && std::holds_alternative<RightParenToken>(m_tokens.back()))
            {
                AddOperator(Command::CommandMUL);
                if (m_display)
                {
                    m_display->BinaryOperatorReceived();
                }
            }
        }

        void Digit(int digit)
        {
            BeginNumericInput();
            CheckInputLimit();
            if (m_input == L"0")
            {
                m_input.clear();
            }
            m_input.push_back(static_cast<wchar_t>(L'0' + digit));
            PublishInput();
        }

        void Decimal()
        {
            BeginNumericInput();
            CheckInputLimit();
            if (m_input.find_first_of(L"eE") != wstring::npos)
            {
                return;
            }
            wchar_t separator = DecimalSeparator();
            if (m_input.find(separator) != wstring::npos || m_input.find(L'.') != wstring::npos)
            {
                return;
            }
            if (m_input.empty() || m_input == L"-" || m_input == L"+")
            {
                m_input += L'0';
            }
            m_input.push_back(separator);
            PublishInput();
        }

        std::optional<wstring> ExponentInput(const wstring& base) const
        {
            if (base.find_first_of(L"eE") != wstring::npos)
            {
                return std::nullopt;
            }
            wstring input = base;
            wchar_t separator = DecimalSeparator();
            const wstring groupingSeparator = m_resourceProvider->GetCEngineString(L"sThousand");
            if (!groupingSeparator.empty() && groupingSeparator != wstring(1, separator))
            {
                size_t position = input.find(groupingSeparator);
                while (position != wstring::npos)
                {
                    input.erase(position, groupingSeparator.size());
                    position = input.find(groupingSeparator, position);
                }
            }
            if (input.find(separator) == wstring::npos && input.find(L'.') == wstring::npos)
            {
                input.push_back(separator);
            }
            input += L"e+";
            return input;
        }

        void BeginRetainedExponent(wstring input)
        {
            size_t begin = CurrentOperandStart();
            m_exponentBase = ParseRange(begin, m_tokens.size());
            m_exponentOperandStart = begin;
            m_exponentBaseDisplay = m_resultText;
            m_input = std::move(input);
            PublishInput();
        }

        void Exponent()
        {
            if (!m_input.empty())
            {
                if (m_input == L"-" || m_input == L"+" || m_input.find_first_of(L"eE") != wstring::npos)
                {
                    return;
                }
                auto input = ExponentInput(m_input);
                if (!input)
                {
                    return;
                }
                if (input->size() > MaxInputCharacters)
                {
                    throw ResourceLimitError("input length limit");
                }
                m_input = std::move(*input);
                PublishInput();
                return;
            }

            if (m_completed && m_result)
            {
                auto input = ExponentInput(m_resultText);
                if (!input)
                {
                    return;
                }
                if (input->size() > MaxInputCharacters)
                {
                    throw ResourceLimitError("input length limit");
                }
                SeedFromResult();
                m_replaceOperandOnInput = true;
                BeginRetainedExponent(std::move(*input));
                return;
            }

            if (!m_replaceOperandOnInput || m_tokens.empty() || !endsWithOperand(m_tokens))
            {
                return;
            }
            auto input = ExponentInput(m_resultText);
            if (!input)
            {
                return;
            }
            if (input->size() > MaxInputCharacters)
            {
                throw ResourceLimitError("input length limit");
            }
            BeginRetainedExponent(std::move(*input));
        }

        void Sign()
        {
            if (!m_input.empty())
            {
                size_t exponent = m_input.find_first_of(L"eE");
                if (exponent != wstring::npos)
                {
                    size_t signPosition = exponent + 1;
                    if (signPosition < m_input.size() && m_input[signPosition] == L'-')
                    {
                        m_input[signPosition] = L'+';
                    }
                    else if (signPosition < m_input.size() && m_input[signPosition] == L'+')
                    {
                        m_input[signPosition] = L'-';
                    }
                    else
                    {
                        m_input.insert(signPosition, 1, L'+');
                    }
                }
                else if (m_input.front() == L'-')
                {
                    m_input.erase(0, 1);
                }
                else
                {
                    m_input.insert(0, 1, L'-');
                }
                PublishInput();
                return;
            }
            UnarySign();
        }

        void Backspace()
        {
            if (m_completed)
            {
                return;
            }
            if (!m_input.empty())
            {
                size_t exponent = m_input.find_first_of(L"eE");
                if (exponent != wstring::npos && m_input.size() == exponent + 2)
                {
                    m_input.erase(exponent);
                    if (m_exponentBase)
                    {
                        m_input.clear();
                        m_resultText = m_exponentBaseDisplay;
                        m_exponentBase.reset();
                        m_exponentBaseDisplay.clear();
                        PublishAll();
                        return;
                    }
                }
                else
                {
                    m_input.pop_back();
                }
                PublishInput();
            }
        }

        void CancelIncompleteRetainedExponent()
        {
            if (!m_exponentBase)
            {
                return;
            }
            size_t exponent = m_input.find_last_of(L"eE");
            if (exponent == wstring::npos || m_input.size() != exponent + 2
                || (m_input.back() != L'+' && m_input.back() != L'-'))
            {
                return;
            }
            m_input.clear();
            m_resultText = m_exponentBaseDisplay;
            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
        }

        void ClearEntry()
        {
            if (m_error || m_completed)
            {
                ClearCalculation();
                return;
            }
            bool removeCommittedOperand = m_exponentBase.has_value()
                || (m_input.empty() && endsWithOperand(m_tokens));
            if (removeCommittedOperand)
            {
                size_t begin = CurrentOperandStart();
                m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
            }
            m_input.clear();
            m_result.reset();
            m_resultLazyDepth = 0;
            m_resultOperationCount = 0;
            m_resultFacts = {};
            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_errorExpressionCandidate.clear();
            m_error = false;
            m_replaceOperandOnInput = false;
            m_resultText = L"0";
            if (m_display)
            {
                m_display->SetIsInError(false);
            }
            PublishAll();
        }

        void ClearCalculation()
        {
            bool retainMode = m_scientificFormat;
            Command retainAngle = m_angle;
            m_tokens.clear();
            m_input.clear();
            m_result.reset();
            m_resultLazyDepth = 0;
            m_resultOperationCount = 0;
            m_resultFacts = {};
            m_exponentBase.reset();
            m_exponentBaseDisplay.clear();
            m_errorExpressionCandidate.clear();
            m_nextIdentity = 0;
            m_resultText = L"0";
            m_error = false;
            m_completed = false;
            m_replaceOperandOnInput = false;
            m_openParentheses = 0;
            m_scientificFormat = retainMode;
            m_angle = retainAngle;
            PublishAll();
        }

        string NextIdentity()
        {
            return "r" + std::to_string(++m_nextIdentity);
        }

        void CommitInput()
        {
            if (m_input.empty())
            {
                return;
            }
            CheckTokenLimit();
            if (m_exponentBase)
            {
                size_t exponentMarker = m_input.find_last_of(L"eE");
                if (exponentMarker == wstring::npos)
                {
                    throw ParseError("missing retained exponent");
                }
                string exponentText = narrowNumber(m_input.substr(exponentMarker + 1), DecimalSeparator());
                int exponent = parseBoundedExponent(exponentText);
                unsigned cost = exponent < 0 ? 2u : 1u;
                if (m_exponentBase->lazyDepth > MaxParseDepth - cost
                    || m_exponentBase->operationCount > MaxOperations - cost)
                {
                    throw ResourceLimitError("lazy expression limit");
                }
                Real factor = rational(epx::details::pow10<Container>(static_cast<unsigned>(std::abs(exponent))));
                if (exponent < 0)
                {
                    factor = epx::inv(std::move(factor));
                }
                Real value = epx::mul(m_exponentBase->value, std::move(factor));
                ensureMagnitude(value);
                ValueFacts facts = m_exponentBase->facts;
                facts.multiple = combineMultiples(
                    MultiplyToken{}, facts, exactDecimalFacts("+1e" + std::to_string(exponent)));
                facts.one = facts.one && exponent == 0;
                if (exponent != 0)
                {
                    facts.identity = NextIdentity();
                }
                wstring display = m_input;
                m_tokens.erase(m_tokens.begin() + m_exponentOperandStart, m_tokens.end());
                m_tokens.emplace_back(NumberToken{
                    make_shared<Real>(std::move(value)),
                    "<retained-exponent>",
                    std::move(display),
                    {m_exponentOperandStart, m_exponentOperandStart + 1},
                    m_exponentBase->lazyDepth + cost,
                    m_exponentBase->operationCount + cost,
                    std::move(facts)});
                m_input.clear();
                m_exponentBase.reset();
                m_exponentBaseDisplay.clear();
                m_replaceOperandOnInput = false;
                return;
            }
            string lexeme = narrowNumber(m_input, DecimalSeparator());
            auto value = make_shared<Real>(parseNumber(lexeme));
            ValueFacts facts = literalFacts(lexeme);
            size_t offset = m_tokens.size();
            wstring display = m_input;
            m_tokens.emplace_back(
                NumberToken{std::move(value), std::move(lexeme), std::move(display), {offset, offset + 1}, 0, 0, std::move(facts)});
            m_input.clear();
        }

        void SeedFromResult()
        {
            if (!m_completed || !m_result)
            {
                return;
            }
            m_tokens.clear();
            m_tokens.emplace_back(NumberToken{
                make_shared<Real>(*m_result),
                "<retained>",
                m_resultText,
                {0, 1},
                m_resultLazyDepth,
                m_resultOperationCount,
                m_resultFacts});
            m_completed = false;
            m_replaceOperandOnInput = false;
            m_openParentheses = 0;
        }

        wstring BinaryDisplay(Command command) const
        {
            switch (command)
            {
            case Command::CommandADD:
                return L"+";
            case Command::CommandSUB:
                return L"-";
            case Command::CommandMUL:
                return L"\u00d7";
            case Command::CommandDIV:
                return L"\u00f7";
            default:
                throw ParseError("invalid binary operator");
            }
        }

        void UpdatePrefixResult(Command nextOperator)
        {
            // Reduce only the current group and operators that bind at least as tightly
            // as the incoming operator. Pending additions must not run before a multiply.
            size_t begin = 0;
            unsigned nesting = 0;
            for (size_t position = m_tokens.size(); position-- > 0;)
            {
                if (std::holds_alternative<RightParenToken>(m_tokens[position]))
                {
                    ++nesting;
                }
                else if (std::holds_alternative<LeftParenToken>(m_tokens[position]))
                {
                    if (nesting == 0)
                    {
                        begin = position + 1;
                        break;
                    }
                    --nesting;
                }
            }

            if (nextOperator == Command::CommandMUL || nextOperator == Command::CommandDIV)
            {
                nesting = 0;
                for (size_t position = m_tokens.size(); position-- > begin;)
                {
                    if (std::holds_alternative<RightParenToken>(m_tokens[position]))
                    {
                        ++nesting;
                    }
                    else if (std::holds_alternative<LeftParenToken>(m_tokens[position]))
                    {
                        --nesting;
                    }
                    else if (nesting == 0 && position > begin
                             && (std::holds_alternative<PlusToken>(m_tokens[position])
                                 || std::holds_alternative<MinusToken>(m_tokens[position]))
                             && !isOperator(m_tokens[position - 1]))
                    {
                        begin = position + 1;
                        break;
                    }
                }
            }

            ParseResult result = ParseRange(begin, m_tokens.size());
            ensureMagnitude(result.value);
            m_result = std::move(result.value);
            m_resultLazyDepth = result.lazyDepth;
            m_resultOperationCount = result.operationCount;
            m_resultFacts = std::move(result.facts);
            m_resultText = Format(*m_result, m_resultFacts);
        }

        void Binary(Command command)
        {
            CancelIncompleteRetainedExponent();
            SeedFromResult();
            if (m_tokens.empty() && !m_input.empty())
            {
                m_errorExpressionCandidate = m_input + L" " + BinaryDisplay(command) + L" ";
            }
            CommitInput();
            m_errorExpressionCandidate.clear();
            if (m_tokens.empty())
            {
                if (command == Command::CommandADD || command == Command::CommandSUB)
                {
                    AddZeroOperand();
                    AddOperator(command);
                    PublishAll();
                }
                return;
            }
            if (isOperator(m_tokens.back()))
            {
                m_tokens.pop_back();
            }
            else if (std::holds_alternative<LeftParenToken>(m_tokens.back())
                     && (command == Command::CommandADD || command == Command::CommandSUB))
            {
                AddZeroOperand();
            }
            else if (!endsWithOperand(m_tokens))
            {
                throw ParseError("operator requires operand");
            }
            else
            {
                UpdatePrefixResult(command);
            }
            AddOperator(command);
            m_replaceOperandOnInput = false;
            if (m_display)
            {
                m_display->BinaryOperatorReceived();
            }
            PublishAll();
        }

        void AddZeroOperand()
        {
            CheckTokenLimit();
            size_t offset = m_tokens.size();
            m_tokens.emplace_back(NumberToken{
                make_shared<Real>(rational(integer(0))),
                "0",
                L"0",
                {offset, offset + 1},
                0,
                0,
                {ZeroProof::ProvenZero, false, "0", false}});
        }

        void AddOperator(Command command)
        {
            CheckTokenLimit();
            SourceSpan span{m_tokens.size(), m_tokens.size() + 1};
            switch (command)
            {
            case Command::CommandADD:
                m_tokens.emplace_back(PlusToken{span});
                break;
            case Command::CommandSUB:
                m_tokens.emplace_back(MinusToken{span});
                break;
            case Command::CommandMUL:
                m_tokens.emplace_back(MultiplyToken{span});
                break;
            case Command::CommandDIV:
                m_tokens.emplace_back(DivideToken{span});
                break;
            default:
                throw ParseError("invalid binary operator");
            }
        }

        void OpenParenthesis()
        {
            StartFreshInputIfCompleted();
            CommitInput();
            if (endsWithOperand(m_tokens))
            {
                AddOperator(Command::CommandMUL);
                if (m_display)
                {
                    m_display->BinaryOperatorReceived();
                }
            }
            if (m_openParentheses >= MaxParseDepth)
            {
                throw ResourceLimitError("parenthesis depth limit");
            }
            CheckTokenLimit();
            SourceSpan span{m_tokens.size(), m_tokens.size() + 1};
            m_tokens.emplace_back(LeftParenToken{span});
            ++m_openParentheses;
            PublishAll();
        }

        void CompleteTrailingOperator()
        {
            if (m_tokens.empty() || !isOperator(m_tokens.back()))
            {
                return;
            }

            Token operation = std::move(m_tokens.back());
            m_tokens.pop_back();
            if (m_tokens.empty() || !endsWithOperand(m_tokens))
            {
                throw ParseError("operator requires operand");
            }

            if (!m_result)
            {
                throw ParseError("missing value for equals completion");
            }
            ParseResult operand{*m_result, m_resultLazyDepth, m_resultOperationCount, m_resultFacts};
            wstring display = m_resultText;
            CheckTokenLimit();
            SourceSpan span{m_tokens.size() + 1, m_tokens.size() + 2};
            m_tokens.push_back(std::move(operation));
            m_tokens.emplace_back(NumberToken{
                make_shared<Real>(operand.value),
                "<auto-completed>",
                std::move(display),
                span,
                operand.lazyDepth,
                operand.operationCount,
                operand.facts});
        }

        void CloseRemainingParentheses()
        {
            while (m_openParentheses > 0)
            {
                CheckTokenLimit();
                SourceSpan span{m_tokens.size(), m_tokens.size() + 1};
                m_tokens.emplace_back(RightParenToken{span});
                --m_openParentheses;
            }
        }

        void CloseParenthesis()
        {
            CommitInput();
            if (m_openParentheses == 0)
            {
                if (m_display)
                {
                    m_display->OnNoRightParenAdded();
                }
                return;
            }
            if (!endsWithOperand(m_tokens))
            {
                throw ParseError("closing parenthesis requires an operand");
            }
            CheckTokenLimit();
            SourceSpan span{m_tokens.size(), m_tokens.size() + 1};
            m_tokens.emplace_back(RightParenToken{span});
            --m_openParentheses;
            ParseResult result = ParseRange(CurrentOperandStart(), m_tokens.size());
            ensureMagnitude(result.value);
            m_result = std::move(result.value);
            m_resultLazyDepth = result.lazyDepth;
            m_resultOperationCount = result.operationCount;
            m_resultFacts = std::move(result.facts);
            m_resultText = Format(*m_result, m_resultFacts);
            PublishAll();
        }

        size_t CurrentOperandStart() const
        {
            if (m_tokens.empty())
            {
                throw ParseError("missing operand");
            }
            if (std::holds_alternative<NumberToken>(m_tokens.back()))
            {
                return m_tokens.size() - 1;
            }
            if (!std::holds_alternative<RightParenToken>(m_tokens.back()))
            {
                throw ParseError("missing operand");
            }
            int nesting = 0;
            for (size_t position = m_tokens.size(); position-- > 0;)
            {
                if (std::holds_alternative<RightParenToken>(m_tokens[position]))
                {
                    ++nesting;
                }
                else if (std::holds_alternative<LeftParenToken>(m_tokens[position]) && --nesting == 0)
                {
                    return position;
                }
            }
            throw ParseError("mismatched parenthesis");
        }

        ParseResult ParseRange(size_t begin, size_t end) const
        {
            vector<Token> range(m_tokens.begin() + begin, m_tokens.begin() + end);
            return PrattParser(range).Parse();
        }

        void ReplaceCurrentOperand(
            Real value, wstring label, unsigned lazyDepth, unsigned operationCount, ValueFacts facts)
        {
            ensureMagnitude(value);
            size_t begin = CurrentOperandStart();
            m_tokens.erase(m_tokens.begin() + begin, m_tokens.end());
            m_tokens.emplace_back(NumberToken{
                make_shared<Real>(value),
                "<retained>",
                std::move(label),
                {begin, begin + 1},
                lazyDepth,
                operationCount,
                facts});
            m_result = std::move(value);
            m_resultLazyDepth = lazyDepth;
            m_resultOperationCount = operationCount;
            m_resultFacts = std::move(facts);
            m_resultText = Format(*m_result, m_resultFacts);
            m_replaceOperandOnInput = true;
            PublishAll();
        }

        Real ApplyUnary(Command command, Real value, const ValueFacts& facts)
        {
            switch (command)
            {
            case Command::CommandSQR:
                return epx::mul(value, value);
            case Command::CommandSQRT:
                if (approximationIsNegative(value))
                {
                    throw epx::negative_radicand_error{};
                }
                return epx::root(std::move(value), 2);
            case Command::CommandREC:
                requireNonZero(value, facts);
                return epx::inv(std::move(value));
            case Command::CommandSIN:
            case Command::CommandCOS:
            case Command::CommandTAN:
                if (auto exact = ExactTrigonometricValue(command, facts))
                {
                    return rational(integer(*exact));
                }
                value = ToRadians(std::move(value));
                if (command == Command::CommandSIN)
                {
                    return epx::sin(std::move(value));
                }
                if (command == Command::CommandCOS)
                {
                    return epx::cos(std::move(value));
                }
                {
                    Real cosine = epx::cos(value);
                    requireNonZero(cosine, {});
                    return epx::mul(epx::sin(std::move(value)), epx::inv(std::move(cosine)));
                }
            case Command::CommandLN:
                if (facts.zero == ZeroProof::ProvenZero || approximationIsNegative(value))
                {
                    throw epx::non_positive_log_error{};
                }
                if (!facts.positive && approximationIsZero(value))
                    throw ResourceLimitError("logarithm sign classification limit");
                return epx::log(std::move(value));
            case Command::CommandLOG:
                if (facts.zero == ZeroProof::ProvenZero || approximationIsNegative(value))
                {
                    throw epx::non_positive_log_error{};
                }
                if (!facts.positive && approximationIsZero(value))
                    throw ResourceLimitError("logarithm sign classification limit");
                return epx::mul(epx::log(std::move(value)), epx::inv(epx::log(rational(integer(10)))));
            case Command::CommandPOWE:
                ensureExponentialArgument(value);
                return epx::exp(std::move(value));
            default:
                throw ParseError("invalid unary operation");
            }
        }

        std::optional<unsigned> ExactQuarterTurns(const ValueFacts& input) const
        {
            if (input.zero == ZeroProof::ProvenZero)
            {
                return 0u;
            }
            auto multiple = exactMultiple(input);
            if (!multiple)
                return std::nullopt;
            const bool radians = m_angle == Command::CommandRAD;
            if (multiple->timesPi != radians)
                return std::nullopt;
            auto numerator = radians ? epx::mul(multiple->numerator, integer(2)) : multiple->numerator;
            auto denominator = radians ? multiple->denominator
                : epx::mul(multiple->denominator, integer(m_angle == Command::CommandGRAD ? 100 : 90));
            auto turns = epx::floor_div(numerator, denominator);
            if (!epx::is_zero(turns.r))
                return std::nullopt;
            auto quadrant = epx::floor_div(turns.q, integer(4)).r;
            return epx::is_zero(quadrant) ? 0u : static_cast<unsigned>(quadrant.digits.front());
        }

        std::optional<int> ExactTrigonometricValue(Command command, const ValueFacts& input) const
        {
            auto quarterTurns = ExactQuarterTurns(input);
            if (!quarterTurns)
                return std::nullopt;
            switch (command)
            {
            case Command::CommandSIN:
                return *quarterTurns % 2 == 0 ? 0 : *quarterTurns == 1 ? 1 : -1;
            case Command::CommandCOS:
                return *quarterTurns % 2 != 0 ? 0 : *quarterTurns == 0 ? 1 : -1;
            case Command::CommandTAN:
                if (*quarterTurns % 2 != 0)
                    throw epx::divide_by_zero_error{};
                return 0;
            default:
                throw ParseError("invalid trigonometric operation");
            }
        }

        ValueFacts UnaryFacts(Command command, const ValueFacts& input)
        {
            switch (command)
            {
            case Command::CommandSQR:
                if (input.squareResult)
                {
                    return valueFacts(*input.squareResult);
                }
                return {input.zero, input.one, input.zero == ZeroProof::ProvenZero ? "0" : NextIdentity(),
                        input.zero == ZeroProof::ProvenNonZero, {}, {}, combineMultiples(MultiplyToken{}, input, input)};
            case Command::CommandSQRT:
            {
                string identity;
                if (input.zero == ZeroProof::ProvenZero)
                {
                    identity = "0";
                }
                else if (auto exactIdentity = exactSquareRootIdentity(input.identity))
                {
                    identity = std::move(*exactIdentity);
                }
                else
                {
                    identity = NextIdentity();
                }
                ValueFacts result{
                    input.zero, identity == "+1e0", std::move(identity), input.zero == ZeroProof::ProvenNonZero};
                result.squareResult = exactValueProof(input);
                return result;
            }
            case Command::CommandREC:
                return {ZeroProof::ProvenNonZero, input.one, input.one ? "+1e0" : NextIdentity(), input.positive,
                        {}, {}, combineMultiples(DivideToken{}, integerFacts(1), input)};
            case Command::CommandSIN:
            case Command::CommandCOS:
            case Command::CommandTAN:
                if (auto exact = ExactTrigonometricValue(command, input))
                    return integerFacts(*exact);
                return {ZeroProof::Unknown, false, NextIdentity(), false};
            case Command::CommandLN:
                if (input.one)
                    return {ZeroProof::ProvenZero, false, "0", false};
                return {ZeroProof::Unknown, false, NextIdentity(), false};
            case Command::CommandLOG:
                if (input.one)
                    return {ZeroProof::ProvenZero, false, "0", false};
                if (auto exponent = powerOfTenExponent(input.identity))
                {
                    return integerFacts(*exponent);
                }
                return {ZeroProof::Unknown, false, NextIdentity(), false};
            case Command::CommandPOWE:
                return {ZeroProof::ProvenNonZero, input.zero == ZeroProof::ProvenZero,
                        input.zero == ZeroProof::ProvenZero ? "+1e0" : NextIdentity(), true};
            default:
                return {};
            }
        }

        unsigned UnaryCost(Command command) const
        {
            switch (command)
            {
            case Command::CommandSQR:
                // mul(x, x) duplicates the retained lazy graph; charge
                // exponentially growing squares conservatively.
                return 8u;
            case Command::CommandSIN:
                return m_angle == Command::CommandRAD ? 2u : 6u;
            case Command::CommandCOS:
                return m_angle == Command::CommandRAD ? 5u : 9u;
            case Command::CommandTAN:
                return m_angle == Command::CommandRAD ? 7u : 11u;
            case Command::CommandLOG:
                return 4u;
            default:
                return 1u;
            }
        }

        Real ToRadians(Real value) const
        {
            if (m_angle == Command::CommandRAD)
            {
                return value;
            }
            Real pi = Pi();
            int divisor = m_angle == Command::CommandGRAD ? 200 : 180;
            return epx::mul(std::move(value), epx::mul(std::move(pi), epx::inv(rational(integer(divisor)))));
        }

        static Real Pi()
        {
            Real one = rational(integer(1));
            return epx::mul(rational(integer(4)), epx::arctan(std::move(one)));
        }

        void Unary(Command command)
        {
            if (m_completed)
            {
                SeedFromResult();
            }
            CommitInput();
            if (m_tokens.empty())
            {
                AddZeroOperand();
            }
            size_t begin = CurrentOperandStart();
            ParseResult oldValue = ParseRange(begin, m_tokens.size());
            unsigned cost = UnaryCost(command);
            if (oldValue.lazyDepth > MaxParseDepth - cost || oldValue.operationCount > MaxOperations - cost)
            {
                throw ResourceLimitError("lazy expression limit");
            }
            const wstring operandDisplay = std::holds_alternative<LeftParenToken>(m_tokens[begin])
                ? ExpressionText(begin + 1, m_tokens.size() - 1, false)
                : OperandDisplay(begin);
            wstring label = UnaryName(command) + L"(" + operandDisplay + L")";
            PublishExpressionText(ExpressionText(0, begin, false) + label);
            Real newValue = ApplyUnary(command, std::move(oldValue.value), oldValue.facts);
            ValueFacts facts = UnaryFacts(command, oldValue.facts);
            ReplaceCurrentOperand(
                std::move(newValue), std::move(label), oldValue.lazyDepth + cost, oldValue.operationCount + cost, std::move(facts));
        }

        void UnarySign()
        {
            if (m_completed)
            {
                SeedFromResult();
            }
            if (m_tokens.empty() || isOperator(m_tokens.back()) || std::holds_alternative<LeftParenToken>(m_tokens.back()))
            {
                m_input = L"-";
                PublishInput();
                return;
            }
            size_t begin = CurrentOperandStart();
            ParseResult oldValue = ParseRange(begin, m_tokens.size());
            if (oldValue.lazyDepth >= MaxParseDepth || oldValue.operationCount >= MaxOperations)
            {
                throw ResourceLimitError("lazy expression limit");
            }
            Real value = negate(std::move(oldValue.value));
            ValueFacts facts = oldValue.facts;
            facts.one = false;
            facts.positive = false;
            facts.identity = negateIdentity(std::move(facts.identity));
            if (facts.multiple)
                epx::negate(facts.multiple->numerator);
            ReplaceCurrentOperand(
                std::move(value),
                L"-(" + OperandDisplay(begin) + L")",
                oldValue.lazyDepth + 1,
                oldValue.operationCount + 1,
                std::move(facts));
        }

        wstring UnaryName(Command command) const
        {
            switch (command)
            {
            case Command::CommandSQR:
                return L"sqr";
            case Command::CommandSQRT:
                return L"\x221A";
            case Command::CommandREC:
                return L"1/";
            case Command::CommandSIN:
                return m_angle == Command::CommandDEG ? L"sin\x2080"
                    : m_angle == Command::CommandRAD   ? L"sin\x1D63"
                                                     : L"sin\x1D4D";
            case Command::CommandCOS:
                return m_angle == Command::CommandDEG ? L"cos\x2080"
                    : m_angle == Command::CommandRAD   ? L"cos\x1D63"
                                                     : L"cos\x1D4D";
            case Command::CommandTAN:
                return m_angle == Command::CommandDEG ? L"tan\x2080"
                    : m_angle == Command::CommandRAD   ? L"tan\x1D63"
                                                     : L"tan\x1D4D";
            case Command::CommandLN:
                return L"ln";
            case Command::CommandLOG:
                return L"log";
            case Command::CommandPOWE:
                return L"e^";
            default:
                return L"?";
            }
        }

        wstring OperandDisplay(size_t begin) const
        {
            return ExpressionText(begin, m_tokens.size(), false);
        }

        void Constant(Command command)
        {
            StartFreshInputIfCompleted();
            if (!m_input.empty() || endsWithOperand(m_tokens))
            {
                return;
            }
            CheckTokenLimit();
            Real value = command == Command::CommandPI ? Pi() : epx::exp(rational(integer(1)));
            wstring name = command == Command::CommandPI ? L"pi" : L"e";
            unsigned depth = command == Command::CommandPI ? 2u : 1u;
            unsigned operations = depth;
            ValueFacts facts{ZeroProof::ProvenNonZero, false, command == Command::CommandPI ? "pi" : "e", true};
            m_tokens.emplace_back(NumberToken{
                make_shared<Real>(value),
                "<constant>",
                std::move(name),
                {m_tokens.size(), m_tokens.size() + 1},
                depth,
                operations,
                facts});
            m_result = std::move(value);
            m_resultLazyDepth = depth;
            m_resultOperationCount = operations;
            m_resultFacts = std::move(facts);
            m_resultText = Format(*m_result, m_resultFacts);
            m_replaceOperandOnInput = true;
            PublishAll();
        }

        void ToggleFormat()
        {
            m_scientificFormat = !m_scientificFormat;
            if (!m_result && m_input.empty())
            {
                m_result = rational(integer(0));
                m_resultFacts = integerFacts(0);
                m_resultLazyDepth = 0;
                m_resultOperationCount = 0;
            }
            if (!m_input.empty())
            {
                CommitInput();
                size_t begin = CurrentOperandStart();
                ParseResult result = ParseRange(begin, m_tokens.size());
                m_result = std::move(result.value);
                m_resultLazyDepth = result.lazyDepth;
                m_resultOperationCount = result.operationCount;
                m_resultFacts = std::move(result.facts);
                m_replaceOperandOnInput = true;
            }
            if (m_result)
            {
                m_resultText = Format(*m_result, m_resultFacts);
                PublishPrimary();
                PublishExpression();
            }
        }

        void Equals()
        {
            if (m_completed)
            {
                return;
            }
            CommitInput();
            if (m_tokens.empty())
            {
                return;
            }
            CompleteTrailingOperator();
            if (!endsWithOperand(m_tokens))
            {
                throw ParseError("incomplete expression");
            }
            CloseRemainingParentheses();
            ParseResult result = PrattParser(m_tokens).Parse();
            ensureMagnitude(result.value);
            wstring expression = ExpressionText();
            if (result.facts.zero != ZeroProof::ProvenZero && result.facts.identity.empty())
            {
                result.facts.identity = NextIdentity();
            }
            m_result = std::move(result.value);
            m_resultLazyDepth = result.lazyDepth;
            m_resultOperationCount = result.operationCount;
            m_resultFacts = std::move(result.facts);
            m_resultText = Format(*m_result, m_resultFacts);
            m_completed = true;
            PublishAll();
            AppendHistory(expression);
        }

        static void TrimFraction(string& number)
        {
            size_t dot = number.find('.');
            if (dot != string::npos)
            {
                while (!number.empty() && number.back() == '0')
                {
                    number.pop_back();
                }
                if (!number.empty() && number.back() == '.')
                {
                    number.pop_back();
                }
            }
            if (number == "-0")
            {
                number = "0";
            }
        }

        static string ToScientific(string fixed)
        {
            bool negative = fixed.front() == '-';
            size_t first = negative ? 1 : 0;
            size_t decimal = fixed.find('.');
            if (decimal == string::npos)
            {
                decimal = fixed.size();
            }
            size_t firstNonZero = fixed.find_first_not_of('0', first);
            if (firstNonZero == decimal)
            {
                firstNonZero = fixed.find_first_not_of('0', decimal + 1);
            }
            if (firstNonZero == string::npos)
            {
                return "0";
            }

            int exponent = firstNonZero < decimal ? static_cast<int>(decimal - firstNonZero - 1)
                : -static_cast<int>(firstNonZero - decimal);
            string digits;
            for (size_t i = firstNonZero; i < fixed.size(); ++i)
            {
                if (fixed[i] != '.')
                {
                    digits.push_back(fixed[i]);
                }
            }

            while (digits.size() > 1 && digits.back() == '0')
            {
                digits.pop_back();
            }
            string result = negative ? "-" : "";
            result.push_back(digits.front());
            result.push_back('.');
            if (digits.size() > 1)
            {
                result.append(digits.substr(1));
            }
            result.push_back('e');
            if (exponent >= 0)
            {
                result.push_back('+');
            }
            result.append(std::to_string(exponent));
            return result;
        }

        vector<unsigned> DecimalGrouping() const
        {
            wstring specification = m_resourceProvider->GetCEngineString(L"sGrouping");
            if (specification.empty())
            {
                specification = L"3;0";
            }
            vector<unsigned> groups;
            unsigned value = 0;
            bool hasDigit = false;
            for (wchar_t character : specification)
            {
                if (character >= L'0' && character <= L'9')
                {
                    hasDigit = true;
                    value = value * 10 + static_cast<unsigned>(character - L'0');
                    if (value > 16)
                    {
                        return {};
                    }
                }
                else if (hasDigit)
                {
                    groups.push_back(value);
                    value = 0;
                    hasDigit = false;
                }
            }
            if (hasDigit)
            {
                groups.push_back(value);
            }
            return groups;
        }

        wstring LocalizeAndGroup(const string& canonical) const
        {
            if (canonical.find('e') != string::npos)
            {
                wstring scientific = widen(canonical);
                wchar_t decimalSeparator = DecimalSeparator();
                if (decimalSeparator != L'.')
                {
                    std::replace(scientific.begin(), scientific.end(), L'.', decimalSeparator);
                }
                return scientific;
            }

            size_t signLength = !canonical.empty() && canonical.front() == '-' ? 1 : 0;
            size_t decimal = canonical.find('.');
            size_t integerEnd = decimal == string::npos ? canonical.size() : decimal;
            wstring integerPart = widen(string_view(canonical).substr(signLength, integerEnd - signLength));
            vector<unsigned> groups = DecimalGrouping();
            wstring delimiter = m_resourceProvider->GetCEngineString(L"sThousand");
            if (delimiter.empty())
            {
                delimiter = L",";
            }
            if (!groups.empty() && groups.front() != 0)
            {
                size_t remaining = integerPart.size();
                size_t groupIndex = 0;
                unsigned groupSize = groups.front();
                while (groupSize != 0 && remaining > groupSize)
                {
                    remaining -= groupSize;
                    integerPart.insert(remaining, delimiter);
                    if (groupIndex + 1 >= groups.size())
                    {
                        if (groups[groupIndex] == 0)
                        {
                            continue;
                        }
                        break;
                    }
                    unsigned next = groups[++groupIndex];
                    if (next != 0)
                    {
                        groupSize = next;
                    }
                }
            }

            wstring result = signLength ? L"-" : L"";
            result += integerPart;
            if (decimal != string::npos)
            {
                result.push_back(DecimalSeparator());
                result += widen(string_view(canonical).substr(decimal + 1));
            }
            return result;
        }

        wstring Format(const Real& value, const ValueFacts& facts) const
        {
            if (facts.zero == ZeroProof::ProvenZero)
            {
                return m_scientificFormat ? LocalizeAndGroup("0.e+0") : L"0";
            }
            bool negative = false;
            if (!facts.positive)
            {
                auto classified = boundedApproximation(value, ClassificationPrecision);
                if (epx::is_zero(classified))
                {
                    throw ResourceLimitError("display zero classification limit");
                }
                negative = epx::is_negative(classified);
            }

            Real absolute = value;
            if (negative)
            {
                absolute = negate(std::move(absolute));
            }
            auto integerPart = absolute.approx(0).get();
            size_t integerDigits = epx::is_zero(integerPart) ? 0 : epx::to_string(integerPart).size();
            string nearUnitProbe;
            if (integerDigits == 0)
            {
                nearUnitProbe = epx::to_string(absolute, MaxFractionalPlaces);
                size_t decimal = nearUnitProbe.find('.');
                size_t integerNonZero = nearUnitProbe.find_first_not_of('0');
                if (integerNonZero != string::npos && integerNonZero < decimal)
                {
                    integerDigits = decimal;
                }
            }
            unsigned precision = static_cast<unsigned>(m_precision);
            string fixed;
            bool forceScientific = false;

            if (integerDigits > precision)
            {
                unsigned decimalExponent = static_cast<unsigned>(integerDigits - 1);
                Real scale = rational(epx::details::pow10<Container>(decimalExponent));
                Real normalized = epx::mul(value, epx::inv(std::move(scale)));
                fixed = epx::to_string(normalized, precision - 1);
                TrimFraction(fixed);
                if (fixed == "10" || fixed == "-10")
                {
                    fixed = fixed.front() == '-' ? "-1" : "1";
                    ++decimalExponent;
                }
                if (fixed.find('.') == string::npos)
                {
                    fixed.push_back('.');
                }
                fixed += "e+" + std::to_string(decimalExponent);
                forceScientific = true;
            }
            else if (integerDigits > 0)
            {
                fixed = epx::to_string(value, precision - static_cast<unsigned>(integerDigits));
            }
            else
            {
                size_t decimal = nearUnitProbe.find('.');
                size_t firstNonZero = nearUnitProbe.find_first_not_of('0', decimal + 1);
                if (firstNonZero == string::npos)
                {
                    throw ResourceLimitError("display magnitude limit");
                }
                unsigned leadingZeros = static_cast<unsigned>(firstNonZero - decimal - 1);
                if (leadingZeros > MaxFractionalPlaces - precision)
                {
                    throw ResourceLimitError("display precision limit");
                }
                fixed = epx::to_string(value, leadingZeros + precision);
            }

            TrimFraction(fixed);
            if (m_scientificFormat && fixed != "0" && !forceScientific)
            {
                fixed = ToScientific(std::move(fixed));
            }
            if (fixed.size() > 1024)
            {
                throw ResourceLimitError("formatted output limit");
            }
            return LocalizeAndGroup(fixed);
        }

        wstring TokenDisplay(const Token& token) const
        {
            if (const auto* number = std::get_if<NumberToken>(&token))
            {
                return number->display;
            }
            if (std::holds_alternative<PlusToken>(token))
                return L"+";
            if (std::holds_alternative<MinusToken>(token))
                return L"-";
            if (std::holds_alternative<MultiplyToken>(token))
                return L"\u00d7";
            if (std::holds_alternative<DivideToken>(token))
                return L"\u00f7";
            if (std::holds_alternative<LeftParenToken>(token))
                return L"(";
            return L")";
        }

        wstring ExpressionText(size_t begin, size_t end, bool includeEquals) const
        {
            wstring result;
            for (size_t position = begin; position < end; ++position)
            {
                const Token& token = m_tokens[position];
                if (isOperator(token))
                {
                    result += L" ";
                    result += TokenDisplay(token);
                    result += L" ";
                }
                else
                {
                    result += TokenDisplay(token);
                }
            }
            if (includeEquals)
                result += L"=";
            return result;
        }

        wstring ExpressionText() const
        {
            return ExpressionText(0, m_tokens.size(), m_completed);
        }

        void AppendHistory(const wstring& expression)
        {
            if (!m_history)
            {
                return;
            }
            auto tokens = make_shared<vector<std::pair<wstring, int>>>();
            tokens->emplace_back(expression + L"=", -1);
            auto commands = make_shared<vector<shared_ptr<IExpressionCommand>>>();
            unsigned index = m_history->AddToHistory(tokens, commands, m_resultText);
            if (m_display)
            {
                m_display->OnHistoryItemAdded(index);
            }
        }

        void PublishInput()
        {
            m_resultText = m_input.empty() ? L"0"
                : m_exponentBase          ? m_input
                                          : LocalizeAndGroup(narrowNumber(m_input, DecimalSeparator()));
            PublishPrimary();
            PublishExpression();
            if (m_display)
            {
                m_display->InputChanged();
            }
        }

        void PublishPrimary()
        {
            if (m_display)
            {
                m_display->SetPrimaryDisplay(m_resultText, m_error);
            }
        }

        void PublishExpression()
        {
            if (!m_display)
            {
                return;
            }
            PublishExpressionText(ExpressionText());
        }

        void PublishExpressionText(wstring expression)
        {
            if (!m_display)
            {
                return;
            }
            auto tokens = make_shared<vector<std::pair<wstring, int>>>();
            if (!expression.empty())
            {
                tokens->emplace_back(std::move(expression), -1);
            }
            auto commands = make_shared<vector<shared_ptr<IExpressionCommand>>>();
            m_display->SetExpressionDisplay(tokens, commands);
            m_display->SetParenthesisNumber(m_openParentheses);
        }

        void PublishAll()
        {
            if (m_display)
            {
                m_display->SetIsInError(m_error);
            }
            PublishPrimary();
            PublishExpression();
            if (m_display)
            {
                m_display->InputChanged();
            }
        }

        IResourceProvider* m_resourceProvider;
        ICalcDisplay* m_display;
        shared_ptr<CalculatorHistory> m_history;
        vector<Token> m_tokens;
        wstring m_input;
        std::optional<Real> m_result;
        unsigned m_resultLazyDepth = 0;
        unsigned m_resultOperationCount = 0;
        ValueFacts m_resultFacts;
        std::optional<ParseResult> m_exponentBase;
        size_t m_exponentOperandStart = 0;
        wstring m_exponentBaseDisplay;
        wstring m_errorExpressionCandidate;
        wstring m_resultText = L"0";
        int32_t m_precision = 32;
        Command m_angle = Command::CommandDEG;
        unsigned m_openParentheses = 0;
        bool m_error = false;
        bool m_completed = false;
        bool m_replaceOperandOnInput = false;
        bool m_scientificFormat = false;
        uint64_t m_nextIdentity = 0;
    };

    EpsilonEngine::EpsilonEngine(
        IResourceProvider* resourceProvider,
        ICalcDisplay* displayCallback,
        shared_ptr<CalculatorHistory> history)
        : m_impl(std::make_unique<Impl>(resourceProvider, displayCallback, std::move(history)))
    {
    }

    EpsilonEngine::~EpsilonEngine() = default;

    bool EpsilonEngine::IsCommandSupported(Command command) noexcept
    {
        if (command >= Command::Command0 && command <= Command::Command9)
        {
            return true;
        }
        switch (command)
        {
        case Command::CommandDEG:
        case Command::CommandRAD:
        case Command::CommandGRAD:
        case Command::CommandSIGN:
        case Command::CommandCLEAR:
        case Command::CommandCENTR:
        case Command::CommandBACK:
        case Command::CommandPNT:
        case Command::CommandDIV:
        case Command::CommandMUL:
        case Command::CommandADD:
        case Command::CommandSUB:
        case Command::CommandSIN:
        case Command::CommandCOS:
        case Command::CommandTAN:
        case Command::CommandLN:
        case Command::CommandLOG:
        case Command::CommandSQRT:
        case Command::CommandSQR:
        case Command::CommandREC:
        case Command::CommandFE:
        case Command::CommandPI:
        case Command::CommandEQU:
        case Command::CommandEXP:
        case Command::CommandOPENP:
        case Command::CommandCLOSEP:
        case Command::CommandPOWE:
        case Command::CommandEuler:
            return true;
        default:
            return false;
        }
    }

    void EpsilonEngine::ProcessCommand(Command command)
    {
        if (!IsCommandSupported(command))
        {
            throw std::invalid_argument("unsupported EpsilonEngine command");
        }
        m_impl->Process(command);
    }

    void EpsilonEngine::Reset()
    {
        m_impl->Reset();
    }

    bool EpsilonEngine::IsInputEmpty() const
    {
        return m_impl->IsInputEmpty();
    }

    bool EpsilonEngine::IsEngineRecording() const
    {
        return m_impl->IsRecording();
    }

    void EpsilonEngine::SetPrecision(int32_t precision)
    {
        m_impl->SetPrecision(precision);
    }

    wchar_t EpsilonEngine::DecimalSeparator() const
    {
        return m_impl->DecimalSeparator();
    }

    wstring EpsilonEngine::GetResult() const
    {
        return m_impl->Result();
    }

    void EpsilonEngine::DisplayError(int32_t errorCode)
    {
        m_impl->DisplayError(errorCode);
    }
}
