// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

namespace CalculationManager
{
    namespace Expression
    {
        class Evaluator;
    }
    struct ExpressionSpan
    {
        size_t begin = 0;
        size_t end = 0;
    };

    enum class ExpressionError
    {
        Syntax,
        Domain,
        DivideByZero,
        Undefined,
        ResourceLimit
    };

    class ExpressionException : public std::runtime_error
    {
    public:
        ExpressionException(ExpressionError code, std::string message, ExpressionSpan span = {})
            : std::runtime_error(std::move(message))
            , code(code)
            , span(span)
        {
        }
        ExpressionError code;
        ExpressionSpan span;
    };

    struct EvaluationLimits
    {
        size_t sourceCharacters = 65536;
        size_t tokens = 8192;
        size_t nodes = 8192;
        unsigned depth = 32;
        unsigned operations = 4096;
    };

    class EpsilonValue
    {
    public:
        EpsilonValue(const EpsilonValue&);
        EpsilonValue& operator=(const EpsilonValue&);
        EpsilonValue(EpsilonValue&&) noexcept;
        EpsilonValue& operator=(EpsilonValue&&) noexcept;
        ~EpsilonValue();

        // Each copy owns an independent lazy graph, including approximation caches.
        std::string Format(int32_t significantDigits = 32, bool scientific = false) const;

    private:
        struct Impl;
        explicit EpsilonValue(std::unique_ptr<Impl> impl);
        std::unique_ptr<Impl> m_impl;
        friend class Expression::Evaluator;
    };

    using EvaluationResult = std::variant<EpsilonValue, ExpressionException>;

    class EpsilonEngine final
    {
    public:
        static EvaluationResult Evaluate(std::string_view source, EvaluationLimits limits = {});
    };
}
