// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Syntax.h"

namespace CalculationManager::Expression
{
    Token Lexer::Next()
    {
        for (;;)
        {
            const char* start = m_cursor;
            auto token = [&](Kind kind) { return Token{kind, {static_cast<size_t>(start - m_source), static_cast<size_t>(m_cursor - m_source)}}; };
/*!re2c
            re2c:define:YYCTYPE = char;
            re2c:define:YYCURSOR = m_cursor;
            re2c:define:YYLIMIT = m_limit;
            re2c:define:YYMARKER = m_marker;
            re2c:yyfill:enable = 0;
            re2c:eof = 0;
            digit = [0-9];
            number = (digit+ ("." digit*)? | "." digit+) ([eE] [+-]? digit+)?;
            identifier = [a-zA-Z_] [a-zA-Z_0-9]*;
            [ \t\r\n]+ { continue; }
            $ { return token(Kind::End); }
            number { return token(Kind::Number); }
            "mod" { return token(Kind::Modulo); }
            identifier { return token(Kind::Identifier); }
            "+" { return token(Kind::Plus); }
            "-" { return token(Kind::Minus); }
            "*" { return token(Kind::Multiply); }
            "/" { return token(Kind::Divide); }
            "^" { return token(Kind::Power); }
            "!" { return token(Kind::Factorial); }
            "(" { return token(Kind::Open); }
            ")" { return token(Kind::Close); }
            "," { return token(Kind::Comma); }
            * { throw ExpressionException(ExpressionError::Syntax, "Invalid expression character", token(Kind::End).span); }
*/
        }
    }
}
