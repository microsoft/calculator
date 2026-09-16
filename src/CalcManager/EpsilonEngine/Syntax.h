// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "EpsilonEngine.h"

#include <string>
#include <vector>

namespace CalculationManager::Expression
{
    enum class Kind
    {
        End,
        Number,
        Identifier,
        Plus,
        Minus,
        Multiply,
        Divide,
        Modulo,
        Power,
        Factorial,
        Open,
        Close,
        Comma
    };
    struct Token
    {
        Kind kind;
        ExpressionSpan span;
    };

    class Lexer
    {
    public:
        explicit Lexer(const std::string& source)
            : m_source(source.data())
            , m_cursor(m_source)
            , m_limit(m_source + source.size())
        {
        }
        Lexer(std::string&&) = delete;
        Token Next();

    private:
        const char* m_source;
        const char* m_cursor;
        const char* m_limit;
        const char* m_marker = nullptr;
    };

    enum class NodeKind
    {
        Number,
        Constant,
        Unit,
        Prefix,
        Binary,
        Call
    };
    struct Node
    {
        NodeKind kind;
        std::string name;
        ExpressionSpan span;
        std::vector<size_t> children;
    };
    struct Ast
    {
        std::vector<Node> nodes;
        std::vector<std::string> identities;
        size_t root = 0;
    };

    Ast Parse(const std::string& source, const EvaluationLimits& limits);
}
