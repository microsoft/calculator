// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <string>

#ifndef GRAPHINGAPI
#ifdef GRAPHING_ENGINE_IMPL
#define GRAPHINGAPI __declspec(dllexport)
#else
#define GRAPHINGAPI __declspec(dllimport)
#endif
#endif

namespace Graphing
{
    struct NonCopyable
    {
        NonCopyable() = default;
        virtual ~NonCopyable() = default;

        NonCopyable(NonCopyable const&) = delete;
        NonCopyable& operator=(NonCopyable const&) = delete;
    };

    struct NonMoveable
    {
        NonMoveable() = default;
        virtual ~NonMoveable() = default;

        NonMoveable(NonMoveable&&) = delete;
        NonMoveable& operator=(NonMoveable&&) = delete;
    };

    struct IExpression
    {
        virtual ~IExpression() = default;

        virtual unsigned int GetExpressionID() const = 0;
        virtual bool IsEmptySet() const = 0;
    };

    struct IVariable
    {
        virtual ~IVariable() = default;

        virtual int GetVariableID() const = 0;
        virtual const std::wstring& GetVariableName() = 0;
    };

    struct IExpressible
    {
        virtual ~IExpressible() = default;

        virtual std::shared_ptr<IExpression> GetExpression() const = 0;
    };

    class Color
    {
    private:
        // Each color channel is given an 8 bit space in a 32 bit uint32_t.
        // The format is (RRGGBBAA).  As an example,
        // Red is FF0000FF (4,278,190,335)
        static constexpr uint8_t redChannelShift = 24;
        static constexpr uint8_t greenChannelShift = 16;
        static constexpr uint8_t blueChannelShift = 8;
        static constexpr uint8_t alphaChannelShift = 0;

    public:
        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;

        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
            : R{ r }, G{ g }, B{ b }, A{ a }
        {}

        constexpr Color(uint8_t r, uint8_t g, uint8_t b) noexcept
            : Color{ r, g, b, 0xFF }
        {}

        constexpr Color() noexcept
            : Color{ 0, 0, 0 }
        {}


        constexpr explicit Color(uint32_t value) noexcept
            : Color{
                static_cast<uint8_t>(value >> redChannelShift),
                static_cast<uint8_t>(value >> greenChannelShift),
                static_cast<uint8_t>(value >> blueChannelShift),
                static_cast<uint8_t>(value >> alphaChannelShift)
        }
        {
        }

        constexpr explicit operator uint32_t() const
        {
            return (A << alphaChannelShift)
                | (R << redChannelShift)
                | (G << greenChannelShift)
                | (B << blueChannelShift);
        }
    };
}
