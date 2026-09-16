// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Numerics.h"
#include "Evaluator.h"
#include "Syntax.h"

#include <utility>

namespace CalculationManager
{
    namespace
    {
        using namespace Numerics;
        struct Computed
        {
            Real value;
            ValueFacts facts;
        };

        Computed Exact(Integer numerator, Integer denominator = integer(1))
        {
            auto multiple = boundedMultiple(std::move(numerator), std::move(denominator), false);
            if (!multiple)
                throw ResourceLimitError("Exact rational magnitude limit");
            auto value = rational(multiple->numerator, multiple->denominator);
            ValueFacts facts;
            facts.zero = epx::is_zero(multiple->numerator) ? ZeroProof::ProvenZero : ZeroProof::ProvenNonZero;
            facts.positive = facts.zero != ZeroProof::ProvenZero && !epx::is_negative(multiple->numerator);
            facts.one = compareSigned(multiple->numerator, multiple->denominator) == 0;
            facts.identity = facts.zero == ZeroProof::ProvenZero ? "0"
                             : facts.one ? "+1e0"
                                         : "q(" + epx::to_string(multiple->numerator) + "," + epx::to_string(multiple->denominator) + ")";
            if (compareSigned(multiple->denominator, integer(1)) == 0)
                facts.identity = literalFacts(epx::to_string(multiple->numerator)).identity;
            facts.multiple = std::move(multiple);
            return { std::move(value), std::move(facts) };
        }
        Computed Exact(int value)
        {
            return Exact(integer(value));
        }
        Computed Pi()
        {
            return { epx::mul(rational(integer(4)), epx::arctan(rational(integer(1)))), { ZeroProof::ProvenNonZero, false, "pi", true } };
        }
        int Sign(const Computed& value)
        {
            if (value.facts.zero == ZeroProof::ProvenZero)
                return 0;
            if (value.facts.positive)
                return 1;
            if (auto multiple = exactMultiple(value.facts))
                return epx::is_zero(multiple->numerator) ? 0 : epx::is_negative(multiple->numerator) ? -1 : 1;
            return certifiedSign(value.value);
        }
        Computed Neg(Computed value)
        {
            value.value = negate(std::move(value.value));
            value.facts.one = false;
            value.facts.positive = false;
            value.facts.identity = negateIdentity(std::move(value.facts.identity));
            if (value.facts.multiple)
                epx::negate(value.facts.multiple->numerator);
            if (auto multiple = exactMultiple(value.facts); multiple && !multiple->timesPi)
                return Exact(multiple->numerator, multiple->denominator);
            return value;
        }
        Computed Binary(const std::string& operation, Computed left, Computed right)
        {
            Token token = operation == "+"   ? Token{ PlusToken{} }
                          : operation == "-" ? Token{ MinusToken{} }
                          : operation == "*" ? Token{ MultiplyToken{} }
                                             : Token{ DivideToken{} };
            if (operation == "/")
            {
                if (right.facts.zero == ZeroProof::ProvenZero && left.facts.zero == ZeroProof::ProvenZero)
                    throw IndefiniteError{};
                requireNonZero(right.value, right.facts);
                right.facts.zero = ZeroProof::ProvenNonZero;
            }
            auto facts = binaryFacts(token, left.facts, right.facts);
            // A rational certificate is also an exact result, not a rounded approximation.
            if (auto multiple = exactMultiple(facts); multiple && !multiple->timesPi)
                return Exact(multiple->numerator, multiple->denominator);
            auto result = operation == "+"   ? epx::add(left.value, right.value)
                          : operation == "-" ? subtract(left.value, right.value)
                          : operation == "*" ? epx::mul(left.value, right.value)
                                             : epx::mul(left.value, epx::inv(right.value));
            if (facts.identity.empty() && !left.facts.identity.empty() && !right.facts.identity.empty())
                facts.identity = "(" + left.facts.identity + operation + right.facts.identity + ")";
            if (facts.zero == ZeroProof::ProvenZero)
                result = rational(integer(0));
            return { std::move(result), std::move(facts) };
        }
        Computed Floor(const Computed& value)
        {
            if (auto multiple = exactMultiple(value.facts); multiple && !multiple->timesPi)
                return Exact(epx::floor_div(multiple->numerator, multiple->denominator).q);
            // approx(p) has absolute error <= 4^-p. Both interval ends must agree.
            auto approximation = boundedApproximation(value.value, ClassificationPrecision);
            auto scale = epx::mul_4exp(integer(1), ClassificationPrecision);
            auto lower = epx::floor_div(epx::sub(approximation, integer(1)), scale).q;
            auto upper = epx::floor_div(epx::add(approximation, integer(1)), scale).q;
            if (compareSigned(lower, upper) != 0)
                throw ResourceLimitError("Integer boundary could not be certified");
            return Exact(std::move(lower));
        }
        Computed Truncate(Computed value)
        {
            return Sign(value) < 0 ? Neg(Floor(Neg(value))) : Floor(value);
        }
        Computed Exp(Computed value)
        {
            ensureExponentialArgument(value.value);
            if (value.facts.zero == ZeroProof::ProvenZero)
                return Exact(1);
            return { epx::exp(value.value), { ZeroProof::ProvenNonZero, false, "exp(" + value.facts.identity + ")", true } };
        }
        Computed Log(Computed value)
        {
            if (Sign(value) <= 0)
                throw ExpressionException(ExpressionError::Domain, "Logarithm requires a positive argument");
            if (value.facts.one)
                return Exact(0);
            return { epx::log(value.value), { ZeroProof::Unknown, false, "ln(" + value.facts.identity + ")", false } };
        }
        Computed Power(Computed base, Computed exponent)
        {
            auto rationalExponent = exactMultiple(exponent.facts);
            const int sign = Sign(base);
            if (exponent.facts.zero == ZeroProof::ProvenZero)
            {
                if (sign == 0)
                    throw IndefiniteError{};
                return Exact(1);
            }
            if (sign == 0)
            {
                if (Sign(exponent) < 0)
                    throw ExpressionException(ExpressionError::DivideByZero, "Negative power of zero");
                return Exact(0);
            }
            if (rationalExponent && !rationalExponent->timesPi)
            {
                const auto& numerator = rationalExponent->numerator;
                const auto& denominator = rationalExponent->denominator;
                if (compareSigned(denominator, integer(1)) == 0)
                {
                    auto magnitude = numerator;
                    magnitude.sgn = epx::sign::positive;
                    if (compareSigned(magnitude, integer(4096)) > 0)
                        throw ResourceLimitError("Integer power work limit");
                    unsigned count = epx::is_zero(magnitude) ? 0 : magnitude.digits.front();
                    auto result = Exact(1);
                    while (count)
                    {
                        if (count & 1)
                            result = Binary("*", std::move(result), base);
                        count >>= 1;
                        if (count)
                            base = Binary("*", base, base);
                        ensureMagnitude(result.value);
                    }
                    return epx::is_negative(numerator) ? Binary("/", Exact(1), std::move(result)) : result;
                }
                if (auto rationalBase = exactMultiple(base.facts); rationalBase && !rationalBase->timesPi && compareSigned(denominator, integer(32)) <= 0)
                {
                    int degree = static_cast<int>(denominator.digits.front());
                    if (sign < 0 && degree % 2 == 0)
                        throw ExpressionException(ExpressionError::Domain, "Even root of a negative value");
                    auto magnitude = rationalBase->numerator;
                    magnitude.sgn = epx::sign::positive;
                    auto p = epx::root(magnitude, degree);
                    auto q = epx::root(rationalBase->denominator, degree);
                    auto integerPower = [degree](Integer number)
                    {
                        auto result = integer(1);
                        for (int i = 0; i < degree; ++i)
                            result = epx::mul(result, number);
                        return result;
                    };
                    if (compareSigned(integerPower(p), magnitude) == 0 && compareSigned(integerPower(q), rationalBase->denominator) == 0)
                    {
                        if (sign < 0)
                            epx::negate(p);
                        return Power(Exact(p, q), Exact(numerator));
                    }
                }
                if (sign < 0)
                {
                    if (epx::floor_div(denominator, integer(2)).r.digits.empty())
                        throw ExpressionException(ExpressionError::Domain, "Even root of a negative value");
                    auto result = Exp(Binary("*", exponent, Log(Neg(base))));
                    return epx::is_zero(epx::floor_div(numerator, integer(2)).r) ? result : Neg(result);
                }
            }
            if (sign < 0)
                throw ExpressionException(ExpressionError::Domain, "Negative base requires an exact rational exponent");
            return Exp(Binary("*", exponent, Log(base)));
        }
        std::optional<unsigned> QuarterTurns(const ValueFacts& facts, const std::string& unit)
        {
            if (facts.zero == ZeroProof::ProvenZero)
                return 0;
            auto multiple = exactMultiple(facts);
            if (!multiple || multiple->timesPi != (unit == "rad"))
                return {};
            auto numerator = unit == "rad" ? epx::mul(multiple->numerator, integer(2)) : multiple->numerator;
            auto denominator = unit == "rad" ? multiple->denominator : epx::mul(multiple->denominator, integer(unit == "deg" ? 90 : 100));
            auto division = epx::floor_div(numerator, denominator);
            if (!epx::is_zero(division.r))
                return {};
            auto quadrant = epx::floor_div(division.q, integer(4)).r;
            return epx::is_zero(quadrant) ? 0u : quadrant.digits.front();
        }
        Computed Radians(Computed value, const std::string& unit)
        {
            return unit == "rad" ? value : Binary("/", Binary("*", value, Pi()), Exact(unit == "deg" ? 180 : 200));
        }
        Computed FromRadians(Computed value, const std::string& unit)
        {
            return unit == "rad" ? value : Binary("/", Binary("*", value, Exact(unit == "deg" ? 180 : 200)), Pi());
        }

        Computed Function(const std::string& name, Computed value, const std::string& unit);

        struct Enclosure
        {
            Integer lower;
            Integer upper;
        };

        class FixedIntervals
        {
        public:
            explicit FixedIntervals(int precision)
                : m_precision(precision)
                , m_scale(epx::mul_4exp(integer(1), precision))
            {
            }
            Enclosure Enclose(const Real& value) const
            {
                auto center = value.approx(m_precision).get();
                return { epx::sub(center, integer(1)), epx::add(center, integer(1)) };
            }
            Enclosure Int(int value) const
            {
                auto n = epx::mul(integer(value), m_scale);
                return { n, n };
            }
            Enclosure Add(const Enclosure& a, const Enclosure& b) const
            {
                return { epx::add(a.lower, b.lower), epx::add(a.upper, b.upper) };
            }
            Enclosure Ratio(const Integer& numerator, const Integer& denominator) const
            {
                if (compareSigned(denominator, integer(0)) <= 0)
                    throw ResourceLimitError("Invalid gamma rational bound");
                auto scaled = epx::mul(numerator, m_scale);
                return { epx::floor_div(scaled, denominator).q, epx::ceil_div(scaled, denominator).q };
            }
            Enclosure Neg(const Enclosure& value) const
            {
                auto lower = value.upper, upper = value.lower;
                epx::negate(lower);
                epx::negate(upper);
                return { lower, upper };
            }
            Enclosure Mul(const Enclosure& a, const Enclosure& b) const
            {
                Integer products[] = { epx::mul(a.lower, b.lower), epx::mul(a.lower, b.upper), epx::mul(a.upper, b.lower), epx::mul(a.upper, b.upper) };
                Integer lower = products[0], upper = products[0];
                for (const auto& product : products)
                {
                    if (compareSigned(product, lower) < 0)
                        lower = product;
                    if (compareSigned(product, upper) > 0)
                        upper = product;
                }
                return { epx::floor_div(lower, m_scale).q, epx::ceil_div(upper, m_scale).q };
            }
            Enclosure DivPositive(const Enclosure& a, const Enclosure& b) const
            {
                if (compareSigned(b.lower, integer(0)) <= 0)
                    throw ResourceLimitError("Gamma denominator classification limit");
                auto squared = epx::mul(m_scale, m_scale);
                return Mul(a, { epx::floor_div(squared, b.upper).q, epx::ceil_div(squared, b.lower).q });
            }
            Enclosure Exp(const Enclosure& value) const
            {
                return { Enclose(epx::exp(rational(value.lower, m_scale))).lower, Enclose(epx::exp(rational(value.upper, m_scale))).upper };
            }
            Enclosure Log(const Enclosure& value) const
            {
                if (compareSigned(value.lower, integer(0)) <= 0)
                    throw ResourceLimitError("Gamma logarithm classification limit");
                return { Enclose(epx::log(rational(value.lower, m_scale))).lower, Enclose(epx::log(rational(value.upper, m_scale))).upper };
            }
            Integer GammaApproximation(Enclosure approximation, int terms, int precision) const
            {
                // Spouge's relative remainder is < 6^-terms for positive x.
                // Include both arithmetic enclosure and truncation error.
                if (compareSigned(approximation.lower, integer(0)) <= 0)
                    throw ResourceLimitError("Gamma cancellation precision limit");
                Integer denominator = integer(1);
                for (int i = 0; i < terms; ++i)
                    denominator = epx::mul(denominator, integer(6));
                auto error = epx::ceil_div(epx::mul(approximation.upper, integer(2)), denominator).q;
                auto scale = epx::mul_4exp(integer(1), m_precision - precision);
                auto lower = epx::floor_div(epx::sub(approximation.lower, error), scale).q;
                auto upper = epx::ceil_div(epx::add(approximation.upper, error), scale).q;
                if (compareSigned(epx::sub(upper, lower), integer(2)) > 0)
                    throw ResourceLimitError("Gamma enclosure precision limit");
                return epx::floor_div(epx::add(lower, upper), integer(2)).q;
            }

        private:
            int m_precision;
            Integer m_scale;
        };

        Computed Factorial(Computed value)
        {
            auto exact = exactMultiple(value.facts);
            if (exact && !exact->timesPi && compareSigned(exact->denominator, integer(2)) == 0)
            {
                auto numerator = exact->numerator;
                auto magnitude = numerator;
                magnitude.sgn = epx::sign::positive;
                if (compareSigned(magnitude, integer(200)) > 0)
                    throw ResourceLimitError("Half-integer factorial limit");
                Computed result{ epx::root(Pi().value, 2), { ZeroProof::ProvenNonZero, false, "sqrt(pi)", true } };
                auto step = integer(-1);
                while (compareSigned(step, numerator) < 0)
                {
                    step = epx::add(step, integer(2));
                    result = Binary("*", result, Exact(step, integer(2)));
                }
                while (compareSigned(step, numerator) > 0)
                {
                    result = Binary("/", result, Exact(step, integer(2)));
                    step = epx::sub(step, integer(2));
                }
                return result;
            }
            if (exact && !exact->timesPi && compareSigned(exact->denominator, integer(1)) == 0)
            {
                if (epx::is_negative(exact->numerator))
                    throw ExpressionException(ExpressionError::Domain, "Factorial pole");
                if (compareSigned(exact->numerator, integer(250)) > 0)
                    throw ResourceLimitError("Factorial magnitude limit");
                unsigned n = epx::is_zero(exact->numerator) ? 0 : exact->numerator.digits.front();
                Integer result = integer(1);
                for (unsigned k = 2; k <= n; ++k)
                    result = epx::mul(result, integer(static_cast<int>(k)));
                return Exact(std::move(result));
            }
            // Spouge's convergent approximation. Coefficients are materialized at
            // guard precision to prevent exponentially large expression closures.
            const int sign = Sign(Binary("+", value, Exact(1)));
            if (sign > 0 && Sign(value) < 0)
            {
                auto raised = Binary("+", value, Exact(1));
                return Binary("/", Factorial(raised), raised);
            }
            if (sign <= 0)
            {
                auto next = Binary("+", value, Exact(1));
                if (Sign(next) == 0)
                    throw ExpressionException(ExpressionError::Domain, "Factorial pole");
                auto shift = Floor(Neg(next));
                auto shiftValue = exactMultiple(shift.facts);
                if (!shiftValue || compareSigned(shiftValue->numerator, integer(100)) > 0)
                    throw ResourceLimitError("Factorial recurrence limit");
                unsigned count = (epx::is_zero(shiftValue->numerator) ? 0 : shiftValue->numerator.digits.front()) + 1;
                auto product = Exact(1);
                auto raised = value;
                for (unsigned i = 0; i < count; ++i)
                {
                    raised = Binary("+", raised, Exact(1));
                    product = Binary("*", product, raised);
                }
                return Binary("/", Factorial(raised), product);
            }
            ensureExponentialArgument(value.value);
            Real result{ [x = value.value](int precision) -> epx::coro::lazy<Integer>
                         {
                             int target = std::max(0, precision) + 32;
                             if (target > 1024)
                                 throw ResourceLimitError("Gamma precision limit");
                             auto bound = x.approx(0).get();
                             if (compareSigned(bound, integer(128)) > 0)
                                 throw ResourceLimitError("Gamma magnitude limit");
                             int extra = epx::is_zero(bound) ? 8 : 8 + 4 * static_cast<int>(bound.digits.front());
                             int a = target + extra + 32;
                             int working = 2 * a + 64;
                             FixedIntervals arithmetic(working);
                             auto argument = arithmetic.Enclose(x);
                             if (compareSigned(argument.lower, integer(0)) <= 0)
                                 throw ResourceLimitError("Gamma positive argument classification limit");
                             auto sum = arithmetic.Enclose(epx::root(epx::mul(rational(integer(2)), Pi().value), 2));
                             Integer factorial = integer(1);
                             auto exponential = arithmetic.Enclose(epx::exp(rational(integer(a - 1))));
                             auto inverseE = arithmetic.Enclose(epx::exp(rational(integer(-1))));
                             for (int k = 1; k < a; ++k)
                             {
                                 auto t = rational(integer(a - k));
                                 auto weight = arithmetic.Ratio(epx::pow(integer(a - k), k - 1), factorial);
                                 auto coefficient = arithmetic.Mul(arithmetic.Mul(weight, arithmetic.Enclose(epx::root(t, 2))), exponential);
                                 if (k % 2 == 0)
                                     coefficient = arithmetic.Neg(coefficient);
                                 auto term = arithmetic.DivPositive(coefficient, arithmetic.Add(argument, arithmetic.Int(k)));
                                 sum = arithmetic.Add(sum, term);
                                 factorial = epx::mul(factorial, integer(k));
                                 exponential = arithmetic.Mul(exponential, inverseE);
                             }
                             auto xa = arithmetic.Add(argument, arithmetic.Int(a));
                             auto power = arithmetic.Add(argument, arithmetic.Enclose(rational(integer(1), integer(2))));
                             auto factor = arithmetic.Exp(arithmetic.Add(arithmetic.Mul(power, arithmetic.Log(xa)), arithmetic.Neg(xa)));
                             co_return arithmetic.GammaApproximation(arithmetic.Mul(factor, sum), a, precision);
                         } };
            return { std::move(result), { ZeroProof::ProvenNonZero, false, "fact(" + value.facts.identity + ")", true } };
        }
        Computed Function(const std::string& name, Computed value, const std::string& unit)
        {
            if (name == "recip")
                return Binary("/", Exact(1), value);
            if (name == "sqr")
                return value.facts.squareResult ? Computed{ epx::mul(value.value, value.value), valueFacts(*value.facts.squareResult) }
                                                : Binary("*", value, value);
            if (name == "cube")
                return Power(value, Exact(3));
            if (name == "sqrt" || name == "cbrt")
            {
                int sign = Sign(value);
                if (sign < 0 && name == "sqrt")
                    throw ExpressionException(ExpressionError::Domain, "Negative square root");
                if (sign == 0)
                    return Exact(0);
                if (auto exact = exactMultiple(value.facts); exact && !exact->timesPi)
                {
                    int degree = name == "sqrt" ? 2 : 3;
                    auto magnitude = exact->numerator;
                    magnitude.sgn = epx::sign::positive;
                    auto p = epx::root(magnitude, degree), q = epx::root(exact->denominator, degree);
                    auto pPower = epx::mul(p, p), qPower = epx::mul(q, q);
                    if (degree == 3)
                    {
                        pPower = epx::mul(pPower, p);
                        qPower = epx::mul(qPower, q);
                    }
                    if (compareSigned(pPower, magnitude) == 0 && compareSigned(qPower, exact->denominator) == 0)
                    {
                        if (sign < 0)
                            epx::negate(p);
                        return Exact(p, q);
                    }
                }
                if (name == "sqrt")
                {
                    if (auto identity = exactSquareRootIdentity(value.facts.identity))
                        return { parseNumber(*identity), literalFacts(*identity) };
                }
                auto result = sign < 0 ? negate(epx::root(negate(value.value), 3)) : epx::root(value.value, name == "sqrt" ? 2 : 3);
                ValueFacts facts{ ZeroProof::ProvenNonZero, false, name + "(" + value.facts.identity + ")", sign > 0 };
                if (name == "sqrt")
                    facts.squareResult = exactValueProof(value.facts);
                return { std::move(result), std::move(facts) };
            }
            if (name == "ln")
                return Log(value);
            if (name == "log" || name == "log10")
            {
                if (auto exponent = powerOfTenExponent(value.facts.identity))
                    return Exact(*exponent);
                return Binary("/", Log(value), Log(Exact(10)));
            }
            if (name == "exp")
                return Exp(value);
            if (name == "abs")
                return Sign(value) < 0 ? Neg(value) : value;
            if (name == "floor")
                return Floor(value);
            if (name == "ceil")
                return Neg(Floor(Neg(value)));
            if (name == "trunc")
                return Truncate(value);
            if (name == "frac")
                return Binary("-", value, Truncate(value));
            if (name == "fact")
                return Factorial(value);
            if (name == "dms" || name == "degrees")
            {
                auto degree = Truncate(value);
                auto minutes = Binary("*", Binary("-", value, degree), Exact(name == "dms" ? 60 : 100));
                auto integralMinutes = Truncate(minutes);
                auto seconds = Binary("*", Binary("-", minutes, integralMinutes), Exact(name == "dms" ? 60 : 100));
                auto divisor = Exact(name == "dms" ? 100 : 60);
                return Binary("+", degree, Binary("/", Binary("+", integralMinutes, Binary("/", seconds, divisor)), divisor));
            }
            if (name == "sec" || name == "csc" || name == "cot" || name == "sech" || name == "csch" || name == "coth")
            {
                auto base = name == "sec" ? "cos" : name == "csc" ? "sin" : name == "cot" ? "tan" : name == "sech" ? "cosh" : name == "csch" ? "sinh" : "tanh";
                return Binary("/", Exact(1), Function(base, value, unit));
            }
            if (name == "asec" || name == "acsc" || name == "acot" || name == "asech" || name == "acsch" || name == "acoth")
            {
                auto base = name == "asec"    ? "acos"
                            : name == "acsc"  ? "asin"
                            : name == "acot"  ? "atan"
                            : name == "asech" ? "acosh"
                            : name == "acsch" ? "asinh"
                                              : "atanh";
                return Function(base, Binary("/", Exact(1), value), unit);
            }
            if (name == "sin" || name == "cos" || name == "tan")
            {
                if (auto turns = QuarterTurns(value.facts, unit))
                {
                    if (name == "sin")
                        return Exact(*turns % 2 == 0 ? 0 : *turns == 1 ? 1 : -1);
                    if (name == "cos")
                        return Exact(*turns % 2 != 0 ? 0 : *turns == 0 ? 1 : -1);
                    if (*turns % 2)
                        throw ExpressionException(ExpressionError::DivideByZero, "Tangent pole");
                    return Exact(0);
                }
                auto radians = Radians(value, unit);
                Computed sine{ epx::sin(radians.value), { ZeroProof::Unknown, false, "sin(" + radians.facts.identity + ")", false } };
                Computed cosine{ epx::cos(radians.value), { ZeroProof::Unknown, false, "cos(" + radians.facts.identity + ")", false } };
                return name == "sin" ? sine : name == "cos" ? cosine : Binary("/", sine, cosine);
            }
            if (name == "asin" || name == "acos")
            {
                if (Sign(Binary("-", value, Exact(1))) > 0 || Sign(Binary("+", value, Exact(1))) < 0)
                    throw ExpressionException(ExpressionError::Domain, "Inverse trigonometric domain");
                if (value.facts.zero == ZeroProof::ProvenZero)
                    return name == "asin" ? Exact(0) : FromRadians(Binary("/", Pi(), Exact(2)), unit);
                auto upper = Binary("-", value, Exact(1));
                auto lower = Binary("+", value, Exact(1));
                if (upper.facts.zero == ZeroProof::ProvenZero)
                    return name == "acos" ? Exact(0) : FromRadians(Binary("/", Pi(), Exact(2)), unit);
                if (lower.facts.zero == ZeroProof::ProvenZero)
                    return FromRadians(name == "acos" ? Pi() : Neg(Binary("/", Pi(), Exact(2))), unit);
                return FromRadians(
                    { name == "asin" ? epx::arcsin(value.value) : epx::arccos(value.value),
                      { ZeroProof::Unknown, false, name + "(" + value.facts.identity + ")", false } },
                    unit);
            }
            if (name == "atan")
            {
                if (value.facts.zero == ZeroProof::ProvenZero)
                    return Exact(0);
                return FromRadians({ epx::arctan(value.value), { value.facts.zero, false, "atan(" + value.facts.identity + ")", value.facts.positive } }, unit);
            }
            if (name == "sinh" || name == "cosh" || name == "tanh")
            {
                auto positive = Exp(value), negative = Exp(Neg(value));
                auto sine = Binary("/", Binary("-", positive, negative), Exact(2));
                auto cosine = Binary("/", Binary("+", positive, negative), Exact(2));
                return name == "sinh" ? sine : name == "cosh" ? cosine : Binary("/", sine, cosine);
            }
            if (name == "asinh")
            {
                if (Sign(value) < 0)
                    return Neg(Function("asinh", Neg(value), unit));
                return Log(Binary("+", value, Function("sqrt", Binary("+", Binary("*", value, value), Exact(1)), unit)));
            }
            if (name == "acosh")
            {
                if (Sign(Binary("-", value, Exact(1))) < 0)
                    throw ExpressionException(ExpressionError::Domain, "acosh requires x >= 1");
                return Log(Binary("+", value, Function("sqrt", Binary("-", Binary("*", value, value), Exact(1)), unit)));
            }
            if (name == "atanh")
            {
                if (Sign(Binary("-", value, Exact(1))) >= 0 || Sign(Binary("+", value, Exact(1))) <= 0)
                    throw ExpressionException(ExpressionError::Domain, "atanh requires -1 < x < 1");
                return Binary("/", Log(Binary("/", Binary("+", Exact(1), value), Binary("-", Exact(1), value))), Exact(2));
            }
            throw ExpressionException(ExpressionError::Syntax, "Unknown function: " + name);
        }
        Computed EvaluateNode(const Expression::Ast& ast, size_t index, std::vector<std::optional<Computed>>& values)
        {
            const auto& node = ast.nodes[index];
            auto child = [&](size_t i)
            {
                auto& value = values.at(node.children.at(i));
                if (!value)
                    throw ExpressionException(ExpressionError::Syntax, "Expected numeric argument", node.span);
                auto result = std::move(*value);
                value.reset();
                return result;
            };
            Computed result = Exact(0);
            using Expression::NodeKind;
            try
            {
                if (node.kind == NodeKind::Number)
                {
                    if (node.name.size() > MaxInputCharacters)
                        throw ResourceLimitError("Numeric literal length limit");
                    result = { parseNumber(node.name), literalFacts(node.name) };
                }
                else if (node.kind == NodeKind::Constant)
                {
                    if (node.name == "pi")
                        result = Pi();
                    else if (node.name == "e")
                        result = Exp(Exact(1));
                    else
                        throw ParseError("Unknown constant");
                }
                else if (node.kind == NodeKind::Unit)
                    throw ParseError("Unit is only valid as a trigonometric argument");
                else if (node.kind == NodeKind::Prefix)
                    result = node.name == "-" ? Neg(child(0)) : child(0);
                else if (node.kind == NodeKind::Binary)
                {
                    auto left = child(0), right = child(1);
                    if (node.name == "^")
                        result = Power(left, right);
                    else if (node.name == "root")
                        result = Power(left, Binary("/", Exact(1), right));
                    else if (node.name == "logbase")
                        result = Binary("/", Log(left), Log(right));
                    else if (node.name == "mod")
                        result = Binary("-", left, Binary("*", right, Floor(Binary("/", left, right))));
                    else
                        result = Binary(node.name, left, right);
                }
                else
                {
                    if (node.children.empty() || node.children.size() > 2)
                        throw ParseError("Invalid function arity");
                    if (node.name == "pow" || node.name == "root" || node.name == "logbase")
                    {
                        if (node.children.size() != 2)
                            throw ParseError("Expected two numeric arguments");
                        auto left = child(0), right = child(1);
                        result = node.name == "pow"    ? Power(left, right)
                                 : node.name == "root" ? Power(left, Binary("/", Exact(1), right))
                                                       : Binary("/", Log(left), Log(right));
                    }
                    else
                    {
                        std::string unit = "rad";
                        if (node.children.size() == 2)
                        {
                            const auto& argument = ast.nodes[node.children[1]];
                            const std::string names = "|sin|cos|tan|sec|csc|cot|asin|acos|atan|asec|acsc|acot|";
                            if (argument.kind != NodeKind::Unit || names.find("|" + node.name + "|") == std::string::npos)
                                throw ParseError("Unexpected angle argument");
                            unit = argument.name;
                        }
                        result = Function(node.name, child(0), unit);
                    }
                }
                if (result.facts.identity.empty())
                {
                    result.facts.identity = ast.identities[index];
                }
                ensureMagnitude(result.value);
                return result;
            }
            catch (ExpressionException& error)
            {
                if (error.span.end == 0)
                    error.span = node.span;
                throw;
            }
        }
    }

    struct EpsilonValue::Impl
    {
        Computed number;
    };
    EpsilonValue::EpsilonValue(std::unique_ptr<Impl> impl)
        : m_impl(std::move(impl))
    {
    }
    EpsilonValue::EpsilonValue(const EpsilonValue& other)
        : m_impl(other.m_impl ? std::make_unique<Impl>(*other.m_impl) : nullptr)
    {
    }
    EpsilonValue& EpsilonValue::operator=(const EpsilonValue& other)
    {
        if (this != &other)
            m_impl = other.m_impl ? std::make_unique<Impl>(*other.m_impl) : nullptr;
        return *this;
    }
    EpsilonValue::EpsilonValue(EpsilonValue&&) noexcept = default;
    EpsilonValue& EpsilonValue::operator=(EpsilonValue&&) noexcept = default;
    EpsilonValue::~EpsilonValue() = default;
    std::string EpsilonValue::Format(int32_t significantDigits, bool scientific) const
    {
        if (!m_impl)
            throw std::logic_error("Moved-from Epsilon value");
        try
        {
            return Numerics::Format(m_impl->number.value, m_impl->number.facts, std::clamp(significantDigits, 1, 100), scientific);
        }
        catch (const epx::divide_by_zero_error&)
        {
            throw ExpressionException(ExpressionError::DivideByZero, "Division by zero during materialization");
        }
        catch (const epx::negative_radicand_error&)
        {
            throw ExpressionException(ExpressionError::Domain, "Negative radicand during materialization");
        }
        catch (const epx::non_positive_log_error&)
        {
            throw ExpressionException(ExpressionError::Domain, "Nonpositive logarithm during materialization");
        }
        catch (const epx::precision_overflow_error&)
        {
            throw ExpressionException(ExpressionError::ResourceLimit, "Materialization precision limit");
        }
        catch (const epx::msd_overflow_error&)
        {
            throw ExpressionException(ExpressionError::ResourceLimit, "Materialization magnitude limit");
        }
    }
    EvaluationResult Expression::Evaluator::Evaluate(std::string_view source, EvaluationLimits limits)
    {
        try
        {
            if (limits.sourceCharacters > 65536 || limits.tokens > 8192 || limits.nodes > 8192 || limits.depth > 32 || limits.operations > 4096)
                throw ExpressionException(ExpressionError::ResourceLimit, "Evaluation limits exceed hard ceilings");
            if (source.size() > limits.sourceCharacters)
                throw ExpressionException(ExpressionError::ResourceLimit, "Expression length limit");
            std::string owned(source);
            auto ast = Expression::Parse(owned, limits);
            std::vector<size_t> costs;
            std::vector<unsigned> depths;
            costs.reserve(ast.nodes.size());
            for (const auto& node : ast.nodes)
            {
                size_t cost = 1;
                unsigned depth = 1;
                for (auto child : node.children)
                {
                    depth = std::max(depth, depths[child] + 1);
                    if (costs[child] > limits.operations - std::min(cost, static_cast<size_t>(limits.operations)))
                        throw ExpressionException(ExpressionError::ResourceLimit, "Expression graph limit", node.span);
                    cost += costs[child];
                }
                const size_t multiplier = node.name == "sqr" ? 2 : node.name == "cube" ? 3 : node.name == "tan" || node.name == "tanh" ? 4 : 1;
                if (cost > limits.operations / multiplier)
                    throw ExpressionException(ExpressionError::ResourceLimit, "Expression graph limit", node.span);
                if (depth > limits.depth)
                    throw ExpressionException(ExpressionError::ResourceLimit, "Evaluation depth limit", node.span);
                costs.push_back(cost * multiplier);
                depths.push_back(depth);
            }
            std::vector<std::optional<Computed>> values(ast.nodes.size());
            for (size_t i = 0; i < ast.nodes.size(); ++i)
                if (ast.nodes[i].kind != Expression::NodeKind::Unit || i == ast.root)
                    values[i] = EvaluateNode(ast, i, values);
            auto result = std::move(*values[ast.root]);
            return EpsilonValue(std::make_unique<EpsilonValue::Impl>(EpsilonValue::Impl{ std::move(result) }));
        }
        catch (const ExpressionException& error)
        {
            return error;
        }
        catch (const epx::divide_by_zero_error&)
        {
            return ExpressionException(ExpressionError::DivideByZero, "Division by zero");
        }
        catch (const epx::negative_radicand_error&)
        {
            return ExpressionException(ExpressionError::Domain, "Negative radicand");
        }
        catch (const epx::non_positive_log_error&)
        {
            return ExpressionException(ExpressionError::Domain, "Nonpositive logarithm");
        }
        catch (const epx::precision_overflow_error&)
        {
            return ExpressionException(ExpressionError::ResourceLimit, "Precision limit");
        }
        catch (const epx::msd_overflow_error&)
        {
            return ExpressionException(ExpressionError::ResourceLimit, "Magnitude classification limit");
        }
        catch (const std::bad_alloc&)
        {
            return ExpressionException(ExpressionError::ResourceLimit, "Allocation limit");
        }
        catch (const std::length_error&)
        {
            return ExpressionException(ExpressionError::ResourceLimit, "Allocation length limit");
        }
    }
}
