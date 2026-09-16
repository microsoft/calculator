// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "EpsilonEngine.h"
#include "../Epsilon/chars.hpp"
#include "../Epsilon/r.hpp"
#include <algorithm>
#include <charconv>
#include <cctype>
#include <optional>
#include <limits>
#include <variant>
#include <vector>
namespace CalculationManager::Numerics
{
    using std::string;
    using std::string_view;
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

    class ParseError : public ExpressionException
    {
    public:
        explicit ParseError(const char* message)
            : ExpressionException(ExpressionError::Syntax, message)
        {
        }
    };

    class ResourceLimitError : public ExpressionException
    {
    public:
        explicit ResourceLimitError(const char* message)
            : ExpressionException(ExpressionError::ResourceLimit, message)
        {
        }
    };

    class IndefiniteError : public ExpressionException
    {
    public:
        IndefiniteError()
            : ExpressionException(ExpressionError::Undefined, "indefinite result")
        {
        }
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

    struct PlusToken
    {
    };
    struct MinusToken
    {
    };
    struct MultiplyToken
    {
    };
    struct DivideToken
    {
    };
    using Token = std::variant<PlusToken, MinusToken, MultiplyToken, DivideToken>;
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
        return { facts.zero, facts.one, facts.identity, facts.positive, facts.multiple };
    }

    ValueFacts valueFacts(const ExactValueProof& proof)
    {
        return { proof.zero, proof.one, proof.identity, proof.positive, {}, {}, proof.multiple };
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
            return { ZeroProof::ProvenZero, false, "0", false };
        }
        string identity = value < 0 ? "-" : "+";
        identity += std::to_string(std::abs(value));
        identity += "e0";
        return { ZeroProof::ProvenNonZero, value == 1, std::move(identity), value > 0 };
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
        return ExactDecimalAtom{ coefficient, negativeExponent ? -exponent : exponent };
    }

    bool scaleExactDecimal(int64_t& coefficient, int places)
    {
        for (int place = 0; place < places; ++place)
        {
            if (coefficient > std::numeric_limits<int64_t>::max() / 10 || coefficient < std::numeric_limits<int64_t>::min() / 10)
            {
                return false;
            }
            coefficient *= 10;
        }
        return true;
    }

    std::optional<string> exactDecimalAddIdentity(string_view leftIdentity, string_view rightIdentity, bool subtractRight)
    {
        auto left = exactDecimalAtom(leftIdentity);
        auto right = exactDecimalAtom(rightIdentity);
        if (!left || !right)
        {
            return std::nullopt;
        }
        int exponent = std::min(left->exponent, right->exponent);
        if (!scaleExactDecimal(left->coefficient, left->exponent - exponent) || !scaleExactDecimal(right->coefficient, right->exponent - exponent))
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
        if ((right->coefficient > 0 && left->coefficient > std::numeric_limits<int64_t>::max() - right->coefficient)
            || (right->coefficient < 0 && left->coefficient < std::numeric_limits<int64_t>::min() - right->coefficient))
        {
            return std::nullopt;
        }
        int64_t coefficient = left->coefficient + right->coefficient;
        if (coefficient == 0)
        {
            return string{ "0" };
        }
        while (coefficient % 10 == 0)
        {
            coefficient /= 10;
            ++exponent;
        }
        uint64_t magnitude = coefficient < 0 ? static_cast<uint64_t>(-(coefficient + 1)) + 1 : static_cast<uint64_t>(coefficient);
        return string(coefficient < 0 ? "-" : "+") + std::to_string(magnitude) + "e" + std::to_string(exponent);
    }

    ValueFacts exactDecimalFacts(const string& identity)
    {
        if (identity == "0")
        {
            return { ZeroProof::ProvenZero, false, "0", false };
        }
        return { ZeroProof::ProvenNonZero, identity == "+1e0", identity, !identity.empty() && identity.front() == '+' };
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
        uint64_t high = std::min(significand, uint64_t{ 0xffffffff });
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
            return { ZeroProof::ProvenZero, false, "0", false };
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
        return { ZeroProof::ProvenNonZero, one, std::move(identity), !negative };
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
        return ExactMultiple{ std::move(numerator), std::move(denominator), timesPi };
    }

    std::optional<ExactMultiple> exactMultiple(const ValueFacts& facts)
    {
        if (facts.multiple)
            return facts.multiple;
        const auto one = epx::create<Container>(1);
        if (facts.zero == ZeroProof::ProvenZero)
            return ExactMultiple{ Integer{}, one, false };
        if (facts.identity == "pi" || facts.identity == "neg(pi)")
            return ExactMultiple{ epx::create<Container>(facts.identity == "pi" ? 1 : -1), one, true };

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
        if (parsed.ec != std::errc{} || parsed.ptr != exponentText.data() + exponentText.size() || exponent < -maxScale || exponent > maxScale)
            return std::nullopt;
        auto scale = epx::details::pow10<Container>(static_cast<unsigned>(std::abs(exponent)));
        return exponent < 0 ? boundedMultiple(std::move(*coefficient), std::move(scale), false) : boundedMultiple(epx::mul(*coefficient, scale), one, false);
    }

    std::optional<ExactMultiple> combineMultiples(const Token& operation, const ValueFacts& left, const ValueFacts& right)
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
            numerator = std::holds_alternative<PlusToken>(operation) ? epx::add(numerator, other) : epx::sub(numerator, other);
            return boundedMultiple(std::move(numerator), epx::mul(lhs->denominator, rhs->denominator), lhs->timesPi);
        }
        if (std::holds_alternative<MultiplyToken>(operation))
        {
            if (lhs->timesPi && rhs->timesPi)
                return std::nullopt;
            return boundedMultiple(epx::mul(lhs->numerator, rhs->numerator), epx::mul(lhs->denominator, rhs->denominator), lhs->timesPi || rhs->timesPi);
        }
        if (!lhs->timesPi && rhs->timesPi)
            return std::nullopt;
        return boundedMultiple(epx::mul(lhs->numerator, rhs->denominator), epx::mul(lhs->denominator, rhs->numerator), lhs->timesPi && !rhs->timesPi);
    }

    ValueFacts basicBinaryFacts(const Token& operation, const ValueFacts& left, const ValueFacts& right)
    {
        ValueFacts result;
        if (std::holds_alternative<PlusToken>(operation))
        {
            if (auto identity = exactDecimalAddIdentity(left.identity, right.identity, false))
                return exactDecimalFacts(*identity);
            if (areOppositeIdentities(left.identity, right.identity))
                return { ZeroProof::ProvenZero, false, "0", false };
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
                return { ZeroProof::ProvenZero, false, "0", false };
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
                return { ZeroProof::ProvenZero, false, "0", false };
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
                return { ZeroProof::ProvenZero, false, "0", false };
            if (right.one)
                return left;
            if (left.zero == ZeroProof::ProvenNonZero && right.zero == ZeroProof::ProvenNonZero)
                result.zero = ZeroProof::ProvenNonZero;
            result.one = right.zero == ZeroProof::ProvenNonZero && !left.identity.empty() && left.identity == right.identity;
            if (result.one)
                result.identity = "+1e0";
            result.positive = left.positive && right.positive;
            if (left.zero == ZeroProof::ProvenNonZero && right.zero == ZeroProof::ProvenNonZero && !right.identity.empty())
            {
                result.multiplyCancellation = FactorCancellationProof{ right.identity, exactValueProof(left) };
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

    int certifiedSign(const Real& value)
    {
        for (int precision = 8;; precision = std::min(ClassificationPrecision, precision * 2))
        {
            auto approximation = boundedApproximation(value, precision);
            if (compareSigned(approximation, integer(1)) > 0)
                return 1;
            if (compareSigned(approximation, integer(-1)) < 0)
                return -1;
            if (precision == ClassificationPrecision)
                throw ResourceLimitError("sign classification limit");
        }
    }

    void requireNonZero(const Real& value, const ValueFacts& facts)
    {
        if (facts.zero == ZeroProof::ProvenZero)
        {
            throw epx::divide_by_zero_error{};
        }
        if (facts.zero == ZeroProof::Unknown)
        {
            certifiedSign(value);
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

    inline void TrimFraction(string& number)
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

    inline string ToScientific(string fixed)
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

        int exponent = firstNonZero < decimal ? static_cast<int>(decimal - firstNonZero - 1) : -static_cast<int>(firstNonZero - decimal);
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

    string Format(const Real& value, const ValueFacts& facts, int32_t m_precision, bool m_scientificFormat)
    {
        if (facts.zero == ZeroProof::ProvenZero)
        {
            return m_scientificFormat ? string("0.e+0") : string("0");
        }
        bool negative = false;
        if (!facts.positive)
        {
            negative = certifiedSign(value) < 0;
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
            for (unsigned places = 8;; places = std::min(MaxFractionalPlaces, places * 2))
            {
                nearUnitProbe = epx::to_string(absolute, places);
                if (nearUnitProbe.find_first_of("123456789") != string::npos || places == MaxFractionalPlaces)
                    break;
            }
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
        return fixed;
    }

}
