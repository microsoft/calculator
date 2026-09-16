// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Syntax.h"
#include <algorithm>

namespace CalculationManager::Expression
{
    namespace
    {
        class Parser
        {
        public:
            Parser(const std::string& source, const EvaluationLimits& limits)
                : m_source(source)
                , m_limits(limits)
                , m_lexer(source)
            {
                Advance();
            }
            Ast Run()
            {
                m_ast.root = Expression(0, 0);
                if (m_current.kind != Kind::End)
                    Fail("Unexpected token after expression");
                return std::move(m_ast);
            }

        private:
            [[noreturn]] void Fail(const char* message) const
            {
                throw ExpressionException(ExpressionError::Syntax, message, m_current.span);
            }
            void Advance()
            {
                m_current = m_lexer.Next();
                if (m_current.kind != Kind::End && ++m_tokens > m_limits.tokens)
                    throw ExpressionException(ExpressionError::ResourceLimit, "Token limit", m_current.span);
            }
            size_t Add(Node node)
            {
                if (m_ast.nodes.size() >= m_limits.nodes)
                    throw ExpressionException(ExpressionError::ResourceLimit, "AST node limit", node.span);
                unsigned depth = 1;
                for (auto child : node.children)
                    depth = std::max(depth, m_depths[child] + 1);
                if (depth > m_limits.depth)
                    throw ExpressionException(ExpressionError::ResourceLimit, "AST depth limit", node.span);
                m_depths.push_back(depth);
                std::string identity = node.name;
                identity += "(";
                for (auto child : node.children)
                {
                    const auto& part = m_ast.identities[child];
                    if (part.size() + identity.size() + 2 > 65536)
                        throw ExpressionException(ExpressionError::ResourceLimit, "Structural identity limit", node.span);
                    identity += part + ",";
                }
                identity += ")";
                m_ast.identities.push_back(std::move(identity));
                m_ast.nodes.push_back(std::move(node));
                return m_ast.nodes.size() - 1;
            }
            std::string Text(const Token& token) const
            {
                return m_source.substr(token.span.begin, token.span.end - token.span.begin);
            }
            static int BindingPower(Kind kind)
            {
                switch (kind)
                {
                case Kind::Plus:
                case Kind::Minus:
                    return 10;
                case Kind::Multiply:
                case Kind::Divide:
                case Kind::Modulo:
                    return 20;
                case Kind::Power:
                    return 40;
                case Kind::Factorial:
                    return 50;
                default:
                    return -1;
                }
            }
            size_t Expression(int minimum, unsigned depth)
            {
                if (depth >= m_limits.depth)
                    throw ExpressionException(ExpressionError::ResourceLimit, "Parser depth limit", m_current.span);
                Token first = m_current;
                Advance();
                size_t left;
                if (first.kind == Kind::Number)
                    left = Add({ NodeKind::Number, Text(first), first.span, {} });
                else if (first.kind == Kind::Plus || first.kind == Kind::Minus)
                {
                    auto child = Expression(30, depth + 1);
                    left = Add({ NodeKind::Prefix, Text(first), { first.span.begin, m_ast.nodes[child].span.end }, { child } });
                }
                else if (first.kind == Kind::Open)
                {
                    left = Expression(0, depth + 1);
                    if (m_current.kind != Kind::Close)
                        Fail("Expected closing parenthesis");
                    m_ast.nodes[left].span = { first.span.begin, m_current.span.end };
                    Advance();
                }
                else if (first.kind == Kind::Identifier)
                {
                    std::string name = Text(first);
                    if (m_current.kind == Kind::Open)
                    {
                        Advance();
                        std::vector<size_t> arguments;
                        if (m_current.kind != Kind::Close)
                        {
                            for (;;)
                            {
                                if (arguments.size() == 3)
                                    Fail("Too many function arguments");
                                arguments.push_back(Expression(0, depth + 1));
                                if (m_current.kind != Kind::Comma)
                                    break;
                                Advance();
                            }
                        }
                        if (m_current.kind != Kind::Close)
                            Fail("Expected closing function parenthesis");
                        auto end = m_current.span.end;
                        Advance();
                        left = Add({ NodeKind::Call, std::move(name), { first.span.begin, end }, std::move(arguments) });
                    }
                    else
                    {
                        auto kind = name == "deg" || name == "rad" || name == "grad" ? NodeKind::Unit : NodeKind::Constant;
                        left = Add({ kind, std::move(name), first.span, {} });
                    }
                }
                else
                    throw ExpressionException(ExpressionError::Syntax, "Expected expression operand", first.span);

                for (;;)
                {
                    const bool namedBinary = m_current.kind == Kind::Identifier && (Text(m_current) == "root" || Text(m_current) == "logbase");
                    int binding = namedBinary ? 40 : BindingPower(m_current.kind);
                    if (binding < minimum)
                        break;
                    Token operation = m_current;
                    Advance();
                    if (operation.kind == Kind::Factorial)
                        left = Add({ NodeKind::Call, "fact", { m_ast.nodes[left].span.begin, operation.span.end }, { left } });
                    else
                    {
                        size_t right = Expression(binding + (operation.kind == Kind::Power ? 0 : 1), depth + 1);
                        left = Add({ NodeKind::Binary, Text(operation), { m_ast.nodes[left].span.begin, m_ast.nodes[right].span.end }, { left, right } });
                    }
                }
                return left;
            }
            const std::string& m_source;
            const EvaluationLimits& m_limits;
            Lexer m_lexer;
            Token m_current{};
            size_t m_tokens = 0;
            Ast m_ast;
            std::vector<unsigned> m_depths;
        };
    }
    Ast Parse(const std::string& source, const EvaluationLimits& limits)
    {
        return Parser(source, limits).Run();
    }
}
